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
#include "borders.h"
#include "cursors.h"
#include "desktops.h"
#include "ecompmgr.h"
#include "emodule.h"
#include "eobj.h"
#include "events.h"
#include "ewins.h"
#include "focus.h"
#include "grabs.h"
#include "groups.h"
#include "hints.h"
#include "snaps.h"
#include "timers.h"
#include "windowmatch.h"
#include "xwin.h"
#include <X11/Xutil.h>
#include <X11/extensions/shape.h>

#define EWIN_TOP_EVENT_MASK \
  (/* ButtonPressMask | ButtonReleaseMask | */ \
   EnterWindowMask | LeaveWindowMask /* | PointerMotionMask */ /* | \
   StructureNotifyMask */)

#define EWIN_CONTAINER_EVENT_MASK \
  (SubstructureNotifyMask | SubstructureRedirectMask)

#define EWIN_CLIENT_EVENT_MASK \
  (/* EnterWindowMask | LeaveWindowMask | */ FocusChangeMask | \
   /* StructureNotifyMask | */ ResizeRedirectMask | \
   PropertyChangeMask | ColormapChangeMask | VisibilityChangeMask)

static int          EwinSlideIn(void *data);

static void         EwinChangesStart(EWin * ewin);
static void         EwinChangesProcess(EWin * ewin);

static void         EwinHandleEventsToplevel(Win win, XEvent * ev, void *prm);
static void         EwinHandleEventsContainer(Win win, XEvent * ev, void *prm);
static void         EwinHandleEventsClient(Win win, XEvent * ev, void *prm);

static void         EwinUnmap2(EWin * ewin);

Window
EwinGetClientXwin(const EWin * ewin)
{
   Win                 win = EwinGetClientWin(ewin);

   return (win) ? WinGetXwin(win) : None;
}

Window
EwinGetContainerXwin(const EWin * ewin)
{
   Win                 win = EwinGetContainerWin(ewin);

   return (win) ? WinGetXwin(win) : None;
}

static EWin        *
EwinCreate(int type)
{
   EWin               *ewin;

   ewin = ECALLOC(EWin, 1);

   ewin->type = type;
   ewin->state.state = (Mode.wm.startup) ? EWIN_STATE_STARTUP : EWIN_STATE_NEW;

   ewin->o.stacked = -1;	/* Not placed on desk yet */
   EoSetDesk(ewin, DesksGetCurrent());
   EoSetLayer(ewin, 4);
   EoSetFade(ewin, 1);
   EoSetShadow(ewin, 1);

   ewin->update.shape = 1;
   ewin->update.border = 1;
   ewin->save_max.x = ewin->save_max.y = ewin->save_max.w = ewin->save_max.h =
      -1;
   ewin->save_fs.x = ewin->save_fs.y = ewin->save_fs.w = ewin->save_fs.h = -1;
   ewin->save_fs.layer = -1;

   ewin->icccm.need_input = 1;

   ewin->icccm.width_min = 0;
   ewin->icccm.height_min = 0;
   ewin->icccm.width_max = 65535;
   ewin->icccm.height_max = 65535;
   ewin->icccm.base_w = 0;
   ewin->icccm.base_h = 0;
   ewin->icccm.w_inc = 1;
   ewin->icccm.h_inc = 1;
   ewin->icccm.aspect_min = 0.0;
   ewin->icccm.aspect_max = 65535.0;
   ewin->icccm.grav = NorthWestGravity;

#if 0				/* ENABLE_GNOME - Not actually used */
   ewin->expanded_width = -1;
   ewin->expanded_height = -1;
#endif
   ewin->area_x = -1;
   ewin->area_y = -1;

   ewin->place.gravity = -1;

   ewin->ewmh.opacity = 0;	/* If 0, ignore */
   ewin->props.focused_opacity = 0;

   return ewin;
}

static int
EwinGetAttributes(EWin * ewin, Win win, Window xwin)
{
   XWindowAttributes   xwa;

   if (!win)
     {
	win = ERegisterWindow(xwin, NULL);
	if (!win)
	   return -1;
     }

   EGetWindowAttributes(win, &xwa);

   ewin->client.win = win;
   ewin->client.x = ewin->save_max.x = ewin->save_fs.x = xwa.x;
   ewin->client.y = ewin->save_max.y = ewin->save_fs.y = xwa.y;
   ewin->client.w = ewin->save_max.w = ewin->save_fs.w = xwa.width;
   ewin->client.h = ewin->save_max.h = ewin->save_fs.h = xwa.height;
   ewin->client.bw = xwa.border_width;
   ewin->client.cmap = xwa.colormap;

   if (EDebug(EDBUG_TYPE_SNAPS))
      Eprintf("Snap get attr  %#lx: %4d+%4d %4dx%4d: %s\n",
	      EwinGetClientXwin(ewin), ewin->client.x, ewin->client.y,
	      ewin->client.w, ewin->client.h, EwinGetTitle(ewin));

   return 0;
}

static void
EwinGetHints(EWin * ewin)
{
   if (EDebug(EDBUG_TYPE_EWINS))
      Eprintf("EwinGetHints %#lx\n", EwinGetClientXwin(ewin));

   ICCCM_GetTitle(ewin);
   ICCCM_GetHints(ewin);
   ICCCM_GetGeoms(ewin);
   MWM_GetHints(ewin, 0);
   ICCCM_GetInfo(ewin);		/* NB! Need group info first */
   HintsGetWindowHints(ewin);
   SessionGetInfo(ewin);
}

static void
EwinHintsInferProps(EWin * ewin)
{
   if (ewin->ewmh.type.b.desktop)
     {
	EoSetLayer(ewin, 0);
	if (!ewin->state.identified)
	   EoSetSticky(ewin, 1);
	ewin->props.focusclick = 1;
	ewin->props.skip_focuslist = 1;
	EwinInhSetUser(ewin, move, 1);
	EwinInhSetUser(ewin, size, 1);
	ewin->props.donthide = 1;
	ewin->props.no_border = 1;
     }
   if (ewin->ewmh.type.b.dock)
     {
	ewin->props.skip_ext_task = 1;
	ewin->props.skip_winlist = 1;
	ewin->props.skip_focuslist = 1;
	if (!ewin->state.identified)
	   EoSetSticky(ewin, 1);
	ewin->props.donthide = 1;
     }
   if (ewin->ewmh.type.b.utility)
     {
	/* Epplets hit this */
	ewin->props.skip_ext_task = 1;
	ewin->props.skip_winlist = 1;
	ewin->props.skip_focuslist = 1;
	ewin->props.never_use_area = 1;
	ewin->props.donthide = 1;
     }
}

static void
EwinManage(EWin * ewin)
{
   XSetWindowAttributes att;
   Win                 frame;
   int                 type;

   if (ewin->client.w <= 0)
      ewin->client.w = 100;
   if (ewin->client.h <= 0)
      ewin->client.h = 100;

   if (ewin->state.docked)
      ewin->inh_wm.b.border = 1;

   frame = EoGetWin(ewin);
   if (!frame)
     {
	type = (ewin->props.no_argb) ? WIN_TYPE_NO_ARGB : WIN_TYPE_CLIENT;
	frame =
	   ECreateObjectWindow(VROOT, ewin->client.x, ewin->client.y,
			       ewin->client.w, ewin->client.h, 0, type,
			       EwinGetClientWin(ewin));
	ewin->win_container =
	   ECreateWindow(frame, 0, 0, ewin->client.w, ewin->client.h, 0);

	EoInit(ewin, EOBJ_TYPE_EWIN, frame, ewin->client.x, ewin->client.y,
	       ewin->client.w, ewin->client.h, 1, NULL);

	EobjListFocusAdd(&ewin->o, 1);
	EobjListOrderAdd(&ewin->o);

	EventCallbackRegister(EoGetWin(ewin), 0,
			      EwinHandleEventsToplevel, ewin);
	EventCallbackRegister(ewin->win_container, 0,
			      EwinHandleEventsContainer, ewin);
	EventCallbackRegister(EwinGetClientWin(ewin), 0,
			      EwinHandleEventsClient, ewin);
     }

   att.event_mask = EWIN_CONTAINER_EVENT_MASK;
   att.do_not_propagate_mask = ButtonPressMask | ButtonReleaseMask;
   EChangeWindowAttributes(ewin->win_container,
			   CWEventMask | CWDontPropagate, &att);
   EMapWindow(ewin->win_container);

   att.event_mask = EWIN_TOP_EVENT_MASK;
   att.do_not_propagate_mask =
      ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
   EChangeWindowAttributes(EoGetWin(ewin), CWEventMask | CWDontPropagate, &att);

   ewin->client.event_mask = EWIN_CLIENT_EVENT_MASK;
   ESelectInput(EwinGetClientWin(ewin), ewin->client.event_mask);

   if (EDebug(EDBUG_TYPE_EWINS))
      Eprintf("EwinManage %#lx frame=%#lx cont=%#lx st=%d\n",
	      EwinGetClientXwin(ewin), EoGetXwin(ewin),
	      EwinGetContainerXwin(ewin), ewin->state.state);

   if (!EwinIsInternal(ewin))
     {
	XShapeSelectInput(disp, EwinGetClientXwin(ewin), ShapeNotifyMask);
	ESetWindowBorderWidth(EwinGetClientWin(ewin), 0);
	ewin->client.bw = 0;
     }

   ICCCM_AdoptStart(ewin);

   /* We must reparent after getting original window position */
   EReparentWindow(EwinGetClientWin(ewin), ewin->win_container, 0, 0);

   EwinUpdateShapeInfo(ewin);

   ModulesSignal(ESIGNAL_EWIN_CREATE, ewin);
}

