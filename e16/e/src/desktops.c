/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2008 Kim Woelders
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
#include "aclass.h"
#include "backgrounds.h"
#include "buttons.h"
#include "desktops.h"
#include "dialog.h"
#include "ecompmgr.h"
#include "emodule.h"
#include "eobj.h"
#include "events.h"
#include "ewins.h"
#include "focus.h"
#include "grabs.h"
#include "hints.h"
#include "iclass.h"
#include "screen.h"
#include "settings.h"
#include "timers.h"
#include "tooltips.h"
#include "xwin.h"
#include <time.h>
#if USE_XRANDR
#include <X11/extensions/Xrandr.h>
#endif

#define EDESK_EVENT_MASK \
  (ButtonPressMask | ButtonReleaseMask | \
   EnterWindowMask | LeaveWindowMask /* | PointerMotionMask | ButtonMotionMask */ | \
   SubstructureNotifyMask | SubstructureRedirectMask | PropertyChangeMask)

#define ENLIGHTENMENT_CONF_NUM_DESKTOPS 32

typedef struct {
   Desk               *current;
   Desk               *previous;
   Desk               *desk[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
   unsigned int        order[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
   int                 drag_x0, drag_y0;
} Desktops;

static void         DeskRaise(unsigned int num);
static void         DeskLower(unsigned int num);
static void         DeskHandleEvents(Win win, XEvent * ev, void *prm);
static void         DeskButtonCallback(EObj * eo, XEvent * ev,
				       ActionClass * ac);

/* The desktops */
static Desktops     desks;

#define _DeskGet(d) (desks.desk[d])

static void
DeskControlsCreate(Desk * dsk)
{
   char                s[512];
   const char         *ic1, *ic2, *ic3;

#if ENABLE_DESKRAY
   const char         *ic4;
#endif
   char                ac1[64], ac2[64], ac3[64];
   Button             *b;
   ActionClass        *ac;
   Action             *a;
   int                 x[3], y[3], w[3], h[3], m, n, o;
   const char         *t;

   if (Conf.desks.dragdir < 0 || Conf.desks.dragdir > 3)
      Conf.desks.dragdir = 2;
   if (Conf.desks.dragbar_ordering < 0 || Conf.desks.dragbar_ordering > 5)
      Conf.desks.dragbar_ordering = 1;
   if (Conf.desks.dragbar_width < 0)
      Conf.desks.dragbar_width = 0;
   else if (Conf.desks.dragbar_width > 64)
      Conf.desks.dragbar_width = 64;
   if (Conf.desks.dragbar_length < 0)
      Conf.desks.dragbar_length = 0;
   else if (Conf.desks.dragbar_length > WinGetW(VROOT))
      Conf.desks.dragbar_length = WinGetW(VROOT);

   Esnprintf(ac1, sizeof(ac1), "DRAGBAR_DESKTOP_%i", dsk->num);
   if (!ActionclassFind(ac1))
     {
	ac = ActionclassCreate(ac1, 0);
	a = ActionCreate(EVENT_MOUSE_DOWN, 0, 0, 0, 1, 0, NULL, NULL);
	ActionclassAddAction(ac, a);

	Esnprintf(s, sizeof(s), "desk drag %i", dsk->num);
	ActionAddTo(a, s);

	a = ActionCreate(EVENT_MOUSE_DOWN, 0, 0, 0, 3, 0, NULL, NULL);
	ActionclassAddAction(ac, a);
	ActionAddTo(a, "menus show deskmenu");

	a = ActionCreate(EVENT_MOUSE_DOWN, 0, 0, 0, 2, 0, NULL, NULL);
	ActionclassAddAction(ac, a);
	ActionAddTo(a, "menus show windowlist");

	if (dsk->num > 0)
	  {
	     t = _("Hold down the mouse button and drag\n"
		   "the mouse to be able to drag the desktop\n"
		   "back and forth.\n"
		   "Click right mouse button for a list of all\n"
		   "Desktops and their applications.\n"
		   "Click middle mouse button for a list of all\n"
		   "applications currently running.\n");
	     ActionclassSetTooltipString(ac, t);
	  }
	else
	  {
	     t = _("This is the Root desktop.\n"
		   "You cannot drag the root desktop around.\n"
		   "Click right mouse button for a list of all\n"
		   "Desktops and their applications.\n"
		   "Click middle mouse button for a list of all\n"
		   "applications currently running.\n");
	     ActionclassSetTooltipString(ac, t);
	  }
     }

   Esnprintf(ac2, sizeof(ac2), "RAISEBUTTON_DESKTOP_%i", dsk->num);
   if (!ActionclassFind(ac2))
     {
	ac = ActionclassCreate(ac2, 0);
	a = ActionCreate(EVENT_MOUSE_UP, 1, 0, 1, 0, 0, NULL, NULL);
	ActionclassAddAction(ac, a);

	Esnprintf(s, sizeof(s), "desk raise %i", dsk->num);
	ActionAddTo(a, s);
	t = _("Click here to raise this desktop\nto the top.\n");
	ActionclassSetTooltipString(ac, t);
     }

   Esnprintf(ac3, sizeof(ac3), "LOWERBUTTON_DESKTOP_%i", dsk->num);
   if (!ActionclassFind(ac3))
     {
	ac = ActionclassCreate(ac3, 0);
	a = ActionCreate(EVENT_MOUSE_UP, 1, 0, 1, 0, 0, NULL, NULL);
	ActionclassAddAction(ac, a);

	Esnprintf(s, sizeof(s), "desk lower %i", dsk->num);
	ActionAddTo(a, s);
	t = _("Click here to lower this desktop\nto the bottom.\n");
	ActionclassSetTooltipString(ac, t);
     }

   if (Conf.desks.dragdir < 2)
     {
	ic1 = "DESKTOP_DRAGBUTTON_VERT";
	ic2 = "DESKTOP_RAISEBUTTON_VERT";
	ic3 = "DESKTOP_LOWERBUTTON_VERT";
#if ENABLE_DESKRAY
	ic4 = "DESKTOP_DESKRAY_VERT";
#endif
     }
   else
     {
	ic1 = "DESKTOP_DRAGBUTTON_HORIZ";
	ic2 = "DESKTOP_RAISEBUTTON_HORIZ";
	ic3 = "DESKTOP_LOWERBUTTON_HORIZ";
#if ENABLE_DESKRAY
	ic4 = "DESKTOP_DESKRAY_HORIZ";
#endif
     }

   switch (Conf.desks.dragbar_ordering)
     {
     case 0:
	m = 0;
	n = 1;
	o = 2;
	break;
     case 1:
	m = 0;
	n = 2;
	o = 1;
	break;
     case 2:
	m = 2;
	n = 0;
	o = 1;
	break;
     case 3:
	m = 1;
	n = 0;
	o = 2;
	break;
     case 4:
	m = 1;
	n = 2;
	o = 0;
	break;
     case 5:
	m = 2;
	n = 1;
	o = 0;
	break;
     default:
	m = 0;
	n = 1;
	o = 2;
	break;
     }

   switch (Conf.desks.dragdir)
     {
     default:
     case 0:
	w[0] = w[1] = w[2] = h[0] = h[1] = Conf.desks.dragbar_width;
	if (Conf.desks.dragbar_length == 0)
	   h[2] = WinGetH(VROOT) - (Conf.desks.dragbar_width * 2);
	else
	   h[2] = Conf.desks.dragbar_length;
	x[0] = x[1] = x[2] = 0;
	y[m] = 0;
	y[n] = y[m] + h[m];
	y[o] = y[n] + h[n];
	break;
     case 1:
	w[0] = w[1] = w[2] = h[0] = h[1] = Conf.desks.dragbar_width;
	if (Conf.desks.dragbar_length == 0)
	   h[2] = WinGetH(VROOT) - (Conf.desks.dragbar_width * 2);
	else
	   h[2] = Conf.desks.dragbar_length;
	x[0] = x[1] = x[2] = WinGetW(VROOT) - Conf.desks.dragbar_width;
	y[m] = 0;
	y[n] = y[m] + h[m];
	y[o] = y[n] + h[n];
	break;
     case 2:
	h[0] = h[1] = h[2] = w[0] = w[1] = Conf.desks.dragbar_width;
	if (Conf.desks.dragbar_length == 0)
	   w[2] = WinGetW(VROOT) - (Conf.desks.dragbar_width * 2);
	else
	   w[2] = Conf.desks.dragbar_length;
	y[0] = y[1] = y[2] = 0;
	x[m] = 0;
	x[n] = x[m] + w[m];
	x[o] = x[n] + w[n];
	break;
     case 3:
	h[0] = h[1] = h[2] = w[0] = w[1] = Conf.desks.dragbar_width;
	if (Conf.desks.dragbar_length == 0)
	   w[2] = WinGetW(VROOT) - (Conf.desks.dragbar_width * 2);
	else
	   w[2] = Conf.desks.dragbar_length;
	y[0] = y[1] = y[2] = WinGetH(VROOT) - Conf.desks.dragbar_width;
	x[m] = 0;
	x[n] = x[m] + w[m];
	x[o] = x[n] + w[n];
	break;
     }

   b = NULL;

   if (Conf.desks.dragbar_width > 0)
     {
	b = ButtonCreate("_DESKTOP_DRAG_CONTROL", 1, ic2, ac2, NULL, NULL,
			 -1, FLAG_FIXED, 1, 99999, 1, 99999, 0, 0, x[0], 0,
			 y[0], 0, 0, w[0], 0, h[0], 0, dsk->num, 0);
	b = ButtonCreate("_DESKTOP_DRAG_CONTROL", 1, ic3, ac3, NULL, NULL,
			 -1, FLAG_FIXED, 1, 99999, 1, 99999, 0, 0, x[1], 0,
			 y[1], 0, 0, w[1], 0, h[1], 0, dsk->num, 0);
	b = ButtonCreate("_DESKTOP_DRAG_CONTROL", 1, ic1, ac1, NULL, NULL,
			 -1, FLAG_FIXED, 1, 99999, 1, 99999, 0, 0, x[2], 0,
			 y[2], 0, 0, w[2], 0, h[2], 0, dsk->num, 0);
	ButtonSetCallback(b, DeskButtonCallback, EoObj(dsk));
     }

#if ENABLE_DESKRAY
   if (dsk->num > 0)
     {
	if (Conf.desks.dragdir == 0)
	  {
	     b = ButtonCreate("_DESKTOP_DESKRAY_DRAG_CONTROL", 2, ic4, ac1,
			      NULL, NULL, 1, FLAG_FIXED_VERT, 1, 99999, 1,
			      99999, 0, 0, EoGetX(dsk), 0, EoGetY(dsk),
			      0, 0, 0, 0, 0, 1, 0, 1);
	  }
	else if (Conf.desks.dragdir == 1)
	  {
	     b = ButtonCreate("_DESKTOP_DESKRAY_DRAG_CONTROL", 2, ic4, ac1,
			      NULL, NULL, 1, FLAG_FIXED_VERT, 1, 99999, 1,
			      99999, 0, 0,
			      EoGetX(dsk) + WinGetW(VROOT) -
			      Conf.desks.dragbar_width, 0, EoGetY(dsk),
			      0, 0, 0, 0, 0, 1, 0, 1);
	  }
	else if (Conf.desks.dragdir == 2)
	  {
	     b = ButtonCreate("_DESKTOP_DESKRAY_DRAG_CONTROL", 2, ic4, ac1,
			      NULL, NULL, 1, FLAG_FIXED_HORIZ, 1, 99999, 1,
			      99999, 0, 0, EoGetX(dsk), 0, EoGetY(dsk),
			      0, 0, 0, 0, 0, 1, 0, 1);
	  }
	else
	  {
	     b = ButtonCreate("_DESKTOP_DESKRAY_DRAG_CONTROL", 2, ic4, ac1,
			      NULL, NULL, 1, FLAG_FIXED_HORIZ, 1, 99999, 1,
			      99999, 0, 0, EoGetX(dsk), 0,
			      EoGetY(dsk) + WinGetH(VROOT) -
			      Conf.desks.dragbar_width, 0, 0, 0, 0, 0, 1, 0, 1);
	  }
     }
#endif

   dsk->tag = b;
}

static void
DeskControlsDestroy(Desk * dsk, int id)
{
   ButtonsForeach(id, dsk, ButtonDestroy);
}

static void
DeskControlsShow(Desk * dsk, int id)
{
   ButtonsForeach(id, dsk, ButtonShow);
}

static void
DeskConfigure(Desk * dsk)
{
   Background         *bg;

   DeskControlsCreate(dsk);
   DeskControlsShow(dsk, 1);

   bg = BackgroundGetForDesk(dsk->num);
   DeskBackgroundSet(dsk, bg);

   if (dsk->num > 0)
     {
	EoMove(dsk, WinGetW(VROOT), 0);
	EoMap(dsk, 0);
     }

   ModulesSignal(ESIGNAL_DESK_ADDED, dsk);
}

static Desk        *
DeskCreate(int desk, int configure)
{
#if USE_COMPOSITE
   EObj               *eo;
#endif
   Desk               *dsk;
   Win                 win;
   char                buf[64];

   if (desk < 0 || desk >= ENLIGHTENMENT_CONF_NUM_DESKTOPS)
      return NULL;

   dsk = ECALLOC(Desk, 1);

   desks.desk[desk] = dsk;
   dsk->num = desk;
   desks.order[desk] = desk;

   win = (desk == 0) ? VROOT : NULL;

   Esnprintf(buf, sizeof(buf), "Desk-%d", desk);
   EoSetNoRedirect(dsk, 1);
   EoInit(dsk, EOBJ_TYPE_DESK, win,
	  0, 0, WinGetW(VROOT), WinGetH(VROOT), 0, buf);
   EventCallbackRegister(EoGetWin(dsk), 0, DeskHandleEvents, dsk);
   dsk->bg.o = EoObj(dsk);
   if (desk == 0)
     {
	desks.current = dsk;
#if !USE_BG_WIN_ON_ALL_DESKS	/* TBD - Use per virtual root bg window? */
#if USE_COMPOSITE
	/* Add background window */
	eo = EobjWindowCreate(EOBJ_TYPE_ROOT_BG,
			      0, 0, WinGetW(VROOT), WinGetH(VROOT),
			      0, "Root-bg");
	eo->floating = 0;
	EobjSetLayer(eo, 0);
	EventCallbackRegister(EobjGetWin(eo), 0, DeskHandleEvents, dsk);
	dsk->bg.o_bg = eo;
	ESelectInput(EobjGetWin(eo), EnterWindowMask);
#endif
#endif
     }
   else
     {
	EoSetFloating(dsk, 1);
	EoSetLayer(dsk, 0);
	/* Set the _XROOT... atoms so apps will find them even before the bg is set */
	HintsSetRootInfo(EoGetWin(dsk), None, 0);
     }

#if USE_BG_WIN_ON_ALL_DESKS	/* TBD - Use per virtual root bg window? */
#if USE_COMPOSITE
   /* Add background window */
   Esnprintf(buf, sizeof(buf), "Desk-bg-%d", desk);
   eo = EobjWindowCreate(EOBJ_TYPE_MISC,
			 0, 0, WinGetW(VROOT), WinGetH(VROOT), 0, buf);
   eo->floating = 0;
   EobjReparent(eo, EoObj(dsk), 0, 0);
   EobjSetLayer(eo, 0);
   dsk->bg.o_bg = eo;
   EventCallbackRegister(EobjGetWin(eo), 0, DeskHandleEvents, dsk);
#endif
#endif

   HintsSetRootHints(EoGetWin(dsk));

   if (configure)
      DeskConfigure(dsk);

   if (desk == 0)
      ESelectInputChange(EoGetWin(dsk), EDESK_EVENT_MASK, 0);
   else
      ESelectInput(EoGetWin(dsk), EDESK_EVENT_MASK);

   return dsk;
}

static void
DeskDestroy(Desk * dsk)
{
   ModulesSignal(ESIGNAL_DESK_REMOVED, dsk);

#if USE_COMPOSITE
   if (dsk->bg.o_bg)
     {
	EventCallbackUnregister(EobjGetWin(dsk->bg.o_bg), 0, DeskHandleEvents,
				dsk);
	EobjWindowDestroy(dsk->bg.o_bg);
     }
#endif
   EventCallbackUnregister(EoGetWin(dsk), 0, DeskHandleEvents, dsk);

   DeskControlsDestroy(dsk, 1);
   DeskControlsDestroy(dsk, 2);

   if (dsk->bg.bg)
      BackgroundDecRefcount(dsk->bg.bg);

   EoFini(dsk);

   desks.desk[dsk->num] = NULL;
   Efree(dsk);
}

EObj               *
DeskGetBackgroundObj(const Desk * dsk)
{
   return (dsk) ? dsk->bg.o : NULL;
}

Pixmap
DeskGetBackgroundPixmap(const Desk * dsk)
{
   if (!dsk)
      return None;
   return dsk->bg.pmap;
}

Background         *
DeskBackgroundGet(const Desk * dsk)
{
   return (dsk) ? dsk->bg.bg : NULL;
}

static void
DeskBackgroundConfigure(Desk * dsk)
{
   Win                 win;
   Pixmap              pmap = dsk->bg.pmap;
   unsigned long       pixel = dsk->bg.pixel;

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf
	 ("DeskBackgroundConfigure %d v=%d %#lx/%#lx: ext=%d pmap=%#lx/%#lx pixel=%#lx/%#lx\n",
	  dsk->num, dsk->viewable, EoGetXwin(dsk), EobjGetXwin(dsk->bg.o),
	  BackgroundIsNone(dsk->bg.bg), pmap, dsk->bg.pmap_set, pixel,
	  dsk->bg.pixel);

#if USE_COMPOSITE
   if (dsk->bg.o_bg)
     {
	if (ECompMgrIsActive())
	  {
	     dsk->bg.o = dsk->bg.o_bg;
	     EobjMap(dsk->bg.o_bg, 0);
	  }
	else
	  {
	     dsk->bg.o = EoObj(dsk);
	     EobjUnmap(dsk->bg.o_bg);
	  }
     }
#endif

   win = EobjGetWin(dsk->bg.o);

   if (dsk->viewable || !dsk->bg.bg)
     {
#if !USE_BG_WIN_ON_ALL_DESKS
	if (ECompMgrDeskConfigure(dsk))
	  {
	     ESetWindowBackgroundPixmap(win, None);
	  }
	else
#endif
	  {
	     if (pmap != None)
	       {
		  ESetWindowBackgroundPixmap(win, pmap);
		  if (dsk->num == 0 && win != VROOT)
		     ESetWindowBackgroundPixmap(VROOT, pmap);
	       }
	     else
	       {
		  ESetWindowBackground(win, pixel);
		  if (dsk->num == 0 && win != VROOT)
		     ESetWindowBackground(VROOT, pixel);
	       }
	     EClearWindow(win);
	  }

	HintsSetRootInfo(EoGetWin(dsk), pmap, pixel);
     }
   else if (dsk->bg.bg)
     {
	if (!Conf.hints.set_xroot_info_on_root_window)
	   HintsSetRootInfo(EoGetWin(dsk), None, 0);

	ESetWindowBackgroundPixmap(win, None);
	if (dsk->num == 0 && win != VROOT)
	   ESetWindowBackgroundPixmap(VROOT, None);
     }
}

static void
DeskBackgroundRefresh(Desk * dsk, int why)
{
   Background         *bg = dsk->bg.bg;
   Pixmap              pmap = dsk->bg.pmap;
   unsigned long       pixel = dsk->bg.pixel;
   int                 changed = 0;
   int                 reconfigure = 0;

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskBackgroundRefresh %d v=%d why=%d pmap=%#lx pixel=%#lx\n",
	      dsk->num, dsk->viewable, why, pmap, pixel);

   switch (why)
     {
     case DESK_BG_REFRESH:
	if (bg && dsk->viewable)
	   BackgroundTouch(bg);
	break;

     case DESK_BG_RECONFIGURE_ALL:
	reconfigure = 1;
	break;

     case DESK_BG_TIMEOUT:
	if (bg && dsk->viewable)
	   BackgroundTouch(bg);
	return;

     case DESK_BG_FREE:
	if (!bg || dsk->viewable)
	   return;
	break;
     }

   if (bg)
     {
	if (dsk->viewable)
	  {
	     if (BackgroundGetSeqNo(bg) == dsk->bg.seq_no)
		goto done;

	     pmap = BackgroundGetPixmap(bg);
	     pixel = 0;

	     if (pmap == None)
		BackgroundRealize(bg, EoGetWin(dsk), None,
				  EoGetW(dsk), EoGetH(dsk), 1, &pmap, &pixel);
	     if (pmap != None)
		BackgroundPixmapSet(bg, pmap);

	     dsk->bg.seq_no = BackgroundGetSeqNo(bg);
	     changed = 1;
	  }
	else
	  {
	     if (dsk->bg.pmap == None)
		return;

	     pmap = None;
	     pixel = 0;
	     dsk->bg.seq_no = 0;
	  }
     }
   else
     {
	pmap = (Mode.root.ext_pmap_valid) ? Mode.root.ext_pmap : None;
	pixel = 0;
	changed = pmap != dsk->bg.pmap_set;
     }

 done:
   dsk->bg.pmap = pmap;
   if (reconfigure || pmap != dsk->bg.pmap_set || pixel != dsk->bg.pixel)
     {
	dsk->bg.pixel = pixel;
	DeskBackgroundConfigure(dsk);
	dsk->bg.pmap_set = pmap;
     }

   if (bg && dsk->viewable)
      if (Conf.hints.set_xroot_info_on_root_window && dsk->num > 0)
	 HintsSetRootInfo(VROOT, pmap, pixel);

   if (changed)
      ModulesSignal(ESIGNAL_BACKGROUND_CHANGE, dsk);
}

void
DeskBackgroundSet(Desk * dsk, Background * bg)
{
   if (!dsk)
      return;

   BackgroundSetForDesk(bg, dsk->num);
   if (bg && BackgroundIsNone(bg))
      bg = NULL;

   if (dsk->bg.bg != bg)
     {
	if (dsk->bg.bg)
	   BackgroundDecRefcount(dsk->bg.bg);
	if (bg)
	   BackgroundIncRefcount(bg);
     }

   dsk->bg.bg = bg;

   DeskBackgroundRefresh(dsk, DESK_BG_REFRESH);
}

void
DesksBackgroundRefresh(Background * bg, int why)
{
   Desk               *dsk;
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
     {
	dsk = _DeskGet(i);
	if (!dsk)		/* May happen during init */
	   continue;
	if (bg && dsk->bg.bg != bg)
	   continue;
	DeskBackgroundRefresh(dsk, why);
     }
}

static void
DeskResize(int desk, int w, int h)
{
   Desk               *dsk;
   int                 x;

   dsk = _DeskGet(desk);

   if (dsk->num != 0)
     {
	x = (dsk->viewable) ? EoGetX(dsk) : WinGetW(VROOT);
	EoMoveResize(dsk, x, 0, w, h);
     }
#if USE_COMPOSITE
   if (dsk->bg.o_bg)
      EobjMoveResize(dsk->bg.o_bg, 0, 0, w, h);
#endif
   DeskBackgroundRefresh(dsk, DESK_BG_REFRESH);
   DeskControlsDestroy(dsk, 1);
   DeskControlsCreate(dsk);
   DeskControlsShow(dsk, 1);
}

Desk               *
DeskGet(unsigned int desk)
{
   if (desk >= Conf.desks.num)
      return NULL;

   return _DeskGet(desk);
}

Desk               *
DeskGetRelative(Desk * dsk, int inc)
{
   unsigned int        desk;

   desk = (dsk) ? dsk->num : 0;
   desk += inc;
   desk %= Conf.desks.num;

   return _DeskGet(desk);
}

void
DeskGetArea(const Desk * dsk, int *ax, int *ay)
{
   if (!dsk)
     {
	*ax = *ay = 0;
	return;
     }
   *ax = dsk->current_area_x;
   *ay = dsk->current_area_y;
}

void
DeskSetArea(Desk * dsk, int ax, int ay)
{
   if (!dsk)
      return;

   dsk->current_area_x = ax;
   dsk->current_area_y = ay;
}

void
DeskSetDirtyStack(Desk * dsk, EObj * eo)
{
   dsk->stack.dirty++;
   dsk->stack.latest = eo;
   if (EobjGetType(eo) == EOBJ_TYPE_EWIN)
      dsk->stack.update_client_list = 1;
   if (EDebug(EDBUG_TYPE_STACKING))
      Eprintf("DeskSetDirtyStack %d (%d): %s\n", dsk->num, dsk->stack.dirty,
	      EobjGetName(eo));
}

void
DeskCurrentGetArea(int *ax, int *ay)
{
   DeskGetArea(desks.current, ax, ay);
}

static void
DeskCurrentSetArea(int ax, int ay)
{
   DeskSetArea(desks.current, ax, ay);
}

unsigned int
DesksGetNumber(void)
{
   return Conf.desks.num;
}

Desk               *
DesksGetCurrent(void)
{
   return desks.current;
}

unsigned int
DesksGetCurrentNum(void)
{
   return desks.current->num;
}

void
DesksSetCurrent(Desk * dsk)
{
   if (!dsk)
      return;
   desks.current = dsk;
}

static void
DesksResize(int w, int h)
{
   unsigned int        i;

   BackgroundsInvalidate(0);

   for (i = 0; i < Conf.desks.num; i++)
      DeskResize(i, w, h);

   EdgeWindowsShow();

   ModulesSignal(ESIGNAL_DESK_RESIZE, NULL);
}

static void
ChangeNumberOfDesktops(unsigned int quantity)
{
   unsigned int        i;
   int                 j, num;
   EWin               *const *lst;

   if (quantity >= ENLIGHTENMENT_CONF_NUM_DESKTOPS)
      quantity = ENLIGHTENMENT_CONF_NUM_DESKTOPS;

   if (quantity <= 0 || quantity == Conf.desks.num)
      return;

   for (i = quantity; i < Conf.desks.num; i++)
      DeskLower(i);

   if (quantity > Conf.desks.num)
     {
	while (Conf.desks.num < quantity)
	  {
	     Conf.desks.num++;
	     DeskCreate(Conf.desks.num - 1, 1);
	  }
     }
   else if (quantity < Conf.desks.num)
     {
	lst = EwinListGetAll(&num);
	for (j = 0; j < num; j++)
	  {
	     if (EoGetDeskNum(lst[j]) >= quantity)
		EwinMoveToDesktop(lst[j], _DeskGet(quantity - 1));
	  }

	while (Conf.desks.num > quantity)
	  {
	     DeskDestroy(_DeskGet(Conf.desks.num - 1));
	     Conf.desks.num--;
	  }
     }

   if (DesksGetCurrentNum() >= Conf.desks.num)
      DeskGotoNum(Conf.desks.num - 1);

   HintsSetDesktopConfig();

   autosave();
}

static void
DesksControlsCreate(void)
{
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
      DeskControlsCreate(_DeskGet(i));
}

static void
DesksControlsDestroy(void)
{
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
      DeskControlsDestroy(_DeskGet(i), 1);
}

static void
DesksControlsShow(void)
{
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
      DeskControlsShow(_DeskGet(i), 1);
}

static void
DesksControlsRefresh(void)
{
   DesksControlsDestroy();
   DesksControlsCreate();
   DesksControlsShow();

   autosave();
}

#if 0				/* Unused */
static void
DeskShowTabs(void)
{
   ButtonsForeach(2, NULL, ButtonShow);
}

static void
DeskHideTabs(void)
{
   ButtonsForeach(2, NULL, ButtonHide);
}
#endif

static void
DeskButtonShowDefault(Button * b)
{
   if (!ButtonDoShowDefault(b))
      return;
   ButtonShow(b);
}

static void
DeskShowButtons(void)
{
   ButtonsForeach(0, NULL, DeskButtonShowDefault);
}

static void
MoveToDeskTop(Desk * dsk)
{
   int                 i, j;

   EoRaise(dsk);

   j = -1;
   i = 0;
   while (j < 0 && i < (int)Conf.desks.num)
     {
	if (desks.order[i] == dsk->num)
	   j = i;
	i++;
     }
   if (j < 0)
      return;
   if (j > 0)
     {
	for (i = j - 1; i >= 0; i--)
	   desks.order[i + 1] = desks.order[i];
	desks.order[0] = dsk->num;
     }
}

static void
MoveToDeskBottom(Desk * dsk)
{
   int                 i, j;

   EoLower(dsk);

   j = -1;
   i = 0;
   while (j < 0 && i < (int)Conf.desks.num)
     {
	if (desks.order[i] == dsk->num)
	   j = i;
	i++;
     }
   if (j < 0)
      return;
   if (j < (int)Conf.desks.num - 1)
     {
	for (i = j; i < (int)Conf.desks.num - 1; i++)
	   desks.order[i] = desks.order[i + 1];
	desks.order[Conf.desks.num - 1] = dsk->num;
     }
}

Desk               *
DesktopAt(int x, int y)
{
   Desk               *dsk;
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
     {
	dsk = _DeskGet(desks.order[i]);
	if (x >= EoGetX(dsk) && x < (EoGetX(dsk) + WinGetW(VROOT)) &&
	    y >= EoGetY(dsk) && y < (EoGetY(dsk) + WinGetH(VROOT)))
	   return _DeskGet(desks.order[i]);
     }
   return _DeskGet(0);
}

static void
DesksStackingCheck(void)
{
   Desk               *dsk;
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
     {
	dsk = _DeskGet(i);
	if (i && !dsk->viewable)
	   continue;
	if (!dsk->stack.dirty)
	   continue;
	DeskRestack(dsk);
     }
}

static void
_DesksIdler(void *data __UNUSED__)
{
   DesksStackingCheck();
}

static void
DeskMove(Desk * dsk, int x, int y)
{
   Desk               *dd;
   unsigned int        i;
   int                 n, v, dx, dy;

   if (dsk->num <= 0)
      return;

   n = -1;
   i = 0;
   while (n < 0 && i < Conf.desks.num)
     {
	if (desks.order[i] == dsk->num)
	   n = i;
	i++;
     }
   if (n < 0)			/* Should not be possible */
      return;

   dx = x - EoGetX(dsk);
   dy = y - EoGetY(dsk);

   if (x == 0 && y == 0)
     {
	/* Desks below are obscured - touch and set unviewable */
	for (i = n + 1; i < Conf.desks.num; i++)
	  {
	     dd = _DeskGet(desks.order[i]);
	     if (dd->viewable)
		BackgroundTouch(dd->bg.bg);
	     dd->viewable = 0;
	  }
     }
   else
     {
	v = dsk->viewable;

	for (i = n + 1; i < Conf.desks.num; i++)
	  {
	     dd = _DeskGet(desks.order[i]);
	     if (!dd->viewable && v)
	       {
		  dd->viewable = 1;
		  DeskBackgroundRefresh(_DeskGet(desks.order[i]),
					DESK_BG_REFRESH);
	       }
	     else if (dd->viewable && !v)
	       {
		  BackgroundTouch(dd->bg.bg);
		  dd->viewable = 0;
	       }

	     if (EoGetX(dd) == 0 && EoGetY(dd) == 0)
		v = 0;
	  }
     }

   EoMove(dsk, x, y);

   if (dsk->tag)
      ButtonMoveRelative(dsk->tag, dx, dy);

#if 0				/* FIXME - Remove? */
   EWin               *const *lst;

   lst = EwinListGetAll(&n);
   for (i = 0; i < (unsigned int)n; i++)
      if (EoGetDesk(lst[i]) == dsk)
	 ICCCM_Configure(lst[i]);
#endif
}

static void
DeskHide(unsigned int desk)
{
   Desk               *dsk;

   if (desk <= 0 || desk >= Conf.desks.num)
      return;

   dsk = _DeskGet(desk);

   if (dsk->viewable)
      BackgroundTouch(dsk->bg.bg);
   dsk->viewable = 0;
   EoMove(dsk, WinGetW(VROOT), 0);
}

static void
DeskEnter(Desk * dsk)
{
   int                 i;

   EGrabServer();

   dsk->viewable = 1;
   DeskBackgroundRefresh(dsk, DESK_BG_REFRESH);
   MoveToDeskTop(dsk);

   desks.previous = desks.current = dsk;

   if (dsk->num == 0)
     {
	for (i = Conf.desks.num - 1; i > 0; i--)
	   DeskHide(desks.order[i]);
     }

   EwinsMoveStickyToDesk(dsk);
   ButtonsMoveStickyToDesk(dsk);
   DesksStackingCheck();
   HintsSetCurrentDesktop();
   EdgeWindowsShow();

   EUngrabServer();
}

void
DeskGotoNum(unsigned int desk)
{
   Desk               *dsk;

   if (Conf.desks.desks_wraparound)
      desk %= Conf.desks.num;
   if (desk >= Conf.desks.num || desk == desks.current->num)
      return;

   dsk = _DeskGet(desk);
   DeskGoto(dsk);
}

static void
DeskSwitchStart(void)
{
   FocusNewDeskBegin();
}

static void
DeskSwitchDone(void)
{
   FocusNewDesk();
}

void
DeskGoto(Desk * dsk)
{
   if (!dsk || dsk == desks.previous)
      return;

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskGoto %d\n", dsk->num);

   ModulesSignal(ESIGNAL_DESK_SWITCH_START, NULL);

   ActionsSuspend();
   DeskSwitchStart();

   if (dsk->num > 0)
     {
	if (Conf.desks.slidein)
	  {
	     if (!dsk->viewable)
	       {
		  int                 x, y;

		  switch (Conf.desks.dragdir)
		    {
		    default:
		    case 0:
		       x = WinGetW(VROOT);
		       y = 0;
		       break;
		    case 1:
		       x = -WinGetW(VROOT);
		       y = 0;
		       break;
		    case 2:
		       x = 0;
		       y = WinGetH(VROOT);
		       break;
		    case 3:
		       x = 0;
		       y = -WinGetH(VROOT);
		       break;
		    }
		  DeskMove(dsk, x, y);
		  DeskEnter(dsk);
		  EobjSlideTo(&dsk->o, x, y, 0, 0, Conf.desks.slidespeed);
	       }
	     else
	       {
		  EobjSlideTo(&dsk->o, EoGetX(dsk), EoGetY(dsk), 0, 0,
			      Conf.desks.slidespeed);
		  DeskEnter(dsk);
	       }
	  }
	else
	  {
	     DeskEnter(dsk);
	  }
	DeskMove(dsk, 0, 0);
     }
   else
     {
	DeskEnter(dsk);
     }

   DeskSwitchDone();
   ActionsResume();

   ModulesSignal(ESIGNAL_DESK_SWITCH_DONE, NULL);

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskGoto %d done\n", dsk->num);
}

static void
UncoverDesktop(unsigned int desk)
{
   Desk               *dsk;

   if (desk >= Conf.desks.num)
      return;

   dsk = _DeskGet(desk);

   dsk->viewable = 1;
   DeskBackgroundRefresh(dsk, DESK_BG_REFRESH);
}

static void
DeskRaise(unsigned int desk)
{
   Desk               *dsk;

   if (desk >= Conf.desks.num)
      return;

   dsk = _DeskGet(desk);

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskRaise(%d) current=%d\n", desk, desks.current->num);

   DeskSwitchStart();
   DeskEnter(dsk);
   DeskSwitchDone();

   ModulesSignal(ESIGNAL_DESK_SWITCH_DONE, NULL);

   ESync(ESYNC_DESKS);
}

static void
DeskLower(unsigned int desk)
{
   Desk               *dsk;

   if ((desk <= 0) || (desk >= Conf.desks.num))
      return;

   dsk = _DeskGet(desk);

   DeskSwitchStart();
   MoveToDeskBottom(dsk);

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskLower(%d) %d -> %d\n", desk, desks.current->num,
	      desks.order[0]);

   desks.previous = desks.current = DeskGet(desks.order[0]);

   EGrabServer();

   UncoverDesktop(desks.order[0]);
   DeskHide(desk);

   EwinsMoveStickyToDesk(desks.current);
   ButtonsMoveStickyToDesk(desks.current);
   DesksStackingCheck();
   DeskSwitchDone();
   if (Mode.mode == MODE_NONE)
      ModulesSignal(ESIGNAL_DESK_SWITCH_DONE, NULL);
   HintsSetCurrentDesktop();

   EUngrabServer();
   ESync(ESYNC_DESKS);
}

