/*
 * Copyright (C) 2007-2008 Kim Woelders
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
#include "cursors.h"
#include "ecompmgr.h"
#include "eimage.h"
#include "emodule.h"
#include "eobj.h"
#include "events.h"
#include "ewins.h"
#include "grabs.h"
#include "hints.h"
#include "tclass.h"
#include "timers.h"
#include "util.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <X11/keysym.h>

#define USE_TIMER    0
#define USE_ANIMATOR 1

/* Magnifier window */
typedef struct {
   EWin               *ewin;
   const char         *title;
#if USE_TIMER
   Timer              *timer;
#endif
   int                 cx, cy;	/* Center */
   int                 scale;	/* Zoom level */
   int                 sx, sy;	/* Scene x,y */
   int                 sw, sh;	/* Scene wxh */
   char                mode;
   char                bpress;
   char                filter;
   char                grabbing;
   char                step;
   unsigned int        damage_count;
   char                update;
} MagWindow;

static void         MagwinDestroy(MagWindow * mw);

static MagWindow   *MagWin = NULL;

static void
MagwinDrawText(MagWindow * mw, int x, int y, const char *txt)
{
   TextClass          *tc;
   int                 cw, ch;

   tc = TextclassFind("COORDS", 1);
   if (!tc)
      return;

   TextSize(tc, 0, 0, 0, txt, &cw, &ch, 17);
   TextDraw(tc, EwinGetClientWin(mw->ewin), None, 0, 0, 0,
	    txt, x, y, cw, ch, 17, 0);
}

static unsigned int
MagwinGetPixel(Drawable draw, unsigned int x, unsigned int y)
{
   EImage             *im;
   unsigned int       *pd, pixel;

   im = EImageGrabDrawable(draw, None, x, y, 1, 1, 0);
   pd = (unsigned int *)EImageGetData(im);
   pixel = *pd;
   EImageFree(im);

   return pixel;
}

static void
MagwinRedraw(MagWindow * mw, int paint)
{
   int                 ww, wh;
   int                 sx, sy, sw, sh;
   double              scale;
   Drawable            draw;
   char                buf[128];
   int                 px, py;
   int                 qx, qy;
   int                 out;
   unsigned int        pixel;

   ww = mw->ewin->client.w;
   wh = mw->ewin->client.h;

   if (mw->scale < -6)
      mw->scale = -6;
   else if (mw->scale > 6)
      mw->scale = 6;
   scale = pow(2., (double)(mw->scale));
   sw = (int)((ww + .999 * scale) / scale);
   if (sw > WinGetW(VROOT))
      scale = (double)ww / (double)WinGetW(VROOT);
   sh = (int)((wh + .999 * scale) / scale);
   if (sh > WinGetH(VROOT) && scale < (double)wh / (double)WinGetH(VROOT))
      scale = (double)wh / (double)WinGetH(VROOT);
   sw = (int)((ww + .999 * scale) / scale);
   sh = (int)((wh + .999 * scale) / scale);
   sx = mw->cx - sw / 2;
   sy = mw->cy - sh / 2;
   if (sx < 0)
      sx = 0;
   else if (sx + sw > WinGetW(VROOT))
      sx = WinGetW(VROOT) - sw;
   if (sy < 0)
      sy = 0;
   else if (sy + sh > WinGetH(VROOT))
      sy = WinGetH(VROOT) - sh;

   mw->sx = sx;
   mw->sy = sy;
   mw->sw = sw;
   mw->sh = sh;

   if (paint)
     {
	int                 dw, dh;

	dw = (int)(sw * scale + .5);
	dh = (int)(sh * scale + .5);
	draw = ECompMgrGetRootBuffer();
	if (draw == None)
	   draw = WinGetXwin(VROOT);
	ScaleRect(VROOT, draw, EwinGetClientWin(mw->ewin),
		  EwinGetClientXwin(mw->ewin), sx, sy, sw, sh,
		  0, 0, dw, dh, (mw->filter) ? EIMAGE_ANTI_ALIAS : 0);
     }

   /* Check if pointer is in magnifier window */
   EQueryPointer(EwinGetClientWin(mw->ewin), &px, &py, NULL, NULL);
   out = px < 0 || px >= mw->ewin->client.w ||
      py < 0 || py >= mw->ewin->client.h;
   /* If inside grab pixel before drawing in window */
   pixel = (out) ? 0 : MagwinGetPixel(EwinGetClientXwin(mw->ewin), px, py);

   /* Show magnified area coordinates */
   Esnprintf(buf, sizeof(buf), "%d,%d %dx%d", sx, sy, sw, sh);
   MagwinDrawText(mw, 10, 10, buf);

   if (out)
      return;

   /* Show info about pixel at cursor (if in magnifier) */
   qx = (int)(px / scale);
   qy = (int)(py / scale);
   if (qx > WinGetW(VROOT) - 1)
      qx = WinGetW(VROOT) - 1;
   if (qy > WinGetH(VROOT) - 1)
      qy = WinGetH(VROOT) - 1;
   Esnprintf(buf, sizeof(buf), "%d,%d: pixel=%#08x", sx + qx, sy + qy, pixel);
   MagwinDrawText(mw, 10, 20, buf);
}

