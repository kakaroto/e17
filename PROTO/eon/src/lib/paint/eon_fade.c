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

/* TODO
 * instead of handling only two images we can do the logic for a list
 * of images at the end the renderer only needs two but we can fake that :)
 */

#define EON_FADE_DEBUG 0
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Fade_Private *)((Eon_Hswitch *)(d))->private)

static Ekeko_Type *_type;
struct _Eon_Fade_Private
{

};

static void _render(Eon_Paint *p, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	eon_engine_fade_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Fade *f;
	Eon_Fade_Private *prv;

	f = (Eon_Fade *) instance;
	f->private = prv = ekeko_type_instance_private_get(_type, instance);
	f->parent.parent.parent.create = eon_engine_fade_create;
	f->parent.parent.parent.render = _render;
	f->parent.parent.parent.free = eon_engine_fade_delete;
}

static void _dtor(void *fade)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_fade_init(void)
{
	_type = ekeko_type_new(EON_TYPE_FADE, sizeof(Eon_Fade),
			sizeof(Eon_Fade_Private), eon_transition_type_get(),
			_ctor, _dtor, eon_transition_appendable);

	eon_type_register(_type, EON_TYPE_FADE);
}

void eon_fade_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Eon_Fade * eon_fade_new(Eon_Document *d)
{
	Eon_Fade *f;

	f = eon_document_object_new(d, EON_TYPE_FADE);

	return f;
}