#if 0				/* Unused */
void
DeskShow(int desk)
{
   Desk               *dsk;
   int                 i;

   if (desk < 0 || desk >= Conf.desks.num)
      return;

   dsk = _DeskGet(desk);

   dsk->viewable = 1;
   DeskBackgroundRefresh(dsk, DESK_BG_REFRESH);
   MoveToDeskTop(desk);

   if (desk == 0)
     {
	for (i = Conf.desks.num - 1; i > 0; i--)
	   DeskHide(desks.order[i]);
     }
}
#endif

static void
DeskRestackSimple(Desk * dsk)
{
   EObj               *const *lst, *eo;
   int                 i, num;
   XWindowChanges      xwc;
   unsigned int        value_mask;

   eo = dsk->stack.latest;
   eo->stacked = 1;

   if (EDebug(EDBUG_TYPE_STACKING))
      Eprintf("DeskRestackSimple %#lx %s\n", EobjGetXwin(eo), EobjGetName(eo));

   lst = EobjListStackGetForDesk(&num, dsk);
   if (num < 2)
      return;

   for (i = 0; i < num; i++)
      if (lst[i] == eo)
	 break;
   if (i >= num)
      return;

   if (i < num - 1)
     {
	xwc.stack_mode = Above;
	xwc.sibling = EobjGetXwin(lst[i + 1]);
     }
   else
     {
	xwc.stack_mode = Below;
	xwc.sibling = EobjGetXwin(lst[i - 1]);
     }
   value_mask = CWSibling | CWStackMode;
   if (EDebug(EDBUG_TYPE_STACKING))
      Eprintf("DeskRestackSimple %#10lx %s %#10lx\n", EobjGetXwin(eo),
	      (xwc.stack_mode == Above) ? "Above" : "Below", xwc.sibling);
   XConfigureWindow(disp, EobjGetXwin(eo), value_mask, &xwc);
}