static int
_MagwinUpdate(MagWindow * mw)
{
   if (mw != MagWin)
      return 0;

   /* Validate ewin */
   if (!EwinFindByPtr(mw->ewin))
      return 0;

   if (!mw->update && Mode.events.damage_count == mw->damage_count)
      return 1;
   mw->damage_count = Mode.events.damage_count;

   /* FIXME - Check damage */

   MagwinRedraw(mw, 1);

   mw->update = 0;

   return 1;
}

#if USE_TIMER
static int
_MagwinTimeout(int val __UNUSED__, void *data)
{
   MagWindow          *mw = (MagWindow *) data;
   int                 again;

   again = _MagwinUpdate(mw);
   if (again)
      return 1;

   mw->timer = NULL;
   return 0;
}
#elif USE_ANIMATOR
static int
_MagwinAnimator(void *data)
{
   MagWindow          *mw = (MagWindow *) data;

   return _MagwinUpdate(mw);
}
#endif

static int
MagwinKeyPress(MagWindow * mw, KeySym key)
{
   switch (key)
     {
     case XK_q:		/* Quit */
     case XK_Escape:
	return 1;
     case XK_g:		/* Toggle grabs */
	if (mw->grabbing)
	  {
	     GrabPointerRelease();
	     GrabKeyboardRelease();
	     mw->grabbing = 0;
	  }
	else
	  {
	     GrabPointerSet(EwinGetClientWin(mw->ewin), ECSR_GRAB, 0);
	     GrabKeyboardSet(EwinGetClientWin(mw->ewin));
	     mw->grabbing = 1;
	  }
	break;
     case XK_f:		/* Toggle filter */
	mw->filter += 1;
	if (mw->filter >= 2)
	   mw->filter = 0;
	break;
     case XK_i:		/* Zoom in */
     case XK_Page_Up:
	mw->scale += 1;
	break;
     case XK_o:		/* Zoom out */
     case XK_Page_Down:
	mw->scale -= 1;
	if (mw->scale < -20)
	   mw->scale = -20;
	break;

     case XK_Left:
	mw->cx -= mw->step;
	if (mw->cx < mw->sw / 2)
	   mw->cx = mw->sw / 2;
	break;
     case XK_Right:
	mw->cx += mw->step;
	if (mw->cx > WinGetW(VROOT) - mw->sw / 2)
	   mw->cx = WinGetW(VROOT) - mw->sw / 2;
	break;
     case XK_Up:
	mw->cy -= mw->step;
	if (mw->cy < mw->sh / 2)
	   mw->cy = mw->sh / 2;
	break;
     case XK_Down:
	mw->cy += mw->step;
	if (mw->cy > WinGetH(VROOT) - mw->sh / 2)
	   mw->cy = WinGetH(VROOT) - mw->sh / 2;
	break;

     case XK_r:		/* Switch render mode */
	Conf.testing.use_render_for_scaling =
	   !Conf.testing.use_render_for_scaling;
	break;

     case XK_s:		/* x/y move step size */
	mw->step = (mw->step == 1) ? 4 : 1;
	break;
     }

   return 0;
}

