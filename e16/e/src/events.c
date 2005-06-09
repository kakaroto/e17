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
#include "E.h"
#include <errno.h>
#include <sys/time.h>
#if USE_XRANDR
#include <X11/extensions/Xrandr.h>
#endif
#if USE_COMPOSITE
#include <X11/extensions/Xcomposite.h>
#include <X11/extensions/Xdamage.h>
#include <X11/extensions/Xfixes.h>
#include <X11/extensions/Xrender.h>
#endif

#if ENABLE_DEBUG_EVENTS
static const char  *EventName(unsigned int type);
#endif

static int          event_base_shape = 0;
static int          error_base_shape = 0;

#if USE_XRANDR
static int          event_base_randr = 0;
static int          error_base_randr = 0;
#endif

#if USE_COMPOSITE
static int          event_base_comp = 0;
static int          error_base_comp = 0;
static int          event_base_damage = 0;
static int          error_base_damage = 0;
static int          event_base_fixes = 0;
static int          error_base_fixes = 0;
static int          event_base_render = 0;
static int          error_base_render = 0;
#endif

static int          evq_num = 0;
static XEvent      *evq_ptr = NULL;

static int          evq_num2 = 0;
static int          evq_cur2 = 0;
static XEvent      *evq_ptr2 = NULL;

#define DOUBLE_CLICK_TIME 250	/* Milliseconds */

static void
EventsExtensionShowInfo(const char *name, int major, int minor,
			int event_base, int error_base)
{
   if (EventDebug(EDBUG_TYPE_VERBOSE))
      Eprintf("Found extension %-10s version %d.%d -"
	      " Event/error base = %d/%d\n", name,
	      major, minor, event_base, error_base);
}

void
EventsInit(void)
{
   int                 major, minor;

   /* Check for the Shape Extension */
   if (XShapeQueryExtension(disp, &event_base_shape, &error_base_shape))
     {
	XShapeQueryVersion(disp, &major, &minor);
	EventsExtensionShowInfo("Shape", major, minor,
				event_base_shape, error_base_shape);
     }
   else
     {
	AlertX(_("X server setup error"), "", "",
	       _("Quit Enlightenment"),
	       _("FATAL ERROR:\n" "\n"
		 "This Xserver does not support the Shape extension.\n"
		 "This is required for Enlightenment to run.\n" "\n"
		 "Your Xserver probably is too old or mis-configured.\n" "\n"
		 "Exiting.\n"));
	EExit(1);
     }

#if USE_XRANDR
   if (XRRQueryExtension(disp, &event_base_randr, &error_base_randr))
     {
	XRRQueryVersion(disp, &major, &minor);
	EventsExtensionShowInfo("RandR", major, minor,
				event_base_randr, error_base_randr);

	/* Listen for RandR events */
	XRRSelectInput(disp, VRoot.win, RRScreenChangeNotifyMask);
     }
#endif

#if USE_COMPOSITE
   if (XCompositeQueryExtension(disp, &event_base_comp, &error_base_comp))
     {
	XCompositeQueryVersion(disp, &major, &minor);
	EventsExtensionShowInfo("Composite", major, minor,
				event_base_comp, error_base_comp);
     }

   if (XDamageQueryExtension(disp, &event_base_damage, &error_base_damage))
     {
	XDamageQueryVersion(disp, &major, &minor);
	EventsExtensionShowInfo("Damage", major, minor,
				event_base_damage, error_base_damage);
     }

   if (XFixesQueryExtension(disp, &event_base_fixes, &error_base_fixes))
     {
	XFixesQueryVersion(disp, &major, &minor);
	EventsExtensionShowInfo("Fixes", major, minor,
				event_base_fixes, error_base_fixes);
     }

   if (XRenderQueryExtension(disp, &event_base_render, &error_base_render))
     {
	XRenderQueryVersion(disp, &major, &minor);
	EventsExtensionShowInfo("Render", major, minor,
				event_base_render, error_base_render);
     }
#endif
}

static void
ModeGetXY(Window rwin, int rx, int ry)
{
   Window              child;

   if (Mode.wm.window)
     {
	XTranslateCoordinates(disp, rwin, VRoot.win,
			      rx, ry, &Mode.x, &Mode.y, &child);
     }
   else
     {
	Mode.x = rx;
	Mode.y = ry;
     }
}

