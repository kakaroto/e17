/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2006-2008 Kim Woelders
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "E.h"
#include "e16-ecore_list.h"
#include "timers.h"
#include <sys/time.h>

#define DEBUG_TIMERS 0

struct _timer {
   double              in_time;
   double              at_time;
   struct _timer      *next;
   int                 (*func) (void *data);
   void               *data;
   char                again;
};

double
GetTime(void)
{
   struct timeval      timev;

   gettimeofday(&timev, NULL);
   return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

static Timer       *q_first = NULL;

static void
_TimerSet(Timer * timer)
{
   Timer              *ptr, *pptr;

#if DEBUG_TIMERS
   if (EDebug(EDBUG_TYPE_TIMERS))
      Eprintf("_TimerSet %p: func=%p data=%p\n", timer, timer->func,
	      timer->data);
#endif

   /* if there is no queue it becomes the queue */
   if (!q_first)
     {
	q_first = timer;
	timer->next = NULL;
     }
   else
     {
	pptr = NULL;
	for (ptr = q_first; ptr; pptr = ptr, ptr = ptr->next)
	  {
	     if (ptr->at_time > timer->at_time)
		break;
	  }
	if (pptr)
	   pptr->next = timer;
	else
	   q_first = timer;
	timer->next = ptr;
     }
}

static void
_TimerDel(Timer * timer)
{
#if DEBUG_TIMERS
   if (EDebug(EDBUG_TYPE_TIMERS))
      Eprintf("_TimerDel %p: func=%p data=%p\n", timer, timer->func,
	      timer->data);
#endif
   Efree(timer);
}

Timer              *
TimerAdd(double in_time, int (*func) (void *data), void *data)
{
   Timer              *timer;

   timer = EMALLOC(Timer, 1);
   if (!timer)
      return NULL;

   if (in_time < 0.)		/* No negative in-times */
      in_time = 0.;

   timer->func = func;
   timer->in_time = in_time;
   timer->at_time = GetTime() + in_time;
   timer->data = data;

   if (EDebug(EDBUG_TYPE_TIMERS))
      Eprintf("TimerAdd %p: func=%p data=%p: %8.3f\n", timer,
	      timer->func, timer->data, in_time);

   _TimerSet(timer);		/* Add to timer queue */

   return timer;
}

double
TimersRun(double tt)
{
   Timer              *timer, *q_old, *q_run;
   double              t;

   timer = q_first;
   if (!q_first)
      return 0.;		/* No timers pending */

   t = tt;
   if (t <= 0.)
      t = timer->at_time;

   q_run = q_old = q_first;
   for (; timer; timer = q_first)
     {
	if (timer->at_time > t + 200e-6)	/* Within 200 us is close enough */
	   break;

	if (EDebug(EDBUG_TYPE_TIMERS))
	   Eprintf("TimersRun - run %p: func=%p data=%p: %8.3lf\n", timer,
		   timer->func, timer->data, timer->at_time - t);

	q_first = timer->next;

	/* Run this callback */
	timer->again = timer->func(timer->data);
	q_run = timer;
     }

   if (q_old != q_first)
     {
	/* At least one timer has run */
	q_run->next = NULL;	/* Terminate expired timer list */

	/* Re-schedule/remove timers that have run */
	for (timer = q_old; timer; timer = q_old)
	  {
	     q_old = timer->next;
	     if (timer->again)
	       {
		  timer->at_time += timer->in_time;
		  _TimerSet(timer);	/* Add to timer queue */
	       }
	     else
	       {
		  _TimerDel(timer);
	       }
	  }
     }

   if (tt <= 0.)		/* Avoid some redundant debug output */
      return tt;

   timer = q_first;

   if (EDebug(EDBUG_TYPE_TIMERS) > 1)
     {
	Timer              *qp;

	for (qp = timer; qp; qp = qp->next)
	   Eprintf("TimersRun - pend %p: func=%p data=%p: %8.3lf\n", qp,
		   qp->func, qp->data, qp->at_time - t);
     }

   if (timer)
     {
	t = timer->at_time - t;
	if (t <= 0.)
	   t = 1e-6;
     }
   else
     {
	t = 0.;
     }

   if (EDebug(EDBUG_TYPE_TIMERS))
      Eprintf("TimersRun - next in %8.3lf\n", t);

   return t;
}

int
TimerDel(Timer * timer)
{
   Timer              *qe, *ptr, *pptr;

   pptr = NULL;
   for (ptr = q_first; ptr; pptr = ptr, ptr = ptr->next)
     {
	qe = ptr;
	if (qe != timer)
	   continue;

	/* Match - remove it from the queue */
	if (pptr)
	   pptr->next = qe->next;
	else
	   q_first = qe->next;

	/* free it */
	_TimerDel(timer);

	/* done */
	return 1;
     }

   return 0;
}

void
TimerSetInterval(Timer * timer, double dt)
{
   timer->in_time = dt;
}

/*
 * Idlers
 */
static Ecore_List  *idler_list = NULL;

typedef void        (IdlerFunc) (void *data);

struct _idler {
   IdlerFunc          *func;
   void               *data;
};

Idler              *
IdlerAdd(IdlerFunc * func, void *data)
{
   Idler              *id;

   id = EMALLOC(Idler, 1);
   if (!id)
      return NULL;

   id->func = func;
   id->data = data;

   if (!idler_list)
      idler_list = ecore_list_new();

   ecore_list_append(idler_list, id);

   return id;
}

void
IdlerDel(Idler * id)
{
   ecore_list_node_remove(idler_list, id);
   Efree(id);
}

static void
_IdlerRun(void *_id, void *prm __UNUSED__)
{
   Idler              *id = (Idler *) _id;

   id->func(id->data);
}

void
IdlersRun(void)
{
   if (EDebug(EDBUG_TYPE_IDLERS))
      Eprintf("IdlersRun\n");

   ecore_list_for_each(idler_list, _IdlerRun, NULL);
}

/*
 * Animators
 */
#define DEBUG_ANIMATORS 0
static Ecore_List  *animator_list = NULL;
static Timer       *animator_timer = NULL;

typedef int         (AnimatorFunc) (void *data);

struct _animator {
   AnimatorFunc       *func;
   void               *data;
};

static void
_AnimatorRun(void *_an, void *prm __UNUSED__)
{
   Animator           *an = (Animator *) _an;
   int                 again;

#if DEBUG_ANIMATORS > 1
   Eprintf("AnimatorRun %p\n", an);
#endif
   again = an->func(an->data);
   if (!again)
      AnimatorDel(an);
}

static int
AnimatorsRun(void *data __UNUSED__)
{
   ecore_list_for_each(animator_list, _AnimatorRun, NULL);

   if (ecore_list_count(animator_list))
     {
	TimerSetInterval(animator_timer, 1e-3 * Conf.animation.step);
	return 1;
     }
   else
     {
	animator_timer = NULL;
	return 0;
     }
}

Animator           *
AnimatorAdd(AnimatorFunc * func, void *data)
{
   Animator           *an;

   an = EMALLOC(Animator, 1);
   if (!an)
      return NULL;

#if DEBUG_ANIMATORS
   Eprintf("AnimatorAdd %p func=%p data=%p\n", an, func, data);
#endif
   an->func = func;
   an->data = data;

   if (!animator_list)
      animator_list = ecore_list_new();

   ecore_list_append(animator_list, an);

   if (ecore_list_count(animator_list) == 1)
     {
	if (Conf.animation.step <= 0)
	   Conf.animation.step = 1;
	/* Animator list was empty - Add to timer qeueue */
	TIMER_ADD(animator_timer, 1e-3 * Conf.animation.step,
		  AnimatorsRun, NULL);
     }

   return an;
}

void
AnimatorDel(Animator * an)
{
#if DEBUG_ANIMATORS
   Eprintf("AnimatorDel %p func=%p data=%p\n", an, an->func, an->data);
#endif

   ecore_list_node_remove(animator_list, an);
   Efree(an);

   if (ecore_list_count(animator_list) == 0)
     {
	/* Animator list is empty - Remove from timer qeueue */
	TIMER_DEL(animator_timer);
     }
}
