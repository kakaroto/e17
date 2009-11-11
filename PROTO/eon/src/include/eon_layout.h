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
#ifndef EON_LAYOUT_H_
#define EON_LAYOUT_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_LAYOUT_REDRAW_CHANGED "redrawChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_LAYOUT_REDRAW;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
#define EON_LAYOUT(o) EKEKO_CAST(o, eon_layout_type_get(), Eon_Layout)
typedef struct _Eon_Layout_Private Eon_Layout_Private;
struct _Eon_Layout
{
	Eon_Paint_Square base;
	/* inform the layout that an area must be flushed
	 * returns EINA_TRUE if the whole layout has been flushed
	 * or EINA_FALSE if only the needed rectangle
	 */
	Eina_Bool (*flush)(Eon_Layout *, Eina_Rectangle *);
	Eon_Layout_Private *prv;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
Ekeko_Type *eon_layout_type_get(void);
EAPI void eon_layout_size_set(Eon_Layout *c, int w, int h);
EAPI void eon_layout_damage_add(Eon_Layout *c, Eina_Rectangle *r);
EAPI void eon_layout_obscure_add(Eon_Layout *c, Eina_Rectangle *r);
EAPI Eon_Input * eon_layout_input_new(Eon_Layout *c);
EAPI Eon_Renderable * eon_layout_renderable_get_at_coord(Eon_Layout *c, unsigned int x, unsigned int y);
EAPI void eon_layout_focus_set(Eon_Layout *c, Eon_Renderable *r);
EAPI Eon_Renderable * eon_layout_focus_get(Eon_Layout *c);

#endif /* EON_LAYOUT_H_ */