/*
 * Derive frame window geometry from client window properties
 */
static void
EwinSetGeometry(EWin * ewin)
{
   int                 x, y, l, r, t, b;
   int                 grav;

   grav = (ewin->state.identified) ? StaticGravity : ewin->icccm.grav;

   EwinGetPosition(ewin, ewin->client.x, ewin->client.y, grav, &x, &y);

   l = ewin->border->border.left;
   r = ewin->border->border.right;
   t = ewin->border->border.top;
   b = ewin->border->border.bottom;

   ewin->client.x = x + l;
   ewin->client.y = y + t;

   EoMoveResize(ewin, x, y, ewin->client.w + l + r, ewin->client.h + t + b);
}

static void
EwinConfigure(EWin * ewin)
{
   EwinStateUpdate(ewin);

   if (!EwinIsInternal(ewin) && Mode.wm.startup)
      EHintsGetInfo(ewin);	/* E restart hints */
   EwinHintsInferProps(ewin);
   SnapshotEwinApply(ewin);	/* Apply saved settings */

   if (ewin->save_fs.layer < 0)
      ewin->save_fs.layer = EoGetLayer(ewin);

   EwinStateUpdate(ewin);	/* Update after snaps etc. */

   ICCCM_Adopt(ewin);

   EwinBorderSelect(ewin);	/* Select border before calculating geometry */
   EwinSetGeometry(ewin);	/* Calculate window geometry before border parts */
   EwinBorderSetTo(ewin, NULL);

   if (!ewin->props.no_button_grabs)
      GrabButtonGrabs(ewin);

   if (ewin->state.shaded)
      EwinInstantShade(ewin, 1);

   EwinUpdateOpacity(ewin);

   HintsSetWindowState(ewin);
   HintsSetWindowOpacity(ewin);

   HintsSetClientList();

   if (EDebug(EDBUG_TYPE_EWINS))
      Eprintf("EwinConfigure %#lx st=%d: %s\n", EwinGetClientXwin(ewin),
	      ewin->state.state, EwinGetTitle(ewin));
}

static void
EwinCleanup(EWin * ewin)
{
   EwinBorderDetach(ewin);
}

static void
EwinDestroy(EWin * ewin)
{
   EWin              **lst;
   int                 i, num;

   if (!ewin)
      return;

   if (ewin->state.state == EWIN_STATE_MAPPED)
      EwinUnmap2(ewin);

   if (EDebug(EDBUG_TYPE_EWINS))
      Eprintf("EwinDestroy %#lx st=%d: %s\n", EwinGetClientXwin(ewin),
	      ewin->state.state, EwinGetTitle(ewin));

   EventCallbackUnregister(EoGetWin(ewin), 0, EwinHandleEventsToplevel, ewin);
   EventCallbackUnregister(ewin->win_container, 0, EwinHandleEventsContainer,
			   ewin);
   EventCallbackUnregister(EwinGetClientWin(ewin), 0, EwinHandleEventsClient,
			   ewin);
   if (!EwinIsInternal(ewin))
      EUnregisterWindow(EwinGetClientWin(ewin));

   SnapshotEwinUnmatch(ewin);

   ModulesSignal(ESIGNAL_EWIN_DESTROY, ewin);

   lst = EwinListTransientFor(ewin, &num);
   for (i = 0; i < num; i++)
     {
	lst[i]->icccm.transient_count--;
	if (lst[i]->icccm.transient_count < 0)	/* Paranoia? */
	   lst[i]->icccm.transient_count = 0;
     }
   Efree(lst);

   EwinCleanup(ewin);
   EobjListOrderDel(&ewin->o);
   EobjListFocusDel(&ewin->o);
   EoFini(ewin);

   HintsSetClientList();

   Efree(ewin->icccm.wm_icon_name);
   Efree(ewin->icccm.wm_role);
   Efree(ewin->icccm.wm_command);
   Efree(ewin->icccm.wm_machine);
   Efree(ewin->ewmh.wm_name);
   Efree(ewin->ewmh.wm_icon_name);
   Efree(ewin->ewmh.wm_icon);
   Efree(ewin->bits);
   Efree(ewin->session_id);
   FreePmapMask(&ewin->mini_pmm);
   GroupsEwinRemove(ewin);

   Efree(ewin);
}

void
DetermineEwinFloat(EWin * ewin, int dx, int dy)
{
   char                dofloat = 0;
   int                 x, y, w, h, xd, yd;
   Desk               *dsk;

   dsk = EoGetDesk(ewin);
   x = EoGetX(ewin);
   y = EoGetY(ewin);
   w = EoGetW(ewin);
   h = EoGetH(ewin);

   xd = EoGetX(dsk);
   yd = EoGetY(dsk);

   if ((dsk->num != 0) && (EoIsFloating(ewin) < 2) &&
       ((xd != 0) || (yd != 0) || (DesksGetCurrent() != dsk)))
     {
	switch (Conf.desks.dragdir)
	  {
	  case 0:
	     if (((x + dx < 0) ||
		  ((x + dx + w <= WinGetW(VROOT)) &&
		   (DesktopAt(xd + x + dx + w - 1, yd) != dsk))))
		dofloat = 1;
	     break;
	  case 1:
	     if (((x + dx + w > WinGetW(VROOT)) ||
		  ((x + dx >= 0) && (DesktopAt(xd + x + dx, yd) != dsk))))
		dofloat = 1;
	     break;
	  case 2:
	     if (((y + dy < 0) ||
		  ((y + dy + h <= WinGetH(VROOT)) &&
		   (DesktopAt(xd, yd + y + dy + h - 1) != dsk))))
		dofloat = 1;
	     break;
	  case 3:
	     if (((y + dy + h > WinGetH(VROOT)) ||
		  ((y + dy >= 0) && (DesktopAt(xd, yd + y + dy) != dsk))))
		dofloat = 1;
	     break;
	  }

	if (dofloat)
	   EwinOpFloatAt(ewin, OPSRC_USER, x + xd, y + yd);
     }
}

EWin               *
GetEwinPointerInClient(void)
{
   int                 px, py;
   EWin               *const *lst, *ewin;
   int                 i, num;
   Desk               *dsk;

   dsk = DesktopAt(Mode.events.cx, Mode.events.cy);
   EQueryPointer(EoGetWin(dsk), &px, &py, NULL, NULL);

   lst = EwinListGetForDesk(&num, dsk);
   for (i = 0; i < num; i++)
     {
	int                 x, y, w, h;

	ewin = lst[i];
	x = EoGetX(ewin);
	y = EoGetY(ewin);
	w = EoGetW(ewin);
	h = EoGetH(ewin);
	if ((px >= x) && (py >= y) && (px < (x + w)) && (py < (y + h)) &&
	    EwinIsMapped(ewin))
	   return ewin;
     }

   return NULL;
}

EWin               *
GetFocusEwin(void)
{
   return Mode.focuswin;
}

EWin               *
GetContextEwin(void)
{
   EWin               *ewin;

   ewin = Mode.context_ewin;
   if (ewin)
      goto done;

   ewin = NULL;

 done:
#if 0
   Eprintf("GetContextEwin %#lx %s\n", EwinGetClientXwin(ewin),
	   EwinGetTitle(ewin));
#endif
   return ewin;
}

void
SetContextEwin(EWin * ewin)
{
   if (ewin && ewin->type == EWIN_TYPE_MENU)
      return;
#if 0
   Eprintf("SetContextEwin %#lx %s\n", EwinGetClientXwin(ewin),
	   EwinGetTitle(ewin));
#endif
   Mode.context_ewin = ewin;
}

/*
 * Derive frame window position from client window and border properties
 */
void
EwinGetPosition(const EWin * ewin, int x, int y, int grav, int *px, int *py)
{
   int                 bw, bd_lr, bd_tb;

   bw = ewin->client.bw;
   bd_lr = ewin->border->border.left + ewin->border->border.right;
   bd_tb = ewin->border->border.top + ewin->border->border.bottom;

   if (grav == 0)
      grav = ewin->icccm.grav;

   switch (grav)
     {
     case NorthWestGravity:
     case WestGravity:
     case SouthWestGravity:
	x -= bw;
	break;
     case NorthGravity:
     case CenterGravity:
     case SouthGravity:
	x -= bd_lr / 2;
	break;
     case NorthEastGravity:
     case EastGravity:
     case SouthEastGravity:
	x -= bd_lr - bw;
	break;
     case StaticGravity:
	x -= ewin->border->border.left;
	break;
     default:
	break;
     }

   switch (grav)
     {
     case NorthWestGravity:
     case NorthGravity:
     case NorthEastGravity:
	y -= bw;
	break;
     case WestGravity:
     case CenterGravity:
     case EastGravity:
	y -= bd_tb / 2;
	break;
     case SouthWestGravity:
     case SouthGravity:
     case SouthEastGravity:
	y -= bd_tb - bw;
	break;
     case StaticGravity:
	y -= ewin->border->border.top;
	break;
     default:
	break;
     }

   *px = x;
   *py = y;
}

