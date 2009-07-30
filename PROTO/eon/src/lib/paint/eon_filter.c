/*
 * eon_filter.c
 *
 *  Created on: 04-feb-2009
 *      Author: jl
 */
#include "Eon.h"
#include "eon_private.h"
#define EON_FILTER_DEBUG 0
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Filter_Private *)((Eon_Filter *)(d))->private)
struct _Eon_Filter_Private
{
	Eon_Coord x;
	Eon_Coord y;
	Eon_Coord w;
	Eon_Coord h;
};

static void _ctor(void *instance)
{
	Eon_Filter *i;
	Eon_Filter_Private *prv;

	i = (Eon_Filter*) instance;
	i->private = prv = ekeko_type_instance_private_get(eon_filter_type_get(), instance);
}

static void _dtor(void *filter)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_FILTER_X;
Ekeko_Property_Id EON_FILTER_Y;
Ekeko_Property_Id EON_FILTER_W;
Ekeko_Property_Id EON_FILTER_H;

EAPI Ekeko_Type *eon_filter_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_FILTER, sizeof(Eon_Filter),
				sizeof(Eon_Filter_Private), ekeko_object_type_get(),
				_ctor, _dtor, NULL);
		EON_FILTER_X = EKEKO_TYPE_PROP_SINGLE_ADD(type, "x", EON_PROPERTY_COORD, OFFSET(Eon_Filter_Private, x));
		EON_FILTER_Y = EKEKO_TYPE_PROP_SINGLE_ADD(type, "y", EON_PROPERTY_COORD, OFFSET(Eon_Filter_Private, y));
		EON_FILTER_W = EKEKO_TYPE_PROP_SINGLE_ADD(type, "w", EON_PROPERTY_COORD, OFFSET(Eon_Filter_Private, w));
		EON_FILTER_H = EKEKO_TYPE_PROP_SINGLE_ADD(type, "h", EON_PROPERTY_COORD, OFFSET(Eon_Filter_Private, h));
	}

	return type;
}
