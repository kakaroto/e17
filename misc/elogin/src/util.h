/***************************************************************************
                          util.h  -  description
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
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define START_LIST_DEL(type, base, cmp) \
type *_p, *_pp; _pp = NULL; _p = (base); while(_p) { if (cmp) { \
if (_pp) _pp->next = _p->next; else (base) = _p->next;
#define END_LIST_DEL \
return; } _pp = _p; _p = _p->next; }
