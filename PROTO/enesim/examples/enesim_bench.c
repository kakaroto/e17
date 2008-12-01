#include "enesim_bench_common.h"

/**
 * TODO
 * make the gradient functions draw pattern boxes
 */

int opt_shift = 0;
int opt_width = 1020;
int opt_height = 1020;
int opt_times = 1;
FILE *opt_file;
int opt_debug = 0;
int opt_rop = ENESIM_FILL;
Enesim_Surface_Format opt_fmt = ENESIM_SURFACE_ARGB8888;
char *opt_bench = "drawer";

/* Place to store the result timmings for every operation */
typedef struct _Result
{
	double time;
} Result;

const char * rop_name(Enesim_Rop rop)
{
	switch (rop)
	{
		case ENESIM_BLEND:
			return "blend";
		case ENESIM_FILL:
			return "fill";
		default:
			return NULL;
	}
	return NULL;
}

int rop_get(const char *name, int *rop)
{
	int ret = 1;
	
	if (!strcmp(name, "blend"))
		*rop = ENESIM_BLEND;
	else if (!strcmp(name, "fill"))
		*rop = ENESIM_FILL;
	else
		ret = 0;
	return ret;
}

int bench_get(const char *name)
{
	if (!strcmp(name, "help"))
		return 0;
	else if (!strcmp(name, "renderer"))
		return 1;
	else if (!strcmp(name, "rasterizer"))
		return 1;
	else if (!strcmp(name, "drawer"))
		return 1;
	else if (!strcmp(name, "transformer"))
		return 1;
	else if (!strcmp(name, "help"))
		return 1;
	return 0;
}

int fmt_get(const char *name, Enesim_Surface_Format *fmt)
{
	int ret = 1;
	if (!strcmp(name, "argb8888"))
		*fmt = ENESIM_SURFACE_ARGB8888;
	else
		ret = 0;
	return ret;
}

double get_time(void)
{
	struct timeval timev;

	gettimeofday(&timev, NULL);
	return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}


void surface_save(Enesim_Surface *s, const char *name)
{
	Enesim_Surface *img;
	int w, h;
	
	enesim_surface_size_get(s, &w, &h);
	img = enesim_surface_new(ENESIM_SURFACE_ARGB8888_UNPRE, w, h);
	//printf("Saving image %s\n", name);
	enesim_surface_convert(s, img);
	image_save(img, name, 0);
	enesim_surface_delete(img);
}

void test_gradient(Enesim_Surface *s)
{
	Enesim_Drawer_Span dspan;
	Enesim_Surface_Data sdata;
	Enesim_Surface_Format sfmt;
	int i;
	Enesim_Color color;
	int skip = 0;

	enesim_surface_data_get(s, &sdata);
	sfmt = enesim_surface_format_get(s);
	/* create a simple gradient */
	dspan = enesim_drawer_span_color_get(ENESIM_FILL, sfmt, 0xffffffff);
	if (!dspan)
	{
		return;
	}
	for (i = 0; i < opt_height; i++)
	{
		int j;
		Enesim_Surface_Data sdata_tmp;
		unsigned char col = ((opt_height - 1) - i) >> opt_shift;

		sdata_tmp = sdata;
		enesim_color_get(&color, col, col, 0, col);
#if 1
		if (!((i + 1) % 16))
			skip = (skip + 1) % 2;
		if (skip)
			enesim_surface_data_increment(&sdata_tmp, sfmt, 16);
		for (j = 0; j < opt_width; j += 32)
		{
			dspan(&sdata_tmp, 16, NULL, color, NULL);
			enesim_surface_data_increment(&sdata_tmp, sfmt, 32);
		}
#else
		dspan(&sdata_tmp, opt_width, NULL, color, NULL);
#endif
		enesim_surface_data_increment(&sdata, sfmt, opt_width);
	}
	surface_save(s, "gradient.png");
}

void test_gradient2(Enesim_Surface *s)
{
	Enesim_Drawer_Span dspan;
	Enesim_Surface_Data sdata;
	Enesim_Surface_Format sfmt;
	int i;
	Enesim_Color color;

	enesim_surface_data_get(s, &sdata);
	sfmt = enesim_surface_format_get(s);
	/* create a simple gradient */
	dspan = enesim_drawer_span_color_get(ENESIM_FILL, sfmt, 0xffffffff);
	if (!dspan)
	{
		return;
	}
	for (i = 0; i < opt_height; i++)
	{
		unsigned char col = i >> opt_shift;

		enesim_color_get(&color, col, 0, 0, col);
		dspan(&sdata, opt_width, NULL, color, NULL);
		enesim_surface_data_increment(&sdata, sfmt, opt_width);
	}
	surface_save(s, "gradient2.png");
}

Enesim_Surface * test_pattern(int w)
{
	Enesim_Surface *s;
	Enesim_Drawer_Span spfnc;
	Enesim_Color color;
	Enesim_Surface_Data sdata;
	int i;
	int spaces = w / 2;
	
	enesim_color_get(&color, 0xff, 0xff, 0xff, 0);
	spfnc = enesim_drawer_span_color_get(ENESIM_FILL, opt_fmt, color);
	if (!spfnc)
		return NULL;
	s = enesim_surface_new(opt_fmt, w, w);
	
	/* draw the pattern */
	enesim_surface_data_get(s, &sdata);
	enesim_surface_data_increment(&sdata, opt_fmt, spaces);
	for (i = 0; i < w / 2; i++)
	{
		int len = i * 2 +1;
		
		spfnc(&sdata, len, NULL, color, NULL);
		enesim_surface_data_increment(&sdata, opt_fmt, len + spaces + --spaces);
	}
	spfnc(&sdata, w, NULL, color, NULL);
	enesim_surface_data_increment(&sdata, opt_fmt, w + 1);
	spaces = 1;
	for (i = 0; i < w / 2; i++)
	{
		int len = (w - 1) - (i * 2 +1);
		
		spfnc(&sdata, len, NULL, color, NULL);
		enesim_surface_data_increment(&sdata, opt_fmt, len + spaces + ++spaces);
	}
	surface_save(s, "pattern.png");
	return s;
}