#define _APPEND_TO_WIN_LIST(win) \
  { \
     wl = EREALLOC(Window, wl, ++tot); \
     wl[tot - 1] = win; \
  }
void
DeskRestack(Desk * dsk)
{
   Window             *wl;
   int                 i, num, tot;
   EObj               *const *lst, *eo;

   if (!dsk->stack.dirty)
      return;

   /* Special case if only one window needs restacking */
   if (dsk->stack.dirty == 1)
     {
	DeskRestackSimple(dsk);
	goto done;
     }

   /* Build the window stack, top to bottom */
   tot = 0;
   wl = NULL;
   lst = EobjListStackGetForDesk(&num, dsk);

   /* Normal objects */
   for (i = 0; i < num; i++)
     {
	eo = lst[i];
	_APPEND_TO_WIN_LIST(EobjGetXwin(eo));
	eo->stacked = 1;
     }

   if (EDebug(EDBUG_TYPE_STACKING))
     {
	Eprintf("DeskRestack %d (%d):\n", dsk->num, dsk->stack.dirty);
	for (i = 0; i < tot; i++)
	   Eprintf(" win=%#10lx parent=%#10lx\n", wl[i],
		   EXWindowGetParent(wl[i]));
     }

   EXRestackWindows(wl, tot);

   Efree(wl);

 done:
   if (dsk->stack.update_client_list)
     {
	dsk->stack.update_client_list = 0;
	HintsSetClientStacking();
	ClickGrabsUpdate();
     }
   dsk->stack.dirty = 0;
   dsk->stack.latest = NULL;
}

