/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#include "emodule.h"
#include "session.h"
#include "timers.h"
#include "xwin.h"
#include <sys/time.h>
#if USE_XSYNC
#include <X11/extensions/sync.h>
#endif
#if USE_XSCREENSAVER
#include <X11/extensions/scrnsaver.h>
#endif
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

typedef struct
{
   int                 event_base, error_base;
   int                 major, minor;
} EServerExtData;

typedef struct
{
   const char         *name;
   unsigned int        ix;
   int                 (*query_ext) (Display * dpy, int *event, int *error);
   int                 (*query_ver) (Display * dpy, int *major, int *minor);
   void                (*init) (int avaliable);
} EServerExt;

static EServerExtData ExtData[8];

#define event_base_shape ExtData[XEXT_SHAPE].event_base
#define event_base_randr ExtData[XEXT_RANDR].event_base
#define event_base_damage ExtData[XEXT_DAMAGE].event_base

static void
ExtInitShape(int available)
{
   if (available)
      return;

   AlertX(_("X server setup error"), _("OK"), NULL, NULL,
	  _("FATAL ERROR:\n" "\n"
	    "This Xserver does not support the Shape extension.\n"
	    "This is required for Enlightenment to run.\n" "\n"
	    "Your Xserver probably is too old or mis-configured.\n" "\n"
	    "Exiting.\n"));
   EExit(1);
}

#if USE_XSYNC
static void
ExtInitSync(int available)
{
   if (!available)
      return;

   if (EventDebug(EDBUG_TYPE_VERBOSE))
     {
	int                 i, num;
	XSyncSystemCounter *xssc;

	xssc = XSyncListSystemCounters(disp, &num);
	for (i = 0; i < num; i++)
	  {
	     Eprintf(" Sync counter %2d: %10s %#lx %#x:%#x\n", i,
		     xssc[i].name, xssc[i].counter,
		     XSyncValueHigh32(xssc[i].resolution),
		     XSyncValueLow32(xssc[i].resolution));
	  }
     }
}
#endif

#if USE_XSCREENSAVER
static void
ExtInitSS(int available)
{
   if (!available)
      return;

   if (EventDebug(EDBUG_TYPE_VERBOSE))
     {
	XScreenSaverInfo   *xssi = XScreenSaverAllocInfo();

	XScreenSaverQueryInfo(disp, VRoot.xwin, xssi);
	Eprintf(" Screen saver window=%#lx\n", xssi->window);
	XFree(xssi);
     }
}
#endif

#if USE_XRANDR
static void
ExtInitRR(int available)
{
   if (!available)
      return;

   /* Listen for RandR events */
   XRRSelectInput(disp, VRoot.xwin, RRScreenChangeNotifyMask);
}
#endif

static const EServerExt Extensions[] = {
   {"Shape", XEXT_SHAPE, XShapeQueryExtension, XShapeQueryVersion,
    ExtInitShape},
#if USE_XSYNC
   {"Sync", XEXT_SYNC, XSyncQueryExtension, XSyncInitialize, ExtInitSync},
#endif
#if USE_XSCREENSAVER
   {"ScrSaver", XEXT_SCRSAVER, XScreenSaverQueryExtension,
    XScreenSaverQueryVersion, ExtInitSS},
#endif
#if USE_XRANDR
   {"RandR", XEXT_RANDR, XRRQueryExtension, XRRQueryVersion, ExtInitRR},
#endif
#if USE_COMPOSITE
   {"Composite", XEXT_COMPOSITE, XCompositeQueryExtension,
    XCompositeQueryVersion, NULL},
   {"Damage", XEXT_DAMAGE, XDamageQueryExtension, XDamageQueryVersion, NULL},
   {"Fixes", XEXT_FIXES, XFixesQueryExtension, XFixesQueryVersion, NULL},
   {"Render", XEXT_RENDER, XRenderQueryExtension, XRenderQueryVersion, NULL},
#endif
};

