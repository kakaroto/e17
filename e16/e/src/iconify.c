/*
 * Copyright (C) 2000-2003 Carsten Haitzler, Geoff Harrison and various contributors
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
#define DECLARE_STRUCT_ICONBOX
#include "E.h"
#include <math.h>

static void         IcondefChecker(int val, void *data);

#define IB_ANIM_TIME 0.25

void
IB_Animate(char iconify, EWin * from, EWin * to)
{
   double              t1, t2, t, i, spd, ii;
   int                 x, y, x1, y1, x2, y2, x3, y3, x4, y4, w, h, fx, fy, fw,
      fh, dx, dy, dw, dh;
   GC                  gc;
   XGCValues           gcv;

   if (mode.startup)
      return;
   GrabX();
   spd = 0.00001;
   gcv.subwindow_mode = IncludeInferiors;
   gcv.function = GXxor;
   gcv.fill_style = FillOpaqueStippled;
   gcv.foreground = WhitePixel(disp, root.scr);
   if (gcv.foreground == 0)
      gcv.foreground = BlackPixel(disp, root.scr);
   gc = XCreateGC(disp, root.win,
		  GCFunction | GCForeground | GCSubwindowMode | GCFillStyle,
		  &gcv);
   t1 = GetTime();
   if (iconify)
     {
	fw = from->w + 4;
	fh = from->h + 4;
	fx = from->x + desks.desk[from->desktop].x - 2;
	fy = from->y + desks.desk[from->desktop].y - 2;
	dw = 4;
	dh = 4;
	dx = to->x + desks.desk[to->desktop].x + (to->w / 2) - 2;
	dy = to->y + desks.desk[to->desktop].y + (to->h / 2) - 2;
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

	     XDrawLine(disp, root.win, gc, x1, y1, x2, y2);
	     XDrawLine(disp, root.win, gc, x2, y2, x3, y3);
	     XDrawLine(disp, root.win, gc, x3, y3, x4, y4);
	     XDrawLine(disp, root.win, gc, x4, y4, x1, y1);

	     XDrawLine(disp, root.win, gc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	     XDrawLine(disp, root.win, gc, x2 + 1, y2 + 1, x3 - 1, y3 - 1);
	     XDrawLine(disp, root.win, gc, x3 + 1, y3 + 1, x4 - 1, y4 - 1);
	     XDrawLine(disp, root.win, gc, x4 + 1, y4 + 1, x1 - 1, y1 - 1);

	     XDrawLine(disp, root.win, gc, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	     XDrawLine(disp, root.win, gc, x2 + 2, y2 + 2, x3 - 2, y3 - 2);
	     XDrawLine(disp, root.win, gc, x3 + 2, y3 + 2, x4 - 2, y4 - 2);
	     XDrawLine(disp, root.win, gc, x4 + 2, y4 + 2, x1 - 2, y1 - 2);

	     XSync(disp, False);
	     t2 = GetTime();
	     t = t2 - t1;
	     t1 = t2;
	     spd = t / IB_ANIM_TIME;

	     XDrawLine(disp, root.win, gc, x1, y1, x2, y2);
	     XDrawLine(disp, root.win, gc, x2, y2, x3, y3);
	     XDrawLine(disp, root.win, gc, x3, y3, x4, y4);
	     XDrawLine(disp, root.win, gc, x4, y4, x1, y1);

	     XDrawLine(disp, root.win, gc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	     XDrawLine(disp, root.win, gc, x2 + 1, y2 + 1, x3 - 1, y3 - 1);
	     XDrawLine(disp, root.win, gc, x3 + 1, y3 + 1, x4 - 1, y4 - 1);
	     XDrawLine(disp, root.win, gc, x4 + 1, y4 + 1, x1 - 1, y1 - 1);

	     XDrawLine(disp, root.win, gc, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	     XDrawLine(disp, root.win, gc, x2 + 2, y2 + 2, x3 - 2, y3 - 2);
	     XDrawLine(disp, root.win, gc, x3 + 2, y3 + 2, x4 - 2, y4 - 2);
	     XDrawLine(disp, root.win, gc, x4 + 2, y4 + 2, x1 - 2, y1 - 2);
	  }
     }
   else
     {
	fw = from->w + 4;
	fh = from->h + 4;
	fx = from->x + desks.desk[from->desktop].x - 2;
	fy = from->y + desks.desk[from->desktop].y - 2;
	dw = 4;
	dh = 4;
	dx = to->x + desks.desk[to->desktop].x + (to->w / 2) - 2;
	dy = to->y + desks.desk[to->desktop].y + (to->h / 2) - 2;
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

	     XDrawLine(disp, root.win, gc, x1, y1, x2, y2);
	     XDrawLine(disp, root.win, gc, x2, y2, x3, y3);
	     XDrawLine(disp, root.win, gc, x3, y3, x4, y4);
	     XDrawLine(disp, root.win, gc, x4, y4, x1, y1);

	     XDrawLine(disp, root.win, gc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	     XDrawLine(disp, root.win, gc, x2 + 1, y2 + 1, x3 - 1, y3 - 1);
	     XDrawLine(disp, root.win, gc, x3 + 1, y3 + 1, x4 - 1, y4 - 1);
	     XDrawLine(disp, root.win, gc, x4 + 1, y4 + 1, x1 - 1, y1 - 1);

	     XDrawLine(disp, root.win, gc, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	     XDrawLine(disp, root.win, gc, x2 + 2, y2 + 2, x3 - 2, y3 - 2);
	     XDrawLine(disp, root.win, gc, x3 + 2, y3 + 2, x4 - 2, y4 - 2);
	     XDrawLine(disp, root.win, gc, x4 + 2, y4 + 2, x1 - 2, y1 - 2);

	     XSync(disp, False);
	     t2 = GetTime();
	     t = t2 - t1;
	     t1 = t2;
	     spd = t / IB_ANIM_TIME;

	     XDrawLine(disp, root.win, gc, x1, y1, x2, y2);
	     XDrawLine(disp, root.win, gc, x2, y2, x3, y3);
	     XDrawLine(disp, root.win, gc, x3, y3, x4, y4);
	     XDrawLine(disp, root.win, gc, x4, y4, x1, y1);

	     XDrawLine(disp, root.win, gc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	     XDrawLine(disp, root.win, gc, x2 + 1, y2 + 1, x3 - 1, y3 - 1);
	     XDrawLine(disp, root.win, gc, x3 + 1, y3 + 1, x4 - 1, y4 - 1);
	     XDrawLine(disp, root.win, gc, x4 + 1, y4 + 1, x1 - 1, y1 - 1);

	     XDrawLine(disp, root.win, gc, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	     XDrawLine(disp, root.win, gc, x2 + 2, y2 + 2, x3 - 2, y3 - 2);
	     XDrawLine(disp, root.win, gc, x3 + 2, y3 + 2, x4 - 2, y4 - 2);
	     XDrawLine(disp, root.win, gc, x4 + 2, y4 + 2, x1 - 2, y1 - 2);
	  }
     }
   XFreeGC(disp, gc);
   UngrabX();
}

void
IconboxIconifyEwin(Iconbox * ib, EWin * ewin)
{
   static int          call_depth = 0;
   char                was_shaded;

   EDBUG(6, "IconifyEwin");
   if (!ewin)
      EDBUG_RETURN_;
   if (GetZoomEWin() == ewin)
      Zoom(NULL);
   if (ewin->ibox)
      EDBUG_RETURN_;
   call_depth++;
   if (call_depth > 256)
     {
	call_depth--;
	return;
     }
   if (!ewin->iconified)
     {
	was_shaded = ewin->shaded;
	AUDIO_PLAY("SOUND_ICONIFY");
	if (ib)
	  {
	     if (ib->animate)
		IB_Animate(1, ewin, ib->ewin);
	     UpdateAppIcon(ewin, ib->icon_mode);
	     IconboxAddEwin(ib, ewin);
	  }
	HideEwin(ewin);
	if (was_shaded != ewin->shaded)
	   InstantShadeEwin(ewin);
	ICCCM_Iconify(ewin);
	if (ewin == mode.focuswin)
	  {
	     char                prev_warp;

	     prev_warp = mode.display_warp;
	     mode.display_warp = 0;
	     GetPrevFocusEwin();
	     mode.display_warp = prev_warp;
	  }
	if (ewin->has_transients)
	  {
	     EWin              **lst;
	     int                 i, num;

	     lst = ListTransientsFor(ewin->client.win, &num);
	     if (lst)
	       {
		  for (i = 0; i < num; i++)
		    {
		       if (!lst[i]->iconified)
			 {
			    HideEwin(lst[i]);
			    lst[i]->iconified = 4;
			    if (lst[i] == mode.focuswin)
			       FocusToEWin(NULL);
			 }
		    }
		  HintsSetClientList();
		  Efree(lst);
	       }
	  }
     }
   call_depth--;

   HintsSetWindowState(ewin);
   EDBUG_RETURN_;
}

void
IconifyEwin(EWin * ewin)
{
   IconboxIconifyEwin(SelectIconboxForEwin(ewin), ewin);
}

void
DeIconifyEwin(EWin * ewin)
{
   static int          call_depth = 0;
   Iconbox            *ib;
   int                 x, y, dx, dy;

   EDBUG(6, "DeIconifyEwin");
   call_depth++;
   if (call_depth > 256)
     {
	call_depth--;
	return;
     }
   if (ewin->iconified)
     {
	ib = SelectIconboxForEwin(ewin);
	RemoveMiniIcon(ewin);

	dx = ewin->w / 2;
	dy = ewin->h / 2;
	x = (ewin->x + dx) % root.w;
	if (x < 0)
	   x += root.w;
	x -= dx;
	y = (ewin->y + dy) % root.h;
	if (y < 0)
	   y += root.h;
	y -= dy;

	dx = x - ewin->x;
	dy = y - ewin->y;

	if (!ewin->sticky)
	   MoveEwinToDesktopAt(ewin, desks.current, ewin->x + dx, ewin->y + dy);
	else
	   MoveEwin(ewin, ewin->x + dx, ewin->y + dy);

	AUDIO_PLAY("SOUND_DEICONIFY");
	if (ib)
	  {
	     if (ib->animate)
		IB_Animate(0, ewin, ib->ewin);
	  }
	RaiseEwin(ewin);
	ShowEwin(ewin);
	ICCCM_DeIconify(ewin);
	FocusToEWin(ewin);
	mode.destroy = 1;
	if (ewin->has_transients)
	  {
	     EWin              **lst, *e;
	     int                 i, num;

	     lst = ListTransientsFor(ewin->client.win, &num);
	     if (lst)
	       {
		  for (i = 0; i < num; i++)
		    {
		       e = lst[i];

		       if (e->iconified != 4)
			  continue;

		       if (!e->sticky)
			  MoveEwinToDesktopAt(e, desks.current,
					      e->x + dx, e->y + dy);
		       else
			  MoveEwin(e, e->x + dx, e->y + dy);
		       RaiseEwin(e);
		       ShowEwin(e);
		       e->iconified = 0;
		    }
		  HintsSetClientList();
		  Efree(lst);
	       }
	  }
     }
   call_depth--;

   HintsSetWindowState(ewin);
   EDBUG_RETURN_;
}

void
MakeIcon(EWin * ewin)
{
   Iconbox            *ib;

   ib = SelectIconboxForEwin(ewin);
   if (ib)
      IconboxAddEwin(ib, ewin);
}

void
RemoveMiniIcon(EWin * ewin)
{
   Iconbox            *ib;

   ib = SelectIconboxForEwin(ewin);
   if (ib)
      IconboxDelEwin(ib, ewin);
}

Iconbox            *
IconboxCreate(char *name)
{
   Iconbox            *ib;

   ib = Emalloc(sizeof(Iconbox));
   ib->name = duplicate(name);
   ib->orientation = 0;
   ib->scrollbar_side = 1;
   ib->arrow_side = 1;
   ib->nobg = 0;
   ib->shownames = 1;
   ib->iconsize = 48;
   ib->icon_mode = 2;
   ib->auto_resize = 0;
   ib->draw_icon_base = 0;
   ib->scrollbar_hide = 0;
   ib->cover_hide = 0;
   ib->auto_resize_anchor = 0;
   /* FIXME: need to have theme settable params for this and get them */
   ib->scroll_thickness = 12;
   ib->arrow_thickness = 12;
   ib->bar_thickness = 8;
   ib->knob_length = 8;
   ib->animate = 1;

   ib->w = 0;
   ib->h = 0;
   ib->pos = 0;
   ib->max = 1;
   ib->force_update = 1;
   ib->arrow1_hilited = 0;
   ib->arrow1_clicked = 0;
   ib->arrow2_hilited = 0;
   ib->arrow2_clicked = 0;
   ib->icon_clicked = 0;
   ib->scrollbar_hilited = 0;
   ib->scrollbar_clicked = 0;
   ib->scrollbox_clicked = 0;
   ib->win = ECreateWindow(root.win, 0, 0, 128, 32, 0);
   ib->icon_win = ECreateWindow(ib->win, 0, 0, 128, 26, 0);
   ib->cover_win = ECreateWindow(ib->win, 0, 0, 128, 26, 0);
   ib->scroll_win = ECreateWindow(ib->win, 6, 26, 116, 6, 0);
   ib->arrow1_win = ECreateWindow(ib->win, 0, 26, 6, 6, 0);
   ib->arrow2_win = ECreateWindow(ib->win, 122, 26, 6, 6, 0);
   ib->scrollbar_win = ECreateWindow(ib->scroll_win, 122, 26, 6, 6, 0);
   ib->scrollbarknob_win = ECreateWindow(ib->scrollbar_win, -20, -20, 4, 4, 0);
   ib->pmap = ECreatePixmap(disp, ib->icon_win, 128, 32, root.depth);
   XSelectInput(disp, ib->icon_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask | PointerMotionMask);
   XSelectInput(disp, ib->scroll_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask);
   XSelectInput(disp, ib->cover_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask);
   XSelectInput(disp, ib->arrow1_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask);
   XSelectInput(disp, ib->arrow2_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask);
   XSelectInput(disp, ib->scrollbar_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask | PointerMotionMask);
   EMapWindow(disp, ib->icon_win);
   EMapWindow(disp, ib->scroll_win);
   EMapWindow(disp, ib->arrow1_win);
   EMapWindow(disp, ib->arrow2_win);
   EMapWindow(disp, ib->scrollbar_win);
   EMapWindow(disp, ib->scrollbarknob_win);
   ib->ewin = NULL;
   ib->num_icons = 0;
   ib->icons = NULL;
   AddItem(ib, ib->name, 0, LIST_TYPE_ICONBOX);
   return ib;
}

