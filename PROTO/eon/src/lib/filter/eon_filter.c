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
#define EON_FILTER_DEBUG 0
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Filter_Private *)((Eon_Filter *)(d))->prv)
struct _Eon_Filter_Private
{
	Eon_Paint *src;
};

static void _ctor(void *instance)
{
	Eon_Filter *i;
	Eon_Filter_Private *prv;

	i = (Eon_Filter *)instance;
	i->prv = prv = ekeko_type_instance_private_get(eon_filter_type_get(), instance);
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
Ekeko_Property_Id EON_FILTER_SRC;

EAPI Ekeko_Type *eon_filter_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_FILTER, sizeof(Eon_Filter),
				sizeof(Eon_Filter_Private), eon_paint_type_get(),
				_ctor, _dtor, NULL);
		EON_FILTER_SRC = EKEKO_TYPE_PROP_SINGLE_ADD(type, "src", EKEKO_PROPERTY_OBJECT, OFFSET(Eon_Filter_Private, src));
	}

	return type;
}

EAPI void eon_filter_src_set(Eon_Filter *f, Eon_Paint *src)
{
	Ekeko_Value v;

	ekeko_value_object_from(&v, (Ekeko_Object *)src);
	ekeko_object_property_value_set((Ekeko_Object *)f, "filter", &v);
}

EAPI Eon_Paint * eon_filter_src_get(Eon_Filter *f)
{
	Eon_Filter_Private *prv = PRIVATE(f);

	return prv->src;
}
