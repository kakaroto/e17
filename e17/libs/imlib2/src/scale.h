#ifndef __SCALE
#define __SCALE 1

DATA32 **
__imlib_CalcYPoints(DATA32 *src, int sw, int sh, int dh, int b1, int b2);
int *
__imlib_CalcXPoints(int sw, int dw, int b1, int b2);
int *
__imlib_CalcApoints(int s, int d, int b1, int b2);
void
__imlib_ScaleSampleRGBA(DATA32 **ypoints, int *xpoints, DATA32 *dest,
		int dxx, int dyy, int dx, int dy, int dw, int dh, int dow);
void
__imlib_ScaleAARGBA(DATA32 **ypoints, int *xpoints, DATA32 *dest,
	    int *xapoints, int *yapoints, char xup, char yup,
	    int dxx, int dyy, int dx, int dy, int dw, int dh, int dow, int sow);
void
__imlib_ScaleAARGB(DATA32 **ypoints, int *xpoints, DATA32 *dest,
	   int *xapoints, int *yapoints, char xup, char yup,
	   int dxx, int dyy, int dx, int dy, int dw, int dh, int dow, int sow);
void
__imlib_Scale_mmx_AARGBA(DATA32 **ypoints, int *xpoints, DATA32 *dest,
	   int *xapoints, int *yapoints, char xup, char yup,
	   int dxx, int dyy, int dx, int dy, int dw, int dh, int dow, int sow);
#endif
