#ifndef __ROTATE
#define __ROTATE 1

#include <X11/Xlib.h>
#include "image.h"
#include "colormod.h"
#include "blend.h"

/*\ Calc precision \*/
#define _ROTATE_PREC 12
#define _ROTATE_PREC_MAX (1 << _ROTATE_PREC)
#define _ROTATE_PREC_BITS (_ROTATE_PREC_MAX - 1)

void __imlib_RotateSample(DATA32 *src, DATA32 *dest, int sow, int sw, int sh,
			  int dow, int dw, int dh, int x, int y, int dx, int dy);
void __imlib_RotateAA(DATA32 *src, DATA32 *dest, int sow, int sw, int sh,
		      int dow, int dw, int dh, int x, int y, int dx, int dy);
DATA32 * __imlib_AddTransBorders(ImlibImage *im_old,
				 int ssx, int ssy, int ssw, int ssh);
void __imlib_BlendImageToImageAtAngle(ImlibImage *im_src, ImlibImage *im_dst,
				      char aa, char blend, char merge_alpha,
				      int ssx, int ssy, int ssw, int ssh,
				      int ddx, int ddy, int ddw, int ddh,
				      ImlibColorModifier *cm, ImlibOp op);


#endif