void
DeskGotoByEwin(EWin * ewin)
{
   if (EoIsSticky(ewin) || EoIsFloating(ewin))
      return;

   DeskGoto(EoGetDesk(ewin));
   DeskCurrentGotoArea(ewin->area_x, ewin->area_y);
}

/*
 * Areas
 */
static int          area_w = 3;
static int          area_h = 3;

void
DesksFixArea(int *ax, int *ay)
{
   if (*ax < 0)
     {
	if (Conf.desks.areas_wraparound)
	   *ax = area_w - 1;
	else
	   *ax = 0;
     }
   else if (*ax >= area_w)
     {
	if (Conf.desks.areas_wraparound)
	   *ax = 0;
	else
	   *ax = area_w - 1;
     }

   if (*ay < 0)
     {
	if (Conf.desks.areas_wraparound)
	   *ay = area_h - 1;
	else
	   *ay = 0;
     }
   else if (*ay >= area_h)
     {
	if (Conf.desks.areas_wraparound)
	   *ay = 0;
	else
	   *ay = area_h - 1;
     }
}

static int
AreaXYToLinear(int ax, int ay)
{
   DesksFixArea(&ax, &ay);
   return (ay * area_w) + ax;
}

static void
AreaLinearToXY(int a, int *ax, int *ay)
{
   if (a < 0)
      a = 0;
   else if (a >= (area_w * area_h))
      a = (area_w * area_h) - 1;
   *ay = a / area_w;
   *ax = a - (*ay * area_w);
}

