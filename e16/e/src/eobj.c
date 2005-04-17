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

int
EobjSetDesk(EObj * eo, int desk)
{
   switch (eo->type)
     {
     default:
	eo->desk = desk;
	break;
     case EOBJ_TYPE_EWIN:
	if (eo->floating)
	   eo->desk = 0;
	else if (eo->sticky || eo->desk < 0)
	   eo->desk = DesksGetCurrent();
	else
	   eo->desk = desk % Conf.desks.num;
	break;
     case EOBJ_TYPE_BUTTON:
	eo->desk = desk;
	break;
     }

   return eo->desk;
}

void
EobjSetLayer(EObj * eo, int layer)
{
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
     case EOBJ_TYPE_DESK:
     case EOBJ_TYPE_MISC:
     case EOBJ_TYPE_EXT:
	eo->ilayer = 10 * eo->layer;
	break;
     }

   if (eo->floating)
      eo->ilayer |= 512;
   else
      eo->ilayer &= ~512;
}

void
EobjSetFloating(EObj * eo, int floating)
{
   if (floating == eo->floating)
      return;

   switch (eo->type)
     {
     default:
	break;
     case EOBJ_TYPE_EWIN:
	if (floating > 1)
	   eo->desk = 0;
	break;
     }
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

   EobjListStackDel(eo);

   if (eo->name)
      Efree(eo->name);
}

static EObj        *
EobjCreate(Window win, int type)
{
   EObj               *eo;
   XWindowAttributes   attr;

   if (!XGetWindowAttributes(disp, win, &attr))
      return NULL;

   eo = Ecalloc(1, sizeof(EObj));
   if (!eo)
      return eo;

   EobjInit(eo, type, win, attr.x, attr.y, attr.width, attr.height, NULL);
   eo->name = ecore_x_icccm_title_get(win);

   return eo;
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
   EobjDestroy(eo);
}

EObj               *
EobjRegister(Window win, int type)
{
   EObj               *eo;

   eo = EobjListStackFind(win);
   if (eo)
      return eo;

   eo = EobjCreate(win, type);
   if (!eo)
      return eo;

#if 1				/* FIXME - TBD */
   if (type == EOBJ_TYPE_EXT)
     {
	EobjSetFloating(eo, 1);
	EobjSetLayer(eo, 4);
	EobjListStackRaise(eo);
     }
#endif

   return eo;
}

void
EobjUnregister(Window win)
{
   EObj               *eo;

   eo = EobjListStackFind(win);
   if (!eo)
      return;

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

   EMapWindow(eo->win);
}

void
EobjUnmap(EObj * eo)
{
   if (!eo->shown)
      return;
   eo->shown = 0;

   EUnmapWindow(eo->win);
}

void
EobjMoveResize(EObj * eo, int x, int y, int w, int h)
{
   eo->x = x;
   eo->y = y;
   eo->w = w;
   eo->h = h;
   if (eo->type == EOBJ_TYPE_EWIN)
     {
	if (EventDebug(250))
	   EDrawableDumpImage(eo->win, "Win1");
	ExMoveResizeWindow(eo, x, y, w, h);
	if (EventDebug(250))
	   EDrawableDumpImage(eo->win, "Win2");
     }
   else
     {
	EMoveResizeWindow(eo->win, x, y, w, h);
     }
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
