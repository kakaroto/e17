/*****************************************************************************/
/* VA Linux Systems Flipbook demo                                            */
/*****************************************************************************/
/*
 * Copyright (C) 1999 Brad Grantham, Geoff Harrison, and VA Linux Systems
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include <stdlib.h>
#include <stdio.h>
#include <jpeglib.h>
#include "loader.h"

unsigned char *load_jpeg_from_disk(char *path) {

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr my_error;
	FILE *disk_image;
	unsigned char *data = NULL, *line[16], *ptr = NULL;
	int i;
	unsigned int x, y;


	if((disk_image = fopen(path,"r"))) {
		cinfo.err = jpeg_std_error(&my_error);
		jpeg_create_decompress(&cinfo);
		jpeg_stdio_src(&cinfo, disk_image);
		cinfo.do_fancy_upsampling = FALSE;
		cinfo.do_block_smoothing = FALSE;
		jpeg_start_decompress(&cinfo);
		data = malloc(cinfo.output_width * cinfo.output_height * 3);
		ptr = data;
		if(data) {
			if (cinfo.output_components == 3) {
				for (y = 0; y < cinfo.output_height; y += cinfo.rec_outbuf_height) {
					for (i = 0; i < cinfo.rec_outbuf_height; i++) {
						line[i] = ptr;
						ptr += cinfo.output_width * 3;
					}
					jpeg_read_scanlines(&cinfo, line, cinfo.rec_outbuf_height);
				}
			} else if (cinfo.output_components == 1) {
				for (i = 0; i < cinfo.rec_outbuf_height; i++) {
					if ((line[i] = malloc(cinfo.output_width)) == NULL) {
						int                 t = 0;

						for (t = 0; t < i; t++)
							free(line[t]);
						jpeg_destroy_decompress(&cinfo);
						return NULL;
					}
				}
				for (y = 0; y < cinfo.output_width; y += cinfo.rec_outbuf_height) {
					jpeg_read_scanlines(&cinfo, line, cinfo.rec_outbuf_height);
					for (i = 0; i < cinfo.rec_outbuf_height; i++) {
						for (x = 0; x < cinfo.output_width; x++) {
							*ptr++ = line[i][x];
							*ptr++ = line[i][x];
							*ptr++ = line[i][x];
						}
					}
				}
				for (i = 0; i < cinfo.rec_outbuf_height; i++)
					free(line[i]);
			}
			jpeg_finish_decompress(&cinfo);

		}
		jpeg_destroy_decompress(&cinfo);

		fclose(disk_image);
	}

	return data;

}