static void
ExtQuery(const EServerExt * ext)
{
   int                 available;
   EServerExtData     *exd = ExtData + ext->ix;

   available = ext->query_ext(disp, &(exd->event_base), &(exd->error_base));

   if (available)
     {
	Mode.server.extensions |= 1 << ext->ix;

	ext->query_ver(disp, &(exd->major), &(exd->minor));

	if (EventDebug(EDBUG_TYPE_VERBOSE))
	   Eprintf("Found extension %-10s version %d.%d -"
		   " Event/error base = %d/%d\n", ext->name,
		   exd->major, exd->minor, exd->event_base, exd->error_base);
     }

   if (ext->init)
      ext->init(available);
}

#define DOUBLE_CLICK_TIME 250	/* Milliseconds */

void
EventsInit(void)
{
   unsigned int        i;

   memset(ExtData, 0, sizeof(ExtData));

   for (i = 0; i < sizeof(Extensions) / sizeof(EServerExt); i++)
      ExtQuery(Extensions + i);

#if USE_COMPOSITE
#define XEXT_MASK_CM_ALL ((1 << XEXT_COMPOSITE) | (1 << XEXT_DAMAGE) | \
                          (1 << XEXT_FIXES) | (1 << XEXT_RENDER))
   if (((Mode.server.extensions & XEXT_MASK_CM_ALL) == XEXT_MASK_CM_ALL) &&
       (ExtData[XEXT_COMPOSITE].major > 0 ||
	ExtData[XEXT_COMPOSITE].minor >= 2))
      Mode.server.extensions |= 1 << XEXT_CM_ALL;
#endif
}

