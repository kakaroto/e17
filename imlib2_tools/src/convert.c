/**********************************************************************
 * convert.c                                              November 2001
 * Horms                                             horms@vergenet.net
 *
 * Clone of ImageMagick's convert programme
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
#include <math.h>

#include "dither.h"

#define DEFAULT_CACHE_SIZE 80 /* In megabytes */
#define DEFAULT_QUALITY 75
#define DEFAULT_COMPRESSION 3

#define imlib_context_set_colour(_r, _g, _b, _a) \
	imlib_context_set_color(_r, _g, _b, _a)

#define imlib_create_colour_modifier() \
	imlib_create_color_modifier()

#define imlib_get_colour_modifier_tables(_r, _g, _b, _a) \
	imlib_get_color_modifier_tables(_r, _g, _b, _a)

#define imlib_set_colour_modifier_tables(_r, _g, _b, _a) \
	imlib_set_color_modifier_tables(_r, _g, _b, _a)

#define imlib_context_set_colour_modifier(_m) \
	imlib_context_set_color_modifier(_m)

#define imlib_reset_colour_modifier() \
	imlib_reset_color_modifier();

#define imlib_modify_colour_modifier_gamma(_g) \
	imlib_modify_color_modifier_gamma(_g)

#define imlib_apply_colour_modifier() \
	imlib_apply_color_modifier()

#define imlib_free_colour_modifier() \
	imlib_apply_color_modifier()

#define __SAFE_FREE(_ptr) \
	if(_ptr!=NULL) { \
		free(_ptr); \
	} \
	_ptr = NULL;

#define __OP_FLIP           0x0001
#define __OP_FLOP           0x0002
#define __OP_FLOP           0x0002
#define __OP_AVERAGE        0x0004
#define __OP_NOANTIALIAS    0x0010
#define __OP_MONOCHROME     0x0020
#define __OP_ORDERED_DITHER 0x0040

typedef struct {
	const char *blur;
	const char *border;
	const char *bordercolour;
	const char *colourise;
	int compression;
	const char *crop;
	const char *channel;
	const char *cache;
	const char *fill;
	const char *scale;
	const char *gamma;
	const char *rotate;
	int quality;
	int nfiles;
	const char **files;
	const char *src;
	const char *dest;
	int bin_ops;
} options_t;


static void usage(int exit_status)
{
	fprintf(exit_status ? stderr : stdout,
		PACKAGE " version " VERSION " Copyright Horms\n\n"
		"Usage: imlib2_convert [options] source destination\n"
		"  options:\n"
		"    -average\n"
		"    -[no]antialias\n"
		"    -blur          <radius>[%%]\n"
		"    -border        <width>[x<height>]\n"
		"    -bordercolour  #<rr><gg><bb>[<aa>]\n"
		"    -cache         megabytes\n"
		"                   (default is %d)\n"
		"    -crop          <width>[x<height>][%%]\n"
		"    -channel       Red|Green|Blue|Alpha\n"
		"                   Matte has the same meaning as Alpha\n"
		"    -coalesce      (The same as average)\n"
		"    -colourise     <percentage>|<red>/<greem>/><blue>\n"
		"    -compression   <integer>\n"
		"                   Must be between 1 and 9. (default is %d)\n"
		"    -[no]dither    Use Floyd Steinberg Dithering instead\n"
		"                   of ordered dithering (default)\n"
		"    -flip\n"
		"    -flop\n"
		"    -gamma         <gamma>|<red>/<green>/<blue>[/<alpha>]\n"
		"    -geometry      <width>[x<height>][%%][<|>][!]\n"
		"    -help\n"
		"    -monochrome\n"
		"    -quality       <integer>\n"
		"                   Must be between 1 and 99. (default is %d)\n"
		"    -rotate        <angle>[<|>]\n"
		"    -scale         <width>[x<height>][%%][<|>][!]\n"
		"\n" 
		" -compression and -quality only only affect saving to image\n"
		" formats whose loader supoorts this option. Currently jpeg\n"
		" and png. Internally they are much the same thing, and its\n"
		" usual to use -compression for png and -quality for jpeg.\n"
		" Accordinlgy, internally, -compression takes prefereance\n"
		" over -quality for png, and vice-versa for jpeg.\n"
		"\n"
		" -[no]dither currently affects images when used in\n"
		" conjunction with the -monocrome option\n"
		"\n",
		DEFAULT_CACHE_SIZE, DEFAULT_COMPRESSION, DEFAULT_QUALITY);

	exit(exit_status);
}



#define __OPTIONS_SAFE_FREE(ptr) \
	if(ptr!=NULL) { \
		free(ptr); \
	} \
	ptr = NULL;

