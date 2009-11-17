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
#define EON_PAINT_ROP_CHANGED "ropChanged"
#define EON_PAINT_COLOR_CHANGED "colorChanged"
#define EON_PAINT_MATRIX_CHANGED "matrixChanged"
#define EON_PAINT_COORDSPACE_CHANGED "coordspaceChanged"
#define EON_PAINT_MATRIXSPACE_CHANGED "matrixspaceChanged"
#define EON_PAINT_STYLE_CHANGED "styleChanged"
#define EON_PAINT_VISIBILITY_CHANGED "visibilityChanged"
#define EON_PAINT_GEOMETRY_CHANGED "geometryChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_PAINT_COLOR;
extern Ekeko_Property_Id EON_PAINT_ROP;
extern Ekeko_Property_Id EON_PAINT_MATRIX;
extern Ekeko_Property_Id EON_PAINT_COORDSPACE;
extern Ekeko_Property_Id EON_PAINT_MATRIXSPACE;
extern Ekeko_Property_Id EON_PAINT_STYLE;
extern Ekeko_Property_Id EON_PAINT_VISIBILITY;
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

#define EON_PAINT(o) EKEKO_CAST(o, eon_paint_type_get(), Eon_Paint)
typedef struct _Eon_Paint_Private Eon_Paint_Private;
struct _Eon_Paint
{
	Eon_Object base;
	Eon_Paint_Private *prv;

	/* called whenever the engine wants to instantiate a new paint object */
	void *(*create)(Eon_Engine *e, Eon_Paint *s);
	/* called whenever the paint is going to be rendered */
	void (*render)(Eon_Paint *p, Eon_Engine *e, void *engine_data,
			void *canvas_data, Eina_Rectangle *clip);
	/* called to check if the point is inside the paint */
	Eina_Bool (*is_inside)(Eon_Paint *p, int x, int y);
	/* called whenever the paint is going to be destroyed */
	void (*free)(Eon_Engine *e, void *engine_data);
	/* called when a parent paint needs to update the coordinates
	 * of its child. The parea defines the area the parent wants
	 * the child to draw
	 * FIXME remove this
	 */
	void (*coordinates_update)(Eon_Paint *p, Eina_Rectangle *parea);
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_paint_type_get(void);

EAPI void eon_paint_matrix_set(Eon_Paint *p, Enesim_Matrix *m);
EAPI void eon_paint_matrix_get(Eon_Paint *p, Enesim_Matrix *m);

EAPI void eon_paint_matrixspace_set(Eon_Paint *p, Eon_Paint_Matrixspace cs);
EAPI Eon_Paint_Matrixspace eon_paint_matrixspace_get(Eon_Paint *p);

EAPI Eon_Paint_Coordspace eon_paint_coordspace_get(Eon_Paint *p);
EAPI void eon_paint_coordspace_set(Eon_Paint *p, Eon_Paint_Coordspace cs);

EAPI void eon_paint_color_set(Eon_Paint *s, Eon_Color color);
EAPI Eon_Color eon_paint_color_get(Eon_Paint *s);

EAPI void eon_paint_rop_set(Eon_Paint *s, Enesim_Rop rop);
EAPI Enesim_Rop eon_paint_rop_get(Eon_Paint *s);

EAPI void eon_paint_style_set(Eon_Paint *p, Eon_Style *s);
EAPI Eon_Style * eon_paint_style_get(Eon_Paint *p);

EAPI Eina_Bool eon_paint_visibility_get(Eon_Paint *p);
EAPI void eon_paint_show(Eon_Paint *p);
EAPI void eon_paint_hide(Eon_Paint *p);
EAPI void eon_paint_visibility_set(Eon_Paint *p, Eina_Bool visible);

EAPI void eon_paint_boundings_get(Eon_Paint *p, Eina_Rectangle *bounds);
EAPI void eon_paint_geometry_get(Eon_Paint *p, Eina_Rectangle *rect);

#endif /* EON_PAINT_H_ */