void
EwinUpdateShapeInfo(EWin * ewin)
{
   EGrabServer();
   ewin->state.shaped =
      EShapeSetShape(ewin->win_container, 0, 0, EwinGetClientWin(ewin));
   EUngrabServer();

   if (EDebug(EX_EVENT_SHAPE_NOTIFY))
      Eprintf("EwinUpdateShapeInfo %#lx cont=%#lx shaped=%d\n",
	      EwinGetClientXwin(ewin), EwinGetContainerXwin(ewin),
	      ewin->state.shaped);
}

void
EwinPropagateShapes(EWin * ewin)
{
   if (!EoIsShown(ewin))
      return;

   if (ewin->state.docked)
      return;

   if (!ewin->update.shape)
      return;

   if (EDebug(EX_EVENT_SHAPE_NOTIFY))
      Eprintf("EwinPropagateShapes %#lx frame=%#lx shaped=%d\n",
	      EwinGetClientXwin(ewin), EoGetXwin(ewin), ewin->state.shaped);

   EoShapeUpdate(ewin, 1);
   ewin->update.shape = 0;
}

void
EwinStateUpdate(EWin * ewin)
{
   ewin->state.inhibit_actions = ewin->props.no_actions;
   ewin->state.inhibit_focus = !ewin->icccm.need_input ||
      EwinInhGetWM(ewin, focus) || ewin->state.iconified;

   ewin->state.inhibit_move =
      EwinInhGetUser(ewin, move) || ewin->state.fullscreen;
   ewin->state.inhibit_resize = ewin->state.iconified || ewin->state.shaded ||
      (ewin->props.no_resize_h && ewin->props.no_resize_v) ||
      EwinInhGetUser(ewin, size) || ewin->state.fullscreen;
   ewin->state.inhibit_iconify = EwinInhGetWM(ewin, iconify);
   ewin->state.inhibit_shade = ewin->state.no_border ||
      ewin->state.iconified || ewin->state.fullscreen;
   ewin->state.inhibit_stick = 0;
   ewin->state.inhibit_max_hor = ewin->state.inhibit_resize ||
      ewin->props.no_resize_h || ewin->state.fullscreen;
   ewin->state.inhibit_max_ver = ewin->state.inhibit_resize ||
      ewin->props.no_resize_v || ewin->state.fullscreen;
   ewin->state.inhibit_fullscreeen =
      ewin->state.inhibit_move || ewin->state.inhibit_resize;
   ewin->state.inhibit_change_desk = ewin->state.iconified;
   ewin->state.inhibit_close = EwinInhGetApp(ewin, close) ||
      EwinInhGetUser(ewin, close);

   ewin->state.donthide = ewin->props.donthide ||
       ewin->props.skip_ext_task || ewin->props.skip_winlist ||
       ewin->props.skip_focuslist;

   SnapshotEwinUpdate(ewin, SNAP_USE_FLAGS);
}

static void
AddToFamily(EWin * ewin, Window xwin)
{
   EWin               *ewin2;
   EWin              **lst;
   int                 i, k, num, fx, fy, x, y;
   char                doslide, manplace;
   Desk               *dsk;

   EGrabServer();

   if (ewin)
      EwinCleanup(ewin);
   else
      ewin = EwinCreate(EWIN_TYPE_NORMAL);
   if (!ewin)
      goto done;

   if (EwinGetAttributes(ewin, NULL, xwin))
     {
	Eprintf("Window is gone %#lx\n", xwin);
	/* We got here by MapRequest. DestroyNotify should follow. */
	goto done;
     }

   EwinGetHints(ewin);
   WindowMatchEwinOps(ewin);	/* Window matches */
   EwinManage(ewin);
   EwinConfigure(ewin);

   /* if it hasn't been planted on a desktop - assign it the current desktop */
   dsk = EoGetDesk(ewin);

   /* if is an afterstep/windowmaker dock app - dock it */
   if (Conf.dock.enable && ewin->state.docked)
      DockIt(ewin);

   ewin2 = NULL;
   if (ewin->icccm.transient)
     {
	if (ewin->icccm.transient_for == None ||
	    ewin->icccm.transient_for == WinGetXwin(VROOT))
	  {
	     /* Group transient */
	     ewin->icccm.transient_for = WinGetXwin(VROOT);
#if 0				/* Maybe? */
	     ewin->layer++;
#endif
	     /* Don't treat this as a normal transient */
	     ewin->icccm.transient = -1;
	  }
	else if (ewin->icccm.transient_for == EwinGetClientXwin(ewin))
	  {
	     /* Some apps actually do this. Why? */
	     ewin->icccm.transient = 0;
	  }
	else
	  {
	     /* Regular transient */
	  }

	if (ewin->icccm.transient)
	  {
	     /* Tag the parent window if this is a transient */
	     lst = EwinListTransientFor(ewin, &num);
	     for (i = 0; i < num; i++)
	       {
		  lst[i]->icccm.transient_count++;
		  if (EoGetLayer(ewin) < EoGetLayer(lst[i]))
		     EoSetLayer(ewin, EoGetLayer(lst[i]));
	       }
	     if (lst)
	       {
		  ewin2 = lst[0];
		  EoSetSticky(ewin, EoIsSticky(lst[0]));
		  Efree(lst);
	       }
	     else
	       {
		  /* No parents? - not a transient */
		  ewin->icccm.transient = 0;
	       }
	  }
     }

   x = EoGetX(ewin);
   y = EoGetY(ewin);
   if (ewin->icccm.transient && Conf.focus.transientsfollowleader)
     {
	EWin               *const *lst2;

	if (!ewin2)
	   ewin2 = EwinFindByClient(ewin->icccm.group);

	if (!ewin2)
	  {
	     lst2 = EwinListGetAll(&num);
	     for (i = 0; i < num; i++)
	       {
		  if ((lst2[i]->state.iconified) ||
		      (ewin->icccm.group != lst2[i]->icccm.group))
		     continue;

		  ewin2 = lst2[i];
		  break;
	       }
	  }

	if (ewin2)
	  {
	     dsk = EoGetDesk(ewin2);
	     if (!Mode.wm.startup && Conf.focus.switchfortransientmap &&
		 !ewin->state.iconified)
		DeskGotoByEwin(ewin2);
	  }
     }

   if (ewin->state.fullscreen)
     {
	EwinOpFullscreen(ewin, OPSRC_WM, 2);
	ewin->state.placed = 1;
	EwinMoveToDesktopAt(ewin, dsk, EoGetX(ewin), EoGetY(ewin));
	EwinShow(ewin);
	goto done;
     }

   if (!ewin->state.identified &&
       (ewin->state.maximized_horz || ewin->state.maximized_vert))
     {
	int                 hor, ver;

	/* New client requested maximisation */
	hor = ewin->state.maximized_horz;
	ver = ewin->state.maximized_vert;
	ewin->state.maximized_horz = ewin->state.maximized_vert = 0;
	MaxSizeHV(ewin, "absolute", hor, ver);
	/* Set old state to current maximized one */
	ewin->save_max.x = EoGetX(ewin);
	ewin->save_max.y = EoGetY(ewin);
	ewin->save_max.w = ewin->client.w;
	ewin->save_max.h = ewin->client.h;
	ewin->state.placed = 0;
     }
   else
     {
	EwinResize(ewin, ewin->client.w, ewin->client.h);
     }

   doslide = manplace = 0;
   if (Mode.place.enable_features > 0)
     {
	/* if set for borderless then dont slide it in */
	if (Conf.place.slidein && !Mode.place.doing_slide &&
	    !ewin->state.no_border && dsk == DesksGetCurrent())
	   doslide = 1;

	if (Conf.place.manual && !Mode.place.doing_manual &&
	    !ewin->state.placed && !ewin->icccm.transient)
	  {
	     if (GrabPointerSet(VROOT, ECSR_GRAB, 0) == GrabSuccess)
		manplace = 1;
	  }
     }

   /* if it hasn't been placed yet.... find a spot for it */
   if ((!ewin->state.placed) && (!manplace))
     {
	/* Place the window below the mouse pointer */
	if (Conf.place.manual_mouse_pointer)
	  {
	     int                 cx, cy;

	     /* if the loser has manual placement on and the app asks to be on */
	     /*  a desktop, then send E to that desktop so the user can place */
	     /* the window there */
	     DeskGoto(dsk);

	     EventsGetXY(&cx, &cy);

	     /* try to center the window on the mouse pointer */
	     cx -= EoGetW(ewin) / 2;
	     cy -= EoGetH(ewin) / 2;

	     /* keep it all on this screen if possible */
	     cx = MIN(cx, WinGetW(VROOT) - EoGetW(ewin));
	     cy = MIN(cy, WinGetH(VROOT) - EoGetH(ewin));
	     cx = MAX(cx, 0);
	     cy = MAX(cy, 0);

	     /* this works for me... */
	     x = cx;
	     y = cy;
	  }
	else if (ewin->ewmh.type.b.dialog)
	  {
	     /* Center unplaced dialogs on parent(if transient) or root */
	     Win                 parent;

	     ewin2 = NULL;
	     if (EwinGetTransientFor(ewin) != None)
		ewin2 = EwinFindByClient(EwinGetTransientFor(ewin));
	     parent = (ewin2) ? EoGetWin(ewin) : VROOT;
	     x = (WinGetW(parent) - EoGetW(ewin)) / 2;
	     y = (WinGetH(parent) - EoGetH(ewin)) / 2;
	  }
	else
	  {
	     ArrangeEwinXY(ewin, &x, &y);
	  }
	ewin->state.placed = 1;
     }

   /* if the window asked to be iconified at the start */
   if (ewin->icccm.start_iconified)
     {
	EwinMoveToDesktopAt(ewin, dsk, x, y);
	ewin->state.state = EWIN_STATE_MAPPED;
	EwinIconify(ewin);
	ewin->state.state = EWIN_STATE_ICONIC;
	goto done;
     }

   /* if we should slide it in and are not currently in the middle of a slide */
   if ((manplace) && (!ewin->state.placed))
     {
	int                 rx, ry;

	/* if the loser has manual placement on and the app asks to be on */
	/*  a desktop, then send E to that desktop so the user can place */
	/* the window there */
	DeskGoto(dsk);

	EQueryPointer(NULL, &rx, &ry, NULL, NULL);
	Mode.events.cx = rx;
	Mode.events.cy = ry;
	ewin->state.placed = 1;
	x = Mode.events.cx + 1;
	y = Mode.events.cy + 1;
	EwinMoveToDesktopAt(ewin, dsk, x, y);
	EwinMove(ewin, x, y);
	EwinShow(ewin);
	GrabPointerSet(VROOT, ECSR_GRAB, 0);
	Mode.place.doing_manual = 1;
	EoSetFloating(ewin, 1);	/* Causes reparenting to root */
	ActionMoveStart(ewin, 0, 0);
	goto done;
     }
   else if (doslide)
     {
	Timer              *slide_timer;

	k = rand() % 4;
	if (k == 0)
	  {
	     fx = (rand() % (WinGetW(VROOT))) - EoGetW(ewin);
	     fy = -EoGetH(ewin);
	  }
	else if (k == 1)
	  {
	     fx = (rand() % (WinGetW(VROOT)));
	     fy = WinGetH(VROOT);
	  }
	else if (k == 2)
	  {
	     fx = -EoGetW(ewin);
	     fy = (rand() % (WinGetH(VROOT)));
	  }
	else
	  {
	     fx = WinGetW(VROOT);
	     fy = (rand() % (WinGetH(VROOT))) - EoGetH(ewin);
	  }
	Mode.place.doing_slide = 1;
	ewin->state.animated = 1;
	FocusEnable(0);

	EwinMoveToDesktopAt(ewin, dsk, fx, fy);
	EwinShow(ewin);
	ewin->req_x = x;
	ewin->req_y = y;
	TIMER_ADD(slide_timer, 0.05, EwinSlideIn, ewin);
     }
   else
     {
	EwinMoveToDesktopAt(ewin, dsk, x, y);
	EwinShow(ewin);
     }

 done:
   EUngrabServer();
}

