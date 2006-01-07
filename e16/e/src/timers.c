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
   double              tally;

   RemoveTimerEvent(name);
   qe = Emalloc(sizeof(Qentry));
   if (!qe)
      return;

   qe->name = Estrdup(name);
   qe->func = func;
   qe->next = NULL;
   qe->in_time = in_time;
   qe->runtime_val = runtime_val;
   qe->runtime_data = runtime_data;
   qe->just_added = 1;

   /* if there is no queue it becomes the queue */
   if (!q_first)
      q_first = qe;
   else
     {
	pptr = NULL;
	ptr = q_first;
	tally = 0.0;
	/* scan along the queue from start until sum of all timer intervals */
	/* greater than time then insert before last entry found (since it */
	/* managed to push the tally over past the end of out new timer) */
	while (ptr)
	  {
	     /* add to tally */
	     tally += ptr->in_time;
	     /* if this entry is after our new one add before this entry */
	     if (tally > in_time)
	       {
		  /* number of seconds of stuff to do before the new entry */
		  tally -= ptr->in_time;
		  /* if the previous pointer exists then add like normal */
		  qe->next = ptr;
		  if (pptr)
		    {
		       pptr->next = qe;
		    }
		  /* no previous - must have to add as first in list */
		  else
		     q_first = qe;
		  /* subtract "stuff to do before" as timers are relative */
		  qe->in_time -= tally;
		  /* if there is a timer after this one, subtract the time this
		   * new timer has to wait from it since its inserted into the 
		   * time span here
		   */
		  if (qe->next)
		     qe->next->in_time -= qe->in_time;
		  /* we're done */
		  return;
	       }
	     pptr = ptr;
	     ptr = ptr->next;
	     /* keep going through the list till we reach the end */
	  }
	/* add to end of list */
	if (pptr)
	   pptr->next = qe;
	else
	   q_first = qe;

	qe->in_time -= tally;
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
   (*(qe->func)) (qe->runtime_val, qe->runtime_data);
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
   ptr = q_first;
   /* hunt through the queue */
   while (ptr)
     {
	/* if the name of a timer matches */
	qe = ptr;
	if (!strcmp(qe->name, name))
	  {
	     /* remove it form the queue */
	     if (pptr)
		pptr->next = qe->next;
	     else
		q_first = qe->next;
	     /* increase the time of the next timer accordingly */
	     if (qe->next)
		qe->next->in_time += qe->in_time;
	     /* free it */
	     if (qe->name)
		Efree(qe->name);
	     if (qe)
		Efree(qe);
	     /* done */
	     return 1;
	  }
	pptr = ptr;
	ptr = ptr->next;
	/* keep going through the queue */
     }

   return 0;
}
