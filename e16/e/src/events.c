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
#include "E.h"
#include <errno.h>
#include <sys/time.h>
#ifdef USE_XRANDR
#include <X11/extensions/Xrandr.h>
#endif

#if ENABLE_DEBUG_EVENTS
static const char  *EventName(unsigned int type);
#endif

static int          event_base_shape = 0;
static int          error_base_shape = 0;

#ifdef USE_XRANDR
static int          event_base_randr = 0;
static int          error_base_randr = 0;
#endif

char                throw_move_events_away = 0;

void
EventsInit(void)
{
   int                 major, minor;

   /* Check for the Shape Extension */
   if (XShapeQueryExtension(disp, &event_base_shape, &error_base_shape))
     {
	XShapeQueryVersion(disp, &major, &minor);
	if (Mode.debug)
	   Eprintf("Found extension Shape version %d.%d\n"
		   " Event/error base = %d/%d\n",
		   major, minor, event_base_shape, error_base_shape);
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
	EExit((void *)1);
     }

#ifdef USE_XRANDR
   if (XRRQueryExtension(disp, &event_base_randr, &error_base_randr))
     {
	XRRQueryVersion(disp, &major, &minor);
	if (Mode.debug)
	   Eprintf("Found extension RandR version %d.%d\n"
		   " Event/error base = %d/%d\n",
		   major, minor, event_base_randr, error_base_randr);

	/* Listen for RandR events */
	XRRSelectInput(disp, root.win, RRScreenChangeNotifyMask);
     }
#endif
}

static void
HandleEvent(XEvent * ev)
{
   void              **lst;
   int                 i, num;

   EDBUG(7, "HandleEvent");

#if ENABLE_DEBUG_EVENTS
   if (EventDebug(ev->type))
      EventShow(ev);
#endif
   Mode.current_event = ev;

   switch (ev->type)
     {
     case KeyPress:
     case KeyRelease:
     case ButtonPress:
     case ButtonRelease:
     case EnterNotify:
     case LeaveNotify:
	if (((ev->type == KeyPress) || (ev->type == KeyRelease))
	    && (ev->xkey.root != root.win))
	  {
	     XSetInputFocus(disp, ev->xkey.root, RevertToPointerRoot,
			    CurrentTime);
	     XSync(disp, False);
	     ev->xkey.time = CurrentTime;
	     XSendEvent(disp, ev->xkey.root, False, 0, ev);
	  }
	else
	  {
	     if (ev->type == KeyPress)
		PagerHideAllHi();
	     WarpFocusHandleEvent(ev);
	     lst = ListItemType(&num, LIST_TYPE_ACLASS_GLOBAL);
	     if (lst)
	       {
		  for (i = 0; i < num; i++)
		     EventAclass(ev, NULL, (ActionClass *) lst[i]);
		  Efree(lst);
	       }
	  }
	break;
     }

   switch (ev->type)
     {
     case KeyPress:		/*  2 */
	DialogEventKeyPress(ev);
	break;
     case KeyRelease:		/*  3 */
	break;
     case ButtonPress:		/*  4 */
	SoundPlay("SOUND_BUTTON_CLICK");
	HandleMouseDown(ev);
	break;
     case ButtonRelease:	/*  5 */
	SoundPlay("SOUND_BUTTON_RAISE");
	HandleMouseUp(ev);
	break;
     case MotionNotify:	/*  6 */
	HandleMotion(ev);
	break;
     case EnterNotify:		/*  7 */
	HandleMouseIn(ev);
	break;
     case LeaveNotify:		/*  8 */
	HandleMouseOut(ev);
	break;
     case FocusIn:		/*  9 */
	HandleFocusIn(ev);
	break;
     case FocusOut:		/* 10 */
	HandleFocusOut(ev);
	break;
     case KeymapNotify:	/* 11 */
	break;
     case Expose:		/* 12 */
	HandleExpose(ev);
	break;
     case GraphicsExpose:	/* 13 */
	break;
     case NoExpose:		/* 14 */
	break;
     case VisibilityNotify:	/* 15 */
	break;
     case CreateNotify:	/* 16 */
	break;
     case DestroyNotify:	/* 17 */
	HandleDestroy(ev);
	break;
     case UnmapNotify:		/* 18 */
	HandleUnmap(ev);
	break;
     case MapNotify:		/* 19 */
	HandleMap(ev);
	break;
     case MapRequest:		/* 20 */
	HandleMapRequest(ev);
	break;
     case ReparentNotify:	/* 21 */
	HandleReparent(ev);
	break;
     case ConfigureNotify:	/* 22 */
	HandleConfigureNotify(ev);
	break;
     case ConfigureRequest:	/* 23 */
	HandleConfigureRequest(ev);
	break;
     case GravityNotify:	/* 24 */
	break;
     case ResizeRequest:	/* 25 */
	HandleResizeRequest(ev);
	break;
     case CirculateNotify:	/* 26 */
	break;
     case CirculateRequest:	/* 27 */
	HandleCirculateRequest(ev);
	break;
     case PropertyNotify:	/* 28 */
	HandleProperty(ev);
	break;
     case SelectionClear:	/* 29 */
	break;
     case SelectionRequest:	/* 30 */
	break;
     case SelectionNotify:	/* 31 */
	break;
     case ColormapNotify:	/* 32 */
	break;
     case ClientMessage:	/* 33 */
	HandleClientMessage(ev);
	break;
     case MappingNotify:	/* 34 */
	break;
     default:
	if (ev->type == event_base_shape + ShapeNotify)
	   HandleChildShapeChange(ev);
#ifdef USE_XRANDR
	else if (ev->type == event_base_randr + RRScreenChangeNotify)
	   HandleScreenChange(ev);
#endif
	break;
     }

   /* Should not be "global" */
   IconboxesHandleEvent(ev);

   EDBUG_RETURN_;
}

