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
#ifdef __EMX__
#include <io.h>			/* for EMX select() */
#endif
#ifdef HAVE_LIBXTST
#include <X11/extensions/XTest.h>
#endif

static int          update_time = 10;
static int          hot_x = 0, hot_y = 0;
static int          sc_x = 0, sc_y = 0;
static char         wait_mode = 0;
static Display     *sc_disp = NULL;
static Window       sc_window = 0;
static Atom         sc_atom = 0;
static ImlibData   *imd = NULL;

Window
SC_GetDestWin(void)
{
   return 0;
}

void
SC_PassEvent(XEvent * ev)
{
   ev = NULL;
}

void
SC_Main(void)
{
   XEvent              ev;
   struct timeval      tv;
   int                 wait_count = 1;

   for (;;)
     {
	if (XCheckWindowEvent
	    (sc_disp, sc_window,
	     PropertyChangeMask | PointerMotionMask | ButtonPressMask |
	     ButtonReleaseMask, &ev))
	  {
	     switch (ev.type)
	       {
	       case KeyPress:
		  printf("EV: KeyPress:\n");
		  break;
	       case KeyRelease:
		  printf("EV: KeyRelease:\n");
		  break;
	       case ButtonPress:
		  printf("EV: ButtonPress:\n");
		  break;
	       case ButtonRelease:
		  printf("EV: ButtonRelease:\n");
		  break;
	       case MotionNotify:
		  printf("EV: MotionNotify:\n");
		  break;
	       case PropertyNotify:
		  {
		     static Atom         a_wait = 0;
		     static Atom         a_hotspot = 0;
		     int                 format_ret;
		     unsigned long       bytes_after, num_ret;
		     Atom                aa;
		     CARD32             *retval;

		     if (!a_wait)
			a_wait = XInternAtom(sc_disp, "ENL_SC_WAIT", False);
		     if (!a_hotspot)
			a_hotspot =
			   XInternAtom(sc_disp, "ENL_SC_HOTSPOT", False);
		     if (ev.xproperty.atom == a_wait)
		       {
			  retval = NULL;
			  XGetWindowProperty(sc_disp, sc_window,
					     ev.xproperty.atom, 0, 0x7fffffff,
					     False, XA_CARDINAL, &aa,
					     &format_ret, &num_ret,
					     &bytes_after,
					     (unsigned char **)(&retval));
			  if (retval)
			    {
			       wait_mode = (char)(*retval);
			       XFree(retval);
			    }
		       }
		     else if (ev.xproperty.atom == a_hotspot)
		       {
			  retval = NULL;
			  XGetWindowProperty(sc_disp, sc_window,
					     ev.xproperty.atom, 0, 0x7fffffff,
					     False, XA_CARDINAL, &aa,
					     &format_ret, &num_ret,
					     &bytes_after,
					     (unsigned char **)(&retval));
			  if (retval)
			    {
			       hot_x = retval[0];
			       hot_y = retval[1];
			       XFree(retval);
			    }
		       }
		  }
	       default:
		  printf("EV: ???\n");
		  break;
	       }
	  }
	if (wait_mode)
	  {
	     char                s[1024], *f;
	     ImlibImage         *im;
	     Pixmap              pmap, mask;

	     Esnprintf(s, sizeof(s), "pix/wait%i.png", wait_count);
	     wait_count++;
	     if (wait_count > 12)
		wait_count = 1;
	     f = FindFile(s);
	     im = NULL;
	     if (f)
		im = Imlib_load_image(imd, f);
	     if (f)
		Efree(f);
	     if (im)
	       {
		  Imlib_render(imd, im, im->rgb_width, im->rgb_height);
		  pmap = Imlib_move_image(imd, im);
		  mask = Imlib_move_mask(imd, im);
		  EShapeCombineMask(sc_disp, sc_window, ShapeBounding, 0, 0,
				    mask, ShapeSet);
		  ESetWindowBackgroundPixmap(sc_disp, sc_window, pmap);
		  Imlib_free_pixmap(imd, pmap);
		  XClearWindow(sc_disp, sc_window);
		  EResizeWindow(sc_disp, sc_window, im->rgb_width,
				im->rgb_height);
		  Imlib_destroy_image(imd, im);
	       }
	  }
	{
	   Window              dw;
	   int                 dd, x, y;
	   unsigned int        mm;

	   XQueryPointer(sc_disp, root.win, &dw, &dw, &dd, &dd, &x, &y, &mm);
	   EMoveWindow(sc_disp, sc_window, x - hot_x, y - hot_y);
	   EMapRaised(sc_disp, sc_window);
	   sc_x = x;
	   sc_y = y;
	}
	tv.tv_sec = 0;
	tv.tv_usec = 1000000 / update_time;
	select(0, NULL, NULL, NULL, &tv);
     }
}

/* routine to set up the software cursor */

