#include "common.h"
#include "rotate.h"
#include "blend.h"

/*\ I have no idea which of these two is faster..
|*| The first one doesn't branch, the second one doesn't multiply..
|*| Tests are inconclusive so far..
|*| Maybe some kind of table lookup would be quicker ?? 
\*/
#if 1
# define RENORM_X_Y_SRC \
src += (x >> _ROTATE_PREC) + ((y >> _ROTATE_PREC) * sow); \
x &= _ROTATE_PREC_BITS; y &= _ROTATE_PREC_BITS;
#else
# define RENORM_X_Y_SRC \
while (x > _ROTATE_PREC_BITS) { \
x -= _ROTATE_PREC_BITS; \
src++;   \
}    \
while (x < 0) { \
x += _ROTATE_PREC_BITS; \
src--;   \
}    \
while (y > _ROTATE_PREC_BITS) { \
y -= _ROTATE_PREC_BITS; \
src += sow;  \
}    \
while (y < 0) { \
y += _ROTATE_PREC_BITS; \
src -= sow;  \
}
#endif

/*\ Rotate by pixel sampling only, target inside source \*/
void
__imlib_RotateSampleInside(DATA32 *src, DATA32 *dest, int sow, int dow,
			   int dw, int dh, int x, int y, int dx, int dy)
{
   int i;
   
   if ((dw < 1) || (dh < 1)) return;
   
   i = 0;
   src += (x >> _ROTATE_PREC) + ((y >> _ROTATE_PREC) * sow);
   x &= _ROTATE_PREC_BITS; y &= _ROTATE_PREC_BITS;
   while (1) {
      do {
	 *dest = *src;
	 /*\ RIGHT; \*/
	 x += dx;
	 y += dy;
	 RENORM_X_Y_SRC;
	 dest++;
	 i++;
      } while (i < dw);
      dh--;
      if (dh <= 0) break;
      /*\ DOWN; \*/
      x -= dy;
      y += dx;
      RENORM_X_Y_SRC;
      dest += dow;
      do {
	 /*\ LEFT; \*/
	 x -= dx;
	 y -= dy;
	 RENORM_X_Y_SRC;
	 dest--;
	 *dest = *src;
	 i--;
      } while (i > 0);
      dh--;
      if (dh <= 0) break;
      /*\ DOWN; \*/
      x -= dy;
      y += dx;
      RENORM_X_Y_SRC;
      dest += dow;
   }
}

/*\ Testing shows this version to be 10% (!!!) faster (overall speed)
|*| So I think we can live with its ugliness..
|*| It's still a bottleneck, with it's 24 MULs per call..
\*/

