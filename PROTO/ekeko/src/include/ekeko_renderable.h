/* EKEKO - Object and property system
 * Copyright (C) 2007-2009 Jorge Luis Zapata
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
#ifndef EKEKO_RENDERABLE_H_
#define EKEKO_RENDERABLE_H_

/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EKEKO_RENDERABLE_GEOMETRY_CHANGED "geometryChanged"
#define EKEKO_RENDERABLE_VISIBILITY_CHANGED "visibilityChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EKEKO_RENDERABLE_GEOMETRY;
extern Ekeko_Property_Id EKEKO_RENDERABLE_VISIBILITY;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
#define EKEKO_RENDERABLE(o) EKEKO_CAST(o, ekeko_renderable_type_get(), Ekeko_Renderable)
typedef struct _Ekeko_Renderable_Private Ekeko_Renderable_Private;

struct _Ekeko_Renderable
{
	Ekeko_Object parent;
	/* Function called whenever the canvas needs this renderable
	 * to be rendered
	 */
	void (*render)(Ekeko_Renderable *r, Eina_Rectangle *rect);
	Eina_Bool (*is_inside)(Ekeko_Renderable *r, int x, int y);
	Ekeko_Renderable_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *ekeko_renderable_type_get(void);
EAPI void ekeko_renderable_geometry_set(Ekeko_Renderable *r, Eina_Rectangle *rect);
EAPI void ekeko_renderable_geometry_get(Ekeko_Renderable *r, Eina_Rectangle *rect);
EAPI Ekeko_Canvas * ekeko_renderable_canvas_get(Ekeko_Renderable *r);
EAPI void ekeko_renderable_show(Ekeko_Renderable *r);
EAPI void ekeko_renderable_hide(Ekeko_Renderable *r);
EAPI void ekeko_renderable_visibility_set(Ekeko_Renderable *r, Eina_Bool visible);
EAPI void ekeko_renderable_visibility_get(Ekeko_Renderable *r, Eina_Bool *visible);
EAPI void ekeko_renderable_move(Ekeko_Renderable *r, int x, int y);
EAPI void ekeko_renderable_resize(Ekeko_Renderable *r, int w, int h);

#endif /* EKEKO_RENDERABLE_H_ */