static void
HandleEvent(XEvent * ev)
{
   void              **lst;
   int                 i, num;

#if ENABLE_DEBUG_EVENTS
   if (EventDebug(ev->type))
      EventShow(ev);
#endif
   Mode.current_event = ev;

   switch (ev->type)
     {
     case KeyPress:
	Mode.last_keycode = ev->xkey.keycode;
     case KeyRelease:
	ModeGetXY(ev->xbutton.root, ev->xkey.x_root, ev->xkey.y_root);
	goto do_stuff;
     case ButtonPress:
     case ButtonRelease:
	ModeGetXY(ev->xbutton.root, ev->xbutton.x_root, ev->xbutton.y_root);
	goto do_stuff;
     case EnterNotify:
	Mode.context_win = ev->xany.window;
	if (ev->xcrossing.mode == NotifyGrab &&
	    ev->xcrossing.detail == NotifyInferior)
	  {
	     Mode.grabs.pointer_grab_window = ev->xany.window;
	     if (!Mode.grabs.pointer_grab_active)
		Mode.grabs.pointer_grab_active = 2;
	  }
	goto do_stuff;
     case LeaveNotify:
	if (ev->xcrossing.mode == NotifyGrab &&
	    ev->xcrossing.detail == NotifyInferior)
	  {
	     Mode.grabs.pointer_grab_window = None;
	     Mode.grabs.pointer_grab_active = 0;
	  }
	goto do_stuff;

      do_stuff:
	TooltipsHandleEvent();	/* TBD */

#if 0				/* FIXME - Why? */
	if (((ev->type == KeyPress) || (ev->type == KeyRelease))
	    && (ev->xkey.root != VRoot.win))
	  {
	     XSetInputFocus(disp, ev->xkey.root, RevertToPointerRoot,
			    CurrentTime);
	     ESync();
	     ev->xkey.time = CurrentTime;
	     XSendEvent(disp, ev->xkey.root, False, 0, ev);
	  }
	else
#endif
	  {
	     lst = ListItemType(&num, LIST_TYPE_ACLASS_GLOBAL);
	     if (lst)
	       {
		  for (i = 0; i < num; i++)
		     EventAclass(ev, GetFocusEwin(), (ActionClass *) lst[i]);
		  Efree(lst);
	       }
	  }
	break;
     }

   switch (ev->type)
     {
     case KeyPress:		/*  2 */
     case KeyRelease:		/*  3 */
	/* Unfreeze keyboard in case we got here by keygrab */
	XAllowEvents(disp, AsyncKeyboard, CurrentTime);
	break;
     case ButtonPress:		/*  4 */
	SoundPlay("SOUND_BUTTON_CLICK");

	Mode.double_click =
	   (((ev->xbutton.time - Mode.last_time) < DOUBLE_CLICK_TIME) &&
	    (ev->xbutton.button == Mode.last_button));

	Mode.last_bpress = ev->xbutton.window;
	Mode.last_time = ev->xbutton.time;
	Mode.last_button = ev->xbutton.button;

	if (Mode.double_click)
	   ev->xbutton.time = 0;
	break;
     case ButtonRelease:	/*  5 */
	SoundPlay("SOUND_BUTTON_RAISE");

#if 0				/* FIXME - TBD */
	/* DON'T handle clicks whilst moving/resizing things */
	if ((Mode.mode != MODE_NONE) &&
	    (!((Mode.place) &&
	       (Mode.mode == MODE_MOVE_PENDING || Mode.mode == MODE_MOVE))))
	  {
	     if ((int)Mode.last_button != (int)ev->xbutton.button)
		return;
	  }
#endif

	ActionsEnd(NULL);
	break;
     case MotionNotify:	/*  6 */
	TooltipsHandleEvent();	/* TBD */

	Mode.px = Mode.x;
	Mode.py = Mode.y;
	ModeGetXY(ev->xmotion.root, ev->xmotion.x_root, ev->xmotion.y_root);

	DesksSetCurrent(DesktopAt(Mode.x, Mode.y));
#if 0				/* FIXME - TBD */
	if ((!(ev->xmotion.state
	       & (Button1Mask | Button2Mask | Button3Mask | Button4Mask |
		  Button5Mask)) && (!Mode.place)))
	  {
	     if (ActionsEnd(NULL))
		return;
	  }
#endif
	ActionsHandleMotion();
	break;
     case EnterNotify:		/*  7 */
	break;
     case LeaveNotify:		/*  8 */
	break;
     case MapRequest:		/* 20 */
	break;
     case ReparentNotify:	/* 21 */
	break;
     case ConfigureNotify:	/* 22 */
	if (ev->xconfigure.window == VRoot.win)
	   RootResize(0, ev->xconfigure.width, ev->xconfigure.height);
	break;
     case ConfigureRequest:	/* 23 */
	break;
     case ResizeRequest:	/* 25 */
	break;
     case CirculateRequest:	/* 27 */
	break;
     case PropertyNotify:	/* 28 */
	break;
     case ClientMessage:	/* 33 */
	HintsProcessClientMessage(&(ev->xclient));
	break;
#if USE_XRANDR
     case EX_EVENT_SCREEN_CHANGE_NOTIFY:
	{
	   XRRScreenChangeNotifyEvent *rrev = (XRRScreenChangeNotifyEvent *) ev;

	   RootResize(1, rrev->width, rrev->height);
	}
	break;
#endif
     }

   /* The new event dispatcher */
   EventCallbacksProcess(ev);

   /* Post-event stuff TBD */
   switch (ev->type)
     {
     case ButtonRelease:	/*  5 */
	Mode.last_bpress = 0;
	Mode.action_inhibit = 0;
	break;

#if 1				/* Do this here? */
     case DestroyNotify:
	EUnregisterWindow(ev->xdestroywindow.window);
	break;
#endif
     }
}

