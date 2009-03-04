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
EAPI void enesim_image_draw(Enesim_Surface *d, Enesim_Context *c,
		Enesim_Surface *s, Eina_Rectangle *srect)
{
	Eina_Rectangle *clip = NULL;
	/* src is the geometry of the src surface which can be bigger (upscale)
	 * or smaller (downscale)
	 */
	if (c->clip.used)
	{
		clip = &c->clip.r;
	}
	enesim_transformation_apply(c->tx, s, srect, d, clip);
}
