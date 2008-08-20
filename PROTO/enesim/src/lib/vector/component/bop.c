/* ENESIM - Direct Rendering Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata
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
#include "Enesim.h"
#include "enesim_private.h"

/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
static const char _name[] = "bop";

typedef struct _Enesim_Bop
{
	Enesim_Component 	*c;
} Enesim_Bop;

static void enesim_bop_generate(void *data, int *num)
{
	Enesim_Bop *d = data;
	float x, y;
	int cmd, i = 0;
	
}

static void enesim_bop_free(void *data)
{
	Enesim_Bop *d = data;

	free(d);
}

static void enesim_bop_init(Enesim_Component *c)
{
	Enesim_Bop *d;

	d = calloc(1, sizeof(Enesim_Bop));

	d->c = c;
	c->data = d;
	c->name = _name;
	c->type = EQUIS_COMPONENT_IO;
	c->generate = enesim_bop_generate;
	c->free = enesim_bop_free;
}
/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI Enesim_Component * enesim_bop_new(void)
{
	Enesim_Component *c;

	c = enesim_component_new();
	enesim_bop_init(c);
	return c;
}