static void
EventsCompress(XEvent * evq, int count)
{
   XEvent             *ev, *ev2;
   int                 i, j, n;
   int                 xa, ya, xb, yb;

#if ENABLE_DEBUG_EVENTS
   /* Debug - should be taken out */
   if (EventDebug(EDBUG_TYPE_COMPRESSION))
      for (i = 0; i < count; i++)
	 if (evq[i].type)
	    Eprintf("EventsCompress-1 %3d %s w=%#lx\n", i,
		    EventName(evq[i].type), evq[i].xany.window);
#endif

   /* Loop through event list, starting with latest */
   for (i = count - 1; i >= 0; i--)
     {
	ev = evq + i;

	switch (ev->type)
	  {
	  case 0:
	     /* Already thrown away */
	     break;

	  case MotionNotify:
	     /* Discard all but last motion event */
	     j = i - 1;
	     n = 0;
	     for (; j >= 0; j--)
	       {
		  ev2 = evq + j;
		  if (ev2->type == ev->type)
		    {
		       n++;
		       ev2->type = 0;
		    }
	       }
#if ENABLE_DEBUG_EVENTS
	     if (n && EventDebug(EDBUG_TYPE_COMPRESSION))
		Eprintf("EventsCompress n=%4d %s %#lx x,y = %d,%d\n",
			n, EventName(ev->type), ev->xmotion.window,
			ev->xmotion.x, ev->xmotion.y);
#endif
	     break;

	  case Expose:
	     n = 0;
	     xa = ev->xexpose.x;
	     xb = xa + ev->xexpose.width;
	     ya = ev->xexpose.y;
	     yb = ya + ev->xexpose.height;
	     for (j = i - 1; j >= 0; j--)
	       {
		  ev2 = evq + j;
		  if (ev2->type == ev->type &&
		      ev2->xexpose.window == ev->xexpose.window)
		    {
		       n++;
		       ev2->type = 0;
		       if (xa > ev2->xexpose.x)
			  xa = ev2->xexpose.x;
		       if (xb < ev2->xexpose.x + ev2->xexpose.width)
			  xb = ev2->xexpose.x + ev2->xexpose.width;
		       if (ya > ev2->xexpose.y)
			  ya = ev2->xexpose.y;
		       if (yb < ev2->xexpose.y + ev2->xexpose.height)
			  yb = ev2->xexpose.y + ev2->xexpose.height;
		    }
	       }
	     if (n)
	       {
		  ev->xexpose.x = xa;
		  ev->xexpose.width = xb - xa;
		  ev->xexpose.y = ya;
		  ev->xexpose.height = yb - ya;
	       }
#if ENABLE_DEBUG_EVENTS
	     if (EventDebug(EDBUG_TYPE_COMPRESSION))
		Eprintf("EventsCompress n=%4d %s %#lx x=%4d-%4d y=%4d-%4d\n",
			n, EventName(ev->type), ev->xexpose.window,
			xa, xb, ya, yb);
#endif
	     break;

	  case EX_EVENT_SHAPE_NOTIFY:
	     n = 0;
	     for (j = i - 1; j >= 0; j--)
	       {
		  ev2 = evq + j;
		  if (ev2->type == ev->type &&
		      ev2->xany.window == ev->xany.window)
		    {
		       n++;
		       ev2->type = 0;
		    }
	       }
#if ENABLE_DEBUG_EVENTS
	     if (n && EventDebug(EDBUG_TYPE_COMPRESSION))
		Eprintf("EventsCompress n=%4d %s %#lx\n",
			n, EventName(ev->type), ev->xmotion.window);
#endif
	     break;

	  case GraphicsExpose:
	  case NoExpose:
	     /* Not using these */
	     ev->type = 0;
	     break;

	  default:
	     break;
	  }
     }

#if ENABLE_DEBUG_EVENTS
   /* Debug - should be taken out */
   if (EventDebug(EDBUG_TYPE_COMPRESSION))
      for (i = 0; i < count; i++)
	 if (evq[i].type)
	    Eprintf("EventsCompress-2 %3d %s w=%#lx\n", i,
		    EventName(evq[i].type), evq[i].xany.window);
#endif
}

