/*
 * eon_fade.c
 *
 *  Created on: 04-feb-2009
 *      Author: jl
 */
#include "Eon.h"
#include "Emage.h"
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

struct _Eon_Fade_Private
{

};

static void _ctor(void *instance)
{
	Eon_Fade *f;
	Eon_Fade_Private *prv;

	f = (Eon_Fade *) instance;
	f->private = prv = ekeko_type_instance_private_get(eon_fade_type_get(), instance);
	f->parent.parent.create = eon_engine_fade_create;
	f->parent.parent.setup = eon_engine_fade_setup;
	f->parent.parent.delete = eon_engine_fade_delete;
}

static void _dtor(void *fade)
{

}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI Ekeko_Type *eon_fade_type_get(void)
{
	static Ekeko_Type *type = NULL;

	if (!type)
	{
		type = ekeko_type_new(EON_TYPE_FADE, sizeof(Eon_Fade),
				sizeof(Eon_Fade_Private), eon_transition_type_get(),
				_ctor, _dtor, eon_transition_appendable);
	}

	return type;
}

EAPI Eon_Fade * eon_fade_new(void)
{
	Eon_Fade *f;

	f = ekeko_type_instance_new(eon_fade_type_get());

	return f;
}
