/*
   Module       : options.c
   Purpose      : Read and evaluate commandline options
   More         : see eplay README
   Homepage     : http://mandrake.net
 */

#include "eplay.h"
#ifdef HAVE_GETOPT_LONG
#include <getopt.h>
#else
#include "getopt.h"
#endif

extern char *optarg;
extern int optind, opterr, optopt;

static char *short_options = "hw:exyzmtb:c:g:nipavo:srd:f";
static struct option long_options[] =
{
	{"help", 0, NULL, 'h'},
	{"wm", 1, NULL, 'w'},
	{"center", 0, NULL, 'e'},
	{"root", 0, NULL, 'x'},
	{"root_t", 0, NULL, 'y'},
	{"root_s", 0, NULL, 'z'},
	{"maxpect", 0, NULL, 'm'},
	{"scale_down", 0, NULL, 't'},
	{"brightness", 1, NULL, 'b'},
	{"contrast", 1, NULL, 'c'},
	{"gamma", 1, NULL, 'g'},
	{"no_filter", 0, NULL, 'n'},
	{"no_statusbar", 0, NULL, 'i'},
	{"transparency", 0, NULL, 'p'},
	{"do_grab", 0, NULL, 'a'},
	{"version", 0, NULL, 'v'},
	{"bg_color", 1, NULL, 'o'},
	{"slide", 0, NULL, 's'},
	{"random", 0, NULL, 'r'},
	{"delay", 1, NULL, 'd'},
	{"fullscreen", 0, NULL, 'f'},
	{0, 0, NULL, 0}
};

int options_read(int argc, char **argv)
{
	int long_index;
	char c;

	while ((c = getopt_long(argc, argv, short_options,
							long_options, &long_index)) != -1) {
		switch (c) {
			case 'h':
				show_help(argv[0], 0);
				break;
			case 'v':
				g_print("eplay (e flipbook) v%s\n", VERSION);
				gdk_exit(0);
				break;
			case 0:
			case '?':
				usage(argv[0], 1);
				gdk_exit(0);
			default:
				break;
		}
	}

	return (optind);
}
