/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2006-2007 Kim Woelders
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

typedef struct _qentry Qentry;
struct _qentry
{
   char               *name;
   double              at_time;
   void                (*func) (int val, void *data);
   struct _qentry     *next;
   int                 runtime_val;
   void               *runtime_data;
};

double
GetTime(void)
{
   struct timeval      timev;

   gettimeofday(&timev, NULL);
   return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

static Qentry      *q_first = NULL;

void
DoIn(const char *name, double in_time, void (*func) (int val, void *data),
     int runtime_val, void *runtime_data)
{
   Qentry             *qe, *ptr, *pptr;

   RemoveTimerEvent(name);
   qe = EMALLOC(Qentry, 1);
   if (!qe)
      return;

   if (in_time < 0.)		/* No negative in-times */
      in_time = 0.;

   if (EDebug(EDBUG_TYPE_EVENTS))
      Eprintf("DoIn %8.3f: %s\n", in_time, name);

   qe->name = Estrdup(name);
   qe->func = func;
   qe->at_time = GetTime() + in_time;
   qe->runtime_val = runtime_val;
   qe->runtime_data = runtime_data;

   /* if there is no queue it becomes the queue */
   if (!q_first)
     {
	q_first = qe;
	qe->next = NULL;
     }
   else
     {
	pptr = NULL;
	for (ptr = q_first; ptr; pptr = ptr, ptr = ptr->next)
	  {
	     if (ptr->at_time > qe->at_time)
		break;
	  }
	if (pptr)
	   pptr->next = qe;
	else
	   q_first = qe;
	qe->next = ptr;
     }
}

double
TimersRun(double tt)
{
   Qentry             *qe;
   double              t;

   qe = q_first;
   if (!q_first)
      return 0.;		/* No timers pending */

   t = tt;
   if (t <= 0.)
      t = qe->at_time;

   for (; qe; qe = q_first)
     {
	if (qe->at_time > t + 200e-6)	/* Within 200 us is close enough */
	   break;

	if (EDebug(EDBUG_TYPE_EVENTS))
	   Eprintf("TimersRun - run %8.3lf: %s\n", qe->at_time - t, qe->name);

	/* remove it */
	q_first = qe->next;

	/* run this callback */
	qe->func(qe->runtime_val, qe->runtime_data);

	/* free the timer */
	if (qe->name)
	   Efree(qe->name);
	Efree(qe);
     }

   if (tt <= 0.)		/* Avoid some redundant debug output */
      return tt;

   if (EDebug(EDBUG_TYPE_EVENTS) > 1)
     {
	Qentry             *qp;

	for (qp = qe; qp; qp = qp->next)
	   Eprintf("TimersRun - pend %8.3lf: %s\n", qp->at_time - t, qp->name);
     }

   t = (qe) ? qe->at_time - t : 0.;

   if (EDebug(EDBUG_TYPE_EVENTS))
      Eprintf("TimersRun - next in %8.3lf\n", t);

   return t;
}

int
RemoveTimerEvent(const char *name)
{
   Qentry             *qe, *ptr, *pptr;

   pptr = NULL;
   for (ptr = q_first; ptr; pptr = ptr, ptr = ptr->next)
     {
	qe = ptr;
	if (strcmp(qe->name, name))
	   continue;

	/* Match - remove it from the queue */
	if (pptr)
	   pptr->next = qe->next;
	else
	   q_first = qe->next;
	/* free it */
	if (qe->name)
	   Efree(qe->name);
	if (qe)
	   Efree(qe);
	/* done */
	return 1;
     }

   return 0;
}

/*
 * Idlers
 */
static Ecore_List  *idler_list = NULL;

typedef void        (IdlerFunc) (void *data);

struct _idler
{
   int                 order;
   IdlerFunc          *func;
   void               *data;
};

Idler              *
IdlerAdd(int order, IdlerFunc * func, void *data)
{
   Idler              *id;

   id = EMALLOC(Idler, 1);
   if (!id)
      return NULL;

   id->order = order;		/* Not used atm. */
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
   ecore_list_remove_node(idler_list, id);
   Efree(id);
}

void
IdlersRun(void)
{
   Idler              *id;

   if (EDebug(EDBUG_TYPE_EVENTS))
      Eprintf("IdlersRun\n");

   ECORE_LIST_FOR_EACH(idler_list, id) id->func(id->data);
}

/*
 * Animators
 */
#define DEBUG_ANIMATORS 0
static Ecore_List  *animator_list = NULL;

typedef int         (AnimatorFunc) (void *data);

struct _animator
{
   char               *name;
   AnimatorFunc       *func;
   void               *data;
};

static void
AnimatorsRun(int val __UNUSED__, void *data __UNUSED__)
{
   Animator           *an;
   int                 again;

   ECORE_LIST_FOR_EACH(animator_list, an)
   {
#if DEBUG_ANIMATORS > 1
      Eprintf("AnimatorRun %p\n", an);
#endif
      again = an->func(an->data);
      if (!again)
	 AnimatorDel(an);
   }

   if (ecore_list_nodes(animator_list))
      DoIn("Anim", 1e-3 * Conf.animation.step, AnimatorsRun, 0, NULL);
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
   an->name = NULL;
   an->func = func;
   an->data = data;

   if (!animator_list)
      animator_list = ecore_list_new();

   ecore_list_append(animator_list, an);

   if (ecore_list_nodes(animator_list) == 1)
     {
	if (Conf.animation.step <= 0)
	   Conf.animation.step = 1;
	/* Animator list was empty - Add to timer qeueue */
	DoIn("Anim", 1e-3 * Conf.animation.step, AnimatorsRun, 0, NULL);
     }

   return an;
}

void
AnimatorDel(Animator * an)
{
#if DEBUG_ANIMATORS
   Eprintf("AnimatorDel %p func=%p data=%p\n", an, an->func, an->data);
#endif

   ecore_list_remove_node(animator_list, an);
   if (an->name)
      Efree(an->name);
   Efree(an);

   if (ecore_list_nodes(animator_list) == 0)
     {
	/* Animator list was empty - Add to timer qeueue */
	RemoveTimerEvent("Anim");
     }
}

void               *
AnimatorGetData(Animator * an)
{
   return an->data;
}
