#ifndef ENESIM_MATRIX_H_
#define ENESIM_MATRIX_H_

/* TODO
 * Create all this Macros
 * Replace all float * with Enesim_Matrix
 */
#define ENESIM_MATRIX_XX(m) m[0]

typedef struct _Enesim_Matrix Enesim_Matrix;
typedef struct _Enesim_Quad Enesim_Quad;


EAPI Enesim_Matrix * enesim_matrix_new(void);
EAPI void enesim_matrix_free(Enesim_Matrix *m);
EAPI void enesim_matrix_values_set(Enesim_Matrix *m, float a, float b, float c,
		float d, float e, float f, float g, float h, float i);
EAPI void enesim_matrix_values_get(Enesim_Matrix *m, float *a, float *b, 
		float *c, float *d, float *e, float *f, float *g, float *h,
		float *i);
EAPI void enesim_matrix_compose(Enesim_Matrix *m1, Enesim_Matrix *m2, Enesim_Matrix *dst);
EAPI void enesim_matrix_translate(Enesim_Matrix *t, float tx, float ty);
EAPI void enesim_matrix_scale(Enesim_Matrix *t, float sx, float sy);
EAPI void enesim_matrix_rotate(Enesim_Matrix *t, float rad);
EAPI void enesim_matrix_identity(Enesim_Matrix *t);
EAPI float enesim_matrix_determinant(Enesim_Matrix *m);
EAPI void enesim_matrix_divide(Enesim_Matrix *m, float scalar);
EAPI void enesim_matrix_inverse(Enesim_Matrix *m, Enesim_Matrix *m2);
EAPI void enesim_matrix_adjoint(Enesim_Matrix *m, Enesim_Matrix *a);
EAPI void enesim_matrix_point_transform(Enesim_Matrix *m, float x, float y, float *xr, float *yr);
EAPI Eina_Bool enesim_matrix_quad_quad_to(Enesim_Matrix *m, Enesim_Quad *src, Enesim_Quad *dst);
EAPI Eina_Bool enesim_matrix_square_quad_to(Enesim_Matrix *m, Enesim_Quad *q);
EAPI Eina_Bool enesim_matrix_quad_square_to(Enesim_Matrix *m, Enesim_Quad *q);
EAPI Enesim_Quad * enesim_quad_new(void);
EAPI void enesim_quad_free(Enesim_Quad *q);
EAPI void enesim_quad_coords_set(Enesim_Quad *q, float x1, float y1, float x2,
		float y2, float x3, float y3, float x4, float y4);
EAPI void enesim_quad_coords_get(Enesim_Quad *q, float *x1, float *y1, 
		float *x2, float *y2, float *x3, float *y3, float *x4,
		float *y4);


#endif /*ENESIM_MATRIX_H_*/
