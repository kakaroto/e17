
#include "common.h"
#include "rotate.h"

/*\ I have no idea which of these two is faster..
|*| The first one doesn't branch, the second one doesn't multiply..
|*| Tests are inconclusive so far..
|*| Maybe some kind of table lookup would be quicker ?? \*/
#if 1
#define RENORM_X_Y_SRC \
 src += (x >> _ROTATE_PREC) + ((y >> _ROTATE_PREC) * sow); \
 x &= _ROTATE_PREC_BITS; y &= _ROTATE_PREC_BITS;
#else
#define RENORM_X_Y_SRC \
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
#define R_VAL(x) (*(((DATA8 *)&(x))+(0)))
#define G_VAL(x) (*(((DATA8 *)&(x))+(1)))
#define B_VAL(x) (*(((DATA8 *)&(x))+(2)))
#define A_VAL(x) (*(((DATA8 *)&(x))+(3)))
#elif __BYTE_ORDER == __BIG_ENDIAN
#define A_VAL(x) (*(((DATA8 *)&(x))+(0)))
#define B_VAL(x) (*(((DATA8 *)&(x))+(1)))
#define G_VAL(x) (*(((DATA8 *)&(x))+(2)))
#define R_VAL(x) (*(((DATA8 *)&(x))+(3)))
#elif __BYTE_ORDER == __PDP_ENDIAN
#define B_VAL(x) (*(((DATA8 *)&(x))+(0)))
#define A_VAL(x) (*(((DATA8 *)&(x))+(1)))
#define R_VAL(x) (*(((DATA8 *)&(x))+(2)))
#define G_VAL(x) (*(((DATA8 *)&(x))+(3)))
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

/*\ These ones don't need the target to be inside the source \*/
void
__imlib_RotateSample(DATA32 *src, DATA32 *dest, int sow, int soh, int dow,
   int dw, int dh, int x, int y, int dx, int dy)
{
 int i;
 DATA32 *st, *sb;
 int xp; /*\ Check if we're inside the source \*/

 if ((dw < 1) || (dh < 1)) return;

 i = 0;
 st = src; sb = src + soh * sow;
 xp = x >> _ROTATE_PREC;
 src += (x >> _ROTATE_PREC) + ((y >> _ROTATE_PREC) * sow);
 x &= _ROTATE_PREC_BITS; y &= _ROTATE_PREC_BITS;
 while (1) {
  do {
   if ((xp >= 0) && (xp < sow) &&
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
   if ((xp >= 0) && (xp < sow) &&
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
__imlib_RotateAA(DATA32 *src, DATA32 *dest, int sow, int soh, int dow,
   int dw, int dh, int x, int y, int dx, int dy)
{
 int i;
 DATA32 *st, *sb;
 int xp; /*\ Check if we're inside the source \*/

 if ((dw < 1) || (dh < 1)) return;

 i = 0;
 st = src; sb = src + (soh - 1) * sow;
 xp = x >> _ROTATE_PREC;
 src += (x >> _ROTATE_PREC) + ((y >> _ROTATE_PREC) * sow);
 x &= _ROTATE_PREC_BITS; y &= _ROTATE_PREC_BITS;
 while (1) {
  do {
   if ((xp >= 0) && (xp < (sow - 1)) &&
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
   if ((xp >= 0) && (xp < (sow - 1)) &&
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
