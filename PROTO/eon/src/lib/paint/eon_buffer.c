/*
 * eon_buffer.c
 *
 *  Created on: 09-feb-2009
 *      Author: jl
 */
#include "Eon.h"
#include "eon_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
#define PRIVATE(d) ((Eon_Buffer_Private *)((Eon_Buffer *)(d))->private)
struct _Eon_Buffer_Private
{
	Enesim_Converter_Format format;
	void *data;
	unsigned int w, h;
	unsigned int stride;
	Enesim_Surface *src;
};

static void _ctor(void *instance)
{
	Eon_Buffer *p;
	Eon_Buffer_Private *prv;

	p = (Eon_Buffer *) instance;
	p->private = prv = ekeko_type_instance_private_get(eon_buffer_type_get(), instance);
	/* TODO on render we should get the pixels from the client in case the pixels
	 * have changed, if it is only a redraw just blit and convert from it
	 */
}

static void _dtor(void *image)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ekeko_Type *eon_buffer_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_BUFFER, sizeof(Eon_Buffer),
				sizeof(Eon_Buffer_Private), eon_paint_type_get(),
				_ctor, _dtor, eon_paint_appendable);
	}

	return type;
}

EAPI Eon_Buffer * eon_buffer_new(void)
{
	Eon_Buffer *p;

	p = ekeko_type_instance_new(eon_buffer_type_get());

	return p;
}

