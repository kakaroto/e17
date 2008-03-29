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
#include "ewins.h"
#include "focus.h"
#include "xwin.h"

#ifdef WITH_ZOOM
#include <X11/extensions/xf86vmode.h>

static int          std_vid_modes_num = 0;
static int          std_vid_mode_cur = 0;
static XF86VidModeModeInfo **std_vid_modes = NULL;

static Win          zoom_mask_1 = 0;
static Win          zoom_mask_2 = 0;
static Win          zoom_mask_3 = 0;
static Win          zoom_mask_4 = 0;
static EWin        *zoom_last_ewin = NULL;
static int          zoom_last_x, zoom_last_y;
static char         zoom_can = 0;

static void
FillStdVidModes(void)
{
   XF86VidModeGetAllModeLines(disp, Dpy.screen,
			      &std_vid_modes_num, &std_vid_modes);
}

static XF86VidModeModeInfo *
FindMode(int w, int h)
{
   XF86VidModeModeInfo *chosen = NULL;
   int                 i, closest = 0x7fffffff;

   for (i = 0; i < std_vid_modes_num; i++)
     {
	int                 value = 0x7fffffff;

	if ((std_vid_modes[i]->hdisplay >= w) &&
	    (std_vid_modes[i]->vdisplay >= h))
	   value = ((std_vid_modes[i]->hdisplay - w) +
		    (std_vid_modes[i]->vdisplay - h));
	if (value < closest)
	  {
	     closest = value;
	     chosen = std_vid_modes[i];
	  }
     }
   return chosen;
}

static int
GetModeIndex(unsigned int dotclock, XF86VidModeModeLine * line)
{
   int                 i;
   const XF86VidModeModeInfo *info;

   for (i = 0; i < std_vid_modes_num; i++)
     {
	info = std_vid_modes[i];
	if (info->dotclock == dotclock &&
	    info->hdisplay == line->hdisplay &&
	    info->vdisplay == line->vdisplay)
	   return i;
     }
   return 0;
}

static const XF86VidModeModeInfo *
SwitchRes(char inout, int x, int y, int w, int h)
{
   static int          vp_x, vp_y;
   XF86VidModeModeInfo *mode = NULL;
   int                 scr;

   scr = Dpy.screen;

   if (inout)
     {
	XF86VidModeModeLine curmode;
	int                 dotclock;

	if (!XF86VidModeGetModeLine(disp, scr, &dotclock, &curmode))
	   return mode;
	XF86VidModeGetViewPort(disp, scr, &vp_x, &vp_y);

	mode = FindMode(w, h);
	if (mode)
	  {
	     XF86VidModeLockModeSwitch(disp, scr, 0);
	     std_vid_mode_cur = GetModeIndex(dotclock, &curmode);
	     XF86VidModeSwitchToMode(disp, scr, mode);
	     XF86VidModeSetViewPort(disp, scr, x, y);
	     XF86VidModeLockModeSwitch(disp, scr, 1);
	  }
     }
   else
     {
	mode = std_vid_modes[std_vid_mode_cur];
	XF86VidModeLockModeSwitch(disp, scr, 0);
	XF86VidModeSwitchToMode(disp, scr, mode);
	XF86VidModeSetViewPort(disp, scr, vp_x, vp_y);
#if 0				/* No, don't lock or we can't switch resolution */
	XF86VidModeLockModeSwitch(disp, scr, 1);
#endif
     }
   return mode;
}

static char
XHasDGA(void)
{
   int                 ev_base, er_base;

   if (XF86VidModeQueryExtension(disp, &ev_base, &er_base))
      return 1;
   else
      return 0;
}

EWin               *
GetZoomEWin(void)
{
   return zoom_last_ewin;
}

void
ReZoom(EWin * ewin)
{
   if ((InZoom()) && (ewin != zoom_last_ewin))
     {
	Zoom(NULL);
	Zoom(ewin);
     }
}

