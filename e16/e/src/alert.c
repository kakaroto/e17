
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

static void         ShowAlert(char *text);

static int          (*IgnoreFunction) (const void *) = NULL;
static const void  *IgnoreParams = NULL;
static char        *IgnoreText = NULL;
static int          (*RestartFunction) (const void *) = NULL;
static const void  *RestartParams = NULL;
static char        *RestartText = NULL;
static int          (*ExitFunction) (const void *) = NULL;
static const void  *ExitParams = NULL;
static char        *ExitText = NULL;

static char        *TitleText = NULL;

void
AlertInit(void)
{
   /* Set up all the text bits that belong on the GSOD */
   AssignTitleText(_("Enlightenment Message Dialog"));
   AssignIgnoreText(_("Ignore this"));
   AssignRestartText(_("Restart Enlightenment"));
   AssignExitText(_("Quit Enlightenment"));

   /* We'll set up what the buttons do now, too */
   AssignRestartFunction(SessionExit, "restart");
   AssignExitFunction(SessionExit, NULL);
}

void
AlertX(const char *title, const char *ignore,
       const char *restart, const char *quit, const char *fmt, ...)
{
   char                text[10240];
   va_list             ap;

   EDBUG(7, "AlertX");

   AssignTitleText(title);
   AssignIgnoreText(ignore);
   AssignRestartText(restart);
   AssignExitText(quit);

   va_start(ap, fmt);
   Evsnprintf(text, 10240, fmt, ap);
   va_end(ap);
   SoundPlay("SOUND_ALERT");
   ShowAlert(text);

   AssignTitleText(_("Enlightenment Message Dialog"));
   AssignIgnoreText(_("Ignore this"));
   AssignRestartText(_("Restart Enlightenment"));
   AssignExitText(_("Quit Enlightenment"));

   EDBUG_RETURN_;
}

void
Alert(const char *fmt, ...)
{
   char                text[10240];
   va_list             ap;

   EDBUG(7, "Alert");

   va_start(ap, fmt);
   Evsnprintf(text, 10240, fmt, ap);
   va_end(ap);
   SoundPlay("SOUND_ALERT");
   ShowAlert(text);

   EDBUG_RETURN_;
}

void
AssignTitleText(const char *text)
{
   EDBUG(7, "AssignTitleText");

   if (TitleText)
      Efree(TitleText);
   TitleText = Estrdup(text);

   EDBUG_RETURN_;
}

void
AssignIgnoreText(const char *text)
{
   EDBUG(7, "AssignIgnoreText");

   if (IgnoreText)
      Efree(IgnoreText);
   IgnoreText = Emalloc(strlen(text) + 6);
   sprintf(IgnoreText, "(F1) %s", text);

   EDBUG_RETURN_;
}

void
AssignRestartText(const char *text)
{
   EDBUG(7, "AssignRestartText");

   if (RestartText)
      Efree(RestartText);
   RestartText = Emalloc(strlen(text) + 6);
   sprintf(RestartText, "(F2) %s", text);

   EDBUG_RETURN_;
}

void
AssignExitText(const char *text)
{
   EDBUG(7, "AssignExitText");

   if (ExitText)
      Efree(ExitText);
   ExitText = Emalloc(strlen(text) + 6);
   sprintf(ExitText, "(F3) %s", text);

   EDBUG_RETURN_;
}

void
AssignIgnoreFunction(int (*FunctionToAssign) (const void *), const void *params)
{
   EDBUG(7, "AssignIgnoreFunction");
   IgnoreFunction = FunctionToAssign;
   IgnoreParams = params;
   EDBUG_RETURN_;
}

void
AssignRestartFunction(int (*FunctionToAssign) (const void *),
		      const void *params)
{
   EDBUG(7, "AssignRestartFunction");
   RestartFunction = FunctionToAssign;
   RestartParams = params;
   EDBUG_RETURN_;
}

void
AssignExitFunction(int (*FunctionToAssign) (const void *), const void *params)
{
   EDBUG(7, "AssignExitFunction");
   ExitFunction = FunctionToAssign;
   ExitParams = params;
   EDBUG_RETURN_;
}

#define DRAW_BOX_OUT(mdd, mgc, mwin, mx, my, mw, mh) \
        AlertDrawBox(mdd, mgc, mwin, mx, my, mw, mh, \
        colorful, cols[0], cols[2], cols[3])
#define DRAW_BOX_IN(mdd, mgc, mwin, mx, my, mw, mh) \
        AlertDrawBox(mdd, mgc, mwin, mx, my, mw, mh, \
        colorful, cols[2], cols[0], cols[3])
