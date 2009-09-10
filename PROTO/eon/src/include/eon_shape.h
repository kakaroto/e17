/*
 * eon_shape.h
 *
 *  Created on: 04-feb-2009
 *      Author: jl
 */

#ifndef EON_SHAPE_H_
#define EON_SHAPE_H_
/*============================================================================*
 *                                 Events                                     *
 *============================================================================*/
#define EON_SHAPE_COLOR_CHANGED "colorChanged"
#define EON_SHAPE_ROP_CHANGED "ropChanged"
#define EON_SHAPE_FILTER_CHANGED "filterChanged"
#define EON_SHAPE_FILL_COLOR_CHANGED "fill_colorChanged"
#define EON_SHAPE_FILL_PAINT_CHANGED "fill_paintChanged"
#define EON_SHAPE_STROKE_COLOR_CHANGED "stroke_colorChanged"
#define EON_SHAPE_STROKE_PAINT_CHANGED "stroke_paintChanged"
#define EON_SHAPE_STROKE_WIDTH_CHANGED "stroke_widthChanged"
#define EON_SHAPE_DRAW_MODE_CHANGED "draw_modeChanged"
#define EON_SHAPE_MATRIX_CHANGED "matrixChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_SHAPE_COLOR;
extern Ekeko_Property_Id EON_SHAPE_ROP;
extern Ekeko_Property_Id EON_SHAPE_FILTER;
extern Ekeko_Property_Id EON_SHAPE_FILL_COLOR;
extern Ekeko_Property_Id EON_SHAPE_FILL_PAINT;
extern Ekeko_Property_Id EON_SHAPE_STROKE_COLOR;
extern Ekeko_Property_Id EON_SHAPE_STROKE_PAINT;
extern Ekeko_Property_Id EON_SHAPE_STROKE_WIDTH;
extern Ekeko_Property_Id EON_SHAPE_DRAW_MODE;
extern Ekeko_Property_Id EON_SHAPE_MATRIX;
/*============================================================================*
 *                                 Class                                      *
 *============================================================================*/
typedef struct _Eon_Shape_Private Eon_Shape_Private;
struct _Eon_Shape
{
	Ekeko_Renderable parent;
	Eon_Shape_Private *private;
	void *(*create)(Eon_Engine *e, Eon_Shape *s);
	void (*render)(Eon_Shape *s, Eon_Engine *e, void *engine_data, void *canvas_data, Eina_Rectangle *clip);
	Eina_Bool (*is_inside)(Eon_Shape *s, int x, int y);
};

EAPI Ekeko_Type *eon_shape_type_get(void);

EAPI void eon_shape_color_set(Eon_Shape *s, Eon_Color color);
EAPI Eon_Color eon_shape_color_get(Eon_Shape *s);

EAPI void eon_shape_rop_set(Eon_Shape *s, Enesim_Rop rop);
EAPI Enesim_Rop eon_shape_rop_get(Eon_Shape *s);

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

EAPI void eon_canvas_matrix_set(Eon_Canvas *c, Enesim_Matrix *m);
EAPI void eon_canvas_matrix_get(Eon_Canvas *c, Enesim_Matrix *m);

#define eon_shape_show(s) ekeko_renderable_show(EKEKO_RENDERABLE((s)))
#define eon_shape_hide(s) ekeko_renderable_hide(EKEKO_RENDERABLE((s)))

#endif /* EON_SHAPE_H_ */