char
InZoom(void)
{
   if (zoom_last_ewin)
      return 1;
   return 0;
}

char
CanZoom(void)
{
   return zoom_can;
}

void
ZoomInit(void)
{
   if (XHasDGA())
     {
	FillStdVidModes();
	if (std_vid_modes_num > 1)
	   zoom_can = 1;
     }
}

static              Win
ZoomMask(int x, int y, int w, int h)
{
   Win                 win;

   if (x < 0 || y < 0 || w <= 0 || h <= 0)
      return 0;

   win = ECreateWindow(VROOT, x, y, w, h, 0);
   ESetWindowBackground(win, Dpy.pixel_black);
   ERaiseWindow(win);
   EMapWindow(win);

   return win;
}

void
Zoom(EWin * ewin)
{
   const XF86VidModeModeInfo *mode;

   if (!CanZoom())
      return;

   if (!ewin)
     {
	if (zoom_last_ewin)
	  {
	     ewin = zoom_last_ewin;
/*           XUngrabPointer(disp, CurrentTime); */
	     EwinMove(ewin, zoom_last_x, zoom_last_y);
	     if (zoom_mask_1)
		EDestroyWindow(zoom_mask_1);
	     if (zoom_mask_2)
		EDestroyWindow(zoom_mask_2);
	     if (zoom_mask_3)
		EDestroyWindow(zoom_mask_3);
	     if (zoom_mask_4)
		EDestroyWindow(zoom_mask_4);
	     SwitchRes(0, 0, 0, 0, 0);
	     EwinWarpTo(ewin);
	     ESync(0);
	     zoom_last_ewin = NULL;
	  }
	return;
     }

   mode = SwitchRes(1, 0, 0, ewin->client.w, ewin->client.h);
   if (mode)
     {
	int                 x1, y1, x2, y2, bl, br, bt, bb;

	zoom_last_ewin = ewin;
	zoom_last_x = EoGetX(ewin);
	zoom_last_y = EoGetY(ewin);
	x1 = (mode->hdisplay - ewin->client.w) / 2;
	if (x1 < 0)
	   x1 = 0;
	y1 = (mode->vdisplay - ewin->client.h) / 2;
	if (y1 < 0)
	   y1 = 0;
	x2 = mode->hdisplay - ewin->client.w - x1;
	if (x2 < 0)
	   x2 = 0;
	y2 = mode->vdisplay - ewin->client.h - y1;
	if (y2 < 0)
	   y2 = 0;
	EwinRaise(ewin);
	EwinBorderGetSize(ewin, &bl, &br, &bt, &bb);
	EwinMove(ewin, -bl + x1, -bt + y1);
	FocusToEWin(ewin, FOCUS_SET);
	EwinWarpTo(ewin);
#if 0				/* Doesn't work as intended */
	XGrabPointer(disp, EwinGetClientXwin(ewin), True,
		     ButtonPressMask | ButtonReleaseMask |
		     PointerMotionMask | ButtonMotionMask |
		     EnterWindowMask | LeaveWindowMask,
		     GrabModeAsync, GrabModeAsync,
		     EwinGetClientXwin(ewin), None, CurrentTime);
#endif
	zoom_mask_1 = ZoomMask(0, 0, x1, mode->vdisplay);
	zoom_mask_2 = ZoomMask(0, 0, mode->hdisplay, y1);
	zoom_mask_3 = ZoomMask(x1 + ewin->client.w, 0, x2, mode->vdisplay);
	zoom_mask_4 = ZoomMask(0, y1 + ewin->client.h, mode->hdisplay, y2);
	ESync(0);
     }
}

#else

EWin               *
GetZoomEWin(void)
{
   return NULL;
}

void
ReZoom(EWin * ewin)
{
   ewin = NULL;
}

char
InZoom(void)
{
   return 0;
}

char
CanZoom(void)
{
   return 0;
}

void
ZoomInit(void)
{
}

void
Zoom(EWin * ewin)
{
   ewin = NULL;
}

#endif
