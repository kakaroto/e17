
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

static void         ShowAlert(char *text);
static void         AlertHandleClick(int button);

static int          (*IgnoreFunction) (void *) = NULL;
static void        *IgnoreParams = NULL;
static char        *IgnoreText = NULL;
static int          (*RestartFunction) (void *) = NULL;
static void        *RestartParams = NULL;
static char        *RestartText = NULL;
static int          (*ExitFunction) (void *) = NULL;
static void        *ExitParams = NULL;
static char        *ExitText = NULL;

static char        *TitleText = NULL;

void
Alert(char *fmt, ...)
{
   char                text[10240];
   va_list             ap;

   EDBUG(7, "Alert");
   SC_Kill();
   va_start(ap, fmt);
/*
 * #ifdef __USE_GNU
 */
   Evsnprintf(text, 10240, fmt, ap);
/*
 * #else
 * vsprintf(text, fmt, ap);
 * #endif
 */
   va_end(ap);
   AUDIO_PLAY("SOUND_ALERT");
   ShowAlert(text);
   EDBUG_RETURN_;
}

void
AssignTitleText(char *text)
{
   EDBUG(7, "AssignTitleText");
   if (TitleText)
      Efree(TitleText);
   TitleText = NULL;
   TitleText = duplicate(text);
   EDBUG_RETURN_;
}

void
AssignIgnoreText(char *text)
{
   EDBUG(7, "AssignIgnoreText");
   if (IgnoreText)
      Efree(IgnoreText);
   IgnoreText = NULL;
   IgnoreText = Emalloc(strlen(text) + 6);
   sprintf(IgnoreText, "(F1) %s", text);
   EDBUG_RETURN_;
}

void
AssignRestartText(char *text)
{
   EDBUG(7, "AssignRestartText");
   if (RestartText)
      Efree(RestartText);
   RestartText = NULL;
   RestartText = Emalloc(strlen(text) + 6);
   sprintf(RestartText, "(F2) %s", text);
   EDBUG_RETURN_;
}

void
AssignExitText(char *text)
{
   EDBUG(7, "AssignExitText");
   if (ExitText)
      Efree(ExitText);
   ExitText = NULL;
   ExitText = Emalloc(strlen(text) + 6);
   sprintf(ExitText, "(F3) %s", text);
   EDBUG_RETURN_;
}

void
AssignIgnoreFunction(int (*FunctionToAssign) (void *), void *params)
{
   EDBUG(7, "AssignIgnoreFunction");
   IgnoreFunction = FunctionToAssign;
   IgnoreParams = params;
   EDBUG_RETURN_;
}

void
AssignRestartFunction(int (*FunctionToAssign) (void *), void *params)
{
   EDBUG(7, "AssignRestartFunction");
   RestartFunction = FunctionToAssign;
   RestartParams = params;
   EDBUG_RETURN_;
}

void
AssignExitFunction(int (*FunctionToAssign) (void *), void *params)
{
   EDBUG(7, "AssignExitFunction");
   ExitFunction = FunctionToAssign;
   ExitParams = params;
   EDBUG_RETURN_;
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
   XColor              xcl;
   Colormap            cmap;
   int                 cols[256];
   int                 cnum, r, g, b, fh, x, y, ww, hh, mh;
   static char        *title = NULL, *str1 = NULL, *str2 = NULL, *str3 = NULL;
   KeyCode             key;

   EDBUG(8, "ShowAlert");
   if (!text)
      EDBUG_RETURN_;
   dd = XOpenDisplay(NULL);
   if (!dd)
     {
	fprintf(stderr, text);
	fflush(stderr);
	EDBUG_RETURN_;
     }
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

#define DRAW_BOX_OUT(mdd, mgc, mwin, mx, my, mw, mh) \
if (colorful) { \
XSetForeground(mdd, mgc, cols[3]); \
XDrawRectangle(mdd, mwin, mgc, mx, my, mw - 1, mh - 1); \
XSetForeground(mdd, mgc, cols[0]); \
XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + mw - 3, my + 1); \
XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + 1, my + mh - 3); \
XSetForeground(mdd, mgc, cols[2]); \
XDrawLine(mdd, mwin, mgc, mx + 2, my + mh - 2, mx + mw - 2, my + mh - 2); \
XDrawLine(mdd, mwin, mgc, mx + mw - 2, my + 2, mx + mw - 2, my + mh - 2); \
} else { \
XDrawRectangle(mdd, mwin, mgc, mx, my, mw - 1, mh - 1); \
}