static int
EventsFetch(XEvent ** evq_p, int *evq_n)
{
   int                 i, n, count;
   XEvent             *evq = *evq_p, *ev;
   int                 qsz = *evq_n;

   /* Fetch the entire event queue */
   for (i = count = qsz; (n = XPending(disp)) > 0;)
     {
	count += n;
	if (count > qsz)
	  {
	     qsz = count;
	     evq = Erealloc(evq, sizeof(XEvent) * qsz);
	  }
	ev = evq + i;
	for (; i < count; i++, ev++)
	  {
	     XNextEvent(disp, ev);

	     /* Map some event types to E internals */
	     if (ev->type == event_base_shape + ShapeNotify)
		ev->type = EX_EVENT_SHAPE_NOTIFY;
#if USE_XRANDR
	     else if (ev->type == event_base_randr + RRScreenChangeNotify)
		ev->type = EX_EVENT_SCREEN_CHANGE_NOTIFY;
#endif
#if USE_COMPOSITE
	     else if (ev->type == event_base_damage + XDamageNotify)
		ev->type = EX_EVENT_DAMAGE_NOTIFY;
#endif
	  }
     }

   EventsCompress(evq, count);

   *evq_p = evq;
   *evq_n = qsz;

   return count;
}

static int
EventsProcess(XEvent ** evq_p, int *evq_n)
{
   int                 i;
   XEvent             *evq;
   int                 qsz;

   /* Fetch the entire event queue */
   EventsFetch(evq_p, evq_n);
   evq = *evq_p;
   qsz = *evq_n;

 again:
   if (EventDebug(EDBUG_TYPE_EVENTS))
      Eprintf("EventsProcess-B %d\n", qsz);

   for (i = 0; i < qsz; i++)
     {
	if (evq[i].type == 0)
	   continue;

	if (EventDebug(EDBUG_TYPE_EVENTS))
	   Eprintf("EventsProcess %d type=%d\n", i, evq[i].type);

	HandleEvent(evq + i);
	evq[i].type = 0;
     }

   if (evq_num2)
     {
	/* Process leftovers */
	Efree(evq_ptr);
	evq = evq_ptr = evq_ptr2;
	qsz = evq_num = evq_num2;
	evq_ptr2 = 0;
	evq_num2 = evq_cur2 = 0;
	goto again;
     }

   if (EventDebug(EDBUG_TYPE_EVENTS))
      Eprintf("EventsProcess-E %d\n", qsz);

   return qsz;
}

#if 0				/* FIXME - Remove? */
void
CheckEvent(void)
{
   int                 i;
   XEvent             *evq;
   int                 qsz;

   /* Fetch the entire event queue */
   EventsFetch(&evq_ptr2, &evq_num2);
   evq = evq_ptr2;
   qsz = evq_num2;

   if (EventDebug(EDBUG_TYPE_EVENTS))
      Eprintf("CheckEvent-B %d/%d\n", evq_cur2, evq_num2);

   for (i = evq_cur2; i < qsz; i++)
     {
	evq_cur2 = i;
	switch (evq[i].type)
	  {
	  case CreateNotify:
	  case DestroyNotify:
	  case ReparentNotify:
	  case UnmapNotify:
	  case MapNotify:
	  case ConfigureNotify:
	  case Expose:
	  case EX_EVENT_DAMAGE_NOTIFY:
	     if (EventDebug(EDBUG_TYPE_EVENTS))
		Eprintf("CheckEvent %d type=%d\n", i, evq[i].type);

	     HandleEvent(evq + i);
	     evq[i].type = 0;
	     break;
	  }
     }

   if (EventDebug(EDBUG_TYPE_EVENTS))
      Eprintf("CheckEvent-E\n");

   ModulesSignal(ESIGNAL_IDLE, NULL);
}
#endif

