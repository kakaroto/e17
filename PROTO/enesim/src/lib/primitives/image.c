/*
 * image.c
 *
 *  Created on: 13-feb-2009
 *      Author: jl
 */
#include "Enesim.h"
#include "enesim_private.h"
/*============================================================================*
 *                                  Local                                     *
 *============================================================================*/
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
EAPI void enesim_image_draw(Enesim_Surface *d, Enesim_Context *c, Eina_Rectangle *drect,
		Enesim_Surface *s, Eina_Rectangle *srect)
{
	/* TODO clip the src rectangle to the context clip */
	enesim_transformation_apply(c->tx, s, srect, d, drect);
}
