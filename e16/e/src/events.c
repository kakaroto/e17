/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
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
#ifdef __EMX__
#include <io.h>			/* EMX select() */
#endif

char                throw_move_events_away = 0;

static char         diddeskaccount = 1;

static void
DeskAccountTimeout(int val, void *data)
{
   EDBUG(5, "DeskAccountTimeout");

   DesktopAccounting();
   diddeskaccount = 1;
   EDBUG_RETURN_;
   data = NULL;
   val = 0;

   EDBUG_RETURN_;
}

char               *
NukeBoringevents(XEvent * ev, int num)
{
   char               *ok;
   int                 i, j;
   int /*first, */     last;

   if (!num)
      return NULL;

   ok = Emalloc(num * sizeof(char));

   for (i = 0; i < num; i++)
     {
/*      DebugEvent(&(ev[i])); */
	ok[i] = 1;
     }
   /* get rid of all but the last motion event */
   last = -1;
   for (i = 0; i < num; i++)
     {
	if (ev[i].type == MotionNotify)
	  {
	     ok[i] = 0;
	     last = i;
	  }
     }
   if ((last >= 0) && (!throw_move_events_away))
      ok[last] = 1;
   throw_move_events_away = 0;
   /* compress all shapenotify events for a window and onyl take the last one */
   /* as beign valid */
   for (i = 0; i < num; i++)
     {
	if (ev[i].type == event_base_shape + ShapeNotify)
	  {
	     Window              win;

	     last = i;
	     win = ev[i].xany.window;
	     for (j = i; j < num; j++)
	       {
		  if ((ev[j].type == event_base_shape + ShapeNotify)
		      && (ev[j].xany.window == win))
		    {
		       ok[j] = 0;
		       last = j;
		    }
	       }
	     ok[last] = 1;
	  }
     }
   /* FIXME: add maprequest compression */
   /* FIXME: add configurerequest compression */
   /* FIXME: add resizerequest compression */
   return ok;
}

#if 0
static void
DebugEvent(XEvent * ev)
{
   EDBUG(8, "DebugEvent");
   if (ev->type == event_base_shape + ShapeNotify)
      fprintf(stderr, "EV: ShapeNotify:\n");
   else
     {
	switch (ev->type)
	  {
	  case KeyPress:
	     fprintf(stderr, "EV: KeyPress:\n");
	     break;
	  case KeyRelease:
	     fprintf(stderr, "EV: KeyRelease:\n");
	     break;
	  case ButtonPress:
	     fprintf(stderr, "EV: ButtonPress:\n");
	     break;
	  case ButtonRelease:
	     fprintf(stderr, "EV: ButtonRelease:\n");
	     break;
	  case MotionNotify:
	     fprintf(stderr, "EV: MotionNotify:\n");
	     break;
	  case EnterNotify:
	     fprintf(stderr, "EV: EnterNotify:\n");
	     break;
	  case LeaveNotify:
	     fprintf(stderr, "EV: LeaveNotify:\n");
	     break;
	  case FocusIn:
	     fprintf(stderr, "EV: FocusIn:\n");
	     break;
	  case FocusOut:
	     fprintf(stderr, "EV: FocusOut:\n");
	     break;
	  case KeymapNotify:
	     fprintf(stderr, "EV: KeymapNotify:\n");
	     break;
	  case Expose:
	     fprintf(stderr, "EV: Expose:\n");
	     break;
	  case GraphicsExpose:
	     fprintf(stderr, "EV: GraphicsExpose:\n");
	     break;
	  case NoExpose:
	     fprintf(stderr, "EV: NoExpose:\n");
	     break;
	  case VisibilityNotify:
	     fprintf(stderr, "EV: VisibilityNotify:\n");
	     break;
	  case CreateNotify:
	     fprintf(stderr, "EV: CreateNotify:\n");
	     break;
	  case DestroyNotify:
	     fprintf(stderr, "EV: DestroyNotify:\n");
	     break;
	  case UnmapNotify:
	     fprintf(stderr, "EV: UnmapNotify:\n");
	     break;
	  case MapNotify:
	     fprintf(stderr, "EV: MapNotify:\n");
	     break;
	  case MapRequest:
	     fprintf(stderr, "EV: MapRequest:\n");
	     break;
	  case ReparentNotify:
	     fprintf(stderr, "EV: ReparentNotify:\n");
	     break;
	  case ConfigureNotify:
	     fprintf(stderr, "EV: ConfigureNotify:\n");
	     break;
	  case ConfigureRequest:
	     fprintf(stderr, "EV: ConfigureRequest:\n");
	     break;
	  case GravityNotify:
	     fprintf(stderr, "EV: GravityNotify:\n");
	     break;
	  case ResizeRequest:
	     fprintf(stderr, "EV: ResizeRequest:\n");
	     break;
	  case CirculateNotify:
	     fprintf(stderr, "EV: CirculateNotify:\n");
	     break;
	  case CirculateRequest:
	     fprintf(stderr, "EV: CirculateRequest:\n");
	     break;
	  case PropertyNotify:
	     fprintf(stderr, "EV: PropertyNotify:\n");
	     break;
	  case SelectionClear:
	     fprintf(stderr, "EV: SelectionClear:\n");
	     break;
	  case SelectionRequest:
	     fprintf(stderr, "EV: SelectionRequest:\n");
	     break;
	  case SelectionNotify:
	     fprintf(stderr, "EV: SelectionNotify:\n");
	     break;
	  case ColormapNotify:
	     fprintf(stderr, "EV: ColormapNotify:\n");
	     break;
	  case ClientMessage:
	     fprintf(stderr, "EV: ClientMessage:\n");
	     break;
	  case MappingNotify:
	     fprintf(stderr, "EV: MappingNotify:\n");
	     break;
	  default:
	     fprintf(stderr, "EV: ???\n");
	     break;
	  }
     }
   EDBUG_RETURN_;
}
#endif