static void
SetAreaSize(int aw, int ah)
{
   if (aw < 1)
      aw = 1;
   if (ah < 1)
      ah = 1;
   Conf.desks.areas_nx = area_w = aw;
   Conf.desks.areas_ny = area_h = ah;
   HintsSetViewportConfig();
   EdgeWindowsShow();
   ModulesSignal(ESIGNAL_AREA_CONFIGURED, NULL);
}

void
DesksGetAreaSize(int *aw, int *ah)
{
   *aw = area_w;
   *ah = area_h;
}

static void
SetNewAreaSize(int ax, int ay)
{

   int                 a, b, i, num;
   EWin               *const *lst;

   if (ax <= 0)
      return;
   if (ay <= 0)
      return;

   DesksGetAreaSize(&a, &b);
   if ((a == ax) && (b == ay))
      return;

   SetAreaSize(ax, ay);

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	if (!EoIsSticky(lst[i]))
	  {
	     if (lst[i]->area_x >= ax)
		EwinMoveToArea(lst[i], ax - 1, lst[i]->area_x);
	     if (lst[i]->area_y >= ay)
		EwinMoveToArea(lst[i], lst[i]->area_x, ay - 1);
	  }
     }

   DeskCurrentGetArea(&a, &b);
   if (a >= ax)
     {
	DeskCurrentGotoArea(ax - 1, b);
	DeskCurrentGetArea(&a, &b);
     }
   if (b >= ay)
      DeskCurrentGotoArea(a, ay - 1);
}

static void
SetCurrentLinearArea(int a)
{
   int                 ax, ay;

   AreaLinearToXY(a, &ax, &ay);
   DeskCurrentGotoArea(ax, ay);
}

static int
GetCurrentLinearArea(void)
{
   int                 ax, ay;

   DeskCurrentGetArea(&ax, &ay);

   return AreaXYToLinear(ax, ay);
}

static void
MoveCurrentLinearAreaBy(int a)
{
   SetCurrentLinearArea(GetCurrentLinearArea() + a);
}

void
DeskCurrentGotoArea(int ax, int ay)
{
   EWin               *const *lst, *ewin;
   int                 i, num, dx, dy, pax, pay;

   if ((Mode.mode == MODE_RESIZE) || (Mode.mode == MODE_RESIZE_H)
       || (Mode.mode == MODE_RESIZE_V))
      return;

   DesksFixArea(&ax, &ay);
   DeskCurrentGetArea(&pax, &pay);

   if (ax == pax && ay == pay)
      return;

   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskCurrentGotoArea %d,%d\n", ax, ay);

   ModulesSignal(ESIGNAL_AREA_SWITCH_START, NULL);

   dx = WinGetW(VROOT) * (ax - pax);
   dy = WinGetH(VROOT) * (ay - pay);

   if (dx < 0)
      SoundPlay("SOUND_MOVE_AREA_LEFT");
   else if (dx > 0)
      SoundPlay("SOUND_MOVE_AREA_RIGHT");
   else if (dy < 0)
      SoundPlay("SOUND_MOVE_AREA_UP");
   else if (dy > 0)
      SoundPlay("SOUND_MOVE_AREA_DOWN");

   ActionsSuspend();

   /* remove lots of event masks from windows.. we dont want to bother */
   /* handling events as a result of our playing wiht windows */
   DeskSwitchStart();

   /* set the current area up in out data structs */
   DeskCurrentSetArea(ax, ay);

   /* move all the windows around */
   lst = EwinListGetAll(&num);
   if (Conf.desks.slidein)
     {
	int                 wnum = 0;
	EObj              **wl = NULL;

	/* create the list of windwos to move */
	for (i = 0; i < num; i++)
	  {
	     ewin = lst[i];
	     if (EoIsSticky(ewin) || ewin->state.iconified)
		continue;
	     if (EoGetDesk(ewin) != DesksGetCurrent() && !EoIsFloating(ewin))
		continue;

	     if (EoIsFloating(ewin) && Conf.movres.mode_move == 0)
		continue;

	     wnum++;
	     wl = EREALLOC(EObj *, wl, wnum);
	     wl[wnum - 1] = &ewin->o;
	  }

	/* slide them */
	if (wl)
	  {
	     EobjsSlideBy(wl, wnum, -dx, -dy, Conf.desks.slidespeed);
	     Efree(wl);
	     EobjsRepaint();
	  }
     }

   /* move all windows to their final positions */
   Mode.move.check = 0;
   for (i = 0; i < num; i++)
     {
	ewin = lst[i];
	if (EwinIsTransientChild(ewin))
	   continue;
	if (EoGetDesk(ewin) != DesksGetCurrent() && !EoIsFloating(ewin))
	   continue;

	if (EoIsSticky(ewin) ||
	    (EoIsFloating(ewin) && Conf.movres.mode_move == 0) ||
	    (!ewin->state.iconified && Conf.desks.slidein))
	   EwinMove(ewin, EoGetX(ewin), EoGetY(ewin));
	else
	   EwinMove(ewin, EoGetX(ewin) - dx, EoGetY(ewin) - dy);
     }
   Mode.move.check = 1;

   if (!Conf.desks.slidein)
      EobjsRepaint();

   /* set hints up for it */
   HintsSetDesktopViewport();

   ActionsResume();

   /* re-focus on a new ewin on that new desktop area */
   DeskSwitchDone();

   ModulesSignal(ESIGNAL_AREA_SWITCH_DONE, DesksGetCurrent());

   /* update which "edge flip resistance" detector windows are visible */
   EdgeWindowsShow();
}

void
DeskCurrentMoveAreaBy(int dx, int dy)
{
   int                 ax, ay;

   DeskCurrentGetArea(&ax, &ay);
   DeskCurrentGotoArea(ax + dx, ay + dy);
}

/*
 * Actions, events
 */
static char         sentpress = 0;

static void
ButtonProxySendEvent(XEvent * ev)
{
   if (Mode.button_proxy_win)
      EXSendEvent(Mode.button_proxy_win, SubstructureNotifyMask, ev);
}

static void
DeskDragStart(int desk)
{
   Desk               *dsk;

   dsk = _DeskGet(desk);

   desks.drag_x0 = Mode.events.cx - EoGetX(dsk);
   desks.drag_y0 = Mode.events.cy - EoGetY(dsk);

   Mode.mode = MODE_DESKDRAG;
}

static void
DeskDragEnd(Desk * dsk __UNUSED__)
{
   Mode.mode = MODE_NONE;
}

static void
DeskDragMotion(Desk * dsk)
{
   int                 x, y;

   x = Mode.events.mx - desks.drag_x0;
   y = Mode.events.my - desks.drag_y0;

   switch (Conf.desks.dragdir)
     {
     case 0:
	if (x < 0)
	   x = 0;
	y = 0;
	break;
     case 1:
	if (x > 0)
	   x = 0;
	y = 0;
	break;
     case 2:
	x = 0;
	if (y < 0)
	   y = 0;
	break;
     case 3:
	x = 0;
	if (y > 0)
	   y = 0;
	break;
     default:
	break;
     }
   DeskMove(dsk, x, y);
}

static void
DeskButtonCallback(EObj * eo, XEvent * ev, ActionClass * ac)
{
   Desk               *dsk;

   if (Mode.mode != MODE_DESKDRAG)
     {
	if (ac)
	   ActionclassEvent(ac, ev, NULL);
	return;
     }

   dsk = (Desk *) eo;
   switch (ev->type)
     {
     case ButtonRelease:
	DeskDragEnd(dsk);
	break;
     case MotionNotify:
	DeskDragMotion(dsk);
	break;
     }
}

static int
DeskCheckAction(Desk * dsk __UNUSED__, XEvent * ev)
{
   ActionClass        *ac;

   ac = ActionclassFind("DESKBINDINGS");
   if (!ac)
      return 0;

   return ActionclassEvent(ac, ev, NULL);
}

