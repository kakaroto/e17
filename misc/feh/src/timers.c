/* timers.c
 *
 * Copyright (C) 1999 Tom Gilbert
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "feh.h"

fehtimer first_timer = NULL;

void
feh_handle_timer (void)
{
  fehtimer ft;

  D (("In feh_handle_timer\n"));
  if (!first_timer)
  {
      D(("   No timer to handle, returning\n"));
      return;
  }
  ft = first_timer;
  first_timer = first_timer->next;
  D(("   Executing timer function now\n"));
  (*(ft->func)) (ft->data);
  D(("   Freeing the timer\n"));
  if (ft && ft->name)
    free (ft->name);
  if (ft)
    free (ft);
}

double
feh_get_time (void)
{
  struct timeval timev;

  D (("In feh_get_time\n"));
  gettimeofday (&timev, NULL);
  return (double) timev.tv_sec + (((double) timev.tv_usec) / 1000000);
}

void
feh_remove_timer (char *name)
{
  fehtimer ft, ptr, pptr;

  D (("In feh_remove_timer, removing %s\n", name));
  pptr = NULL;
  ptr = first_timer;
  while (ptr)
    {
	D(("   Stepping through event list\n"));
      ft = ptr;
      if (!strcmp (ft->name, name))
	{
          D (("   Found it. Removing\n"));
	  if (pptr)
	    pptr->next = ft->next;
	  else
	    first_timer = ft->next;
	  if (ft->next)
	    ft->next->in += ft->in;
	  if (ft->name)
	    free (ft->name);
	  if (ft)
	    free (ft);
	  return;
	}
      pptr = ptr;
      ptr = ptr->next;
    }
}

void
feh_add_timer (void (*func) (void *data), void *data, double in, char *name)
{
  fehtimer ft, ptr, pptr;
  double tally;

  D (("In feh_add_timer, adding timer %s for %f seconds time\n", name, in));
  feh_remove_timer (name);
  ft = malloc (sizeof (_fehtimer));
  ft->next = NULL;
  ft->func = func;
  ft->data = data;
  ft->name = estrdup (name);
  ft->just_added = 1;
  ft->in = in;
  D(("ft->in = %f\n",ft->in));
  tally = 0.0;
  if (!first_timer)
  {
      D(("No first timer\n"));
      first_timer = ft;
  }
  else
    {
      D(("There is a first timer\n"));
      pptr = NULL;
      ptr = first_timer;
      tally = 0.0;
      while (ptr)
	{
	  tally += ptr->in;
	  if (tally > in)
	    {
	      tally -= ptr->in;
	      ft->next = ptr;
	      if (pptr)
		pptr->next = ft;
	      else
		first_timer = ft;
	      ft->in -= tally;
	      if (ft->next)
		ft->next->in -= ft->in;
	      return;
	    }
	  pptr = ptr;
	  ptr = ptr->next;
	}
      if (pptr)
	pptr->next = ft;
      else
	first_timer = ft;
      ft->in -= tally;
    }
  D(("ft->in = %f\n",ft->in));
}