EWin               *
AddInternalToFamily(Win win, const char *bname, int type,
		    const EWinOps * ops, void *ptr)
{
   EWin               *ewin;

   EGrabServer();

   ewin = EwinCreate(type);
   if (!ewin)
      goto done;

   ewin->props.donthide = 1;
   EwinGetAttributes(ewin, win, None);
   WindowMatchEwinOps(ewin);	/* Window matches */
   EwinManage(ewin);

   ewin->data = ptr;
   ewin->ops = ops;
   if (ops && ops->Init)
      ops->Init(ewin);		/* Type specific initialisation */

   if (bname)
      ewin->border = BorderFind(bname);

   EwinConfigure(ewin);

#if 0
   Eprintf("Desk=%d, layer=%d, sticky=%d, floating=%d\n",
	   EoGetDesk(ewin), EoGetLayer(ewin), EoIsSticky(ewin),
	   EoIsFloating(ewin));
#endif

 done:
   EUngrabServer();

   return ewin;
}

static void
EwinUnmap1(EWin * ewin)
{
   /* The client may have been unmapped but the frame is not yet */

   if (GetZoomEWin() == ewin)
      Zoom(NULL);

   ActionsEnd(ewin);

   if (Mode.place.doing_slide)
     {
#if 0				/* No event processing during slides - No use doing this here */
	DrawEwinShape(ewin, Conf.slidemode, ewin->shape_x, ewin->shape_y,
		      ewin->client.w, ewin->client.h, 2);
#endif
#if 0				/* FIXME - Do this right */
	RemoveTimerEvent("Slide");
	Mode.place.doing_slide = 0;
	FocusEnable(1);
#endif
     }
}

static void
EwinUnmap2(EWin * ewin)
{
   /* The frame has been unmapped */

   FocusToEWin(ewin, FOCUS_EWIN_UNMAP);
   if (ewin == Mode.mouse_over_ewin)
      Mode.mouse_over_ewin = NULL;
   if (ewin == Mode.context_ewin)
      Mode.context_ewin = NULL;

   ModulesSignal(ESIGNAL_EWIN_UNMAP, ewin);
}

static void
EwinWithdraw(EWin * ewin, Win to)
{
   Window              win;
   int                 x, y;

   /* Only external clients should go here */

   if (EDebug(EDBUG_TYPE_EWINS))
      Eprintf("EwinWithdraw %#lx st=%d: %s\n", EwinGetClientXwin(ewin),
	      ewin->state.state, EwinGetTitle(ewin));

   EGrabServer();

   ESelectInput(EwinGetClientWin(ewin), NoEventMask);
   XShapeSelectInput(disp, EwinGetClientXwin(ewin), NoEventMask);

   if (EXWindowGetParent(EwinGetClientXwin(ewin)) == EwinGetContainerXwin(ewin))
     {
	/* Park the client window on the new root */
	x = ewin->client.x;
	y = ewin->client.y;
	ETranslateCoordinates(EwinGetClientWin(ewin), VROOT,
			      -ewin->border->border.left,
			      -ewin->border->border.top, &x, &y, &win);
	EReparentWindow(EwinGetClientWin(ewin), to, x, y);
	HintsDelWindowHints(ewin);
     }
   ICCCM_Withdraw(ewin);

   ESync(0);
   EUngrabServer();
}

static void
EwinEventMapRequest(EWin * ewin, Window win)
{
   if (ewin)
     {
	if (ewin->state.state == EWIN_STATE_ICONIC)
	   EwinDeIconify(ewin);
	else if (ewin->state.state == EWIN_STATE_WITHDRAWN)
	   AddToFamily(ewin, win);
	else
	  {
	     Eprintf("AddToFamily: Already managing %s %#lx\n", "A",
		     EwinGetClientXwin(ewin));
	     EReparentWindow(EwinGetClientWin(ewin), ewin->win_container, 0, 0);
	  }
     }
   else
     {
	/* Check if we are already managing it */
	ewin = EwinFindByClient(win);

	/* Some clients MapRequest more than once ?!? */
	if (ewin)
	  {
	     Eprintf("AddToFamily: Already managing %s %#lx\n", "B",
		     EwinGetClientXwin(ewin));
	     EReparentWindow(EwinGetClientWin(ewin), ewin->win_container, 0, 0);
	     EwinShow(ewin);
	  }
	else
	   AddToFamily(NULL, win);
     }
}

static void
EwinEventDestroy(EWin * ewin)
{
   if (EDebug(EDBUG_TYPE_EWINS))
      Eprintf("EwinEventDestroy %#lx st=%d: %s\n", EwinGetClientXwin(ewin),
	      ewin->state.state, EwinGetTitle(ewin));

   EwinDestroy(ewin);
}

static void
EwinEventReparent(EWin * ewin)
{
   Window              parent;

   EGrabServer();

   /* Refetch parent window. We cannot rely on the one in the event. */
   if (EoIsGone(ewin))
      parent = None;
   else
      parent = EXWindowGetParent(EwinGetClientXwin(ewin));
   if (EDebug(EDBUG_TYPE_EWINS))
      Eprintf("EwinEventReparent %#lx st=%d parent=%#lx: %s\n",
	      EwinGetClientXwin(ewin), ewin->state.state, parent,
	      EwinGetTitle(ewin));
   if (parent != EwinGetContainerXwin(ewin))
      EwinDestroy(ewin);

   EUngrabServer();
}

static void
EwinEventMap(EWin * ewin, XEvent * ev)
{
   int                 old_state;

   /* Catch clients setting OR without proper withdrawal (just unmap/map) */
   if (ev->xmap.override_redirect)
      return;

   old_state = ewin->state.state;
   ewin->state.state = EWIN_STATE_MAPPED;

   if (EDebug(EDBUG_TYPE_EWINS))
      Eprintf("EwinEventMap %#lx st=%d: %s\n", EwinGetClientXwin(ewin),
	      ewin->state.state, EwinGetTitle(ewin));

   /* If first time we may want to focus it (unless during startup) */
   if (old_state == EWIN_STATE_NEW)
      FocusToEWin(ewin, FOCUS_EWIN_NEW);
   else
      FocusToEWin(ewin, FOCUS_SET);

   ModulesSignal(ESIGNAL_EWIN_CHANGE, ewin);
}

