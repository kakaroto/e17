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
#include "Ekeko.h"
#include "ekeko_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/**
 * @brief
 */
struct _Ekeko_Property
{
	char *name;
	Type_Property_Type prop_type;
	ssize_t curr_offset;
	ssize_t prev_offset;
	ssize_t changed_offset;
	Ekeko_Value_Type value_type;
	Ekeko_Type *type;
	Ekeko_Property_Id id;
};
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Ekeko_Property * property_new(Ekeko_Type *type, char *prop_name, Type_Property_Type prop_type,
		Ekeko_Value_Type value_type, ssize_t curr_offset, ssize_t prev_offset,
		ssize_t changed_offset)
{
	Ekeko_Property *property;
	static Ekeko_Property_Id id = 0;

#ifdef EKEKO_DEBUG
	printf("[Ekeko_Property] new %s property at offset %d with id %d\n", prop_name, curr_offset, id + 1);
#endif
	property = malloc(sizeof(Ekeko_Property));
	property->name = strdup(prop_name);
	property->prop_type = prop_type;
	property->value_type = value_type;
	property->curr_offset = curr_offset;
	property->prev_offset = prev_offset;
	property->changed_offset = changed_offset;
	property->type = type;
	property->id = ++id;

	return property;
}

/* TODO make this getters inline on private.h */
Ekeko_Type * property_type_get(Ekeko_Property *p)
{
	return p->type;
}
ssize_t property_curr_offset_get(Ekeko_Property *p)
{
	return p->curr_offset;
}

ssize_t property_prev_offset_get(Ekeko_Property *p)
{
	return p->prev_offset;
}

ssize_t property_changed_offset_get(Ekeko_Property *p)
{
	return p->changed_offset;
}


Type_Property_Type property_ptype_get(Ekeko_Property *p)
{
	return p->prop_type;
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ekeko_Property_Id ekeko_property_id_get(const Ekeko_Property *p)
{
	return p->id;
}

EAPI const char * ekeko_property_name_get(const Ekeko_Property *p)
{
	return p->name;
}

EAPI Ekeko_Value_Type ekeko_property_value_type_get(Ekeko_Property *p)
{
	return p->value_type;
}