static options_t *options(int argc, char **argv)
{
	options_t *opt;
	poptContext context;
	const char *optarg = NULL;
	const char **file;
	int c;

	struct poptOption options[] = {
		{"average", 'a', POPT_ARGFLAG_ONEDASH, NULL, 'a'},
		{"coalesce", 'a', POPT_ARGFLAG_ONEDASH, NULL, 'a'},
		{"antialias", 'A', POPT_ARGFLAG_ONEDASH, NULL, 'A'},
		{"noantialias", 'n', POPT_ARGFLAG_ONEDASH, NULL, 'n'},
		{"blur", 'b', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 'b'},
		{"border", '\0', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 128},
		{"bordercolour", '\0',
		 POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 129},
		{"bordercolor", '\0',
		 POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 129},
		{"cache", '\0', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 130},
		{"colourise", '\0', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 131},
		{"colorize", '\0', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 131},
		{"compression", '\0', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 132},
		{"crop", 'c', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 'c'},
		{"channel", 'C', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 'C'},
		{"flip", 'f', POPT_ARGFLAG_ONEDASH, NULL, 'f'},
		{"flop", 'F', POPT_ARGFLAG_ONEDASH, NULL, 'F'},
		{"gamma", 'G', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 'G'},
		{"geometry", 'g', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 'g'},
		{"scale", 'g', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 'g'},
		{"help", 'h', POPT_ARGFLAG_ONEDASH, NULL, 'h'},
		{"monochrome", 'm', POPT_ARGFLAG_ONEDASH, NULL, 'm'},
		{"dither", 'd', POPT_ARGFLAG_ONEDASH, NULL, 'd'},
		{"nodither", 'N', POPT_ARGFLAG_ONEDASH, NULL, 'N'},
		{"quality", 'q', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 'q'},
		{"rotate", 'r', POPT_ARG_STRING | POPT_ARGFLAG_ONEDASH,
		 NULL, 'r'},
		{NULL, 0, 0, NULL, 0}
	};

	opt = (options_t *) malloc(sizeof(options_t));
	if (opt == NULL) {
		fprintf(stderr, "Could not allocate memory for options.\n");
		return (NULL);
	}

	opt->blur = NULL;
	opt->border = NULL;
	opt->bordercolour = NULL;
	opt->colourise = NULL;
	opt->compression = DEFAULT_COMPRESSION;
	opt->crop = NULL;
	opt->bin_ops = 0;
	opt->fill = NULL;
	opt->gamma = NULL;
	opt->scale = NULL;
	opt->quality = DEFAULT_QUALITY;
	opt->rotate = NULL;
	opt->src = NULL;
	opt->dest = NULL;
	opt->files = NULL;
	opt->nfiles = 0;

	context = poptGetContext("imlib2_convert", argc, (const char **) argv,
				 options, 0);

	while ((c = poptGetNextOpt(context)) >= 0) {
		optarg = poptGetOptArg(context);

		switch (c) {
		case 'A':
			/* Does nothing as this is the default */
			break;
		case 'n':
			opt->bin_ops |= __OP_NOANTIALIAS;
			break;
		case 'a':
			opt->bin_ops |= __OP_AVERAGE;
			break;
		case 'b':
			opt->blur = optarg;
			break;
		case 128:
			opt->border = optarg;
			break;
		case 129:
			opt->bordercolour = optarg;
			break;
		case 130:
			opt->cache = optarg;
			break;
		case 131:
			opt->colourise = optarg;
			break;
		case 132:
			opt->compression = atoi(optarg);
			if (opt->compression < 1 || opt->compression > 9)
				usage(-1);
			break;
		case 'c':
			opt->crop = optarg;
			break;
		case 'C':
			opt->channel = optarg;
			break;
		case 'f':
			opt->bin_ops |= __OP_FLIP;
			break;
		case 'F':
			opt->bin_ops |= __OP_FLOP;
			break;
		case 'g':
			opt->scale = optarg;
			break;
		case 'G':
			opt->gamma = optarg;
			break;
		case 'h':
			usage(0);
			break;
		case 'q':
			opt->quality = atoi(optarg);
			if (opt->quality < 1 || opt->quality > 99)
				usage(-1);
			break;
		case 'r':
			opt->rotate = optarg;
			break;
		case 'm':
			opt->bin_ops |= __OP_MONOCHROME;
			break;
		case 'd':
			/* Does nothing as this is the default */
			break;
		case 'N':
			opt->bin_ops |= __OP_ORDERED_DITHER;
			break;
		default:
			fprintf(stderr, "Unknown option\n\n");
			usage(-1);
			return (NULL);
		}
	}

	if ((opt->files = poptGetArgs(context)) == NULL ||
			(opt->src = *(opt->files)) == NULL) {
		fprintf(stderr, "No source or destination file\n\n");
		usage(-1);
	}

	for(file = opt->files, opt->nfiles = 0; *file != NULL; 
			file++, opt->nfiles++) { 
		opt->dest = *file; 
	}
	if (opt->dest == opt->src) {
		fprintf(stderr, "No destination file\n\n");
		usage(-1);
	}

	return (opt);
}


static int calculate_colour(const char *spec, int *r, int *g, int *b,
		int *a)
{
	char *end;
	unsigned long int l;
	int orig_w;
	int orig_h;

	orig_w = imlib_image_get_width();
	orig_h = imlib_image_get_height();

	*r = 0;
	*g = 0;
	*b = 0;
	*a = 255;

	if (spec == NULL) {
		return (0);
	}

	if (*spec++ != '#') {
		return (-1);
	}

	l = strtoul(spec, &end, 16);
	if (l < 0 || l == ULONG_MAX || *end != '\0') {
		return (-1);
	}

	if(strlen(spec) > 6) {
		*r = (l >> 24) & 0xff;
		*g = (l >> 16) & 0xff;
		*b = (l >> 8)  & 0xff;
		*a = l & 0xff;
	}
	else {
		*r = (l >> 16) & 0xff;
		*g = (l >> 8)  & 0xff;
		*b = l & 0xff;
	}

	return (0);
}