void
IconboxDestroy(Iconbox * ib)
{
   int                 i;

   RemoveItem((char *)ib, 0, LIST_FINDBY_POINTER, LIST_TYPE_ICONBOX);
   if (ib->name)
      Efree(ib->name);
   for (i = 0; i < ib->num_icons; i++)
      DeIconifyEwin(ib->icons[i]);
   if (ib->icons)
      Efree(ib->icons);
   if (ib->pmap)
      EFreePixmap(disp, ib->pmap);
   EDestroyWindow(disp, ib->win);
   Efree(ib);
   autosave();
}

Window
IconboxGetWin(Iconbox * ib)
{
   return ib->win;
}

static void
IB_Reconfigure(Iconbox * ib)
{
   ImageClass         *ic;
   EWin               *ewin;
   int                 extra = 0;

   ewin = ib->ewin;
   ib->force_update = 1;
   ewin->client.width.min = 8;
   ewin->client.height.min = 8;
   ewin->client.width.max = 16384;
   ewin->client.height.max = 16384;
   ewin->client.no_resize_h = 0;
   ewin->client.no_resize_v = 0;

   if (ib->orientation)
     {
	ic = FindItem("ICONBOX_VERTICAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	   extra = ic->padding.left + ic->padding.right;
	ewin->client.width.max = ewin->client.width.min =
	   ib->iconsize + ib->scroll_thickness + extra;
	ewin->client.no_resize_h = 1;
	ib->max_min = ewin->client.height.min;
     }
   else
     {
	ic = FindItem("ICONBOX_HORIZONTAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	   extra = ic->padding.left + ic->padding.right;
	ewin->client.height.max = ewin->client.height.min =
	   ib->iconsize + ib->scroll_thickness + extra;
	ewin->client.no_resize_v = 1;
	ib->max_min = ewin->client.width.min;
     }

   ICCCM_MatchSize(ewin);
}

void
IconboxShow(Iconbox * ib)
{
   EWin               *ewin = NULL;
   XClassHint         *xch;
   XTextProperty       xtp;
   char                pq;

   pq = queue_up;
   queue_up = 0;
   xtp.encoding = XA_STRING;
   xtp.format = 8;
   xtp.value = (unsigned char *)("Iconbox");
   xtp.nitems = strlen((char *)(xtp.value));
   XSetWMName(disp, ib->win, &xtp);
   xch = XAllocClassHint();
   xch->res_name = ib->name;
   xch->res_class = "Enlightenment_IconBox";
   XSetClassHint(disp, ib->win, xch);
   XFree(xch);
   MatchToSnapInfoIconbox(ib);
   ewin = AddInternalToFamily(ib->win, 1, "ICONBOX", EWIN_TYPE_ICONBOX, ib);

   if (ewin)
     {
	Snapshot           *sn;

	ib->ewin = ewin;
	ewin->ibox = ib;
	IB_Reconfigure(ib);
	sn = FindSnapshot(ewin);
	ConformEwinToDesktop(ewin);
	ShowEwin(ewin);
	if (sn)
	  {
	     ResizeEwin(ewin, ewin->client.w, ewin->client.h);
	  }
	else
	  {
	     MakeWindowSticky(ewin);
	     MoveResizeEwin(ewin, root.w - 160, root.h - 160, 160, 160);
	  }
     }
   queue_up = pq;
}

void
IconboxHide(Iconbox * ib)
{
   if (ib->ewin)
      HideEwin(ib->ewin);
}

void
IconboxAddEwin(Iconbox * ib, EWin * ewin)
{
   int                 i;

   /* check if its already there - then dont add */
   for (i = 0; i < ib->num_icons; i++)
     {
	if (ib->icons[i] == ewin)
	   return;
     }
   ib->num_icons++;
   ib->icons = Erealloc(ib->icons, sizeof(EWin *) * ib->num_icons);
   ib->icons[ib->num_icons - 1] = ewin;
   IconboxRedraw(ib);
}

void
IconboxDelEwin(Iconbox * ib, EWin * ewin)
{
   int                 i, j;

   for (i = 0; i < ib->num_icons; i++)
     {
	if (ib->icons[i] == ewin)
	  {
	     for (j = i; j < ib->num_icons - 1; j++)
		ib->icons[j] = ib->icons[j + 1];
	     ib->num_icons--;
	     if (ib->num_icons > 0)
		ib->icons = Erealloc(ib->icons, sizeof(EWin *) * ib->num_icons);
	     else
	       {
		  Efree(ib->icons);
		  ib->icons = NULL;
	       }
	     IconboxRedraw(ib);
	     return;
	  }
     }
}

static void
IB_SnapEWin(EWin * ewin)
{
   int                 w, h, ord, rn, i;
   GC                  gc;
   XGCValues           gcv;
   XRectangle         *r = NULL;
   Iconbox            *ib;
   ImageClass         *ic;

   if (!ewin->visible)
      return;
   w = 40;
   h = 40;
   ib = SelectIconboxForEwin(ewin);
   if (ib)
     {
	w = ib->iconsize;
	h = ib->iconsize;
     }
   if (ib->draw_icon_base)
     {
	ic = FindItem("DEFAULT_ICON_BUTTON", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	  {
	     w -= ic->padding.left + ic->padding.right;
	     h -= ic->padding.top + ic->padding.bottom;
	  }
     }
   if (ewin->w > ewin->h)
      h = (w * ewin->h) / ewin->w;
   else
      w = (h * ewin->w) / ewin->h;
   if (w < 4)
      w = 4;
   if (h < 4)
      h = 4;

   ewin->icon_pmm.type = 0;
   ewin->icon_w = w;
   ewin->icon_h = h;
   ewin->icon_pmm.pmap = ECreatePixmap(disp, ewin->win, w, h, root.depth);
   PagerScaleRect(ewin->icon_pmm.pmap, ewin->win, 0, 0, 0, 0, ewin->w, ewin->h,
		  w, h);
   r = EShapeGetRectangles(disp, ewin->win, ShapeBounding, &rn, &ord);
   ewin->icon_pmm.mask = ECreatePixmap(disp, ewin->win, w, h, 1);
   gc = XCreateGC(disp, ewin->icon_pmm.mask, 0, &gcv);
   if (r)
     {
	XSetForeground(disp, gc, 0);
	XFillRectangle(disp, ewin->icon_pmm.mask, gc, 0, 0, w, h);
	XSetForeground(disp, gc, 1);
	for (i = 0; i < rn; i++)
	  {
	     int                 x, y, ww, hh;

	     x = (r[i].x * w) / ewin->w;
	     y = (r[i].y * h) / ewin->h;
	     ww = (r[i].width * w) / ewin->w;
	     hh = (r[i].height * h) / ewin->h;
	     if (ww < 4)
		ww = 4;
	     if (hh < 4)
		hh = 4;
	     XFillRectangle(disp, ewin->icon_pmm.mask, gc, x, y, ww, hh);
	  }
	XFree(r);
     }
   else
     {
	XSetForeground(disp, gc, 1);
	XFillRectangle(disp, ewin->icon_pmm.mask, gc, 0, 0, w, h);
     }

   XFreeGC(disp, gc);

   if ((ewin->icon_w < 1) || (ewin->icon_h < 1))
      FreePmapMask(&ewin->icon_pmm);
}

static void
IB_GetAppIcon(EWin * ewin)
{
   /* get the applications icon pixmap and make a copy... */
   int                 x, y;
   unsigned int        w, h, depth, bw;
   Window              rt;

   if (!ewin->client.icon_pmap)
      return;

   w = 0;
   h = 0;
   EGetGeometry(disp, ewin->client.icon_pmap, &rt, &x, &y, &w, &h, &bw, &depth);

   ewin->icon_pmm.type = 0;
   ewin->icon_w = (int)w;
   ewin->icon_h = (int)h;
   ewin->icon_pmm.pmap = ECreatePixmap(disp, root.win, w, h, root.depth);
   if (ewin->client.icon_mask)
      ewin->icon_pmm.mask = ECreatePixmap(disp, root.win, w, h, 1);

   if (depth == 1)
     {
	GC                  gc;
	XGCValues           gcv;

	gc = XCreateGC(disp, ewin->icon_pmm.pmap, 0, &gcv);
	XSetForeground(disp, gc, WhitePixel(disp, root.scr));
	XFillRectangle(disp, ewin->icon_pmm.pmap, gc, 0, 0, w, h);
	XSetClipOrigin(disp, gc, 0, 0);
	XSetClipMask(disp, gc, ewin->client.icon_pmap);
	XSetForeground(disp, gc, BlackPixel(disp, root.scr));
	XFillRectangle(disp, ewin->icon_pmm.pmap, gc, 0, 0, w, h);
	XFreeGC(disp, gc);
     }
   else
      EPastePixmap(ewin->icon_pmm.pmap, ewin->client.icon_pmap, 0, 0, w, h);

   if (ewin->client.icon_mask)
      EPastePixmap(ewin->icon_pmm.mask, ewin->client.icon_mask, 0, 0, w, h);

   if ((ewin->icon_w < 1) || (ewin->icon_h < 1))
      FreePmapMask(&ewin->icon_pmm);
}

static void
IB_PasteDefaultBase(Drawable d, int x, int y, int w, int h)
{
   ImageClass         *ic;
   PmapMask            pmm;

   /* get the base pixmap */
   ic = FindItem("DEFAULT_ICON_BUTTON", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (!ic)
      return;

   IclassApplyCopy(ic, d, w, h, 0, 0, STATE_NORMAL, &pmm, 1);
   PastePixmap(disp, d, pmm.pmap, pmm.mask, x, y);
   FreePmapMask(&pmm);
}

static void
IB_PasteDefaultBaseMask(Drawable d, int x, int y, int w, int h)
{
   ImageClass         *ic;
   PmapMask            pmm;
   GC                  gc;
   XGCValues           gcv;

   /* get the base pixmap */
   ic = FindItem("DEFAULT_ICON_BUTTON", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (!ic)
      return;

   IclassApplyCopy(ic, d, w, h, 0, 0, STATE_NORMAL, &pmm, 1);
   if (pmm.mask)
     {
	PasteMask(disp, d, pmm.mask, x, y, w, h);
     }
   else
     {
	gc = XCreateGC(disp, d, 0, &gcv);
	XSetForeground(disp, gc, 1);
	XFillRectangle(disp, d, gc, x, y, w, h);
	XFreeGC(disp, gc);
     }
   FreePmapMask(&pmm);
}

static Icondef    **
IB_ListIcondef(int *num)
{
   return (Icondef **) ListItemType(num, LIST_TYPE_ICONDEF);
}

static Icondef     *
IB_MatchIcondef(char *title, char *name, char *class)
{
   /* return an icondef that matches the data given */
   Icondef           **il, *idef;
   int                 i, num;

   il = IB_ListIcondef(&num);
   if (il)
     {
	for (i = 0; i < num; i++)
	  {
	     char                match = 1;

	     if ((il[i]->title_match) && (!title))
		match = 0;
	     if ((il[i]->name_match) && (!name))
		match = 0;
	     if ((il[i]->class_match) && (!class))
		match = 0;
	     if ((il[i]->title_match) && (title))
	       {
		  if (!matchregexp(il[i]->title_match, title))
		     match = 0;
	       }
	     if ((il[i]->name_match) && (name))
	       {
		  if (!matchregexp(il[i]->name_match, name))
		     match = 0;
	       }
	     if ((il[i]->class_match) && (class))
	       {
		  if (!matchregexp(il[i]->class_match, class))
		     match = 0;
	       }
	     if (match)
	       {
		  idef = il[i];
		  Efree(il);
		  return idef;
	       }
	  }
	Efree(il);
     }
   return NULL;
}

static void
IB_GetEIcon(EWin * ewin)
{
   /* get the icon defined for this window in E's iconf match file */
   Icondef            *idef;
   ImageClass         *ic;
   Imlib_Image        *im;
   int                 w, h, mw, mh;
   Iconbox            *ib;

   idef = IB_MatchIcondef(ewin->client.title, ewin->client.name,
			  ewin->client.class);

   if (!idef)
      return;

   im = ELoadImage(idef->icon_file);
   if (!im)
      return;

   imlib_context_set_image(im);
   w = imlib_image_get_width();
   h = imlib_image_get_height();

   ib = SelectIconboxForEwin(ewin);
   if (ib)
     {
	mw = ib->iconsize;
	mh = ib->iconsize;
	if (ib->draw_icon_base)
	  {
	     ic = FindItem("DEFAULT_ICON_BUTTON", 0, LIST_FINDBY_NAME,
			   LIST_TYPE_ICLASS);
	     if (ic)
	       {
		  mw -= ic->padding.left + ic->padding.right;
		  mh -= ic->padding.top + ic->padding.bottom;
	       }
	  }
	if (mw < w)
	  {
	     h = (mw * h) / w;
	     w = mw;
	  }
	if (mh < h)
	  {
	     w = (mh * w) / h;
	     h = mh;
	  }
     }

   imlib_render_pixmaps_for_whole_image_at_size(&ewin->icon_pmm.pmap,
						&ewin->icon_pmm.mask, w, h);
   ewin->icon_pmm.type = 1;
   ewin->icon_w = w;
   ewin->icon_h = h;
   imlib_free_image();
}

void
IB_AddIcondef(char *title, char *name, char *class, char *file)
{
   /* add match for a window pointing to an iconfile */
   /* form: "*term*" "name*" "*class" "path/to_image.png" */
   Icondef            *idef;

   idef = Emalloc(sizeof(Icondef));
   if (!idef)
      return;
   idef->title_match = duplicate(title);
   idef->name_match = duplicate(name);
   idef->class_match = duplicate(class);
   idef->icon_file = duplicate(file);
   AddItem(idef, "", 0, LIST_TYPE_ICONDEF);
}

static void
IB_RemoveIcondef(Icondef * idef)
{
   /* remove the pointed to icondef from our database */
   Icondef            *idef2;

   idef2 = RemoveItem((char *)idef, 0, LIST_FINDBY_POINTER, LIST_TYPE_ICONDEF);
   if (!idef2)
      return;
   if (idef->title_match)
      Efree(idef->title_match);
   if (idef->name_match)
      Efree(idef->name_match);
   if (idef->class_match)
      Efree(idef->class_match);
   if (idef->icon_file)
      Efree(idef->icon_file);
   Efree(idef);
}

static time_t       last_icondefs_time = 0;

static void
IB_LoadIcondefs(void)
{
   /* load the icon defs */
   char               *ff = NULL, s[1024], *s1, *s2, *s3, *s4;
   FILE               *f;

   ff = FindFile("icondefs.cfg");
   if (!ff)
      return;
   f = fopen(ff, "r");
   while (fgets(s, 1024, f))
     {
	s[strlen(s) - 1] = 0;
	/* file format : */
	/* "icon/image.png" "*title*" "*name*" "*class*" */
	/* any field except field 1 can be NULL if you dont care */
	/* the default match is: */
	/* "icon/defailt_image.png" NULL NULL NULL */
	/* and must be first in the file */
	s1 = field(s, 0);
	s2 = field(s, 1);
	s3 = field(s, 2);
	s4 = field(s, 3);
	if (s1)
	   IB_AddIcondef(s2, s3, s4, s1);
	if (s1)
	   Efree(s1);
	if (s2)
	   Efree(s2);
	if (s3)
	   Efree(s3);
	if (s4)
	   Efree(s4);
     }
   fclose(f);
   last_icondefs_time = moddate(ff);
   Efree(ff);
}

static void
IB_ReLoadIcondefs(void)
{
   /* stat the icondefs and compare mod date to last known mod date - if */
   /* modified, delete all icondefs and load again */
   char               *ff = NULL;
   Icondef           **idef;
   int                 i, num;

   ff = FindFile("icondefs.cfg");
   if (!ff)
     {
	idef = IB_ListIcondef(&num);
	if (idef)
	  {
	     for (i = 0; i < num; i++)
		IB_RemoveIcondef(idef[i]);
	     Efree(idef);
	  }
	return;
     }
   if (moddate(ff) > last_icondefs_time)
     {
	idef = IB_ListIcondef(&num);
	if (idef)
	  {
	     for (i = 0; i < num; i++)
		IB_RemoveIcondef(idef[i]);
	     Efree(idef);
	  }
	IB_LoadIcondefs();
     }
   Efree(ff);
}

static void
IcondefChecker(int val, void *data)
{
   IB_ReLoadIcondefs();
   DoIn("ICONDEF_CHECK", 2.0, IcondefChecker, 0, NULL);
   val = 0;
   data = NULL;
}

#if 0				/* Not used */
static void
IB_SaveIcondefs(void)
{
   /* save the icondefs */
   char                s[1024];
   FILE               *f;

   Esnprintf(s, sizeof(s), "%s/icondefs.cfg", UserEDir());
   f = fopen(s, "w");
   if (f)
     {
	Icondef           **idef;
	int                 i, num;

	idef = IB_ListIcondef(&num);
	for (i = num - 1; i >= 0; i--)
	  {
	     char               *f1, *f2, *f3, *f4;

	     f1 = idef[i]->icon_file;
	     f2 = idef[i]->title_match;
	     f3 = idef[i]->name_match;
	     f4 = idef[i]->class_match;

	     if (f1)
	       {
		  fprintf(f, "\"%s\" ", f1);
		  if (f2)
		     fprintf(f, "\"%s\" ", f2);
		  else
		     fprintf(f, "NULL ");
		  if (f3)
		     fprintf(f, "\"%s\" ", f3);
		  else
		     fprintf(f, "NULL ");
		  if (f4)
		     fprintf(f, "\"%s\"\n", f4);
		  else
		     fprintf(f, "NULL\n");
	       }
	  }
	fclose(f);
	last_icondefs_time = moddate(s);
     }
}
#endif

Iconbox           **
ListAllIconboxes(int *num)
{
   /* list all currently available Iconboxes */
   return (Iconbox **) ListItemType(num, LIST_TYPE_ICONBOX);
}

Iconbox            *
SelectIconboxForEwin(EWin * ewin)
{
   /* find the appropriate iconbox from all available ones for this app */
   /* if it is to be iconified, or if it is alreayd return which iconbox */
   /* it's in */
   Iconbox           **ib, *ib_sel = NULL;
   int                 i, j, num = 0;

   if (!ewin)
      return NULL;
   ib = ListAllIconboxes(&num);
   if (ib)
     {
	if (ewin->iconified)
	  {
	     /* find the iconbox this window got iconifed into */
	     for (i = 0; i < num; i++)
	       {
		  for (j = 0; j < ib[i]->num_icons; j++)
		    {
		       if (ib[i]->icons[j] == ewin)
			 {
			    Iconbox            *ibr;

			    ibr = ib[i];
			    Efree(ib);
			    return ibr;
			 }
		    }
	       }
	  }
	else
	  {
	     /* pick the closest iconbox physically on screen to put it in */
	     int                 min_dist;

	     ib_sel = ib[0];
	     min_dist = 0x7fffffff;
	     for (i = 0; i < num; i++)
	       {
		  int                 dx, dy, dist;

		  if (ib[i]->ewin == NULL)
		     continue;
		  dx = (ib[i]->ewin->x + (ib[i]->ewin->w / 2)) - (ewin->x +
								  (ewin->w /
								   2));
		  dy = (ib[i]->ewin->y + (ib[i]->ewin->h / 2)) - (ewin->y +
								  (ewin->h /
								   2));
		  dist = (dx * dx) + (dy * dy);
		  if ((!ib[i]->ewin->sticky)
		      && (ib[i]->ewin->desktop != ewin->desktop))
		     dist += (root.w * root.w) + (root.h * root.h);
		  if (dist < min_dist)
		    {
		       min_dist = dist;
		       ib_sel = ib[i];
		    }
	       }
	  }
	Efree(ib);
     }
#if 0
   else
     {
	/* If there are no iconboxes, create one. */
	doCreateIconbox(NULL);
	ib = ListAllIconboxes(&num);
	if (ib)
	  {			/* paranoia */
	     ib_sel = ib[0];
	     Efree(ib);
	  }
     }
#endif

   return ib_sel;
}

void
IconboxUpdateEwinIcon(Iconbox * ib, EWin * ewin, int icon_mode)
{
   int                 i;

   if (ib->icon_mode != icon_mode)
      return;

   for (i = 0; i < ib->num_icons; i++)
     {
	if (ib->icons[i] != ewin)
	   continue;
	UpdateAppIcon(ewin, icon_mode);
	IconboxRedraw(ib);
	break;
     }
}

void
IconboxesUpdateEwinIcon(EWin * ewin, int icon_mode)
{
   Iconbox           **ib;
   int                 i, num = 0;

   ib = ListAllIconboxes(&num);
   if (ib)
     {
	for (i = 0; i < num; i++)
	   IconboxUpdateEwinIcon(ib[i], ewin, icon_mode);
	Efree(ib);
     }
}

void
UpdateAppIcon(EWin * ewin, int imode)
{
   /* free whatever we had before */
   FreePmapMask(&ewin->icon_pmm);

   switch (imode)
     {
     case 0:
	/* snap first - if fails try app, then e */
	if (!ewin->icon_pmm.pmap)
	  {
	     if (ewin->shaded)
		InstantUnShadeEwin(ewin);
	     RaiseEwin(ewin);
	     IB_SnapEWin(ewin);
	  }
	if (!ewin->icon_pmm.pmap)
	   IB_GetAppIcon(ewin);
	if (!ewin->icon_pmm.pmap)
	   IB_GetEIcon(ewin);
	break;
     case 1:
	/* try app first, then e, then snap */
	if (!ewin->icon_pmm.pmap)
	   IB_GetAppIcon(ewin);
	if (!ewin->icon_pmm.pmap)
	   IB_GetEIcon(ewin);
	if (!ewin->icon_pmm.pmap)
	  {
	     if (ewin->shaded)
		InstantUnShadeEwin(ewin);
	     RaiseEwin(ewin);
	     IB_SnapEWin(ewin);
	  }
	break;
     case 2:
	/* try E first, then snap */
	if (!ewin->icon_pmm.pmap)
	   IB_GetEIcon(ewin);
	if (!ewin->icon_pmm.pmap)
	  {
	     if (ewin->shaded)
		InstantUnShadeEwin(ewin);
	     RaiseEwin(ewin);
	     IB_SnapEWin(ewin);
	  }
	break;
     default:
	break;
     }
}

static void
IB_CalcMax(Iconbox * ib)
{
   int                 i, x, y;

   x = 0;
   y = 0;
   for (i = 0; i < ib->num_icons; i++)
     {
	int                 w, h;
	EWin               *ewin;

	w = 8;
	h = 8;
	ewin = ib->icons[i];
	if (!ewin->icon_pmm.pmap)
	   UpdateAppIcon(ewin, ib->icon_mode);
	if (ewin->icon_pmm.pmap)
	  {
	     w = ewin->icon_w;
	     h = ewin->icon_h;
	  }
	if (ib->draw_icon_base)
	  {
	     x += ib->iconsize;
	     y += ib->iconsize;
	  }
	else
	  {
	     x += w + 2;
	     y += h + 2;
	  }
     }
   if (ib->orientation)
      ib->max = y - 2;
   else
      ib->max = x - 2;

   if (ib->max < ib->max_min)
      ib->max = ib->max_min;
}

static EWin        *
IB_FindIcon(Iconbox * ib, int px, int py)
{
   int                 i, x = 0, y = 0;
   ImageClass         *ic = NULL;

   if (ib->orientation)
     {
	ic = FindItem("ICONBOX_VERTICAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	y = -ib->pos;
	x = 0;
	if (ic)
	  {
	     x += ic->padding.left;
	     y += ic->padding.top;
	  }
     }
   else
     {
	ic = FindItem("ICONBOX_HORIZONTAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	x = -ib->pos;
	y = 0;
	if (ic)
	  {
	     x += ic->padding.left;
	     y += ic->padding.top;
	  }
     }

   for (i = 0; i < ib->num_icons; i++)
     {
	int                 w, h, xx, yy;
	EWin               *ewin;

	w = 8;
	h = 8;
	ewin = ib->icons[i];
	if (!ewin->icon_pmm.pmap)
	   UpdateAppIcon(ewin, ib->icon_mode);
	if (ewin->icon_pmm.pmap)
	  {
	     w = ewin->icon_w;
	     h = ewin->icon_h;
	     xx = x;
	     yy = y;
	     if (ib->orientation)
	       {
		  if (ib->draw_icon_base)
		     yy += (ib->iconsize - h) / 2;
		  xx += (ib->iconsize - w) / 2;
	       }
	     else
	       {
		  if (ib->draw_icon_base)
		     xx += (ib->iconsize - w) / 2;
		  yy += (ib->iconsize - h) / 2;
	       }
	     if ((px >= (xx - 1)) && (py >= (yy - 1)) && (px < (xx + w + 1))
		 && (py < (yy + h + 1)))
		return ewin;
	  }
	if (ib->orientation)
	  {
	     if (ib->draw_icon_base)
		y += ib->iconsize;
	     else
		y += h + 2;
	  }
	else
	  {
	     if (ib->draw_icon_base)
		x += ib->iconsize;
	     else
		x += w + 2;
	  }
     }
   return NULL;
}

static void
IB_DrawScroll(Iconbox * ib)
{
   ImageClass         *ic;
   char                show_sb = 1;

   if (ib->orientation)
     {
	int                 bs, bw, bx;

	ic = FindItem("ICONBOX_SCROLLBAR_BASE_VERTICAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ib->arrow_side < 3)
	   bs = ib->h - (ib->arrow_thickness * 2);
	else
	   bs = ib->h;
	bw = (ib->h * bs) / ib->max;
	if (ic)
	  {
	     bs -= (ic->padding.top + ic->padding.bottom);
	     bw = ((ib->h - (ic->padding.top + ic->padding.bottom)) * bs) /
		ib->max;
	  }
	if (bs < 1)
	   bs = 1;
	if (bw > bs)
	   bw = bs;
	if (bw < 1)
	   bw = 1;
	bx = ((ib->pos * bs) / ib->max);
	if (ic)
	   bx += ic->padding.top;
	if ((ib->scrollbar_hide) && (bw == bs))
	   show_sb = 0;
	ic = FindItem("ICONBOX_SCROLLKNOB_VERTICAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if ((ic) && (bw > ib->knob_length))
	   EMoveResizeWindow(disp, ib->scrollbarknob_win, 0,
			     (bw - ib->knob_length) / 2, ib->bar_thickness,
			     ib->knob_length);
	else
	   EMoveResizeWindow(disp, ib->scrollbarknob_win, -9999, -9999,
			     ib->bar_thickness, ib->knob_length);
	if (show_sb)
	  {
	     /* fix this area */
	     if (ib->scrollbar_side == 1)
		/* right */
	       {
		  /* start */
		  if (ib->arrow_side == 0)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 ib->w - ib->scroll_thickness, 0,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 ib->w - ib->scroll_thickness,
					 ib->arrow_thickness * 2,
					 ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2));
		    }
		  /* both ends */
		  else if (ib->arrow_side == 1)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 ib->w - ib->scroll_thickness, 0,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->scroll_thickness,
					 ib->h - ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 ib->w - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2));
		    }
		  /* end */
		  else if (ib->arrow_side == 2)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 ib->w - ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2),
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->scroll_thickness,
					 ib->h - ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 ib->w - ib->scroll_thickness, 0,
					 ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2));
		    }
		  /* no arrows */
		  else
		    {
		       EUnmapWindow(disp, ib->arrow1_win);
		       EUnmapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 ib->w - ib->scroll_thickness, 0,
					 ib->scroll_thickness, ib->h);
		    }
	       }
	     else
		/* left */
	       {
		  /* start */
		  if (ib->arrow_side == 0)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win, 0, 0,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win, 0,
					 ib->arrow_thickness * 2,
					 ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2));
		    }
		  /* both ends */
		  else if (ib->arrow_side == 1)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win, 0, 0,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win, 0,
					 ib->h - ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2));
		    }
		  /* end */
		  else if (ib->arrow_side == 2)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win, 0,
					 ib->h - (ib->arrow_thickness * 2),
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win, 0,
					 ib->h - ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win, 0, 0,
					 ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2));
		    }
		  /* no arrows */
		  else
		    {
		       EUnmapWindow(disp, ib->arrow1_win);
		       EUnmapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->scroll_win, 0, 0,
					 ib->scroll_thickness, ib->h);
		    }
	       }
	  }
	else
	  {
	     EMoveResizeWindow(disp, ib->scroll_win, -9999, -9999, 2, 2);
	     EMoveResizeWindow(disp, ib->arrow1_win, -9999, -9999, 2, 2);
	     EMoveResizeWindow(disp, ib->arrow2_win, -9999, -9999, 2, 2);
	  }
	EMoveResizeWindow(disp, ib->scrollbar_win,
			  (ib->scroll_thickness - ib->bar_thickness) / 2, bx,
			  ib->bar_thickness, bw);

	ic = FindItem("ICONBOX_SCROLLBAR_BASE_VERTICAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	   IclassApply(ic, ib->scroll_win, -1, -1, 0, 0, STATE_NORMAL, 0);
	ic = FindItem("ICONBOX_SCROLLBAR_KNOB_VERTICAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->scrollbar_hilited)
		state = STATE_HILITED;
	     if (ib->scrollbar_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->scrollbar_win, -1, -1, 0, 0, state, 0);
	  }
	ic = FindItem("ICONBOX_SCROLLKNOB_VERTICAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->scrollbar_hilited)
		state = STATE_HILITED;
	     if (ib->scrollbar_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->scrollbarknob_win, -1, -1, 0, 0, state, 0);
	  }
	ic = FindItem("ICONBOX_ARROW_UP", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->arrow1_hilited)
		state = STATE_HILITED;
	     if (ib->arrow1_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->arrow1_win, -1, -1, 0, 0, state, 0);
	  }
	ic = FindItem("ICONBOX_ARROW_DOWN", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->arrow2_hilited)
		state = STATE_HILITED;
	     if (ib->arrow2_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->arrow2_win, -1, -1, 0, 0, state, 0);
	  }
	/* remove this coment when fixed */
     }
   else
     {
	int                 bs, bw, bx;

	ic = FindItem("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ib->arrow_side < 3)
	   bs = ib->w - (ib->arrow_thickness * 2);
	else
	   bs = ib->w;
	bw = (ib->w * bs) / ib->max;
	if (ic)
	  {
	     bs -= (ic->padding.left + ic->padding.right);
	     bw = ((ib->w - (ic->padding.left + ic->padding.right)) * bs) /
		ib->max;
	  }
	if (bs < 1)
	   bs = 1;
	if (bw > bs)
	   bw = bs;
	if (bw < 1)
	   bw = 1;
	bx = ((ib->pos * bs) / ib->max);
	if (ic)
	   bx += ic->padding.left;
	if ((ib->scrollbar_hide) && (bw == bs))
	   show_sb = 0;
	ic = FindItem("ICONBOX_SCROLLKNOB_HORIZONTAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if ((ic) && (bw > ib->knob_length))
	   EMoveResizeWindow(disp, ib->scrollbarknob_win,
			     (bw - ib->knob_length) / 2, 0, ib->knob_length,
			     ib->bar_thickness);
	else
	   EMoveResizeWindow(disp, ib->scrollbarknob_win, -9999, -9999,
			     ib->knob_length, ib->bar_thickness);

	if (show_sb)
	  {
	     if (ib->scrollbar_side == 1)
		/* bottom */
	       {
		  /* start */
		  if (ib->arrow_side == 0)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win, 0,
					 ib->h - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->arrow_thickness,
					 ib->h - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 ib->arrow_thickness * 2,
					 ib->h - ib->scroll_thickness,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->scroll_thickness);
		    }
		  /* both ends */
		  else if (ib->arrow_side == 1)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win, 0,
					 ib->h - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->arrow_thickness,
					 ib->h - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 ib->arrow_thickness,
					 ib->h - ib->scroll_thickness,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->scroll_thickness);
		    }
		  /* end */
		  else if (ib->arrow_side == 2)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->h - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->arrow_thickness,
					 ib->h - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win, 0,
					 ib->h - ib->scroll_thickness,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->scroll_thickness);
		    }
		  /* no arrows */
		  else
		    {
		       EUnmapWindow(disp, ib->arrow1_win);
		       EUnmapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->scroll_win, 0,
					 ib->h - ib->scroll_thickness, ib->w,
					 ib->scroll_thickness);
		    }
	       }
	     else
		/* top */
	       {
		  /* start */
		  if (ib->arrow_side == 0)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win, 0, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->arrow_thickness, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 ib->arrow_thickness * 2, 0,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->scroll_thickness);
		    }
		  /* both ends */
		  else if (ib->arrow_side == 1)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win, 0, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->arrow_thickness, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win,
					 ib->arrow_thickness, 0,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->scroll_thickness);
		    }
		  /* end */
		  else if (ib->arrow_side == 2)
		    {
		       EMapWindow(disp, ib->arrow1_win);
		       EMapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->arrow1_win,
					 ib->w - (ib->arrow_thickness * 2), 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->arrow2_win,
					 ib->w - ib->arrow_thickness, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(disp, ib->scroll_win, 0, 0,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->scroll_thickness);
		    }
		  /* no arrows */
		  else
		    {
		       EUnmapWindow(disp, ib->arrow1_win);
		       EUnmapWindow(disp, ib->arrow2_win);
		       EMoveResizeWindow(disp, ib->scroll_win, 0, 0, ib->w,
					 ib->scroll_thickness);
		    }
	       }
	  }
	else
	  {
	     EMoveResizeWindow(disp, ib->scroll_win, -9999, -9999, 2, 2);
	     EMoveResizeWindow(disp, ib->arrow1_win, -9999, -9999, 2, 2);
	     EMoveResizeWindow(disp, ib->arrow2_win, -9999, -9999, 2, 2);
	  }

	EMoveResizeWindow(disp, ib->scrollbar_win, bx,
			  (ib->scroll_thickness - ib->bar_thickness) / 2, bw,
			  ib->bar_thickness);

	ic = FindItem("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	   IclassApply(ic, ib->scroll_win, -1, -1, 0, 0, STATE_NORMAL, 0);
	ic = FindItem("ICONBOX_SCROLLBAR_KNOB_HORIZONTAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->scrollbar_hilited)
		state = STATE_HILITED;
	     if (ib->scrollbar_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->scrollbar_win, -1, -1, 0, 0, state, 0);
	  }
	ic = FindItem("ICONBOX_SCROLLKNOB_HORIZONTAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->scrollbar_hilited)
		state = STATE_HILITED;
	     if (ib->scrollbar_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->scrollbarknob_win, -1, -1, 0, 0, state, 0);
	  }
	ic = FindItem("ICONBOX_ARROW_LEFT", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->arrow1_hilited)
		state = STATE_HILITED;
	     if (ib->arrow1_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->arrow1_win, -1, -1, 0, 0, state, 0);
	  }
	ic = FindItem("ICONBOX_ARROW_RIGHT", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->arrow2_hilited)
		state = STATE_HILITED;
	     if (ib->arrow2_clicked)
		state = STATE_CLICKED;
	     IclassApply(ic, ib->arrow2_win, -1, -1, 0, 0, state, 0);
	  }
     }
   PropagateShapes(ib->win);
   if (ib->ewin)
     {
	Border             *b;

	b = ib->ewin->border;
	SyncBorderToEwin(ib->ewin);
	if (ib->ewin->border == b)
	   PropagateShapes(ib->ewin->win);
     }
}

