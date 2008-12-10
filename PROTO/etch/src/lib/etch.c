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
		Etch_Time rcurr;

		/*printf("[%g] %g %g %d\n", etch_time_double_to(&e->curr),
				etch_time_double_to(&a->start),
				etch_time_double_to(&a->end),
				a->repeat);*/
		if (etch_time_ge(&e->curr, &a->start) == EINA_FALSE)
			continue;
		/* only once */
		if (a->repeat == 1)
		{
			if (etch_time_le(&e->curr, &a->end) == EINA_FALSE)
				continue;
			rcurr = e->curr;
		}
		/* in case the animation repeats check for it */
		else
		{
			Etch_Time tmp;
			Etch_Time length;

			/* n times */
			if (a->repeat)
			{
				Etch_Time tmp2;
				Etch_Time rend;
				
				/* FIXME the length can be precalculated when repeat is set */
				tmp2 = a->end;
				etch_time_multiply(&tmp2, a->repeat);
				etch_time_sub(&tmp2, &a->start, &rend);
				if (etch_time_le(&e->curr, &rend) == EINA_FALSE)
					continue;
			}
			/* FIXME the length can be precalculated when a keyframe time is set */
			etch_time_sub(&a->end, &a->start, &length);
			//printf("length %g\n", etch_time_double_to(&length));
			etch_time_sub(&e->curr, &a->start, &tmp);
			//printf("relative %g\n", etch_time_double_to(&tmp));
			etch_time_mod(&tmp, &length, &rcurr);
			//printf("mod %g\n", etch_time_double_to(&rcurr));
			etch_time_increment(&rcurr, &a->start);
			//printf("final %g (%g)\n", etch_time_double_to(&rcurr), etch_time_double_to(&e->curr));
		}
		etch_animation_animate(a, &rcurr);
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
	
	e = calloc(1, sizeof(Etch));
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
	etch_time_increment(&e->curr, &e->tpf);
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
	etch_time_secs_to(&e->curr, secs, usecs);
}
/**
 * To be documented
 * FIXME: To be fixed
 */
EAPI void etch_timer_goto(Etch *e, unsigned long frame)
{
	Etch_Time t;

	e->frame = frame;
	t = e->tpf;
	etch_time_multiply(&t, frame);
	e->curr = t;
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
