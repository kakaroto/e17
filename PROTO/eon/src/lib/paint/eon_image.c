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
#define EON_IMAGE_DEBUG 0
/* TODO
 * + create a temporary image when the size has changed of size of the image itself
 * with some pattern in to inform that the image is loading on the background
 * + This paint should be a child of a buffer paint
 */
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Image_Private *)((Eon_Image *)(d))->private)

static Ekeko_Type *_type;
struct _Eon_Image_Private
{
	struct
	{
		char *curr;
		char *prev;
		int changed;
	} file;
	struct {
		Enesim_Surface *img;
		Eina_Bool loaded;
	} src;
	/* TODO add l, t, r, b */
};

static void _loader_callback(Enesim_Surface *s, void *data, int error)
{
	Eon_Image *i = (Eon_Image *)data;
	Eon_Image_Private *prv = PRIVATE(i);

	/* TODO check for error */
	if (error)
	{
		printf("[Eon_Image] Error %d %s\n", error, eina_error_msg_get(error));
	}
	/* Right now when the image is succesfully loaded we change a flag
	 * and an event is triggered. So a shape that references this paint object
	 * should register to this signal and mark itself as dirty whenever the
	 * signal is emitted.
	 */
	else
	{
		Ekeko_Value v;

		ekeko_value_bool_from(&v, EINA_TRUE);
		ekeko_object_property_value_set((Ekeko_Object *)i, "loaded", &v);
		eon_paint_change((Eon_Paint *)i);
	}
}

static void _file_change(const Ekeko_Object *o, Ekeko_Event *e, void *data)
{
	Ekeko_Event_Mutation *em = (Ekeko_Event_Mutation *)e;
	Eon_Image *i = (Eon_Image *)o;
	Eon_Image_Private *prv = PRIVATE(o);

	if (em->state != EVENT_MUTATION_STATE_POST)
		return;
	prv->src.loaded = EINA_FALSE;
	if (prv->src.img)
		enesim_surface_delete(prv->src.img);
	/* call emage to load an image */
	eon_cache_image_load(em->curr->value.string_value, &prv->src.img, ENESIM_FORMAT_ARGB8888, _loader_callback, i, NULL);
}

static void _render(Eon_Paint *p, Eon_Engine *eng, void *engine_data, void *canvas_data, Eina_Rectangle *clip)
{
	eon_engine_image_render(eng, engine_data, canvas_data, clip);
}

static void _ctor(void *instance)
{
	Eon_Image *i;
	Eon_Image_Private *prv;

	i = (Eon_Image*) instance;
	i->private = prv = ekeko_type_instance_private_get(_type, instance);
	i->parent.parent.create = eon_engine_image_create;
	i->parent.parent.free = eon_engine_image_delete;
	i->parent.parent.render = _render;
	ekeko_event_listener_add((Ekeko_Object *)i, EON_IMAGE_FILE_CHANGED,
			 _file_change, EINA_FALSE, NULL);
}

static void _dtor(void *image)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
Eina_Bool eon_image_loaded(Eon_Image *i)
{
	Eon_Image_Private *prv;

	prv = PRIVATE(i);
	return prv->src.loaded;
}

Eina_Bool eon_image_size_get(Eon_Image *i, int *w, int *h)
{
	Eon_Image_Private *prv = PRIVATE(i);

	if (!prv->src.loaded)
		return EINA_FALSE;

	enesim_surface_size_get(prv->src.img, w, h);
	return EINA_TRUE;
}

Enesim_Surface * eon_image_surface_get(Eon_Image *i)
{
	Eon_Image_Private *prv = PRIVATE(i);

	return prv->src.img;
}

void eon_image_init(void)
{
	_type = ekeko_type_new(EON_TYPE_IMAGE, sizeof(Eon_Image),
			sizeof(Eon_Image_Private),
			eon_paint_square_type_get(),
			_ctor, _dtor, eon_paint_appendable);
	EON_IMAGE_FILE = EKEKO_TYPE_PROP_DOUBLE_ADD(_type, "file",
			EKEKO_PROPERTY_STRING,
			OFFSET(Eon_Image_Private, file.curr),
			OFFSET(Eon_Image_Private, file.prev),
			OFFSET(Eon_Image_Private, file.changed));
	EON_IMAGE_LOADED = EKEKO_TYPE_PROP_SINGLE_ADD(_type, "loaded",
			EKEKO_PROPERTY_BOOL,
			OFFSET(Eon_Image_Private, src.loaded));

	eon_type_register(_type, EON_TYPE_IMAGE);
}

void eon_image_shutdown(void)
{
	eon_type_unregister(_type);
}
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
Ekeko_Property_Id EON_IMAGE_FILE;
Ekeko_Property_Id EON_IMAGE_LOADED;

/**
 * Creates a new image
 * @param[in] d The document to create this image on
 * @return The newly created image object
 */
EAPI Eon_Image * eon_image_new(Eon_Document *d)
{
	Eon_Image *i;

	i = eon_document_object_new(d, EON_TYPE_IMAGE);

	return i;
}
/**
 * Sets the file path
 * @param[in] i The image to set the file path on
 */
EAPI void eon_image_file_set(Eon_Image *i, const char *file)
{
	Ekeko_Value v;

	ekeko_value_str_from(&v, file);
	ekeko_object_property_value_set((Ekeko_Object *)i, "file", &v);
}
/**
 * Gets the file path of an image
 * @param[in] i The image to get the file path from
 * @return The file path of an image
 */
EAPI const char * eon_image_file_get(Eon_Image *i)
{
	Eon_Image_Private *prv;

	prv = PRIVATE(i);
	return prv->file.curr;
}
