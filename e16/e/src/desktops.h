/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison,
 *                         and various contributors
 * Copyright (C) 2004-2005 Kim Woelders
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
#ifndef _DESKTOPS_H_
#define _DESKTOPS_H_

#include "eobj.h"

typedef struct _desk Desk;
struct _button;

struct _desk
{
   EObj                o;
   unsigned int        num;
   char                viewable;
   char                bg_isset;
   Background         *bg;
   struct _button     *tag;
   int                 current_area_x;
   int                 current_area_y;
   long                event_mask;
   Pixmap              pmap;
   struct
   {
      int                 dirty;
      EObj               *latest;
      char                update_client_list;
   } stack;
};

/* desktops.c */
Desk               *DeskGet(unsigned int desk);
Desk               *DeskGetRelative(Desk * dsk, int inc);
Background         *DeskGetBackground(const Desk * dsk);
void                DeskGetArea(const Desk * dsk, int *ax, int *ay);
void                DeskSetArea(Desk * dsk, int ax, int ay);
int                 DeskIsViewable(const Desk * dsk);
void                DeskSetDirtyStack(Desk * dsk, EObj * eo);

void                DeskRefresh(Desk * dsk);
void                DeskAssignBg(unsigned int desk, Background * bg);
void                DeskSetBg(Desk * dsk, Background * bg, int refresh);
Desk               *DesktopAt(int x, int y);
void                DeskSwitchStart(void);
void                DeskSwitchDone(void);
void                DeskGoto(Desk * dsk);
void                DeskGotoNum(unsigned int desk);
void                DeskGotoByEwin(EWin * ewin);

void                DeskRestack(Desk * dsk);

unsigned int        DesksGetNumber(void);
Desk               *DesksGetCurrent(void);
unsigned int        DesksGetCurrentNum(void);
void                DesksSetCurrent(Desk * dsk);
void                DeskGetCurrentArea(int *ax, int *ay);
void                DeskSetCurrentArea(int ax, int ay);
void                DesksClear(void);

#endif /* _DESKTOPS_H_ */