/*\ bigendian and littleendian byte-from-int macro's \*/
#if __BYTE_ORDER == __LITTLE_ENDIAN
# define R_VAL(x) (*(((DATA8 *)&(x))+(0)))
# define G_VAL(x) (*(((DATA8 *)&(x))+(1)))
# define B_VAL(x) (*(((DATA8 *)&(x))+(2)))
# define A_VAL(x) (*(((DATA8 *)&(x))+(3)))
#elif __BYTE_ORDER == __BIG_ENDIAN
# define A_VAL(x) (*(((DATA8 *)&(x))+(0)))
# define B_VAL(x) (*(((DATA8 *)&(x))+(1)))
# define G_VAL(x) (*(((DATA8 *)&(x))+(2)))
# define R_VAL(x) (*(((DATA8 *)&(x))+(3)))
#elif __BYTE_ORDER == __PDP_ENDIAN
# define B_VAL(x) (*(((DATA8 *)&(x))+(0)))
# define A_VAL(x) (*(((DATA8 *)&(x))+(1)))
# define R_VAL(x) (*(((DATA8 *)&(x))+(2)))
# define G_VAL(x) (*(((DATA8 *)&(x))+(3)))
#else
#error Unknown byte endianness.
#endif
static inline DATA32
Interp_ARGB(DATA32 ul, DATA32 ur, DATA32 ll, DATA32 lr, int x, int y)
{
   DATA32 rv;
   
   R_VAL(rv) = ((((R_VAL(ul)) * (_ROTATE_PREC_MAX - x) +
		  (R_VAL(ur)) * x) * (_ROTATE_PREC_MAX - y) +
		 ((R_VAL(ll)) * (_ROTATE_PREC_MAX - x) +
		  (R_VAL(lr)) * x) * y) >> (2 * _ROTATE_PREC));
   
   G_VAL(rv) = ((((G_VAL(ul)) * (_ROTATE_PREC_MAX - x) +
		  (G_VAL(ur)) * x) * (_ROTATE_PREC_MAX - y) +
		 ((G_VAL(ll)) * (_ROTATE_PREC_MAX - x) +
		  (G_VAL(lr)) * x) * y) >> (2 * _ROTATE_PREC));
   
   B_VAL(rv) = ((((B_VAL(ul)) * (_ROTATE_PREC_MAX - x) +
		  (B_VAL(ur)) * x) * (_ROTATE_PREC_MAX - y) +
		 ((B_VAL(ll)) * (_ROTATE_PREC_MAX - x) +
		  (B_VAL(lr)) * x) * y) >> (2 * _ROTATE_PREC));
   
   A_VAL(rv) = ((((A_VAL(ul)) * (_ROTATE_PREC_MAX - x) +
		  (A_VAL(ur)) * x) * (_ROTATE_PREC_MAX - y) +
		 ((A_VAL(ll)) * (_ROTATE_PREC_MAX - x) +
		  (A_VAL(lr)) * x) * y) >> (2 * _ROTATE_PREC));
   
   return rv;
}

/*\ Same as last function, but with antialiasing \*/
void
__imlib_RotateAAInside(DATA32 *src, DATA32 *dest, int sow, int dow,
		       int dw, int dh, int x, int y, int dx, int dy)
{
   int i;
   
   if ((dw < 1) || (dh < 1)) return;
   
   i = 0;
   src += (x >> _ROTATE_PREC) + ((y >> _ROTATE_PREC) * sow);
   x &= _ROTATE_PREC_BITS; y &= _ROTATE_PREC_BITS;
   while (1) {
      do {
	 *dest = Interp_ARGB(src[0], src[1],
			     src[sow], src[sow + 1], x, y);
	 /*\ RIGHT; \*/
	 x += dx;
	 y += dy;
	 RENORM_X_Y_SRC;
	 dest++;
	 i++;
      } while (i < dw);
      dh--;
      if (dh <= 0) break;
      /*\ DOWN; \*/
      x -= dy;
      y += dx;
      RENORM_X_Y_SRC;
      dest += dow;
      do {
	 /*\ LEFT; \*/
	 x -= dx;
	 y -= dy;
	 RENORM_X_Y_SRC;
	 --dest;
	 *dest = Interp_ARGB(src[0], src[1],
			     src[sow], src[sow + 1], x, y);
	 --i;
      } while (i > 0);
      dh--;
      if (dh <= 0) break;
      /*\ DOWN; \*/
      x -= dy;
      y += dx;
      RENORM_X_Y_SRC;
      dest += dow;
   }
}

static int
__check_inside_coords(int x, int y, int dx, int dy,
		      int dw, int dh, int sow, int soh)
{
   sow <<= _ROTATE_PREC;
   soh <<= _ROTATE_PREC;
   
   if ((x < 0) || (y < 0) || (x >= sow) || (y >= soh))
      return 0;
   x += dx * dw; y += dy * dw;
   if ((x < 0) || (y < 0) || (x >= sow) || (y >= soh))
      return 0;
   x -= dy * dh; y += dx * dh;
   if ((x < 0) || (y < 0) || (x >= sow) || (y >= soh))
      return 0;
   x -= dx * dw; y -= dy * dw;
   if ((x < 0) || (y < 0) || (x >= sow) || (y >= soh))
      return 0;
   
   return 1;
}

