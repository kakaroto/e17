/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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
/*
 * Author: Merlin Hughes
 *  - merlin@merlin.org
 *
 *  This code is free software.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 *
 *  TODO: The Warp Focus Title Window could list all windows in the
 *  warp ring with pretty window icons, and display which one is
 *  currently selected instead of just the title of the currently
 *  selected one.
 */

#include "E.h"

static void         WarpFocusShowTitle(EWin * ewin);
static void         WarpFocusHideTitle(void);

static int          warpFocusIndex = 0;
static char         warpFocusTitleShowing = 0;
static Window       warpFocusTitleWindow = 0;
static int          warptitles_num = 0;
static Window      *warptitles = NULL;
static EWin       **warptitles_ewin = NULL;
static unsigned int warpFocusKey = 0;

int
WarpFocusHandleEvent(XEvent * ev)
{
   EDBUG(5, "WarpFocusHandleEvent");

   if (!Conf.warplist.enable)
      EDBUG_RETURN(0);

   if (ev->type != KeyPress && ev->type != KeyRelease)
      EDBUG_RETURN(0);

#if 0				/* Debug */
   Eprintf("WarpFocusHandleEvent win=%#x key=%#x(%#x) %d\n",
	   (unsigned)ev->xkey.window, ev->xkey.keycode,
	   warpFocusKey, warpFocusTitleShowing);
#endif
   if (warpFocusTitleShowing)
     {
	if (ev->xkey.keycode != warpFocusKey)
	   WarpFocusFinish();
     }
   else
     {
	warpFocusKey = ev->xkey.keycode;
     }

   EDBUG_RETURN(0);
}

void
WarpFocus(int delta)
{
   EWin               *const *lst0;
   EWin              **lst, *ewin;
   int                 i, num0, num;

   EDBUG(5, "WarpFocus");

   if (!Conf.warplist.enable)
      EDBUG_RETURN_;

   lst = (EWin **) ListItemType(&num, LIST_TYPE_WARP_RING);
   if (!lst)
     {
	lst0 = EwinListGetFocus(&num0);
	num = 0;
	lst = NULL;
	for (i = num0 - 1; i >= 0; --i)
	  {
	     ewin = lst0[i];
	     if (		/* Either visible or iconified */
		   ((EwinIsOnScreen(ewin)) || (ewin->iconified))
		   /* Exclude windows that explicitely say so */
		   && (!ewin->skipfocus)
		   /* Keep shaded windows if conf say so */
		   && ((!ewin->shaded) || (Conf.warplist.warpshaded))
		   /* Keep sticky windows if conf say so */
		   && ((!ewin->sticky) || (Conf.warplist.warpsticky))
		   /* Keep iconified windows if conf say so */
		   && ((!ewin->iconified) || (Conf.warplist.warpiconified))
		   /*&& (ewin->client.mwm_decor_title) &&
		    * (ewin->client.mwm_decor_border) */
		)
		AddItem(ewin, "", 0, LIST_TYPE_WARP_RING);
	  }
	MoveItemToListBottom(GetFocusEwin(), LIST_TYPE_WARP_RING);
	lst = (EWin **) ListItemType(&num, LIST_TYPE_WARP_RING);
	warpFocusIndex = num - 1;
     }

   if (lst)
     {
	warpFocusIndex = (warpFocusIndex + num + delta) % num;
	ewin = lst[warpFocusIndex];
	if (!FindItem((char *)ewin, 0, LIST_FINDBY_POINTER, LIST_TYPE_EWIN))
	   ewin = NULL;
	if (ewin)
	   FocusToEWin(ewin, FOCUS_WARP_NEXT);
	WarpFocusShowTitle(ewin);
	Efree(lst);
     }

   EDBUG_RETURN_;
}

void
WarpFocusFinish(void)
{
   EWin              **lst, *ewin;
   int                 num;

   EDBUG(5, "WarpFocusFinish");

   lst = (EWin **) ListItemType(&num, LIST_TYPE_WARP_RING);
   if (lst)
     {
	ewin = lst[warpFocusIndex];

	WarpFocusHideTitle();
	if (!FindItem((char *)ewin, 0, LIST_FINDBY_POINTER, LIST_TYPE_EWIN))
	   ewin = NULL;
	if (ewin)
	  {
	     if (Conf.warplist.warpiconified && ewin->iconified)
		DeIconifyEwin(ewin);
	     FocusToEWin(ewin, FOCUS_WARP_DONE);
	  }
	Efree(lst);

	while (RemoveItem("", 0, LIST_FINDBY_NONE, LIST_TYPE_WARP_RING));
     }

   EDBUG_RETURN_;
}