static void
EwinEventUnmap(EWin * ewin, XEvent * ev)
{
   if (EDebug(EDBUG_TYPE_EWINS))
      Eprintf("EwinEventUnmap %#lx st=%d: %s\n", EwinGetClientXwin(ewin),
	      ewin->state.state, EwinGetTitle(ewin));

   if (ewin->state.state == EWIN_STATE_STARTUP ||
       ewin->state.state == EWIN_STATE_NEW)
     {
#if 0
	/* We get here after reparenting to container and occasionally in
	 * other(?) situations */
	Eprintf("EwinEventUnmap %#lx: Ignoring bogus Unmap event\n",
		EwinGetClientXwin(ewin));
#endif
	return;
     }

   /* Ignore synthetic events */
   if (ev->xany.send_event)
      return;

   if (ewin->state.state == EWIN_STATE_WITHDRAWN)
      return;

   if (ewin->state.iconified)
      ewin->state.state = EWIN_STATE_ICONIC;
   else
      ewin->state.state = EWIN_STATE_WITHDRAWN;

   EwinUnmap1(ewin);
   EWindowSetMapped(EwinGetClientWin(ewin), 0);
   EoUnmap(ewin);
   EwinUnmap2(ewin);

   if (ewin->state.state == EWIN_STATE_ICONIC)
      return;

   if (EwinIsInternal(ewin))
     {
#if 1				/* FIXME - Remove? */
	/* We should never get here */
	Eprintf("FIXME: This cannot happen (%s)\n", EoGetName(ewin));
#endif
	return;
     }

   if (EoIsGone(ewin))
      return;

   EwinWithdraw(ewin, VROOT);
}

static void
EwinEventConfigureRequest(EWin * ewin, XEvent * ev)
{
   Window              winrel;
   EWin               *ewin2;
   int                 x = 0, y = 0, w = 0, h = 0;
   XWindowChanges      xwc;

   if (ewin)
     {
	x = EoGetX(ewin);
	y = EoGetY(ewin);
	w = ewin->client.w;
	h = ewin->client.h;
	winrel = 0;
	/* This is shady - some clients send root coords, some use the
	 * ICCCM ones sent by us */
	if (!EwinInhGetApp(ewin, move))
	  {
#if 1				/* FIXME - ??? */
	     if (ev->xconfigurerequest.value_mask & CWX)
		x = ev->xconfigurerequest.x;
	     if (ev->xconfigurerequest.value_mask & CWY)
		y = ev->xconfigurerequest.y;
#else
	     if (ev->xconfigurerequest.value_mask & CWX)
		x = ev->xconfigurerequest.x - EoGetX(EoGetDesk(ewin));
	     if (ev->xconfigurerequest.value_mask & CWY)
		y = ev->xconfigurerequest.y - EoGetY(EoGetDesk(ewin));
#endif
	  }
	if (!EwinInhGetApp(ewin, size))
	  {
	     if (ev->xconfigurerequest.value_mask & CWWidth)
		w = ev->xconfigurerequest.width;
	     if (ev->xconfigurerequest.value_mask & CWHeight)
		h = ev->xconfigurerequest.height;
	  }
	if (ev->xconfigurerequest.value_mask & CWSibling)
	   winrel = ev->xconfigurerequest.above;
	if (ev->xconfigurerequest.value_mask & CWStackMode)
	  {
	     ewin2 = EwinFindByClient(winrel);
	     if (ewin2)
		winrel = EoGetXwin(ewin2);
	     xwc.sibling = winrel;
	     xwc.stack_mode = ev->xconfigurerequest.detail;
	     if (Mode.mode == MODE_NONE)
	       {
		  if (xwc.stack_mode == Above)
		     EwinRaise(ewin);
		  else if (xwc.stack_mode == Below)
		     EwinLower(ewin);
	       }
	  }

	if (ev->xconfigurerequest.value_mask & (CWX | CWY))
	  {
	     /* Correct position taking gravity into account */
	     EwinGetPosition(ewin, x, y, 0, &x, &y);
	  }

	Mode.move.check = 0;	/* Don't restrict client requests */
	EwinMoveResize(ewin, x, y, w, h);
	Mode.move.check = 1;
	ReZoom(ewin);
     }
   else
     {
	xwc.x = ev->xconfigurerequest.x;
	xwc.y = ev->xconfigurerequest.y;
	xwc.width = ev->xconfigurerequest.width;
	xwc.height = ev->xconfigurerequest.height;
	xwc.border_width = ev->xconfigurerequest.border_width;
	xwc.sibling = ev->xconfigurerequest.above;
	xwc.stack_mode = ev->xconfigurerequest.detail;
	XConfigureWindow(disp, ev->xconfigurerequest.window,
			 ev->xconfigurerequest.value_mask, &xwc);
     }
}

static void
EwinEventResizeRequest(EWin * ewin, XEvent * ev)
{
   if (ewin)
     {
	EwinResize(ewin, ev->xresizerequest.width, ev->xresizerequest.height);
	ReZoom(ewin);
     }
   else
     {
	XResizeWindow(disp, ev->xresizerequest.window,
		      ev->xresizerequest.width, ev->xresizerequest.height);
     }
}

static void
EwinEventCirculateRequest(EWin * ewin, XEvent * ev)
{
   if (ewin)
     {
	if (ev->xcirculaterequest.place == PlaceOnTop)
	   EwinRaise(ewin);
	else
	   EwinLower(ewin);
     }
   else
     {
	if (ev->xcirculaterequest.place == PlaceOnTop)
	   XRaiseWindow(disp, ev->xcirculaterequest.window);
	else
	   XLowerWindow(disp, ev->xcirculaterequest.window);
     }
}

static void
EwinEventPropertyNotify(EWin * ewin, XEvent * ev)
{
   if (EwinIsInternal(ewin))
      return;

   EGrabServer();
   EwinChangesStart(ewin);

   HintsProcessPropertyChange(ewin, ev);
   EwinStateUpdate(ewin);

   EwinChangesProcess(ewin);
   EUngrabServer();
}

static void
EwinEventShapeChange(EWin * ewin, XEvent * ev)
{
#define se ((XShapeEvent *)ev)
   if (EDebug(EX_EVENT_SHAPE_NOTIFY))
      Eprintf("EwinEventShapeChange %#lx %s: state.shaped=%d ev->shaped=%d\n",
	      EwinGetClientXwin(ewin), EoGetName(ewin), ewin->state.shaped,
	      se->shaped);
   if (!se->shaped && !ewin->state.shaped)
      return;
   EwinUpdateShapeInfo(ewin);
   ewin->update.shape = 1;
   EwinPropagateShapes(ewin);
#undef se
}

static void
EwinEventVisibility(EWin * ewin, int state)
{
   ewin->state.visibility = state;
}

void
EwinReparent(EWin * ewin, Win parent)
{
   EwinWithdraw(ewin, parent);
}

void
EwinRaise(EWin * ewin)
{
   static int          call_depth = 0;
   EWin              **lst;
   int                 i, num;

   if (call_depth > 256)
      return;
   call_depth++;

   num = EoRaise(ewin);

   if (EDebug(EDBUG_TYPE_RAISELOWER))
      Eprintf("EwinRaise(%d) %#lx %s n=%d\n", call_depth,
	      EwinGetClientXwin(ewin), EwinGetTitle(ewin), num);

   if (num == 0)		/* Quit if stacking is unchanged */
      goto done;

   lst = EwinListTransients(ewin, &num, 1);
   for (i = 0; i < num; i++)
      EwinRaise(lst[i]);
   Efree(lst);

   if (call_depth == 1)
     {
	ModulesSignal(ESIGNAL_EWIN_CHANGE, ewin);
	ClickGrabsUpdate();
     }

 done:
   call_depth--;
}

void
EwinLower(EWin * ewin)
{
   static int          call_depth = 0;
   EWin              **lst;
   int                 i, num;

   if (call_depth > 256)
      return;
   call_depth++;

   num = EoLower(ewin);

   if (EDebug(EDBUG_TYPE_RAISELOWER))
      Eprintf("EwinLower(%d) %#lx %s n=%d\n", call_depth,
	      EwinGetClientXwin(ewin), EwinGetTitle(ewin), num);

   if (num == 0)		/* Quit if stacking is unchanged */
      goto done;

   lst = EwinListTransientFor(ewin, &num);
   for (i = 0; i < num; i++)
      EwinLower(lst[i]);
   Efree(lst);

   if (call_depth == 1)
     {
	ModulesSignal(ESIGNAL_EWIN_CHANGE, ewin);
	ClickGrabsUpdate();
     }

 done:
   call_depth--;
}

void
EwinShow(EWin * ewin)
{
   if (EoIsShown(ewin))
      return;

   if (EwinGetClientWin(ewin))
     {
#if 0				/* FIXME - Why? */
	if (ewin->state.shaded)
	   EMoveResizeWindow(ewin->win_container, -30, -30, 1, 1);
#endif
	EMapWindow(EwinGetClientWin(ewin));
     }

   if (ewin->update.shape)
     {
	ewin->o.shown = 1;
	EwinPropagateShapes(ewin);
	ewin->o.shown = 0;
     }

   EoMap(ewin, 0);

   EwinStateUpdate(ewin);

   if (ewin->place.gravity < 0)
      EwinSetPlacementGravity(ewin, EoGetX(ewin), EoGetY(ewin));
}

void
EwinHide(EWin * ewin)
{
   if (!EwinIsInternal(ewin) && (!EoIsShown(ewin) || !EwinIsMapped(ewin)))
      return;

   EwinUnmap1(ewin);

   EUnmapWindow(EwinGetClientWin(ewin));
   EoUnmap(ewin);

   EwinUnmap2(ewin);

   EwinStateUpdate(ewin);

   if (!EwinIsInternal(ewin) || ewin->state.iconified)
      return;

   if (EwinGetClientWin(ewin))
     {
	ESelectInput(EwinGetClientWin(ewin), NoEventMask);
	XShapeSelectInput(disp, EwinGetClientXwin(ewin), NoEventMask);
     }

   if (ewin->ops && ewin->ops->Close)
      ewin->ops->Close(ewin);

   EwinDestroy(ewin);
}

