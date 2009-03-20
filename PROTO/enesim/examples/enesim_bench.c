#include "enesim_bench_common.h"

/**
 * TODO
 * + Make the gradient functions draw pattern boxes
 * + Add a html output mode, for easy viewing of every format/op/time
 * + Split this into differnet benchs instead of only one?
 * + Add a mask and src formats
 */

int opt_width = 256;
int opt_height = 256;
int opt_times = 1;
FILE *opt_file;
int opt_debug = 0;
int opt_rop = ENESIM_FILL;
Enesim_Format *opt_fmt;
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
	{
		opt_bench = "renderer";
		return 1;
	}
	else if (!strcmp(name, "rasterizer"))
	{
		opt_bench = "rasterizer";
		return 1;
	}
	else if (!strcmp(name, "drawer"))
	{
		opt_bench = "drawer";
		return 1;
	}
	else if (!strcmp(name, "transformer"))
	{
		opt_bench = "transformer";
		return 1;
	}
	return 0;
}

char * opacity_get(Enesim_Surface_Pixel *p)
{
	uint8_t alpha;

	enesim_surface_pixel_components_to(p, &alpha, NULL, NULL, NULL);
	if (alpha == 0xff)
		return "opaque";
	else
		return "transparent";
}

int fmt_get(const char *name, Enesim_Format **fmt)
{
	Eina_Iterator *it;
	Enesim_Format *f;

	it = enesim_format_iterator_new();
	while (eina_iterator_next(it, (void **)&f))
	{
		if (!strcmp(name, enesim_format_name_get(f)))
		{
			*fmt = f;
			return 1;
		}
	}
	eina_iterator_free(it);
	return 0;
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
	img = enesim_surface_new(enesim_format_argb8888_unpre_get(), w, h);
	enesim_surface_convert(s, img, NULL);
	image_save(img, name, 0);
	enesim_surface_delete(img);
}

void surfaces_create(Enesim_Surface **src, Enesim_Format *sfmt,
		Enesim_Surface **dst, Enesim_Format *dfmt,
		Enesim_Surface **msk, Enesim_Format *mfmt)
{
	if (src)
	{
		if (*src) enesim_surface_delete(*src);
		*src = enesim_surface_new(sfmt, opt_width, opt_height);
		test_gradient(*src);
	}
	if (dst)
	{
		if (*dst) enesim_surface_delete(*dst);
		*dst = enesim_surface_new(dfmt, opt_width, opt_height);
		test_gradient2(*dst);
	}
	if (msk)
	{
		if (*msk) enesim_surface_delete(*msk);
		*msk = enesim_surface_new(mfmt, opt_width, opt_height);
		test_gradient3(*msk);
	}
}


void test_gradient(Enesim_Surface *s)
{
	Enesim_Drawer_Span dspan;
	Enesim_Surface_Data sdata;
	Enesim_Format *sfmt;
	Enesim_Surface_Pixel color;
	int i;
	int skip = 0;

	enesim_surface_data_get(s, &sdata);
	sfmt = enesim_surface_format_get(s);
	/* create a simple gradient */
	enesim_surface_pixel_components_from(&color, sfmt, 0xff, 0xff, 0xff, 0xff);
	dspan = enesim_drawer_span_color_get(ENESIM_FILL, sfmt, &color);
	if (!dspan)
	{
		return;
	}
	for (i = 0; i < opt_height; i++)
	{
		int j;
		Enesim_Surface_Data sdata_tmp;
		uint8_t col = ((opt_height - 1) - i) * 255 / opt_height;

		sdata_tmp = sdata;
		enesim_surface_pixel_components_from(&color, sfmt, col, col/2, 0, col);
#if 1
		if (!((i + 1) % 16))
			skip = (skip + 1) % 2;
		if (skip)
			enesim_surface_data_increment(&sdata_tmp, 16);
		for (j = 0; j < opt_width; j += 32)
		{
			dspan(&sdata_tmp, 16, NULL, &color, NULL);
			enesim_surface_data_increment(&sdata_tmp, 32);
		}
#else
		dspan(&sdata_tmp, opt_width, NULL, color, NULL);
#endif
		enesim_surface_data_increment(&sdata, opt_width);
	}
}