#define DRAW_BOX_IN(mdd, mgc, mwin, mx, my, mw, mh) \
if (colorful) { \
XSetForeground(mdd, mgc, cols[3]); \
XDrawRectangle(mdd, mwin, mgc, mx, my, mw - 1, mh - 1); \
XSetForeground(mdd, mgc, cols[2]); \
XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + mw - 3, my + 1); \
XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + 1, my + mh - 3); \
XSetForeground(mdd, mgc, cols[0]); \
XDrawLine(mdd, mwin, mgc, mx + 2, my + mh - 2, mx + mw - 2, my + mh - 2); \
XDrawLine(mdd, mwin, mgc, mx + mw - 2, my + 2, mx + mw - 2, my + mh - 2); \
} else { \
XDrawRectangle(mdd, mwin, mgc, mx, my, mw - 1, mh - 1); \
}

#define DRAW_THIN_BOX_IN(mdd, mgc, mwin, mx, my, mw, mh) \
if (colorful) { \
XSetForeground(mdd, mgc, cols[2]); \
XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + mw - 3, my + 1); \
XDrawLine(mdd, mwin, mgc, mx + 1, my + 1, mx + 1, my + mh - 3); \
XSetForeground(mdd, mgc, cols[0]); \
XDrawLine(mdd, mwin, mgc, mx + 2, my + mh - 2, mx + mw - 2, my + mh - 2); \
XDrawLine(mdd, mwin, mgc, mx + mw - 2, my + 2, mx + mw - 2, my + mh - 2); \
}

#define DRAW_HEADER(mdd, mgc, mwin, mx, my, mstr) \
if (colorful) { \
XSetForeground(mdd, mgc, cols[2]); \
XDrawString(mdd, mwin, mgc, mx + 1, my + 1, mstr, strlen(mstr)); \
XDrawString(mdd, mwin, mgc, mx + 2, my + 1, mstr, strlen(mstr)); \
XDrawString(mdd, mwin, mgc, mx + 2, my + 2, mstr, strlen(mstr)); \
XDrawString(mdd, mwin, mgc, mx + 1, my + 2, mstr, strlen(mstr)); \
XSetForeground(mdd, mgc, cols[3]); \
XDrawString(mdd, mwin, mgc, mx - 1, my, mstr, strlen(mstr)); \
XDrawString(mdd, mwin, mgc, mx, my - 1, mstr, strlen(mstr)); \
XDrawString(mdd, mwin, mgc, mx + 1, my, mstr, strlen(mstr)); \
XDrawString(mdd, mwin, mgc, mx, my + 1, mstr, strlen(mstr)); \
XSetForeground(mdd, mgc, cols[4]); \
XDrawString(mdd, mwin, mgc, mx, my, mstr, strlen(mstr)); \
} else { \
XDrawString(mdd, mwin, mgc, mx, my, mstr, strlen(mstr)); \
}

#define DRAW_STRING(mdd, mgc, mwin, mx, my, mstr) \
if (colorful) { \
XSetForeground(mdd, mgc, cols[3]); \
XDrawString(mdd, mwin, mgc, mx, my, mstr, strlen(mstr)); \
} else { \
XDrawString(mdd, mwin, mgc, mx, my, mstr, strlen(mstr)); \
}