static int calculate_geometry(const char *spec, int *w, int *h)
{
	char *end;
	long int l;
	int absolute = 0;
	int decrease = 0;
	int increase = 0;
	int percentage = 0;
	int orig_w;
	int orig_h;

	orig_w = imlib_image_get_width();
	orig_h = imlib_image_get_height();

	if (spec == NULL) {
		*w = orig_w;
		*h = orig_h;
		return (0);
	}

	l = strtol(spec, &end, 10);
	if (l < 0 || l > INT_MAX) {
		return (-1);
	}
	*w = (int) l;
	*h = *w;

	if (*end == 'x') {
		spec = end + 1;
		l = strtol(spec, &end, 10);
		if (l < 0 || l > INT_MAX) {
			return (-1);
		}
		*h = (int) l;
	}

	for (spec = end; *spec != '\0'; spec++) {
		switch (*spec) {
		case '%':
			percentage++;
			break;
		case '>':
			increase++;
			break;
		case '<':
			decrease++;
			break;
		case '!':
			absolute++;
			break;
		default:
			return (-1);
		}
	}

	if (decrease && increase) {
		return (-1);
	}

	if (percentage) {
		*w = *w * orig_w / 100;
		*h = *h * orig_h / 100;
	}

	if (!absolute) {
		double ratio_w;
		double ratio_h;

		ratio_w = (double) *w / (double) orig_w;
		ratio_h = (double) *h / (double) orig_h;

		if (ratio_h > ratio_w) {
			*h = orig_h * ratio_w;
		} else {
			*w = orig_w * ratio_h;
		}
	}

	if (decrease) {
		if (!(orig_w > *w && orig_h > *h)) {
			*w = orig_w;
			*h = orig_h;
		}
	} else if (increase) {
		if (!(orig_w < *w && orig_h < *h)) {
			*w = orig_w;
			*h = orig_h;
		}
	}

	return (0);
}


static Imlib_Image image_scale(Imlib_Image image, const char *geometry)
{
	Imlib_Image out_image;
	int w = 0;
	int h = 0;

	imlib_context_set_image(image);

	if (geometry == NULL) {
		return (image);
	}

	if (calculate_geometry(geometry, &w, &h) < 0) {
		fprintf(stderr, "Invalid geometry\n\n");
		usage(-1);
	}

	if (w == imlib_image_get_width() && h == imlib_image_get_height()) {
		return (image);
	}

	if(w == 0 || h == 0) {
		fprintf(stderr, "zero dimension for scale\n");
		exit(-1);
	}

	out_image = imlib_create_cropped_scaled_image(0, 0,
						      imlib_image_get_width
						      (),
						      imlib_image_get_height
						      (), w, h);
	if (out_image == NULL) {
		fprintf(stderr, "Could not create scaled image\n");
		return (NULL);
	}

	imlib_free_image();
	image = out_image;
	imlib_context_set_image(image);

	return (image);
}

#define __CROP_MULTI 0x01


static int calculate_crop(const char *spec, int *w, int *h, int *x, int *y,
		int *flags)
{
	char *end;
	long int l;
	int percentage = 0;
	int orig_w;
	int orig_h;
	int x_dir = 1;
	int y_dir = 1;

	orig_w = imlib_image_get_width();
	orig_h = imlib_image_get_height();

	*x = 0;
	*y = 0;
	*flags = __CROP_MULTI;
	*w = orig_w;
	*h = orig_h;

	if (spec == NULL) {
		return (0);
	}

	l = strtol(spec, &end, 10);
	if (l < 0 || l > INT_MAX) {
		return (-1);
	}
	*w = (int) l;

	if (*end == 'x') {
		spec = end + 1;
		l = strtol(spec, &end, 10);
		if (l < 0 || l > INT_MAX) {
			return (-1);
		}
		*h = (int) l;
	}

	if (*end == '+' || *end == '-') {
		*flags = 0;
		spec = end + 1;
		l = strtol(spec, &end, 10);
		if (l < 0 || l > INT_MAX) {
			return (-1);
		}
		*x = (int) l;
		if (*(spec - 1) == '-') {
			x_dir = -1;
		}
	}

	if (*end == '+' || *end == '-') {
		spec = end + 1;
		l = strtol(spec, &end, 10);
		if (l < 0 || l > INT_MAX) {
			return (-1);
		}
		*y = (int) l;
		if (*(spec - 1) == '-') {
			y_dir = -1;
		}
	}

	for (spec = end; *spec != '\0'; spec++) {
		switch (*spec) {
		case '%':
			percentage++;
			break;
		default:
			return (-1);
		}
	}

	if (percentage) {
		*w = *w * orig_w / 100;
		*h = *h * orig_h / 100;
	}

	if (x_dir < 0) {
		*x = orig_w - *w - *x;
	}
	if (y_dir < 0) {
		*y = orig_h - *h - *y;
	}

	if(*w == 0 && *h ==0) {
		*flags = 0;
	}


	return (0);
}


