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
#define PRIVATE(d) ((Eon_Buffer_Private *)((Eon_Buffer *)(d))->prv)

static Ekeko_Type *_type = NULL;
struct _Eon_Buffer_Private
{
	Enesim_Converter_Format format;
	Enesim_Converter_Data *data;
};

static void _render(Eon_Paint *p, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	eon_engine_buffer_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Buffer *b;
	Eon_Buffer_Private *prv;

	b = (Eon_Buffer *) instance;
	b->prv = prv = ekeko_type_instance_private_get(_type, instance);
	b->parent.parent.create = eon_engine_buffer_create;
	b->parent.parent.render = _render;
	b->parent.parent.delete = eon_engine_buffer_delete;
}

static void _dtor(void *image)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
void eon_buffer_init(void)
{
	_type = ekeko_type_new(EON_TYPE_BUFFER, sizeof(Eon_Buffer),
			sizeof(Eon_Buffer_Private), eon_paint_square_type_get(),
			_ctor, _dtor, eon_paint_appendable);
}

void eon_buffer_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_BUFFER_FORMAT;

EAPI Eon_Buffer * eon_buffer_new(Eon_Document *d)
{
	Eon_Buffer *b;

	b = eon_document_object_new(d, EON_TYPE_BUFFER);

	return b;
}

EAPI void eon_buffer_format_set(Eon_Document *d, Enesim_Converter_Format f)
{
	/* a change too */
}

EAPI void eon_buffer_data_set(Eon_Buffer *b, Enesim_Converter_Data *cdata)
{
	/* a change too */
}

EAPI void eon_buffer_update(Eon_Buffer *b)
{
	/* TODO notify parent canvas that we have changed */
	/* TODO notify reference objects about the change */
}
