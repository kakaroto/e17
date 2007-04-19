/*
 * Copyright (C) 2007 Kim Woelders
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
typedef struct
{
   EWin               *ewin;
   EImage             *im;
   int                 cx, cy;	/* Center */
   int                 scale;	/* Zoom level */
   int                 sx, sy;	/* Scene x,y */
   int                 sw, sh;	/* Scene wxh */
   char                mode;
   char                bpress;
   char                filter;
   char                grabbing;
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
   unsigned int       *pd;

   ww = mw->ewin->client.w;
   wh = mw->ewin->client.h;

   if (mw->scale < -6)
      mw->scale = -6;
   else if (mw->scale > 6)
      mw->scale = 6;
   scale = pow(2., (double)(mw->scale));
   sw = (int)((ww + .999 * scale) / scale);
   if (sw > VRoot.w)
      scale = (double)ww / (double)VRoot.w;
   sh = (int)((wh + .999 * scale) / scale);
   if (sh > VRoot.h && scale < (double)wh / (double)VRoot.h)
      scale = (double)wh / (double)VRoot.h;
   sw = (int)((ww + .999 * scale) / scale);
   sh = (int)((wh + .999 * scale) / scale);
   sx = mw->cx - sw / 2;
   sy = mw->cy - sh / 2;
   if (sx < 0)
      sx = 0;
   else if (sx + sw > VRoot.w)
      sx = VRoot.w - sw;
   if (sy < 0)
      sy = 0;
   else if (sy + sh > VRoot.h)
      sy = VRoot.h - sh;

   mw->sx = sx;
   mw->sy = sy;
   mw->sw = sw;
   mw->sh = sh;

   if (paint)
     {
	if (mw->im)
	   EImageDecache(mw->im);

	draw = (VRoot.pmap != None) ? VRoot.pmap : VRoot.xwin;
	mw->im = EImageGrabDrawable(draw, None, sx, sy, sw, sh, 0);
	EImageRenderOnDrawable(mw->im, EwinGetClientWin(mw->ewin),
			       EwinGetClientXwin(mw->ewin),
			       (mw->filter) ? EIMAGE_ANTI_ALIAS : 0,
			       0, 0, (int)(sw * scale + .5),
			       (int)(sh * scale + .5));
     }

   /* Show magnified area coordinates */
   Esnprintf(buf, sizeof(buf), "%d,%d %dx%d", sx, sy, sw, sh);
   MagwinDrawText(mw, 10, 10, buf);

   /* Show info about pixel at cursor (if in magnifier) */
   EQueryPointer(EwinGetClientWin(mw->ewin), &px, &py, NULL, NULL);
   if (px < 0 || px >= mw->ewin->client.w || py < 0 || py >= mw->ewin->client.h)
      return;
   qx = (int)(px / scale);
   qy = (int)(py / scale);
   if (qx > VRoot.w - 1)
      qx = VRoot.w - 1;
   if (qy > VRoot.h - 1)
      qy = VRoot.h - 1;
   if (!mw->im)
      return;
   pd = (unsigned int *)EImageGetData(mw->im);
#if 0
   Esnprintf(buf, sizeof(buf), "%d,%d: pixel=%#08x (%d,%d)",
	     sx + qx, sy + qy, pd[qy * sw + qx], px, py);
#else
   Esnprintf(buf, sizeof(buf), "%d,%d: pixel=%#08x",
	     sx + qx, sy + qy, pd[qy * sw + qx]);
#endif
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
static void
_MagwinTimeout(int val __UNUSED__, void *data)
{
   MagWindow          *mw = (MagWindow *) data;
   int                 again;

   again = _MagwinUpdate(mw);
   if (!again)
      return;

   DoIn("magwin", .050, _MagwinTimeout, 0, data);
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
	mw->cx -= 4;
	if (mw->cx < mw->sw / 2)
	   mw->cx = mw->sw / 2;
	break;
     case XK_Right:
	mw->cx += 4;
	if (mw->cx > VRoot.w - mw->sw / 2)
	   mw->cx = VRoot.w - mw->sw / 2;
	break;
     case XK_Up:
	mw->cy -= 4;
	if (mw->cy < mw->sh / 2)
	   mw->cy = mw->sh / 2;
	break;
     case XK_Down:
	mw->cy += 4;
	if (mw->cy > VRoot.h - mw->sh / 2)
	   mw->cy = VRoot.h - mw->sh / 2;
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
	     mw->cx = Mode.events.x;
	     mw->cy = Mode.events.y;
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
	_MagwinTimeout(1, mw);
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
   EoSetSticky(ewin, 1);
   EoSetShadow(ewin, 0);
}

static void
_MagEwinClose(EWin * ewin)
{
   MagwinDestroy((MagWindow *) ewin->data);
   ewin->client.win = NULL;
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

   win = VRoot.win;
   w = width;
   h = height;
   x = ((win->w - w) / 2);
   y = ((win->h - h) / 2);

   win = ECreateClientWindow(VRoot.win, x, y, w, h);

   HintsSetWindowName(win, title);
   HintsSetWindowClass(win, "Magnifier", "Enlightenment_Magnifier");

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

   EQueryPointer(VRoot.win, &mw->cx, &mw->cy, NULL, NULL);
   mw->scale = 1;

   return mw;
}

static void
MagwinDestroy(MagWindow * mw)
{
#if USE_TIMER
   RemoveTimerEvent("magwin");
#endif
   EventCallbackUnregister(EwinGetClientWin(mw->ewin), 0, MagwinEvent, mw);
   EDestroyWindow(EwinGetClientWin(mw->ewin));
   if (mw->im)
      EImageDecache(mw->im);
   Efree(mw);
}

static void
MagwinShow(void)
{
   if (MagWin)
      return;

   MagWin = MagwinCreate("Magnifier", VRoot.w / 4, VRoot.h / 4);
   if (!MagWin)
     {
	Eprintf("Failed to create magnifier window\n");
	return;
     }

   EwinShow(MagWin->ewin);
}

#if 0				/* FIXME - Remove? */
static void
MagwinHide(void)
{
   if (!MagWin)
      return;

   EwinHide(MagWin->ewin);
}
#endif

/*
 * MagWin Module
 */

static void
MagwinIpc(const char *params, Client * c __UNUSED__)
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
#if 0				/* FIXME - Remove? */
   else if (!strcmp(cmd, "hide"))
     {
	MagwinHide();
     }
#endif
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
