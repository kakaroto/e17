/***************************************************************************
                          events.c  -  description
                             -------------------
    begin                : Mon Apr 10 2000
    copyright            : (C) 2000 by Chris Thomas
    email                : x5rings@fsck.org
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "x.h"
#include "mem.h"
#include "events.h"
#include <unistd.h>

static void         Elogin_SetupEvents(void);
static void         Elogin_TranslateEvents(XEvent * events);
char               *Elogin_NukeBoringEvents(XEvent * ev, int num);
static void         Elogin_HandleEvent(void);
static void         Elogin_AddEvent(Eevent_Type type, void *event,
				    void (*ev_free) (void *event));
static void         Elogin_FreeEvent(void *event);
static void         Elogin_HandleButtonPress(XEvent * xev);
static void         Elogin_CleanQuit(void);
char                throw_move_events_away = 0;

Eevent             *events = NULL;
Eevent             *last_event = NULL;

static void
Elogin_CleanQuit(void)
{
   exit(0);
}

void
Elogin_EventLoop(void)
{

   /* Wait for events */
   for (;;)
     {
	usleep(1);
	Elogin_SetupEvents();
     }
}

static void
Elogin_SetupEvents(void)
{
   XEvent             *events = NULL;
   int                 i;
   int                 num_events = 0, size_events = 0;
   char               *ok;

   e_flush();
   while (e_events_pending())
     {
	num_events++;
	if (num_events > size_events)
	  {
	     size_events += 64;
	     if (events)
	       {
		  REALLOC(events, XEvent, size_events);
	       }
	     else
		events = NEW(XEvent, size_events);
	  }
	e_get_next_event(&(events[num_events - 1]));
     }

   ok = Elogin_NukeBoringEvents(events, num_events);
   if (ok)
     {
	for (i = 0; i < num_events; i++)
	  {
	     if (ok[i])
	       {
//                              printf("ok\n");
		  Elogin_TranslateEvents(&(events[i]));
	       }
	  }
	FREE(ok);
	FREE(events);
     }

/*	if (events)
	{
		Elogin_TranslateEvents(events, num_events);
		FREE(events);
	}
*/

/* IMPORTANT heh */
   e_flush();
   events = NULL;
   return;
}

char               *
Elogin_NukeBoringEvents(XEvent * ev, int num)
{
   char               *ok;
   int                 i;
   int                 last;

   if (!num)
      return NULL;

   ok = NEW(ok, num);

   for (i = 0; i < num; i++)
     {
	ok[i] = 1;
     }

   last = -1;
   for (i = 0; i < num; i++)
     {
	if ((ev[i].type == XEV_MOUSE_MOVE))
	  {
//                      printf("yeah\n");
	     ok[i] = 0;
	     last = i;
	  }
     }

/* I dunno why mandrake uses this.. it doesnt effect anything afaik */
   if ((last >= 0) && (!throw_move_events_away))
      ok[last] = 1;
   throw_move_events_away = 0;

   return ok;
}

static void
Elogin_TranslateEvents(XEvent * events)
{
//      printf("Hey we got to translate an event\n");
   switch (events->type)
     {
     case XEV_BUTTON_PRESS:
	{
	   Elogin_HandleButtonPress(events);
	   Elogin_HandleEvent();
	}
	break;
     case XEV_IN_OUT:
	break;
     default:
	break;
     }
}

static void
Elogin_HandleEvent(void)
{
   if (((events && last_event) && (events == last_event)) ||
       ((events && last_event) && (last_event->next != last_event)))
     {
//      printf("hey we got to handle an event\n");
	switch (last_event->type)
	  {
	  case EV_MOUSE_DOWN:
	     {
		printf("Button %i was pressed at coords: X=%i Y=%i\n",
		       ((Ev_Mouse_Down *) last_event->event)->button,
		       ((Ev_Mouse_Down *) last_event->event)->x,
		       ((Ev_Mouse_Down *) last_event->event)->y);

		if (((Ev_Mouse_Down *) last_event->event)->button == 1)
		   Elogin_CleanQuit();
	     }
	     break;
	  case EV_MOUSE_IN:
	     break;
	  case EV_MOUSE_OUT:
	     break;
	  default:
	     break;
	  }
     }
}

static void
Elogin_AddEvent(Eevent_Type type, void *event, void (*ev_free) (void *event))
{
   Eevent             *ev;

   ev = NEW(Eevent, 1);
   ev->type = type;
   ev->event = event;
   ev->next = NULL;
   ev->ev_free = ev_free;
   if (!events)
      events = ev;
   else
      last_event->next = ev;
   last_event = ev;
}

static void
Elogin_FreeEvent(void *event)
{
   FREE(event);
}

static void
Elogin_HandleButtonPress(XEvent * xev)
{
   Ev_Mouse_Down      *e;

   e = NEW(Ev_Mouse_Down, 1);
   e->win = xev->xbutton.window;
   e->root = xev->xbutton.root;
   e->button = xev->xbutton.button;
   e->x = xev->xbutton.x;
   e->y = xev->xbutton.y;
   e->rx = xev->xbutton.x_root;
   e->ry = xev->xbutton.y_root;

   Elogin_AddEvent(EV_MOUSE_DOWN, e, Elogin_FreeEvent);
}
