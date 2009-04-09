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
Enesim_Format opt_fmt = ENESIM_FORMAT_ARGB8888;
char *opt_bench = "drawer";
Enesim_Cpu *opt_cpu = NULL;

double get_time(void)
{
	struct timeval timev;

	gettimeofday(&timev, NULL);
	return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

char * opacity_get(uint32_t color, Enesim_Format f)
{
	uint8_t alpha;

	enesim_surface_pixel_components_to(color, f, &alpha, NULL, NULL, NULL);
	if (alpha == 0xff)
		return "opaque";
	else
		return "transparent";
}

void surface_save(Enesim_Surface *s, const char *name)
{
	image_save(s, name, 0);
}

void surfaces_create(Enesim_Surface **src, Enesim_Format sfmt,
		Enesim_Surface **dst, Enesim_Format dfmt,
		Enesim_Surface **msk, Enesim_Format mfmt)
{
	if (src)
	{
		if (*src) enesim_surface_delete(*src);
		*src = enesim_surface_new(sfmt, opt_width, opt_height);
		test_gradient1(*src);
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
	printf("-c, --cpu             What cpu to use\n");
	printf("-m, --fmt             Surface format to use [argb8888_pre], use help to get a list\n");
	printf("-w, --width           Surface width\n");
	printf("-e, --height          Surface height\n");
	printf("-b, --bench           Benchmark to run, use help to get a list\n");
}

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
	else if (!strcmp(name, "cpu"))
	{
		opt_bench = "cpu";
		return 1;
	}
	return 0;
}


int cpu_get(const char *name, Enesim_Cpu **cpu)
{
	Enesim_Cpu **cpus;
	int numcpus;

	cpus = enesim_cpu_get(&numcpus);
	while (numcpus)
	{
		if (!strcmp(enesim_cpu_name_get(cpus[numcpus - 1]), name))
		{
			*cpu = cpus[numcpus - 1];
			return 1;
		}
		numcpus--;
	}
	return 0;
}

void cpu_help(void)
{
	Enesim_Cpu **cpus;
	int numcpus;

	cpus = enesim_cpu_get(&numcpus);
	while (numcpus)
	{
		printf("%s\n", enesim_cpu_name_get(cpus[numcpus - 1]));
		numcpus--;
	}
}

int fmt_get(const char *name, Enesim_Format *fmt)
{
	Enesim_Format f = 0;

	while (f < ENESIM_FORMATS)
	{
		if (!strcmp(name, enesim_format_name_get(f)))
		{
			*fmt = f;
			return 1;
		}
		f++;
	}
	return 0;
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
	printf("cpu\n");
	printf("all\n");
}

void fmt_help(void)
{
	Enesim_Format f = 0;

	while (f < ENESIM_FORMATS)
	{
		printf("%s\n", enesim_format_name_get(f));
		f++;
	}
}

void test_finish(const char *name, Enesim_Rop rop, Enesim_Surface *dst,
		Enesim_Surface *src, uint32_t *color, Enesim_Surface *mask)
{
	Enesim_Format sfmt;
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

		snprintf(tmp2, 256, "_%s", opacity_get(*color, sfmt));
		strncat(tmp, tmp2, 256);
	}
	snprintf(file, 256, "%s.png", tmp);
	surface_save(dst, file);
}


int main(int argc, char **argv)
{
	Enesim_Cpu **cpus;
	int numcpus;
	char *short_options = "dhf:t:r:m:w:e:b:c:";
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
		{"cpu", 1, 0, 'c'},
		{0, 0, 0, 0}
	};
	int option;
	char c;
	char *file = "benchmark.txt";

	enesim_init();
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
			case 'c':
				if (!cpu_get(optarg, &opt_cpu))
				{
					cpu_help();
					return 1;
				}
				break;
			default:
				break;
		}
	}
ok:
	printf("Enesim Bench\n");
	cpus = enesim_cpu_get(&numcpus);
	if (!numcpus)
	{
		printf("No cpu's registered\n");
		return -1;
	}
	/* default cpu set to index 0 */
	if (!opt_cpu)
	{
		opt_cpu = cpus[0];
	}
	printf("* CPU = %s\n", enesim_cpu_name_get(opt_cpu));
	printf("* BENCH = %s\n", opt_bench);
	printf("* SIZE = %dx%d\n", opt_width, opt_height);
	printf("* ROP = %s\n", rop_name(opt_rop));
	printf("* FMT = %s\n", enesim_format_name_get(opt_fmt));
	printf("* TIMES = %d\n", opt_times);

	if (!strcmp(opt_bench, "renderer"))
	{
		//renderer_bench();
	}
	else if (!strcmp(opt_bench, "rasterizer"))
	{
		//rasterizer_bench();
	}
	else if (!strcmp(opt_bench, "drawer"))
	{
		drawer_bench();
	}
	else if (!strcmp(opt_bench, "transformer"))
	{
		//transformer_bench();
	}
	else if (!strcmp(opt_bench, "cpu"))
	{
		//cpu_bench();
	}
	else if (!strcmp(opt_bench, "all"))
	{
		drawer_bench();
		//transformer_bench();
		//rasterizer_bench();
		//renderer_bench();
	}
	enesim_shutdown();
	/* this bench should be on test
	 * matrix_bench();
	 */

	return 0;
}
