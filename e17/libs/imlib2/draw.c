#include <X11/Xlib.h>
#include <X11/extensions/XShm.h>
#include "common.h"
#include "image.h"
#include "blend.h"
#include "rend.h"
#include "draw.h"

char
__imlib_CreatePixmapsForImage(Display *d, Drawable w, Visual *v, int depth, 
			      Colormap cm, ImlibImage *im, Pixmap *p, Mask *m,
			      int sx, int sy, int sw, int sh,
			      int dw, int dh,
			      char anitalias, char hiq, char dither_mask,
			      ImlibColorModifier *cmod)
{
   ImlibImagePixmap *ip = NULL;
   Pixmap pmap = 0;
   Pixmap mask = 0;
   long long mod_count = 0;
   
   if (cmod)
      mod_count = cmod->modification_count;
   ip = __imlib_FindCachedImagePixmap(im, dw, dh, d, v, depth, sx, sy, 
				      sw, sh, cm, anitalias, hiq, dither_mask,
				      mod_count);
   if (ip)
     {
	if (p)
	   *p = ip->pixmap;
	if (m)
	   *m = ip->mask;
	return 2;
     }
   if (p)
     {
	pmap = XCreatePixmap(d, w, dw, dh, depth);
	*p = pmap;
     }
   if (m)
     {
	if (IMAGE_HAS_ALPHA(im))
	   mask = XCreatePixmap(d, w, dw, dh, 1);
	*m = mask;
     }
   __imlib_RenderImage(d, im, pmap, mask, v, cm, depth, sx, sy, sw, sh, 0, 0, 
		       dw, dh, anitalias, hiq, 0, dither_mask, cmod, 
		       OP_COPY);
   return 1;
}