void test_gradient2(Enesim_Surface *s)
{
	Enesim_Drawer_Span dspan;
	Enesim_Surface_Data sdata;
	Enesim_Format *sfmt;
	Enesim_Surface_Pixel color;
	int i;

	enesim_surface_data_get(s, &sdata);
	sfmt = enesim_surface_format_get(s);
	/* create a simple gradient */
	enesim_surface_pixel_components_from(&color, sfmt, 0xaa, 0xff, 0xff, 0xff);
	dspan = enesim_drawer_span_color_get(ENESIM_FILL, sfmt, &color);
	if (!dspan)
	{
		return;
	}
	for (i = 0; i < opt_height; i++)
	{
		uint8_t col = (i * 255)/opt_height;

		enesim_surface_pixel_components_from(&color, sfmt, col, col, col, 0);
		dspan(&sdata, opt_width, NULL, &color, NULL);
		enesim_surface_data_increment(&sdata, opt_width);
	}
}

void test_gradient3(Enesim_Surface *s)
{
	Enesim_Drawer_Span dspan;
	Enesim_Surface_Data sdata;
	Enesim_Format *sfmt;
	Enesim_Surface_Pixel color;
	int i;

	enesim_surface_data_get(s, &sdata);
	sfmt = enesim_surface_format_get(s);
	/* create a simple gradient */
	enesim_surface_pixel_components_from(&color, sfmt, 0xaa, 0xff, 0xff, 0xff);
	dspan = enesim_drawer_span_color_get(ENESIM_FILL, sfmt, &color);
	if (!dspan)
	{
		return;
	}
	for (i = 0; i < opt_height/2; i++)
	{
		uint8_t col = (i * 255)/(opt_height/2);

		enesim_surface_pixel_components_from(&color, sfmt, col, 0, col / 2, col);
		dspan(&sdata, opt_width, NULL, &color, NULL);
		enesim_surface_data_increment(&sdata, opt_width);
	}
	for (i = 0; i < opt_height/2; i++)
	{
		uint8_t col = 255 - (i * 255)/(opt_height/2);

		enesim_surface_pixel_components_from(&color, sfmt, col, 0, col / 2, col);
		dspan(&sdata, opt_width, NULL, &color, NULL);
		enesim_surface_data_increment(&sdata, opt_width);
	}
}

Enesim_Surface * test_pattern(int w)
{
	Enesim_Surface *s;
	Enesim_Drawer_Span spfnc;
	Enesim_Surface_Pixel color;
	Enesim_Surface_Data sdata;
	int i;
	int spaces = w / 2;

	enesim_surface_pixel_components_from(&color, opt_fmt, 0xaa, 0xaa, 0xff, 0);
	spfnc = enesim_drawer_span_color_get(ENESIM_FILL, opt_fmt, &color);
	if (!spfnc)
		return NULL;
	s = enesim_surface_new(opt_fmt, w, w);

	/* draw the pattern */
	enesim_surface_data_get(s, &sdata);
	enesim_surface_data_increment(&sdata, spaces);
	for (i = 0; i < w / 2; i++)
	{
		int len = i * 2 +1;
		int nspaces = spaces - 1;

		spfnc(&sdata, len, NULL, &color, NULL);
		enesim_surface_data_increment(&sdata, len + spaces + nspaces);
		spaces--;
	}
	spfnc(&sdata, w, NULL, &color, NULL);
	enesim_surface_data_increment(&sdata,  w + 1);
	spaces = 1;
	for (i = 0; i < w / 2; i++)
	{
		int len = (w - 1) - (i * 2 +1);
		int nspaces = spaces + 1;

		spfnc(&sdata, len, NULL, &color, NULL);
		enesim_surface_data_increment(&sdata, len + spaces + nspaces);
		spaces++;
	}
	surface_save(s, "pattern.png");
	return s;
}

