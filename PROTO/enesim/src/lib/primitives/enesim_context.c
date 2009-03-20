/*
 * enesim_context.c
 *
 *  Created on: 12-feb-2009
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
EAPI Enesim_Context * enesim_context_new(void)
{
	Enesim_Context *c;

	c = calloc(1, sizeof(Enesim_Context));
	//c->tx = enesim_transformation_new();

	return c;
}

EAPI void enesim_context_delete(Enesim_Context *c)
{
	//enesim_transformation_delete(c->tx);
	free(c);
}

EAPI void enesim_context_color_set(Enesim_Context *c, uint32_t argb)
{
	c->color = argb;
	//enesim_transformation_color_set(c->tx, argb);
}

EAPI uint32_t enesim_context_color_get(Enesim_Context *c)
{
	return c->color;
}

EAPI void enesim_context_rop_set(Enesim_Context *c, Enesim_Rop rop)
{
	c->rop = rop;
	//enesim_transformation_rop_set(c->tx, rop);
}

EAPI void enesim_context_matrix_set(Enesim_Context *c, Enesim_Matrix *m)
{
	//enesim_transformation_matrix_set(c->tx, m);
}

EAPI void enesim_context_clip_set(Enesim_Context *c, Eina_Rectangle *r)
{
	c->clip.used = EINA_TRUE;
	c->clip.r = *r;
}
