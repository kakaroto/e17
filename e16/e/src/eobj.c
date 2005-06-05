/*
 * Copyright (C) 2004-2005 Kim Woelders
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
#include "ecompmgr.h"
#include "ecore-e16.h"

void
EobjSetDesk(EObj * eo, int desk)
{
   switch (eo->type)
     {
     default:
	break;

     case EOBJ_TYPE_EWIN:
#if 0
	if (eo->floating > 1)
	   desk = 0;
	else if (eo->sticky || eo->desk < 0)
	   desk = DesksGetCurrent();
	else
#endif
	   desk = desk % Conf.desks.num;
	break;
     }

   if (desk != eo->desk)
     {
	if (eo->stacked > 0)
	   DeskSetDirtyStack(desk);
	eo->desk = desk;
     }
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

int
EobjIsShaped(const EObj * eo)
{
   switch (eo->type)
     {
     default:
	return 0;		/* FIXME */
     case EOBJ_TYPE_EWIN:
	return ((EWin *) eo)->client.shaped;
     }
}

void
EobjInit(EObj * eo, int type, Window win, int x, int y, int w, int h,
	 const char *name)
{
   eo->type = type;
   eo->win = win;
   eo->x = x;
   eo->y = y;
   eo->w = w;
   eo->h = h;
   if (name)
      eo->name = Estrdup(name);
#if USE_COMPOSITE
   if (eo->opacity == 0)
      eo->opacity = 0xFFFFFFFF;
   eo->shadow = 1;
   ECompMgrWinNew(eo);
#endif
   if (eo->win != VRoot.win)
      EobjListStackAdd(eo, 1);

   if (EventDebug(EDBUG_TYPE_EWINS))
      Eprintf("EobjInit: %#lx %s\n", eo->win, eo->name);
}

void
EobjFini(EObj * eo)
{
   if (EventDebug(EDBUG_TYPE_EWINS))
      Eprintf("EobjFini: %#lx %s\n", eo->win, eo->name);

#if USE_COMPOSITE
   ECompMgrWinDel(eo);
#endif

   EobjListStackDel(eo);

   if (eo->name)
      Efree(eo->name);
}

void
EobjDestroy(EObj * eo)
{
   if (EventDebug(EDBUG_TYPE_EWINS))
      Eprintf("EobjDestroy: %#lx %s\n", eo->win, eo->name);

   EobjFini(eo);

   Efree(eo);
}

EObj               *
EobjWindowCreate(int type, int x, int y, int w, int h, int su, const char *name)
{
   EObj               *eo;

   eo = Ecalloc(1, sizeof(EObj));

   if (type == EOBJ_TYPE_EVENT)
      eo->win = ECreateEventWindow(VRoot.win, x, y, w, h);
   else
      eo->win = ECreateWindow(VRoot.win, x, y, w, h, su);
   if (eo->win == None)
     {
	Efree(eo);
	return NULL;
     }

   eo->floating = 1;
   EobjSetLayer(eo, 20);
   EobjInit(eo, type, eo->win, x, y, w, h, name);

   return eo;
}

void
EobjWindowDestroy(EObj * eo)
{
   EDestroyWindow(eo->win);
   eo->gone = 1;
   EobjDestroy(eo);
}

EObj               *
EobjRegister(Window win, int type)
{
   EObj               *eo;
   XWindowAttributes   attr;

   eo = EobjListStackFind(win);
   if (eo)
      return eo;

   if (!XGetWindowAttributes(disp, win, &attr))
      return NULL;

   eo = Ecalloc(1, sizeof(EObj));
   if (!eo)
      return eo;

   EobjInit(eo, type, win, attr.x, attr.y, attr.width, attr.height, NULL);
   eo->name = ecore_x_icccm_title_get(win);

#if 1				/* FIXME - TBD */
   if (type == EOBJ_TYPE_EXT)
     {
	EobjSetFloating(eo, 1);
	EobjSetLayer(eo, 4);
     }
#endif

   return eo;
}

void
EobjUnregister(EObj * eo)
{
   EobjDestroy(eo);
}

void
EobjMap(EObj * eo, int raise)
{
   if (eo->shown)
      return;
   eo->shown = 1;

   if (raise)
      EobjListStackRaise(eo);

   if (eo->stacked <= 0)
     {
	if (eo->stacked < 0)
	   DeskSetDirtyStack(eo->desk);
	StackDesktop(eo->desk);
     }

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

   EUnmapWindow(eo->win);
#if USE_COMPOSITE
   if (eo->cmhook)
      ECompMgrWinUnmap(eo);
#endif
   eo->shown = 0;
}

void
EobjMoveResize(EObj * eo, int x, int y, int w, int h)
{
   int                 move, resize;

   move = x != eo->x || y != eo->y;
   resize = w != eo->w || h != eo->h;
   eo->x = x;
   eo->y = y;
   eo->w = w;
   eo->h = h;
#if USE_COMPOSITE
   if (eo->type == EOBJ_TYPE_EWIN)
     {
	if (EventDebug(250))
	   EDrawableDumpImage(eo->win, "Win1");
	ECompMgrMoveResizeFix(eo, x, y, w, h);
	if (EventDebug(250))
	   EDrawableDumpImage(eo->win, "Win2");
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
   EobjMoveResize(eo, x, y, eo->w, eo->h);
}

void
EobjResize(EObj * eo, int w, int h)
{
   EobjMoveResize(eo, eo->x, eo->y, w, h);
}

void
EobjReparent(EObj * eo, int desk, int x, int y)
{
   Desk               *d;
   int                 move;

   d = DeskGet(desk);
   if (!d)
      return;

   move = x != eo->x || y != eo->y;
   eo->x = x;
   eo->y = y;

   EReparentWindow(eo->win, EoGetWin(d), x, y);
#if USE_COMPOSITE
   if (eo->shown && eo->cmhook)
      ECompMgrWinReparent(eo, desk, move);
#endif
   EobjSetDesk(eo, desk);
}

int
EobjRaise(EObj * eo)
{
   int                 num;

   num = EobjListStackRaise(eo);
   if (num == 0)
      return num;
#if USE_COMPOSITE
   if (eo->shown && eo->cmhook)
      ECompMgrWinChangeStacking(eo);
#endif
   return num;
}

int
EobjLower(EObj * eo)
{
   int                 num;

   num = EobjListStackLower(eo);
   if (num == 0)
      return num;
#if USE_COMPOSITE
   if (eo->shown && eo->cmhook)
      ECompMgrWinChangeStacking(eo);
#endif
   return num;
}

void
EobjChangeShape(EObj * eo)
{
#if USE_COMPOSITE
   if (eo->shown && eo->cmhook)
      ECompMgrWinChangeShape(eo);
#else
   eo = NULL;
#endif
}

#if USE_COMPOSITE
Pixmap
EobjGetPixmap(const EObj * eo)
{
   Pixmap              pmap = None;

   pmap = ECompMgrWinGetPixmap(eo);
   return pmap;
}

void
EobjChangeOpacity(EObj * eo, unsigned int opacity)
{
   eo->opacity = opacity;
   ECompMgrWinChangeOpacity(eo, opacity);
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
   struct _xy
   {
      int                 x, y;
   }                  *xy;

   if (num <= 0)
      return;

   xy = Emalloc(sizeof(struct _xy) * num);
   if (!xy)
      return;

   for (i = 0; i < num; i++)
     {
	xy[i].x = peo[i]->x;
	xy[i].y = peo[i]->y;
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
EobjsRepaint(void)
{
#if USE_COMPOSITE
   ECompMgrRepaint();
#endif
   ESync();
}