static void
AlertDrawBox(Display * mdd, GC mgc, Window mwin, int mx, int my, int mw, int mh,
	     int colorful, unsigned long c1, unsigned long c2, unsigned long cb)
{
   if (colorful)
     {
	XSetForeground(mdd, mgc, cb);
	XDrawRectangle(mdd, mwin, mgc, mx, my, mw - 1, mh - 1);
	XSetForeground(mdd, mgc, c1);
	XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + mw - 3, my + 1);
	XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + 1, my + mh - 3);
	XSetForeground(mdd, mgc, c2);
	XDrawLine(mdd, mwin, mgc, mx + 2, my + mh - 2, mx + mw - 2,
		  my + mh - 2);
	XDrawLine(mdd, mwin, mgc, mx + mw - 2, my + 2, mx + mw - 2,
		  my + mh - 2);
     }
   else
     {
	XDrawRectangle(mdd, mwin, mgc, mx, my, mw - 1, mh - 1);
     }
}

#define DRAW_THIN_BOX_IN(mdd, mgc, mwin, mx, my, mw, mh) \
        AlertDrawThinBoxIn(mdd, mgc, mwin, mx, my, mw, mh, \
        colorful, cols[2], cols[0])
static void
AlertDrawThinBoxIn(Display * mdd, GC mgc, Window mwin, int mx, int my, int mw,
		   int mh, int colorful, unsigned long c1, unsigned long c2)
{
   if (colorful)
     {
	XSetForeground(mdd, mgc, c1);
	XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + mw - 3, my + 1);
	XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + 1, my + mh - 3);
	XSetForeground(mdd, mgc, c2);
	XDrawLine(mdd, mwin, mgc, mx + 2, my + mh - 2, mx + mw - 2,
		  my + mh - 2);
	XDrawLine(mdd, mwin, mgc, mx + mw - 2, my + 2, mx + mw - 2,
		  my + mh - 2);
     }
}

#define DRAW_HEADER(mdd, mgc, mwin, mx, my, mstr) \
        AlertDrawHeader(mdd, mgc, mwin, mx, my, mstr, \
        colorful, cols[2], cols[3], cols[4])
static void
AlertDrawHeader(Display * mdd, GC mgc, Window mwin, int mx, int my,
		const char *mstr, int colorful, unsigned long cb,
		unsigned long ct1, unsigned long ct2)
{
   int                 len = strlen(mstr);

   if (colorful)
     {
	XSetForeground(mdd, mgc, cb);
	XDrawString(mdd, mwin, mgc, mx + 1, my + 1, mstr, len);
	XDrawString(mdd, mwin, mgc, mx + 2, my + 1, mstr, len);
	XDrawString(mdd, mwin, mgc, mx + 2, my + 2, mstr, len);
	XDrawString(mdd, mwin, mgc, mx + 1, my + 2, mstr, len);
	XSetForeground(mdd, mgc, ct1);
	XDrawString(mdd, mwin, mgc, mx - 1, my, mstr, len);
	XDrawString(mdd, mwin, mgc, mx, my - 1, mstr, len);
	XDrawString(mdd, mwin, mgc, mx + 1, my, mstr, len);
	XDrawString(mdd, mwin, mgc, mx, my + 1, mstr, len);
	XSetForeground(mdd, mgc, ct2);
	XDrawString(mdd, mwin, mgc, mx, my, mstr, len);
     }
   else
     {
	XDrawString(mdd, mwin, mgc, mx, my, mstr, len);
     }
}

#define DRAW_STRING(mdd, mgc, mwin, mx, my, mstr) \
        AlertDrawString(mdd, mgc, mwin, mx, my, mstr, \
        colorful, cols[3])
static void
AlertDrawString(Display * mdd, GC mgc, Window mwin, int mx, int my,
		const char *mstr, int colorful, unsigned long ct1)
{
   int                 len = strlen(mstr);

   if (colorful)
     {
	XSetForeground(mdd, mgc, ct1);
	XDrawString(mdd, mwin, mgc, mx, my, mstr, len);
     }
   else
     {
	XDrawString(mdd, mwin, mgc, mx, my, mstr, len);
     }
}

