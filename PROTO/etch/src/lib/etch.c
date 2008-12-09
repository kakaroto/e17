/* ETCH - Timeline Based Animation Library
 * Copyright (C) 2007-2008 Jorge Luis Zapata, Hisham Mardam-Bey
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
#include "Etch.h"
#include "etch_private.h"

/**
 * TODO
 * + maybe a function to call whenever the fps timer has match? like:
 * etc_timer_notify(Etch *)
 * + maybe remove the _timer_ prefix?
 * TODO remove every double and use Etch_Time
 */
/*============================================================================*
 *                                  Local                                     * 
 *============================================================================*/
#define DEFAULT_FPS 30

static void _fps_to_time(unsigned long frame, unsigned long *time)
{
	/* giving a frame transform it to secs|usec representation */ 
}

static void _process(Etch *e)
{
	Etch_Animation *a;

	/* TODO use e->start and e->end */
	/* iterate over the list of animations */
	EINA_INLIST_FOREACH(e->animations, a)
	{
		/* check that the animation start and end is between our time */
		//printf("[%g] %g %g\n", e->curr, a->start, a->end);
		if ((e->curr >= a->start) && (e->curr <= a->end))
		{
			etch_animation_animate(a, e->curr);
		}		
	}
}
/*============================================================================*
 *                                 Global                                     * 
 *============================================================================*/

/*============================================================================*
 *                                   API                                      * 
 *============================================================================*/
/**
 * Create a new Etch instance
 */
EAPI Etch * etch_new(void)
{
	Etch *e;
	
	e = malloc(sizeof(Etch));
	e->animations = NULL;
	e->frame = 0;
	e->curr = 0;
	e->fps = DEFAULT_FPS;
	return e;
}

/**
 * Delete the Etch instance
 */
EAPI void etch_delete(Etch *e)
{
	assert(e);
	/* remove every object */
	free(e);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void etch_timer_fps_set(Etch *e, unsigned int fps)
{
	double spf;

	assert(e);

	e->fps = fps;
	spf = (double)1/fps;
	etch_time_double_from(&e->tpf, spf);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI unsigned int etch_timer_fps_get(Etch *e)
{
	assert(e);
	return e->fps;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void etch_timer_tick(Etch *e)
{
	assert(e);
	/* TODO check for overflow */
	e->frame++;
	e->curr += (double)1/e->fps;
	_process(e);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI int etch_timer_has_end(Etch *e)
{
	/* we need a function to get the last frame/marker to know when the
	 * animations have finished, on the application we can make it run again,
	 * stop, whatever */
	
	return 0;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void etch_timer_get(Etch *e, unsigned long *secs, unsigned long *usecs)
{
	Etch_Time t;
	
	etch_time_double_from(&t, e->curr);
	if (secs) *secs = t.secs;
	if (usecs) *usecs = t.usecs;
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void etch_timer_goto(Etch *e, unsigned long frame)
{
	e->frame = frame;
	e->curr = (double)frame/e->fps;
	_process(e);
}
/**
 * Create a new animation
 * @param dtype Data type the animation will animate
 */
EAPI Etch_Animation * etch_animation_add(Etch *e, Etch_Data_Type dtype,
		Etch_Animation_Callback cb, void *data)
{
	Etch_Animation *a;

	a = etch_animation_new(dtype, cb, data);
	e->animations = eina_inlist_append(e->animations, EINA_INLIST_GET(a));

	return a;
}
