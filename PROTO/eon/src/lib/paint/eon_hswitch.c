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
#include "Emage.h"
#include "eon_private.h"

/* TODO
 * instead of handling only two images we can do the logic for a list
 * of images at the end the renderer only needs two but we can fake that :)
 */

#define EON_HSWITCH_DEBUG 0
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Hswitch_Private *)((Eon_Hswitch *)(d))->private)

struct _Eon_Hswitch_Private
{

};

static void _ctor(void *instance)
{
	Eon_Hswitch *i;
	Eon_Hswitch_Private *prv;

	i = (Eon_Hswitch *) instance;
	i->private = prv = ekeko_type_instance_private_get(eon_hswitch_type_get(), instance);
	i->parent.parent.create = eon_engine_hswitch_create;
	//i->parent.parent.setup = eon_engine_hswitch_setup;
	i->parent.parent.delete = eon_engine_hswitch_delete;
}

static void _dtor(void *hswitch)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ekeko_Type *eon_hswitch_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_HSWITCH, sizeof(Eon_Hswitch),
				sizeof(Eon_Hswitch_Private), eon_transition_type_get(),
				_ctor, _dtor, eon_transition_appendable);
	}

	return type;
}

EAPI Eon_Hswitch * eon_hswitch_new(void)
{
	Eon_Hswitch *hs;

	hs = ekeko_type_instance_new(eon_hswitch_type_get());

	return hs;
}
