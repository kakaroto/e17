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
#define PRIVATE(d) ((Eon_Style_Applier_Private *)((Eon_Style_Applier *)(d))->prv)

struct _Eon_Style_Applier_Private
{
};

static void _ctor(Ekeko_Object *o)
{
	Eon_Style_Applier *s;
	Eon_Style_Applier_Private *prv;

	s = (Eon_Style_Applier *)o;
	s->prv = prv = ekeko_type_instance_private_get(
			eon_style_applier_type_get(), o);
}

static void _dtor(Ekeko_Object *o)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ekeko_Type * eon_style_applier_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_STYLE_APPLIER,
				sizeof(Eon_Style_Applier),
				sizeof(Eon_Style_Applier_Private),
				ekeko_object_type_get(),
				_ctor, _dtor, NULL);

	}

	return type;
}