static void
WarpFocusShowTitle(EWin * ewin)
{
   TextClass          *tc;
   ImageClass         *ic;
   char                pq;
   int                 i, x, y, w, h, num, ww, hh;
   static int          mw, mh;

   tc = FindItem("WARPFOCUS", 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
   if (!tc)
      tc = FindItem("COORDS", 0, LIST_FINDBY_NAME, LIST_TYPE_TCLASS);
   ic = FindItem("WARPFOCUS", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (!ic)
      ic = FindItem("COORDS", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if ((!ic) || (!tc))
      return;

   if (!warpFocusTitleWindow)
      warpFocusTitleWindow = ECreateWindow(root.win, 0, 0, 1, 1, 1);

   pq = queue_up;
   queue_up = 0;
   XRaiseWindow(disp, warpFocusTitleWindow);

   if (!warpFocusTitleShowing)
     {
	EWin              **lst;

	w = 0;
	h = 0;
	lst = (EWin **) ListItemType(&num, LIST_TYPE_WARP_RING);
	if (lst)
	  {
	     for (i = 0; i < num; i++)
	       {
		  warptitles_num++;
		  warptitles =
		     Erealloc(warptitles, (sizeof(Window) * warptitles_num));
		  warptitles_ewin =
		     Erealloc(warptitles_ewin,
			      (sizeof(EWin *) * warptitles_num));
		  warptitles[warptitles_num - 1] =
		     ECreateWindow(warpFocusTitleWindow, 0, 0, 1, 1, 0);
		  EMapWindow(disp, warptitles[warptitles_num - 1]);
		  warptitles_ewin[warptitles_num - 1] = lst[i];
		  TextSize(tc, 0, 0, 0, EwinGetTitle(lst[i]), &ww, &hh, 17);
		  if (ww > w)
		     w = ww;
		  if (hh > h)
		     h = hh;
	       }
	     Efree(lst);
	  }
	w += (ic->padding.left + ic->padding.right);
	h += (ic->padding.top + ic->padding.bottom);
	GetPointerScreenGeometry(&x, &y, &ww, &hh);
	x += (ww - w) / 2;
	y += (hh - h * warptitles_num) / 2;
	mw = w;
	mh = h;
	EMoveResizeWindow(disp, warpFocusTitleWindow, x, y, w,
			  (h * warptitles_num));
	for (i = 0; i < warptitles_num; i++)
	  {
	     EMoveResizeWindow(disp, warptitles[i], 0, (h * i), mw, mh);
	     if (ewin == warptitles_ewin[i])
		IclassApply(ic, warptitles[i], mw, mh, 0, 0, STATE_CLICKED, 0);
	     else
		IclassApply(ic, warptitles[i], mw, mh, 0, 0, STATE_NORMAL, 0);
	  }
	PropagateShapes(warpFocusTitleWindow);
	EMapWindow(disp, warpFocusTitleWindow);

	/*
	 * Grab the keyboard. The grab is automatically released when
	 * WarpFocusHideTitle unmaps warpFocusTitleWindow.
	 */
	XGrabKeyboard(disp, warpFocusTitleWindow, False, GrabModeAsync,
		      GrabModeAsync, CurrentTime);
     }

   for (i = 0; i < warptitles_num; i++)
     {
	if (!FindItem((char *)warptitles_ewin[i], 0, LIST_FINDBY_POINTER,
		      LIST_TYPE_EWIN))
	   warptitles_ewin[i] = NULL;
	if (warptitles_ewin[i])
	  {
	     int                 state =
		(ewin == warptitles_ewin[i]) ? STATE_CLICKED : STATE_NORMAL;

	     IclassApply(ic, warptitles[i], mw, mh, 0, 0, state, 0);
	     TclassApply(ic, warptitles[i], mw, mh, 0, 0, state, 0,
			 tc, EwinGetTitle(warptitles_ewin[i]));
	  }
     }

   PropagateShapes(warpFocusTitleWindow);
   queue_up = pq;
   XFlush(disp);
   warpFocusTitleShowing = 1;
}

static void
WarpFocusHideTitle(void)
{
   int                 i;

   if (warpFocusTitleWindow)
     {
	EUnmapWindow(disp, warpFocusTitleWindow);
	for (i = 0; i < warptitles_num; i++)
	   EDestroyWindow(disp, warptitles[i]);
     }

   if (warptitles)
      Efree(warptitles);
   if (warptitles_ewin)
      Efree(warptitles_ewin);
   warptitles = NULL;
   warptitles_ewin = NULL;
   warptitles_num = 0;
   warpFocusTitleShowing = 0;
}