Enesim_Surface * test_pattern2(int w)
{
	Enesim_Surface *s;
	Enesim_Drawer_Span spfnc;
	Enesim_Surface_Pixel color;
	Enesim_Surface_Data sdata;
	int i;
	int spaces = w / 2;

	enesim_surface_pixel_components_from(&color, opt_fmt, 0xaa, 0xaa, 0xff, 0);
	spfnc = enesim_drawer_span_color_get(ENESIM_FILL, opt_fmt, &color);
	if (!spfnc)
		return NULL;
	s = enesim_surface_new(opt_fmt, w, w);

	/* draw the pattern */
	enesim_surface_data_get(s, &sdata);
	enesim_surface_data_increment(&sdata, spaces);
	for (i = 0; i < w / 2; i++)
	{
		int len = i * 2 +1;
		int nspaces = spaces - 1;

		enesim_surface_pixel_components_from(&color, opt_fmt, i * (255 / w), 0xff, 0, 0xff);
		spfnc(&sdata, len, NULL, &color, NULL);
		enesim_surface_data_increment(&sdata, len + spaces + nspaces);
		spaces--;
	}
	spfnc(&sdata, w, NULL, &color, NULL);
	enesim_surface_data_increment(&sdata,  w + 1);
	spaces = 1;
	for (i = 0; i < w / 2; i++)
	{
		int len = (w - 1) - (i * 2 +1);
		int nspaces = spaces + 1;

		enesim_surface_pixel_components_from(&color, opt_fmt, (w / 2 - i) * (255 / w), 0xff, 0, 0xff);
		spfnc(&sdata, len, NULL, &color, NULL);
		enesim_surface_data_increment(&sdata, len + spaces + nspaces);
		spaces++;
	}
	surface_save(s, "pattern2.png");
	return s;
}


void test_finish(const char *name, Enesim_Rop rop, Enesim_Surface *dst,
		Enesim_Surface *src, Enesim_Surface_Pixel *color, Enesim_Surface *mask)
{
	Enesim_Format *sfmt;
	char file[256];
	char tmp[256];

	if (!opt_debug)
		return;

	sfmt = enesim_surface_format_get(dst);
	snprintf(tmp, 256, "%s_%s_%s", name, rop_name(rop), enesim_format_name_get(sfmt));
	if (src)
	{
		char tmp2[256];

		sfmt = enesim_surface_format_get(src);
		snprintf(tmp2, 256, "_%s", enesim_format_name_get(sfmt));
		strncat(tmp, tmp2, 256);
	}
	if (mask)
	{
		char tmp2[256];

		sfmt = enesim_surface_format_get(mask);
		snprintf(tmp2, 256, "_%s", enesim_format_name_get(sfmt));
		strncat(tmp, tmp2, 256);
	}
	/* append the color (transparent/opaque) */
	if (color)
	{
		char tmp2[256];

		snprintf(tmp2, 256, "_%s", opacity_get(color));
		strncat(tmp, tmp2, 256);
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
	printf("spanner\n");
	printf("all\n");
}

void fmt_help(void)
{
	Eina_Iterator *it;
	Enesim_Format *f;

	it = enesim_format_iterator_new();
	while (eina_iterator_next(it, (void **)&f))

	{
		printf("%s\n", enesim_format_name_get(f));
	}
	eina_iterator_free(it);
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

	enesim_init();
	fmt_get("argb8888", &opt_fmt);
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
	printf("* FMT = %s\n", enesim_format_name_get(opt_fmt));
	printf("* TIMES = %d\n", opt_times);
	if (!strcmp(opt_bench, "renderer"))
		renderer_bench();
	else if (!strcmp(opt_bench, "rasterizer"))
		rasterizer_bench();
	else if (!strcmp(opt_bench, "drawer"))
		drawer_bench();
	else if (!strcmp(opt_bench, "transformer"))
	{
		//transformer_bench();
	}
	else if (!strcmp(opt_bench, "all"))
	{
		drawer_bench();
		//transformer_bench();
		rasterizer_bench();
		renderer_bench();
	}
	enesim_shutdown();
	/* this bench should be on test
	 * matrix_bench();
	 */

	return 0;
}