static void
IB_FixPos(Iconbox * ib)
{
   if (ib->orientation)
     {
	ImageClass         *ic;
	int                 v = 0;

	ic = FindItem("ICONBOX_SCROLLBAR_BASE_VERTICAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	v = ib->max - ib->h;
	if (ic)
	   v += ic->padding.top + ic->padding.bottom;
	if (ib->pos > v)
	   ib->pos = v;
     }
   else
     {
	ImageClass         *ic;
	int                 v = 0;

	ic = FindItem("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 0, LIST_FINDBY_NAME,
		      LIST_TYPE_ICLASS);
	v = ib->max - ib->w;
	if (ic)
	   v += ic->padding.left + ic->padding.right;
	if (ib->pos > v)
	   ib->pos = v;
     }
   if (ib->pos < 0)
      ib->pos = 0;

}

void
IconboxRedraw(Iconbox * ib)
{
   Pixmap              m = 0;
   char                pq;
   char                was_shaded = 0;
   int                 i, x, y, w, h;
   ImageClass         *ib_ic_cover, *ib_ic_box;
   int                 ib_x0, ib_y0, ib_xlt, ib_ylt, ib_ww, ib_hh;

   if (!ib)
      return;
   if (!ib->ewin)
      return;

   if (ib->orientation)
     {
	ib_ic_box = FindItem("ICONBOX_VERTICAL", 0,
			     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
     }
   else
     {
	ib_ic_box = FindItem("ICONBOX_HORIZONTAL", 0,
			     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
     }

   x = ib->ewin->x;
   y = ib->ewin->y;
   w = ib->w;
   h = ib->h;

   if (ib->auto_resize)
     {
	int                 add = 0;

	if (ib->ewin->shaded)
	  {
	     was_shaded = 1;
	     UnShadeEwin(ib->ewin);
	  }

	IB_CalcMax(ib);
	if (ib->orientation)
	  {
	     if (ib_ic_box)
		add = ib_ic_box->padding.top + ib_ic_box->padding.bottom;
	     add += ib->max;
	     if (ib->ewin->border)
	       {
		  if ((ib->ewin->border->border.top +
		       ib->ewin->border->border.bottom + add) > root.h)
		     add =
			root.h - (ib->ewin->border->border.top +
				  ib->ewin->border->border.bottom);
	       }
	     y += (((ib->ewin->client.h - add) * ib->auto_resize_anchor) >> 10);
	     h = add;
	     if (ib->ewin->border)
	       {
		  if ((ib->ewin->y + ib->ewin->border->border.top +
		       ib->ewin->border->border.bottom + add) > root.h)
		     y = root.h - (ib->ewin->border->border.top +
				   ib->ewin->border->border.bottom + add);
	       }
	  }
	else
	  {
	     if (ib_ic_box)
		add = ib_ic_box->padding.left + ib_ic_box->padding.right;
	     add += ib->max;
	     if (ib->ewin->border)
	       {
		  if ((ib->ewin->border->border.left +
		       ib->ewin->border->border.right + add) > root.w)
		     add =
			root.w - (ib->ewin->border->border.left +
				  ib->ewin->border->border.right);
	       }
	     x += (((ib->ewin->client.w - add) * ib->auto_resize_anchor) >> 10);
	     w = add;
	     if (ib->ewin->border)
	       {
		  if ((ib->ewin->x + ib->ewin->border->border.left +
		       ib->ewin->border->border.right + add) > root.w)
		     x = root.w - (ib->ewin->border->border.left +
				   ib->ewin->border->border.right + add);
	       }
	  }
     }

   if (ib->force_update || (x != ib->ewin->x) || (y != ib->ewin->y)
       || (w != ib->ewin->client.w) || (h != ib->ewin->client.h))
     {
	ib->w = w;
	ib->h = h;
	MoveResizeEwin(ib->ewin, x, y, w, h);
	EResizeWindow(disp, ib->win, w, h);
	EFreePixmap(disp, ib->pmap);
	ib->pmap = ECreatePixmap(disp, ib->icon_win, w, h, root.depth);
	RememberImportantInfoForEwins(ib->ewin);
	ib->force_update = 0;
     }

   if (was_shaded)
      ShadeEwin(ib->ewin);

   pq = queue_up;
   queue_up = 0;

   IB_CalcMax(ib);
   IB_FixPos(ib);
   IB_DrawScroll(ib);

   if (ib->orientation)
     {
	ib_ic_cover = FindItem("ICONBOX_COVER_VERTICAL", 0,
			       LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	ib_x0 = 0;
	ib_y0 = -ib->pos;
	ib_xlt = (ib->scrollbar_side == 1) ? 0 : ib->scroll_thickness;
	ib_ylt = 0;
	ib_ww = ib->w - ib->scroll_thickness;
	ib_hh = ib->h;
     }
   else
     {
	ib_ic_cover = FindItem("ICONBOX_COVER_HORIZONTAL", 0,
			       LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	ib_x0 = -ib->pos;
	ib_y0 = 0;
	ib_xlt = 0;
	ib_ylt = (ib->scrollbar_side == 1) ? 0 : ib->scroll_thickness;
	ib_ww = ib->w;
	ib_hh = ib->h - ib->scroll_thickness;
     }

   EMoveResizeWindow(disp, ib->icon_win, ib_xlt, ib_ylt, ib_ww, ib_hh);

   if (ib_ic_cover && (!(ib->cover_hide)))
     {
	EMoveResizeWindow(disp, ib->cover_win, ib_xlt, ib_ylt, ib_ww, ib_hh);
	EMapWindow(disp, ib->cover_win);
	IclassApply(ib_ic_cover, ib->cover_win, -1, -1, 0, 0, STATE_NORMAL, 0);
     }
   else
     {
	EMoveResizeWindow(disp, ib->cover_win, -30000, -30000, 2, 2);
	EUnmapWindow(disp, ib->cover_win);
     }

   if (!ib->nobg)
     {
	if (ib_ic_box)
	  {
	     PmapMask            pmm;

	     GetWinWH(ib->icon_win, (unsigned int *)&w, (unsigned int *)&h);
	     IclassApplyCopy(ib_ic_box, ib->icon_win, w, h, 0, 0, STATE_NORMAL,
			     &pmm, 1);
	     EShapeCombineMask(disp, ib->icon_win, ShapeBounding, 0, 0,
			       pmm.mask, ShapeSet);
	     PastePixmap(disp, ib->pmap, pmm.pmap, pmm.mask, 0, 0);
	     FreePmapMask(&pmm);
	  }
	/* Else what ? */
     }
   else
     {
	GC                  gc;
	XGCValues           gcv;

	GetWinWH(ib->icon_win, (unsigned int *)&w, (unsigned int *)&h);
	m = ECreatePixmap(disp, ib->icon_win, w, h, 1);
	gc = XCreateGC(disp, m, 0, &gcv);
	XSetForeground(disp, gc, 0);
	XFillRectangle(disp, m, gc, 0, 0, w, h);
	XFreeGC(disp, gc);
     }

   x = ib_x0;
   y = ib_y0;
   if (ib_ic_box)
     {
	x += ib_ic_box->padding.left;
	y += ib_ic_box->padding.top;
     }

   for (i = 0; i < ib->num_icons; i++)
     {
	EWin               *ewin;

	w = 8;
	h = 8;

	ewin = ib->icons[i];

	if (!ewin->icon_pmm.pmap)
	   UpdateAppIcon(ewin, ib->icon_mode);
	if (ewin->icon_pmm.pmap)
	  {
	     int                 xoff, yoff;

	     w = ewin->icon_w;
	     h = ewin->icon_h;
	     if (ib->orientation)
	       {
		  xoff = (ib->iconsize - w) / 2;
		  yoff = 0;
	       }
	     else
	       {
		  xoff = 0;
		  yoff = (ib->iconsize - h) / 2;
	       }
	     if (ib->draw_icon_base)
	       {
		  IB_PasteDefaultBase(ib->pmap, x, y, ib->iconsize,
				      ib->iconsize);
		  if (ib->nobg)
		     IB_PasteDefaultBaseMask(m, x, y, ib->iconsize,
					     ib->iconsize);
	       }

	     if (ib->draw_icon_base)
		PastePixmap(disp, ib->pmap, ewin->icon_pmm.pmap,
			    ewin->icon_pmm.mask,
			    x + ((ib->iconsize - w) / 2),
			    y + ((ib->iconsize - h) / 2));
	     else
		PastePixmap(disp, ib->pmap, ewin->icon_pmm.pmap,
			    ewin->icon_pmm.mask, x + xoff, y + yoff);

	     if (ib->nobg)
		PasteMask(disp, m, ewin->icon_pmm.mask,
			  x + xoff, y + yoff, w, h);
	  }

	if (ib->orientation)
	   y += (ib->draw_icon_base) ? ib->iconsize : h + 2;
	else
	   x += (ib->draw_icon_base) ? ib->iconsize : w + 2;
     }

   if (ib->nobg)
     {
	EShapeCombineMask(disp, ib->icon_win, ShapeBounding, 0, 0, m, ShapeSet);
	EFreePixmap(disp, m);
	if (ib->num_icons == 0)
	   EMoveWindow(disp, ib->icon_win, -ib->w, -ib->h);
     }
   ESetWindowBackgroundPixmap(disp, ib->icon_win, ib->pmap);
   XClearWindow(disp, ib->icon_win);

   PropagateShapes(ib->win);
   ICCCM_GetShapeInfo(ib->ewin);
   PropagateShapes(ib->ewin->win);

   queue_up = pq;
}

void
IconboxResize(Iconbox * ib, int w, int h)
{
   if ((ib->w == w) && (ib->h == h))
      return;

   ib->w = w;
   ib->h = h;
   ib->force_update = 1;
   IconboxRedraw(ib);
}

static void
IB_Scroll(Iconbox * ib, int dir)
{
   ib->pos += dir;
   IB_FixPos(ib);
   IconboxRedraw(ib);
}

static void
IB_ShowMenu(Iconbox * ib, int x, int y)
{
   static Menu        *p_menu = NULL;
   MenuItem           *mi;
   char                s[1024];

   if (p_menu)
      DestroyMenu(p_menu);
   p_menu = CreateMenu();

   AddTitleToMenu(p_menu, _("Iconbox Options"));
   p_menu->name = duplicate("__IBOX_MENU");
   p_menu->style =
      FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
   Esnprintf(s, sizeof(s), "iconbox %s", ib->name);
   mi = CreateMenuItem(_("This Iconbox Settings..."), NULL, ACTION_CONFIG, s,
		       NULL);
   AddItemToMenu(p_menu, mi);
   mi = CreateMenuItem(_("Close Iconbox"), NULL, ACTION_KILL, NULL, NULL);
   AddItemToMenu(p_menu, mi);
   mi = CreateMenuItem(_("Create New Iconbox"), NULL, ACTION_CREATE_ICONBOX,
		       NULL, NULL);
   AddItemToMenu(p_menu, mi);
   AddItem(p_menu, p_menu->name, 0, LIST_TYPE_MENU);
   Esnprintf(s, sizeof(s), "named %s", p_menu->name);
   spawnMenu(s);
   x = 0;
   y = 0;
}

void
IB_CompleteRedraw(Iconbox * ib)
{
   IB_Reconfigure(ib);
   IconboxRedraw(ib);
}

void
IB_Setup(void)
{
   EWin              **lst;
   int                 i, num;
   Iconbox           **ibl;

   IcondefChecker(0, NULL);
   ibl = ListAllIconboxes(&num);
   if (ibl)
     {
	for (i = 0; i < num; i++)
	   IconboxShow(ibl[i]);
	Efree(ibl);
     }
   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     if (lst[i]->client.start_iconified)
		IconifyEwin(lst[i]);
	  }
	Efree(lst);
     }
}

void
IconboxesHandleEvent(XEvent * ev)
{
   Iconbox           **ib;
   int                 i, num;

   if (mode.mode != MODE_NONE)
      return;

   ib = ListAllIconboxes(&num);
   if (!ib)
      return;

   for (i = 0; i < num; i++)
     {
	if (ev->xany.window == ib[i]->scroll_win)
	  {
	     if (ev->type == ButtonPress)
	       {
		  if (ev->xbutton.button == 1)
		     ib[i]->scrollbox_clicked = 1;
		  else if (ev->xbutton.button == 3)
		     IB_ShowMenu(ib[i], ev->xbutton.x, ev->xbutton.y);
	       }
	     else if ((ev->type == ButtonRelease) && (ib[i]->scrollbox_clicked))
	       {
		  int                 x, y, w, h;

		  ib[i]->scrollbox_clicked = 0;
		  GetWinXY(ib[i]->scrollbar_win, &x, &y);
		  GetWinWH(ib[i]->scrollbar_win, (unsigned int *)&w,
			   (unsigned int *)&h);
		  if (ev->xbutton.x < x)
		     IB_Scroll(ib[i], -8);
		  if (ev->xbutton.x > (x + w))
		     IB_Scroll(ib[i], 8);
	       }
	  }
	if (ev->xany.window == ib[i]->scrollbar_win)
	  {
	     static int          px, py;

	     if (ev->type == ButtonPress)
	       {
		  if (ev->xbutton.button == 1)
		    {
		       px = ev->xbutton.x_root;
		       py = ev->xbutton.y_root;
		       ib[i]->scrollbar_clicked = 1;
		    }
		  else if (ev->xbutton.button == 3)
		     IB_ShowMenu(ib[i], ev->xbutton.x, ev->xbutton.y);
	       }
	     else if ((ev->type == ButtonRelease) && (ib[i]->scrollbar_clicked))
		ib[i]->scrollbar_clicked = 0;
	     else if (ev->type == EnterNotify)
		ib[i]->scrollbar_hilited = 1;
	     else if (ev->type == LeaveNotify)
		ib[i]->scrollbar_hilited = 0;
	     else if ((ev->type == MotionNotify) && (ib[i]->scrollbar_clicked))
	       {
		  int                 dx, dy, bs, x, y;
		  ImageClass         *ic;

		  dx = ev->xmotion.x_root - px;
		  dy = ev->xmotion.y_root - py;
		  px = ev->xmotion.x_root;
		  py = ev->xmotion.y_root;

		  if (ib[i]->orientation)
		    {
		       ic = FindItem("ICONBOX_SCROLLBAR_BASE_VERTICAL", 0,
				     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       GetWinXY(ib[i]->scrollbar_win, &x, &y);
		       bs = ib[i]->h - (ib[i]->arrow_thickness * 2);
		       if (ic)
			 {
			    bs -= (ic->padding.top + ic->padding.bottom);
			    y -= ic->padding.top;
			 }
		       if (bs < 1)
			  bs = 1;
		       ib[i]->pos = ((y + dy + 1) * ib[i]->max) / bs;
		       IB_FixPos(ib[i]);
		       IconboxRedraw(ib[i]);
		    }
		  else
		    {
		       ic = FindItem("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 0,
				     LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       GetWinXY(ib[i]->scrollbar_win, &x, &y);
		       bs = ib[i]->w - (ib[i]->arrow_thickness * 2);
		       if (ic)
			 {
			    bs -= (ic->padding.left + ic->padding.right);
			    x -= ic->padding.left;
			 }
		       if (bs < 1)
			  bs = 1;
		       ib[i]->pos = ((x + dx + 1) * ib[i]->max) / bs;
		       IB_FixPos(ib[i]);
		       IconboxRedraw(ib[i]);
		    }
	       }
	     IB_DrawScroll(ib[i]);
	  }
	else if (ev->xany.window == ib[i]->cover_win)
	  {
	     if (ev->type == ButtonPress)
		IB_ShowMenu(ib[i], ev->xbutton.x, ev->xbutton.y);
	  }
	else if (ev->xany.window == ib[i]->arrow1_win)
	  {
	     if (ev->type == ButtonPress)
	       {
		  if (ev->xbutton.button == 1)
		     ib[i]->arrow1_clicked = 1;
		  else if (ev->xbutton.button == 3)
		     IB_ShowMenu(ib[i], ev->xbutton.x, ev->xbutton.y);
	       }
	     else if ((ev->type == ButtonRelease) && (ib[i]->arrow1_clicked))
	       {
		  ib[i]->arrow1_clicked = 0;
		  IB_Scroll(ib[i], -8);
	       }
	     else if (ev->type == EnterNotify)
		ib[i]->arrow1_hilited = 1;
	     else if (ev->type == LeaveNotify)
		ib[i]->arrow1_hilited = 0;
	     IB_DrawScroll(ib[i]);
	  }
	else if (ev->xany.window == ib[i]->arrow2_win)
	  {
	     if (ev->type == ButtonPress)
	       {
		  if (ev->xbutton.button == 1)
		     ib[i]->arrow2_clicked = 1;
		  else if (ev->xbutton.button == 3)
		     IB_ShowMenu(ib[i], ev->xbutton.x, ev->xbutton.y);
	       }
	     else if ((ev->type == ButtonRelease) && (ib[i]->arrow2_clicked))
	       {
		  ib[i]->arrow2_clicked = 0;
		  IB_Scroll(ib[i], 8);
	       }
	     else if (ev->type == EnterNotify)
		ib[i]->arrow2_hilited = 1;
	     else if (ev->type == LeaveNotify)
		ib[i]->arrow2_hilited = 0;
	     IB_DrawScroll(ib[i]);
	  }
	else if (ev->xany.window == ib[i]->icon_win)
	  {
	     static EWin        *name_ewin = NULL;

	     if ((ev->type == MotionNotify) || (ev->type == EnterNotify))
	       {
		  EWin               *ewin = NULL;
		  ToolTip            *tt = NULL;

		  if (ev->type == MotionNotify)
		    {
		       ewin = IB_FindIcon(ib[i], ev->xmotion.x, ev->xmotion.y);
		       mode.x = ev->xmotion.x_root;
		       mode.y = ev->xmotion.y_root;
		    }
		  else
		    {
		       ewin =
			  IB_FindIcon(ib[i], ev->xcrossing.x, ev->xcrossing.y);
		       mode.x = ev->xcrossing.x_root;
		       mode.y = ev->xcrossing.y_root;
		    }
		  if (ewin != name_ewin)
		    {
		       if (ib[i]->shownames)
			 {
			    tt = FindItem("ICONBOX", 0, LIST_FINDBY_NAME,
					  LIST_TYPE_TOOLTIP);
			    if (tt)
			      {
				 name_ewin = ewin;
				 HideToolTip(tt);
				 if (ewin)
				   {
				      if ((ewin->client.icon_name)
					  && (strlen(ewin->client.icon_name) >
					      0))
					 ShowToolTip(tt, ewin->client.icon_name,
						     NULL, mode.x, mode.y);
				      else
					 ShowToolTip(tt, ewin->client.title,
						     NULL, mode.x, mode.y);
				   }
			      }
			 }
		    }
	       }
	     else if (ev->type == LeaveNotify)
	       {
		  ToolTip            *tt = NULL;

		  tt = FindItem("ICONBOX", 0, LIST_FINDBY_NAME,
				LIST_TYPE_TOOLTIP);
		  if (tt)
		    {
		       HideToolTip(tt);
		       name_ewin = NULL;
		    }
	       }
	     else if (ev->type == ButtonPress)
	       {
		  if (ev->xbutton.button == 1)
		     ib[i]->icon_clicked = 1;
		  else if (ev->xbutton.button == 3)
		     IB_ShowMenu(ib[i], ev->xbutton.x, ev->xbutton.y);
	       }
	     else if ((ev->type == ButtonRelease) && (ib[i]->icon_clicked))
	       {
		  EWin               *ewin;
		  EWin              **gwins;
		  int                 j, num;
		  char                iconified;

		  ib[i]->icon_clicked = 0;
		  ewin = IB_FindIcon(ib[i], ev->xbutton.x, ev->xbutton.y);
		  if (ewin)
		    {
		       ToolTip            *tt = NULL;

		       tt = FindItem("ICONBOX", 0, LIST_FINDBY_NAME,
				     LIST_TYPE_TOOLTIP);
		       if (tt)
			  HideToolTip(tt);
		       gwins =
			  ListWinGroupMembersForEwin(ewin, ACTION_ICONIFY,
						     mode.nogroup, &num);
		       iconified = ewin->iconified;

		       if (gwins)
			 {
			    for (j = 0; j < num; j++)
			      {
				 if ((gwins[j]->iconified) && (iconified))
				    DeIconifyEwin(gwins[j]);
			      }
			    Efree(gwins);
			 }
		    }
	       }
	  }
     }
   Efree(ib);
}