void
CheckEvent(void)
{
   XEvent              ev;

   EDBUG(7, "CheckEvent");
   while (XPending(disp))
     {
	XNextEvent(disp, &ev);
	HandleEvent(&ev);
     }
   EDBUG_RETURN_;
}

static void
EventsCompress(XEvent * evq, int count)
{
   XEvent             *ev, *ev2;
   int                 i, j, n;
   int                 xa, ya, xb, yb;

   /* Debug - should be taken out */
   if (EventDebug(EDBUG_TYPE_COMPRESSION))
      for (i = 0; i < count; i++)
	 Eprintf("EventsCompress-1 %3d t=%s w=%#lx\n", i,
		 EventName(evq[i].type), evq[i].xany.window);

   /* Loop through event list, starting with latest */
   for (i = count - 1; i > 0; i--)
     {
	ev = evq + i;

	switch (ev->type)
	  {
	  case 0:
	     /* Already thrown away */
	     break;

	  case MotionNotify:
	     if (throw_move_events_away)	/* Discard all motion events */
		j = i;
	     else		/* Discard all but last motion event */
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
	     if (n && EventDebug(EDBUG_TYPE_COMPRESSION))
		Eprintf("EventsCompress MotionNotify %#lx n=%4d x,y = %d,%d\n",
			ev->xmotion.window, n, ev->xmotion.x, ev->xmotion.y);
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
	     if (EventDebug(EDBUG_TYPE_COMPRESSION))
		Eprintf
		   ("EventsCompress Expose %#lx n=%4d x=%4d-%4d y=%4d-%4d\n",
		    ev->xexpose.window, n, xa, xb, ya, yb);
	     break;

	  default:
	     if (ev->type == event_base_shape + ShapeNotify)
	       {
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
		  if (n && EventDebug(EDBUG_TYPE_COMPRESSION))
		     Eprintf("EventsCompress ShapeNotify %#lx n=%4d\n",
			     ev->xmotion.window, n);
	       }
	     break;
	  }
     }

   /* Debug - should be taken out */
   if (EventDebug(EDBUG_TYPE_COMPRESSION))
      for (i = 0; i < count; i++)
	 Eprintf("EventsCompress-2 %3d t=%s w=%#lx\n", i,
		 EventName(evq[i].type), evq[i].xany.window);
}

static int
EventsProcess(XEvent ** evq_ptr, int *evq_siz)
{
   int                 i, n, count;
   XEvent             *evq = *evq_ptr;
   int                 qsz = *evq_siz;

   /* Fetch the entire event queue */
   for (i = count = 0; (n = XPending(disp)) > 0;)
     {
	count += n;
	if (count > qsz)
	  {
	     qsz = count;
	     evq = Erealloc(evq, sizeof(XEvent) * qsz);
	  }
	for (; i < count; i++)
	   XNextEvent(disp, evq + i);
     }

   EventsCompress(evq, count);

   for (i = 0; i < count; i++)
     {
	if (evq[i].type)
	   HandleEvent(evq + i);
     }

   *evq_ptr = evq;
   *evq_siz = qsz;

   return count;
}

#ifdef DEBUG
#define DBUG_STACKSTART \
  int save = call_level + 1;
#define DBUG_STACKCHECK \
  if (save != call_level) { \
    fprintf (stderr, "Unstack error: ["); \
    for (save = 0; save < 4; ++ save) \
      fprintf (stderr, "%s%s", save ? ", " : "", call_stack[save]); \
    fprintf (stderr, "]\n"); \
    save = call_level; \
  }