static void
DeskEventButtonPress(Desk * dsk, XEvent * ev)
{
   /* Don't handle desk bindings while doing stuff */
   if (Mode.mode)
      return;

   GrabPointerRelease();

   if (!DeskCheckAction(dsk, ev))
      ButtonProxySendEvent(ev);
}

static void
DeskEventButtonRelease(Desk * dsk, XEvent * ev)
{
   /* Don't handle desk bindings while doing stuff */
   if (Mode.mode)
      return;

   if (sentpress)
     {
	/* We never get here? */
	sentpress = 0;
	ButtonProxySendEvent(ev);
     }

   DeskCheckAction(dsk, ev);
}

static void
DeskRootResize(int root, int w, int h)
{
   if (EDebug(EDBUG_TYPE_DESKS))
      Eprintf("DeskRootResize %d %dx%d\n", root, w, h);

   if (root && (VROOT != RROOT))
     {
	WinGetW(RROOT) = w;
	WinGetH(RROOT) = h;
     }

   /* Quit if no change */
   if (w == WinGetW(VROOT) && h == WinGetH(VROOT))
      return;

   EWindowSync(VROOT);

   /* Quit if size is not final */
   if (w != WinGetW(VROOT) || h != WinGetH(VROOT))
      return;

   ScreenInit();
   DesksResize(w, h);

   Mode.screen.w_old = WinGetW(VROOT);
   Mode.screen.h_old = WinGetH(VROOT);
}

static ActionClass *
DeskGetAclass(void *data __UNUSED__)
{
   return ActionclassFind("DESKBINDINGS");
}

static void
DeskPropertyChange(Desk * dsk, XEvent * ev)
{
   Pixmap              pmap;

   if (ev->xproperty.atom == E_XROOTPMAP_ID)
     {
	/* Possible race here? */
	pmap = HintsGetRootPixmap(EoGetWin(dsk));
	if (EDebug(EDBUG_TYPE_DESKS))
	   Eprintf("DeskPropertyChange win=%#lx _XROOTPMAP_ID=%#lx\n",
		   ev->xany.window, pmap);
	if (ev->xany.window != WinGetXwin(VROOT))
	   return;
	if (pmap == dsk->bg.pmap)
	   return;
	if (pmap == Mode.root.ext_pmap)
	   return;
	Mode.root.ext_pmap = pmap;
	Mode.root.ext_pmap_valid = EDrawableCheck(pmap, 0);
	DesksBackgroundRefresh(NULL, DESK_BG_REFRESH);
     }
   else if (ev->xproperty.atom == E_XROOTCOLOR_PIXEL)
     {
	if (EDebug(EDBUG_TYPE_DESKS))
	   Eprintf("DeskPropertyChange win=%#lx _XROOTCOLOR_PIXEL\n",
		   ev->xany.window);
	if (ev->xany.window != WinGetXwin(VROOT))
	   return;
     }
}

static void
DeskHandleEvents(Win win __UNUSED__, XEvent * ev, void *prm)
{
   Desk               *dsk = (Desk *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	DeskEventButtonPress(dsk, ev);
	break;
     case ButtonRelease:
	DeskEventButtonRelease(dsk, ev);
	break;

     case EnterNotify:
	FocusHandleEnter(NULL, ev);
	break;
     case LeaveNotify:
	FocusHandleLeave(NULL, ev);
	break;

     case MotionNotify:
	/* Motion over desk buttons doesn't go here - We probably don't care much. */
	DesksSetCurrent(DesktopAt(Mode.events.mx, Mode.events.my));
	TooltipsSetPending(1, DeskGetAclass, dsk);
	break;

     case ConfigureNotify:
	if (ev->xconfigure.window == WinGetXwin(VROOT))
	   DeskRootResize(0, ev->xconfigure.width, ev->xconfigure.height);
	break;

     case PropertyNotify:
	if (ev->xany.window == WinGetXwin(VROOT))
	   DeskPropertyChange(dsk, ev);
	break;

#if USE_XRANDR
     case EX_EVENT_SCREEN_CHANGE_NOTIFY:
	{
	   XRRScreenChangeNotifyEvent *rrev = (XRRScreenChangeNotifyEvent *) ev;

	   DeskRootResize(1, rrev->width, rrev->height);
	}
	break;
#endif
     }
}

/* Settings */

static void
DeskDragdirSet(const char *params)
{
   Desk               *dsk;
   unsigned int        i;
   int                 pd;

   pd = Conf.desks.dragdir;

   if (params && params[0])
      Conf.desks.dragdir = atoi(params);
   else
     {
	Conf.desks.dragdir++;
	if (Conf.desks.dragdir > 3)
	   Conf.desks.dragdir = 0;
     }

   if (pd == Conf.desks.dragdir)
      return;

   for (i = 1; i < Conf.desks.num; i++)
     {
	dsk = _DeskGet(i);
	EoMove(dsk, (dsk->viewable) ? 0 : WinGetW(VROOT), 0);
     }
   DesksControlsRefresh();
}

static void
DeskDragbarOrderSet(const char *params)
{
   int                 pd;

   pd = Conf.desks.dragbar_ordering;

   if (params && params[0])
      Conf.desks.dragbar_ordering = atoi(params);
   else
     {
	Conf.desks.dragbar_ordering++;
	if (Conf.desks.dragbar_ordering > 5)
	   Conf.desks.dragbar_ordering = 0;
     }

   if (pd == Conf.desks.dragbar_ordering)
      return;

   DesksControlsRefresh();
}

#if 0				/* FIXME */

static int
doDragbarWidthSet(EWin * edummy, const char *params)
{
   int                 pd;
   Button             *b;

   pd = Conf.desks.dragbar_width;
   if (params)
      Conf.desks.dragbar_width = atoi(params);

   if (pd != Conf.desks.dragbar_width)
     {
	DesksControlsRefresh();
     }
   return 0;
}

static int
doDragbarLengthSet(EWin * edummy, const char *params)
{
   int                 pd;
   Button             *b;

   pd = Conf.desks.dragbar_length;
   if (params)
      Conf.desks.dragbar_length = atoi(params);

   if (pd != Conf.desks.dragbar_length)
     {
	DesksControlsRefresh();
     }
   return 0;
}
#endif

#if ENABLE_DESKRAY
static int
doDeskray(EWin * edummy, const char *params)
{
   if (params)
     {
	if (!atoi(params))
	  {
	     DeskHideTabs();
	     Conf.deskmode = MODE_NONE;
	  }
	else
	  {
	     Conf.deskmode = MODE_DESKRAY;
	     DeskShowTabs();
	  }
     }
   else
     {
	if (Conf.deskmode == MODE_DESKRAY)
	  {
	     DeskHideTabs();
	     Conf.deskmode = MODE_NONE;
	  }
	else
	  {
	     Conf.deskmode = MODE_DESKRAY;
	     DeskShowTabs();
	  }
     }
   return 0;
}
#endif /* ENABLE_DESKRAY */

static void
DesksInit(void)
{
   unsigned int        i;

   memset(&desks, 0, sizeof(desks));

   Mode.screen.w_old = WinGetW(VROOT);
   Mode.screen.h_old = WinGetH(VROOT);

   /* Backward compatibility hack */
   if (Conf.desks.edge_flip_resistance <= 0)
      Conf.desks.edge_flip_mode = EDGE_FLIP_OFF;

   desks.previous = NULL;

   for (i = 0; i < Conf.desks.num; i++)
      DeskCreate(i, 0);

   SetAreaSize(Conf.desks.areas_nx, Conf.desks.areas_ny);

   /* Retreive stuff from last time we were loaded if we're restarting */
   EHintsGetDeskInfo();

   HintsSetDesktopConfig();
}

static void
DesksConfigure(void)
{
   unsigned int        i;

   for (i = 0; i < Conf.desks.num; i++)
      DeskConfigure(_DeskGet(i));

   UncoverDesktop(0);
}

/*
 * Desktops Module
 */

static void
DesksSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	DesksInit();
	break;

     case ESIGNAL_CONFIGURE:
	DesksConfigure();
	break;

     case ESIGNAL_START:
	/* Draw all the buttons that belong on the desktop */
	DeskShowButtons();
	IdlerAdd(_DesksIdler, NULL);
	break;
     }
}

/*
 * Dialogs
 */
static int          tmp_desktops;
static DItem       *tmp_desk_text;
static char         tmp_desktop_slide;
static int          tmp_desktop_slide_speed;
static char         tmp_desktop_wraparound;
static char         tmp_dragbar;
static int          tmp_dragdir;

static void
CB_ConfigureDesktops(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	ChangeNumberOfDesktops(tmp_desktops);
	Conf.desks.slidein = tmp_desktop_slide;
	Conf.desks.slidespeed = tmp_desktop_slide_speed;
	Conf.desks.desks_wraparound = tmp_desktop_wraparound;

	if ((Conf.desks.dragdir != tmp_dragdir) ||
	    ((tmp_dragbar) && (Conf.desks.dragbar_width < 1)) ||
	    ((!tmp_dragbar) && (Conf.desks.dragbar_width > 0)))
	  {
	     if (tmp_dragbar)
		Conf.desks.dragbar_width = 16;
	     else
		Conf.desks.dragbar_width = 0;
	     Conf.desks.dragdir = tmp_dragdir;
	     DesksControlsRefresh();
	  }
     }
   autosave();
}