void test_finish(const char *name, Enesim_Rop rop, Enesim_Surface *dst,
		Enesim_Surface *src, Enesim_Surface *mask)
{
	Enesim_Surface_Format sfmt;
	char file[256];
	char tmp[256];

	if (!opt_debug)
		return;
	
	sfmt = enesim_surface_format_get(dst);
	snprintf(tmp, 256, "%s_%s_%s", name, rop_name(rop), enesim_surface_format_name_get(sfmt));
	if (src)
	{
		char tmp2[256];

		sfmt = enesim_surface_format_get(src);
		snprintf(tmp2, 256, "_%s", enesim_surface_format_name_get(sfmt));
		strncat(tmp, tmp2, 256);
	}
	if (mask)
	{
		char tmp2[256];

		sfmt = enesim_surface_format_get(mask);
		snprintf(tmp2, 256, "_%s", enesim_surface_format_name_get(sfmt));
		strncat(tmp, tmp2, 256);
	}
	/* now save the source image in case it exists */
	if (src)
	{
		char tmp2[256];

		sfmt = enesim_surface_format_get(src);
		snprintf(tmp2, 256, "%s_source.png", tmp);
		surface_save(src, tmp2);
	}
	snprintf(file, 256, "%s.png", tmp);
	surface_save(dst, file);
}

void help(void)
{
	printf("Usage: enesim_bench [OPTION]...\n");
	printf("Runs a benchmark test for enesim operations\n");
	printf("Option can be one of the following:\n");
	printf("-h, --help            Print this screen\n");
	printf("-t, --times           Number of times to run the test [10]\n");
	printf("-d, --debug           Dump temporal images [off]\n");
	printf("-f, --file            File to place the statistics [benchmark.txt]\n");
	printf("-r, --rop             Raster operation to use [fill], use help to get a list\n");
	printf("-m, --fmt             Surface format to use [argb8888_pre], use help to get a list\n");
	printf("-w, --width           Surface width\n");
	printf("-e, --height          Surface height\n");
	printf("-b, --bench           Benchmark to run, use help to get a list\n");
}

void rop_help(void)
{
	int rop;
	
	for (rop = 0; rop < ENESIM_ROPS; rop++)
	{
		printf("%s\n", rop_name(rop));
	}
}

void bench_help(void)
{
	printf("drawer\n");
	printf("transformer\n");
	printf("rasterizer\n");
	printf("renderer\n");
	printf("all\n");
}

void fmt_help(void)
{
	int fmt;
	
	for (fmt = 0; fmt < ENESIM_SURFACE_FORMATS; fmt++)
	{
		printf("%s\n", enesim_surface_format_name_get(fmt));
	}
}


int main(int argc, char **argv)
{
	char *short_options = "dhf:t:r:m:w:e:b:";
	struct option long_options[] = {
		{"opt_debug", 0, 0, 'd'},
		{"help", 0, 0, 'h'},
		{"times", 1, 0, 't'},
		{"file", 1, 0, 'f'},
		{"rop", 1, 0, 'r'},
		{"fmt", 1, 0, 'm'},
		{"width", 1, 0, 'w'},
		{"height", 1, 0, 'e'},
		{"bench", 1, 0, 'b'},
		{0, 0, 0, 0}
	};
	int option;
	char c;
	char *file = "benchmark.txt";

	/* handle the parameters */
	while ((c = getopt_long(argc, argv, short_options, long_options,
	                                &option)) != -1)
	{
		/* arm bug ? */
		if (c == 255)
			goto ok;
		switch (c)
		{
			case 'h':
				help();
				return 0;
			case 't':
				opt_times = atoi(optarg);
				break;
			case 'f':
				file = optarg;
				opt_file = fopen(file, "w+");
				if (!opt_file)
				{
					help();
					return 0;
				}
				break;
			case 'd':
				opt_debug = 1;
				break;
			case 'r':
				if (!rop_get(optarg, &opt_rop))
				{
					rop_help();
					return 1;
				}
				break;
			case 'm':
				
				if (!fmt_get(optarg, &opt_fmt))
				{
					fmt_help();
					return 1;
				}
				break;
			case 'w':
				opt_width = atoi(optarg);
				break;
			case 'e':
				opt_height = atoi(optarg);
				break;
			case 'b':
				if (!bench_get(optarg))
				{
					bench_help();
					return 1;
				}
				break;
			default:
				break;
		}
	}
ok:
	printf("Enesim Bench\n");
	printf("* BENCH = %s\n", opt_bench);
	printf("* SIZE = %dx%d\n", opt_width, opt_height);
	printf("* ROP = %s\n", rop_name(opt_rop));
	printf("* FMT = %s\n", enesim_surface_format_name_get(opt_fmt));
	printf("* TIMES = %d\n", opt_times);
	enesim_init();
	if (!strcmp(opt_bench, "renderer"))
		renderer_bench();
	else if (!strcmp(opt_bench, "rasterizer"))
		rasterizer_bench();
	else if (!strcmp(opt_bench, "drawer"))
		drawer_bench();	
	else if (!strcmp(opt_bench, "transformer"))
		transformer_bench();
	else if (!strcmp(opt_bench, "all"))
	{
		drawer_bench();
		transformer_bench();
		rasterizer_bench();
		renderer_bench();
	}
	enesim_shutdown();
	/* this bench should be on test
	 * matrix_bench();
	 */

	return 0;
}