static void
ModeGetXY(Window rwin, int rx, int ry)
{
   Window              child;

   if (Mode.wm.window)
     {
	XTranslateCoordinates(disp, rwin, VRoot.xwin,
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
   Win                 win;

#if ENABLE_DEBUG_EVENTS
   if (EventDebug(ev->type))
      EventShow(ev);
#endif

   win = ELookupXwin(ev->xany.window);

   switch (ev->type)
     {
     case KeyPress:
	Mode.events.last_keycode = ev->xkey.keycode;
     case KeyRelease:
	Mode.events.time = ev->xkey.time;
	ModeGetXY(ev->xbutton.root, ev->xkey.x_root, ev->xkey.y_root);
#if 0				/* FIXME - Why? */
	if (ev->xkey.root != VRoot.xwin)
	  {
	     XSetInputFocus(disp, ev->xkey.root, RevertToPointerRoot,
			    CurrentTime);
	     ESync();
	     ev->xkey.time = CurrentTime;
	     XSendEvent(disp, ev->xkey.root, False, 0, ev);
	     return;
	  }
#endif
	Mode.events.on_screen = ev->xkey.same_screen;
	goto do_stuff;

     case ButtonPress:
     case ButtonRelease:
	Mode.events.time = ev->xbutton.time;
	ModeGetXY(ev->xbutton.root, ev->xbutton.x_root, ev->xbutton.y_root);
	Mode.events.on_screen = ev->xbutton.same_screen;
	goto do_stuff;

     case MotionNotify:
	Mode.events.time = ev->xmotion.time;
	Mode.events.px = Mode.events.x;
	Mode.events.py = Mode.events.y;
	ModeGetXY(ev->xmotion.root, ev->xmotion.x_root, ev->xmotion.y_root);
	Mode.events.on_screen = ev->xmotion.same_screen;

	ActionsHandleMotion();
	break;

     case EnterNotify:
	Mode.context_win = win;
	Mode.events.time = ev->xcrossing.time;
	Mode.events.on_screen = ev->xcrossing.same_screen;
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
	Mode.events.on_screen = ev->xcrossing.same_screen;
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
	if (ev->xany.window == VRoot.xwin)
	   ActionclassesGlobalEvent(ev);
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
	   ((ev->xbutton.time - Mode.events.last_btime < DOUBLE_CLICK_TIME) &&
	    ev->xbutton.button == Mode.events.last_button &&
	    ev->xbutton.window == Mode.events.last_bpress2);

	Mode.events.last_bpress = ev->xbutton.window;
	Mode.events.last_bpress2 = ev->xbutton.window;
	Mode.events.last_btime = ev->xbutton.time;
	Mode.events.last_button = ev->xbutton.button;
	break;
     case ButtonRelease:	/*  5 */
	SoundPlay("SOUND_BUTTON_RAISE");
	ActionsEnd(NULL);
	break;
     }

   /* The new event dispatcher */
   EventCallbacksProcess(win, ev);

   /* Post-event stuff TBD */
   switch (ev->type)
     {
     case ButtonRelease:	/*  5 */
	Mode.events.last_bpress = 0;
	Mode.action_inhibit = 0;
	break;

#if 1				/* Do this here? */
     case DestroyNotify:
	EUnregisterXwin(ev->xdestroywindow.window);
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

	  case DestroyNotify:
	     for (j = i - 1; j >= 0; j--)
	       {
		  ev2 = evq + j;
		  switch (ev2->type)
		    {
		    case CreateNotify:
		       if (ev2->xcreatewindow.window !=
			   ev->xdestroywindow.window)
			  continue;
		       ev2->type = EX_EVENT_CREATE_GONE;
		       j = -1;	/* Break for() */
		       break;
		    case DestroyNotify:
		       break;
		    case UnmapNotify:
		       if (ev2->xunmap.window != ev->xdestroywindow.window)
			  continue;
		       ev2->type = EX_EVENT_UNMAP_GONE;
		       break;
		    case MapNotify:
		       if (ev2->xmap.window != ev->xdestroywindow.window)
			  continue;
		       ev2->type = EX_EVENT_MAP_GONE;
		       break;
		    case MapRequest:
		       if (ev2->xmaprequest.window != ev->xdestroywindow.window)
			  continue;
		       ev2->type = EX_EVENT_MAPREQUEST_GONE;
		       break;
		    case ReparentNotify:
		       if (ev2->xreparent.window != ev->xdestroywindow.window)
			  continue;
		       ev2->type = EX_EVENT_REPARENT_GONE;
		       break;
		    case ConfigureRequest:
		       if (ev2->xconfigurerequest.window !=
			   ev->xdestroywindow.window)
			  continue;
		       ev2->type = 0;
		       break;
		    default:
		       /* Nuke all other events on a destroyed window */
		       if (ev2->xany.window != ev->xdestroywindow.window)
			  continue;
		       ev2->type = 0;
		       break;
		    }
	       }
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
	   EventShow(evq + i);

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
EventsMain(void)
{
   static int          evq_alloc = 0;
   static int          evq_fetch = 0;
   static XEvent      *evq_ptr = NULL;
   fd_set              fdset;
   struct timeval      tval;
   double              time1, time2, dt;
   int                 count, pfetch;
   int                 fdsize;
   int                 xfd, smfd;

   time1 = GetTime();

   for (;;)
     {
	pfetch = 0;
	count = EventsProcess(&evq_ptr, &evq_alloc, &pfetch);

	if (EventDebug(EDBUG_TYPE_EVENTS))
	   Eprintf("EventsMain - Idlers\n");
	IdlersRun();

	if (pfetch)
	  {
	     evq_fetch =
		(pfetch > evq_fetch) ? pfetch : (3 * evq_fetch + pfetch) / 4;
	     if (EventDebug(EDBUG_TYPE_EVENTS))
		Eprintf("EventsMain - Alloc/fetch/pfetch/peak=%d/%d/%d/%d)\n",
			evq_alloc, evq_fetch, pfetch, count);
	     if ((evq_ptr) && ((evq_alloc - evq_fetch) > 64))
	       {
		  evq_alloc = 0;
		  Efree(evq_ptr);
		  evq_ptr = NULL;
	       }
	  }

	FD_ZERO(&fdset);
	xfd = ConnectionNumber(disp);
	FD_SET(xfd, &fdset);
	smfd = GetSMfd();
	if (smfd >= 0)
	   FD_SET(smfd, &fdset);
	fdsize = MAX(xfd, smfd) + 1;

	/* time2 = current time */
	time2 = GetTime();
	dt = time2 - time1;
	time1 = time2;
	if (dt < 0.0)
	   dt = 0.0;
	/* dt = time spent since we last were here */

	count = TimersPending(&time2);
	if (count >= 0)
	  {
	     if (count > 0)
	       {
		  if (XPending(disp))
		     continue;
		  tval.tv_sec = (long)time2;
		  tval.tv_usec =
		     (long)((time2 - ((double)tval.tv_sec)) * 1000000);
		  count = select(fdsize, &fdset, NULL, NULL, &tval);
	       }
	     if (count == 0)
	       {
		  TimersRun();
	       }
	  }
	else
	  {
	     if (XPending(disp))
		continue;
	     count = select(fdsize, &fdset, NULL, NULL, NULL);
	  }

	if (EventDebug(EDBUG_TYPE_EVENTS))
	   Eprintf
	      ("EventsMain - count=%d xfd=%d:%d smfd=%d:%d dt=%lf time2=%lf\n",
	       count, xfd, FD_ISSET(xfd, &fdset), smfd,
	       (smfd >= 0) ? FD_ISSET(smfd, &fdset) : 0, dt, time2);

	if (count > 0)
	  {
	     if ((smfd >= 0) && (FD_ISSET(smfd, &fdset)))
	       {
		  if (EventDebug(EDBUG_TYPE_EVENTS))
		     Eprintf("EventsMain - ICE\n");
		  ProcessICEMSGS();
	       }
	  }
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
     case EX_EVENT_CREATE_GONE:
	return "Create-Gone";
     case EX_EVENT_UNMAP_GONE:
	return "Unmap-Gone";
     case EX_EVENT_MAP_GONE:
	return "Map-Gone";
     case EX_EVENT_MAPREQUEST_GONE:
	return "MapRequest-Gone";
     case EX_EVENT_REPARENT_GONE:
	return "Reparent-Gone";
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
	Eprintf
	   ("%#08lx EV-%s win=%#lx sub=%#lx m=%s d=%s sscreen=%d focus=%d\n",
	    ser, name, win, ev->xcrossing.subwindow,
	    EventNotifyModeName(ev->xcrossing.mode),
	    EventNotifyDetailName(ev->xcrossing.detail),
	    ev->xcrossing.same_screen, ev->xcrossing.focus);
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
     case EX_EVENT_CREATE_GONE:
     case EX_EVENT_UNMAP_GONE:
     case EX_EVENT_MAP_GONE:
     case EX_EVENT_MAPREQUEST_GONE:
	Eprintf("%#08lx EV-%s ev=%#lx win=%#lx\n", ser, name, win,
		ev->xcreatewindow.window);
	break;
     case ReparentNotify:
     case EX_EVENT_REPARENT_GONE:
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
	   ("%#08lx EV-%s win=%#lx ev_type=%s(%ld) data: %08lx %08lx %08lx %08lx %08lx\n",
	    ser, name, win, txt, ev->xclient.message_type,
	    ev->xclient.data.l[0], ev->xclient.data.l[1], ev->xclient.data.l[2],
	    ev->xclient.data.l[3], ev->xclient.data.l[4]);
	XFree(txt);
	break;
     case MappingNotify:
      case_common:
	Eprintf("%#08lx EV-%s win=%#lx\n", ser, name, win);
	break;
     case EX_EVENT_SHAPE_NOTIFY:
#define se ((XShapeEvent *)ev)
	Eprintf("%#08lx EV-%s win=%#lx kind=%d shaped=%d %d,%d %dx%d\n",
		ser, name, win, se->kind, se->shaped,
		se->x, se->y, se->width, se->height);
#undef se
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