#define ALLOC_COLOR(d,m,c) \
if (!XAllocColor(d, m, c)) \
{ \
colorful = 0; \
goto CN; \
}

   cnum = 0;
   cmap = 0;
   colorful = 0;
   if (DefaultDepth(dd, DefaultScreen(dd)) > 4)
      colorful = 1;
   if (colorful)
     {
	cmap = DefaultColormap(dd, DefaultScreen(dd));
	r = 220;
	g = 220;
	b = 220;
	xcl.red = (r << 8) | r;
	xcl.green = (g << 8) | g;
	xcl.blue = (b << 8) | b;
	ALLOC_COLOR(dd, cmap, &xcl);
	cols[cnum++] = xcl.pixel;
	r = 160;
	g = 160;
	b = 160;
	xcl.red = (r << 8) | r;
	xcl.green = (g << 8) | g;
	xcl.blue = (b << 8) | b;
	ALLOC_COLOR(dd, cmap, &xcl);
	cols[cnum++] = xcl.pixel;
	r = 100;
	g = 100;
	b = 100;
	xcl.red = (r << 8) | r;
	xcl.green = (g << 8) | g;
	xcl.blue = (b << 8) | b;
	ALLOC_COLOR(dd, cmap, &xcl);
	cols[cnum++] = xcl.pixel;
	r = 0;
	g = 0;
	b = 0;
	xcl.red = (r << 8) | r;
	xcl.green = (g << 8) | g;
	xcl.blue = (b << 8) | b;
	ALLOC_COLOR(dd, cmap, &xcl);
	cols[cnum++] = xcl.pixel;
	r = 255;
	g = 255;
	b = 255;
	xcl.red = (r << 8) | r;
	xcl.green = (g << 8) | g;
	xcl.blue = (b << 8) | b;
	ALLOC_COLOR(dd, cmap, &xcl);
	cols[cnum++] = xcl.pixel;
     }
 CN:
   wid = DisplayWidth(dd, DefaultScreen(dd));
   hih = DisplayHeight(dd, DefaultScreen(dd));
   w = (wid - 600) / 2;
   h = (hih - 440) / 2;
   mask =
      CWBackPixel | CWBorderPixel | CWOverrideRedirect | CWSaveUnder |
      CWBackingStore;
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
   win =
      XCreateWindow(dd, DefaultRootWindow(dd), -100, -100, 1, 1, 0,
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
     {
	xfs = XLoadQueryFont(dd, "-*-helvetica-*-r-*-*-12-*-*-*-*-*-*-*");
     }
   if (!xfs)
     {
	xfs = XLoadQueryFont(dd, "fixed");
     }
   font = xfs->fid;
   fh = xfs->ascent + xfs->descent;
   XSetFont(dd, gc, font);
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
   XSelectInput(dd, win, KeyPressMask | KeyReleaseMask | ExposureMask);

#define DRAW_ALERT \
w = XTextWidth(xfs, title, strlen(title)); \
DRAW_HEADER(dd, gc, win, (600 - w) / 2, 5 + xfs->ascent, title); \
DRAW_BOX_OUT(dd, gc, win, 0, 0, ww, fh + 10); \
DRAW_BOX_OUT(dd, gc, win, 0, fh + 10 - 1, ww, hh - fh - fh - 30 + 2); \
DRAW_BOX_OUT(dd, gc, win, 0, 440 - fh - 20, ww, fh + 20); \
i = 0; \
j = 0; \
k = fh + 10; \
while (text[i]) { \
line[j++] = text[i++]; \
if (line[j - 1] == '\n') { \
line[j - 1] = 0; \
j = 0; \
DRAW_STRING(dd, gc, win, 6, 6 + k + fh, line); \
k += fh + 2; \
} \
} \
if (sscanf(str1, "%s", line) > 0) \
{ \
h = XTextWidth(xfs, str1, strlen(str1)); \
w = 10 + (((580 - mh) * 0) / 4); \
DRAW_HEADER(dd, gc, b1, 5 + (mh - h) / 2, fh + 5 - xfs->descent, str1); \
DRAW_BOX_OUT(dd, gc, b1, 0, 0, mh + 10, fh + 10); \
DRAW_THIN_BOX_IN(dd, gc, win, w - 7, 440 - 17 - fh, mh + 14, fh + 14); \
} \
if (sscanf(str2, "%s", line) > 0) \
{ \
h = XTextWidth(xfs, str2, strlen(str2)); \
w = 10 + (((580 - mh) * 1) / 2); \
DRAW_HEADER(dd, gc, b2, 5 + (mh - h) / 2, fh + 5 - xfs->descent, str2); \
DRAW_BOX_OUT(dd, gc, b2, 0, 0, mh + 10, fh + 10); \
DRAW_THIN_BOX_IN(dd, gc, win, w - 7, 440 - 17 - fh, mh + 14, fh + 14); \
} \
if (sscanf(str3, "%s", line) > 0) \
{ \
h = XTextWidth(xfs, str3, strlen(str3)); \
w = 10 + (((580 - mh) * 2) / 2); \
DRAW_HEADER(dd, gc, b3, 5 + (mh - h) / 2, fh + 5 - xfs->descent, str3); \
DRAW_BOX_OUT(dd, gc, b3, 0, 0, mh + 10, fh + 10); \
DRAW_THIN_BOX_IN(dd, gc, win, w - 7, 440 - 17 - fh, mh + 14, fh + 14); \
XSync(dd, False); \
}

   DRAW_ALERT;

   w = 1;
   while (w == 1)
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
		  XSync(dd, False);
		  AlertHandleClick(1);
		  w = 0;
	       }
	     key = XKeysymToKeycode(dd, XStringToKeysym("F2"));
	     if (key == ev.xkey.keycode)
	       {
		  DRAW_BOX_IN(dd, gc, b2, 0, 0, mh + 10, fh + 10);
		  XSync(dd, False);
		  sleep(1);
		  DRAW_BOX_OUT(dd, gc, b2, 0, 0, mh + 10, fh + 10);
		  XSync(dd, False);
		  AlertHandleClick(2);
		  w = 0;
	       }
	     key = XKeysymToKeycode(dd, XStringToKeysym("F3"));
	     if (key == ev.xkey.keycode)
	       {
		  DRAW_BOX_IN(dd, gc, b3, 0, 0, mh + 10, fh + 10);
		  XSync(dd, False);
		  sleep(1);
		  DRAW_BOX_OUT(dd, gc, b3, 0, 0, mh + 10, fh + 10);
		  XSync(dd, False);
		  AlertHandleClick(3);
		  w = 0;
	       }
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
	     w = 1;
	     XSync(dd, False);
	     break;
	  case ButtonRelease:
	     if (ev.xbutton.window == b1)
	       {
		  DRAW_BOX_OUT(dd, gc, b1, 0, 0, mh + 10, fh + 10);
		  XSync(dd, False);
		  AlertHandleClick(1);
		  w = 0;
	       }
	     else if (ev.xbutton.window == b2)
	       {
		  DRAW_BOX_OUT(dd, gc, b2, 0, 0, mh + 10, fh + 10);
		  XSync(dd, False);
		  AlertHandleClick(2);
		  w = 0;
	       }
	     else if (ev.xbutton.window == b3)
	       {
		  DRAW_BOX_OUT(dd, gc, b3, 0, 0, mh + 10, fh + 10);
		  XSync(dd, False);
		  AlertHandleClick(3);
		  w = 0;
	       }
	     break;
	  case Expose:
	     DRAW_ALERT;
	     w = 1;
	     break;
	  default:
	     break;
	  }
     }
   XDestroyWindow(dd, win);
   XFreeGC(dd, gc);
   XFreeFont(dd, xfs);
   XUnloadFont(dd, font);
   if (cnum > 0)
      XFreeColors(dd, cmap, (unsigned long *)cols, cnum, 0);
   XCloseDisplay(dd);
   EDBUG_RETURN_;
}

static void
AlertHandleClick(int button)
{
   EDBUG(9, "AlertHandleClick");
   switch (button)
     {
     case 1:
	if (IgnoreFunction)
	   (*(IgnoreFunction)) (IgnoreParams);
	break;
     case 2:
	if (RestartFunction)
	   (*(RestartFunction)) (RestartParams);
	break;
     case 3:
	if (ExitFunction)
	   (*(ExitFunction)) (ExitParams);
	break;
     default:
	break;
     }
   EDBUG_RETURN_;
}
