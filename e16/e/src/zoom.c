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

#ifdef WITH_ZOOM
#include <X11/extensions/xf86vmode.h>

static int          std_vid_modes_num = 0;
static int          std_vid_mode_cur = 0;
static XF86VidModeModeInfo **std_vid_modes = NULL;

static Window       zoom_mask_1 = 0;
static Window       zoom_mask_2 = 0;
static EWin        *zoom_last_ewin = NULL;
static int          zoom_last_x, zoom_last_y;
static char         zoom_can = 0;

static void         FillStdVidModes(void);
static XF86VidModeModeInfo *FindMode(int w, int h);
static int          GetModeJumpCount(XF86VidModeModeInfo * new);
static char         SwitchRes(char inout, int x, int y, int w, int h);
static char         XHasDGA(void);

static void
FillStdVidModes(void)
{
   XF86VidModeGetAllModeLines(disp, root.scr, &std_vid_modes_num,
                              &std_vid_modes);
}

static XF86VidModeModeInfo *
FindMode(int w, int h)
{
   XF86VidModeModeInfo *chosen = NULL;
   int                 i, closest = 0x7fffffff;

   for (i = 0; i < std_vid_modes_num; i++)
     {
        int                 close = 0x7fffffff;

        if ((std_vid_modes[i]->hdisplay >= w)
            && (std_vid_modes[i]->vdisplay >= h))
           close =
               ((std_vid_modes[i]->hdisplay - w) +
                (std_vid_modes[i]->vdisplay - h));
        if (close < closest)
          {
             closest = close;
             chosen = std_vid_modes[i];
          }
     }
   return chosen;
}

static int
GetModeJumpCount(XF86VidModeModeInfo * new)
{
   int                 i, next;

   next = 0;
   for (i = 0; i < std_vid_modes_num; i++)
     {
        if ((new->hdisplay == std_vid_modes[i]->hdisplay)
            && (new->vdisplay == std_vid_modes[i]->vdisplay))
           next = i;
     }
   return next;
}

static char
SwitchRes(char inout, int x, int y, int w, int h)
{
   XF86VidModeModeInfo *mode;
   XF86VidModeModeLine curmode;
   int                 dotclock, jump, i;

   if (inout)
     {
        if (!XF86VidModeGetModeLine(disp, root.scr, &dotclock, &curmode))
           return 0;
        mode = FindMode(w, h);
        if (mode)
          {
             jump = GetModeJumpCount(mode);
             XWarpPointer(disp, None, root.win, 0, 0, 0, 0, x, y);
             XF86VidModeSetViewPort(disp, root.scr, x, y);
             XF86VidModeLockModeSwitch(disp, root.scr, 0);
             for (i = 0; i < jump; i++)
                XF86VidModeSwitchMode(disp, root.scr, 1);
             std_vid_mode_cur = jump;
             XF86VidModeLockModeSwitch(disp, root.scr, 1);
          }
        else
           return 0;
     }
   else
     {
        XF86VidModeLockModeSwitch(disp, root.scr, 0);
        for (i = 0; i < (std_vid_modes_num - std_vid_mode_cur); i++)
           XF86VidModeSwitchMode(disp, root.scr, 1);
        XF86VidModeSetViewPort(disp, root.scr, x, y);
        std_vid_mode_cur = 0;
     }
   return 1;
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

void
Zoom(EWin * ewin)
{
   if (!CanZoom())
      return;
   if (!ewin)
     {
        if (zoom_last_ewin)
          {
/*           XUngrabPointer(disp, CurrentTime); */
             MoveEwin(zoom_last_ewin, zoom_last_x, zoom_last_y);
             ICCCM_Configure(zoom_last_ewin);
             XDestroyWindow(disp, zoom_mask_1);
             XDestroyWindow(disp, zoom_mask_2);
             SwitchRes(0, 0, 0, 0, 0);
             XSync(disp, False);
          }
        zoom_last_ewin = NULL;
        return;
     }
   if (SwitchRes(1, 0, 0, ewin->client.w, ewin->client.h))
     {
        zoom_last_ewin = ewin;
        zoom_last_x = ewin->x;
        zoom_last_y = ewin->y;
        RaiseEwin(ewin);
        MoveEwin(ewin, -ewin->border->border.left, -ewin->border->border.top);
        ICCCM_Configure(ewin);
        FocusToEWin(ewin);
/*      XGrabPointer(disp, ewin->client.win, False, 0,
 * ButtonPressMask | ButtonReleaseMask |
 * PointerMotionMask | ButtonMotionMask |
 * EnterWindowMask | LeaveWindowMask,
 * GrabModeAsync, GrabModeAsync, None, 
 * ewin->client.win, None, 
 * CurrentTime); */
        zoom_mask_1 =
            ECreateWindow(root.win, ewin->client.w, 0, root.w, root.h, 0);
        zoom_mask_2 =
            ECreateWindow(root.win, 0, ewin->client.h, root.w, root.h, 0);
        XSetWindowBackgroundPixmap(disp, zoom_mask_1, None);
        XSetWindowBackgroundPixmap(disp, zoom_mask_2, None);
        XSetWindowBackground(disp, zoom_mask_1, BlackPixel(disp, root.scr));
        XSetWindowBackground(disp, zoom_mask_2, BlackPixel(disp, root.scr));
        XRaiseWindow(disp, zoom_mask_1);
        XRaiseWindow(disp, zoom_mask_2);
        XMapWindow(disp, zoom_mask_1);
        XMapWindow(disp, zoom_mask_2);
        XSync(disp, False);
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
