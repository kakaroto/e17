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
#ifndef EON_SHAPE_H_
#define EON_SHAPE_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_SHAPE_FILTER_CHANGED "filterChanged"
#define EON_SHAPE_FILL_COLOR_CHANGED "fill_colorChanged"
#define EON_SHAPE_FILL_PAINT_CHANGED "fill_paintChanged"
#define EON_SHAPE_STROKE_COLOR_CHANGED "stroke_colorChanged"
#define EON_SHAPE_STROKE_PAINT_CHANGED "stroke_paintChanged"
#define EON_SHAPE_STROKE_WIDTH_CHANGED "stroke_widthChanged"
#define EON_SHAPE_DRAW_MODE_CHANGED "draw_modeChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_SHAPE_FILTER;
extern Ekeko_Property_Id EON_SHAPE_FILL_COLOR;
extern Ekeko_Property_Id EON_SHAPE_FILL_PAINT;
extern Ekeko_Property_Id EON_SHAPE_STROKE_COLOR;
extern Ekeko_Property_Id EON_SHAPE_STROKE_PAINT;
extern Ekeko_Property_Id EON_SHAPE_STROKE_WIDTH;
extern Ekeko_Property_Id EON_SHAPE_DRAW_MODE;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Shape_Private Eon_Shape_Private;
struct _Eon_Shape
{
	Eon_Paint parent;
	Eon_Shape_Private *private;
};

EAPI Ekeko_Type *eon_shape_type_get(void);

EAPI void eon_shape_filter_set(Eon_Shape *s, Eon_Filter *filter);
EAPI Eon_Filter * eon_shape_filter_get(Eon_Shape *s);

EAPI void eon_shape_fill_paint_set(Eon_Shape *s, Eon_Paint *paint);
EAPI Eon_Paint * eon_shape_fill_paint_get(Eon_Shape *s);
EAPI void eon_shape_fill_color_set(Eon_Shape *s, Eon_Color color);
EAPI Eon_Color eon_shape_fill_color_get(Eon_Shape *s);

EAPI void eon_shape_stroke_paint_set(Eon_Shape *s, Eon_Paint *paint);
EAPI Eon_Paint * eon_shape_stroke_paint_get(Eon_Shape *s);
EAPI void eon_shape_stroke_color_set(Eon_Shape *s, Eon_Color color);
EAPI Eon_Color eon_shape_stroke_color_get(Eon_Shape *s);
EAPI void eon_shape_stroke_width_set(Eon_Shape *s, float width);
EAPI float eon_shape_stroke_width_get(Eon_Shape *s);

EAPI void eon_shape_draw_mode_set(Eon_Shape *s, Enesim_Shape_Draw_Mode draw_mode);
EAPI Enesim_Shape_Draw_Mode eon_shape_draw_mode_get(Eon_Shape *s);

#define eon_shape_show(s) eon_paint_show(EON_PAINT((s)))
#define eon_shape_hide(s) eon_paint_hide(EON_PAINT((s)))

#endif /* EON_SHAPE_H_ */
