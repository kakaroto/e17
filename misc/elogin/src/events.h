/***************************************************************************
                          events.h  -  description
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
#ifndef E_EVENTS_H
#define E_EVENTS_H 1

#include "x.h"

typedef struct _eev Eevent;

enum _eev_type
{
   EV_MOUSE_MOVE,
   EV_MOUSE_IN,
   EV_MOUSE_OUT,
   EV_MOUSE_DOWN
};

typedef enum _eev_type Eevent_Type;

struct _eev
{
   Eevent_Type         type;
   void               *event;
   void                (*ev_free) (void *evnt);
   Eevent             *next;
};

typedef struct _ev_mouse_down Ev_Mouse_Down;

struct _ev_mouse_down
{
   Window              root, win;
   int                 button;
   int                 x, y;
   int                 rx, ry;
};

void                Elogin_EventLoop(void);

#endif
