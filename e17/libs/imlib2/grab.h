#ifndef __GRAB
#define __GRAB 1
DATA32 *
GrabDrawableToRGBA(Display *d, Drawable p, Pixmap m, Visual *v, Colormap cm, 
		   int depth, int x, int y, int w, int h, char domask);
#endif
