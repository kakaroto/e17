/*
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
#include "desktops.h"
#include "e16-ecore_hints.h"
#include "ecompmgr.h"
#include "eobj.h"
#include "ewins.h"		/* FIXME - Should not be here */
#include "hints.h"
#include "xwin.h"

int
OpacityFix(int op, int op_0)
{
   if (op <= 0)
      op = op_0;
   else if (op > 255)
      op = 100;
   else if (op > 100)		/* Hack to convert old 0-255 range */
      op = (100 * op) / 255;
   return op;
}

unsigned int
OpacityFromPercent(int opx)
{
   unsigned int        op = (unsigned int)opx;

   /* op is 0-100, extend to 32 bit */
   /* op <= 0 and op > 100 is mapped to 100 (opaque) */
   if (op >= 100)
      return 0xffffffff;
   return op * 42949672;
}

int
OpacityToPercent(unsigned int opacity)
{
   return (int)(opacity / 42949672);
}

void
EobjSetLayer(EObj * eo, int layer)
{
   int                 ilayer = eo->ilayer;

   eo->layer = layer;
   /*
    * For usual EWin's the internal layer is the "old" E-layer * 10.
    *
    * Internal layers:
    *   0: Root
    *   3: Desktop type apps
    *   5: Below buttons
    *  10: Lowest ewins
    *  15: Normal buttons
    *  20: Normal below ewins
    *  40: Normal ewins
    *  60: Above ewins
    *  75: Above buttons
    *  80: Ontop ewins
    * 100: E-Dialogs
    * 512-: Floating windows
    * + 0: Virtual desktops
    * +30: E-Menus
    * +40: Override redirects
    * +40: E-Tooltips
    */

   switch (eo->type)
     {
     case EOBJ_TYPE_EWIN:
	eo->ilayer = 10 * eo->layer;
	if (eo->ilayer == 0)
	   eo->ilayer = 3;
	break;

     case EOBJ_TYPE_BUTTON:
	if (eo->layer > 0)
	   eo->ilayer = 75;	/* Ontop */
	else if (eo->layer == 0)
	   eo->ilayer = 15;	/* Normal */
	else if (eo->layer < 0)
	   eo->ilayer = 5;	/* Below */
	if (eo->layer > 0 && eo->sticky)
	   eo->floating = 1;
	break;

     default:
	eo->ilayer = 10 * eo->layer;
	break;
     }

   if (eo->floating)
      eo->ilayer |= 512;
   else
      eo->ilayer &= ~512;
   if (eo->ghost)
      eo->ilayer |= 1024;

   if (eo->ilayer != ilayer)
      EobjRaise(eo);
}

void
EobjSetFloating(EObj * eo, int floating)
{
   if (floating == eo->floating)
      return;

#if 0
   switch (eo->type)
     {
     default:
	break;
     case EOBJ_TYPE_EWIN:
	if (floating > 1)
	   eo->desk = 0;
	break;
     }
#endif

   eo->floating = floating;
   EobjSetLayer(eo, eo->layer);
}

#if 1				/* FIXME - Remove */
int
EobjIsShaped(const EObj * eo)
{
   switch (eo->type)
     {
     default:
	return 0;		/* FIXME */
     case EOBJ_TYPE_EWIN:
	return ((EWin *) eo)->state.shaped;
     }
}
#endif

#if USE_GLX
#define WINTYPE(t) ((t == EOBJ_TYPE_GLX) ? WIN_TYPE_GLX : WIN_TYPE_INTERNAL)
#else
#define WINTYPE(t) WIN_TYPE_INTERNAL
#endif

