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
struct _Eon_Compound_Private
{

};

static void _render(Eon_Paint *p, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	//eon_engine_compound_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Compound *c;
	Eon_Compound_Private *prv;

	c = (Eon_Compound *) instance;
	c->private = prv = ekeko_type_instance_private_get(eon_compound_type_get(), instance);
	//f->parent.parent.parent.create = eon_engine_fade_create;
	//f->parent.parent.parent.render = _render;
	//f->parent.parent.parent.delete = eon_engine_fade_delete;
}

static Eina_Bool _appendable(void *o, void *child)
{
	if (ekeko_type_instance_is_of(child, EON_TYPE_COMPOUND_LAYER))
		return EINA_TRUE;
	else
		return eon_paint_appendable(o, child);
}

static void _dtor(void *fade)
{

}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ekeko_Type *eon_compound_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_COMPOUND, sizeof(Eon_Compound),
				sizeof(Eon_Compound_Private), eon_paint_square_type_get(),
				_ctor, _dtor, _appendable);
	}

	return type;
}

EAPI Eon_Compound * eon_compound_new(void)
{
	Eon_Compound *c;

	c = ekeko_type_instance_new(eon_compound_type_get());

	return c;
}