static void image_crop_multi(Imlib_Image image, const int w, 
		const int h, const char *dest)
{
	int x;
	int y;
	int ww;
	int hh;
	int no;
	int len;
	int orig_w;
	int orig_h;
	int digits;
	char *buf;
	char *name;
	char *suffix;
	Imlib_Image out_image;

	imlib_context_set_image(image);
	orig_w = imlib_image_get_width();
	orig_h = imlib_image_get_height();

	no = 0;
	for(y = 0 ; y < orig_h ; y+=h) {
		for(x = 0 ; x < orig_w ; x+=w) {
			no++;
		}
	}

	digits = (int)log10(no) + 1;
	len = strlen(dest) +  digits + 2;

	if((buf = (char *)malloc(len)) == NULL ||
			(name = strdup(dest)) == NULL) {
		fprintf(stderr, "Could not allocate memory for filename\n");
		exit (-1);
	}

	if((suffix = strrchr(name, '.')) == NULL || *(suffix+1) == '\0') {
		fprintf(stderr, "No suffix in filename\n");
		exit (-1);
	}

	*suffix++ = '\0';

	no = 0;
	for(y = 0 ; y < orig_h ; y+=h) {
		for(x = 0 ; x < orig_w ; x+=w) {
			ww = (w+x>orig_w)?orig_w-x:w;
			hh = (h+y>orig_h)?orig_h-y:h;
			out_image = imlib_create_cropped_image(x, y, 
					ww, hh);
			if (out_image == NULL) {
				fprintf(stderr, "Could not create cropped "
						"image\n");
				exit (-1);
			}
			imlib_context_set_image(out_image);

			snprintf(buf, len, "%s.%0*d.%s", name, digits, no, 
					suffix);
			imlib_save_image(buf);
			imlib_free_image();
			imlib_context_set_image(image);
			no++;
		}
	}

	free(name);
	free(buf);

	exit(0);
}


static Imlib_Image image_crop_single(Imlib_Image image, 
		const int x, const int y, const int w, const int h)
{
	Imlib_Image out_image;

	out_image = imlib_create_cropped_image(x, y, w, h);
	if (out_image == NULL) {
		fprintf(stderr, "Could not create cropped image\n");
		return (NULL);
	}

	imlib_free_image();
	image = out_image;
	imlib_context_set_image(image);

	return (image);
}


/* Compare two pixels, ignoring the alpha channel */
#define __image_crop_auto_cmp(_p1, _p2) \
	((_p1&0xffffff) == (_p2&0xffffff))


static Imlib_Image image_crop_auto(Imlib_Image image, const int x,
		const int y, const char *bordercolour)
{
	int w;
	int h;
	int orig_w;
	int orig_h;
	int xx;
	int xxx;
	int yy;
	int x_offset;
	int y_offset;
	int r;
	int g;
	int b;
	int a;
	DATA32 *data;
	DATA32 *pixel;
	DATA32 desired;

	if (calculate_colour(bordercolour, &r, &g, &b, &a) < 0) {
		fprintf(stderr, "Invalid bordercolour\n\n");
		usage(-1);

	}
	desired = (a<<24) + (r<<16) + (g<<8) + b;

	imlib_context_set_image(image);
	if((data=imlib_image_get_data_for_reading_only()) == NULL) {
		fprintf(stderr, "Could not get image data for autocrop\n");
		return(NULL);
	}

	imlib_context_set_image(image);
	orig_w = imlib_image_get_width();
	orig_h = imlib_image_get_height();

	/* Find y offset */
	pixel = data;
	y_offset = 0;
	for(yy = 0; yy < orig_h; yy++){
		for(xx = 0; xx < orig_w; xx++){
			if(!__image_crop_auto_cmp(*pixel, desired)) {
				break;
			}
			pixel++;
		}
		if(!__image_crop_auto_cmp(*pixel, desired)) {
			break;
		}
		y_offset=yy+1;
	}

	/* Find height */
	pixel = data + (orig_h * orig_w) - 1;
	h = orig_h;
	for(yy = orig_h - 1 ; yy >= y_offset ; yy--){
		for(xx = orig_w - 1 ; xx >= 0 ; xx--){
			if(!__image_crop_auto_cmp(*pixel, desired)) {
				break;
			}
			pixel--;
		}
		if(!__image_crop_auto_cmp(*pixel, desired)) {
			break;
		}
		h=yy;
	}
	h -= y_offset;

	/* Find x offset */
	x_offset = orig_w;
	for(yy = y_offset; yy < y_offset+h; yy++){
		pixel = (yy > 0)?data + (orig_w * yy) - 1:data;
		for(xx = 0, xxx = 0; xx < orig_w; xx++){
			if(!__image_crop_auto_cmp(*pixel, desired)) {
				break;
			}
			if(xx>xxx) {
				xxx=xx;
			}
			pixel++;
		}
		if(xxx < x_offset) {
			x_offset = xxx;
		}
	}
	
	/* Find width */
	w = 0;
	for(yy = h +y_offset - 1; yy > y_offset; yy--){
		pixel = (yy > 0)?data + (orig_w * yy) - 1:data;
		for(xx = orig_w, xxx = orig_w ; xx >= x_offset; xx--){
			if(!__image_crop_auto_cmp(*pixel, desired)) {
				break;
			}
			if(xx-1<xxx) {
				xxx=xx-1;
			}
			pixel--;
		}
		if(w < xxx) {
			w = xxx;
		}
	}
	w -= x_offset;

	if(x>x_offset) {
		w += x_offset;
		x_offset=0;
	}
	else {
		w += x;
		x_offset -= x;
	}

	if(y>y_offset) {
		h += y_offset;
		y_offset=0;
	}
	else {
		h += y;
		y_offset -= y;
	}

	w=(x > orig_w - w)?orig_w:w+x;
	h=(y > orig_h - h)?orig_h:h+y;

	return(image_crop_single(image, x_offset, y_offset, w, h));
}


