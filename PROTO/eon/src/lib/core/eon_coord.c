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
#if 0
static inline void _coord_changed(Ekeko_Object *o, Ekeko_Event_Mutation *em,
		Eon_Coord *coord, Eon_Coord *ccoord, Eon_Coord *clength,
		Eina_Bool *relative)
{
	Eon_Coord *c, *p;

	c = em->curr->value.pointer_value;
	p = em->prev->value.pointer_value;
	if (c->type == EON_COORD_RELATIVE)
	{
		coord->final = ccoord->final + ((clength->final * coord->value) / 100);
		if (p->type == EON_COORD_ABSOLUTE)
		{
			*relative = EINA_TRUE;
		}
	}
	else
	{
		coord->final = coord->value;
		if (p->type == EON_COORD_RELATIVE)
		{
			*relative = EINA_FALSE;
		}
	}
}

static inline _parent_coord_changed(Eon_Coord *coord, Eon_Coord *ccoord, Eon_Coord *clength)
{
	if (coord->type == EON_COORD_RELATIVE)
	{
		coord->final = ccoord->final + ((clength->final * coord->value) / 100);
	}
}

static inline _parent_length_changed(Eon_Coord *length, Eon_Coord *clength)
{
	if (length->type == EON_COORD_RELATIVE)
	{
		length->final = (clength->final * length->value) / 100;
	}
}

static inline void _length_changed(Ekeko_Object *o, Ekeko_Event_Mutation *em,
		Eon_Coord *length, Eon_Coord *clength, Eina_Bool *relative)
{
	Eon_Coord *c, *p;

	c = em->curr->value.pointer_value;
	p = em->prev->value.pointer_value;
	if (c->type == EON_COORD_RELATIVE)
	{
		length->final = (clength->final * length->value) / 100;
		if (p->type == EON_COORD_ABSOLUTE)
		{
			*relative = EINA_TRUE;
		}
	}
	else
	{
		length->final = length->value;
		if (p->type == EON_COORD_RELATIVE)
		{
			*relative = EINA_FALSE;
		}
	}
}
/*============================================================================*
 *                                 Global                                     *
 *============================================================================*/
/* x, y, w, and h are the object's coordinates
 * o is the object we want to change
 * em has the parent's event
 */
Eina_Bool eon_coord_parent_changed(Ekeko_Object *o, Ekeko_Event_Mutation *em,
		Eon_Coord *x, Eon_Coord *y, Eon_Coord *w, Eon_Coord *h,
		Eon_Coord *cx, Eon_Coord *cy, Eon_Coord *cw, Eon_Coord *ch)
{
	if (em->state == EVENT_MUTATION_STATE_POST)
		return EINA_FALSE;

	printf("[Eon_Coord] PROPERTY %s\n", em->prop);
	if (!strcmp(em->prop, "x"))
	{
		_parent_coord_changed(x, cx, cw);
		return EINA_TRUE;
	}
	else if (!strcmp(em->prop, "y"))
	{
		_parent_coord_changed(y, cy, ch);
		return EINA_TRUE;
	}
	else if (!strcmp(em->prop, "w"))
	{
		_parent_length_changed(w, cw);
		return EINA_TRUE;
	}
	else if (!strcmp(em->prop, "h"))
	{
		_parent_length_changed(h, ch);
		return EINA_TRUE;
	}
	return EINA_FALSE;
}

/* Useful function to check whenever a coordinate has changed to be relative
 * or absolute, usually called on the modified event
 */
Eina_Bool eon_coord_modified(Ekeko_Object *o, Ekeko_Event_Mutation *em,
		Eon_Coord *x, Eon_Coord *y, Eon_Coord *w, Eon_Coord *h,
		Eon_Coord *cx, Eon_Coord *cy, Eon_Coord *cw, Eon_Coord *ch,
		Eina_Bool *relative)
{
	//printf("[EON_Coord] Coord Modified %d %d %d %d - %d %d %d %d\n", x->final, y->final, w->final, h->final, cx->final, cy->final, cw->final, ch->final);
	if (!strcmp(em->prop, "x"))
	{
		_coord_changed(o, em, x, cx, cw, relative);
		return EINA_TRUE;
	}
	else if (!strcmp(em->prop, "y"))
	{
		_coord_changed(o, em, y, cy, ch, relative);
		return EINA_TRUE;
	}
	else if (!strcmp(em->prop, "w"))
	{
		_length_changed(o, em, w, cw, relative);
		return EINA_TRUE;
	}
	else if (!strcmp(em->prop, "h"))
	{
		_length_changed(o, em, h, ch, relative);
		return EINA_TRUE;
	}
	return EINA_FALSE;
}
#endif

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
