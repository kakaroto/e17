#ifndef __ROTATE
#define __ROTATE 1

/*\ Calc precision \*/
#define _ROTATE_PREC 12
#define _ROTATE_PREC_MAX (1 << _ROTATE_PREC)
#define _ROTATE_PREC_BITS (_ROTATE_PREC_MAX - 1)

void __imlib_RotateSampleInside(DATA32 *src, DATA32 *dest, int sow, int dow,
    int dw, int dh, int x, int y, int dx, int dy);
void __imlib_RotateAAInside(DATA32 *src, DATA32 *dest, int sow, int dow,
   int dw, int dh, int x, int y, int dx, int dy);
void __imlib_RotateSample(DATA32 *src, DATA32 *dest, int sow, int soh, int dow,
   int dw, int dh, int x, int y, int dx, int dy);
void __imlib_RotateAA(DATA32 *src, DATA32 *dest, int sow, int soh, int dow,
   int dw, int dh, int x, int y, int dx, int dy);

#endif
