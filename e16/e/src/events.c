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
#include "emodule.h"
#include "ewins.h"		/* FIXME - Should not be here */
#include "xwin.h"
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
			      rx, ry, &Mode.events.x, &Mode.events.y, &child);
     }
   else
     {
	Mode.events.x = rx;
	Mode.events.y = ry;
     }
}

static void
HandleEvent(XEvent * ev)
{
#if ENABLE_DEBUG_EVENTS
   if (EventDebug(ev->type))
      EventShow(ev);
#endif

   switch (ev->type)
     {
     case KeyPress:
	Mode.events.last_keycode = ev->xkey.keycode;
     case KeyRelease:
	Mode.events.time = ev->xkey.time;
	ModeGetXY(ev->xbutton.root, ev->xkey.x_root, ev->xkey.y_root);
#if 0				/* FIXME - Why? */
	if (ev->xkey.root != VRoot.win)
	  {
	     XSetInputFocus(disp, ev->xkey.root, RevertToPointerRoot,
			    CurrentTime);
	     ESync();
	     ev->xkey.time = CurrentTime;
	     XSendEvent(disp, ev->xkey.root, False, 0, ev);
	     return;
	  }
#endif
	goto do_stuff;

     case ButtonPress:
     case ButtonRelease:
	Mode.events.time = ev->xbutton.time;
	ModeGetXY(ev->xbutton.root, ev->xbutton.x_root, ev->xbutton.y_root);
	goto do_stuff;

     case MotionNotify:
	Mode.events.time = ev->xmotion.time;
	break;

     case EnterNotify:
	Mode.context_win = ev->xany.window;
	Mode.events.time = ev->xcrossing.time;
	if (ev->xcrossing.mode == NotifyGrab &&
	    ev->xcrossing.detail == NotifyInferior)
	  {
	     Mode.grabs.pointer_grab_window = ev->xany.window;
	     if (!Mode.grabs.pointer_grab_active)
		Mode.grabs.pointer_grab_active = 2;
	  }
	goto do_stuff;

     case LeaveNotify:
	Mode.events.time = ev->xcrossing.time;
	if (ev->xcrossing.mode == NotifyGrab &&
	    ev->xcrossing.detail == NotifyInferior)
	  {
	     Mode.grabs.pointer_grab_window = None;
	     Mode.grabs.pointer_grab_active = 0;
	  }
	goto do_stuff;

     case PropertyNotify:
	Mode.events.time = ev->xproperty.time;
	break;

      do_stuff:
	ActionclassesEvent(ev, GetFocusEwin());
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

	Mode.events.double_click =
	   (((ev->xbutton.time - Mode.events.last_btime) < DOUBLE_CLICK_TIME) &&
	    (ev->xbutton.button == Mode.events.last_button));

	Mode.events.last_bpress = ev->xbutton.window;
	Mode.events.last_btime = ev->xbutton.time;
	Mode.events.last_button = ev->xbutton.button;

	if (Mode.events.double_click)
	   ev->xbutton.time = 0;
	break;
     case ButtonRelease:	/*  5 */
	SoundPlay("SOUND_BUTTON_RAISE");
	ActionsEnd(NULL);
	break;

     case MotionNotify:	/*  6 */
	Mode.events.px = Mode.events.x;
	Mode.events.py = Mode.events.y;
	ModeGetXY(ev->xmotion.root, ev->xmotion.x_root, ev->xmotion.y_root);

	DesksSetCurrent(DesktopAt(Mode.events.x, Mode.events.y));

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
	Mode.events.last_bpress = 0;
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
   for (i = count = 0; (n = XPending(disp)) > 0;)
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
EventsProcess(XEvent ** evq_p, int *evq_n, int *evq_f)
{
   int                 i, n, count;
   XEvent             *evq;

   /* Fetch the entire event queue */
   n = EventsFetch(evq_p, evq_n);
   evq = *evq_p;

   if (EventDebug(EDBUG_TYPE_EVENTS))
      Eprintf("EventsProcess-B %d\n", n);

   for (i = count = 0; i < n; i++)
     {
	if (evq[i].type == 0)
	   continue;

	if (EventDebug(EDBUG_TYPE_EVENTS) > 1)
	   Eprintf("EventsProcess %d type=%d\n", i, evq[i].type);

	count++;
	HandleEvent(evq + i);
	evq[i].type = 0;
     }

   if (EventDebug(EDBUG_TYPE_EVENTS))
      Eprintf("EventsProcess-E %d/%d\n", count, n);

   if (n > *evq_f)
      *evq_f = n;

   return count;
}

/*
 * This is the primary event loop.  Everything that is going to happen in the
 * window manager has to start here at some point.  This is where all the
 * events from the X server are interpreted, timer events are inserted, etc
 */
void
WaitEvent(void)
{
   static int          evq_alloc = 0;
   static int          evq_fetch = 0;
   static XEvent      *evq_ptr = NULL;
   fd_set              fdset;
   struct timeval      tval;
   static struct timeval tval_last = { 0, 0 };
   double              time1, time2;
   Qentry             *qe;
   int                 count, pcount, pfetch;
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

   pcount = pfetch = 0;
   for (;;)
     {
	count = EventsProcess(&evq_ptr, &evq_alloc, &pfetch);

	if (count > pcount)
	   pcount = count;

	if (XPending(disp))
	   continue;

	if (EventDebug(EDBUG_TYPE_EVENTS))
	   Eprintf("WaitEvent - Idlers\n");
	DialogsCheckUpdate();	/* FIXME - Shouldn't be here */
	ModulesSignal(ESIGNAL_IDLE, NULL);

	if (!XPending(disp))
	   break;
     }

   if (pfetch)
     {
	evq_fetch =
	   (pfetch > evq_fetch) ? pfetch : (3 * evq_fetch + pfetch) / 4;
	if (EventDebug(EDBUG_TYPE_EVENTS))
	   Eprintf("WaitEvent - Alloc/fetch/pfetch/peak=%d/%d/%d/%d)\n",
		   evq_alloc, evq_fetch, pfetch, pcount);
	if ((evq_ptr) && ((evq_alloc - evq_fetch) > 64))
	  {
	     evq_alloc = 0;
	     Efree(evq_ptr);
	     evq_ptr = NULL;
	  }
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
     {
	if (EventDebug(EDBUG_TYPE_EVENTS))
	   Eprintf("WaitEvent - ICE\n");
	ProcessICEMSGS();
     }

   if ((!(FD_ISSET(xfd, &fdset))) && (qe) && (count == 0)
       && (((smfd >= 0) && (!(FD_ISSET(smfd, &fdset)))) || (smfd < 0)))
     {
	if (EventDebug(EDBUG_TYPE_EVENTS))
	   Eprintf("WaitEvent - Timers (%s)\n", qe->name);
	HandleTimerEvent();
     }
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
	  {
	     if (onoff)
		ev_debug_flags[ix]++;
	     else
		ev_debug_flags[ix] = 0;
	  }
	if (!s)
	   break;
	param = s + 1;
     }
}

int
EventDebug(unsigned int type)
{
   return (ev_debug &&
	   (type < sizeof(ev_debug_flags))) ? ev_debug_flags[type] : 0;
}

void
EventDebugSet(unsigned int type, int value)
{
   if (type >= sizeof(ev_debug_flags))
      return;

   ev_debug = 1;
   ev_debug_flags[type] += value;
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
	Eprintf("%#08lx EV-%s win=%#lx sub=%#lx state=%#x button=%#x\n", ser,
		name, win, ev->xbutton.subwindow, ev->xbutton.state,
		ev->xbutton.button);
	break;
     case MotionNotify:
	Eprintf("%#08lx EV-%s win=%#lx sub=%#lx\n", ser, name, win,
		ev->xcrossing.subwindow);
	break;
     case EnterNotify:
     case LeaveNotify:
	Eprintf("%#08lx EV-%s win=%#lx sub=%#lx m=%s d=%s\n", ser, name, win,
		ev->xcrossing.subwindow,
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
