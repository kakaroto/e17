/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "icons.h"
#include <X11/keysym.h>

typedef struct
{
   EWin               *ewin;
   Window              win;
   char               *txt;
} WarplistItem;

static void         WarpFocusHandleEvent(XEvent * ev, void *prm);

static EObj        *warpFocusWindow = NULL;

static int          warpFocusIndex = 0;
static unsigned int warpFocusKey = 0;
static int          warplist_num = 0;
static WarplistItem *warplist;

#define ICON_PAD 2

static void
WarpFocusShow(EWin * ewin)
{
   TextClass          *tc;
   ImageClass         *ic;
   int                 i, x, y, w, h, num, ww, hh;
   static int          mw, mh, tw, th;
   char                s[1024];
   WarplistItem       *wl;

   tc = TextclassFind("WARPFOCUS", 0);
   if (!tc)
      tc = TextclassFind("COORDS", 1);

   ic = ImageclassFind("WARPFOCUS", 0);
   if (!ic)
      ic = ImageclassFind("COORDS", 1);

   if ((!ic) || (!tc))
      return;

   if (!warpFocusWindow)
     {
	EObj               *eo;

	eo = EobjWindowCreate(EOBJ_TYPE_MISC, 0, 0, 1, 1, 1, "Warp");
	if (!eo)
	   return;

	warpFocusWindow = eo;

	EventCallbackRegister(eo->win, 0, WarpFocusHandleEvent, NULL);

	TooltipsEnable(0);
     }

   if (!warpFocusWindow->shown)
     {
	EWin              **lst;

	w = 0;
	h = 0;
	lst = (EWin **) ListItemType(&num, LIST_TYPE_WARP_RING);
	if (lst)
	  {
	     for (i = 0; i < num; i++)
	       {
		  warplist_num++;
		  warplist = Erealloc(warplist,
				      warplist_num * sizeof(WarplistItem));
		  wl = warplist + warplist_num - 1;
		  wl->win = ECreateWindow(warpFocusWindow->win, 0, 0, 1, 1, 0);
		  EMapWindow(wl->win);
		  wl->ewin = lst[i];
		  Esnprintf(s, sizeof(s), (lst[i]->iconified) ? "[%s]" : "%s",
			    EwinGetName(lst[i]));
		  wl->txt = strdup(s);
		  TextSize(tc, 0, 0, 0, warplist[warplist_num - 1].txt, &ww,
			   &hh, 17);
		  if (ww > w)
		     w = ww;
		  if (hh > h)
		     h = hh;
	       }
	     Efree(lst);
	  }

	tw = w;			/* Text size */
	th = h;
	w += (ic->padding.left + ic->padding.right);
	h += (ic->padding.top + ic->padding.bottom);
	if (Conf.warplist.icon_mode != 0)
	   w += h;
	mw = w;			/* Focus list item size */
	mh = h;

	GetPointerScreenAvailableArea(&x, &y, &ww, &hh);
	x += (ww - w) / 2;
	y += (hh - h * warplist_num) / 2;
	EobjMoveResize(warpFocusWindow, x, y, w, h * warplist_num);

	for (i = 0; i < warplist_num; i++)
	  {
	     EMoveResizeWindow(warplist[i].win, 0, (h * i), mw, mh);
	     if (ewin == warplist[i].ewin)
		ImageclassApply(ic, warplist[i].win, mw, mh, 0, 0,
				STATE_CLICKED, 0, ST_WARPLIST);
	     else
		ImageclassApply(ic, warplist[i].win, mw, mh, 0, 0, STATE_NORMAL,
				0, ST_WARPLIST);
	  }

	EShapePropagate(warpFocusWindow->win);
	EobjMap(warpFocusWindow, 0);

	/*
	 * Grab the keyboard. The grab is automatically released when
	 * WarpFocusHide unmaps warpFocusWindow.
	 */
	XGrabKeyboard(disp, warpFocusWindow->win, False, GrabModeAsync,
		      GrabModeAsync, CurrentTime);
     }

   for (i = 0; i < warplist_num; i++)
     {
	wl = warplist + i;

	if (!FindItem((char *)wl->ewin, 0, LIST_FINDBY_POINTER, LIST_TYPE_EWIN))
	   wl->ewin = NULL;
	if (wl->ewin)
	  {
	     int                 state;

	     state = (ewin == wl->ewin) ? STATE_CLICKED : STATE_NORMAL;

	     ImageclassApply(ic, wl->win, mw, mh, 0, 0, state, 0, ST_WARPLIST);

	     /* New icon stuff */
	     if (Conf.warplist.icon_mode != 0)
	       {
		  int                 icon_size = mh - 2 * ICON_PAD;
		  Imlib_Image        *im;

		  TextDraw(tc, wl->win, 0, 0, state, wl->txt,
			   ic->padding.left + mh, ic->padding.top,
			   tw, th, 0, 0);

		  im = EwinIconImageGet(wl->ewin, icon_size,
					Conf.warplist.icon_mode);
		  if (!im)
		     continue;

		  imlib_context_set_image(im);
		  imlib_context_set_drawable(wl->win);
		  imlib_context_set_blend(1);
		  imlib_render_image_on_drawable_at_size(ic->padding.left +
							 ICON_PAD, ICON_PAD,
							 icon_size, icon_size);
		  imlib_free_image();
		  imlib_context_set_blend(0);
	       }
	     else
	       {
		  TextclassApply(ic, wl->win, mw, mh, 0, 0, state, 0,
				 tc, wl->txt);
	       }
	  }
     }

   EShapePropagate(warpFocusWindow->win);
   EFlush();
}