#else
#define DBUG_STACKSTART
#define DBUG_STACKCHECK
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
   static int          evq_num = 0;
   static XEvent      *evq = NULL;

   DBUG_STACKSTART;

   EDBUG(7, "WaitEvent");
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

   count = EventsProcess(&evq, &evq_num);

   DBUG_STACKCHECK;

   HandleDrawQueue();
   XFlush(disp);
   pcount = count;

   DBUG_STACKCHECK;

   count = EventsProcess(&evq, &evq_num);

   if (count > 0)
      XFlush(disp);

   if (pcount > count)
      count = pcount;
   if ((evq) && ((evq_num - count) > 64))
     {
	evq_num = 0;
	Efree(evq);
	evq = NULL;
     }

   DBUG_STACKCHECK;

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
     {
	EDBUG_RETURN_;
     }
   if ((smfd >= 0) && (count > 0) && (FD_ISSET(smfd, &fdset)))
      ProcessICEMSGS();

   DBUG_STACKCHECK;

   if ((!(FD_ISSET(xfd, &fdset))) && (qe) && (count == 0)
       && (((smfd >= 0) && (!(FD_ISSET(smfd, &fdset)))) || (smfd < 0)))
      HandleTimerEvent();

   DBUG_STACKCHECK;

   EDBUG_RETURN_;
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
   if (type < N_EVENT_NAMES)
      return TxtEventNames[type];

   return "Unknown";
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
	Eprintf("EV-%s win=%#lx state=%#x button=%#x\n", name, win,
		ev->xbutton.state, ev->xbutton.button);
	break;
     case MotionNotify:
	goto case_common;
     case EnterNotify:
     case LeaveNotify:
	Eprintf("EV-%s win=%#lx m=%s d=%s\n", name, win,
		EventNotifyModeName(ev->xcrossing.mode),
		EventNotifyDetailName(ev->xcrossing.detail));
	break;
     case FocusIn:
     case FocusOut:
	Eprintf("EV-%s win=%#lx m=%s d=%s\n", name, win,
		EventNotifyModeName(ev->xfocus.mode),
		EventNotifyDetailName(ev->xfocus.detail));
	break;
     case KeymapNotify:
     case Expose:
     case GraphicsExpose:
     case NoExpose:
     case VisibilityNotify:
	goto case_common;
     case CreateNotify:
     case DestroyNotify:
     case UnmapNotify:
     case MapNotify:
     case MapRequest:
	Eprintf("EV-%s ev=%#lx win=%#lx\n", name, win,
		ev->xcreatewindow.window);
	break;
     case ReparentNotify:
	goto case_common;
     case ConfigureNotify:
	Eprintf("EV-%s: win=%#lx event=%#lx %d+%d %dx%d bw=%d above=%#lx\n",
		name, ev->xconfigure.window, win,
		ev->xconfigure.x, ev->xconfigure.y,
		ev->xconfigure.width, ev->xconfigure.height,
		ev->xconfigure.border_width, ev->xconfigure.above);
	break;
     case ConfigureRequest:
	Eprintf
	   ("EV-%s: win=%#lx parent=%#lx m=%#lx %d+%d %dx%d bw=%d above=%#lx stk=%d\n",
	    name, ev->xconfigurerequest.window, win,
	    ev->xconfigurerequest.value_mask, ev->xconfigurerequest.x,
	    ev->xconfigurerequest.y, ev->xconfigurerequest.width,
	    ev->xconfigurerequest.height, ev->xconfigurerequest.border_width,
	    ev->xconfigurerequest.above, ev->xconfigurerequest.detail);
	break;
     case GravityNotify:
	goto case_common;
     case ResizeRequest:
	Eprintf("EV-%s: win=%#lx %dx%d\n",
		name, win, ev->xresizerequest.width, ev->xresizerequest.height);
	break;
     case CirculateNotify:
     case CirculateRequest:
	goto case_common;
     case PropertyNotify:
	txt = XGetAtomName(disp, ev->xproperty.atom);
	Eprintf("EV-%s: win=%#lx Atom=%s(%ld)\n",
		name, win, txt, ev->xproperty.atom);
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
	   ("EV-%s win=%#lx ev_type=%s(%ld) data[0-3]= %08lx %08lx %08lx %08lx\n",
	    name, win, txt, ev->xclient.message_type, ev->xclient.data.l[0],
	    ev->xclient.data.l[1], ev->xclient.data.l[2],
	    ev->xclient.data.l[3]);
	XFree(txt);
	break;
     case MappingNotify:
      case_common:
	Eprintf("EV-%s win=%#lx\n", name, win);
	break;
     default:
	if (ev->type == event_base_shape + ShapeNotify)
	   Eprintf("EV-ShapeNotify win=%#lx\n", win);
#ifdef USE_XRANDR
	else if (ev->type == event_base_randr + RRScreenChangeNotify)
	   Eprintf("EV-RRScreenChangeNotify win=%#lx\n", win);
#endif
	else
	   Eprintf("EV-??? Type=%d win=%#lx\n", ev->type, win);
	break;
     }
}

#else

void
EventDebugInit(const char *param)
{
}

#endif /* ENABLE_DEBUG_EVENTS */