static void
HKeyPress(XEvent * ev)
{
   Dialog             *d;

   EDBUG(7, "HKeyPress");
   d = FindDialog(ev->xkey.window);
   if (d)
     {
	int                 i;

	for (i = 0; i < d->num_bindings; i++)
	  {
	     if (ev->xkey.keycode == d->keybindings[i].key)
		(d->keybindings[i].func) (d->keybindings[i].val,
					  d->keybindings[i].data);
	  }
     }
   EDBUG_RETURN_;
}

static void
HKeyRelease(XEvent * ev)
{
   EDBUG(7, "HKeyRelease");
   ev = NULL;
   EDBUG_RETURN_;
}

#if 0
struct _pbuf
{
   int                 w, h, depth;
   Pixmap              id;
   void               *stack[32];
   struct _pbuf       *next;
};
extern struct _pbuf *pbuf = NULL;

#endif

static void
HButtonPress(XEvent * ev)
{
   EDBUG(7, "HButtonPress");
   ApplySclass(FindItem
	       ("SOUND_BUTTON_CLICK", 0, LIST_FINDBY_NAME, LIST_TYPE_SCLASS));
   HandleMouseDown(ev);
#if 0
   {
      int                 x, y, maxh = 0, count = 0, mcount = 0, ww, hh;
      struct _pbuf       *pb;
      GC                  gc;
      XGCValues           gcv;

      gc = XCreateGC(disp, root.win, 0, &gcv);
      XSetForeground(disp, gc, WhitePixel(disp, root.scr));
      fprintf(stderr, "Pixmaps allocated:\n");
      x = 0;
      y = 0;
      XClearWindow(disp, root.win);
      for (pb = pbuf; pb; pb = pb->next)
	{
	   ww = pb->w;
	   hh = pb->h;
	   if (ww > 64)
	      ww = 64;
	   if (hh > 64)
	      hh = 64;
	   if (x + ww > root.w)
	     {
		x = 0;
		y += maxh;
		maxh = 0;
	     }
	   XCopyArea(disp, pb->id, root.win, gc, 0, 0, ww, hh, x, y);
	   XDrawRectangle(disp, root.win, gc, x, y, ww, hh);
	   x += ww;
	   if (hh > maxh)
	      maxh = hh;
	   count++;
	   if (pb->depth == 1)
	      mcount++;
	   fprintf(stderr,
		   "%08x (%5ix%5i %i) : " "%x %x %x %x %x %x %x %x "
		   "%x %x %x %x %x %x %x %x " "%x %x %x %x %x %x %x %x "
		   "%x %x %x %x %x %x %x %x\n", pb->id, pb->w, pb->h,
		   pb->depth, pb->stack[0], pb->stack[1], pb->stack[2],
		   pb->stack[3], pb->stack[4], pb->stack[5], pb->stack[6],
		   pb->stack[7], pb->stack[8], pb->stack[9], pb->stack[10],
		   pb->stack[11], pb->stack[12], pb->stack[13], pb->stack[14],
		   pb->stack[15], pb->stack[16], pb->stack[17], pb->stack[18],
		   pb->stack[19], pb->stack[20], pb->stack[21], pb->stack[22],
		   pb->stack[23], pb->stack[24], pb->stack[25], pb->stack[26],
		   pb->stack[27], pb->stack[28], pb->stack[29], pb->stack[30],
		   pb->stack[31]);
	}
      fprintf(stderr, "Total %i, %i of them bitmaps\n", count, mcount);
      XFreeGC(disp, gc);
   }
#endif
   EDBUG_RETURN_;
}

