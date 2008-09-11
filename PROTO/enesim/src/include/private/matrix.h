#ifndef MATRIX_H_
#define MATRIX_H_

#define MATRIX_XX(m) m->xx
#define MATRIX_XY(m) m->xy
#define MATRIX_XZ(m) m->xz
#define MATRIX_YX(m) m->yx
#define MATRIX_YY(m) m->yy
#define MATRIX_YZ(m) m->yz
#define MATRIX_ZX(m) m->zx
#define MATRIX_ZY(m) m->zy
#define MATRIX_ZZ(m) m->zz
#define MATRIX_SIZE 9

#define QUAD_X0(q) q->x0 
#define QUAD_Y0(q) q->y0
#define QUAD_X1(q) q->x1 
#define QUAD_Y1(q) q->y1
#define QUAD_X2(q) q->x2 
#define QUAD_Y2(q) q->y2
#define QUAD_X3(q) q->x3 
#define QUAD_Y3(q) q->y3

struct _Enesim_Matrix
{
	float xx, xy, xz;
	float yx, yy, yz;
	float zx, zy, zz;
};

struct _Enesim_Quad
{
	float x0, y0;
	float x1, y1;
	float x2, y2;
	float x3, y3;
};

#endif /*MATRIX_H_*/