void
EobjInit(EObj * eo, int type, Win win, int x, int y, int w, int h,
	 int su, const char *name)
{
   if (!eo->desk)
      eo->desk = DeskGet(0);

   if (!win)
     {
	if (type == EOBJ_TYPE_EVENT)
	  {
	     win = ECreateEventWindow(VROOT, x, y, w, h);
	     eo->inputonly = 1;
	  }
	else
	  {
	     win = ECreateObjectWindow(EoGetWin(eo->desk), x, y, w, h, su,
				       WINTYPE(type), NULL);
	  }
     }
   eo->type = type;
   eo->win = win;
   eo->shaped = -1;
   if (!win)
      return;

   if (type == EOBJ_TYPE_EXT)
     {
	eo->icccm.wm_name = ecore_x_icccm_title_get(WinGetXwin(win));
	ecore_x_icccm_name_class_get(WinGetXwin(win),
				     &eo->icccm.wm_res_name,
				     &eo->icccm.wm_res_class);
     }
   else if (name)
      eo->icccm.wm_name = Estrdup(name);
   if (!eo->icccm.wm_name)
      eo->icccm.wm_name = Estrdup("-?-");

   if (type != EOBJ_TYPE_EWIN && type != EOBJ_TYPE_EXT)
      HintsSetWindowName(eo->win, eo->icccm.wm_name);

#if USE_COMPOSITE
   switch (type)
     {
     case EOBJ_TYPE_EVENT:
     case EOBJ_TYPE_MISC_NR:
     case EOBJ_TYPE_ROOT_BG:
	eo->noredir = 1;
	break;
     }
   ECompMgrWinNew(eo);
#endif
   if (EobjGetXwin(eo) != WinGetXwin(VROOT))
      EobjListStackAdd(eo, 1);

   if (EDebug(EDBUG_TYPE_EWINS))
      Eprintf("EobjInit: %#lx %s\n", EobjGetXwin(eo), EobjGetName(eo));
}

void
EobjFini(EObj * eo)
{
   if (EDebug(EDBUG_TYPE_EWINS))
      Eprintf("EobjFini: %#lx %s\n", EobjGetXwin(eo), EobjGetName(eo));

   EobjListStackDel(eo);

#if USE_COMPOSITE
   if (!eo->external)
      eo->gone = 1;		/* Actually not yet (but soon) */

   if (eo->cmhook)
      ECompMgrWinDel(eo);
#endif

   if (eo->external)
      EUnregisterWindow(eo->win);
   else
      EDestroyWindow(eo->win);

   Efree(eo->icccm.wm_name);
   Efree(eo->icccm.wm_res_name);
   Efree(eo->icccm.wm_res_class);
}

void
EobjDestroy(EObj * eo)
{
   if (EDebug(EDBUG_TYPE_EWINS))
      Eprintf("EobjDestroy: %#lx %s\n", EobjGetXwin(eo), EobjGetName(eo));

   EobjFini(eo);

   Efree(eo);
}

EObj               *
EobjWindowCreate(int type, int x, int y, int w, int h, int su, const char *name)
{
   EObj               *eo;

   eo = ECALLOC(EObj, 1);

   eo->floating = 1;
   EobjSetLayer(eo, 20);
   EobjInit(eo, type, eo->win, x, y, w, h, su, name);
   if (eo->win == None)
     {
	Efree(eo);
	eo = NULL;
     }

   return eo;
}

void
EobjWindowDestroy(EObj * eo)
{
   EobjDestroy(eo);
}

EObj               *
EobjRegister(Window xwin, int type)
{
   EObj               *eo;
   XWindowAttributes   attr;
   Win                 win;

   eo = EobjListStackFind(xwin);
   if (eo)
      return eo;

   if (!XGetWindowAttributes(disp, xwin, &attr))
      return NULL;

   if (type == EOBJ_TYPE_EXT && !attr.override_redirect)
      return NULL;

   win = ERegisterWindow(xwin, &attr);
   if (!win)
      return NULL;

   eo = ECALLOC(EObj, 1);
   if (!eo)
      return eo;

#if __cplusplus
   if (attr.c_class == InputOnly)
#else
   if (attr.class == InputOnly)
#endif
      eo->inputonly = 1;

   eo->external = 1;
   eo->fade = 1;
   eo->shadow = 1;

   EobjInit(eo, type, win, attr.x, attr.y, attr.width, attr.height, 0, NULL);

#if 1				/* FIXME - TBD */
   if (type == EOBJ_TYPE_EXT)
     {
	eo->shaped = 0;		/* FIXME - Assume unshaped for now */
	EobjSetFloating(eo, 1);
	EobjSetLayer(eo, 4);
     }
#endif
#if 0
   Eprintf("EobjRegister: %#lx type=%d or=%d: %s\n", xwin, type,
	   attr.override_redirect, EobjGetName(eo));
#endif

   return eo;
}

void
EobjUnregister(EObj * eo)
{
#if 0
   Eprintf("EobjUnregister: %#lx type=%d: %s\n", eo->win, eo->type,
	   EobjGetName(eo));
#endif
   EobjDestroy(eo);
}