static Imlib_Image image_crop(Imlib_Image image, const char *crop, 
		const char *dest, const char *bordercolour)
{
	int w = 0;
	int h = 0;
	int x = 0;
	int y = 0;
	int flags = 0;

	imlib_context_set_image(image);

	if (crop == NULL) {
		return (image);
	}

	if (calculate_crop(crop, &w, &h, &x, &y, &flags) < 0) {
		fprintf(stderr, "Invalid crop\n\n");
		usage(-1);
	}

	if(w == 0 && h == 0) {
		return(image_crop_auto(image, x, y, bordercolour));
	}

	if(w == 0 || h == 0) {
		fprintf(stderr, "zero dimension for crop\n");
		exit(-1);
	}

	if (w == imlib_image_get_width() && h == imlib_image_get_height()) {
		return (image);
	}

	if(flags&__CROP_MULTI) {
		/* image_crop_multi does not return */
		image_crop_multi(image, w, h, dest);
	}
	else {
		image = image_crop_single(image, x, y, w, h);
		if (image == NULL) {
			fprintf(stderr, "Could not crop image\n");
			return (NULL);
		}
	}

	return (image);
}


static int calculate_blur(const char *spec, int *r)
{
	char *end;
	double d;
	int percentage = 0;
	int orig_w;
	int orig_h;

	if (spec == NULL) {
		*r = 0;
		return (0);
	}

	d = strtod(spec, &end);
	if (d < 0 || d > INT_MAX) {
		return (-1);
	}

	for (spec = end; *spec != '\0'; spec++) {
		switch (*spec) {
		case '%':
			percentage++;
			break;
		default:
			return (-1);
		}
	}

	if (percentage) {
		if (d >= 100) {
			return (-1);
		}
		orig_w = imlib_image_get_width();
		orig_h = imlib_image_get_height();

		d = d * (double) ((orig_w > orig_h) ? orig_w : orig_h) /
		    100;
	}

	*r = (int) d;

	return (0);
}


static Imlib_Image image_blur(Imlib_Image image, const char *blur)
{
	int r;

	imlib_context_set_image(image);

	if (blur == NULL) {
		return (image);
	}

	if (calculate_blur(blur, &r) < 0) {
		fprintf(stderr, "Invalid blur\n\n");
		usage(-1);
	}

	if (r != 0) {
		imlib_image_blur(r);
	}

	return (image);
}


static int calculate_border(const char *spec, int *x, int *y, int *w,
			    int *h)
{
	char *end;
	long int l;
	int orig_w;
	int orig_h;

	orig_w = imlib_image_get_width();
	orig_h = imlib_image_get_height();

	if (spec == NULL) {
		*x = 0;
		*y = 0;
		*w = orig_w;
		*h = orig_h;
		return (0);
	}

	l = strtol(spec, &end, 10);
	if (l < 0 || l > INT_MAX) {
		return (-1);
	}

	*x = (int) l;
	*y = *x;

	if (*end == 'x') {
		spec = end + 1;
		l = strtol(spec, &end, 10);
		if (l < 0 || l > INT_MAX) {
			return (-1);
		}
		*y = (int) l;
	}

	if (*end != '\0') {
		return (-1);
	}

	*w = *x + *x + orig_w;
	*h = *y + *y + orig_h;

	return (0);
}


static Imlib_Image image_border(Imlib_Image image, const char *border,
				const char *bordercolour)
{
	int x;
	int y;
	int w;
	int h;
	int r;
	int g;
	int b;
	int a;
	int src_w;
	int src_h;
	Imlib_Image new_image;

	imlib_context_set_image(image);

	if (border == NULL) {
		return (image);
	}

	src_w = imlib_image_get_width();
	src_h = imlib_image_get_height();

	if (calculate_colour(bordercolour, &r, &g, &b, &a) < 0) {
		fprintf(stderr, "Invalid bordercolour\n\n");
		usage(-1);

	}

	if (calculate_border(border, &x, &y, &w, &h) < 0) {
		fprintf(stderr, "Invalid border\n\n");
		usage(-1);
	}

	if (x == 0 && y == 0) {
		return (image);
	}

	if ((new_image = imlib_create_image(w, h)) < 0) {
		fprintf(stderr, "Could not create new image with border\n");
		return (NULL);
	}

	imlib_context_set_image(new_image);
	imlib_context_set_colour(r, g, b, a);
	imlib_image_fill_rectangle(0, 0, w, h);
	imlib_blend_image_onto_image(image, 0, 0, 0, src_w, src_h,
				     x, y, src_w, src_h);

	return (new_image);
}


static Imlib_Image image_flop(Imlib_Image image, const int flop)
{
	imlib_context_set_image(image);

	if (flop) {
		imlib_image_flip_horizontal();
	}

	return (image);
}


static Imlib_Image image_flip(Imlib_Image image, const int flip)
{
	imlib_context_set_image(image);

	if (flip) {
		imlib_image_flip_vertical();
	}

	return (image);
}


static int calculate_gamma(const char *spec, double *r, double *g,
			   double *b, double *a)
{
	char *end;

	*r = 1.0;
	*g = 1.0;
	*b = 1.0;
	*a = 1.0;
		
	if (spec == NULL) {
		return (0);
	}

	*r = strtod(spec, &end);
	if (*r < 0 || *r > INT_MAX) {
		return (-1);
	}
	*g = *r;
	*b = *r;

	if (*end == '\0') {
		return (0);
	}

	if (*end != '/') {
		return (-1);
	}

	spec = end + 1;
	*g = strtod(spec, &end);
	if (*g < 0 || *g > INT_MAX) {
		return (-1);
	}

	if (*end != '/') {
		return (-1);
	}

	spec = end + 1;
	*b = strtod(spec, &end);
	if (*b < 0 || *b > INT_MAX) {
		return (-1);
	}

	if (*end == '\0') {
		return (0);
	}

	spec = end + 1;
	*a = strtod(spec, &end);
	if (*a < 0 || *a > INT_MAX) {
		return (-1);
	}

	if (*end != '\0') {
		return (-1);
	}

	return (0);
}