/*
 * This is the primary event loop.  Everything that is going to happen in the
 * window manager has to start here at some point.  This is where all the
 * events from the X server are interpreted, timer events are inserted, etc
 */
void
WaitEvent(void)
{
   fd_set              fdset;
   struct timeval      tval;
   static struct timeval tval_last = { 0, 0 };
   double              time1, time2;
   Qentry             *qe;
   int                 count, pcount;
   int                 fdsize;
   int                 xfd, smfd;

   smfd = GetSMfd();
   xfd = ConnectionNumber(disp);
   fdsize = MAX(xfd, smfd) + 1;

   /* if we've never set the time we were last here before */
   if ((tval_last.tv_sec == 0) && (tval_last.tv_usec == 0))
      gettimeofday(&tval_last, NULL);

   /* time1 = time we last entered this routine */
   time1 = ((double)tval_last.tv_sec) + (((double)tval_last.tv_usec) / 1000000);
   gettimeofday(&tval, NULL);
   tval_last.tv_sec = tval.tv_sec;
   tval_last.tv_usec = tval.tv_usec;

   /* time2 = current time */
   time2 = ((double)tval.tv_sec) + (((double)tval.tv_usec) / 1000000);
   time2 -= time1;
   if (time2 < 0.0)
      time2 = 0.0;
   /* time2 = time spent since we last were here */

   evq_num = 0;
   count = EventsProcess(&evq_ptr, &evq_num);

   DialogsCheckUpdate();
   ModulesSignal(ESIGNAL_IDLE, NULL);

   XFlush(disp);
   pcount = count;

   evq_num = 0;
   count = EventsProcess(&evq_ptr, &evq_num);

   if (count > 0)
      XFlush(disp);

   if (pcount > count)
      count = pcount;
   if ((evq_ptr) && ((evq_num - count) > 64))
     {
	evq_num = 0;
	Efree(evq_ptr);
	evq_ptr = NULL;
     }

   FD_ZERO(&fdset);
   FD_SET(xfd, &fdset);
   if (smfd >= 0)
      FD_SET(smfd, &fdset);

   qe = GetHeadTimerQueue();
   if (qe)
     {
	if (qe->just_added)
	  {
	     qe->just_added = 0;
	     time1 = qe->in_time;
	  }
	else
	  {
	     time1 = qe->in_time - time2;
	     if (time1 < 0.0)
		time1 = 0.0;
	     qe->in_time = time1;
	  }
	tval.tv_sec = (long)time1;
	tval.tv_usec = (long)((time1 - ((double)tval.tv_sec)) * 1000000);
	count = select(fdsize, &fdset, NULL, NULL, &tval);
     }
   else
      count = select(fdsize, &fdset, NULL, NULL, NULL);

   if (count < 0)
      return;

   if ((smfd >= 0) && (count > 0) && (FD_ISSET(smfd, &fdset)))
      ProcessICEMSGS();

   if ((!(FD_ISSET(xfd, &fdset))) && (qe) && (count == 0)
       && (((smfd >= 0) && (!(FD_ISSET(smfd, &fdset)))) || (smfd < 0)))
      HandleTimerEvent();
}

#if ENABLE_DEBUG_EVENTS
/*
 * Event debug stuff
 */
#define N_DEBUG_FLAGS 256
static char         ev_debug;
static char         ev_debug_flags[N_DEBUG_FLAGS];

/*
 * param is <EventNumber>[:<EventNumber> ... ]
 */
void
EventDebugInit(const char *param)
{
   const char         *s;
   int                 ix, onoff;

   if (!param)
      return;

   for (;;)
     {
	s = strchr(param, ':');
	if (!param[0])
	   break;
	ev_debug = 1;
	ix = strtol(param, NULL, 0);
	onoff = (ix >= 0);
	if (ix < 0)
	   ix = -ix;
	if (ix < N_DEBUG_FLAGS)
	   ev_debug_flags[ix] = onoff;
	if (!s)
	   break;
	param = s + 1;
     }
}

