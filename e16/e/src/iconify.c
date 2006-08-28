/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
 * Copyright (C) 2004-2006 Kim Woelders
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
#include "container.h"
#include "desktops.h"
#include "e16-ecore_hints.h"
#include "emodule.h"
#include "eobj.h"
#include "ewins.h"
#include "hints.h"
#include "icons.h"
#include "timers.h"
#include "tooltips.h"
#include "xwin.h"
#include <math.h>

static Container   *SelectIconboxForEwin(EWin * ewin);

/* Silly hack to avoid name clash warning when using -Wshadow */
#define y1 y1_

#define IB_ANIM_TIME 0.25

static void
IB_Animate(char iconify, EWin * from, EWin * to)
{
   double              t1, t2, t, i, spd, ii;
   int                 x, y, x1, y1, x2, y2, x3, y3, x4, y4, w, h, fx, fy, fw,
      fh, dx, dy, dw, dh;
   Window              root = VRoot.xwin;
   GC                  gc;
   XGCValues           gcv;
   Desk               *dskf, *dskt;

   if (Mode.wm.startup)
      return;

   dskf = EoGetDesk(from);
   dskt = EoGetDesk(to);

   EobjsRepaint();
   EGrabServer();

   spd = 0.00001;
   gcv.subwindow_mode = IncludeInferiors;
   gcv.function = GXxor;
   gcv.fill_style = FillOpaqueStippled;
   gcv.foreground = WhitePixel(disp, VRoot.scr);
   if (gcv.foreground == 0)
      gcv.foreground = BlackPixel(disp, VRoot.scr);
   gc = EXCreateGC(root,
		   GCFunction | GCForeground | GCSubwindowMode | GCFillStyle,
		   &gcv);
   t1 = GetTime();
   if (iconify)
     {
	fw = EoGetW(from) + 4;
	fh = EoGetH(from) + 4;
	fx = EoGetX(from) + EoGetX(dskf) - 2;
	fy = EoGetY(from) + EoGetY(dskf) - 2;
	dw = 4;
	dh = 4;
	dx = EoGetX(to) + EoGetX(dskt) + (EoGetW(to) / 2) - 2;
	dy = EoGetY(to) + EoGetY(dskt) + (EoGetH(to) / 2) - 2;
	for (i = 0.0; i < 1.0; i += spd)
	  {
	     ii = 1.0 - i;

	     x = (fx * ii) + (dx * i);
	     y = (fy * ii) + (dy * i);
	     w = (fw * ii) + (dw * i);
	     h = (fh * ii) + (dh * i);

	     x = (2 * x + w) / 2;	/* x middle */
	     y = (2 * y + h) / 2;	/* y middle */
	     w /= 2;		/* width/2 */
	     h /= 2;		/* height/2 */

	     x1 = x + w * (1 - .5 * sin(3.14159 + i * 6.2831853072));
	     y1 = y + h * cos(i * 6.2831853072);
	     x2 = x + w * (1 - .5 * sin(i * 6.2831853072));
	     y2 = y - h * cos(i * 6.2831853072);
	     x3 = x - w * (1 - .5 * sin(3.14159 + i * 6.2831853072));
	     y3 = y - h * cos(i * 6.2831853072);
	     x4 = x - w * (1 - .5 * sin(i * 6.2831853072));
	     y4 = y + h * cos(i * 6.2831853072);

	     XDrawLine(disp, root, gc, x1, y1, x2, y2);
	     XDrawLine(disp, root, gc, x2, y2, x3, y3);
	     XDrawLine(disp, root, gc, x3, y3, x4, y4);
	     XDrawLine(disp, root, gc, x4, y4, x1, y1);

	     XDrawLine(disp, root, gc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	     XDrawLine(disp, root, gc, x2 + 1, y2 + 1, x3 - 1, y3 - 1);
	     XDrawLine(disp, root, gc, x3 + 1, y3 + 1, x4 - 1, y4 - 1);
	     XDrawLine(disp, root, gc, x4 + 1, y4 + 1, x1 - 1, y1 - 1);

	     XDrawLine(disp, root, gc, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	     XDrawLine(disp, root, gc, x2 + 2, y2 + 2, x3 - 2, y3 - 2);
	     XDrawLine(disp, root, gc, x3 + 2, y3 + 2, x4 - 2, y4 - 2);
	     XDrawLine(disp, root, gc, x4 + 2, y4 + 2, x1 - 2, y1 - 2);

	     ESync();
	     t2 = GetTime();
	     t = t2 - t1;
	     t1 = t2;
	     spd = t / IB_ANIM_TIME;

	     XDrawLine(disp, root, gc, x1, y1, x2, y2);
	     XDrawLine(disp, root, gc, x2, y2, x3, y3);
	     XDrawLine(disp, root, gc, x3, y3, x4, y4);
	     XDrawLine(disp, root, gc, x4, y4, x1, y1);

	     XDrawLine(disp, root, gc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	     XDrawLine(disp, root, gc, x2 + 1, y2 + 1, x3 - 1, y3 - 1);
	     XDrawLine(disp, root, gc, x3 + 1, y3 + 1, x4 - 1, y4 - 1);
	     XDrawLine(disp, root, gc, x4 + 1, y4 + 1, x1 - 1, y1 - 1);

	     XDrawLine(disp, root, gc, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	     XDrawLine(disp, root, gc, x2 + 2, y2 + 2, x3 - 2, y3 - 2);
	     XDrawLine(disp, root, gc, x3 + 2, y3 + 2, x4 - 2, y4 - 2);
	     XDrawLine(disp, root, gc, x4 + 2, y4 + 2, x1 - 2, y1 - 2);
	  }
     }
   else
     {
	fw = EoGetW(from) + 4;
	fh = EoGetH(from) + 4;
	fx = EoGetX(from) + EoGetX(dskf) - 2;
	fy = EoGetY(from) + EoGetY(dskf) - 2;
	dw = 4;
	dh = 4;
	dx = EoGetX(to) + EoGetX(dskt) + (EoGetW(to) / 2) - 2;
	dy = EoGetY(to) + EoGetY(dskt) + (EoGetH(to) / 2) - 2;
	for (i = 1.0; i >= 0.0; i -= spd)
	  {
	     ii = 1.0 - i;

	     x = (fx * ii) + (dx * i);
	     y = (fy * ii) + (dy * i);
	     w = (fw * ii) + (dw * i);
	     h = (fh * ii) + (dh * i);

	     x = (2 * x + w) / 2;	/* x middle */
	     y = (2 * y + h) / 2;	/* y middle */
	     w /= 2;		/* width/2 */
	     h /= 2;		/* height/2 */

	     x1 = x + w * (1 - .5 * sin(3.14159 + i * 6.2831853072));
	     y1 = y + h * cos(i * 6.2831853072);
	     x2 = x + w * (1 - .5 * sin(i * 6.2831853072));
	     y2 = y - h * cos(i * 6.2831853072);
	     x3 = x - w * (1 - .5 * sin(3.14159 + i * 6.2831853072));
	     y3 = y - h * cos(i * 6.2831853072);
	     x4 = x - w * (1 - .5 * sin(i * 6.2831853072));
	     y4 = y + h * cos(i * 6.2831853072);

	     XDrawLine(disp, root, gc, x1, y1, x2, y2);
	     XDrawLine(disp, root, gc, x2, y2, x3, y3);
	     XDrawLine(disp, root, gc, x3, y3, x4, y4);
	     XDrawLine(disp, root, gc, x4, y4, x1, y1);

	     XDrawLine(disp, root, gc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	     XDrawLine(disp, root, gc, x2 + 1, y2 + 1, x3 - 1, y3 - 1);
	     XDrawLine(disp, root, gc, x3 + 1, y3 + 1, x4 - 1, y4 - 1);
	     XDrawLine(disp, root, gc, x4 + 1, y4 + 1, x1 - 1, y1 - 1);

	     XDrawLine(disp, root, gc, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	     XDrawLine(disp, root, gc, x2 + 2, y2 + 2, x3 - 2, y3 - 2);
	     XDrawLine(disp, root, gc, x3 + 2, y3 + 2, x4 - 2, y4 - 2);
	     XDrawLine(disp, root, gc, x4 + 2, y4 + 2, x1 - 2, y1 - 2);

	     ESync();
	     t2 = GetTime();
	     t = t2 - t1;
	     t1 = t2;
	     spd = t / IB_ANIM_TIME;

	     XDrawLine(disp, root, gc, x1, y1, x2, y2);
	     XDrawLine(disp, root, gc, x2, y2, x3, y3);
	     XDrawLine(disp, root, gc, x3, y3, x4, y4);
	     XDrawLine(disp, root, gc, x4, y4, x1, y1);

	     XDrawLine(disp, root, gc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	     XDrawLine(disp, root, gc, x2 + 1, y2 + 1, x3 - 1, y3 - 1);
	     XDrawLine(disp, root, gc, x3 + 1, y3 + 1, x4 - 1, y4 - 1);
	     XDrawLine(disp, root, gc, x4 + 1, y4 + 1, x1 - 1, y1 - 1);

	     XDrawLine(disp, root, gc, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	     XDrawLine(disp, root, gc, x2 + 2, y2 + 2, x3 - 2, y3 - 2);
	     XDrawLine(disp, root, gc, x3 + 2, y3 + 2, x4 - 2, y4 - 2);
	     XDrawLine(disp, root, gc, x4 + 2, y4 + 2, x1 - 2, y1 - 2);
	  }
     }
   EXFreeGC(gc);
   EUngrabServer();
}

static int
IconboxObjEwinFind(Container * ct, EWin * ewin)
{
   return ContainerObjectFind(ct, ewin);
}

static void
IconboxObjEwinAdd(Container * ct, EWin * ewin)
{
   int                 i;

   i = ContainerObjectAdd(ct, ewin);
   if (i < 0)
      return;

   ct->objs[i].im = EwinIconImageGet(ewin, ct->iconsize, ct->icon_mode);
   ContainerRedraw(ct);
}

static void
IconboxObjEwinDel(Container * ct, EWin * ewin)
{
   int                 i;

   i = IconboxObjEwinFind(ct, ewin);
   if (i < 0)
      return;

   if (ct->objs[i].im)
      EImageFree(ct->objs[i].im);

   ContainerObjectDel(ct, ewin);
}

static void
IconboxesEwinIconify(EWin * ewin)
{
   Container          *ct;

   SoundPlay("SOUND_ICONIFY");

   ct = SelectIconboxForEwin(ewin);
   if (!ct)
      return;

   IconboxObjEwinAdd(ct, ewin);

   if (ct->animate && !ewin->state.showingdesk)
      IB_Animate(1, ewin, ct->ewin);
}

static void
IconboxesEwinDeIconify(EWin * ewin)
{
   Container          *ct;

   SoundPlay("SOUND_DEICONIFY");

   ct = SelectIconboxForEwin(ewin);
   if (!ct)
      return;

   if (ct->animate && !ewin->state.showingdesk)
      IB_Animate(0, ewin, ct->ewin);

   IconboxObjEwinDel(ct, ewin);
   ContainerRedraw(ct);
   EobjsRepaint();
}

static void
RemoveMiniIcon(EWin * ewin)
{
   Container          *ct;

   ct = SelectIconboxForEwin(ewin);
   if (!ct)
      return;

   IconboxObjEwinDel(ct, ewin);
   ContainerRedraw(ct);
}

static int
IconboxFindEwin(Container * ct, void *data)
{
   EWin               *ewin = data;

   return IconboxObjEwinFind(ct, ewin) >= 0;

}

static Container   *
SelectIconboxForEwin(EWin * ewin)
{
   /* find the appropriate iconbox from all available ones for this app */
   /* if it is to be iconified, or if it is alreayd return which iconbox */
   /* it's in */
   Container          *ct, *ib_sel = NULL;

   if (!ewin)
      return NULL;

   if (ewin->state.iconified)
     {
	/* find the iconbox this window got iconifed into */
	ib_sel = ContainersIterate(IconboxFindEwin, IB_TYPE_ICONBOX, ewin);
     }
   else
     {
	/* pick the closest iconbox physically on screen to put it in */
	int                 min_dist;
	int                 dx, dy, dist;
	int                 i, num;
	Container         **lst;

	lst = ContainersGetList(&num);
	min_dist = 0x7fffffff;
	for (i = 0; i < num; i++)
	  {
	     ct = lst[i];
	     if (ct->ewin == NULL || ct->type != IB_TYPE_ICONBOX)
		continue;

	     dx = (EoGetX(ct->ewin) + (EoGetW(ct->ewin) / 2)) -
		(EoGetX(ewin) + (EoGetW(ewin) / 2));
	     dy = (EoGetY(ct->ewin) + (EoGetH(ct->ewin) / 2)) -
		(EoGetY(ewin) + (EoGetH(ewin) / 2));
	     dist = (dx * dx) + (dy * dy);
	     if ((!EoIsSticky(ct->ewin)) &&
		 (EoGetDesk(ct->ewin) != EoGetDesk(ewin)))
		dist += (VRoot.w * VRoot.w) + (VRoot.h * VRoot.h);
	     if (dist < min_dist)
	       {
		  min_dist = dist;
		  ib_sel = ct;
	       }
	  }
	if (lst)
	   Efree(lst);
     }

   return ib_sel;
}

static void
IconboxUpdateEwinIcon(Container * ct, EWin * ewin, int icon_mode)
{
   int                 i;

   if (ct->icon_mode != icon_mode)
      return;

   i = IconboxObjEwinFind(ct, ewin);
   if (i < 0)
      return;

   if (ct->objs[i].im)
      EImageFree(ct->objs[i].im);
   ct->objs[i].im = EwinIconImageGet(ewin, ct->iconsize, icon_mode);

   ContainerRedraw(ct);
}

static void
IconboxesUpdateEwinIcon(EWin * ewin, int icon_mode)
{
   int                 i, num;
   Container         **lst, *ct;

   lst = ContainersGetList(&num);
   for (i = 0; i < num; i++)
     {
	ct = lst[i];
	IconboxUpdateEwinIcon(ct, ewin, icon_mode);
     }
   if (lst)
      Efree(lst);
}

static void
IconboxFindIconSize(EImage * im, int *pw, int *ph, int size)
{
   int                 w, h, minsz, maxwh;

   EImageGetSize(im, &w, &h);

   maxwh = (w > h) ? w : h;
   if (maxwh <= 1)
      goto done;

   minsz = (size * 3) / 4;

   if (maxwh < minsz || maxwh > size)
     {
	w = (w * size) / maxwh;
	h = (h * size) / maxwh;
     }

 done:
   *pw = w;
   *ph = h;
}

static void
IconboxInit(Container * ct)
{
   ct->wm_name = "Iconbox";
   ct->menu_title = _("Iconbox Options");
   ct->dlg_title = _("Iconbox Settings");
   ct->iconsize = 48;
   ct->animate = 1;
}

static void
IconboxExit(Container * ct, int wm_exit)
{
   while (ct->num_objs)
     {
	if (!wm_exit)
	   EwinDeIconify(ct->objs[0].obj);
	IconboxObjEwinDel(ct, ct->objs[0].obj);
     }
}

static void
IconboxSighan(Container * ct __UNUSED__, int sig, void *prm)
{
   EWin               *ewin;

   switch (sig)
     {
     case ESIGNAL_EWIN_ICONIFY:
	ewin = (EWin *) prm;
	IconboxesEwinIconify(ewin);
	break;
     case ESIGNAL_EWIN_DEICONIFY:
	ewin = (EWin *) prm;
	IconboxesEwinDeIconify(ewin);
	break;
     case ESIGNAL_EWIN_DESTROY:
	ewin = (EWin *) prm;
	if (ewin->state.iconified)
	   RemoveMiniIcon(ewin);
	break;
     case ESIGNAL_EWIN_CHANGE_ICON:
	ewin = (EWin *) prm;
	if (ewin->state.iconified)
	   IconboxesUpdateEwinIcon(ewin, 1);
	break;
     }
}

static void
IconboxEvent(Container * ct, XEvent * ev)
{
   static EWin        *name_ewin = NULL;
   ToolTip            *tt;
   EWin               *ewin;
   int                 x, y;
   const char         *name;

   switch (ev->type)
     {
     case ButtonPress:
	if (ev->xbutton.button == 1)
	   ct->icon_clicked = 1;
	break;

     case ButtonRelease:
	if (!ct->icon_clicked)
	   break;
	ct->icon_clicked = 0;

	ewin = ContainerObjectFindByXY(ct, ev->xbutton.x, ev->xbutton.y);
	if (!ewin)
	   break;

	tt = TooltipFind("ICONBOX");
	if (tt)
	   TooltipHide(tt);

	EwinOpIconify(ewin, OPSRC_USER, 0);
	break;

     case MotionNotify:
	x = ev->xmotion.x;
	y = ev->xmotion.y;
	goto do_motion;

     case EnterNotify:
	x = ev->xcrossing.x;
	y = ev->xcrossing.y;
	goto do_motion;

      do_motion:
	if (!ct->shownames)
	   break;

	ewin = ContainerObjectFindByXY(ct, x, y);
	if (ewin == name_ewin)
	   break;
	name_ewin = ewin;

	tt = TooltipFind("ICONBOX");
	if (!tt)
	   break;

	TooltipHide(tt);
	if (!ewin)
	   break;

	name = EwinGetIconName(ewin);
	if (name)
	   TooltipShow(tt, name, NULL, Mode.events.x, Mode.events.y);
	break;

     case LeaveNotify:
	tt = TooltipFind("ICONBOX");
	if (tt)
	  {
	     TooltipHide(tt);
	     name_ewin = NULL;
	  }
	break;
     }
}

static void
IconboxObjSizeCalc(Container * ct, ContainerObject * cto)
{
   /* Inner size */
   cto->wi = cto->hi = 8;
   if (cto->im)
      IconboxFindIconSize(cto->im, &cto->wi, &cto->hi, ct->iconsize);
}

static void
IconboxObjPlace(Container * ct __UNUSED__, ContainerObject * cto, EImage * im)
{
   int                 w, h;

   if (!cto->im)
      return;

   EImageGetSize(cto->im, &w, &h);
   EImageBlend(im, cto->im, 1, 0, 0, w, h,
	       cto->xi, cto->yi, cto->wi, cto->hi, 1, 1);
}

const ContainerOps  IconboxOps = {
   IconboxInit,
   IconboxExit,
   IconboxSighan,
   IconboxEvent,
   IconboxObjSizeCalc,
   IconboxObjPlace,
};
