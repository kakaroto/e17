#include "E.h"

double
GetTime(void)
{
   struct timeval      timev;

   gettimeofday(&timev, NULL);
   return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}

static Qentry      *q_first = NULL;

void
DoIn(char *name, double in_time, void (*func) (int val, void *data),
     int runtime_val, void *runtime_data)
{
   Qentry             *qe, *ptr, *pptr;
   double              tally;

   EDBUG(5, "DoIn");
   qe = Emalloc(sizeof(Qentry));
   if (!qe)
      EDBUG_RETURN_;
   qe->name = duplicate(name);
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
		  EDBUG_RETURN_;
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
   EDBUG_RETURN_;
}

Qentry             *
GetHeadTimerQueue(void)
{
   EDBUG(6, "GetHeadTimerQueue");
   EDBUG_RETURN(q_first);
}

void
HandleTimerEvent(void)
{
   Qentry             *qe;

   EDBUG(6, "HandleTimerQueue");
   /* no timers - exit */
   if (!q_first)
      EDBUG_RETURN_;

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
   EDBUG_RETURN_;
}

void
RemoveTimerEvent(char *name)
{
   Qentry             *qe, *ptr, *pptr;

   EDBUG(6, "RemoveTimerEvent");
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
	     EDBUG_RETURN_;
	  }
	pptr = ptr;
	ptr = ptr->next;
	/* keep going through the queue */
     }
   EDBUG_RETURN_;
}
