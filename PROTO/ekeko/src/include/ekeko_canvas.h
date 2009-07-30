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
#ifndef EKEKO_CANVAS_H_
#define EKEKO_CANVAS_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EKEKO_CANVAS_REDRAW_CHANGED "redrawChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EKEKO_CANVAS_REDRAW;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Ekeko_Canvas_Private Ekeko_Canvas_Private;
struct _Ekeko_Canvas
{
	Ekeko_Renderable parent;
	/* inform the canvas that an area must be flushed
	 * returns EINA_TRUE if the whole canvas has been flushed
	 * or EINA_FALSE if only the needed rectangle
	 */
	Eina_Bool (*flush)(Ekeko_Canvas *, Eina_Rectangle *);
	Ekeko_Canvas_Private *private;
};
/*============================================================================*
 *                                Functions                                   *
 *============================================================================*/
Ekeko_Type *ekeko_canvas_type_get(void);
EAPI void ekeko_canvas_size_set(Ekeko_Canvas *c, int w, int h);
EAPI void ekeko_canvas_damage_add(Ekeko_Canvas *c, Eina_Rectangle *r);
EAPI void ekeko_canvas_obscure_add(Ekeko_Canvas *c, Eina_Rectangle *r);
EAPI Ekeko_Input * ekeko_canvas_input_new(Ekeko_Canvas *c);
EAPI Ekeko_Renderable * ekeko_canvas_renderable_get_at_coord(Ekeko_Canvas *c, unsigned int x, unsigned int y);

#endif /* EKEKO_CANVAS_H_ */
