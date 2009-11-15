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
	unsigned int w;
	unsigned int h;
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
	b->parent.parent.free = eon_engine_buffer_delete;
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
	EON_BUFFER_FORMAT = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "format",
			EKEKO_PROPERTY_INT,
			OFFSET(Eon_Buffer_Private, format));
	EON_BUFFER_DATA = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "data",
			EKEKO_PROPERTY_POINTER,
			OFFSET(Eon_Buffer_Private, data));
	EON_BUFFER_WIDTH = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "dwidth",
			EKEKO_PROPERTY_INT,
			OFFSET(Eon_Buffer_Private, w));
	EON_BUFFER_HEIGHT = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "dheight",
			EKEKO_PROPERTY_INT,
			OFFSET(Eon_Buffer_Private, h));

	eon_type_register(_type, EON_TYPE_BUFFER);
}

void eon_buffer_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_BUFFER_FORMAT;
Ekeko_Property_Id EON_BUFFER_DATA;
Ekeko_Property_Id EON_BUFFER_WIDTH;
Ekeko_Property_Id EON_BUFFER_HEIGHT;

EAPI Eon_Buffer * eon_buffer_new(Eon_Document *d)
{
	Eon_Buffer *b;

	b = eon_document_object_new(d, EON_TYPE_BUFFER);

	return b;
}

EAPI void eon_buffer_format_set(Eon_Buffer *b, Enesim_Converter_Format f)
{
	Ekeko_Value v;

	ekeko_value_int_from(&v, f);
	ekeko_object_property_value_set((Ekeko_Object *)b, "format", &v);
}

EAPI Enesim_Converter_Format eon_buffer_format_get(Eon_Buffer *b)
{
	Eon_Buffer_Private *prv;

	prv = PRIVATE(b);
	return prv->format;
}

EAPI Enesim_Converter_Data * eon_buffer_data_get(Eon_Buffer *b)
{
	Eon_Buffer_Private *prv;

	prv = PRIVATE(b);
	return prv->data;
}

EAPI void eon_buffer_data_set(Eon_Buffer *b, Enesim_Converter_Data *cdata)
{
	Ekeko_Value v;

	ekeko_value_pointer_from(&v, cdata);
	ekeko_object_property_value_set((Ekeko_Object *)b, "data", &v);
}

EAPI void eon_buffer_data_width_set(Eon_Buffer *b, unsigned int w)
{
	Ekeko_Value v;

	ekeko_value_int_from(&v, w);
	ekeko_object_property_value_set((Ekeko_Object *)b, "dwidth", &v);
}

EAPI unsigned int eon_buffer_data_width_get(Eon_Buffer *b)
{
	Eon_Buffer_Private *prv;

	prv = PRIVATE(b);
	return prv->w;
}

EAPI void eon_buffer_data_height_set(Eon_Buffer *b, unsigned int h)
{
	Ekeko_Value v;

	ekeko_value_int_from(&v, h);
	ekeko_object_property_value_set((Ekeko_Object *)b, "dheight", &v);
}

EAPI unsigned int eon_buffer_data_height_get(Eon_Buffer *b)
{
	Eon_Buffer_Private *prv;

	prv = PRIVATE(b);
	return prv->h;
}

EAPI void eon_buffer_data_update(Eon_Buffer *b)
{
	Eon_Buffer_Private *prv;
	Eon_Canvas *c;
	Eon_Document *d;
	Eon_Engine *e;

	prv = PRIVATE(b);
	if (!prv->data)
		return;

	d = eon_object_document_get(b);
	e = eon_document_engine_get(d);
	eon_engine_buffer_update(e, eon_object_engine_data_get((Eon_Object *)b));
}

