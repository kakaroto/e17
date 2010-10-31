/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2010 Kim Woelders
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
#include "borders.h"
#include "ewins.h"
#include "focus.h"
#include "hints.h"
#include "xwin.h"
#if USE_XRANDR
#include <X11/extensions/Xrandr.h>
#endif

static EWin        *zoom_last_ewin = NULL;
static signed char  zoom_can = 0;

#define USE_xf86vmode 1
#if USE_xf86vmode
#include <X11/extensions/xf86vmode.h>

static int          std_vid_modes_num = 0;
static int          std_vid_mode_cur = 0;
static XF86VidModeModeInfo **std_vid_modes = NULL;

static XF86VidModeModeInfo *
FindMode(int w, int h)
{
   XF86VidModeModeInfo *chosen = NULL;
   int                 i, closest = 0x7fffffff;

#if USE_XRANDR
   if ((Mode.screen.rotation == RR_Rotate_90) ||
       (Mode.screen.rotation == RR_Rotate_270))
     {
	i = w;
	w = h;
	h = i;
     }
#endif
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

static int
SwitchRes(char inout, int x, int y, int w, int h, int *dw, int *dh)
{
   static int          vp_x, vp_y;
   XF86VidModeModeInfo *mode;
   int                 scr;

   scr = Dpy.screen;

   if (inout)
     {
	XF86VidModeModeLine curmode;
	int                 dotclock;
	int                 rx, ry;

	if (!XF86VidModeGetModeLine(disp, scr, &dotclock, &curmode))
	   return 0;
	XF86VidModeGetViewPort(disp, scr, &vp_x, &vp_y);

	mode = FindMode(w, h);
	if (mode)
	  {
#if USE_XRANDR
	     int                 vw, vh;

	     vw = WinGetW(VROOT);
	     vh = WinGetH(VROOT);
	     /* x and y relative to unrotated display */
	     if (Mode.screen.rotation == RR_Rotate_90)
	       {
		  rx = y;
		  ry = vw - mode->vdisplay - x;
	       }
	     else if (Mode.screen.rotation == RR_Rotate_270)
	       {
		  rx = vh - mode->hdisplay - y;
		  ry = x;
	       }
	     else if (Mode.screen.rotation == RR_Rotate_180)
	       {
		  rx = vw - mode->hdisplay - x;
		  ry = vh - mode->vdisplay - y;
	       }
	     else
#endif
	       {
		  rx = x;
		  ry = y;
	       }
#if USE_XRANDR
	     if ((Mode.screen.rotation == RR_Rotate_90) ||
		 (Mode.screen.rotation == RR_Rotate_270))
	       {
		  *dw = mode->vdisplay;
		  *dh = mode->hdisplay;
	       }
	     else
#endif
	       {
		  *dw = mode->hdisplay;
		  *dh = mode->vdisplay;
	       }
	     XF86VidModeLockModeSwitch(disp, scr, 0);
	     std_vid_mode_cur = GetModeIndex(dotclock, &curmode);
	     XF86VidModeSwitchToMode(disp, scr, mode);
	     XF86VidModeSetViewPort(disp, scr, rx, ry);
	     XF86VidModeLockModeSwitch(disp, scr, 1);
	     return 1;
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
   return 0;
}

static void
ZoomInit(void)
{
   int                 ev_base, er_base;

   zoom_can = -1;

   if (!XF86VidModeQueryExtension(disp, &ev_base, &er_base))
      return;

   XF86VidModeGetAllModeLines(disp, Dpy.screen,
			      &std_vid_modes_num, &std_vid_modes);

   if (std_vid_modes_num > 1)
      zoom_can = 1;
}

#else

static int
SwitchRes(char inout, int x, int y, int w, int h, int *dw, int *dh)
{
   inout = 0;
   x = y = w = h = 0;
   *dw = *dh = 0;
   return 0;
}

static void
ZoomInit(void)
{
}

#endif

void
Zoom(EWin * ewin, int on)
{
   int                 dw, dh;

   if (Mode.wm.window)
      return;

   if (zoom_can == 0)
      ZoomInit();

   if (zoom_can <= 0)
      return;

   if (!on)
     {
	/* Unzoom */

	if (ewin && ewin != zoom_last_ewin)
	   return;
	ewin = zoom_last_ewin;
	if (!ewin)
	   return;

	zoom_last_ewin = NULL;

	SwitchRes(0, 0, 0, 0, 0, NULL, NULL);
	EwinBorderSetTo(ewin, ewin->normal_border);
	ewin->state.zoomed = 0;
	EwinMoveResize(ewin, ewin->save_fs.x, ewin->save_fs.y,
		       ewin->client.w, ewin->client.h);
     }
   else
     {
	/* Zoom */

	if (ewin->state.fullscreen)
	   return;

	on = SwitchRes(1, 0, 0, ewin->client.w, ewin->client.h, &dw, &dh);
	if (!on)
	   return;

	zoom_last_ewin = ewin;
	ewin->save_fs.x = EoGetX(ewin);
	ewin->save_fs.y = EoGetY(ewin);
	EwinRaise(ewin);
	EwinBorderSetTo(ewin, BorderCreateFiller(ewin->client.w,
						 ewin->client.h, dw, dh));
	EwinMoveResize(ewin, 0, 0, ewin->client.w, ewin->client.h);
	ewin->state.zoomed = 1;
	FocusToEWin(ewin, FOCUS_SET);
     }

   EwinWarpTo(ewin, 1);
   ESync(0);
   EwinStateUpdate(ewin);
   HintsSetWindowState(ewin);
}

void
ReZoom(EWin * ewin)
{
   if (zoom_last_ewin && ewin == zoom_last_ewin)
     {
	Zoom(ewin, 0);
	Zoom(ewin, 1);
     }
}