static Imlib_Image image_gamma_numeric(Imlib_Image image, double r,
		double g, double b, double a)
{
	Imlib_Color_Modifier modifier;
	DATA8 r_table[256];
	DATA8 g_table[256];
	DATA8 b_table[256];
	DATA8 a_table[256];
	DATA8 dummy_table[256];

	imlib_context_set_image(image);

	if (r == 1.0 && g == 1.0 && b == 1.0 && a == 1.0) {
		return (image);
	}

	if ((modifier = imlib_create_colour_modifier()) == NULL) {
		fprintf(stderr, "Could not create colour modifier\n");
		return (NULL);
	}

	imlib_context_set_colour_modifier(modifier);
	imlib_reset_colour_modifier();
	if (r == g && r == b && r == a) {
		imlib_modify_colour_modifier_gamma(r);
	} else {
		imlib_get_colour_modifier_tables(r_table, g_table, b_table,
						a_table);

		if(r != 1.0) {
			imlib_modify_colour_modifier_gamma(r);
			imlib_get_colour_modifier_tables(r_table, dummy_table, 
					dummy_table, dummy_table);
			imlib_reset_colour_modifier();
		}

		if(g != 1.0) {
			imlib_modify_colour_modifier_gamma(g);
			imlib_get_colour_modifier_tables(dummy_table, g_table,
							dummy_table, 
							dummy_table);
			imlib_reset_colour_modifier();
		}

		if(b != 1.0) {
			imlib_modify_colour_modifier_gamma(b);
			imlib_get_colour_modifier_tables(dummy_table, 
					dummy_table, b_table, dummy_table);
			imlib_reset_colour_modifier();
		}

		if(a != 1.0) {
			imlib_modify_colour_modifier_gamma(a);
			imlib_get_colour_modifier_tables(dummy_table, 
					dummy_table, dummy_table, a_table);
			imlib_reset_colour_modifier();
		}

		imlib_set_colour_modifier_tables(r_table, g_table, b_table,
						a_table);
	}

	imlib_apply_colour_modifier();
	imlib_free_colour_modifier();

	return (image);
}


static Imlib_Image image_gamma(Imlib_Image image, const char *spec)
{
	double r;
	double g;
	double b;
	double a;

	imlib_context_set_image(image);

	if (spec == NULL) {
		return (image);
	}

	if (calculate_gamma(spec, &r, &g, &b, &a) < 0) {
		fprintf(stderr, "Invalid gamma\n\n");
		usage(-1);
	}

	return(image_gamma_numeric(image, r, g, b, a));
}


static int calculate_angle(const char *spec, double *a)
{
	int increase = 0;
	int decrease = 0;
	char *end;
	int w;
	int h;

	if (spec == NULL) {
		*a = 0.0;
		return (0);
	}

	*a = strtod(spec, &end);
	if (*a < INT_MIN || *a > INT_MAX) {
		return (-1);
	}

	for (spec = end; *spec != '\0'; spec++) {
		switch (*spec) {
		case '>':
			increase++;
			break;
		case '<':
			decrease++;
			break;
		default:
			return (-1);
		}
	}

	if(increase && decrease) {
		return(-1);
	}

	w = imlib_image_get_width();
	h = imlib_image_get_height();

	if(increase && !(w>h)) {
		*a = 0.0;
	}
	else if(decrease && !(w<h)) {
		*a = 0.0;
	}
	
	while(*a >= 360) {
		*a -= 360;
	}
	while(*a <= -360) {
		*a += 360;
	}
	if(*a < 0) {
		*a = 360 + *a;
	}

	return (0);
}


static Imlib_Image image_rotate(Imlib_Image image, const char *spec,
		const char *bordercolour)
{
	int r;
	int g;
	int b;
	int a;
	int w;
	int h;
	double angle;
	Imlib_Image new_image;

	imlib_context_set_image(image);

	if (spec == NULL) {
		return (image);
	}

	if (calculate_angle(spec, &angle) < 0) {
		fprintf(stderr, "Invalid angle\n\n");
		usage(-1);
	}

	if (angle == 0.0) {
		return (image);
	}
	else if (angle == 90.0) {
		imlib_image_orientate(1);
		return(image);
	}
	else if (angle == 180.0) {
		imlib_image_orientate(2);
		return(image);
	}
	else if (angle == 270.0) {
		imlib_image_orientate(3);
		return(image);
	}

	angle = angle/90*acos(0);

	if (calculate_colour(bordercolour, &r, &g, &b, &a) < 0) {
		fprintf(stderr, "Invalid bordercolour\n\n");
		usage(-1);

	}

	if ((new_image = imlib_create_rotated_image(angle)) == NULL) {
		fprintf(stderr, "Could not create rotated image\n");
		return (NULL);
	}

	imlib_free_image();
	imlib_context_set_image(new_image);
	w = imlib_image_get_width();
	h = imlib_image_get_height();

	if ((image = imlib_create_image(w, h)) < 0) {
		fprintf(stderr, "Could not create image for rotation\n");
		return (NULL);
	}

	imlib_context_set_image(image);
	imlib_context_set_colour(r, g, b, a);
	imlib_image_fill_rectangle(0, 0, w, h);
	imlib_blend_image_onto_image(new_image, 0, 0, 0, w, h,
				     0, 0, w, h);
	
	imlib_context_set_image(new_image);
	imlib_free_image();
	imlib_context_set_image(image);

	return(image_crop_auto(image, 0, 0, bordercolour));
}