void
EwinKill(EWin * ewin)
{
   if (EwinIsInternal(ewin))
      return;

   XKillClient(disp, EwinGetClientXwin(ewin));

#if 0				/* Wait for unmap/destroy for now */
   EwinUnmap1(ewin);
   EoUnmap(ewin);
   EwinUnmap2(ewin);

   EwinDestroy(ewin);
#endif
}

void
EwinSetTitle(EWin * ewin, const char *title)
{
   HintsSetWindowName(EwinGetClientWin(ewin), title);

   _EFDUP(ewin->o.icccm.wm_name, title);
   _EFDUP(ewin->ewmh.wm_name, title);
}

void
EwinSetClass(EWin * ewin, const char *name, const char *clss)
{
   HintsSetWindowClass(EwinGetClientWin(ewin), name, clss);

   _EFDUP(ewin->o.icccm.wm_res_name, name);
   _EFDUP(ewin->o.icccm.wm_res_class, clss);
}

const char         *
EwinGetTitle(const EWin * ewin)
{
   const char         *name;

   if (!ewin)
      return NULL;
   name = ewin->ewmh.wm_name;
   if (name)
      goto done;
   name = EwinGetIcccmName(ewin);
   if (name)
      goto done;

 done:
   return (name && name[0]) ? name : NULL;
}

const char         *
EwinGetIconName(const EWin * ewin)
{
   const char         *name;

   name = ewin->ewmh.wm_icon_name;
   if (name)
      goto done;
   name = ewin->icccm.wm_icon_name;
   if (name)
      goto done;

   return EwinGetTitle(ewin);

 done:
   return (name && strlen(name)) ? name : NULL;
}

const char         *
EwinBorderGetName(const EWin * ewin)
{
   return (ewin->border) ? BorderGetName(ewin->border) : "?";
}

void
EwinBorderGetSize(const EWin * ewin, int *bl, int *br, int *bt, int *bb)
{
   const Border       *b = ewin->border;

   if (!b)
     {
	*bl = *br = *bt = *bb = 0;
	return;
     }

   *bl = b->border.left;
   *br = b->border.right;
   *bt = b->border.top;
   *bb = b->border.bottom;
}

void
EwinBorderUpdateState(EWin * ewin)
{
   EwinBorderDraw(ewin, 0, 0);
}

int
EwinIsOnScreen(const EWin * ewin)
{
   int                 x, y, w, h;

   if (EoIsSticky(ewin))
      return 1;
   if (EoGetDesk(ewin) != DesksGetCurrent())
      return 0;

   x = EoGetX(ewin);
   y = EoGetY(ewin);
   w = EoGetW(ewin);
   h = EoGetH(ewin);

   if (x + w <= 0 || x >= WinGetW(VROOT) || y + h <= 0 || y >= WinGetH(VROOT))
      return 0;

   return 1;
}

/*
 * Save current position in absolute viewport coordinates
 */
void
EwinRememberPositionSet(EWin * ewin)
{
   int                 ax, ay;

   ewin->req_x = EoGetX(ewin);
   ewin->req_y = EoGetY(ewin);
   if (!EoIsSticky(ewin))
     {
	DeskGetArea(EoGetDesk(ewin), &ax, &ay);
	ewin->req_x += ax * WinGetW(VROOT);
	ewin->req_y += ay * WinGetH(VROOT);
     }
}

/*
 * Get saved position in relative viewport coordinates
 */
void
EwinRememberPositionGet(EWin * ewin, Desk * dsk, int *px, int *py)
{
   int                 x, y, ax, ay;

   x = ewin->req_x;
   y = ewin->req_y;
   if (!EoIsSticky(ewin))
     {
	DeskGetArea(dsk, &ax, &ay);
	x -= ax * WinGetW(VROOT);
	y -= ay * WinGetH(VROOT);
     }

   *px = x;
   *py = y;
}

/*
 * Set placement gravity
 */
void
EwinSetPlacementGravity(EWin * ewin, int x, int y)
{
   int                 w, h, ax, ay, wd, hd;
   Desk               *dsk;

   dsk = EoGetDesk(ewin);
   wd = EoGetW(dsk);
   hd = EoGetH(dsk);
   DeskGetArea(dsk, &ax, &ay);

   w = EoGetW(ewin);
   h = EoGetH(ewin);

   /* Get relative area */
   ewin->place.ax = ewin->area_x;
   ewin->place.ay = ewin->area_y;
   ax = ewin->place.ax - ax;
   ay = ewin->place.ay - ay;

   x -= ax * wd;
   y -= ay * hd;

   if (x <= (wd - w) / 2)
     {
	if (y <= (hd - h) / 2)
	  {
	     ewin->place.gravity = EWIN_GRAVITY_NW;
	     ewin->place.gx = x;
	     ewin->place.gy = y;
	  }
	else
	  {
	     ewin->place.gravity = EWIN_GRAVITY_SW;
	     ewin->place.gx = x;
	     ewin->place.gy = hd - (y + h);
	  }
     }
   else
     {
	if (y <= (hd - h) / 2)
	  {
	     ewin->place.gravity = EWIN_GRAVITY_NE;
	     ewin->place.gx = wd - (x + w);
	     ewin->place.gy = y;
	  }
	else
	  {
	     ewin->place.gravity = EWIN_GRAVITY_SE;
	     ewin->place.gx = wd - (x + w);
	     ewin->place.gy = hd - (y + h);
	  }
     }

#if 0				/* Debug */
   Eprintf("Set gravity %d,%d %d,%d %d %d,%d %d,%d: %s\n", ax, ay, x, y,
	   ewin->place.gravity, ewin->place.ax, ewin->place.ay,
	   ewin->place.gx, ewin->place.gy, EwinGetTitle(ewin));
#endif
}

void
EwinReposition(EWin * ewin)
{
   int                 wdo, hdo, wdn, hdn;
   int                 x, y, w, h, ax, ay, xn, yn;

   wdo = Mode.screen.w_old;
   hdo = Mode.screen.h_old;
   wdn = WinGetW(VROOT);
   hdn = WinGetH(VROOT);

   x = EoGetX(ewin);
   y = EoGetY(ewin);
   w = EoGetW(ewin);
   h = EoGetH(ewin);

   /* Get relative area */
   if (EoIsSticky(ewin))
     {
	ax = ay = 0;
     }
   else
     {
	DeskGetArea(EoGetDesk(ewin), &ax, &ay);
	ax = ewin->place.ax - ax;
	ay = ewin->place.ay - ay;
     }

   x -= ax * wdo;
   y -= ay * hdo;

   /* Reposition to same distance from screen edges determined by
    * placement gravity.
    * Fall back to left/top if this causes left/top to go offscreen */
   switch (ewin->place.gravity)
     {
     default:
     case EWIN_GRAVITY_NW:
     case EWIN_GRAVITY_SW:
	xn = ewin->place.gx;
	break;
     case EWIN_GRAVITY_NE:
     case EWIN_GRAVITY_SE:
	xn = wdn - w - ewin->place.gx;
	break;
     }
   if (x > 0 && xn < 0)
      xn = x;

   switch (ewin->place.gravity)
     {
     default:
     case EWIN_GRAVITY_NW:
     case EWIN_GRAVITY_NE:
	yn = ewin->place.gy;
	break;
     case EWIN_GRAVITY_SW:
     case EWIN_GRAVITY_SE:
	yn = hdn - h - ewin->place.gy;
	break;
     }
   if (y > 0 && yn < 0)
      yn = y;

#if 0				/* Debug */
   Eprintf("Reposition %d,%d -> %d,%d: %s\n", x, y, xn, yn, EwinGetTitle(ewin));
#endif

   xn += ax * wdn;
   yn += ay * hdn;

   EwinMove(ewin, xn, yn);
}

void
EwinWarpTo(EWin * ewin)
{
   if (ewin == Mode.mouse_over_ewin)
      return;

   if (ewin->state.iconified)
      return;

   EXWarpPointer(EoGetXwin(ewin), EoGetW(ewin) / 2, EoGetH(ewin) / 2);
   Mode.mouse_over_ewin = ewin;
}

typedef union {
   unsigned int        all;
   struct {
      unsigned char       rsvd;
      unsigned char       inh_app;
      unsigned char       inh_user;
      unsigned char       inh_wm;
   } f;
} EWinMiscFlags;

typedef union {
   unsigned int        all;
   struct {
      unsigned            nua:1;
      unsigned            ctf:1;
      unsigned            nbg:1;
      unsigned:           5;
      unsigned:           16;
      unsigned            no_fade:1;
      unsigned            no_shadow:1;
      unsigned:           6;
   } f;
} EWinMiscFlags2;

void
EwinFlagsEncode(const EWin * ewin, unsigned int *flags)
{
   EWinMiscFlags       fm;
   EWinMiscFlags2      fm2;

   fm.all = 0;
   fm.f.inh_app = ewin->inh_app.all;
   fm.f.inh_user = ewin->inh_user.all;
   fm.f.inh_wm = ewin->inh_wm.all;

   fm2.all = 0;
   fm2.f.nua = ewin->props.never_use_area;
   fm2.f.ctf = ewin->props.focusclick;
   fm2.f.nbg = ewin->props.no_button_grabs;
#if USE_COMPOSITE
   fm2.f.no_fade = !EoGetFade(ewin);
   fm2.f.no_shadow = !EoGetShadow(ewin);
#endif

   flags[0] = fm.all;
   flags[1] = fm2.all;
}