static void
ShowAlert(char *text)
{
   Window              win = 0, b1 = 0, b2 = 0, b3 = 0;
   Display            *dd;
   int                 wid, hih, w, h, i, j, k, mask;
   XGCValues           gcv;
   GC                  gc;
   char                line[1024];
   XEvent              ev;
   XFontStruct        *xfs;
   Font                font;
   XSetWindowAttributes att;
   char                colorful;
   unsigned long       cols[256];
   XColor              xcl;
   Colormap            cmap;
   int                 cnum, fh, x, y, ww, hh, mh;
   static char        *title = NULL, *str1 = NULL, *str2 = NULL, *str3 = NULL;
   KeyCode             key;
   int                 button;

   EDBUG(8, "ShowAlert");
   if (!text)
      EDBUG_RETURN_;

   /*
    * We may get here from obscure places like an X-error or signal handler
    * and things seem to work properly only if we do a new XOpenDisplay().
    */
   dd = XOpenDisplay(NULL);
   if (!dd)
     {
	fprintf(stderr, text);
	fflush(stderr);
	EDBUG_RETURN_;
     }
   cmap = DefaultColormap(dd, DefaultScreen(dd));

   title = TitleText;
   str1 = IgnoreText;
   str2 = RestartText;
   str3 = ExitText;
   if (!title)
      title = _("Enlightenment Error");
   if (!str1)
      str1 = _("Ignore");
   if (!str2)
      str2 = _("Restart");
   if (!str3)
      str3 = _("Exit");

   cnum = 0;
   colorful = 0;
   if (DefaultDepth(dd, DefaultScreen(dd)) > 4)
     {
	ESetColor(&xcl, 220, 220, 220);
	if (!XAllocColor(dd, cmap, &xcl))
	   goto CN;
	cols[cnum++] = xcl.pixel;
	ESetColor(&xcl, 160, 160, 160);
	if (!XAllocColor(dd, cmap, &xcl))
	   goto CN;
	cols[cnum++] = xcl.pixel;
	ESetColor(&xcl, 100, 100, 100);
	if (!XAllocColor(dd, cmap, &xcl))
	   goto CN;
	cols[cnum++] = xcl.pixel;
	ESetColor(&xcl, 0, 0, 0);
	if (!XAllocColor(dd, cmap, &xcl))
	   goto CN;
	cols[cnum++] = xcl.pixel;
	ESetColor(&xcl, 255, 255, 255);
	if (!XAllocColor(dd, cmap, &xcl))
	   goto CN;
	cols[cnum++] = xcl.pixel;
	colorful = 1;
     }
 CN:

   wid = DisplayWidth(dd, DefaultScreen(dd));
   hih = DisplayHeight(dd, DefaultScreen(dd));
   w = (wid - 600) / 2;
   h = (hih - 440) / 2;

   if (colorful)
      att.background_pixel = cols[1];
   else
      att.background_pixel = BlackPixel(dd, DefaultScreen(dd));
   if (colorful)
      att.border_pixel = cols[3];
   else
      att.border_pixel = WhitePixel(dd, DefaultScreen(dd));
   att.backing_store = Always;
   att.save_under = True;
   att.override_redirect = True;
   mask = CWBackPixel | CWBorderPixel | CWOverrideRedirect | CWSaveUnder |
      CWBackingStore;
   win = XCreateWindow(dd, DefaultRootWindow(dd), -100, -100, 1, 1, 0,
		       DefaultDepth(dd, DefaultScreen(dd)), InputOutput,
		       DefaultVisual(dd, DefaultScreen(dd)), mask, &att);

   if (sscanf(str1, "%s", line) > 0)
     {
	b1 = XCreateWindow(dd, win, -100, -100, 1, 1, 0,
			   DefaultDepth(dd, DefaultScreen(dd)), InputOutput,
			   DefaultVisual(dd, DefaultScreen(dd)), mask, &att);
	EMapWindow(dd, b1);
     }
   if (sscanf(str2, "%s", line) > 0)
     {
	b2 = XCreateWindow(dd, win, -100, -100, 1, 1, 0,
			   DefaultDepth(dd, DefaultScreen(dd)), InputOutput,
			   DefaultVisual(dd, DefaultScreen(dd)), mask, &att);
	EMapWindow(dd, b2);
     }
   if (sscanf(str3, "%s", line) > 0)
     {
	b3 = XCreateWindow(dd, win, -100, -100, 1, 1, 0,
			   DefaultDepth(dd, DefaultScreen(dd)), InputOutput,
			   DefaultVisual(dd, DefaultScreen(dd)), mask, &att);
	EMapWindow(dd, b3);
     }

   gc = XCreateGC(dd, win, 0, &gcv);
   if (colorful)
      XSetForeground(dd, gc, cols[3]);
   else
      XSetForeground(dd, gc, att.border_pixel);
   fh = 0;
   xfs = NULL;
   if (!xfs)
      xfs = XLoadQueryFont(dd, "-*-helvetica-*-r-*-*-12-*-*-*-*-*-*-*");
   if (!xfs)
      xfs = XLoadQueryFont(dd, "fixed");
   font = xfs->fid;
   fh = xfs->ascent + xfs->descent;
   XSetFont(dd, gc, font);

   XSelectInput(dd, win, KeyPressMask | KeyReleaseMask | ExposureMask);
   XMapWindow(dd, win);
   XGrabPointer(dd, win, True, ButtonPressMask | ButtonReleaseMask,
		GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
   XGrabKeyboard(dd, win, True, GrabModeAsync, GrabModeAsync, CurrentTime);
   XSetInputFocus(dd, win, RevertToPointerRoot, CurrentTime);

   XGrabServer(dd);
   XSync(dd, False);

   for (i = 0; i < 600; i += 40)
     {
	ww = i;
	hh = (i * 440) / 600;
	x = (wid - ww) >> 1;
	y = (hih - hh) >> 1;
	XMoveResizeWindow(dd, win, x, y, ww, hh);
	DRAW_BOX_OUT(dd, gc, win, 0, 0, ww, hh);
	XSync(dd, False);
     }
   ww = 600;
   hh = 440;
   x = (wid - 600) >> 1;
   y = (hih - 440) >> 1;
   XMoveResizeWindow(dd, win, x, y, ww, hh);
   XUngrabServer(dd);
   XSync(dd, False);

   mh = XTextWidth(xfs, str1, strlen(str1)) + 10;
   h = XTextWidth(xfs, str2, strlen(str2)) + 10;
   if (h > mh)
      mh = h;
   h = XTextWidth(xfs, str3, strlen(str3)) + 10;
   if (h > mh)
      mh = h;
   if (sscanf(str1, "%s", line) > 0)
     {
	h = XTextWidth(xfs, str1, strlen(str1));
	w = 10 + (((580 - mh) * 0) / 4);
	XMoveResizeWindow(dd, b1, w - 5, 440 - 15 - fh, mh + 10, fh + 10);
	XSelectInput(dd, b1,
		     ButtonPressMask | ButtonReleaseMask | ExposureMask);
     }
   if (sscanf(str2, "%s", line) > 0)
     {
	h = XTextWidth(xfs, str2, strlen(str2));
	w = 10 + (((580 - mh) * 1) / 2);
	XMoveResizeWindow(dd, b2, w - 5, 440 - 15 - fh, mh + 10, fh + 10);
	XSelectInput(dd, b2,
		     ButtonPressMask | ButtonReleaseMask | ExposureMask);
     }
   if (sscanf(str3, "%s", line) > 0)
     {
	h = XTextWidth(xfs, str3, strlen(str3));
	w = 10 + (((580 - mh) * 2) / 2);
	XMoveResizeWindow(dd, b3, w - 5, 440 - 15 - fh, mh + 10, fh + 10);
	XSelectInput(dd, b3,
		     ButtonPressMask | ButtonReleaseMask | ExposureMask);
     }
   XSync(dd, False);

   button = 0;
   for (; button == 0;)
     {
	XNextEvent(dd, &ev);
	switch (ev.type)
	  {
	  case KeyPress:
	     key = XKeysymToKeycode(dd, XStringToKeysym("F1"));
	     if (key == ev.xkey.keycode)
	       {
		  DRAW_BOX_IN(dd, gc, b1, 0, 0, mh + 10, fh + 10);
		  XSync(dd, False);
		  sleep(1);
		  DRAW_BOX_OUT(dd, gc, b1, 0, 0, mh + 10, fh + 10);
		  button = 1;
		  break;
	       }
	     key = XKeysymToKeycode(dd, XStringToKeysym("F2"));
	     if (key == ev.xkey.keycode)
	       {
		  DRAW_BOX_IN(dd, gc, b2, 0, 0, mh + 10, fh + 10);
		  XSync(dd, False);
		  sleep(1);
		  DRAW_BOX_OUT(dd, gc, b2, 0, 0, mh + 10, fh + 10);
		  button = 2;
		  break;
	       }
	     key = XKeysymToKeycode(dd, XStringToKeysym("F3"));
	     if (key == ev.xkey.keycode)
	       {
		  DRAW_BOX_IN(dd, gc, b3, 0, 0, mh + 10, fh + 10);
		  XSync(dd, False);
		  sleep(1);
		  DRAW_BOX_OUT(dd, gc, b3, 0, 0, mh + 10, fh + 10);
		  button = 3;
		  break;
	       }
	     XSync(dd, False);
	     break;

	  case ButtonPress:
	     if (ev.xbutton.window == b1)
	       {
		  DRAW_BOX_IN(dd, gc, b1, 0, 0, mh + 10, fh + 10);
	       }
	     else if (ev.xbutton.window == b2)
	       {
		  DRAW_BOX_IN(dd, gc, b2, 0, 0, mh + 10, fh + 10);
	       }
	     else if (ev.xbutton.window == b3)
	       {
		  DRAW_BOX_IN(dd, gc, b3, 0, 0, mh + 10, fh + 10);
	       }
	     XSync(dd, False);
	     break;

	  case ButtonRelease:
	     if (ev.xbutton.window == b1)
	       {
		  DRAW_BOX_OUT(dd, gc, b1, 0, 0, mh + 10, fh + 10);
		  button = 1;
	       }
	     else if (ev.xbutton.window == b2)
	       {
		  DRAW_BOX_OUT(dd, gc, b2, 0, 0, mh + 10, fh + 10);
		  button = 2;
	       }
	     else if (ev.xbutton.window == b3)
	       {
		  DRAW_BOX_OUT(dd, gc, b3, 0, 0, mh + 10, fh + 10);
		  button = 3;
	       }
	     XSync(dd, False);
	     break;

	  case Expose:
	     /* Flush all other Expose events */
	     while (XCheckTypedWindowEvent(dd, ev.xexpose.window, Expose, &ev))
		w = XTextWidth(xfs, title, strlen(title));

	     DRAW_HEADER(dd, gc, win, (600 - w) / 2, 5 + xfs->ascent, title);
	     DRAW_BOX_OUT(dd, gc, win, 0, 0, ww, fh + 10);
	     DRAW_BOX_OUT(dd, gc, win, 0, fh + 10 - 1, ww,
			  hh - fh - fh - 30 + 2);
	     DRAW_BOX_OUT(dd, gc, win, 0, 440 - fh - 20, ww, fh + 20);
	     i = 0;
	     j = 0;
	     k = fh + 10;
	     while (text[i])
	       {
		  line[j++] = text[i++];
		  if (line[j - 1] == '\n')
		    {
		       line[j - 1] = 0;
		       j = 0;
		       DRAW_STRING(dd, gc, win, 6, 6 + k + fh, line);
		       k += fh + 2;
		    }
	       }
	     if (sscanf(str1, "%s", line) > 0)
	       {
		  h = XTextWidth(xfs, str1, strlen(str1));
		  w = 10 + (((580 - mh) * 0) / 4);
		  DRAW_HEADER(dd, gc, b1, 5 + (mh - h) / 2,
			      fh + 5 - xfs->descent, str1);
		  DRAW_BOX_OUT(dd, gc, b1, 0, 0, mh + 10, fh + 10);
		  DRAW_THIN_BOX_IN(dd, gc, win, w - 7, 440 - 17 - fh, mh + 14,
				   fh + 14);
	       }
	     if (sscanf(str2, "%s", line) > 0)
	       {
		  h = XTextWidth(xfs, str2, strlen(str2));
		  w = 10 + (((580 - mh) * 1) / 2);
		  DRAW_HEADER(dd, gc, b2, 5 + (mh - h) / 2,
			      fh + 5 - xfs->descent, str2);
		  DRAW_BOX_OUT(dd, gc, b2, 0, 0, mh + 10, fh + 10);
		  DRAW_THIN_BOX_IN(dd, gc, win, w - 7, 440 - 17 - fh, mh + 14,
				   fh + 14);
	       }
	     if (sscanf(str3, "%s", line) > 0)
	       {
		  h = XTextWidth(xfs, str3, strlen(str3));
		  w = 10 + (((580 - mh) * 2) / 2);
		  DRAW_HEADER(dd, gc, b3, 5 + (mh - h) / 2,
			      fh + 5 - xfs->descent, str3);
		  DRAW_BOX_OUT(dd, gc, b3, 0, 0, mh + 10, fh + 10);
		  DRAW_THIN_BOX_IN(dd, gc, win, w - 7, 440 - 17 - fh, mh + 14,
				   fh + 14);
	       }
	     XSync(dd, False);
	     break;

	  default:
	     break;
	  }
     }

   switch (button)
     {
     case 1:
	if (IgnoreFunction)
	   IgnoreFunction(IgnoreParams);
	break;
     case 2:
	if (RestartFunction)
	   RestartFunction(RestartParams);
	break;
     case 3:
	if (ExitFunction)
	   ExitFunction(ExitParams);
	break;
     default:
	break;
     }

   XDestroyWindow(dd, win);
   XFreeGC(dd, gc);
   XFreeFont(dd, xfs);
   XUnloadFont(dd, font);
   if (cnum > 0)
      XFreeColors(dd, cmap, cols, cnum, 0);
   XCloseDisplay(dd);

   EDBUG_RETURN_;
}