static void
HButtonRelease(XEvent * ev)
{
   EDBUG(7, "HButtonRelease");
   ApplySclass(FindItem
	       ("SOUND_BUTTON_RAISE", 0, LIST_FINDBY_NAME, LIST_TYPE_SCLASS));
   HandleMouseUp(ev);
   EDBUG_RETURN_;
}

static void
HMotionNotify(XEvent * ev)
{
   EDBUG(7, "HMotionNotify");
   HandleMotion(ev);
   EDBUG_RETURN_;
}

static void
HEnterNotify(XEvent * ev)
{
   EDBUG(7, "HEnterNotify");
   if (mode.mode == MODE_NONE)
     {
	/*
	 * multi screen handling -- root windows receive
	 * enter / leave notify
	 */
	if (ev->xany.window == root.win)
	  {
	     PagerHideAllHi();
	     if (!mode.focuswin || FOCUS_POINTER == mode.focusmode)
		HandleFocusWindow(root.focuswin);
	  }
	else
	  {
	     HandleMouseIn(ev);
	     HandleFocusWindow(ev->xcrossing.window);
	  }
     }
   EDBUG_RETURN_;
}

static void
HLeaveNotify(XEvent * ev)
{
   EDBUG(7, "HLeaveNotify");
   if (mode.mode == MODE_NONE)
     {
	HandleMouseOut(ev);

	/*
	 * If we are leaving the root window, we are switching
	 * screens on a multi screen system - need to unfocus
	 * to allow other desk to grab focus...
	 */
	if (ev->xcrossing.window == root.win)
	  {
	     if (ev->xcrossing.mode == NotifyNormal
		 && ev->xcrossing.detail != NotifyInferior && mode.focuswin)
		HandleFocusWindow(root.focuswin);
	     else
		HandleFocusWindow(ev->xcrossing.window);
	  }
/* THIS caused the "emacs focus bug" ? */
/*      else */
/*      HandleFocusWindow(ev->xcrossing.window); */
     }
   EDBUG_RETURN_;
}

static void
HFocusIn(XEvent * ev)
{
   EDBUG(7, "HFocusIn");
   if (ev->xfocus.detail != NotifyPointer)
      HandleFocusWindowIn(ev->xfocus.window);
   EDBUG_RETURN_;
}

static void
HFocusOut(XEvent * ev)
{
   EDBUG(7, "HFocusOut");
   if (ev->xfocus.detail == NotifyNonlinear)
     {
	Window              rt, ch;
	int                 d;
	unsigned int        ud;

	XQueryPointer(disp, root.win, &rt, &ch, &d, &d, &d, &d, &ud);
	if (rt != root.win)
	  {
/*           fprintf(stderr, "HandleFocusWindowIn\n"); */
	     HandleFocusWindowIn(0);
	  }
     }
   EDBUG_RETURN_;
}

