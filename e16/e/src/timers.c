/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
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
#include <sys/time.h>

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
   qe = Emalloc(sizeof(Qentry));
   if (!qe)
      return;

   if (EventDebug(EDBUG_TYPE_EVENTS))
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

Qentry             *
GetHeadTimerQueue(void)
{
   return q_first;
}

void
HandleTimerEvent(void)
{
   Qentry             *qe;

   /* no timers - exit */
   if (!q_first)
      return;

   /* get the first timer */
   qe = q_first;
   /* remove it */
   q_first = q_first->next;
   /* run this callback */
   qe->func(qe->runtime_val, qe->runtime_data);
   /* free the timer */
   if (qe->name)
      Efree(qe->name);
   if (qe)
      Efree(qe);
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

static Ecore_List  *idler_list = NULL;

typedef struct _idler Idler;
typedef void        (IdlerFunc) (void *data);

struct _idler
{
   int                 order;
   IdlerFunc          *func;
   void               *data;
};

void
IdlerAdd(int order, IdlerFunc * func, void *data)
{
   Idler              *id;

   id = Emalloc(sizeof(Idler));
   if (!id)
      return;

   id->order = order;		/* Not used atm. */
   id->func = func;
   id->data = data;

   if (!idler_list)
      idler_list = ecore_list_new();

   ecore_list_append(idler_list, id);
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

   ECORE_LIST_FOR_EACH(idler_list, id) id->func(id->data);
}
