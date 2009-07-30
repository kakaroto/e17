#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define EON_IMAGE_DEBUG 0
#define PRIVATE(d) ((Eon_Checker_Private *)((Eon_Checker *)(d))->private)

struct _Eon_Checker_Private
{
	Eon_Coord w, h;
	Eon_Color color1, color2;
};

static void _ctor(void *instance)
{
	Eon_Checker *sq;
	Eon_Checker_Private *prv;

	sq = (Eon_Checker *)instance;
	sq->private = prv = ekeko_type_instance_private_get(eon_checker_type_get(), instance);
	sq->parent.create = eon_engine_sqpattern_create;
	sq->parent.setup = eon_engine_sqpattern_setup;
	sq->parent.delete = eon_engine_sqpattern_delete;
}

static void _dtor(void *image)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_CHECKER_SW;
Ekeko_Property_Id EON_CHECKER_SH;
Ekeko_Property_Id EON_CHECKER_COLOR1;
Ekeko_Property_Id EON_CHECKER_COLOR2;

EAPI Ekeko_Type *eon_checker_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_CHECKER, sizeof(Eon_Checker),
				sizeof(Eon_Checker_Private), eon_paint_type_get(),
				_ctor, _dtor, eon_paint_appendable);
		EON_CHECKER_COLOR1 = EKEKO_TYPE_PROP_SINGLE_ADD(type, "color1", EON_PROPERTY_COLOR, OFFSET(Eon_Checker_Private, color1));
		EON_CHECKER_COLOR2 = EKEKO_TYPE_PROP_SINGLE_ADD(type, "color2", EON_PROPERTY_COLOR, OFFSET(Eon_Checker_Private, color2));
		EON_CHECKER_SW = EKEKO_TYPE_PROP_SINGLE_ADD(type, "sw", EON_PROPERTY_COORD, OFFSET(Eon_Checker_Private, w));
		EON_CHECKER_SH = EKEKO_TYPE_PROP_SINGLE_ADD(type, "sh", EON_PROPERTY_COORD, OFFSET(Eon_Checker_Private, h));
	}

	return type;
}

EAPI Eon_Checker * eon_checker_new(void)
{
	Eon_Checker *sq;

	sq = ekeko_type_instance_new(eon_checker_type_get());

	return sq;
}

EAPI Eon_Color eon_checker_color1_get(Eon_Checker *sq)
{
	Eon_Checker_Private *prv = PRIVATE(sq);

	return prv->color1;
}

EAPI Eon_Color eon_checker_color2_get(Eon_Checker *sq)
{
	Eon_Checker_Private *prv = PRIVATE(sq);

	return prv->color2;
}

EAPI void eon_checker_color1_set(Eon_Checker *sq, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)sq, "color1", &v);
}

EAPI void eon_checker_color2_set(Eon_Checker *sq, Eon_Color color)
{
	Ekeko_Value v;

	eon_value_color_from(&v, color);
	ekeko_object_property_value_set((Ekeko_Object *)sq, "color2", &v);
}
