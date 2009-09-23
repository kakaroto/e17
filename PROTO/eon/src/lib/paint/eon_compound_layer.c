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
#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Compound_Layer_Private *)((Eon_Compound_Layer *)(d))->private)
struct _Eon_Compound_Layer_Private
{
	Enesim_Rop rop;
	Eon_Paint *paint;
};

static void _ctor(void *instance)
{
	Eon_Compound_Layer *l;
	Eon_Compound_Layer_Private *prv;

	l = (Eon_Compound_Layer *) instance;
	l->private = prv = ekeko_type_instance_private_get(eon_compound_layer_type_get(), instance);
}

static void _dtor(void *fade)
{

}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_COMPOUND_LAYER_ROP;
Ekeko_Property_Id EON_COMPOUND_LAYER_PAINT;

EAPI Ekeko_Type *eon_compound_layer_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_COMPOUND_LAYER, sizeof(Eon_Compound_Layer),
				sizeof(Eon_Compound_Layer_Private), ekeko_object_type_get(),
				_ctor, _dtor, NULL);
		EON_COMPOUND_LAYER_ROP = EKEKO_TYPE_PROP_SINGLE_ADD(type,
				"rop", EKEKO_PROPERTY_INT,
				OFFSET(Eon_Compound_Layer_Private, rop));
		EON_COMPOUND_LAYER_PAINT = EKEKO_TYPE_PROP_SINGLE_ADD(type,
				"paint", EKEKO_PROPERTY_OBJECT,
				OFFSET(Eon_Compound_Layer_Private, paint));
	}

	return type;
}

EAPI Eon_Compound_Layer * eon_compound_layer_new(void)
{
	Eon_Compound_Layer *l;

	l = ekeko_type_instance_new(eon_compound_layer_type_get());

	return l;
}

Enesim_Rop eon_compound_layer_rop_get(Eon_Compound_Layer *l)
{
	Eon_Compound_Layer_Private *prv = PRIVATE(l);

	return prv->rop;
}

Eon_Paint * eon_compound_layer_paint_get(Eon_Compound_Layer *l)
{
	Eon_Compound_Layer_Private *prv = PRIVATE(l);

	return prv->paint;
}
