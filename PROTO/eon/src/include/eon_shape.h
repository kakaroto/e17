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
#define EON_SHAPE_FILL_CHANGED "fillChanged"
/*============================================================================*
 *                               Properties                                   *
 *============================================================================*/
extern Ekeko_Property_Id EON_SHAPE_COLOR;
extern Ekeko_Property_Id EON_SHAPE_ROP;
extern Ekeko_Property_Id EON_SHAPE_FILTER;
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
};

EAPI Ekeko_Type *eon_shape_type_get(void);

EAPI void eon_shape_color_set(Eon_Shape *s, Eon_Color color);
EAPI Eon_Color eon_shape_color_get(Eon_Shape *s);

EAPI void eon_shape_rop_set(Eon_Shape *s, Enesim_Rop rop);
EAPI Enesim_Rop eon_shape_rop_get(Eon_Shape *s);

EAPI void eon_shape_filter_set(Eon_Shape *s, Eon_Filter *filter);
EAPI Eon_Filter * eon_shape_filter_get(Eon_Shape *s);

EAPI void eon_shape_fill_set(Eon_Shape *s, Eon_Paint *paint);
EAPI Eon_Paint * eon_shape_fill_get(Eon_Shape *s);

#define eon_shape_show(s) ekeko_renderable_show(EKEKO_RENDERABLE((s)))
#define eon_shape_hide(s) ekeko_renderable_hide(EKEKO_RENDERABLE((s)))

#endif /* EON_SHAPE_H_ */