void
EobjMap(EObj * eo, int raise)
{
   if (eo->shown)
      return;
   eo->shown = 1;

   if (raise)
      EobjListStackRaise(eo, 0);

   if (eo->stacked <= 0 || raise > 1)
      DeskRestack(eo->desk);

   if (eo->shaped < 0)
      EobjShapeUpdate(eo, 0);

   EMapWindow(eo->win);
#if USE_COMPOSITE
   ECompMgrWinMap(eo);
#endif
}

void
EobjUnmap(EObj * eo)
{
   if (!eo->shown)
      return;

#if USE_COMPOSITE
   if (eo->cmhook)
      ECompMgrWinUnmap(eo);
#endif
   EUnmapWindow(eo->win);
   eo->shown = 0;
}

void
EobjMoveResize(EObj * eo, int x, int y, int w, int h)
{
   int                 move, resize;

   move = x != EobjGetX(eo) || y != EobjGetY(eo);
   resize = w != EobjGetW(eo) || h != EobjGetH(eo);
#if USE_COMPOSITE
   if (eo->type == EOBJ_TYPE_EWIN)
     {
	ECompMgrMoveResizeFix(eo, x, y, w, h);
     }
   else
#endif
     {
	EMoveResizeWindow(eo->win, x, y, w, h);
     }
#if USE_COMPOSITE
   if (eo->cmhook)
      ECompMgrWinMoveResize(eo, move, resize, 0);
#endif
}

void
EobjMove(EObj * eo, int x, int y)
{
   EobjMoveResize(eo, x, y, EobjGetW(eo), EobjGetH(eo));
}

void
EobjResize(EObj * eo, int w, int h)
{
   EobjMoveResize(eo, EobjGetX(eo), EobjGetY(eo), w, h);
}

void
EobjDamage(EObj * eo)
{
#if USE_COMPOSITE
   if (eo->cmhook)
      ECompMgrWinDamageArea(eo, 0, 0, 0, 0);
#else
   eo = NULL;
#endif
}

void
EobjReparent(EObj * eo, EObj * dst, int x, int y)
{
   int                 move;

   move = x != EobjGetX(eo) || y != EobjGetY(eo);

   EReparentWindow(eo->win, dst->win, x, y);
   if (dst->type == EOBJ_TYPE_DESK)
     {
	Desk               *dsk = (Desk *) dst;

	if (eo->stacked < 0)
	  {
	     eo->desk = NULL;
	     eo->stacked = 0;
	  }
	if (eo->desk != dsk)
	   DeskSetDirtyStack(dsk, eo);
#if USE_COMPOSITE
	if (eo->cmhook)
	   ECompMgrWinReparent(eo, dsk, move);
#endif
	eo->desk = dsk;
     }
   else
     {
	EobjListStackDel(eo);
#if USE_COMPOSITE
	if (eo->cmhook)
	   ECompMgrWinDel(eo);
#endif
     }
}

int
EobjRaise(EObj * eo)
{
#if USE_COMPOSITE
   int                 num;

   num = EobjListStackRaise(eo, 1);
   if (num == 0)
      return num;

   if (num < 0)
      num = EobjListStackRaise(eo, 0);
   if (eo->shown && eo->cmhook)
      ECompMgrWinRaiseLower(eo, num);
   if (num > 0)
      num = EobjListStackRaise(eo, 0);

   return num;
#else
   return EobjListStackRaise(eo, 0);
#endif
}

int
EobjLower(EObj * eo)
{
#if USE_COMPOSITE
   int                 num;

   num = EobjListStackLower(eo, 1);
   if (num == 0)
      return num;

   if (num < 0)
      num = EobjListStackLower(eo, 0);
   if (eo->shown && eo->cmhook)
      ECompMgrWinRaiseLower(eo, num);
   if (num > 0)
      num = EobjListStackLower(eo, 0);

   return num;
#else
   return EobjListStackLower(eo, 0);
#endif
}