int
EventDebug(unsigned int type)
{
   return ev_debug && (type < sizeof(ev_debug_flags)) && ev_debug_flags[type];
}

void
EventDebugSet(unsigned int type, int value)
{
   if (type >= sizeof(ev_debug_flags))
      return;

   ev_debug = 1;
   ev_debug_flags[type] = value;
}

static const char  *const TxtEventNames[] = {
   "Error", "Reply", "KeyPress", "KeyRelease", "ButtonPress",
   "ButtonRelease", "MotionNotify", "EnterNotify", "LeaveNotify", "FocusIn",
   "FocusOut", "KeymapNotify", "Expose", "GraphicsExpose", "NoExpose",
   "VisibilityNotify", "CreateNotify", "DestroyNotify", "UnmapNotify",
   "MapNotify",
   "MapRequest", "ReparentNotify", "ConfigureNotify", "ConfigureRequest",
   "GravityNotify",
   "ResizeRequest", "CirculateNotify", "CirculateRequest", "PropertyNotify",
   "SelectionClear",
   "SelectionRequest", "SelectionNotify", "ColormapNotify", "ClientMessage",
   "MappingNotify"
};
#define N_EVENT_NAMES (sizeof(TxtEventNames)/sizeof(char*))

static const char  *
EventName(unsigned int type)
{
   static char         buf[16];

   if (type < N_EVENT_NAMES)
      return TxtEventNames[type];

   switch (type)
     {
     case EX_EVENT_SHAPE_NOTIFY:
	return "ShapeNotify";
     case EX_EVENT_SCREEN_CHANGE_NOTIFY:
	return "ScreenChangeNotify";
     case EX_EVENT_DAMAGE_NOTIFY:
	return "DamageNotify";
     }

   sprintf(buf, "%d", type);
   return buf;
}

static const char  *const TxtEventNotifyModeNames[] = {
   "NotifyNormal", "NotifyGrab", "NotifyUngrab", "NotifyWhileGrabbed"
};
#define N_EVENT_NOTIFY_MODE_NAMES (sizeof(TxtEventNotifyModeNames)/sizeof(char*))

static const char  *
EventNotifyModeName(unsigned int mode)
{
   if (mode < N_EVENT_NOTIFY_MODE_NAMES)
      return TxtEventNotifyModeNames[mode];

   return "Unknown";
}

static const char  *const TxtEventNotifyDetailNames[] = {
   "NotifyAncestor", "NotifyVirtual", "NotifyInferior", "NotifyNonlinear",
   "NotifyNonlinearVirtual", "NotifyPointer", "NotifyPointerRoot",
   "NotifyDetailNone"
};
#define N_EVENT_NOTIFY_DETAIL_NAMES (sizeof(TxtEventNotifyDetailNames)/sizeof(char*))

static const char  *
EventNotifyDetailName(unsigned int detail)
{
   if (detail < N_EVENT_NOTIFY_DETAIL_NAMES)
      return TxtEventNotifyDetailNames[detail];

   return "Unknown";
}

