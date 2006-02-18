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
#include "e16-ecore_hints.h"
#include "xwin.h"
#include <sys/time.h>

static              Window
ExtInitWinMain(void)
{
   Ecore_X_Window      win;
   XGCValues           gcv;
   GC                  gc;
   Pixmap              pmap, mask;
   Atom                a;
   int                 i;
   XSetWindowAttributes attr;

   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("ExtInitWinMain enter\n");

   disp = EDisplayOpen(NULL, -1);

   EGrabServer();

#if 0
   imlib_set_cache_size(2048 * 1024);
   imlib_set_font_cache_size(512 * 1024);
   imlib_set_color_usage(128);
#endif

   imlib_context_set_display(disp);
   imlib_context_set_visual(DefaultVisual(disp, DefaultScreen(disp)));
   imlib_context_set_colormap(DefaultColormap(disp, DefaultScreen(disp)));

   attr.backing_store = NotUseful;
   attr.override_redirect = True;
   attr.colormap = VRoot.cmap;
   attr.border_pixel = 0;
   attr.background_pixel = 0;
   attr.save_under = True;
   win = XCreateWindow(disp, VRoot.win, 0, 0, VRoot.w, VRoot.h, 0,
		       CopyFromParent, InputOutput, CopyFromParent,
		       CWOverrideRedirect | CWSaveUnder | CWBackingStore |
		       CWColormap | CWBackPixel | CWBorderPixel, &attr);

   pmap = XCreatePixmap(disp, win, VRoot.w, VRoot.h, VRoot.depth);
   gcv.subwindow_mode = IncludeInferiors;
   gc = XCreateGC(disp, win, GCSubwindowMode, &gcv);
   XCopyArea(disp, VRoot.win, pmap, gc, 0, 0, VRoot.w, VRoot.h, 0, 0);
   XSetWindowBackgroundPixmap(disp, win, pmap);
   XMapRaised(disp, win);
   XFreePixmap(disp, pmap);
   XFreeGC(disp, gc);

   a = XInternAtom(disp, "ENLIGHTENMENT_RESTART_SCREEN", False);
   ecore_x_window_prop_window_set(VRoot.win, a, &win, 1);

   XSelectInput(disp, win, StructureNotifyMask);

   EUngrabServer();
   ESync();

   {
      Window              w2, ww;
      char                s[1024];
      Imlib_Image        *im;
      struct timeval      tv;
      int                 dd, x, y;
      unsigned int        mm;
      Cursor              cs = 0;
      XColor              cl;

      w2 = XCreateWindow(disp, win, 0, 0, 32, 32, 0, CopyFromParent,
			 InputOutput, CopyFromParent,
			 CWOverrideRedirect | CWBackingStore | CWColormap |
			 CWBackPixel | CWBorderPixel, &attr);

      pmap = XCreatePixmap(disp, w2, 16, 16, 1);
      gc = XCreateGC(disp, pmap, 0, NULL);
      XSetForeground(disp, gc, 0);
      XFillRectangle(disp, pmap, gc, 0, 0, 16, 16);
      XFreeGC(disp, gc);

      mask = XCreatePixmap(disp, w2, 16, 16, 1);
      gc = XCreateGC(disp, mask, 0, NULL);
      XSetForeground(disp, gc, 0);
      XFillRectangle(disp, mask, gc, 0, 0, 16, 16);
      XFreeGC(disp, gc);

      cs = XCreatePixmapCursor(disp, pmap, mask, &cl, &cl, 0, 0);
      XDefineCursor(disp, win, cs);
      XDefineCursor(disp, w2, cs);

      for (i = 1;; i++)
	{
	   if (i > 12)
	      i = 1;

	   Esnprintf(s, sizeof(s), "pix/wait%i.png", i);
	   if (EventDebug(EDBUG_TYPE_SESSION))
	      Eprintf("ExtInitWinCreate - child %s\n", s);

	   im = ELoadImage(s);
	   if (im)
	     {
		imlib_context_set_image(im);
		imlib_context_set_drawable(w2);
		imlib_render_pixmaps_for_whole_image(&pmap, &mask);
		XShapeCombineMask(disp, w2, ShapeBounding, 0, 0, mask,
				  ShapeSet);
		XSetWindowBackgroundPixmap(disp, w2, pmap);
		imlib_free_pixmap_and_mask(pmap);
		XClearWindow(disp, w2);
		XQueryPointer(disp, win, &ww, &ww, &dd, &dd, &x, &y, &mm);
		XMoveResizeWindow(disp, w2,
				  x - imlib_image_get_width() / 2,
				  y - imlib_image_get_height() / 2,
				  imlib_image_get_width(),
				  imlib_image_get_height());
		XMapWindow(disp, w2);
		imlib_free_image();
	     }
	   tv.tv_sec = 0;
	   tv.tv_usec = 50000;
	   select(0, NULL, NULL, NULL, &tv);
	   ESync();
	}
   }

   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("ExtInitWinMain exit\n");

   EDisplayClose();

   exit(0);
}

Window
ExtInitWinCreate(void)
{
   Ecore_X_Window      win_ex;	/* Hmmm.. */
   Window              win;
   Atom                a;

   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("ExtInitWinCreate\n");

   a = XInternAtom(disp, "ENLIGHTENMENT_RESTART_SCREEN", False);
   ESync();

   if (fork())
     {
	/* Parent */
	EUngrabServer();

	for (;;)
	  {
	     if (EventDebug(EDBUG_TYPE_SESSION))
		Eprintf("ExtInitWinCreate - parent\n");

	     /* Hack to give the child some space. Not foolproof. */
	     sleep(1);

	     if (ecore_x_window_prop_window_get(VRoot.win, a, &win_ex, 1) > 0)
		break;
	  }

	win = win_ex;
	if (EventDebug(EDBUG_TYPE_SESSION))
	   Eprintf("ExtInitWinCreate - parent - %#lx\n", win);

	return win;
     }

   /* Child - Create the init window */

   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("ExtInitWinCreate - child\n");

   /* Clean up inherited stuff */

   SignalsRestore();

   EDisplayDisconnect();
   /* on solairs connection stays up - close */

   ExtInitWinMain();

   /* We will never get here */
   return None;
}

static Window       init_win_ext = None;

void
ExtInitWinSet(Window win)
{
   init_win_ext = win;
}

Window
ExtInitWinGet(void)
{
   return init_win_ext;
}

void
ExtInitWinKill(void)
{
   if (!disp || init_win_ext == None)
      return;

   if (EventDebug(EDBUG_TYPE_SESSION))
      Eprintf("Kill init window %#lx\n", init_win_ext);
   XKillClient(disp, init_win_ext);
   init_win_ext = None;
}
