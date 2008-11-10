/*****************************************************************************/
/* VA Linux Systems Flipbook demo                                            */
/*****************************************************************************/
/*
 * Copyright (C) 2000 Brad Grantham, Geoff Harrison, and VA Linux Systems
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
#include "hooks.h"

#include <gdk/gdkprivate.h>
#include <gdk/gdkx.h>

#include <sys/ipc.h>
#include <sys/shm.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/Xos.h>
#include <X11/extensions/XShm.h>

#define INDEX_RGB(r,g,b)  fast_rgb[(r<<10)|(g<<5)|(b)]
#define ERROR_RED(rr,i)   rr-id->palette[i].r;
#define ERROR_GRN(gg,i)   gg-id->palette[i].g;
#define ERROR_BLU(bb,i)   bb-id->palette[i].b;

#define DITHER_ERROR(Der1,Der2,Dex,Der,Deg,Deb) \
ter=&(Der1[Dex]);\
(*ter)+=(Der*7)>>4;ter++;\
(*ter)+=(Deg*7)>>4;ter++;\
(*ter)+=(Deb*7)>>4;\
ter=&(Der2[Dex-6]);\
(*ter)+=(Der*3)>>4;ter++;\
(*ter)+=(Deg*3)>>4;ter++;\
(*ter)+=(Deb*3)>>4;ter++;\
(*ter)+=(Der*5)>>4;ter++;\
(*ter)+=(Deg*5)>>4;ter++;\
(*ter)+=(Deb*5)>>4;ter++;\
(*ter)+=Der>>4;ter++;\
(*ter)+=Deg>>4;ter++;\
(*ter)+=Deb>>4;

#define COLOR_INDEX(i)    id->palette[i].pixel

void get_jpeg_stats(char *path)
{

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr my_error;
	FILE *disk_image;

	set_width(0);
	set_height(0);
	if ((disk_image = fopen(path, "r"))) {
		cinfo.err = jpeg_std_error(&my_error);
		jpeg_create_decompress(&cinfo);
		jpeg_stdio_src(&cinfo, disk_image);
		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);

		set_width(cinfo.output_width);
		set_height(cinfo.output_height);

		jpeg_destroy_decompress(&cinfo);
		fclose(disk_image);
	}

	return;
}

unsigned char *load_jpeg_from_disk(char *path)
{

	struct jpeg_decompress_struct cinfo;
	struct jpeg_error_mgr my_error;
	FILE *disk_image;
	unsigned char *data = NULL, *line[16], *ptr = NULL;
	int i;
	unsigned int x, y;

	if ((disk_image = fopen(path, "r"))) {
		cinfo.err = jpeg_std_error(&my_error);
		jpeg_create_decompress(&cinfo);
		jpeg_stdio_src(&cinfo, disk_image);
		cinfo.do_fancy_upsampling = FALSE;
		cinfo.do_block_smoothing = FALSE;
		jpeg_read_header(&cinfo, TRUE);
		jpeg_start_decompress(&cinfo);
		data = malloc(cinfo.output_width * cinfo.output_height * 3);
		ptr = data;
		if (data) {
			if (cinfo.output_components == 3) {
				for (y = 0; y < cinfo.output_height;
						y += cinfo.rec_outbuf_height) {
					for (i = 0; i < cinfo.rec_outbuf_height; i++) {
						line[i] = ptr;
						ptr += cinfo.output_width * 3;
					}
					jpeg_read_scanlines(&cinfo, line, cinfo.rec_outbuf_height);
				}
			} else if (cinfo.output_components == 1) {
				for (i = 0; i < cinfo.rec_outbuf_height; i++) {
					if ((line[i] = malloc(cinfo.output_width)) == NULL) {
						int t = 0;

						for (t = 0; t < i; t++)
							free(line[t]);
						jpeg_destroy_decompress(&cinfo);
						return NULL;
					}
				}
				for (y = 0; y < cinfo.output_width;
						y += cinfo.rec_outbuf_height) {
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

		} else {
			printf("hmm.  this sucked\n");
		}

		jpeg_destroy_decompress(&cinfo);

		fclose(disk_image);
	}

	return data;

}

GdkPixmap *PixmapFromData(unsigned char *data)
{

	Display            *disp;
	int screen;
	int depth;
	Window root;
	Window              base_window;

	Visual *visual;
	XImage *xim;
	XImage             *last_xim;

	GC tgc;
	XGCValues gcv;
	unsigned char *tmp, **yarray, *ptr22;
	int *error, *er1, *er2, *xarray, bpp;
	Pixmap pmap;
	int w, h;
	XShmSegmentInfo     last_shminfo;
	GdkPixmap *pixmap;
	XSetWindowAttributes at;
	unsigned long       mask;
	GdkWindowPrivate   *private;
	int ex,/*inc,pos,*/x,y,w3;
	/*int *ter; */
	unsigned char *ptr2;

	at.border_pixel = 0;
	at.backing_store = NotUseful;
	at.background_pixel = 0;
	at.save_under = False;
	at.override_redirect = True;
	mask = CWOverrideRedirect | CWBackPixel | CWBorderPixel |
		CWBackingStore | CWSaveUnder;

	private = (GdkWindowPrivate *) & gdk_root_parent;


	disp = (Display *) gdk_display;
	screen = DefaultScreen(disp);
	root = DefaultRootWindow(disp);
	visual = DefaultVisual(disp,screen);
	depth = DefaultDepth(disp,screen);
	base_window = XCreateWindow(disp, private->xwindow,
			-100, -100, 10, 10, 0, depth, InputOutput,
			visual, mask, &at);

	if (depth <= 8) {
		bpp = 1;
	} else if (depth <= 16) {
		bpp = 2;
	} else if (depth <= 24) {
		bpp = 3;
	} else {
		bpp = 4;
	}

	w = get_width();
	h = get_height();

	xarray = malloc(sizeof(int) * w);
	yarray = malloc(sizeof(unsigned char *) * h);

	error = (int *) malloc(sizeof(int) * (w + 2) * 2 * 3);
	er1 = error;
	er2 = error + ((w + 2) * 3);

	w3 = w * 3;

	ptr22 = data;


	if (!error) {
		fprintf(stderr, "ERROR: Cannot allocate RAM for image dither buffer\n");
		return 0;
	}

	for (ex = 0; ex < ((w + 2) * 3 * 2); ex++)
		error[ex] = 0;

	xim = XShmCreateImage(disp, visual, depth, ZPixmap, NULL, &last_shminfo,
			w, h);
	if (!xim) {
		fprintf(stderr, "flipbook ERROR: Mit-SHM can't create XImage for Shared Pixmap Wrapper\n");
		fprintf(stderr, "             Falling back on Shared XImages\n");
	}
	last_shminfo.shmid =
		shmget(IPC_PRIVATE, xim->bytes_per_line * xim->height,
				IPC_CREAT | 0777);
	if (last_shminfo.shmid == -1) {
		fprintf(stderr, "flipbook ERROR: SHM can't get SHM Identifier for Shared Pixmap Wrapper\n");
		fprintf(stderr,
				"             Falling back on Shared XImages\n");
		XDestroyImage(xim);
	}
	last_shminfo.shmaddr = xim->data = shmat(last_shminfo.shmid, 0, 0);
	if (xim->data == (char *) -1) {
		fprintf(stderr, "flipbook ERROR: SHM can't attach SHM Segment for Shared Pixmap Wrapper\n");
		fprintf(stderr, "             Falling back on Shared XImages\n");
		XDestroyImage(xim);
		shmctl(last_shminfo.shmid, IPC_RMID, 0);
	}
	last_shminfo.readOnly = False;
	XShmAttach(disp, &last_shminfo);
	tmp = (unsigned char *) xim->data;
	last_xim = xim;
	pmap = XShmCreatePixmap(disp, base_window, last_shminfo.shmaddr,
			&last_shminfo, w, h, depth);
	tgc =
		XCreateGC(disp, pmap, GCGraphicsExposures, &gcv);

	ptr2 = data;
	for(y=0; y < h ; y++) {
		for (x = 0; x < w; x++) {
			char mypixel[4];

			printf("test x=%d (%d) y=%d (%d)\n",x,w,y,h);
			mypixel[0] = *ptr2++;
			mypixel[1] = *ptr2++;
			mypixel[2] = *ptr2;
			mypixel[3] = 0;
			XPutPixel(xim, x, y, (unsigned long)mypixel);
		}
	}

	{

		GdkWindowPrivate *private;
		GdkWindowPrivate *window_private;

		gdk_threads_enter();
		private = g_new0(GdkWindowPrivate, 1);
		pixmap = (GdkPixmap *) private;

		window_private = (GdkWindowPrivate *) 
			gdk_window_foreign_new(base_window);
		gdk_threads_leave();

		private->xdisplay = window_private->xdisplay;
		private->window_type = GDK_WINDOW_PIXMAP;
		private->xwindow = pmap;
		private->colormap = gdk_colormap_get_system();
		private->children = NULL;
		private->parent = NULL;
		private->x = 0;
		private->y = 0;
		private->width = w;
		private->height = h;
		private->resize_count = 0;
		private->ref_count = 1;
		private->destroyed = 0;

		gdk_threads_enter();
		gdk_xid_table_insert(&private->xwindow, pixmap);
		gdk_threads_leave();

	}
	XFreeGC(disp, tgc);
	XSync(disp, False);
	XShmDetach(disp, &last_shminfo);
	XDestroyImage(xim);
	shmdt(last_shminfo.shmaddr);
	shmctl(last_shminfo.shmid, IPC_RMID, 0);

	/* cleanup */
	XSync(disp, False);
	free(error);
	free(xarray);
	free(yarray);

	return pixmap;

}