#define __IMAGE_AVERAGE_CLEAN_UP \
	__SAFE_FREE(data); \
	__SAFE_FREE(data_r); \
	__SAFE_FREE(data_g); \
	__SAFE_FREE(data_b); \
	__SAFE_FREE(data_a);

Imlib_Image image_average(Imlib_Image image, const int average,
		const char **files) {
	DATA32 *data_r = NULL;
	DATA32 *data_g = NULL;
	DATA32 *data_b = NULL;
	DATA32 *data_a = NULL;
	DATA32 *data = NULL;
	const char **file;
	int nfile;
	int w;
	int h;
	int i;
	int size;

	imlib_context_set_image(image);

	if(!average) {
		return(image);
	}

	w = imlib_image_get_width();
	h = imlib_image_get_height();
	imlib_free_image();

	size = w*h*sizeof(DATA32);

	if((data_r = (DATA32 *)malloc(size)) == NULL ||
			(data_g = (DATA32 *)malloc(size)) == NULL ||
			(data_b = (DATA32 *)malloc(size)) == NULL ||
			(data_a = (DATA32 *)malloc(size)) == NULL) {
		fprintf(stderr, "Could not allocate memory for averaging\n");
		return(NULL);
	}
	memset(data_r, 0, size);
	memset(data_g, 0, size);
	memset(data_b, 0, size);
	memset(data_a, 0, size);

	size = w*h;
	for(file = files, nfile = 0 ; *(file + 1) != NULL ;
			file++, nfile++) {
		image = imlib_load_image(*file);
		if (image == NULL) {
			fprintf(stderr, "Could not load %s\n", *file);
			return (NULL);
		}
		imlib_context_set_image(image);

		if(w != imlib_image_get_width() ||
				h != imlib_image_get_height()) {
			fprintf(stderr, "Images are not the same width, "
					"cannot average\n");
			__IMAGE_AVERAGE_CLEAN_UP;
			return(NULL);
		}

		if((data = imlib_image_get_data()) == NULL) {
			__IMAGE_AVERAGE_CLEAN_UP;
			fprintf(stderr, "Could not get image data for "
					"averging\n");
			return(NULL);
		}

		for(i = 0 ; i < size ; i++) {
			data_a[i] += (data[i] >> 24) & 0xff;
			data_r[i] += (data[i] >> 16) & 0xff;
			data_g[i] += (data[i] >> 8) & 0xff;
			data_b[i] += data[i] & 0xff;
		}

		imlib_free_image();
	}

	size = w*h*sizeof(DATA32);
	if((data = (DATA32 *)malloc(size)) == NULL) {
		__IMAGE_AVERAGE_CLEAN_UP;
		fprintf(stderr, "Could not allocate memory for averaging\n");
		return(NULL);
	}
	
	size = w*h;
	for(i = 0 ; i < size ; i++) {
		data[i] = ((data_a[i]/nfile) << 24)
			+ ((data_r[i]/nfile) << 16)
			+ ((data_g[i]/nfile) << 8)
			+ (data_b[i]/nfile);
	}

	image = imlib_create_image_using_copied_data(w, h, data);
	if(image == NULL) {
		__IMAGE_AVERAGE_CLEAN_UP;
		fprintf(stderr, "Could not create image for averaging\n");
		return(NULL);
	}

	__IMAGE_AVERAGE_CLEAN_UP;
	imlib_context_set_image(image);

	return(image);
}


static int calculate_channel(const char *channel, double *r, double *g, 
		double *b, double *a) 
{
	if(channel == NULL) {
		return(-1);
	}

	*r = 0;
	*g = 0;
	*b = 0;
	*a = 0;

	if(strcasecmp("red", channel) == 0) {
		*r = 1;
	}
	else if(strcasecmp("green", channel) == 0) {
		*g = 1;
	}
	else if(strcasecmp("blue", channel) == 0) {
		*b = 1;
	}
	else if(strcasecmp("alpha", channel) == 0 ||
			strcasecmp("matte", channel) == 0) {
		*a = 1;
	}
	else {
		return(-1);
	}

	return(0);
}

static Imlib_Image image_channel(Imlib_Image image, const char *channel)
{
	double r;
	double g;
	double b;
	double a;

	if(calculate_channel(channel, &r, &g, &b, &a) < 0) {
		return(image);
	}

	imlib_context_set_image(image);

	image_gamma_numeric(image, r, g, b, a);

	return(image);
}


static Imlib_Image image_monochrome(Imlib_Image image, 
		const int monochrome, const int ordered_dither)
{
	imlib_context_set_image(image);

	if(!monochrome) {
		return(image);
	}

	if(ordered_dither) {
		return(dither_ordered_image(image, 8));
	}

	return(dither_floyd_image(image, DITHER_FLOYD_ALTERNATE));
}


#define __IMAGE_COLOURISE_8BIT(i) \
	((i<0)?0:((i>255)?255:i))

