/*
 * eon_paint.h
 *
 *  Created on: 13-jul-2009
 *      Author: jl
 */

#ifndef EON_PAINT_H_
#define EON_PAINT_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_PAINT_X_CHANGED "xChanged"
#define EON_PAINT_Y_CHANGED "yChanged"
#define EON_PAINT_W_CHANGED "wChanged"
#define EON_PAINT_H_CHANGED "hChanged"
#define EON_PAINT_MATRIX_CHANGED "matrixChanged"
#define EON_PAINT_COORDSPACE_CHANGED "coordspaceChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_PAINT_X;
extern Ekeko_Property_Id EON_PAINT_Y;
extern Ekeko_Property_Id EON_PAINT_W;
extern Ekeko_Property_Id EON_PAINT_H;
extern Ekeko_Property_Id EON_PAINT_MATRIX;
extern Ekeko_Property_Id EON_PAINT_COORDSPACE;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef enum _Eon_Paint_Coordspace
{
	EON_COORDSPACE_OBJECT,
	EON_COORDSPACE_USER,
} Eon_Paint_Coordspace;

typedef struct _Eon_Paint_Private Eon_Paint_Private;
struct _Eon_Paint
{
	Ekeko_Object parent;
	Eon_Paint_Private *private;
	/* Called whenever a shape object references a paint object */
	void *(*create)(Eon_Engine *e, Eon_Paint *p);
	/* Called whenever a shape needs to be rendered and has a paint object as
	 * the fill property */
	Eina_Bool (*setup)(Eon_Engine *e, void *engine_data, Eon_Shape *s);
	/* Called whenever the last shape object unreferences this paint object */
	void (*delete)(Eon_Engine *e, void *engine_data);
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_paint_type_get(void);
EAPI void eon_paint_coords_get(Eon_Paint *p, Eon_Coord *x, Eon_Coord *y, Eon_Coord *w, Eon_Coord *h);
EAPI Eon_Paint_Coordspace eon_paint_coordspace_get(Eon_Paint *p);

#endif /* EON_PAINT_H_ */
