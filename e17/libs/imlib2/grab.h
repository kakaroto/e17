#ifndef __GRAB
#define __GRAB 1
char
__imlib_GrabDrawableToRGBA(DATA32 *data, int ox, int oy, int ow, int oh,
			   Display *d, Drawable p, Pixmap m, Visual *v,
			   Colormap cm, int depth, int x, int y,
			   int w, int h, char domask, char grab);
#endif
