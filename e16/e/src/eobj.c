/*
 * Copyright (C) 2004 Kim Woelders
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

const char         *
EobjGetName(const EObj * eo)
{
   switch (eo->type)
     {
     default:
	return "?";
     case EOBJ_TYPE_EWIN:
	return EwinGetName((EWin *) eo);
     case EOBJ_TYPE_BUTTON:
	return ButtonGetName((Button *) eo);
     case EOBJ_TYPE_OVERR:
	return eo->name;
     }
}

int
EobjGetDesk(const EObj * eo)
{
   return (eo->sticky) ? DesksGetCurrent() : eo->desk;
}

int
EobjSetDesk(EObj * eo, int desk)
{
   switch (eo->type)
     {
     default:
	eo->desk = desk;
	break;
     case EOBJ_TYPE_EWIN:
	if (eo->sticky || eo->desk < 0)
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
    *   0: Desktop (possibly virtual)
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
    * 300: E-Menus
    * 500: Floating ewins
    * TBD: Override redirect
    */

   if (eo->floating)
     {
	eo->ilayer = 500;
	return;
     }

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
     case EOBJ_TYPE_OVERR:
     case EOBJ_TYPE_OTHER:
	eo->ilayer = 10 * eo->layer;
	break;
     }
}

void
EobjSetFloating(EObj * eo, int floating)
{
   if (floating == eo->floating)
      return;

   eo->floating = floating;
   EobjSetLayer(eo, eo->layer);
}

void
EobjInit(EObj * eo, int type, int x, int y, int w, int h)
{
   eo->type = type;
   eo->x = x;
   eo->y = y;
   eo->w = w;
   eo->h = h;
#if USE_COMPOSITE
   eo->opacity = 0xFFFFFFFF;
   eo->shadow = 1;
#endif
}

#if USE_COMPOSITE
EObj               *
EobjCreate(Window win, int type)
{
   EObj               *eo;
   XWindowAttributes   attr;

   if (!XGetWindowAttributes(disp, win, &attr))
      return NULL;
   if (!attr.override_redirect)
      return NULL;

   eo = Ecalloc(1, sizeof(EObj));

   eo->win = win;

   EobjInit(eo, type, attr.x, attr.y, attr.width, attr.height);

   return eo;
}

void
EobjDestroy(EObj * eo)
{
   _EFREE(eo->name);
   Efree(eo);
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

   /* Just for debug */
   eo->name = e16_icccm_name_get(win);

   if (EventDebug(EDBUG_TYPE_EWINS))
      Eprintf("EobjRegister: %#lx %s\n", win, eo->name);

   EobjSetLayer(eo, 80);
   EobjListStackAdd(eo, 1);

   return eo;
}

void
EobjUnregister(Window win)
{
   EObj               *eo;

   eo = EobjListStackFind(win);
   if (!eo)
      return;
   if (eo->type != EOBJ_TYPE_OVERR)
      return;

   if (EventDebug(EDBUG_TYPE_EWINS))
      Eprintf("EobjUnregister: %#lx %s\n", win, eo->name);

   EobjListStackDel(eo);
}

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
   ECompMgrWinChangeOpacity(eo, opacity);
}
#endif