/*\ These ones don't need the target to be inside the source \*/
void
__imlib_RotateSample(DATA32 *src, DATA32 *dest, int sow, int sw, int sh,
		     int dow, int dw, int dh, int x, int y, int dx, int dy)
{
   int i;
   DATA32 *st, *sb;
   int xp; /*\ Check if we're inside the source \*/
   
   if ((dw < 1) || (dh < 1)) return;
   
   if (__check_inside_coords(x, y, dx, dy, dw, dh, sw, sh)) {
      __imlib_RotateSampleInside(src, dest, sow, dow,
				 dw, dh, x, y, dx, dy);
      return;
      
   }
   
   i = 0;
   st = src; sb = src + sh * sow;
   xp = x >> _ROTATE_PREC;
   src += (x >> _ROTATE_PREC) + ((y >> _ROTATE_PREC) * sow);
   x &= _ROTATE_PREC_BITS; y &= _ROTATE_PREC_BITS;
   while (1) {
      do {
	 if ((xp >= 0) && (xp < sw) &&
	     (src >= st) && (src < sb))
	    *dest = *src;
	 /*\ RIGHT; \*/
	 x += dx;
	 y += dy;
	 xp += (x >> _ROTATE_PREC);
	 RENORM_X_Y_SRC;
	 dest++;
	 i++;
	 
      } while (i < dw);
      dh--;
      if (dh <= 0) break;
      /*\ DOWN; \*/
      x -= dy;
      y += dx;
      xp += (x >> _ROTATE_PREC);
      RENORM_X_Y_SRC;
      dest += dow;
      do {
	 /*\ LEFT; \*/
	 x -= dx;
	 y -= dy;
	 xp += (x >> _ROTATE_PREC);
	 RENORM_X_Y_SRC;
	 --dest;
	 if ((xp >= 0) && (xp < sw) &&
	     (src >= st) && (src < sb))
	    *dest = *src;
	 --i;
	 
      } while (i > 0);
      dh--;
      if (dh <= 0) break;
      /*\ DOWN; \*/
      x -= dy;
      y += dx;
      xp += (x >> _ROTATE_PREC);
      RENORM_X_Y_SRC;
      dest += dow;
      
   }
}

/*\ With antialiasing \*/
void
__imlib_RotateAA(DATA32 *src, DATA32 *dest, int sow, int sw, int sh,
		 int dow, int dw, int dh, int x, int y, int dx, int dy)
{
   int i;
   DATA32 *st, *sb;
   int xp; /*\ Check if we're inside the source \*/
   
   if ((dw < 1) || (dh < 1)) return;
   
   if (__check_inside_coords(x, y, dx, dy, dw, dh, sw - 1, sh - 1)) {
      __imlib_RotateAAInside(src, dest, sow, dow,
			     dw, dh, x, y, dx, dy);
      return;
      
   }
   
   i = 0;
   st = src; sb = src + (sh - 1) * sow;
   xp = x >> _ROTATE_PREC;
   src += (x >> _ROTATE_PREC) + ((y >> _ROTATE_PREC) * sow);
   x &= _ROTATE_PREC_BITS; y &= _ROTATE_PREC_BITS;
   while (1) {
      do {
	 if ((xp >= 0) && (xp < (sw - 1)) &&
	     (src >= st) && (src < sb))
	    *dest = Interp_ARGB(src[0], src[1],
				src[sow], src[sow + 1], x, y);
	 /*\ RIGHT; \*/
	 x += dx;
	 y += dy;
	 xp += (x >> _ROTATE_PREC);
	 RENORM_X_Y_SRC;
	 dest++;
	 i++;
	 
      } while (i < dw);
      dh--;
      if (dh <= 0) break;
      /*\ DOWN; \*/
      x -= dy;
      y += dx;
      xp += (x >> _ROTATE_PREC);
      RENORM_X_Y_SRC;
      dest += dow;
      do {
	 /*\ LEFT; \*/
	 x -= dx;
	 y -= dy;
	 xp += (x >> _ROTATE_PREC);
	 RENORM_X_Y_SRC;
	 --dest;
	 if ((xp >= 0) && (xp < (sw - 1)) &&
	     (src >= st) && (src < sb))
	    *dest = Interp_ARGB(src[0], src[1],
				src[sow], src[sow + 1], x, y);
	 --i;	 
      } while (i > 0);
      dh--;
      if (dh <= 0) break;
      /*\ DOWN; \*/
      x -= dy;
      y += dx;
      xp += (x >> _ROTATE_PREC);
      RENORM_X_Y_SRC;
      dest += dow;      
   }   
}