static Imlib_Image image_colourise(Imlib_Image image,
		const char *colourise, const char *fill)
{
	int r = 0;
	int g = 0;
	int b = 0;
	int rr;
	int gg;
	int bb;
	DATA32 *data;
	DATA32 *pixel;

	imlib_context_set_image(image);

	if((data = imlib_image_get_data()) == NULL) {
		fprintf(stderr, "could not get image data for colourise\n");
	}

	pixel = data + imlib_image_get_height() * imlib_image_get_width() - 1;
	do {
		rr = __IMAGE_COLOURISE_8BIT( ( ( *pixel >> 16 ) & 0xff ) + r);
		gg = __IMAGE_COLOURISE_8BIT( ( ( *pixel >> 8 ) & 0xff ) + b);
		bb = __IMAGE_COLOURISE_8BIT( ( *pixel & 0xff ) + g );
		*pixel = ( *pixel & 0xff000000 ) + ( rr << 16 ) + 
			( bb << 8 ) + gg;
	} while (pixel-- > data);

	imlib_image_put_back_data(data);

	return(image);
}


int calculate_cache(const char *cache) 
{
	char *end;
	long int l;

	if(cache == NULL) {
		return(DEFAULT_CACHE_SIZE);
	}

	l = strtol(cache, &end, 10);
	if (l < 0 || l > INT_MAX) {
		return (-1);
	}

	return((int) l);
}


static int set_cache(const char *cache) {
	int size;

	if((size = calculate_cache(cache)) < 0) {
		fprintf(stderr, "Could not determine cache size");
		return(-1);
	}

	imlib_set_cache_size(size * 1024 * 1024);

	return(0);
}


static int set_antialiasing(const int antialiasing) {
	imlib_context_set_anti_alias((char)(antialiasing?0:1));

	return(0);
}


int main(int argc, char **argv)
{
	options_t *opt;
	Imlib_Image image;
	char *format;

	opt = options(argc, argv);
	if (opt == NULL) {
		fprintf(stderr, "Error processing options\n");
		exit(-1);
	}

	/* Set Antialiasing */
	if (set_antialiasing(opt->bin_ops&__OP_NOANTIALIAS) < 0) {
		fprintf(stderr, "Could not set antialiasing\n");
		return (-1);
	}

	/* Set Cache Size*/
	if (set_cache(opt->cache) < 0) {
		fprintf(stderr, "Could not set cache size\n");
		return (-1);
	}

	/* Load first image */
	image = imlib_load_image(opt->src);
	if (image == NULL) {
		fprintf(stderr, "Could not load %s\n", opt->src);
		return (-1);
	}
	imlib_context_set_image(image);

	/* Set format for output */
	format = strrchr(opt->dest, '.');
	if (format)
		imlib_image_set_format(format + 1);

	/* Average Image */
	image = image_average(image, opt->bin_ops&__OP_AVERAGE,
			opt->files);
	if (image == NULL) {
		fprintf(stderr, "Averaging failed\n");
		return (-1);
	}

	/* Blur Image */
	if ((image = image_blur(image, opt->blur)) == NULL) {
		fprintf(stderr, "Bluring failed\n");
		return (-1);
	}

	/* Crop Image */
	image = image_crop(image, opt->crop, opt->dest, opt->bordercolour);
	if (image == NULL) {
		fprintf(stderr, "Cropping failed\n");
		return (-1);
	}

	/* Channel Image */
	image = image_channel(image, opt->channel);
	if (image == NULL) {
		fprintf(stderr, "Channel selection failed\n");
		return (-1);
	}

	/* Scale Image */
	if ((image = image_scale(image, opt->scale)) == NULL) {
		fprintf(stderr, "Scaling failed\n");
		return (-1);
	}

	/* Put a Border on the Image */
	image = image_border(image, opt->border, opt->bordercolour);
	if (image == NULL) {
		fprintf(stderr, "Border failed\n");
		return (-1);
	}

	/* Flip the Image Vertically */
	image = image_flip(image, opt->bin_ops&__OP_FLIP);
	if (image == NULL) {
		fprintf(stderr, "Flip failed\n");
		return (-1);
	}

	/* Flip the Image Horizontally */
	image = image_flop(image, opt->bin_ops&__OP_FLOP);
	if (image == NULL) {
		fprintf(stderr, "Flop failed\n");
		return (-1);
	}

	/* Modify the Gamma of the Image*/
	image = image_gamma(image, opt->gamma);
	if (image == NULL) {
		fprintf(stderr, "Gamma correction failed\n");
		return (-1);
	}

	/* Rotate the Image */
	image = image_rotate(image, opt->rotate, opt->bordercolour);
	if (image == NULL) {
		fprintf(stderr, "Rotation failed\n");
		return (-1);
	}

	/* Convert image to monochrome */
	image = image_monochrome(image, opt->bin_ops&__OP_MONOCHROME, 
			opt->bin_ops&__OP_ORDERED_DITHER);
	if(image == NULL) {
		fprintf(stderr, "Could not convert image to monochrome\n");
		return(-1);
	}
#if 0 /* this just makes the image red.. well now it doesnt.. */
	image = image_colourise(image, NULL, NULL);
#endif

	imlib_image_attach_data_value("quality", NULL, opt->quality, NULL);
	imlib_image_attach_data_value("compression", NULL, opt->compression, 
				      NULL);
	/* save the image */
	imlib_save_image(opt->dest);

	return (0);
}