static void
WarpFocusHide(void)
{
   int                 i;

   if (warpFocusWindow && warpFocusWindow->shown)
     {
	EobjUnmap(warpFocusWindow);
	for (i = 0; i < warplist_num; i++)
	  {
	     EDestroyWindow(warplist[i].win);
	     Efree(warplist[i].txt);
	  }
#if 0				/* We might as well keep it around */
	EventCallbackUnregister(warpFocusWindow->win, 0, WarpFocusHandleEvent,
				NULL);
	EobjWindowDestroy(warpFocusWindow);
	warpFocusWindow = None;
#endif

	TooltipsEnable(1);
     }

   if (warplist)
      Efree(warplist);
   warplist = NULL;
   warplist_num = 0;
}

void
WarpFocus(int delta)
{
   EWin               *const *lst0;
   EWin              **lst, *ewin;
   int                 i, num0, num;

   /* Remember invoking keycode (ugly hack) */
   if (!warpFocusWindow || !warpFocusWindow->shown)
      warpFocusKey = Mode.last_keycode;

   lst = (EWin **) ListItemType(&num, LIST_TYPE_WARP_RING);
   if (!lst)
     {
	lst0 = EwinListFocusGet(&num0);
	num = 0;
	lst = NULL;
	for (i = num0 - 1; i >= 0; --i)
	  {
	     ewin = lst0[i];
	     if (		/* Either visible or iconified */
		   ((EwinIsOnScreen(ewin)) || (ewin->iconified)) &&
		   /* Exclude windows that explicitely say so */
		   (!ewin->skipfocus) &&
		   /* Keep shaded windows if conf say so */
		   ((!ewin->shaded) || (Conf.warplist.showshaded)) &&
		   /* Keep sticky windows if conf say so */
		   ((!EoIsSticky(ewin)) || (Conf.warplist.showsticky)) &&
		   /* Keep iconified windows if conf say so */
		   ((!ewin->iconified) || (Conf.warplist.showiconified)))
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
	  {
	     if (Conf.focus.raise_on_next)
		RaiseEwin(ewin);
	     if (Conf.focus.warp_on_next)
		if (ewin != Mode.mouse_over_ewin && !ewin->iconified)
		   XWarpPointer(disp, None, EoGetWin(ewin), 0, 0, 0, 0,
				EoGetW(ewin) / 2, EoGetH(ewin) / 2);
	     if (Conf.warplist.warpfocused)
		FocusToEWin(ewin, FOCUS_SET);
	     WarpFocusShow(ewin);
	  }
	Efree(lst);
     }
}

static void
WarpFocusFinish(void)
{
   EWin              **lst, *ewin;
   int                 num;

   lst = (EWin **) ListItemType(&num, LIST_TYPE_WARP_RING);
   if (!lst)
      return;

   ewin = lst[warpFocusIndex];

   WarpFocusHide();
   if (!FindItem((char *)ewin, 0, LIST_FINDBY_POINTER, LIST_TYPE_EWIN))
      ewin = NULL;
   if (ewin)
     {
	if (Conf.warplist.showiconified && ewin->iconified)
	   EwinDeIconify(ewin);
	if (Conf.warplist.raise_on_select)
	   RaiseEwin(ewin);
	if (Conf.warplist.warp_on_select)
	   if (ewin != Mode.mouse_over_ewin)
	      XWarpPointer(disp, None, EoGetWin(ewin), 0, 0, 0, 0,
			   EoGetW(ewin) / 2, EoGetH(ewin) / 2);
	FocusToEWin(ewin, FOCUS_SET);
     }
   Efree(lst);

   while (RemoveItem("", 0, LIST_FINDBY_NONE, LIST_TYPE_WARP_RING));
}

static void
WarpFocusHandleEvent(XEvent * ev, void *prm __UNUSED__)
{
   switch (ev->type)
     {
#if 0				/* Not necessary when sampling keycode in events.c */
     case KeyPress:
	if (warpFocusWindow->shown && ev->xany.window == VRoot.win)
	   warpFocusKey = ev->xkey.keycode;
	break;
#endif
     case KeyRelease:
	if (warpFocusWindow->shown && ev->xkey.keycode != warpFocusKey)
	   WarpFocusFinish();
	break;
     }
}

static void
WarplistInit(void)
{
#if 0				/* Not necessary when sampling keycode in events.c */
   /* Ugly hack to get the invoking key press */
   EventCallbackRegister(VRoot.win, 0, WarpFocusHandleEvent, NULL);
#endif
}

/*
 * Warplist module
 */

static void
WarplistCfgValidate(void)
{
   if (Conf.warplist.icon_mode < 0 || Conf.warplist.icon_mode > 3)
      Conf.warplist.icon_mode = 3;
}

static void
WarplistSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	WarplistInit();
	WarplistCfgValidate();
	break;
     }
}

static const CfgItem WarplistCfgItems[] = {
   CFG_ITEM_INT(Conf.warplist, icon_mode, 3),
   CFG_ITEM_BOOL(Conf.warplist, enable, 1),
   CFG_ITEM_BOOL(Conf.warplist, showsticky, 1),
   CFG_ITEM_BOOL(Conf.warplist, showshaded, 1),
   CFG_ITEM_BOOL(Conf.warplist, showiconified, 0),
   CFG_ITEM_BOOL(Conf.warplist, warpfocused, 1),
   CFG_ITEM_BOOL(Conf.warplist, raise_on_select, 1),
   CFG_ITEM_BOOL(Conf.warplist, warp_on_select, 0),
};
#define N_CFG_ITEMS (sizeof(WarplistCfgItems)/sizeof(CfgItem))

EModule             ModWarplist = {
   "warplist", "warp",
   WarplistSighan,
   {0, NULL},
   {N_CFG_ITEMS, WarplistCfgItems}
};