void
EwinFlagsDecode(EWin * ewin, const unsigned int *flags)
{
   EWinMiscFlags       fm;
   EWinMiscFlags2      fm2;

   fm.all = flags[0];
   ewin->inh_app.all = fm.f.inh_app;
   ewin->inh_user.all = fm.f.inh_user;
   ewin->inh_wm.all = fm.f.inh_wm;

   fm2.all = flags[1];
   ewin->props.never_use_area = fm2.f.nua;
   ewin->props.focusclick = fm2.f.ctf;
   ewin->props.no_button_grabs = fm2.f.nbg;
#if USE_COMPOSITE
   EoSetFade(ewin, !fm2.f.no_fade);
   EoSetShadow(ewin, !fm2.f.no_shadow);
#endif
}

void
EwinUpdateOpacity(EWin * ewin)
{
   unsigned int        opacity;

   opacity = 0;
   if (ewin->state.moving || ewin->state.resizing)
      opacity = OpacityFromPercent(Conf.opacity.movres);
   else if (ewin->state.active)
      opacity = ewin->props.focused_opacity;
   if (opacity == 0)
      opacity = ewin->ewmh.opacity;
   if (opacity == 0)
      opacity = ewin->state.active ?
	 OpacityFromPercent(Conf.opacity.focused) :
	 OpacityFromPercent(Conf.opacity.unfocused);
   if (opacity == 0)
      opacity = 0xffffffff;	/* Fallback */

   EoChangeOpacity(ewin, opacity);
}

/*
 * Slidein
 */
static int
EwinSlideIn(void *data)
{
   EWin               *ewin = (EWin *) data;

   /* May be gone */
   if (!EwinFindByPtr(ewin))
      goto done;

   SlideEwinTo(ewin, EoGetX(ewin), EoGetY(ewin), ewin->req_x, ewin->req_y,
	       Conf.place.slidespeedmap, Conf.place.slidemode);

 done:
   Mode.place.doing_slide = 0;
   FocusEnable(1);

   return 0;
}

/*
 * Change requests
 */
static struct {
   unsigned int        flags;
   EWin                ewin_old;
} EWinChanges;

void
EwinChange(EWin * ewin __UNUSED__, unsigned int flag)
{
   EWinChanges.flags |= flag;
}

void
EwinChangesStart(EWin * ewin)
{
   EWinChanges.flags = 0;
   /* Brute force :) */
   EWinChanges.ewin_old = *ewin;
}

void
EwinChangesProcess(EWin * ewin)
{
   if (!EWinChanges.flags)
      return;

   if (EWinChanges.flags & EWIN_CHANGE_NAME)
     {
	EwinBorderUpdateInfo(ewin);
	EwinBorderCalcSizes(ewin, 1);
     }

   if (EWinChanges.flags & EWIN_CHANGE_DESKTOP)
     {
	Desk               *desk, *pdesk;

	desk = EoGetDesk(ewin);
	pdesk = EoGetDesk(&EWinChanges.ewin_old);
	if (desk != pdesk && !EoIsSticky(ewin))
	  {
	     EoSetDesk(ewin, pdesk);
	     EwinMoveToDesktop(ewin, desk);
	  }
     }

   if (EWinChanges.flags & EWIN_CHANGE_ICON_PMAP)
     {
	ModulesSignal(ESIGNAL_EWIN_CHANGE_ICON, ewin);
     }

   if (EWinChanges.flags & EWIN_CHANGE_OPACITY)
     {
	EoChangeOpacity(ewin, ewin->ewmh.opacity);
	SnapshotEwinUpdate(ewin, SNAP_USE_OPACITY);
     }

   if (EWinChanges.flags & EWIN_CHANGE_ATTENTION)
     {
	HintsSetWindowState(ewin);
     }

   EWinChanges.flags = 0;
}

EWin              **
EwinListTransients(const EWin * ewin, int *num, int group)
{
   EWin               *const *ewins, **lst, *ew;
   int                 i, j, n;

   j = 0;
   lst = NULL;

   if (EwinGetTransientCount(ewin) <= 0)
      goto done;

   ewins = EwinListGetAll(&n);

   /* Find regular transients */
   for (i = 0; i < n; i++)
     {
	ew = ewins[i];

	/* Skip self-reference */
	if (ew == ewin)
	   continue;

	if (EwinGetTransientFor(ew) == EwinGetClientXwin(ewin))
	  {
	     lst = EREALLOC(EWin *, lst, j + 1);
	     lst[j++] = ew;
	  }
     }

   if (!group)
      goto done;

   /* Group transients (if ewin is not a transient) */
   if (EwinIsTransient(ewin))
      goto done;

   for (i = 0; i < n; i++)
     {
	ew = ewins[i];

	/* Skip self-reference */
	if (ew == ewin)
	   continue;

	if (EwinGetTransientFor(ew) == WinGetXwin(VROOT) &&
	    EwinGetWindowGroup(ew) == EwinGetWindowGroup(ewin))
	  {
	     lst = EREALLOC(EWin *, lst, j + 1);
	     lst[j++] = ew;
	  }
     }

 done:
   *num = j;
   return lst;
}

EWin              **
EwinListTransientFor(const EWin * ewin, int *num)
{
   EWin               *const *ewins, **lst, *ew;
   int                 i, j, n;

   j = 0;
   lst = NULL;

   if (!EwinIsTransient(ewin))
      goto done;

   ewins = EwinListGetAll(&n);
   for (i = 0; i < n; i++)
     {
	ew = ewins[i];

	/* Skip self-reference */
	if (ew == ewin)
	   continue;

	/* Regular parent or if root trans, top level group members */
	if ((EwinGetTransientFor(ewin) == EwinGetClientXwin(ew)) ||
	    (!EwinIsTransient(ew) &&
	     EwinGetTransientFor(ewin) == WinGetXwin(VROOT) &&
	     EwinGetWindowGroup(ew) == EwinGetWindowGroup(ewin)))
	  {
	     lst = EREALLOC(EWin *, lst, j + 1);
	     lst[j++] = ew;
	  }
     }

 done:
   *num = j;
   return lst;
}

static void
EwinsTouch(Desk * dsk)
{
   int                 i, num;
   EWin               *const *lst, *ewin;

   if (!dsk)
      lst = EwinListGetAll(&num);
   else
      lst = EwinListGetForDesk(&num, dsk);

   for (i = num - 1; i >= 0; i--)
     {
	ewin = lst[i];
	if (EwinIsMapped(ewin) && EwinIsOnScreen(ewin))
#if 1				/* FIXME - Which one? */
	   EwinMove(ewin, EoGetX(ewin), EoGetY(ewin));
#else
	   EwinResize(ewin, ewin->client.w, ewin->client.h);
#endif
     }
}

static void
EwinsReposition(void)
{
   int                 i, num;
   EWin               *const *lst;

   lst = EwinListGetAll(&num);
   for (i = num - 1; i >= 0; i--)
      EwinReposition(lst[i]);
}

void
EwinsMoveStickyToDesk(Desk * dsk)
{
   EWin               *const *lst, *ewin;
   int                 i, num;

   lst = EwinListStackGet(&num);
   for (i = 0; i < num; i++)
     {
	ewin = lst[num - 1 - i];
	if (!EoIsSticky(ewin) && !EoIsFloating(ewin))
	   continue;
	if (EwinIsTransientChild(ewin))
	   continue;

	EwinMoveToDesktop(ewin, dsk);
     }
}

void
EwinsManage(void)
{
   Window             *xwins, xwin, par, rt;
   XWindowAttributes   attr;
   unsigned int        i, num;

#ifdef USE_EXT_INIT_WIN
   Window              init_win = ExtInitWinGet();
#endif

   xwins = NULL;
   num = 0;
   XQueryTree(disp, WinGetXwin(VROOT), &rt, &par, &xwins, &num);
   if (!xwins)
      return;

   for (i = 0; i < num; i++)
     {
	xwin = xwins[i];

	/* Skip if already "known" */
	if (EobjListStackFind(xwin))
	   continue;

	if (!XGetWindowAttributes(disp, xwin, &attr))
	   continue;

	if (attr.map_state == IsUnmapped)
	   continue;

	if (attr.override_redirect)
	  {
	     XUnmapWindow(disp, xwin);	/* Makes the CM catch it on map */
	     XMapRaised(disp, xwin);
#ifdef USE_EXT_INIT_WIN
	     if (init_win)
		XRaiseWindow(disp, init_win);
#endif
	  }
	else
	  {
	     AddToFamily(NULL, xwin);
	  }
     }
   XFree(xwins);
}

void
EwinsSetFree(void)
{
   int                 i, num;
   EWin               *const *lst, *ewin;

   if (EDebug(EDBUG_TYPE_SESSION))
      Eprintf("EwinsSetFree\n");

   EHintsSetInfoOnAll();

   lst = EwinListStackGet(&num);
   for (i = num - 1; i >= 0; i--)
     {
	ewin = lst[i];
	if (EwinIsInternal(ewin))
	   continue;

	if (ewin->state.iconified)
	   ICCCM_DeIconify(ewin);

	/* This makes E determine the client window stacking at exit */
	EwinInstantUnShade(ewin);
	EReparentWindow(EwinGetClientWin(ewin), RROOT,
			ewin->client.x, ewin->client.y);
     }
}

