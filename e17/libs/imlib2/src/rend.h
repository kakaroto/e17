#ifndef __REND
#define __REND 1
void
__imlib_RenderImage(Display *d, ImlibImage *im,
		    Drawable w, Drawable m,
		    Visual *v, Colormap cm, int depth,
		    int sx, int sy, int sw, int sh,
		    int dx, int dy, int dw, int dh,
		    char anitalias, char hiq, char blend, char dither_mask,
		    ImlibColorModifier *cmod, ImlibOp op);
#endif
