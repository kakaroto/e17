/* EON - Canvas and Toolkit library
 * Copyright (C) 2008-2009 Jorge Luis Zapata
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library.
 * If not, see <http://www.gnu.org/licenses/>.
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
#define EON_PAINT_MATRIXSPACE_CHANGED "matrixspaceChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_PAINT_X;
extern Ekeko_Property_Id EON_PAINT_Y;
extern Ekeko_Property_Id EON_PAINT_W;
extern Ekeko_Property_Id EON_PAINT_H;
extern Ekeko_Property_Id EON_PAINT_MATRIX;
extern Ekeko_Property_Id EON_PAINT_COORDSPACE;
extern Ekeko_Property_Id EON_PAINT_MATRIXSPACE;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef enum _Eon_Paint_Coordspace
{
	EON_COORDSPACE_OBJECT,
	EON_COORDSPACE_USER,
} Eon_Paint_Coordspace;

typedef enum _Eon_Paint_Matrixspace
{
	EON_MATRIXSPACE_OBJECT, /* use the same object's matrix */
	EON_MATRIXSPACE_USER, /* use the paint's matrix as is */
	EON_MATRIXSPACE_COMPOSE, /* compose paint's matrix with object's matrix */
} Eon_Paint_Matrixspace;

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
EAPI void eon_paint_coords_get(Eon_Paint *p, Eon_Coord *x, Eon_Coord *y,
		Eon_Coord *w, Eon_Coord *h);

EAPI Eon_Paint_Coordspace eon_paint_coordspace_get(Eon_Paint *p);
EAPI void eon_paint_coordspace_set(Eon_Paint *p, Eon_Paint_Coordspace cs);

EAPI void eon_paint_x_rel_set(Eon_Paint *p, int x);
EAPI void eon_paint_x_set(Eon_Paint *p, int x);
EAPI void eon_paint_y_set(Eon_Paint *p, int y);
EAPI void eon_paint_y_rel_set(Eon_Paint *p, int y);
EAPI void eon_paint_w_set(Eon_Paint *p, int w);
EAPI void eon_paint_w_rel_set(Eon_Paint *p, int w);
EAPI void eon_paint_h_set(Eon_Paint *p, int h);
EAPI void eon_paint_h_rel_set(Eon_Paint *p, int h);

#endif /* EON_PAINT_H_ */
