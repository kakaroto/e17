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
#ifndef EON_RENDERABLE_H_
#define EON_RENDERABLE_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_RENDERABLE_GEOMETRY_CHANGED "geometryChanged"
#define EON_RENDERABLE_VISIBILITY_CHANGED "visibilityChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_RENDERABLE_GEOMETRY;
extern Ekeko_Property_Id EON_RENDERABLE_VISIBILITY;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
#define EON_RENDERABLE(o) EKEKO_CAST(o, eon_renderable_type_get(), Eon_Renderable)
typedef struct _Eon_Renderable_Private Eon_Renderable_Private;

struct _Eon_Renderable
{
	Eon_Object base;
	/* Function called whenever the layout needs this renderable
	 * to be rendered
	 */
	void (*render)(Eon_Renderable *r, Eina_Rectangle *rect);
	Eina_Bool (*is_inside)(Eon_Renderable *r, int x, int y);
	Eon_Renderable_Private *prv;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
EAPI Ekeko_Type *eon_renderable_type_get(void);
EAPI void eon_renderable_geometry_set(Eon_Renderable *r, Eina_Rectangle *rect);
EAPI void eon_renderable_geometry_get(Eon_Renderable *r, Eina_Rectangle *rect);
EAPI Eon_Layout * eon_renderable_layout_get(Eon_Renderable *r);
EAPI void eon_renderable_show(Eon_Renderable *r);
EAPI void eon_renderable_hide(Eon_Renderable *r);
EAPI void eon_renderable_visibility_set(Eon_Renderable *r, Eina_Bool visible);
EAPI void eon_renderable_visibility_get(Eon_Renderable *r, Eina_Bool *visible);
EAPI void eon_renderable_move(Eon_Renderable *r, int x, int y);
EAPI void eon_renderable_resize(Eon_Renderable *r, int w, int h);

#endif /* EON_RENDERABLE_H_ */