void
EventShow(const XEvent * ev)
{
   unsigned long       ser = ev->xany.serial;
   Window              win = ev->xany.window;
   const char         *name = EventName(ev->type);
   char               *txt;

   switch (ev->type)
     {
     case KeyPress:
     case KeyRelease:
	goto case_common;
     case ButtonPress:
     case ButtonRelease:
	Eprintf("%#08lx EV-%s win=%#lx state=%#x button=%#x\n", ser, name, win,
		ev->xbutton.state, ev->xbutton.button);
	break;
     case MotionNotify:
	goto case_common;
     case EnterNotify:
     case LeaveNotify:
	Eprintf("%#08lx EV-%s win=%#lx m=%s d=%s\n", ser, name, win,
		EventNotifyModeName(ev->xcrossing.mode),
		EventNotifyDetailName(ev->xcrossing.detail));
	break;
     case FocusIn:
     case FocusOut:
	Eprintf("%#08lx EV-%s win=%#lx m=%s d=%s\n", ser, name, win,
		EventNotifyModeName(ev->xfocus.mode),
		EventNotifyDetailName(ev->xfocus.detail));
	break;
     case KeymapNotify:
     case Expose:
     case GraphicsExpose:
	Eprintf("%#08lx EV-%s: win=%#lx %d+%d %dx%d\n", ser, name, win,
		ev->xexpose.x, ev->xexpose.y,
		ev->xexpose.width, ev->xexpose.height);
	break;
     case VisibilityNotify:
	Eprintf("%#08lx EV-%s win=%#lx state=%d\n", ser, name, win,
		ev->xvisibility.state);
	break;
     case CreateNotify:
     case DestroyNotify:
     case UnmapNotify:
     case MapNotify:
     case MapRequest:
	Eprintf("%#08lx EV-%s ev=%#lx win=%#lx\n", ser, name, win,
		ev->xcreatewindow.window);
	break;
     case ReparentNotify:
	Eprintf("%#08lx EV-%s ev=%#lx win=%#lx parent=%#lx %d+%d\n", ser, name,
		win, ev->xreparent.window, ev->xreparent.parent,
		ev->xreparent.x, ev->xreparent.y);
	break;
     case ConfigureNotify:
	Eprintf
	   ("%#08lx EV-%s: ev=%#lx, win=%#lx %d+%d %dx%d bw=%d above=%#lx\n",
	    ser, name, win, ev->xconfigure.window, ev->xconfigure.x,
	    ev->xconfigure.y, ev->xconfigure.width, ev->xconfigure.height,
	    ev->xconfigure.border_width, ev->xconfigure.above);
	break;
     case ConfigureRequest:
	Eprintf
	   ("%#08lx EV-%s: ev=%#lx win=%#lx m=%#lx %d+%d %dx%d bw=%d above=%#lx stk=%d\n",
	    ser, name, win, ev->xconfigurerequest.window,
	    ev->xconfigurerequest.value_mask, ev->xconfigurerequest.x,
	    ev->xconfigurerequest.y, ev->xconfigurerequest.width,
	    ev->xconfigurerequest.height, ev->xconfigurerequest.border_width,
	    ev->xconfigurerequest.above, ev->xconfigurerequest.detail);
	break;
     case GravityNotify:
	goto case_common;
     case ResizeRequest:
	Eprintf("%#08lx EV-%s: win=%#lx %dx%d\n",
		ser, name, win, ev->xresizerequest.width,
		ev->xresizerequest.height);
	break;
     case CirculateNotify:
     case CirculateRequest:
	goto case_common;
     case PropertyNotify:
	txt = XGetAtomName(disp, ev->xproperty.atom);
	Eprintf("%#08lx EV-%s: win=%#lx Atom=%s(%ld)\n",
		ser, name, win, txt, ev->xproperty.atom);
	XFree(txt);
	break;
     case SelectionClear:
     case SelectionRequest:
     case SelectionNotify:
     case ColormapNotify:
	goto case_common;
     case ClientMessage:
	txt = XGetAtomName(disp, ev->xclient.message_type);
	Eprintf
	   ("%#08lx EV-%s win=%#lx ev_type=%s(%ld) data[0-3]= %08lx %08lx %08lx %08lx\n",
	    ser, name, win, txt, ev->xclient.message_type,
	    ev->xclient.data.l[0], ev->xclient.data.l[1], ev->xclient.data.l[2],
	    ev->xclient.data.l[3]);
	XFree(txt);
	break;
     case MappingNotify:
      case_common:
	Eprintf("%#08lx EV-%s win=%#lx\n", ser, name, win);
	break;
     case EX_EVENT_SHAPE_NOTIFY:
	Eprintf("%#08lx EV-%s win=%#lx\n", ser, name, win);
	break;
#if USE_XRANDR
     case EX_EVENT_SCREEN_CHANGE_NOTIFY:
	Eprintf("%#08lx EV-%s win=%#lx\n", ser, name, win);
	break;
#endif
#if USE_COMPOSITE
#define de ((XDamageNotifyEvent *)ev)
     case EX_EVENT_DAMAGE_NOTIFY:
	Eprintf("%#08lx EV-%s win=%#lx %d+%d %dx%d\n", ser, name, win,
		de->area.x, de->area.y, de->area.width, de->area.height);
	break;
#undef de
#endif
     default:
	Eprintf("%#08lx EV-%s win=%#lx\n", ser, name, win);
	break;
     }
}

#else

void
EventDebugInit(const char *param __UNUSED__)
{
}

#endif /* ENABLE_DEBUG_EVENTS */