void
SC_Init(void)
{
   Atom                aa;
   CARD32              val;
   int                 format_ret;
   unsigned long       bytes_after, num_ret;
   XSetWindowAttributes attr;
   Window             *retval = NULL;
   Pixmap              pmap, mask;
   Cursor              cs;
   XColor              cl;
   GC                  gc;
   XGCValues           gcv;

   sc_atom = XInternAtom(disp, "ENLIGHTENMENT_SOFT_CURSOR", False);
   XSync(disp, False);
   if (fork())
     {
	UngrabX();
	for (;;)
	  {
	     retval = NULL;
	     XGetWindowProperty(disp, root.win, sc_atom, 0, 0x7fffffff, True,
				XA_CARDINAL, &aa, &format_ret, &num_ret,
				&bytes_after, (unsigned char **)(&retval));
	     if (retval)
		break;
	  }
	sc_window = *retval;
	fflush(stdout);
	XFree(retval);
	return;
     }
   XSetErrorHandler((XErrorHandler) NULL);
   XSetIOErrorHandler((XIOErrorHandler) NULL);
   signal(SIGHUP, SIG_DFL);
   signal(SIGINT, SIG_DFL);
   signal(SIGQUIT, SIG_DFL);
   signal(SIGILL, SIG_DFL);
   signal(SIGABRT, SIG_DFL);
   signal(SIGFPE, SIG_IGN);
   signal(SIGSEGV, SIG_IGN);
   signal(SIGPIPE, SIG_DFL);
   signal(SIGALRM, SIG_DFL);
   signal(SIGTERM, SIG_DFL);
   signal(SIGUSR1, SIG_DFL);
   signal(SIGUSR2, SIG_DFL);
   signal(SIGCHLD, SIG_DFL);
#ifdef SIGTSTP
   signal(SIGTSTP, SIG_DFL);
#endif
   signal(SIGBUS, SIG_IGN);
   sc_disp = XOpenDisplay(DisplayString(disp));
   XGrabServer(sc_disp);
#ifdef HAVE_LIBXTST
   {
      int                 test_event_base, test_error_base, test_v1, test_v2;

      if (XTestQueryExtension
	  (sc_disp, &test_event_base, &test_error_base, &test_v1, &test_v2))
	 XTestGrabControl(sc_disp, True);
   }
#endif
   printf("sc going\n");
   imd = Imlib_init(sc_disp);
   attr.backing_store = NotUseful;
   attr.override_redirect = True;
   attr.colormap = root.cmap;
   attr.border_pixel = 0;
   attr.background_pixel = 0;
   attr.save_under = True;
   sc_window =
      XCreateWindow(sc_disp, root.win, 0, 0, 32, 32, 0, root.depth,
		    InputOutput, root.vis,
		    CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		    CWColormap | CWBackPixel | CWBorderPixel, &attr);
   XSelectInput(sc_disp, sc_window,
		PropertyChangeMask | PointerMotionMask | ButtonPressMask |
		ButtonReleaseMask);
   sc_atom = XInternAtom(disp, "ENLIGHTENMENT_SOFT_CURSOR", False);
   val = sc_window;
   XChangeProperty(sc_disp, root.win, sc_atom, XA_CARDINAL, 32,
		   PropModeReplace, (unsigned char *)&val, 1);
   pmap = ECreatePixmap(sc_disp, sc_window, 16, 16, 1);
   mask = ECreatePixmap(sc_disp, sc_window, 16, 16, 1);
   gc = XCreateGC(sc_disp, pmap, 0, &gcv);
   XSetForeground(sc_disp, gc, 0);
   XFillRectangle(sc_disp, pmap, gc, 0, 0, 16, 16);
   XFillRectangle(sc_disp, mask, gc, 0, 0, 16, 16);
   XFreeGC(sc_disp, gc);

   cs = XCreatePixmapCursor(sc_disp, pmap, mask, &cl, &cl, 0, 0);
   XDefineCursor(sc_disp, sc_window, cs);

   EMapRaised(sc_disp, sc_window);

   XGrabPointer(sc_disp, sc_window, True,
		ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
		ButtonMotionMask, GrabModeAsync, GrabModeAsync, None, cs,
		CurrentTime);
   XUngrabServer(sc_disp);
   SC_Main();
   exit(0);
}

/* routines for E main to call to communicate to software cursor */

void
SC_SetWait(void)
{
   static Atom         a = 0;
   CARD32              val;

   if (!a)
      a = XInternAtom(disp, "ENL_SC_WAIT", False);
   val = 1;
   XChangeProperty(disp, sc_window, a, XA_CARDINAL, 32, PropModeReplace,
		   (unsigned char *)&val, 1);
}

void
SC_Normal(void)
{
   static Atom         a = 0;
   CARD32              val;

   if (!a)
      a = XInternAtom(disp, "ENL_SC_WAIT", False);
   val = 0;
   XChangeProperty(disp, sc_window, a, XA_CARDINAL, 32, PropModeReplace,
		   (unsigned char *)&val, 1);
}

void
SC_SetHotspot(int x, int y)
{
   static Atom         a = 0;
   CARD32              val[2];

   if (!a)
      a = XInternAtom(disp, "ENL_SC_HOTSPOT", False);
   val[0] = x;
   val[1] = y;
   XChangeProperty(disp, sc_window, a, XA_CARDINAL, 32, PropModeReplace,
		   (unsigned char *)&val, 2);
}

void
SC_SetImage(char *file)
{
   ImlibImage         *im;
   Pixmap              pmap, mask;

   im = ELoadImage(file);
   if (im)
     {
	Imlib_render(pImlibData, im, im->rgb_width, im->rgb_height);
	pmap = Imlib_move_image(pImlibData, im);
	mask = Imlib_move_mask(pImlibData, im);
	if (pmap)
	  {
	     ESetWindowBackgroundPixmap(disp, sc_window, pmap);
	     EShapeCombineMask(disp, sc_window, ShapeBounding, 0, 0, mask,
			       ShapeSet);
	  }
	Imlib_free_pixmap(pImlibData, pmap);
	Imlib_destroy_image(pImlibData, im);
     }
}

void
SC_Kill(void)
{
   if (sc_window)
     {
	XKillClient(disp, sc_window);
	XSync(disp, False);
	sc_window = 0;
     }
}
