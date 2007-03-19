/**********************************************************************
 * dither.c                                               November 2001
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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <X11/Xlib.h>
#include <Imlib2.h>

#include "dither.h"

/* Bayer 2x2 Dither Matrix */
static int d2[2][2] = {
	{ 0, 2 },
	{ 3, 1 }
};


#define __INTENSITY(_pixel) \
	( ( (((_pixel) >> 16) & 0xff) + (((_pixel) >> 8) & 0xff) + \
		((_pixel) & 0xff) ) /3 )

#define __M_GET(_matrix, _d, _x, _y) \
	*((int *)_matrix+((_x)+((_d)*(_y))))

#define __M_SET(_matrix, _d, _x, _y, _v) \
	*((int *)_matrix+((_x)+((_d)*(_y)))) = (_v);

static int **dither_ordered_next_matrix(int d, int **m) {
	int **mm;
	int x;
	int y;
	int xx;
	int yy;
	int val;

	if((mm=(int **)malloc(d*d*4*sizeof(int))) == NULL) {
		return(NULL);
	}

	for(yy = 0; yy < 2 ; yy++) {
		for(xx = 0; xx < 2 ; xx++) {
			for(y = 0; y < d ; y++) {
				for(x = 0; x < d ; x++) {
					val = 4 * __M_GET(m, d, x, y);
					val += __M_GET(d2, 2, xx, yy);
					__M_SET(mm, d*2, xx*d+x, yy*d+y, val);
				}
			}
		}
	}

	return(mm);
}

static int **dither_ordered_matrix(int dimension) 
{
	int d;
	int **m;
	int **mm = NULL;

	/* Need to check that dimension is sane */

	if(dimension == 2) {
		return ((int **)d2);
	}
	
	if((m = dither_ordered_next_matrix(2, (int **)d2)) == NULL) {
		return(NULL);
	}

	if(dimension == 4) {
		return(m);
	}

	d = 4;
	while(1) {
		mm = dither_ordered_next_matrix(d, m);
		d *= 2;
		free(m);
		if(mm == NULL) {
			return(NULL);
		}
		if(d >= dimension) {
			return(mm);
		}
		m = mm;
	}

	return(NULL);
}


Imlib_Image *dither_ordered_image(Imlib_Image image, int dimension) 
{
	int w;
	int h;
	int x;
	int y;
	int i;
	int j;
	int levels;
	int **matrix;
	DATA32 *data;
	DATA32 *pixel;

	/* What is the Matrix? */
	matrix = dither_ordered_matrix(dimension);
	if(matrix == NULL) {
		/* There is no spoon */
		fprintf(stderr, "Invalid dimension for dither matrix\n");
		return(NULL);
	}

	imlib_context_set_image(image);

	if((data=imlib_image_get_data()) == NULL) {
                fprintf(stderr, "Could not get image data for dither\n");
                return(NULL);
        }

	w = imlib_image_get_width();
	h = imlib_image_get_height();
	levels = dimension*dimension + 1;

	pixel = data;
	for(y = 0 ; y < h ; y++) {
		for(x = 0 ; x < w ; x++) {
			i = x%dimension;
			j = y%dimension;
			if(((__INTENSITY(*pixel) * levels) >> 8)  > 
					__M_GET(matrix, dimension, i, j)) {
				*pixel=0xffffffff;
			}
			else {
				*pixel=0xff000000;
			}
			pixel++;
		}
	}

	imlib_image_put_back_data(data);

	return(image);
}


Imlib_Image *dither_floyd_image(Imlib_Image image, int flag) 
{
	int w;
	int h;
	int x;
	int y;
	int error;
	int this_error;
	int cumulative_error;
	DATA32 *data;
	DATA32 *pixel;
	int *diffusion;
	int *pixel_d;
	int dir;

	imlib_context_set_image(image);

	if((data=imlib_image_get_data()) == NULL) {
                fprintf(stderr, "Could not get image data for dither\n");
                return(NULL);
        }

	w = imlib_image_get_width();
	h = imlib_image_get_height();

	if( (diffusion = (int *)malloc(w * h * sizeof(int))) == NULL) {
		fprintf(stderr, "Could not allocate memory for diffusion\n");
		return(NULL);
	}
	memset(diffusion, 0, w * h * sizeof(int));

	dir = 1;
	pixel = data;
	pixel_d = diffusion;
	for(y = 0 ; y < h ; y++) {
		for(x = 0 ; x < w ; x++) {
			cumulative_error = 0;
			error = __INTENSITY(*pixel) + *pixel_d;
			if(error > 255) {
				*pixel = 0xffffffff;
				error -= 255;
			}
			else if(error > 127) {
				*pixel = 0xffffffff;
				error = - (255 - error);
			}
			else {
				*pixel = 0xff000000;
			}

			this_error = (7 * error) >> 4;
			cumulative_error += this_error;
			if(x < w-1) {
				*(pixel_d+dir) += this_error;
			}

			if(y < h-1) {
				this_error = (3 * error) >> 4;
				cumulative_error += this_error;
				if(x) {
					*(pixel_d+w-dir) += this_error;
				}
				
				this_error = (5 * error) >> 4;
				cumulative_error += this_error;
				*(pixel_d+w) += this_error;

				this_error = (error - cumulative_error) >> 4;
				if(x < w-1) {
					*(pixel_d+w+dir) += this_error;
				}
			}
			pixel+=dir;
			pixel_d+=dir;
		}
		if(flag & DITHER_FLOYD_ALTERNATE) {
			dir = -dir;
			pixel += w;
			pixel_d += w;
		}
	}

	free(diffusion);
	imlib_image_put_back_data(data);

	return(image);
}
