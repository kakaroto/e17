/**********************************************************************
 * identify.c                                             November 2001
 * Horms                                             horms@vergenet.net
 *
 * Clone of ImageMagick's identify programme
 *
 * imlib2_tools
 * Clone of ImageMagick command line programmes using Imlib2.
 * Copyright (C) 2001  Horms
 * 
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, sublicense, and/or sell copies
 * of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *   
 * The above copyright notice and this permission notice shall be
 * included in all copies of the Software, its documentation and
 * marketing & publicity materials, and acknowledgment shall be given
 * in the documentation, materials and software packages that this
 * Software was used.
 *      
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 **********************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <X11/Xlib.h>
#include <Imlib2.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#ifdef HAVE_LIMITS_H
#include <limits.h>
#endif
#include <popt.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <ctype.h>


#define imlib_context_set_colour(_r, _g, _b, _a) \
	imlib_context_set_color(_r, _g, _b, _a)

typedef struct {
	const char **src;
} options_t;


static void usage(int exit_status)
{
	fprintf(exit_status ? stderr : stdout,
		PACKAGE " version " VERSION " Copyright Horms\n\n"
		"Usage: imlib2_identify [options] source ...\n"
		"    options:\n"
		"        -ping (for backwards compatibility\n"
		"        -help\n"
		"\n");

	exit(exit_status);
}


static options_t *options(int argc, char **argv)
{
	options_t *opt;
	poptContext context;
	int c;

	struct poptOption options[] = {
		{"help", 'h', POPT_ARGFLAG_ONEDASH, NULL, 'h'},
		{"ping", 'p', POPT_ARGFLAG_ONEDASH, NULL, 'p'},
		{NULL, 0, 0, NULL, 0}
	};

	opt = (options_t *) malloc(sizeof(options_t));
	if (opt == NULL) {
		fprintf(stderr, "Internal error\n");
		return (NULL);
	}

	opt->src = NULL;

	context = poptGetContext("imlib2_identify", argc, (const char **) argv,
				 options, 0);

	while ((c = poptGetNextOpt(context)) >= 0) {
		switch (c) {
		case 'p':
			/* This option does nothing */
			break;
		case 'h':
			usage(0);
			break;
		default:
			fprintf(stderr, "Unknown option\n\n");
			usage(-1);
			return (NULL);
		}
	}

	if ((opt->src = poptGetArgs(context)) == NULL || opt->src[0] == NULL) {
		fprintf(stderr, "No source file\n\n");
		usage(-1);
	}


	return (opt);
}

static int image_identify(const char *src) 
{
	Imlib_Image image;
	time_t start;
	struct stat stat_buf;
	char *format;
	char *c;

	start = time(NULL);

	if(stat(src, &stat_buf)){
		fprintf(stderr, "Internal error\n");
		return(-1);
	}

	image = imlib_load_image(src);
	if (image == NULL) {
		fprintf(stderr, "Could not load %s\n", src);
		return (-1);
	}
	imlib_context_set_image(image);

	if((format = imlib_image_format()) == NULL) {
		fprintf(stderr, "Internal error\n");
		return(-1);
	}
	if((format = strdup(imlib_image_format())) == NULL) {
		fprintf(stderr, "Internal error\n");
		return(-1);
	}

	for(c = format; *c != '\0' ; c++) {
		*c = toupper(*c);
	}

	printf("%s %dx%d Unknown %ldb %s %.0fs\n", src, 
			imlib_image_get_width(),
			imlib_image_get_height(), 
			stat_buf.st_size, format,
			difftime(start, time(NULL)));

	imlib_free_image();

	return(0);
}

int main(int argc, char **argv)
{
	const char **src;
	options_t *opt;

	if((opt = options(argc, argv)) == NULL) {
		fprintf(stderr, "Error processing options\n\n");
		return(-1);
	}

	for(src = opt->src; *src != NULL ; src++) {
		image_identify(*src);
	}

	return (0);
}
