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
/* TODO this might be inlined */
void eon_coord_length_calculate(Eon_Coord *sl, int plength, int *l)
{
	if (sl->type == EON_COORD_RELATIVE)
	{
		*l = sl->value * plength / 100;
	}
	else
	{
		*l = sl->value;
	}
}

/* TODO this might be inlined */
void eon_coord_calculate(Eon_Coord *sc, int pc, int plength, int *c)
{
	if (sc->type == EON_COORD_RELATIVE)
	{
		*c = pc + ((sc->value * plength) / 100);
	}
	else
	{
		*c = sc->value;
	}
}

void eon_coord_length_change(const Ekeko_Object *o, Eon_Coord *dst, Eon_Coord *curr,
		Eon_Coord *prev, int length, Ekeko_Object *parent,
		const char *levent, Event_Listener el)
{
	if (curr->type == EON_COORD_RELATIVE)
	{
		dst->final = (length * curr->value) / 100;;
		if (prev->type == EON_COORD_ABSOLUTE)
		{
			/* add the length callback */
			ekeko_event_listener_add(parent, levent, el, EINA_FALSE, (void *)o);
		}
	}
	else
	{
		dst->final = curr->value;
		if (prev->type == EON_COORD_RELATIVE)
		{
			/* remove the length callback */
			ekeko_event_listener_remove(parent, levent, el, EINA_FALSE, (void *)o);
		}
	}
}

void eon_coord_change(const Ekeko_Object *o, Eon_Coord *dst, Eon_Coord *curr,
		Eon_Coord *prev, int coord, int length, Ekeko_Object *parent,
		const char *cevent, const char *levent, Event_Listener el)
{
	if (curr->type == EON_COORD_RELATIVE)
	{
		dst->final = coord + (length * curr->value) / 100;;
		if (prev->type == EON_COORD_ABSOLUTE)
		{
			/* add the length callback */
			ekeko_event_listener_add(parent, levent, el, EINA_FALSE, (void *)o);
			/* add the coord callback */
			if (cevent)
				ekeko_event_listener_add(parent, cevent, el, EINA_FALSE, (void *)o);
		}
	}
	else
	{
		dst->final = curr->value;
		if (prev->type == EON_COORD_RELATIVE)
		{
			/* remove the length callback */
			ekeko_event_listener_remove(parent, levent, el, EINA_FALSE, (void *)o);
			/* remove the coord callback */
			if (cevent)
				ekeko_event_listener_add(parent, cevent, el, EINA_FALSE, (void *)o);
		}
	}
}

/*============================================================================*
 *                                   API                                      *
 *============================================================================*/
