/*
 * Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
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

void
DockIt(EWin * ewin)
{
   ActionClass        *ac;
   ImageClass         *ic;
   Button             *bt;
   char                id[32];

   EDBUG(3, "DockIt");
   Esnprintf(id, sizeof(id), "%i", ewin->client.win);
   ac = 0;
   ic = FindItem("DEFAULT_DOCK_BUTTON", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (mode.dockstartx >= 0)
     {
	bt = CreateButton(id, ic, ac, NULL, NULL, 1, 0, 64, 64, 64, 64, 0, 0,
			  mode.dockstartx, 0, mode.dockstarty, 0, 0, 0, 0, 0, 1,
			  0, 1);
     }
   else
     {
	bt = CreateButton(id, ic, ac, NULL, NULL, 1, 0, 64, 64, 64, 64, 0, 2046,
			  0, 0, 0, 1023, 0, 0, 0, 0, 1, 0, 1);
     }
   UngrabX();

   if (!bt)
      EDBUG_RETURN_;

   ShowButton(bt);
   FindEmptySpotForButton(bt, "DOCK_APP_BUTTON", mode.dockdirmode);

   AddItem(ewin, "EWIN", ewin->client.win, LIST_TYPE_EWIN);
   AddItem(bt, "DOCK_APP_BUTTON", ewin->client.win, LIST_TYPE_BUTTON);

   EmbedWindowIntoButton(bt, ewin->client.win);
   ShowEwin(ewin);

   EDBUG_RETURN_;
}

void
DockDestroy(EWin * ewin)
{

   Button             *bt;

   EDBUG(3, "DockDestroy");
   bt = FindItem(NULL, ewin->client.win, LIST_FINDBY_ID, LIST_TYPE_BUTTON);
   if (bt)
      DestroyButton(RemoveItem
		    (NULL, ewin->client.win, LIST_FINDBY_ID, LIST_TYPE_BUTTON));
   EDBUG_RETURN_;
}