/*
 * Event handlers
 */

static int
ActionsCheck(const char *which, EWin * ewin, XEvent * ev)
{
   ActionClass        *ac;

   if (Mode.action_inhibit)	/* Probably not here */
      return 0;

   ac = ActionclassFind(which);
   if (!ac)
      return 0;

   if (ev->type == ButtonPress)
     {
	GrabPointerSet(EoGetWin(ewin), ECSR_GRAB, 0);
	FocusToEWin(ewin, FOCUS_CLICK);
     }
   else if (ev->type == ButtonRelease)
     {
	GrabPointerRelease();
     }

   return ActionclassEvent(ac, ev, ewin);
}

#define DEBUG_EWIN_EVENTS 0
static void
EwinHandleEventsToplevel(Win win __UNUSED__, XEvent * ev, void *prm)
{
   EWin               *ewin = (EWin *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	ActionsCheck("BUTTONBINDINGS", ewin, ev);
	break;
     case ButtonRelease:
	ActionsEnd(NULL);
	ActionsCheck("BUTTONBINDINGS", ewin, ev);
	BorderCheckState(ewin, ev);
	break;
     case EnterNotify:
	FocusHandleEnter(ewin, ev);
	break;
     case LeaveNotify:
	FocusHandleLeave(ewin, ev);
	break;
     case MotionNotify:
	ActionsHandleMotion();
	break;
     default:
#if DEBUG_EWIN_EVENTS
	Eprintf("EwinHandleEventsToplevel: type=%2d win=%#lx: %s\n",
		ev->type, EwinGetClientXwin(ewin), EwinGetTitle(ewin));
#endif
	break;
     }
}

static void
EwinHandleEventsContainer(Win win __UNUSED__, XEvent * ev, void *prm)
{
   EWin               *ewin = (EWin *) prm;

#if 0
   Eprintf("EwinHandleEventsContainer: type=%2d win=%#lx: %s\n",
	   ev->type, EwinGetClientXwin(ewin), EwinGetTitle(ewin));
#endif
   switch (ev->type)
     {
     case ButtonPress:
	FocusHandleClick(ewin, EwinGetContainerWin(ewin));
	break;

     case MapRequest:
	if (ev->xmaprequest.window != EwinGetClientXwin(ewin))
	   break;
	EwinEventMapRequest(ewin, ev->xmaprequest.window);
	break;
     case ConfigureRequest:
	if (ev->xconfigurerequest.window != EwinGetClientXwin(ewin))
	   break;
	EwinEventConfigureRequest(ewin, ev);
	break;
     case ResizeRequest:
	if (ev->xresizerequest.window != EwinGetClientXwin(ewin))
	   break;
	EwinEventResizeRequest(ewin, ev);
	break;
     case CirculateRequest:
	if (ev->xcirculaterequest.window != EwinGetClientXwin(ewin))
	   break;
	EwinEventCirculateRequest(ewin, ev);
	break;

     case DestroyNotify:
	if (ev->xdestroywindow.window != EwinGetClientXwin(ewin))
	   break;
	EwinEventDestroy(ewin);
	break;

     case EX_EVENT_UNMAP_GONE:
	if (ev->xunmap.window != EwinGetClientXwin(ewin))
	   break;
	EoSetGone(ewin);
	goto do_unmap;
     case UnmapNotify:
	if (ev->xunmap.window != EwinGetClientXwin(ewin))
	   break;
      do_unmap:
	EwinEventUnmap(ewin, ev);
	break;

     case MapNotify:
	if (ev->xmap.window != EwinGetClientXwin(ewin))
	   break;
	EwinEventMap(ewin, ev);
	break;

     case EX_EVENT_REPARENT_GONE:
	if (ev->xreparent.window != EwinGetClientXwin(ewin))
	   break;
	EoSetGone(ewin);
	goto do_reparent;
     case ReparentNotify:
	if (ev->xreparent.window != EwinGetClientXwin(ewin))
	   break;
      do_reparent:
	EwinEventReparent(ewin);
	break;

     case EX_EVENT_MAP_GONE:
     case GravityNotify:
     case ConfigureNotify:
	break;

     default:
	Eprintf("EwinHandleEventsContainer: type=%2d win=%#lx: %s\n",
		ev->type, EwinGetClientXwin(ewin), EwinGetTitle(ewin));
	break;
     }
}

static void
EwinHandleEventsClient(Win win __UNUSED__, XEvent * ev, void *prm)
{
   EWin               *ewin = (EWin *) prm;

   switch (ev->type)
     {
     case FocusIn:
     case FocusOut:
	if (ev->xfocus.detail == NotifyInferior)
	   break;
	if (ewin->border->aclass)
	   ActionclassEvent(ewin->border->aclass, ev, ewin);
	FocusHandleChange(ewin, ev);
	break;
     case ConfigureNotify:
     case GravityNotify:
	break;
     case VisibilityNotify:
	EwinEventVisibility(ewin, ev->xvisibility.state);
	break;

     case PropertyNotify:
	EwinEventPropertyNotify(ewin, ev);
	break;

     case ClientMessage:
	HintsProcessClientClientMessage(ewin, &(ev->xclient));
	break;

     case EX_EVENT_SHAPE_NOTIFY:
	EwinEventShapeChange(ewin, ev);
	break;

     default:
#if DEBUG_EWIN_EVENTS
	Eprintf("EwinHandleEventsClient: type=%2d win=%#lx: %s\n",
		ev->type, EwinGetClientXwin(ewin), EwinGetTitle(ewin));
#endif
	break;
     }
}

static void
EwinHandleEventsRoot(Win win __UNUSED__, XEvent * ev, void *prm __UNUSED__)
{
   EWin               *ewin;

   switch (ev->type)
     {
     case MapRequest:
	EwinEventMapRequest(NULL, ev->xmaprequest.window);
	break;
     case ConfigureRequest:
#if 0
	Eprintf("EwinHandleEventsRoot ConfigureRequest %#lx\n",
		ev->xconfigurerequest.window);
#endif
	ewin = EwinFindByClient(ev->xconfigurerequest.window);
	EwinEventConfigureRequest(ewin, ev);
	break;
     case ResizeRequest:
#if 0
	Eprintf("EwinHandleEventsRoot ResizeRequest %#lx\n",
		ev->xresizerequest.window);
#endif
	ewin = EwinFindByClient(ev->xresizerequest.window);
	EwinEventResizeRequest(ewin, ev);
	break;
     case CirculateRequest:
#if 0
	Eprintf("EwinHandleEventsRoot CirculateRequest %#lx\n",
		ev->xcirculaterequest.window);
#endif
	EwinEventCirculateRequest(NULL, ev);
	break;

     case UnmapNotify:
     case EX_EVENT_UNMAP_GONE:
	/* Catch clients unmapped after MapRequest but before being reparented */
	ewin = EwinFindByClient(ev->xunmap.window);
	if (!ewin)
	   break;
	if (ev->type == EX_EVENT_UNMAP_GONE)
	   EoSetGone(ewin);
	EwinEventUnmap(ewin, ev);
	break;

     case DestroyNotify:
	/* Catch clients destroyed after MapRequest but before being reparented */
	ewin = EwinFindByClient(ev->xdestroywindow.window);
	if (!ewin)
	   break;
	EwinEventDestroy(ewin);
	break;

     case ReparentNotify:
     case EX_EVENT_REPARENT_GONE:
	ewin = EwinFindByClient(ev->xreparent.window);
	if (!ewin)
	   break;
	if (ev->type == EX_EVENT_REPARENT_GONE)
	   EoSetGone(ewin);
	EwinEventReparent(ewin);
	break;

     case ClientMessage:
	HintsProcessRootClientMessage(&(ev->xclient));
	break;

     default:
#if 0
	Eprintf("EwinHandleEventsRoot: type=%2d win=%#lx\n",
		ev->type, ev->xany.window);
#endif
	break;
     }
}

static void
EwinsInit(void)
{
   EventCallbackRegister(VROOT, 0, EwinHandleEventsRoot, NULL);
}

/*
 * Ewins module
 * This is the WM.
 */

static void
EwinsSighan(int sig, void *prm)
{
   switch (sig)
     {
     case ESIGNAL_INIT:
	EwinsInit();
	break;
#if 0
     case ESIGNAL_START:
	EwinsManage();
	break;
#endif
     case ESIGNAL_DESK_RESIZE:
	EwinsReposition();
	break;
     case ESIGNAL_THEME_TRANS_CHANGE:
	EwinsTouch(DesksGetCurrent());
	break;
     case ESIGNAL_BACKGROUND_CHANGE:
	EwinsTouch((Desk *) prm);
	break;
     }
}

#if 0
static const IpcItem EwinsIpcArray[] = {
};
#define N_IPC_FUNCS (sizeof(EwinsIpcArray)/sizeof(IpcItem))
#else
#define N_IPC_FUNCS   0
#define EwinsIpcArray NULL
#endif

/*
 * Module descriptor
 */
extern const EModule ModEwins;
const EModule       ModEwins = {
   "ewins", NULL,
   EwinsSighan,
   {N_IPC_FUNCS, EwinsIpcArray}
   ,
   {0, NULL}
};