void
EobjShapeUpdate(EObj * eo, int propagate)
{
#if USE_COMPOSITE
   int                 was_shaped = eo->shaped;
#endif

   if (propagate)
      eo->shaped = EShapePropagate(eo->win) != 0;
   else
      eo->shaped = EShapeCheck(eo->win) != 0;

#if USE_COMPOSITE
   if (was_shaped <= 0 && eo->shaped <= 0)
      return;

   /* Shape may still be unchanged. Well ... */
   if (eo->shown && eo->cmhook)
      ECompMgrWinChangeShape(eo);
#endif
}

Pixmap
EobjGetPixmap(const EObj * eo)
{
   Pixmap              pmap = None;

#if USE_COMPOSITE
   pmap = ECompMgrWinGetPixmap(eo);
#else
   eo = NULL;
#endif
   return pmap;
}

void
EobjChangeOpacity(EObj * eo, unsigned int opacity)
{
#if USE_COMPOSITE
   if (eo->opacity == opacity)
      return;
   eo->opacity = opacity;
   ECompMgrWinChangeOpacity(eo, opacity);
#else
   eo = NULL;
   opacity = 0;
#endif
}

#if USE_COMPOSITE
void
EobjChangeShadow(EObj * eo, int shadow)
{
   ECompMgrWinChangeShadow(eo, shadow);
}
#endif

void
EobjSlideTo(EObj * eo, int fx, int fy, int tx, int ty, int speed)
{
   int                 k, x, y;

   EGrabServer();

   ETimedLoopInit(0, 1024, speed);
   for (k = 0; k <= 1024;)
     {
	x = ((fx * (1024 - k)) + (tx * k)) >> 10;
	y = ((fy * (1024 - k)) + (ty * k)) >> 10;
	EobjMove(eo, x, y);

	k = ETimedLoopNext();
     }
   EobjMove(eo, tx, ty);

   EUngrabServer();
}

void
EobjsSlideBy(EObj ** peo, int num, int dx, int dy, int speed)
{
   int                 i, k, x, y;
   struct _xy {
      int                 x, y;
   }                  *xy;

   if (num <= 0)
      return;

   xy = EMALLOC(struct _xy, num);
   if (!xy)
      return;

   for (i = 0; i < num; i++)
     {
	xy[i].x = EobjGetX(peo[i]);
	xy[i].y = EobjGetY(peo[i]);
     }

   ETimedLoopInit(0, 1024, speed);
   for (k = 0; k <= 1024;)
     {
	for (i = 0; i < num; i++)
	  {
	     x = ((xy[i].x * (1024 - k)) + ((xy[i].x + dx) * k)) >> 10;
	     y = ((xy[i].y * (1024 - k)) + ((xy[i].y + dy) * k)) >> 10;
	     EobjMove(peo[i], x, y);
	  }

	k = ETimedLoopNext();
     }

   for (i = 0; i < num; i++)
      EobjMove(peo[i], xy[i].x + dx, xy[i].y + dy);

   Efree(xy);
}

void
EobjSlideSizeTo(EObj * eo, int fx, int fy, int tx, int ty, int fw, int fh,
		int tw, int th, int speed)
{
   int                 k, x, y, w, h;

   ETimedLoopInit(0, 1024, speed);
   for (k = 0; k <= 1024;)
     {
	x = ((fx * (1024 - k)) + (tx * k)) >> 10;
	y = ((fy * (1024 - k)) + (ty * k)) >> 10;
	w = ((fw * (1024 - k)) + (tw * k)) >> 10;
	h = ((fh * (1024 - k)) + (th * k)) >> 10;
	EobjMoveResize(eo, x, y, w, h);

	k = ETimedLoopNext();
     }
   EobjMoveResize(eo, tx, ty, tw, th);
}

void
EobjsRepaint(void)
{
#if USE_COMPOSITE
   if (ECompMgrIsActive())
      ECompMgrRepaint();
   else
#endif
      ESync(0);
}

#if USE_COMPOSITE
void
EobjsOpacityUpdate(int op_or)
{
   EObj               *eo, *const *lst;
   int                 i, num;

   lst = EobjListStackGet(&num);
   for (i = 0; i < num; i++)
     {
	eo = lst[i];
	switch (eo->type)
	  {
	  default:
	     break;
	  case EOBJ_TYPE_EWIN:
	     EwinUpdateOpacity((EWin *) eo);
	     break;
	  case EOBJ_TYPE_EXT:
	     EobjChangeOpacity(eo, OpacityFromPercent(op_or));
	     break;
	  }
     }
}
#endif