/*\ Helper: Expand an image in all directions with transparent borders
 |*|  Creates a new image, no caching (TODO)
\*/
DATA32 *
__imlib_AddTransBorders(ImlibImage *im, int ssx, int ssy, int ssw, int ssh)
{
   DATA32 *data;
   int i, w, h;
   DATA32 *p;
   
   if ((ssx < 0) || (ssy < 0) || (ssw < 0) || (ssh < 0) ||
       ((ssw + ssx) > im->w) || ((ssh + ssy) > im->h))
      return NULL;
   data = malloc((ssw + 2) * (ssh + 2) * sizeof(DATA32));
   
   for (i = ssh; --i >= 0; ) {
      memcpy(data + 1 + ((1 + i) * (ssw + 2)),
	     im->data + ssx + ((i + ssy) * im->w),
	     ssw * sizeof(DATA32));
      
   }
   
   p = data + 1;
   for (i = ssw; --i >= 0; ) {
      p[0] = p[(ssw + 2)];
      A_VAL(p[0]) = 0;
      p++;
      
   }
   p = data + 1 + (((ssh + 2) - 1) * (ssw + 2));
   for (i = ssw; --i >= 0; ) {
      p[0] = p[-(ssw + 2)];
      A_VAL(p[0]) = 0;
      p++;
      
   }
   p = data;
   for (i = ssh + 2; --i >= 0; ) {
      p[0] = p[1];
      A_VAL(p[0]) = 0;
      p += (ssw + 2);
      
   }
   p = data + ssw + 1;
   for (i = ssh + 2; --i >= 0; ) {
      p[0] = p[-1];
      A_VAL(p[0]) = 0;
      p += (ssw + 2);
      
   }
   return data;
   
}