static void
HKeymapNotify(XEvent * ev)
{
   EDBUG(7, "HKeymapNotify");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
HExpose(XEvent * ev)
{
   EDBUG(7, "HExpose");
   HandleExpose(ev);
   EDBUG_RETURN_;
}

static void
HGraphicsExpose(XEvent * ev)
{
   EDBUG(7, "HGraphicsExpose");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
HNoExpose(XEvent * ev)
{
   EDBUG(7, "HNoExpose");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
HVisibilityNotify(XEvent * ev)
{
   EDBUG(7, "HVisibilityNotify");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
HCreateNotify(XEvent * ev)
{
   EDBUG(7, "HCreateNotify");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
HDestroyNotify(XEvent * ev)
{
   EDBUG(7, "HDestroyNotify");
   HandleDestroy(ev);
   EDBUG_RETURN_;
}

static void
HUnmapNotify(XEvent * ev)
{
   EDBUG(7, "HUnmapNotify");
   HandleUnmap(ev);
   EDBUG_RETURN_;
}

static void
HMapNotify(XEvent * ev)
{
   EDBUG(7, "HMapNotify");
   HandleMap(ev);
   EDBUG_RETURN_;
}

static void
HMapRequest(XEvent * ev)
{
   EDBUG(7, "HMapRequest");
   HandleMapRequest(ev);
   EDBUG_RETURN_;
}

static void
HReparentNotify(XEvent * ev)
{
   EDBUG(7, "HReparentNotify");
   HandleReparent(ev);
   EDBUG_RETURN_;
}

static void
HConfigureNotify(XEvent * ev)
{
   EDBUG(7, "HConfigureNotify");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
HConfigureRequest(XEvent * ev)
{
   EDBUG(7, "HConfigureRequest");
   HandleConfigureRequest(ev);
   EDBUG_RETURN_;
}

static void
HGravityNotify(XEvent * ev)
{
   EDBUG(7, "HGravityNotify");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
HResizeRequest(XEvent * ev)
{
   EDBUG(7, "HResizeRequest");
   HandleResizeRequest(ev);
   EDBUG_RETURN_;
}

static void
HCirculateNotify(XEvent * ev)
{
   EDBUG(7, "HCirculateNotify");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
HCirculateRequest(XEvent * ev)
{
   EDBUG(7, "HCirculateRequest");
   HandleCirculate(ev);
   EDBUG_RETURN_;
}

static void
HPropertyNotify(XEvent * ev)
{
   EDBUG(7, "HPropertyNotify");
   HandleProperty(ev);
   EDBUG_RETURN_;
}

static void
HSelectionClear(XEvent * ev)
{
   EDBUG(7, "HSelectionClear");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
HSelectionRequest(XEvent * ev)
{
   EDBUG(7, "HSelectionRequest");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
HSelectionNotify(XEvent * ev)
{
   EDBUG(7, "HSelectionNotify");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
HColormapNotify(XEvent * ev)
{
   EDBUG(7, "HColormapNotify");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
HClientMessage(XEvent * ev)
{
   EDBUG(7, "HClientMessage");
   HandleClientMessage(ev);
   EDBUG_RETURN_;
}

static void
HMappingNotify(XEvent * ev)
{
   EDBUG(7, "HMappingNotify");
   ev = NULL;
   EDBUG_RETURN_;
}

static void
DefaultFunc(XEvent * ev)
{
   EDBUG(7, "DefaultFunc");
   ev = NULL;
   EDBUG_RETURN_;
}

static HandleStruct HArray[] = {
   {DefaultFunc},
   {DefaultFunc},
   {HKeyPress},
   {HKeyRelease},
   {HButtonPress},
   {HButtonRelease},
   {HMotionNotify},
   {HEnterNotify},
   {HLeaveNotify},
   {HFocusIn},
   {HFocusOut},
   {HKeymapNotify},
   {HExpose},
   {HGraphicsExpose},
   {HNoExpose},
   {HVisibilityNotify},
   {HCreateNotify},
   {HDestroyNotify},
   {HUnmapNotify},
   {HMapNotify},
   {HMapRequest},
   {HReparentNotify},
   {HConfigureNotify},
   {HConfigureRequest},
   {HGravityNotify},
   {HResizeRequest},
   {HCirculateNotify},
   {HCirculateRequest},
   {HPropertyNotify},
   {HSelectionClear},
   {HSelectionRequest},
   {HSelectionNotify},
   {HColormapNotify},
   {HClientMessage},
   {HMappingNotify},
   {DefaultFunc}
};

static void
HandleEvent(XEvent * ev)
{
   void              **lst;
   int                 i, num;

   EDBUG(7, "HandleEvent");

   if (ev->type == event_base_shape + ShapeNotify)
      HandleChildShapeChange(ev);

   if ((ev->type == KeyPress) || (ev->type == KeyRelease)
       || (ev->type == ButtonPress) || (ev->type == ButtonRelease)
       || (ev->type == EnterNotify) || (ev->type == LeaveNotify))
     {
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
	     WarpFocusHandleEvent(ev);
	     lst = ListItemType(&num, LIST_TYPE_ACLASS_GLOBAL);
	     if (lst)
	       {
		  for (i = 0; i < num; i++)
		     EventAclass(ev, (ActionClass *) lst[i]);
		  Efree(lst);
	       }
	  }
     }

   if (ev->type <= 35)
      HArray[ev->type].func(ev);

   IconboxHandleEvent(ev);

   if (diddeskaccount)
     {
	DoIn("DESKTOP_ACCOUNTING_TIMEOUT", 30.0, DeskAccountTimeout, 0, NULL);
	diddeskaccount = 0;
     }

   EDBUG_RETURN_;
}

void
CheckEvent()
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

  /* This is the primary event loop.  Everything that is going to happen in the
   * window manager has to start here at some point.  This is where all the
   * events from the X server are interpreted, timer events are inserted, etc
   */

void
WaitEvent()
{
/*  XEvent              ev; */
   fd_set              fdset;
   struct timeval      tval;
   static struct timeval tval_last = { 0, 0 };
   double              time1, time2;
   Qentry             *qe;
   int                 count, pcount;
   int                 fdsize;
   int                 xfd, smfd;
   int                 i;
   static int          evq_num = 0;
   static XEvent      *evq = NULL;
   char               *ok;

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

   count = 0;
   while (XPending(disp))
     {
	count++;
	if (count > evq_num)
	  {
	     evq_num += 16;
	     if (!evq)
		evq = Emalloc(sizeof(XEvent) * evq_num);
	     else
		evq = Erealloc(evq, sizeof(XEvent) * evq_num);
	  }
	XNextEvent(disp, &(evq[count - 1]));
     }
   /* remove multiple extraneous events here */
   ok = NukeBoringevents(evq, count);
   if (ok)
     {
	for (i = 0; i < count; i++)
	  {
	     if (ok[i])
		HandleEvent(&(evq[i]));
	  }
	Efree(ok);
     }

   DBUG_STACKCHECK;

   HandleDrawQueue();
   XFlush(disp);
   pcount = count;

   DBUG_STACKCHECK;

   count = 0;
   while (XPending(disp))
     {
	count++;
	if (count > evq_num)
	  {
	     evq_num += 16;
	     if (!evq)
		evq = Emalloc(sizeof(XEvent) * evq_num);
	     else
		evq = Erealloc(evq, sizeof(XEvent) * evq_num);
	  }
	XNextEvent(disp, &(evq[count - 1]));
     }
   /* remove multiple extraneous events here */
   ok = NukeBoringevents(evq, count);
   if (ok)
     {
	for (i = 0; i < count; i++)
	  {
	     if (ok[i])
		HandleEvent(&(evq[i]));
	  }
	Efree(ok);
     }
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