static void
CB_DesktopDisplayRedraw(Dialog * d, int val, void *data)
{
   static char         called = 0;
   static int          prev_desktops = -1;
   static Win          wins[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
   DItem              *di;
   int                 i;
   int                 w, h;
   Win                 win;
   char                s[64];
   ImageClass         *ic;

   if (val == 1)
      called = 0;

   if ((val != 1) && (prev_desktops == tmp_desktops))
      return;

   prev_desktops = tmp_desktops;
   di = (DItem *) data;
   win = DialogItemAreaGetWindow(di);
   DialogItemAreaGetSize(di, &w, &h);

   if (!called)
     {
	ic = ImageclassFind("SETTINGS_DESKTOP_AREA", 1);
	ImageclassApply(ic, win, 0, 0, STATE_NORMAL, ST_SOLID);
	for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; i++)
	   wins[i] = 0;
	called = 1;
     }

   for (i = 0; i < tmp_desktops; i++)
     {
	if (!wins[i])
	  {
	     Background         *bg;

	     wins[i] = ECreateWindow(win, 0, 0, 64, 48, 0);
	     ESetWindowBorderWidth(wins[i], 1);

	     bg = DeskBackgroundGet(DeskGet(i));
	     if (bg)
	       {
		  Pixmap              pmap;

		  pmap = EGetWindowBackgroundPixmap(wins[i]);
		  BackgroundApplyPmap(bg, wins[i], pmap, 64, 48);
	       }
	     else
	       {
		  ic = ImageclassFind("SETTINGS_DESKTOP_AREA", 1);
		  ImageclassApply(ic, wins[i], 0, 0, STATE_NORMAL, ST_SOLID);
	       }
	  }
     }

   for (i = tmp_desktops - 1; i >= 0; i--)
     {
	int                 num;

	num = tmp_desktops - 1;
	if (num < 1)
	   num = 1;
	EMoveWindow(wins[i], (i * (w - 64 - 2)) / num,
		    (i * (h - 48 - 2)) / num);
	ERaiseWindow(wins[i]);
	EMapWindow(wins[i]);
     }

   for (i = tmp_desktops; i < (int)Conf.desks.num; i++)
     {
	if (!wins[i])
	   continue;
	EUnmapWindow(wins[i]);
     }

   if (tmp_desktops > 1)
      Esnprintf(s, sizeof(s), _("%i Desktops"), tmp_desktops);
   else
      Esnprintf(s, sizeof(s), _("%i Desktop"), tmp_desktops);
   DialogItemSetText(tmp_desk_text, s);
   DialogDrawItems(d, tmp_desk_text, 0, 0, 99999, 99999);
}

static void
CB_DesktopDisplayAreaRedraw(DItem * di, int val __UNUSED__,
			    void *data __UNUSED__)
{
   CB_DesktopDisplayRedraw(DialogItemGetDialog(di), 1, di);
}

static void
_DlgFillDesks(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *slider, *radio;
   char                s[64];

   tmp_desktops = Conf.desks.num;
   tmp_desktop_slide = Conf.desks.slidein;
   tmp_desktop_slide_speed = Conf.desks.slidespeed;
   tmp_desktop_wraparound = Conf.desks.desks_wraparound;
   if (Conf.desks.dragbar_width < 1)
      tmp_dragbar = 0;
   else
      tmp_dragbar = 1;
   tmp_dragdir = Conf.desks.dragdir;

   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Number of virtual desktops:\n"));

   di = tmp_desk_text = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   if (tmp_desktops > 1)
      Esnprintf(s, sizeof(s), _("%i Desktops"), tmp_desktops);
   else
      Esnprintf(s, sizeof(s), _("%i Desktop"), tmp_desktops);
   DialogItemSetText(di, s);

   di = slider = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetBounds(di, 1, 32);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSetColSpan(di, 2);
   DialogItemSliderSetValPtr(di, &tmp_desktops);

   di = DialogAddItem(table, DITEM_AREA);
   DialogItemSetColSpan(di, 2);
   DialogItemAreaSetSize(di, 128, 96);
   DialogItemAreaSetInitFunc(di, CB_DesktopDisplayAreaRedraw);

   DialogItemSetCallback(slider, CB_DesktopDisplayRedraw, 0, di);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Slide desktops around when changing"));
   DialogItemCheckButtonSetPtr(di, &tmp_desktop_slide);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 1024, 512);
   DialogItemSetText(di, _("Desktop Slide speed:\n"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetColSpan(di, 2);
   DialogItemSliderSetBounds(di, 0, 20000);
   DialogItemSliderSetUnits(di, 500);
   DialogItemSliderSetJump(di, 1000);
   DialogItemSliderSetValPtr(di, &tmp_desktop_slide_speed);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Wrap desktops around"));
   DialogItemCheckButtonSetPtr(di, &tmp_desktop_wraparound);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Display desktop dragbar"));
   DialogItemCheckButtonSetPtr(di, &tmp_dragbar);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Drag bar position:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Top"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Bottom"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Left"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetText(di, _("Right"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_dragdir);
}

const DialogDef     DlgDesks = {
   "CONFIGURE_DESKTOPS",
   N_("Desks"),
   N_("Multiple Desktop Settings"),
   "SOUND_SETTINGS_DESKTOPS",
   "pix/desktops.png",
   N_("Enlightenment Multiple Desktop\n" "Settings Dialog\n"),
   _DlgFillDesks,
   DLG_OAC, CB_ConfigureDesktops,
};

static int          tmp_area_x;
static int          tmp_area_y;
static int          tmp_edge_flip;
static int          tmp_edge_resist;
static DItem       *tmp_area_text;
static char         tmp_area_wraparound;

static void
CB_ConfigureAreas(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	SetNewAreaSize(tmp_area_x, 9 - tmp_area_y);
	Conf.desks.areas_wraparound = tmp_area_wraparound;
	Conf.desks.edge_flip_mode = tmp_edge_flip;
	if (tmp_edge_resist < 1)
	   tmp_edge_resist = 1;
	Conf.desks.edge_flip_resistance = tmp_edge_resist;
     }
   autosave();
}

static void
CB_AreaDisplayRedraw(Dialog * d __UNUSED__, int val, void *data)
{
   static int          prev_ax = 0, prev_ay = 0;
   static char         called = 0;
   static Win          awin;
   char                s[64];
   DItem              *di;
   Win                 win;
   int                 w, h;

   if (val == 1)
      called = 0;

   if ((val != 1) && ((prev_ax == tmp_area_x) && (prev_ay == tmp_area_y)))
      return;

   prev_ax = tmp_area_x;
   prev_ay = tmp_area_y;

   di = (DItem *) data;
   win = DialogItemAreaGetWindow(di);
   DialogItemAreaGetSize(di, &w, &h);

   if (!called)
     {
	ImageClass         *ic;

	ic = ImageclassFind("SETTINGS_AREA_AREA", 1);
	ImageclassApply(ic, win, 0, 0, STATE_NORMAL, ST_SOLID);

	awin = ECreateWindow(win, 0, 0, 18, 14, 0);
	ic = ImageclassFind("SETTINGS_AREADESK_AREA", 1);
	ImageclassApply(ic, awin, 0, 0, STATE_NORMAL, ST_SOLID);

	called = 1;
     }
   EMoveResizeWindow(awin, ((w / 2) - (9 * tmp_area_x)),
		     ((h / 2) - (7 * (9 - tmp_area_y))), 18 * tmp_area_x,
		     14 * (9 - tmp_area_y));
   EMapWindow(awin);

   if ((tmp_area_x > 1) || ((9 - tmp_area_y) > 1))
      Esnprintf(s, sizeof(s), _("%i x %i\nScreens in size"), tmp_area_x,
		9 - tmp_area_y);
   else
      Esnprintf(s, sizeof(s), _("1\nScreen in size"));
   DialogItemSetText(tmp_area_text, s);
   DialogDrawItems(d, tmp_area_text, 0, 0, 99999, 99999);
}

static void
CB_AreaDisplayAreaRedraw(DItem * di, int val __UNUSED__, void *data __UNUSED__)
{
   CB_AreaDisplayRedraw(DialogItemGetDialog(di), 1, di);
}