/*\ Should this be in blend.c ?? \*/
#define LINESIZE 16
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
void
__imlib_BlendImageToImageAtAngle(ImlibImage *im_src, ImlibImage *im_dst,
				 char aa, char blend, char merge_alpha,
				 int ssx, int ssy, int ssw, int ssh,
				 int ddx1, int ddy1, int ddx2, int ddy2,
				 ImlibColorModifier *cm, ImlibOp op)
{
   int ddw, ddh, x, y, dx, dy, i;
   double xy2;
   DATA32 *data, *src;
   
   if ((ssw < 0) || (ssh < 0))
      return;
   
   if ((!(im_src->data)) && (im_src->loader) && (im_src->loader->load))
      im_src->loader->load(im_src, NULL, 0, 1);
   if (!im_src->data)
      return;
   if ((!(im_dst->data)) && (im_dst->loader) && (im_src->loader->load))
      im_dst->loader->load(im_dst, NULL, 0, 1);
   if (!im_dst->data)
      return;
   
   ddw = ddx2 - ddx1;
   ddh = ddy2 - ddy1;
   if ((ssw == ddw) & (ssh == ddh)) {
      if (!IMAGE_HAS_ALPHA(im_src))
	 blend = 0;
      if (!IMAGE_HAS_ALPHA(im_dst))
	 merge_alpha = 0;
      
      __imlib_BlendRGBAToData(im_src->data, im_src->w, im_src->h,
			      im_dst->data, im_dst->w, im_dst->h,
			      ssx, ssy, ddx1, ddy1,
			      ssw, ssh, blend, merge_alpha, cm, op);
      return;
      
   }
   /*\ Complicated gonio.  Works on paper..
   |*| Too bad it doesn't all fit into integer math.. 
   \*/
   xy2 = (double)(ddh * ddh + ddw * ddw);
   if (xy2 == 0) return;
   dx = (((double)((ssh * ddh) + (ssw * ddw)) * _ROTATE_PREC_MAX) / xy2);
   dy = (((double)((ssh * ddw) - (ssw * ddh)) * _ROTATE_PREC_MAX) / xy2);
   x = ddy1 * dy - ddx1 * dx;
   y = -ddx1 * dy - ddy1 * dx;
   
   if (ssx < 0) {
      x += ssx * _ROTATE_PREC_MAX;
      ssw += ssx;
      ssx = 0;
      
   }
   if (ssy < 0) {
      y += ssy * _ROTATE_PREC_MAX;
      ssh += ssy;
      ssy = 0;
      
   }
   if ((ssw + ssx) > im_src->w) ssw = im_src->w - ssx;
   if ((ssh + ssy) > im_src->h) ssh = im_src->h - ssy;
   
   src = im_src->data + ssx + ssy * im_src->w;
   if (aa) {
      src = __imlib_AddTransBorders(im_src, ssx, ssy, ssw, ssh);
      if (!src) return;
      x += _ROTATE_PREC_MAX;
      y += _ROTATE_PREC_MAX;
      
   }
   data = malloc(im_dst->w * LINESIZE * sizeof(DATA32));
   if (!data) {
      if (aa) free(src);
      return;
      
   }
   for (i = 0; i < im_dst->h; i += LINESIZE) {
      int x2, y2, w, h, l, r;
      
      h = MIN(LINESIZE, im_dst->h - i);
      
      x2 = x - h * dy;
      y2 = y + h * dx;
      
      w = ssw << _ROTATE_PREC;
      h = ssh << _ROTATE_PREC;
      /*\ Pretty similar code \*/
      if (dx > 0) {
	 if (dy > 0) {
	    l = MAX(-y2 / dy, -x / dx);
	    r = MIN((h - y) / dy, (w - x2) / dx);
	    
	 } else {
	    l = MAX(-x2 / dx, (h - y) / dy);
	    r = MIN(-y2 / dy, (w - x) / dx);
	    
	 }
	 
      } else {
	 if (dy > 0) {
	    l = MAX(-y / dy, (w - x2) / dx);
	    r = MIN(-x / dx, (h - y2) / dy);
	    
	 } else {
	    l = MAX((h - y2) / dy, (w - x) / dx);
	    r = MIN(-y / dy, -x2 / dx);
	    
	 }
	 
      }
      l--; r += 2; /*\ Be paranoid about roundoff errors \*/
      if (l < 0) l = 0;
      if (r > im_dst->w) r = im_dst->w;
      if (r <= l) {
	 x = x2; y = y2;
	 continue;
	 
      }
      
      w = r - l;
      h = MIN(LINESIZE, im_dst->h - i);
      memset(data, 0, h * w * sizeof(DATA32));
      x += l * dx;
      y += l * dy;
      if (aa) {
	 __imlib_RotateAA(src, data,
			  ssw + 2, ssw + 2, ssh + 2, w,
			  w, h, x, y, dx, dy);
	 
      } else {
	 __imlib_RotateSample(src, data, im_src->w,
			      ssw, ssh, w, w, h, x, y, dx, dy);
	 
      }
      __imlib_BlendRGBAToData(data, w, h, im_dst->data,
			      im_dst->w, im_dst->h, 0, 0, l, i, w, h,
			      blend, merge_alpha, cm, op);
      x = x2; y = y2;
      
   }
   if (aa) free(src);
   free(data);
}