static void
MagwinEvent(Win win __UNUSED__, XEvent * ev, void *prm)
{
   MagWindow          *mw = (MagWindow *) prm;
   KeySym              key;
   int                 done = 0;

   switch (ev->type)
     {
     default:
	break;

     case KeyPress:
	key = XLookupKeysym(&ev->xkey, 0);
	done = MagwinKeyPress(mw, key);
	mw->update = 1;
	break;

     case ButtonPress:
	switch (ev->xbutton.button)
	  {
	  default:
	     break;
	  case 1:
	     MagwinKeyPress(mw, XK_g);
	     break;
	  case 3:
	     MagwinKeyPress(mw, XK_f);
	     break;
	  case 4:
	     MagwinKeyPress(mw, XK_i);
	     break;
	  case 5:
	     MagwinKeyPress(mw, XK_o);
	     break;
	  }
	mw->bpress = 1;
	mw->update = 1;
	break;
     case ButtonRelease:
	mw->bpress = 0;
	break;

     case MotionNotify:
	if (mw->grabbing)
	  {
	     mw->cx = Mode.events.mx;
	     mw->cy = Mode.events.my;
	     mw->update = 1;
	  }
	else
	  {
	     mw->update = 1;
	  }
	break;

     case MapNotify:
	MagwinKeyPress(mw, XK_g);
#if USE_TIMER
	TIMER_ADD(mw->timer, .050, _MagwinTimeout, 0, mw);
#elif USE_ANIMATOR
	AnimatorAdd(_MagwinAnimator, mw);
#endif
	mw->update = 1;
	break;
     }

   if (done)
      EwinHide(mw->ewin);
}

static void
_MagEwinInit(EWin * ewin)
{
   MagWindow          *mw = (MagWindow *) ewin->data;

   EwinSetTitle(ewin, mw->title);
   EwinSetClass(ewin, "Magnifier", "Enlightenment_Magnifier");

   EoSetSticky(ewin, 1);
   EoSetShadow(ewin, 0);
}

static void
_MagEwinClose(EWin * ewin)
{
   MagwinDestroy((MagWindow *) ewin->data);
   ewin->data = NULL;
   MagWin = NULL;
}

static const EWinOps _MagEwinOps = {
   _MagEwinInit,
   NULL,
   NULL,
   _MagEwinClose,
};

static MagWindow   *
MagwinCreate(const char *title, int width, int height)
{
   MagWindow          *mw;
   Win                 win;
   int                 x, y, w, h;

   mw = ECALLOC(MagWindow, 1);
   if (!mw)
      return NULL;

   win = VROOT;
   w = width;
   h = height;
   x = ((win->w - w) / 2);
   y = ((win->h - h) / 2);

   win = ECreateClientWindow(VROOT, x, y, w, h);

   mw->title = title;
   mw->ewin = AddInternalToFamily(win, NULL, EWIN_TYPE_MISC, &_MagEwinOps, mw);
   if (!mw->ewin)
     {
	Efree(mw);
	return NULL;
     }

   mw->ewin->o.ghost = 1;
   EoSetLayer(mw->ewin, 10);
   EwinMoveToDesktop(mw->ewin, EoGetDesk(mw->ewin));
   EwinMoveResize(mw->ewin, EoGetX(mw->ewin), EoGetY(mw->ewin), w, h);

   mw->ewin->client.event_mask |=
      KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
      StructureNotifyMask;
   ESelectInput(win, mw->ewin->client.event_mask);

   EventCallbackRegister(win, 0, MagwinEvent, mw);

   EQueryPointer(VROOT, &mw->cx, &mw->cy, NULL, NULL);
   mw->scale = 1;
   mw->step = 4;

   return mw;
}

static void
MagwinDestroy(MagWindow * mw)
{
#if USE_TIMER
   TIMER_DEL(mw->timer);
#endif
   EventCallbackUnregister(EwinGetClientWin(mw->ewin), 0, MagwinEvent, mw);
   EDestroyWindow(EwinGetClientWin(mw->ewin));
   Efree(mw);
}

static void
MagwinShow(void)
{
   if (MagWin)
      return;

   MagWin = MagwinCreate(_("Magnifier"),
			 WinGetW(VROOT) / 4, WinGetH(VROOT) / 4);
   if (!MagWin)
     {
	Eprintf("Failed to create magnifier window\n");
	return;
     }

   EwinShow(MagWin->ewin);
}

/*
 * MagWin Module
 */

static void
MagwinIpc(const char *params)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 len;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || !strcmp(cmd, "show"))
     {
	MagwinShow();
     }
}

static const IpcItem MagwinIpcArray[] = {
   {
    MagwinIpc,
    "magwin", "mag",
    "Magnifier functions",
    "  magwin show\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(MagwinIpcArray)/sizeof(IpcItem))

/*
 * Module descriptor
 */
extern const EModule ModMagwin;
const EModule       ModMagwin = {
   "magwin", NULL,
   NULL,
   {N_IPC_FUNCS, MagwinIpcArray},
   {0, NULL}
};
