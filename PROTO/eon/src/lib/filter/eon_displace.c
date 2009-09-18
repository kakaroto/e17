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
#define PRIVATE(d) ((Eon_Displace_Private *)((Eon_Displace *)(d))->private)
struct _Eon_Displace_Private
{
	Eon_Filter *map;
};

static void _ctor(void *instance)
{
	Eon_Displace *i;
	Eon_Displace_Private *prv;

	i = (Eon_Displace *) instance;
	i->private = prv = ekeko_type_instance_private_get(eon_displace_type_get(), instance);
}

static void _dtor(void *filter)
{

}

static Eina_Bool _appendable(void *instance, void *child)
{
	if (!ekeko_type_instance_is_of(child, EON_TYPE_ANIMATION))
	{
		return EINA_FALSE;
	}
	return EINA_TRUE;
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_DISPLACE_MAP;

EAPI Ekeko_Type *eon_displace_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_FILTER, sizeof(Eon_Displace),
				sizeof(Eon_Displace_Private), eon_filter_type_get(),
				_ctor, _dtor, _appendable);
		EON_DISPLACE_SRC = EKEKO_TYPE_PROP_SINGLE_ADD(type, "src", EKEKO_PROPERTY_OBJECT, OFFSET(Eon_Displace_Private, src));
		EON_DISPLACE_MAP = EKEKO_TYPE_PROP_SINGLE_ADD(type, "map", EKEKO_PROPERTY_OBJECT, OFFSET(Eon_Displace_Private, map));
	}

	return type;
}

EAPI Eon_Displace * eon_displace_new(Eon_Canvas *c)
{
	Eon_Displace *fe;

	fe = ekeko_type_instance_new(eon_displace_type_get());
	ekeko_object_child_append((Ekeko_Object *)c, (Ekeko_Object *)fe);

	return fe;
}

EAPI void eon_displace_map_set(Eon_Displace *d, Eon_Paint *p)
{
	Eon_Displace_Private *prv = PRIVATE(d);

}