static void
_DlgFillAreas(Dialog * d __UNUSED__, DItem * table, void *data __UNUSED__)
{
   DItem              *di, *slider, *slider2, *table2, *radio;
   char                s[64];

   tmp_area_wraparound = Conf.desks.areas_wraparound;

   tmp_edge_flip = Conf.desks.edge_flip_mode;
   tmp_edge_resist = Conf.desks.edge_flip_resistance;

   DesksGetAreaSize(&tmp_area_x, &tmp_area_y);
   tmp_area_y = 9 - tmp_area_y;

   DialogItemTableSetOptions(table, 1, 0, 0, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetText(di, _("Virtual Desktop size:\n"));

   di = tmp_area_text = DialogAddItem(table, DITEM_TEXT);
   if ((tmp_area_x > 1) || (tmp_area_y > 1))
      Esnprintf(s, sizeof(s), _("%i x %i\nScreens in size"), tmp_area_x,
		9 - tmp_area_y);
   else
      Esnprintf(s, sizeof(s), _("1\nScreen in size"));
   DialogItemSetText(di, s);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemTableSetOptions(table2, 2, 0, 0, 0);

   di = DialogAddItem(table2, DITEM_NONE);

   di = slider = DialogAddItem(table2, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetBounds(di, 1, 8);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSliderSetValPtr(di, &tmp_area_x);

   di = slider2 = DialogAddItem(table2, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetOrientation(di, 0);
   DialogItemSetFill(di, 0, 1);
   DialogItemSliderSetBounds(di, 1, 8);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSliderSetValPtr(di, &tmp_area_y);

   di = DialogAddItem(table2, DITEM_AREA);
   DialogItemAreaSetSize(di, 160, 120);
   DialogItemAreaSetInitFunc(di, CB_AreaDisplayAreaRedraw);

   DialogItemSetCallback(slider, CB_AreaDisplayRedraw, 0, di);
   DialogItemSetCallback(slider2, CB_AreaDisplayRedraw, 0, di);

   di = DialogAddItem(table, DITEM_SEPARATOR);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetText(di, _("Wrap virtual desktops around"));
   DialogItemCheckButtonSetPtr(di, &tmp_area_wraparound);

   di = DialogAddItem(table, DITEM_SEPARATOR);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetAlign(di, 0, 512);
   DialogItemSetText(di, _("Edge Flip Mode:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Off"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, EDGE_FLIP_OFF);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("On"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, EDGE_FLIP_ON);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetText(di, _("Only when moving window"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, EDGE_FLIP_MOVE);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_edge_flip);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetText(di, _("Resistance at edge of screen:\n"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSliderSetMinLength(di, 10);
   DialogItemSliderSetBounds(di, 1, 100);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 10);
   DialogItemSliderSetValPtr(di, &tmp_edge_resist);
}

const DialogDef     DlgAreas = {
   "CONFIGURE_AREA",
   N_("Areas"),
   N_("Virtual Desktop Settings"),
   "SOUND_SETTINGS_AREA",
   "pix/areas.png",
   N_("Enlightenment Virtual Desktop\n" "Settings Dialog\n"),
   _DlgFillAreas,
   DLG_OAC, CB_ConfigureAreas,
};

/*
 * IPC functions
 */

static void
DeskOpGoto(unsigned int desk)
{
   Desk               *dsk;
   Desk               *pd = DesksGetCurrent();

   if (desk >= Conf.desks.num)
      return;

   dsk = _DeskGet(desk);

   DeskGoto(dsk);

   if (DesksGetCurrent() != pd)
      SoundPlay("SOUND_DESKTOP_SHUT");
}

static void
DeskOpGotoRel(int drel)
{
   int                 desk;

   desk = (int)DesksGetCurrentNum() + drel;
   if (Conf.desks.desks_wraparound)
      desk = (desk + Conf.desks.num) % Conf.desks.num;

   DeskOpGoto((unsigned int)desk);
}

static void
DeskOpDrag(int desk)
{
   DeskDragStart(desk);
}

static void
DesksIpcDesk(const char *params)
{
   const char         *p;
   char                cmd[128], prm[128];
   int                 len;
   unsigned int        desk;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %100s %n", cmd, prm, &len);
	p += len;
     }

   desk = DesksGetCurrentNum();

   if (!p || cmd[0] == '?')
     {
	IpcPrintf("Current Desktop: %d/%d\n", desk, Conf.desks.num);
     }
   else if (!strncmp(cmd, "cfg", 3))
     {
	DialogShowSimple(&DlgDesks, NULL);
     }
   else if (!strncmp(cmd, "set", 3))
     {
	sscanf(prm, "%i", &desk);
	ChangeNumberOfDesktops(desk);
     }
   else if (!strncmp(cmd, "list", 2))
     {
	Desk               *dsk;

	for (desk = 0; desk < Conf.desks.num; desk++)
	  {
	     dsk = _DeskGet(desk);
	     IpcPrintf
		("Desk %d: viewable=%d order=%d  x,y=%4d,%4d wxh=%4dx%4d  area x,y=%d,%d  pmap=%#lx\n",
		 desk, dsk->viewable, desks.order[desk],
		 EoGetX(dsk), EoGetY(dsk), EoGetW(dsk), EoGetH(dsk),
		 dsk->current_area_x, dsk->current_area_y, dsk->bg.pmap);
	  }
     }
   else if (!strncmp(cmd, "goto", 2))
     {
	sscanf(prm, "%i", &desk);
	DeskOpGoto(desk);
     }
   else if (!strncmp(cmd, "next", 2))
     {
	DeskOpGotoRel(1);
     }
   else if (!strncmp(cmd, "prev", 2))
     {
	DeskOpGotoRel(-1);
     }
   else if (!strncmp(cmd, "this", 2))
     {
	DeskOpGotoRel(0);
     }
   else if (!strncmp(cmd, "raise", 2))
     {
	sscanf(prm, "%i", &desk);
	SoundPlay("SOUND_DESKTOP_RAISE");
	DeskRaise(desk);
     }
   else if (!strncmp(cmd, "lower", 2))
     {
	sscanf(prm, "%i", &desk);
	SoundPlay("SOUND_DESKTOP_LOWER");
	DeskLower(desk);
     }
   else if (!strcmp(cmd, "drag"))
     {
	if (prm[0])
	   desk = atoi(prm);
	DeskOpDrag(desk);
     }
   else if (!strcmp(cmd, "dragbar"))
     {
	if (!strncmp(prm, "dir", 3))
	   DeskDragdirSet(p);
	else if (!strncmp(prm, "ord", 3))
	   DeskDragbarOrderSet(p);
     }
   else if (!strcmp(cmd, "clear"))
     {
	EwinsShowDesktop(!Mode.showing_desktop);
     }
   else if (!strncmp(cmd, "arrange", 3))
     {
	ArrangeEwins(prm);
     }
}

static void
DesksIpcArea(const char *params)
{
   const char         *p;
   char                cmd[128], prm[128];
   int                 len;
   int                 ax, ay, dx, dy;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %100s %n", cmd, prm, &len);
	p += len;
     }

   DeskCurrentGetArea(&ax, &ay);

   if (!p || cmd[0] == '?')
     {
	IpcPrintf("Current Area: %d %d\n", ax, ay);
     }
   else if (!strncmp(cmd, "cfg", 3))
     {
	DialogShowSimple(&DlgAreas, NULL);
     }
   else if (!strncmp(cmd, "set", 3))
     {
	sscanf(params, "%*s %i %i", &ax, &ay);
	SetNewAreaSize(ax, ay);
     }
   else if (!strncmp(cmd, "goto", 2))
     {
	sscanf(params, "%*s %i %i", &ax, &ay);
	DeskCurrentGotoArea(ax, ay);
     }
   else if (!strncmp(cmd, "move", 2))
     {
	dx = dy = 0;
	sscanf(params, "%*s %i %i", &dx, &dy);
	DeskCurrentMoveAreaBy(dx, dy);
     }
   else if (!strncmp(cmd, "lgoto", 2))
     {
	sscanf(params, "%*s %i", &ax);
	SetCurrentLinearArea(ax);
     }
   else if (!strncmp(cmd, "lmove", 2))
     {
	dx = 0;
	sscanf(params, "%*s %i", &dx);
	MoveCurrentLinearAreaBy(dx);
     }
}

static const IpcItem DesksIpcArray[] = {
   {
    DesksIpcDesk,
    "desk", NULL,
    "Desktop functions",
    "  desk ?               Desktop info\n"
    "  desk cfg             Configure desktops\n"
    "  desk drag            Start deskdrag\n"
    "  desk set <nd>        Set number of desktops\n"
    "  desk goto <d>        Goto specified desktop\n"
    "  desk list            Show desk info\n"
    "  desk next            Goto next desktop\n"
    "  desk prev            Goto previous desktop\n"
    "  desk this            Goto this desktop\n"
    "  desk lower <d>       Lower desktop\n"
    "  desk raise <d>       Raise desktop\n"
    "  desk dragbar pos     Set dragbar position\n"
    "  desk dragbar order   Set dragbar button order\n"
    "  desk arrange         Arrange windows on desktop\"\n"
    "  desk clear           \"Show Desktop\"\n"}
   ,
   {
    DesksIpcArea,
    "area", NULL,
    "Area functions",
    "  area ?               Area info\n"
    "  area cfg             Configure areas\n"
    "  area set <nx> <ny>   Set area size\n"
    "  area goto <ax> <ay>  Goto specified area\n"
    "  area move <dx> <dy>  Move relative to current area\n"
    "  area lgoto <al>      Goto specified linear area\n"
    "  area lmove <dl>      Move relative to current linear area\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(DesksIpcArray)/sizeof(IpcItem))

static void
DesksCfgFuncCount(void *item __UNUSED__, const char *value)
{
   ChangeNumberOfDesktops(atoi(value));
}

static void
DesksCfgFuncDragdir(void *item __UNUSED__, const char *value)
{
   DeskDragdirSet(value);
}

static void
DesksCfgFuncDragdbarOrder(void *item __UNUSED__, const char *value)
{
   DeskDragbarOrderSet(value);
}

static void
AreasCfgFuncSizeX(void *item __UNUSED__, const char *value)
{
   int                 ax, ay;

   DesksGetAreaSize(&ax, &ay);
   SetNewAreaSize(atoi(value), ay);
}

static void
AreasCfgFuncSizeY(void *item __UNUSED__, const char *value)
{
   int                 ax, ay;

   DesksGetAreaSize(&ax, &ay);
   SetNewAreaSize(ax, atoi(value));
}

static const CfgItem DesksCfgItems[] = {
   CFG_FUNC_INT(Conf.desks, num, 2, DesksCfgFuncCount),
   CFG_FUNC_INT(Conf.desks, dragdir, 2, DesksCfgFuncDragdir),
   CFG_ITEM_INT(Conf.desks, dragbar_width, 16),
   CFG_ITEM_INT(Conf.desks, dragbar_length, 0),
   CFG_FUNC_INT(Conf.desks, dragbar_ordering, 1, DesksCfgFuncDragdbarOrder),
   CFG_ITEM_BOOL(Conf.desks, desks_wraparound, 0),
   CFG_ITEM_BOOL(Conf.desks, slidein, 1),
   CFG_ITEM_INT(Conf.desks, slidespeed, 6000),

   CFG_FUNC_INT(Conf.desks, areas_nx, 2, AreasCfgFuncSizeX),
   CFG_FUNC_INT(Conf.desks, areas_ny, 1, AreasCfgFuncSizeY),
   CFG_ITEM_BOOL(Conf.desks, areas_wraparound, 0),

   CFG_ITEM_INT(Conf.desks, edge_flip_mode, EDGE_FLIP_ON),
   CFG_ITEM_INT(Conf.desks, edge_flip_resistance, 25),
};
#define N_CFG_ITEMS (sizeof(DesksCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
extern const EModule ModDesktops;
const EModule       ModDesktops = {
   "desktops", "desk",
   DesksSighan,
   {N_IPC_FUNCS, DesksIpcArray},
   {N_CFG_ITEMS, DesksCfgItems}
};
