/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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

struct _pager
{
   char               *name;
   Window              win;
   Pixmap              pmap;
   PmapMask            bgpmap;
   int                 desktop;
   int                 w, h;
   int                 dw, dh;
   char                visible;
   int                 update_phase;
   EWin               *ewin;
   Window              sel_win;
   char                hi_visible;
   Window              hi_win;
   EWin               *hi_ewin;
   int                 hi_win_w, hi_win_h;
};

#define PAGER_EVENT_MOUSE_OUT -1
#define PAGER_EVENT_MOTION     0
#define PAGER_EVENT_MOUSE_IN   1

static void         PagerDrawQueueCallback(DrawQueue * dq);

static void         PagerEwinUpdateFromPager(Pager * p, EWin * ewin);

static void         PagerEventMainWin(XEvent * ev, void *prm);
static void         PagerEventHiWin(XEvent * ev, void *prm);

static Pager       *mode_context_pager = NULL;

static Pager       *
PagerCreate(void)
{
   Pager              *p;
   int                 ax, ay;
   char                pq;
   ImageClass         *ic;
   XSetWindowAttributes attr;
   static char         did_dialog = 0;

   if (!Conf.pagers.enable)
      return NULL;

   if ((!did_dialog) && (Conf.pagers.snap))
      did_dialog = 1;

   GetAreaSize(&ax, &ay);
   p = Ecalloc(1, sizeof(Pager));
   p->name = NULL;
   attr.colormap = VRoot.cmap;
   attr.border_pixel = 0;
   attr.background_pixel = 0;
   attr.save_under = False;
   p->w = ((48 * VRoot.w) / VRoot.h) * ax;
   p->h = 48 * ay;
   p->dw = ((48 * VRoot.w) / VRoot.h);
   p->dh = 48;
   p->win = ECreateWindow(VRoot.win, 0, 0, p->w, p->h, 0);
   EventCallbackRegister(p->win, 0, PagerEventMainWin, p);
   p->pmap = ECreatePixmap(p->win, p->w, p->h, VRoot.depth);
   ESetWindowBackgroundPixmap(p->win, p->pmap);
   p->hi_win = ECreateWindow(VRoot.win, 0, 0, 3, 3, 0);
   EventCallbackRegister(p->hi_win, 0, PagerEventHiWin, p);
   p->hi_visible = 0;
   p->hi_ewin = NULL;
   ESelectInput(p->hi_win,
		ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
		EnterWindowMask | LeaveWindowMask);
   p->desktop = 0;
   p->visible = 0;
   p->update_phase = 0;
   p->ewin = NULL;
   p->sel_win = ECreateWindow(p->win, 0, 0, p->w / ax, p->h / ay, 0);
   pq = Mode.queue_up;
   Mode.queue_up = 0;
   ic = ImageclassFind("PAGER_SEL", 0);
   if (ic)
      ImageclassApply(ic, p->sel_win, p->w / ax, p->h / ay, 0, 0, STATE_NORMAL,
		      0, ST_PAGER);
   Mode.queue_up = pq;
   return p;
}

static void
PagerDestroy(Pager * p)
{
   char                s[4096];

   RemoveItem("PAGER", p->win, LIST_FINDBY_ID, LIST_TYPE_PAGER);
   Esnprintf(s, sizeof(s), "__.%x", (unsigned)p->win);
   RemoveTimerEvent(s);
   if (p->name)
      Efree(p->name);
   EDestroyWindow(p->win);
   if (p->hi_win)
      EDestroyWindow(p->hi_win);
   if (p->pmap)
      EFreePixmap(p->pmap);
   FreePmapMask(&p->bgpmap);
   Efree(p);
}

static void
ScaleRect(Window src, Pixmap dst, Pixmap * pdst, int sx, int sy, int sw, int sh,
	  int dx, int dy, int dw, int dh, int scale)
{
   Imlib_Image        *im;
   Pixmap              pmap, mask;
   GC                  gc;

   scale = (scale) ? 2 : 1;

   imlib_context_set_drawable(src);
   im = imlib_create_scaled_image_from_drawable(None, sx, sy, sw, sh,
						scale * dw, scale * dh, 1, 0);
   imlib_context_set_image(im);
   imlib_context_set_anti_alias(1);
   imlib_render_pixmaps_for_whole_image_at_size(&pmap, &mask, dw, dh);
   if (pdst)
     {
	*pdst = pmap;
     }
   else
     {
	gc = ECreateGC(dst, 0, NULL);
	XCopyArea(disp, pmap, dst, gc, 0, 0, dw, dh, dx, dy);
	EFreeGC(gc);
	imlib_free_pixmap_and_mask(pmap);
     }
   imlib_free_image();
}

static void
PagerUpdateTimeout(int val __UNUSED__, void *data)
{
   Pager              *p;
   char                s[4096];
   static double       last_time = 0.0;
   double              cur_time, in;
   static int          calls = 0;
   int                 y, y2, phase, ax, ay, cx, cy, ww, hh, xx, yy;
   static int          offsets[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };

   p = (Pager *) data;
   Esnprintf(s, sizeof(s), "__.%x", (unsigned)p->win);
   /* prevent runaway pager timeouts - dont knwo how it happens - but hack */
   /* around to stop it */
   cur_time = GetTime();
   if ((cur_time - last_time) < 0.05)
      calls++;
   last_time = cur_time;
   in = 1 / ((double)Conf.pagers.scanspeed);
   if (calls > 50)
     {
	calls = 0;
	in = 0.25;
     }
   if (Conf.pagers.scanspeed > 0)
      DoIn(s, in, PagerUpdateTimeout, 0, p);
   if (!Conf.pagers.snap)
      return;
   if (!p->visible)
      return;
   if (p->desktop != DesksGetCurrent())
      return;
   if (p->ewin && p->ewin->visibility == VisibilityFullyObscured)
      return;
   if (Mode.mode != MODE_NONE)
      return;

   GetAreaSize(&ax, &ay);
   DeskGetCurrentArea(&cx, &cy);
   ww = p->w / ax;
   hh = p->h / ay;
   xx = cx * ww;
   yy = cy * hh;
   phase = p->update_phase;
#if 0
   /* Due to a bug in imlib2 <= 1.2.0 we have to scan left->right in stead
    * of top->bottom, at least for now. */
   y = ((phase & 0xfffffff8) + offsets[phase % 8]) % hh;
   y2 = (y * VRoot.h) / hh;

   ScaleRect(VRoot.win, p->pmap, NULL, 0, y2, VRoot.w, VRoot.h / hh,
	     xx, yy + y, ww, 1, Conf.pagers.hiq);
   EClearArea(p->win, xx, yy + y, ww, 1, False);
   y2 = p->h;
#else
   y = ((phase & 0xfffffff8) + offsets[phase % 8]) % ww;
   y2 = (y * VRoot.w) / ww;

   ScaleRect(VRoot.win, p->pmap, NULL, y2, 0, VRoot.w / ww, VRoot.h,
	     xx + y, yy, 1, hh, Conf.pagers.hiq);
   EClearArea(p->win, xx + y, yy, 1, hh, False);
   y2 = p->w;
#endif
   p->update_phase++;
   if (p->update_phase >= y2)
     {
	int                 i, num;
	EWin               *const *lst;

	lst = EwinListGetForDesk(&num, p->desktop);
	for (i = 0; i < num; i++)
	   PagerEwinUpdateFromPager(p, lst[i]);

	p->update_phase = 0;
     }
}

static void
PagerUpdateHiWin(Pager * p, EWin * ewin)
{
   Imlib_Image        *im;

   if (!p->hi_visible || !ewin->mini_pmm.pmap)
      return;

   imlib_context_set_drawable(ewin->mini_pmm.pmap);
   im = imlib_create_image_from_drawable(0, 0, 0,
					 ewin->mini_w, ewin->mini_h, 0);
   imlib_context_set_image(im);
   imlib_context_set_drawable(p->hi_win);
   imlib_render_image_on_drawable_at_size(0, 0, p->hi_win_w, p->hi_win_h);
   imlib_free_image_and_decache();
}

static void
PagerEwinUpdateMini(Pager * p, EWin * ewin)
{
   int                 w, h, ax, ay, cx, cy;

   if (!Conf.pagers.enable)
      return;

   GetAreaSize(&ax, &ay);
   DeskGetArea(p->desktop, &cx, &cy);

   w = ((EoGetW(ewin)) * (p->w / ax)) / VRoot.w;
   h = ((EoGetH(ewin)) * (p->h / ay)) / VRoot.h;

   if (w < 1)
      w = 1;
   if (h < 1)
      h = 1;
   if ((ewin->mini_w != w) || (ewin->mini_h != h))
     {
	FreePmapMask(&ewin->mini_pmm);

	ewin->mini_w = w;
	ewin->mini_h = h;

	if ((EoGetDesk(ewin) != DesksGetCurrent()) || (ewin->area_x != cx)
	    || (ewin->area_y != cy) || (!Conf.pagers.snap))
	  {
	     ImageClass         *ic = NULL;

	     ic = ImageclassFind("PAGER_WIN", 0);
	     if (ic)
		ImageclassApplyCopy(ic, EoGetWin(ewin), w, h, 0, 0,
				    STATE_NORMAL, &ewin->mini_pmm, 1, ST_PAGER);
	  }
	else
	  {
	     ewin->mini_pmm.type = 1;
	     ewin->mini_pmm.mask = None;
	     ScaleRect(EoGetWin(ewin), None, &ewin->mini_pmm.pmap, 0, 0,
		       EoGetW(ewin), EoGetH(ewin), 0, 0, w, h, Conf.pagers.hiq);
	  }
     }

   if (ewin == p->hi_ewin)
      PagerUpdateHiWin(p, ewin);
}

static void
PagerRedraw(Pager * p, char newbg)
{
   int                 x, y, ax, ay, cx, cy;
   GC                  gc;
   EWin               *const *lst;
   int                 i, num;

   if (!Conf.pagers.enable || Mode.mode == MODE_DESKSWITCH)
      return;

   if (Mode.queue_up)
     {
	DrawQueue          *dq;

	dq = Ecalloc(1, sizeof(DrawQueue));
	dq->func = PagerDrawQueueCallback;
	dq->win = p->win;
	dq->redraw_pager = p;
	dq->newbg = newbg;
	AddItem(dq, "DRAW", dq->win, LIST_TYPE_DRAW);
	return;
     }

   /* Desk may be gone */
   if (p->desktop >= DesksGetNumber())
      return;

   p->update_phase = 0;
   GetAreaSize(&ax, &ay);
   DeskGetArea(p->desktop, &cx, &cy);

   gc = ECreateGC(p->pmap, 0, NULL);
   if (gc)
     {
	if ((newbg > 0) && (newbg < 3))
	  {
	     FreePmapMask(&p->bgpmap);

	     if (!Conf.pagers.snap)
	       {
		  ImageClass         *ic = NULL;

		  ic = ImageclassFind("PAGER_BACKGROUND", 0);
		  if (ic)
		     ImageclassApplyCopy(ic, p->win, p->w / ax, p->h / ay, 0, 0,
					 STATE_NORMAL, &p->bgpmap, 0, ST_PAGER);
	       }
	     else
	       {
		  Background         *bg;

		  bg = DeskGetBackground(p->desktop);
		  if (bg)
		    {
		       char                s[4096];
		       char               *uniq;
		       Imlib_Image        *im;

		       uniq = BackgroundGetUniqueString(bg);
		       Esnprintf(s, sizeof(s), "%s/cached/pager/%s.%i.%i.%s",
				 EDirUserCache(), BackgroundGetName(bg),
				 (p->w / ax), (p->h / ay), uniq);
		       Efree(uniq);

		       im = imlib_load_image(s);
		       if (im)
			 {
			    imlib_context_set_image(im);
			    p->bgpmap.type = 1;
			    imlib_render_pixmaps_for_whole_image_at_size(&p->
									 bgpmap.
									 pmap,
									 &p->
									 bgpmap.
									 mask,
									 (p->w /
									  ax),
									 (p->h /
									  ay));
			    imlib_free_image_and_decache();
			 }
		       else
			 {
			    p->bgpmap.type = 0;
			    p->bgpmap.pmap =
			       ECreatePixmap(p->win, p->w / ax, p->h / ay,
					     VRoot.depth);
			    p->bgpmap.mask = None;
			    BackgroundApply(bg, p->bgpmap.pmap, 0);
			    imlib_context_set_drawable(p->bgpmap.pmap);
			    im =
			       imlib_create_image_from_drawable(0, 0, 0,
								(p->w / ax),
								(p->h / ay), 0);
			    imlib_context_set_image(im);
			    imlib_image_set_format("png");
			    imlib_save_image(s);
			    imlib_free_image_and_decache();
			 }
		    }
		  else
		    {
		       p->bgpmap.type = 0;
		       p->bgpmap.pmap =
			  ECreatePixmap(p->win, p->w / ax, p->h / ay,
					VRoot.depth);
		       p->bgpmap.mask = None;
		       XSetForeground(disp, gc, BlackPixel(disp, VRoot.scr));
		       XDrawRectangle(disp, p->bgpmap.pmap, gc, 0, 0, p->dw,
				      p->dh);
		       XSetForeground(disp, gc, WhitePixel(disp, VRoot.scr));
		       XFillRectangle(disp, p->bgpmap.pmap, gc, 1, 1, p->dw - 2,
				      p->dh - 2);
		    }
	       }
	  }

	for (y = 0; y < ay; y++)
	  {
	     for (x = 0; x < ax; x++)
		XCopyArea(disp, p->bgpmap.pmap, p->pmap, gc, 0, 0, p->w / ax,
			  p->h / ay, x * (p->w / ax), y * (p->h / ay));
	  }

	lst = EwinListGetForDesk(&num, p->desktop);
	for (i = num - 1; i >= 0; i--)
	  {
	     EWin               *ewin;
	     int                 wx, wy, ww, wh;

	     ewin = lst[i];
	     if (!ewin->iconified && ewin->shown)
	       {
		  wx = ((EoGetX(ewin) +
			 (cx * VRoot.w)) * (p->w / ax)) / VRoot.w;
		  wy = ((EoGetY(ewin) +
			 (cy * VRoot.h)) * (p->h / ay)) / VRoot.h;
		  ww = ((EoGetW(ewin)) * (p->w / ax)) / VRoot.w;
		  wh = ((EoGetH(ewin)) * (p->h / ay)) / VRoot.h;
		  PagerEwinUpdateMini(p, ewin);
		  if (ewin->mini_pmm.pmap)
		    {
		       if (ewin->mini_pmm.mask)
			 {
			    XSetClipMask(disp, gc, ewin->mini_pmm.mask);
			    XSetClipOrigin(disp, gc, wx, wy);
			 }
		       XCopyArea(disp, ewin->mini_pmm.pmap, p->pmap, gc, 0, 0,
				 ww, wh, wx, wy);
		       if (ewin->mini_pmm.mask)
			  XSetClipMask(disp, gc, None);
		    }
		  else
		    {
		       XSetForeground(disp, gc, BlackPixel(disp, VRoot.scr));
		       XDrawRectangle(disp, p->pmap, gc, wx - 1, wy - 1, ww + 1,
				      wh + 1);
		       XSetForeground(disp, gc, WhitePixel(disp, VRoot.scr));
		       XFillRectangle(disp, p->pmap, gc, wx, wy, ww, wh);
		    }
	       }
	  }

	if (newbg < 2)
	  {
	     ESetWindowBackgroundPixmap(p->win, p->pmap);
	     EClearWindow(p->win);
	  }

	EFreeGC(gc);
     }
}

static void
PagerForceUpdate(Pager * p)
{
   int                 ww, hh, xx, yy, ax, ay, cx, cy;
   EWin               *const *lst;
   int                 i, num;

   if (!Conf.pagers.enable || Mode.mode == MODE_DESKSWITCH)
      return;

   if (Mode.queue_up)
     {
	DrawQueue          *dq;

	dq = Ecalloc(1, sizeof(DrawQueue));
	dq->func = PagerDrawQueueCallback;
	dq->win = p->win;
	dq->pager = p;
	AddItem(dq, "DRAW", dq->win, LIST_TYPE_DRAW);
	return;
     }

   /* Desk may be gone */
   if (p->desktop >= DesksGetNumber())
      return;

   if ((p->desktop != DesksGetCurrent()) || (!Conf.pagers.snap))
     {
	PagerRedraw(p, 0);
	return;
     }

   p->update_phase = 0;
   GetAreaSize(&ax, &ay);
   DeskGetArea(p->desktop, &cx, &cy);
   ww = p->w / ax;
   hh = p->h / ay;
   xx = cx * ww;
   yy = cy * hh;

   ScaleRect(VRoot.win, p->pmap, NULL, 0, 0, VRoot.w, VRoot.h, xx, yy, ww, hh,
	     Conf.pagers.hiq);
   EClearWindow(p->win);

   lst = EwinListGetForDesk(&num, p->desktop);
   for (i = 0; i < num; i++)
      PagerEwinUpdateFromPager(p, lst[i]);
}

static void
PagerDrawQueueCallback(DrawQueue * dq)
{
   if (dq->pager)
      PagerForceUpdate(dq->pager);
   else if (dq->redraw_pager)
      PagerRedraw(dq->redraw_pager, dq->newbg);
}

static void
PagerEwinMoveResize(EWin * ewin, int resize __UNUSED__)
{
   Pager              *p = ewin->data;
   int                 w, h;
   int                 ax, ay, cx, cy;
   char                pq;
   ImageClass         *ic;
   EWin               *const *lst;
   int                 i, num;

   if (!Conf.pagers.enable || !p)
      return;

   w = ewin->client.w;
   h = ewin->client.h;
   if ((w == p->w) && (h == p->h))
      return;

   GetAreaSize(&ax, &ay);
   EFreePixmap(p->pmap);
   FreePmapMask(&p->bgpmap);
   EResizeWindow(p->win, w, h);
   p->w = w;
   p->h = h;
   p->dw = w / ax;
   p->dh = h / ay;
   p->pmap = ECreatePixmap(p->win, p->w, p->h, VRoot.depth);
   if (p->visible)
      PagerRedraw(p, 1);
   ESetWindowBackgroundPixmap(p->win, p->pmap);
   EClearWindow(p->win);
   if (p->ewin)
     {
	double              aspect;

	aspect = ((double)VRoot.w) / ((double)VRoot.h);
	p->ewin->client.w_inc = ax * 4;
	p->ewin->client.h_inc = ay * 8;
	p->ewin->client.aspect_min = aspect * ((double)ax / (double)ay);
	p->ewin->client.aspect_max = aspect * ((double)ax / (double)ay);
     }
   pq = Mode.queue_up;
   Mode.queue_up = 0;
   ic = ImageclassFind("PAGER_SEL", 0);
   if (ic)
     {
	DeskGetArea(p->desktop, &cx, &cy);
	EMoveResizeWindow(p->sel_win, cx * p->dw, cy * p->dh, p->dw, p->dh);
	ImageclassApply(ic, p->sel_win, p->dw, p->dh, 0, 0, STATE_NORMAL, 0,
			ST_PAGER);
     }
   Mode.queue_up = pq;

   lst = EwinListGetForDesk(&num, p->desktop);
   for (i = 0; i < num; i++)
      PagerEwinUpdateMini(p, lst[i]);
}

static void
PagerEwinRefresh(EWin * ewin)
{
   /* This doesn't do anything anymore apparently
    * --Mandrake
    * This is new code.
    * It can be removed but I have kept it around, just in case /Kim */
   return;
   ewin = NULL;
}

static void
PagerEwinClose(EWin * ewin)
{
   PagerDestroy(ewin->data);
   ewin->data = NULL;
}

static void
PagerEwinInit(EWin * ewin, void *ptr)
{
   ewin->data = ptr;

   ewin->MoveResize = PagerEwinMoveResize;
   ewin->Refresh = PagerEwinRefresh;
   ewin->Close = PagerEwinClose;

   ewin->skiptask = 1;
   ewin->skip_ext_pager = 1;
   ewin->skipfocus = 1;
   ewin->skipwinlist = 1;
   ewin->neverfocus = 1;
   ewin->props.autosave = 1;

   EoSetSticky(ewin, 1);
}

static void
PagerShow(Pager * p)
{
   EWin               *ewin = NULL;
   char                s[4096];
   char                pq;

   if (!Conf.pagers.enable)
      return;

   if (p->ewin)
     {
	ShowEwin(p->ewin);
	return;
     }

   Esnprintf(s, sizeof(s), "%i", p->desktop);
   HintsSetWindowClass(p->win, s, "Enlightenment_Pager");

   pq = Mode.queue_up;
   Mode.queue_up = 0;

   ewin = AddInternalToFamily(p->win, NULL, EWIN_TYPE_PAGER, p, PagerEwinInit);
   if (ewin)
     {
	int                 ax, ay, w, h;
	double              aspect;

	ewin->client.event_mask |=
	   ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
	ESelectInput(p->win, ewin->client.event_mask);

	aspect = ((double)VRoot.w) / ((double)VRoot.h);
	GetAreaSize(&ax, &ay);
	ewin->client.aspect_min = aspect * ((double)ax / (double)ay);
	ewin->client.aspect_max = aspect * ((double)ax / (double)ay);
	ewin->client.w_inc = ax * 4;
	ewin->client.h_inc = ay * 8;
	ewin->client.width.min = 10 * ax;
	ewin->client.height.min = 8 * ay;
	ewin->client.width.max = 320 * ax;
	ewin->client.height.max = 240 * ay;

	p->ewin = ewin;
	p->visible = 1;

	/* get the size right damnit! */
	w = ewin->client.w;
	h = ewin->client.h;
	ewin->client.w = 1;
	ewin->client.h = 1;
	if (ewin->client.already_placed)
	  {
	     MoveResizeEwin(ewin, EoGetX(ewin), EoGetY(ewin), w, h);
	  }
	else
	  {
	     /* no snapshots ? first time ? make a row on the bottom left up */
	     MoveResizeEwin(ewin, 0, VRoot.h - (Conf.desks.num -
						p->desktop) * EoGetH(ewin), w,
			    h);
	  }
	PagerRedraw(p, 1);

	/* show the pager ewin */
	ShowEwin(ewin);
	if (Conf.pagers.snap)
	  {
	     Esnprintf(s, sizeof(s), "__.%x", (unsigned)p->win);
	     if (Conf.pagers.scanspeed > 0)
		DoIn(s, 1 / ((double)Conf.pagers.scanspeed), PagerUpdateTimeout,
		     0, p);
	  }
	AddItem(p, "PAGER", p->win, LIST_TYPE_PAGER);
     }

   Mode.queue_up = pq;
}

static Pager      **
PagersForDesktop(int d, int *num)
{
   Pager             **pp = NULL;
   Pager             **pl = NULL;
   int                 i, pnum;

   if (!Conf.pagers.enable)
      return NULL;

   *num = 0;
   pl = (Pager **) ListItemType(&pnum, LIST_TYPE_PAGER);
   if (pl)
     {
	for (i = 0; i < pnum; i++)
	  {
	     if (pl[i]->desktop == d)
	       {
		  (*num)++;
		  pp = Erealloc(pp, sizeof(Pager *) * (*num));
		  pp[(*num) - 1] = pl[i];
	       }
	  }
	Efree(pl);
     }
   return pp;
}

static void
RedrawPagersForDesktop(int d, char newbg)
{
   Pager             **pl;
   int                 i, num;

   if (!Conf.pagers.enable)
      return;

   pl = PagersForDesktop(d, &num);
   for (i = 0; i < num; i++)
      PagerRedraw(pl[i], newbg);
   if (pl)
      Efree(pl);
}

static void
ForceUpdatePagersForDesktop(int d)
{
   Pager             **pl;
   int                 i, num;

   if (!Conf.pagers.enable)
      return;

   pl = PagersForDesktop(d, &num);
   for (i = 0; i < num; i++)
      PagerForceUpdate(pl[i]);
   if (pl)
      Efree(pl);
}

static void
PagerEwinUpdateFromPager(Pager * p, EWin * ewin)
{
   int                 x, y, w, h, ax, ay, cx, cy;
   static GC           gc = 0;

   if (!Conf.pagers.snap)
     {
	PagerEwinUpdateMini(p, ewin);
	return;
     }
   if (!Conf.pagers.enable)
      return;

   GetAreaSize(&ax, &ay);
   DeskGetArea(p->desktop, &cx, &cy);
   x = ((EoGetX(ewin) + (cx * VRoot.w)) * (p->w / ax)) / VRoot.w;
   y = ((EoGetY(ewin) + (cy * VRoot.h)) * (p->h / ay)) / VRoot.h;
   w = ((EoGetW(ewin)) * (p->w / ax)) / VRoot.w;
   h = ((EoGetH(ewin)) * (p->h / ay)) / VRoot.h;
   if (!gc)
      gc = ECreateGC(p->pmap, 0, NULL);

   /* NB! If the pixmap/mask was created by imlib, free it. Due to imlibs */
   /*     image/pixmap cache it may be in use elsewhere. */
   if (ewin->mini_pmm.pmap &&
       ((ewin->mini_pmm.type) || (ewin->mini_w != w) || (ewin->mini_h != h)))
      FreePmapMask(&ewin->mini_pmm);

   if (!ewin->mini_pmm.pmap)
     {
	ewin->mini_w = w;
	ewin->mini_h = h;
	ewin->mini_pmm.type = 0;
	ewin->mini_pmm.pmap = ECreatePixmap(p->win, w, h, VRoot.depth);
	ewin->mini_pmm.mask = None;
     }
   XCopyArea(disp, p->pmap, ewin->mini_pmm.pmap, gc, x, y, w, h, 0, 0);

   if (ewin == p->hi_ewin)
      PagerUpdateHiWin(p, ewin);
}

static void
PagerReArea(Pager * p)
{
   int                 w, h, ax, ay;
   double              aspect;

   GetAreaSize(&ax, &ay);

   w = p->dw * ax;
   h = p->dh * ay;
   if (p->ewin)
     {

	aspect = ((double)VRoot.w) / ((double)VRoot.h);
	p->ewin->client.w_inc = ax * 4;
	p->ewin->client.h_inc = ay * 8;
	p->ewin->client.aspect_min = aspect * ((double)ax / (double)ay);
	p->ewin->client.aspect_max = aspect * ((double)ax / (double)ay);
	MoveResizeEwin(p->ewin, EoGetX(p->ewin), EoGetY(p->ewin), w, h);
     }
}

static void
PagerEwinOutsideAreaUpdate(EWin * ewin)
{
   if (!Conf.pagers.enable)
      return;

   if (EoIsSticky(ewin))
     {
	int                 i;

	for (i = 0; i < Conf.desks.num; i++)
	   RedrawPagersForDesktop(i, 0);
	ForceUpdatePagersForDesktop(EoGetDesk(ewin));
     }
   else if (EoGetDesk(ewin) != DesksGetCurrent())
     {
	RedrawPagersForDesktop(EoGetDesk(ewin), 0);
	ForceUpdatePagersForDesktop(EoGetDesk(ewin));
     }
   else
     {
	if ((EoGetX(ewin) < 0) || (EoGetY(ewin) < 0)
	    || ((EoGetX(ewin) + EoGetW(ewin)) > VRoot.w)
	    || ((EoGetY(ewin) + EoGetH(ewin)) > VRoot.h))
	   RedrawPagersForDesktop(EoGetDesk(ewin), 3);
     }

   ForceUpdatePagersForDesktop(EoGetDesk(ewin));
}

static EWin        *
EwinInPagerAt(Pager * p, int x, int y)
{
   int                 wx, wy, ww, wh, ax, ay, cx, cy;
   EWin               *const *lst;
   int                 i, num;

   if (!Conf.pagers.enable)
      return NULL;

   GetAreaSize(&ax, &ay);
   DeskGetArea(p->desktop, &cx, &cy);

   lst = EwinListGetForDesk(&num, p->desktop);
   for (i = 0; i < num; i++)
     {
	EWin               *ewin;

	ewin = lst[i];
	if (!ewin->iconified && ewin->shown)
	  {
	     wx = ((EoGetX(ewin) + (cx * VRoot.w)) * (p->w / ax)) / VRoot.w;
	     wy = ((EoGetY(ewin) + (cy * VRoot.h)) * (p->h / ay)) / VRoot.h;
	     ww = ((EoGetW(ewin)) * (p->w / ax)) / VRoot.w;
	     wh = ((EoGetH(ewin)) * (p->h / ay)) / VRoot.h;
	     if ((x >= wx) && (y >= wy) && (x < (wx + ww)) && (y < (wy + wh)))
		return ewin;
	  }
     }

   return NULL;
}

static void
PagerAreaAt(Pager * p, int x, int y, int *ax, int *ay)
{
   int                 asx, asy;

   if (!Conf.pagers.enable)
      return;

   GetAreaSize(&asx, &asy);
   *ax = x / (p->w / asx);
   *ay = y / (p->h / asy);
}

static void
PagerMenuShow(Pager * p, int x, int y)
{
   static Menu        *p_menu = NULL, *pw_menu = NULL;
   MenuItem           *mi;
   EWin               *ewin;
   char                s[1024];
   int                 ax, ay;

   if (!Conf.pagers.enable)
      return;

   ewin = EwinInPagerAt(p, x, y);
   if (ewin)
     {
	if (pw_menu)
	   MenuDestroy(pw_menu);

	pw_menu =
	   MenuCreate("__DESK_WIN_MENU", _("Window Options"), NULL, NULL);

	Esnprintf(s, sizeof(s), "wop %#lx ic", ewin->client.win);
	mi = MenuItemCreate(_("Iconify"), NULL, s, NULL);
	MenuAddItem(pw_menu, mi);

	Esnprintf(s, sizeof(s), "wop %#lx close", ewin->client.win);
	mi = MenuItemCreate(_("Close"), NULL, s, NULL);
	MenuAddItem(pw_menu, mi);

	Esnprintf(s, sizeof(s), "wop %#lx kill", ewin->client.win);
	mi = MenuItemCreate(_("Annihilate"), NULL, s, NULL);
	MenuAddItem(pw_menu, mi);

	Esnprintf(s, sizeof(s), "wop %#lx st", ewin->client.win);
	mi = MenuItemCreate(_("Stick / Unstick"), NULL, s, NULL);
	MenuAddItem(pw_menu, mi);

	EFunc("menus show __DESK_WIN_MENU");
	return;
     }

   PagerAreaAt(p, x, y, &ax, &ay);
   if (p_menu)
      MenuDestroy(p_menu);
   p_menu = MenuCreate("__DESK_MENU", _("Desktop Options"), NULL, NULL);

   mi = MenuItemCreate(_("Pager Settings..."), NULL, "pg cfg", NULL);
   MenuAddItem(p_menu, mi);

   mi = MenuItemCreate(_("Snapshotting On"), NULL, "pg snap on", NULL);
   MenuAddItem(p_menu, mi);

   mi = MenuItemCreate(_("Snapshotting Off"), NULL, "pg snap off", NULL);
   MenuAddItem(p_menu, mi);

   if (Conf.pagers.snap)
     {
	mi = MenuItemCreate(_("High Quality On"), NULL, "pg hiq on", NULL);
	MenuAddItem(p_menu, mi);

	mi = MenuItemCreate(_("High Quality Off"), NULL, "pg hiq off", NULL);
	MenuAddItem(p_menu, mi);
     }

   EFunc("menus show __DESK_MENU");
}

static void
PagerClose(Pager * p)
{
   EUnmapWindow(p->win);
}

static void
UpdatePagerSel(void)
{
   Pager             **pl;
   Pager              *p;
   int                 i, pnum, cx, cy;
   ImageClass         *ic;

   if (!Conf.pagers.enable)
      return;

   pl = (Pager **) ListItemType(&pnum, LIST_TYPE_PAGER);
   if (!pl)
      return;

   for (i = 0; i < pnum; i++)
     {
	p = pl[i];
	if (p->desktop != DesksGetCurrent())
	   EUnmapWindow(p->sel_win);
	else
	  {
	     DeskGetArea(p->desktop, &cx, &cy);
	     EMoveWindow(p->sel_win, cx * p->dw, cy * p->dh);
	     EMapWindow(p->sel_win);
	     ic = ImageclassFind("PAGER_SEL", 0);
	     if (ic)
		ImageclassApply(ic, p->sel_win, p->dw, p->dh, 0, 0,
				STATE_NORMAL, 0, ST_PAGER);
	  }
     }
   Efree(pl);
}

static void
PagerShowTt(EWin * ewin)
{
   static EWin        *tt_ewin = NULL;
   ToolTip            *tt;

   if (!Conf.pagers.title || (ewin == tt_ewin))
      return;

   if (MenusActive())		/* Don't show Tooltip when menu is up */
      return;

   tt = FindItem("PAGER", 0, LIST_FINDBY_NAME, LIST_TYPE_TOOLTIP);
   if (tt)
     {
	if (ewin)
	   TooltipShow(tt, EwinGetIconName(ewin), NULL, Mode.x, Mode.y);
	else
	   TooltipHide(tt);
     }

   tt_ewin = ewin;
}

static void
PagerHideHi(Pager * p)
{
   if (p->hi_visible)
     {
	p->hi_visible = 0;
	EUnmapWindow(p->hi_win);
     }
   p->hi_ewin = NULL;

   PagerShowTt(NULL);
}

#if 0
static void
PagerHideAllHi(void)
{
   Pager             **pl = NULL;
   int                 i, pnum;

   if (!Conf.pagers.enable)
      return;

   pl = (Pager **) ListItemType(&pnum, LIST_TYPE_PAGER);
   if (pl)
     {
	for (i = 0; i < pnum; i++)
	   PagerHideHi(pl[i]);
	Efree(pl);
     }
}
#endif

static void
PagerShowHi(Pager * p, EWin * ewin, int x, int y, int w, int h)
{
   char                pq;
   ImageClass         *ic = NULL;

   if (MenusActive())		/* Don't show HiWin when menu is up */
      return;

   pq = Mode.queue_up;

   p->hi_win_w = 2 * w;
   p->hi_win_h = 2 * h;

   ic = ImageclassFind("PAGER_WIN", 0);
   EMoveResizeWindow(p->hi_win, x, y, w, h);
   EMapRaised(p->hi_win);
   if (ewin->mini_pmm.pmap)
     {
	Imlib_Image        *im;
	Pixmap              pmap, mask;
	int                 xx, yy, ww, hh, i;

	imlib_context_set_drawable(ewin->mini_pmm.pmap);
	im = imlib_create_image_from_drawable(0, 0, 0, ewin->mini_w,
					      ewin->mini_h, 0);
	imlib_context_set_image(im);
	if (w > h)
	  {
	     for (i = w; i < (w * 2); i++)
	       {
		  ww = i;
		  hh = (i * h) / w;
		  xx = x + ((w - ww) / 2);
		  yy = y + ((h - hh) / 2);
		  imlib_render_pixmaps_for_whole_image_at_size(&pmap,
							       &mask, ww, hh);
		  ESetWindowBackgroundPixmap(p->hi_win, pmap);
		  imlib_free_pixmap_and_mask(pmap);
		  EMoveResizeWindow(p->hi_win, xx, yy, ww, hh);
		  EClearWindow(p->hi_win);
		  {
		     int                 px, py;

		     PointerAt(&px, &py);
		     if ((px < x) || (py < y) || (px >= (x + w))
			 || (py >= (y + h)))
		       {
			  imlib_free_image_and_decache();
			  EUnmapWindow(p->hi_win);
			  goto done;
		       }
		  }
	       }
	  }
	else
	  {
	     for (i = h; i < (h * 2); i++)
	       {
		  ww = (i * w) / h;
		  hh = i;
		  xx = x + ((w - ww) / 2);
		  yy = y + ((h - hh) / 2);
		  imlib_render_pixmaps_for_whole_image_at_size(&pmap,
							       &mask, ww, hh);
		  ESetWindowBackgroundPixmap(p->hi_win, pmap);
		  imlib_free_pixmap_and_mask(pmap);
		  EMoveResizeWindow(p->hi_win, xx, yy, ww, hh);
		  EClearWindow(p->hi_win);
		  {
		     int                 px, py;

		     PointerAt(&px, &py);
		     if ((px < x) || (py < y) || (px >= (x + w))
			 || (py >= (y + h)))
		       {
			  imlib_free_image_and_decache();
			  EUnmapWindow(p->hi_win);
			  goto done;
		       }
		  }
	       }
	  }
	EMoveResizeWindow(p->hi_win, x - (w / 2), y - (h / 2), w * 2, h * 2);
	imlib_context_set_image(im);
	imlib_context_set_drawable(p->hi_win);
	imlib_render_image_on_drawable_at_size(0, 0, p->hi_win_w, p->hi_win_h);
	imlib_free_image_and_decache();
     }
   else if (ic)
     {
	int                 xx, yy, ww, hh, i;

	Mode.queue_up = 0;
	if (w > h)
	  {
	     for (i = w; i < (w * 2); i++)
	       {
		  ww = i;
		  hh = (i * h) / w;
		  xx = x + ((w - ww) / 2);
		  yy = y + ((h - hh) / 2);
		  ImageclassApply(ic, p->hi_win, ww, hh, 0, 0, STATE_NORMAL, 0,
				  ST_PAGER);
		  EMoveResizeWindow(p->hi_win, xx, yy, ww, hh);
		  EClearWindow(p->hi_win);
		  {
		     int                 px, py;

		     PointerAt(&px, &py);
		     if ((px < x) || (py < y) || (px >= (x + w))
			 || (py >= (y + h)))
		       {
			  EUnmapWindow(p->hi_win);
			  goto done;
		       }
		  }
	       }
	  }
	else
	  {
	     for (i = h; i < (h * 2); i++)
	       {
		  ww = (i * w) / h;
		  hh = i;
		  xx = x + ((w - ww) / 2);
		  yy = y + ((h - hh) / 2);
		  ImageclassApply(ic, p->hi_win, ww, hh, 0, 0, STATE_NORMAL, 0,
				  ST_PAGER);
		  EMoveResizeWindow(p->hi_win, xx, yy, ww, hh);
		  EClearWindow(p->hi_win);
		  {
		     int                 px, py;

		     PointerAt(&px, &py);
		     if ((px < x) || (py < y) || (px >= (x + w))
			 || (py >= (y + h)))
		       {
			  EUnmapWindow(p->hi_win);
			  goto done;
		       }
		  }
	       }
	  }
	EMoveResizeWindow(p->hi_win, x - (w / 2), y - (h / 2), w * 2, h * 2);
     }
   else
     {
	Pixmap              pmap;
	GC                  gc;
	int                 xx, yy, ww, hh, i;

	pmap = ECreatePixmap(p->hi_win, w * 2, h * 2, VRoot.depth);
	ESetWindowBackgroundPixmap(p->hi_win, pmap);
	gc = ECreateGC(pmap, 0, NULL);

	if (w > h)
	  {
	     for (i = w; i < (w * 2); i++)
	       {
		  ww = i;
		  hh = (i * h) / w;
		  xx = x + ((w - ww) / 2);
		  yy = y + ((h - hh) / 2);
		  XSetForeground(disp, gc, BlackPixel(disp, VRoot.scr));
		  XFillRectangle(disp, pmap, gc, 0, 0, ww, hh);
		  XSetForeground(disp, gc, WhitePixel(disp, VRoot.scr));
		  XFillRectangle(disp, pmap, gc, 1, 1, ww - 2, hh - 2);
		  EMoveResizeWindow(p->hi_win, xx, yy, ww, hh);
		  EClearWindow(p->hi_win);
		  {
		     int                 px, py;

		     PointerAt(&px, &py);
		     if ((px < x) || (py < y) || (px >= (x + w))
			 || (py >= (y + h)))
		       {
			  EFreePixmap(pmap);
			  EUnmapWindow(p->hi_win);
			  goto done1;
		       }
		  }
	       }
	  }
	else
	  {
	     for (i = h; i < (h * 2); i++)
	       {
		  ww = (i * w) / h;
		  hh = i;
		  xx = x + ((w - ww) / 2);
		  yy = y + ((h - hh) / 2);
		  XSetForeground(disp, gc, BlackPixel(disp, VRoot.scr));
		  XFillRectangle(disp, pmap, gc, 0, 0, ww, hh);
		  XSetForeground(disp, gc, WhitePixel(disp, VRoot.scr));
		  XFillRectangle(disp, pmap, gc, 1, 1, ww - 2, hh - 2);
		  EMoveResizeWindow(p->hi_win, xx, yy, ww, hh);
		  EClearWindow(p->hi_win);
		  {
		     int                 px, py;

		     PointerAt(&px, &py);
		     if ((px < x) || (py < y) || (px >= (x + w))
			 || (py >= (y + h)))
		       {
			  EFreePixmap(pmap);
			  EUnmapWindow(p->hi_win);
			  goto done1;
		       }
		  }
	       }
	  }
	EFreePixmap(pmap);
	EMoveResizeWindow(p->hi_win, x - (w / 2), y - (h / 2), w * 2, h * 2);
      done1:
	EFreeGC(gc);
     }
   p->hi_visible = 1;
   p->hi_ewin = ewin;

 done:
   Mode.queue_up = pq;
}

static void
PagerHandleMotion(Pager * p, Window win, int x, int y, int in)
{
   int                 hx, hy;
   Window              rw, cw;
   EWin               *ewin = NULL;

   if (!Conf.pagers.enable)
      return;

   XQueryPointer(disp, p->win, &rw, &cw, &hx, &hy, &x, &y, &hx);

   if (x >= 0 && x < p->w && y >= 0 && y < p->h)
      ewin = EwinInPagerAt(p, x, y);
   else
      ewin = NULL;

   if (!Conf.pagers.zoom)
     {
	if (in == PAGER_EVENT_MOUSE_OUT)
	   PagerShowTt(NULL);
	else
	   PagerShowTt(ewin);
	return;
     }

   if (ewin == NULL)
     {
	PagerHideHi(p);
	return;
     }

   if (in == PAGER_EVENT_MOUSE_OUT)
     {
	PagerShowTt(NULL);
     }
   else if ((in == PAGER_EVENT_MOTION) && EoGetLayer(ewin) <= 0)
     {
	if (p->hi_ewin)
	   PagerHideHi(p);
	PagerShowTt(ewin);
     }
   else if ((in == PAGER_EVENT_MOTION) && ewin != p->hi_ewin)
     {
	int                 wx, wy, ww, wh, ax, ay, cx, cy, px, py;

	PagerHideHi(p);
	GetAreaSize(&ax, &ay);
	DeskGetArea(p->desktop, &cx, &cy);

	wx = ((EoGetX(ewin) + (cx * VRoot.w)) * (p->w / ax)) / VRoot.w;
	wy = ((EoGetY(ewin) + (cy * VRoot.h)) * (p->h / ay)) / VRoot.h;
	ww = ((EoGetW(ewin)) * (p->w / ax)) / VRoot.w;
	wh = ((EoGetH(ewin)) * (p->h / ay)) / VRoot.h;
	XTranslateCoordinates(disp, p->win, VRoot.win, 0, 0, &px, &py, &cw);
	PagerShowHi(p, ewin, px + wx, py + wy, ww, wh);
	PagerShowTt(ewin);
     }
   else if (in == PAGER_EVENT_MOTION)
     {
	PagerShowTt(ewin);
     }

   return;
   win = 0;

}

static void
NewPagerForDesktop(int desk)
{

   Pager              *p;
   char                s[1024];

   p = PagerCreate();
   if (p)
     {
	p->desktop = desk;
	Esnprintf(s, sizeof(s), "%i", desk);
	HintsSetWindowName(p->win, s);
	PagerShow(p);
     }
}

static void
PagerSetHiQ(char onoff)
{
   Pager             **pl;
   EWin               *const *lst;
   int                 i, num;

   Conf.pagers.hiq = onoff;

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	lst[i]->mini_w = 0;
	lst[i]->mini_h = 0;
     }

   pl = (Pager **) ListItemType(&num, LIST_TYPE_PAGER);
   for (i = 0; i < num; i++)
     {
	PagerHideHi(pl[i]);
	PagerRedraw(pl[i], 2);
	PagerForceUpdate(pl[i]);
     }
   if (pl)
      Efree(pl);
}

static void
PagerSetSnap(char onoff)
{
   Pager             **pl;
   EWin               *const *lst;
   int                 i, num;
   char                s[256];

   Conf.pagers.snap = onoff;

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	lst[i]->mini_w = 0;
	lst[i]->mini_h = 0;
     }

   pl = (Pager **) ListItemType(&num, LIST_TYPE_PAGER);
   for (i = 0; i < num; i++)
     {
	PagerHideHi(pl[i]);
	PagerRedraw(pl[i], 2);
	PagerForceUpdate(pl[i]);
	if (Conf.pagers.snap)
	  {
	     Esnprintf(s, sizeof(s), "__.%x", (unsigned)pl[i]->win);
	     if (Conf.pagers.scanspeed > 0)
		DoIn(s, 1 / ((double)Conf.pagers.scanspeed),
		     PagerUpdateTimeout, 0, pl[i]);
	  }
     }
   if (pl)
      Efree(pl);
}

/*
 * Pager event handlers
 */

static int         *gwin_px, *gwin_py;

static void
PagerEventUnmap(Pager * p)
{
   PagerHideHi(p);
   if (p == mode_context_pager)
     {
	mode_context_pager = NULL;
	Mode.mode = MODE_NONE;
     }
}

static void
EwinGroupMove(EWin * ewin, int desk, int x, int y)
{
   int                 i, num, dx, dy, newdesk;
   EWin              **gwins;

   if (!ewin)
      return;

   /* Move all group members */
   newdesk = desk != EoGetDesk(ewin);
   dx = x - EoGetX(ewin);
   dy = y - EoGetY(ewin);
   gwins =
      ListWinGroupMembersForEwin(ewin, GROUP_ACTION_MOVE, Mode.nogroup, &num);
   for (i = 0; i < num; i++)
     {
	if (gwins[i]->type == EWIN_TYPE_PAGER || gwins[i]->fixedpos)
	   continue;

	if (newdesk)
	   MoveEwinToDesktopAt(gwins[i], desk, EoGetX(gwins[i]) + dx,
			       EoGetY(gwins[i]) + dy);
	else
	   MoveEwin(gwins[i], EoGetX(gwins[i]) + dx, EoGetY(gwins[i]) + dy);
     }
   if (gwins)
      Efree(gwins);
}

static void
PagerEwinMove(Pager * p, Pager * pd)
{
   int                 x, y, dx, dy, px, py;
   int                 ax, ay, cx, cy;
   Window              child;

   GetAreaSize(&ax, &ay);
   DeskGetArea(pd->desktop, &cx, &cy);

   /* Delta in pager coords */
   dx = Mode.x - Mode.px;
   dy = Mode.y - Mode.py;

   if (dx || dy)
     {
	/* Move mini window */
	GetWinXY(p->hi_win, &x, &y);
	x += dx;
	y += dy;
	ERaiseWindow(p->hi_win);
	EMoveWindow(p->hi_win, x, y);
     }

   /* Find real window position */
   XTranslateCoordinates(disp, p->hi_win, pd->win, 0, 0, &px, &py, &child);
   x = (px * ax * VRoot.w) / pd->w - cx * VRoot.w;
   y = (py * ay * VRoot.h) / pd->h - cy * VRoot.h;

   /* Move all group members */
   EwinGroupMove(p->hi_ewin, pd->desktop, x, y);
}

static int
PagerEventMotion(Pager * p, XEvent * ev)
{
   int                 used = 0;

   switch (Mode.mode)
     {
     case MODE_NONE:
	if (p == NULL)
	   break;
	used = 1;
	PagerHandleMotion(p, ev->xmotion.window, ev->xmotion.x,
			  ev->xmotion.y, PAGER_EVENT_MOTION);
	break;

     case MODE_PAGER_DRAG_PENDING:
     case MODE_PAGER_DRAG:
	Mode.mode = MODE_PAGER_DRAG;
	if (p == NULL)
	   break;
	used = 1;

	if (!FindItem((char *)p->hi_ewin, 0, LIST_FINDBY_POINTER,
		      LIST_TYPE_EWIN))
	   p->hi_ewin = NULL;
	if ((!p->hi_ewin) ||
	    (p->hi_ewin->type == EWIN_TYPE_PAGER) || (p->hi_ewin->fixedpos))
	   break;

	PagerEwinMove(p, p);
	break;
     }

   return used;
}

static void
PagerEventMouseDown(Pager * p, XEvent * ev)
{
   Window              win = ev->xbutton.window, child;
   int                 i, num, px, py, in_pager;
   EWin               *ewin, **gwins;

   gwins =
      ListWinGroupMembersForEwin(p->hi_ewin, GROUP_ACTION_MOVE, Mode.nogroup,
				 &num);
   gwin_px = calloc(num, sizeof(int));
   gwin_py = calloc(num, sizeof(int));

   for (i = 0; i < num; i++)
     {
	gwin_px[i] = EoGetX(gwins[i]);
	gwin_py[i] = EoGetY(gwins[i]);
     }

   if (gwins)
      Efree(gwins);

   px = ev->xbutton.x;
   py = ev->xbutton.y;
   /* If hi-win, translate x,y to pager window coordinates */
   if (win == p->hi_win)
      XTranslateCoordinates(disp, win, p->win, px, py, &px, &py, &child);
   in_pager = (px >= 0 && py >= 0 && px < p->w && py < p->h);

   if ((int)ev->xbutton.button == Conf.pagers.menu_button)
     {
	if (in_pager)
	  {
	     PagerHideHi(p);
	     PagerMenuShow(p, px, py);
	  }
     }
   else if ((int)ev->xbutton.button == Conf.pagers.win_button)
     {
	ewin = EwinInPagerAt(p, px, py);
	if ((ewin) && (ewin->type != EWIN_TYPE_PAGER))
	  {
	     Window              dw;
	     int                 wx, wy, ww, wh, ax, ay, cx, cy;

	     PagerHideHi(p);
	     GetAreaSize(&ax, &ay);
	     DeskGetArea(p->desktop, &cx, &cy);

	     wx = ((EoGetX(ewin) + (cx * VRoot.w)) * (p->w / ax)) / VRoot.w;
	     wy = ((EoGetY(ewin) + (cy * VRoot.h)) * (p->h / ay)) / VRoot.h;
	     ww = ((EoGetW(ewin)) * (p->w / ax)) / VRoot.w;
	     wh = ((EoGetH(ewin)) * (p->h / ay)) / VRoot.h;
	     XTranslateCoordinates(disp, p->win, VRoot.win, 0, 0, &px, &py,
				   &dw);
	     EMoveResizeWindow(p->hi_win, px + wx, py + wy, ww, wh);
	     ESetWindowBackgroundPixmap(p->hi_win, ewin->mini_pmm.pmap);
	     EMapRaised(p->hi_win);
	     GrabPointerSet(p->hi_win, ECSR_ACT_MOVE, 1);
	     p->hi_visible = 1;
	     p->hi_ewin = ewin;
	     p->hi_win_w = ww;
	     p->hi_win_h = wh;
	     Mode.mode = MODE_PAGER_DRAG_PENDING;
	     mode_context_pager = p;
	  }
     }
}

static int
PagerEventMouseUp(Pager * p, XEvent * ev)
{
   Window              win = ev->xbutton.window, child;
   int                 used = 0;
   int                 i, num, px, py, in_pager, in_vroot;
   EWin               *ewin, **gwins;
   int                 x, y, pax, pay;
   int                 mode_was;

   mode_was = Mode.mode;
   Mode.mode = MODE_NONE;

   px = ev->xbutton.x;
   py = ev->xbutton.y;
   /* If hi-win, translate x,y to pager window coordinates */
   if (win == p->hi_win)
      XTranslateCoordinates(disp, win, p->win, px, py, &px, &py, &child);
   in_pager = (px >= 0 && py >= 0 && px < p->w && py < p->h);

   in_vroot = (Mode.x >= 0 && Mode.x < VRoot.w &&
	       Mode.y >= 0 && Mode.y < VRoot.h);

   if (((int)ev->xbutton.button == Conf.pagers.sel_button))
     {
	if (win != Mode.last_bpress || !in_pager)
	   goto done;
	PagerAreaAt(p, px, py, &pax, &pay);
	DeskGoto(p->desktop);
	if (p->desktop != DesksGetCurrent())
	   SoundPlay("SOUND_DESKTOP_SHUT");
	SetCurrentArea(pax, pay);
     }
   else if (((int)ev->xbutton.button == Conf.pagers.win_button))
     {
	int                 prev_desk = -1;

	if (!FindItem((char *)p->hi_ewin, 0,
		      LIST_FINDBY_POINTER, LIST_TYPE_EWIN))
	   p->hi_ewin = NULL;

	switch (mode_was)
	  {
	  case MODE_PAGER_DRAG:
	     if (!p->hi_ewin)
		break;

	     /* Remember old desk for the dragged window */
	     prev_desk = EoGetDesk(p->hi_ewin);

	     /* Find which pager or iconbox we are in (if any) */
	     ewin = GetEwinPointerInClient();
	     if ((ewin) && (ewin->type == EWIN_TYPE_PAGER))
	       {
		  PagerEwinMove(p, ewin->data);
	       }
	     else if ((ewin) && (ewin->type == EWIN_TYPE_ICONBOX))
	       {
		  /* Pointer is in iconbox */

		  /* Don't iconify an iconbox by dragging */
		  if (p->hi_ewin->props.inhibit_iconify)
		     break;

		  /* Iconify after moving back to pre-drag position */
		  gwins =
		     ListWinGroupMembersForEwin(p->hi_ewin, GROUP_ACTION_MOVE,
						Mode.nogroup, &num);
		  for (i = 0; i < num; i++)
		    {
		       if (gwins[i]->type != EWIN_TYPE_PAGER)
			 {
			    MoveEwin(gwins[i], gwin_px[i], gwin_py[i]);
			    EwinIconify(gwins[i]);
			 }
		    }
		  if (gwins)
		     Efree(gwins);
	       }
	     else if (ewin && ewin->props.vroot)
	       {
		  /* Dropping onto virtual root */
		  EwinReparent(p->hi_ewin, ewin->client.win);
	       }
	     else if (!in_vroot)
	       {
		  /* Move back to real root */
		  EwinReparent(p->hi_ewin, RRoot.win);
	       }
	     else
	       {
		  /* Pointer is not in pager or iconbox */
		  /* Move window(s) to pointer location */
		  x = Mode.x - EoGetW(p->hi_ewin) / 2;
		  y = Mode.y - EoGetH(p->hi_ewin) / 2;
		  EwinGroupMove(p->hi_ewin, DesksGetCurrent(), x, y);
	       }
	     break;

	  default:
	     if (!in_pager)
		break;
	     PagerAreaAt(p, px, py, &pax, &pay);
	     DeskGoto(p->desktop);
	     SetCurrentArea(pax, pay);
	     ewin = EwinInPagerAt(p, px, py);
	     if (ewin)
	       {
		  RaiseEwin(ewin);
		  FocusToEWin(ewin, FOCUS_SET);
	       }
	     break;
	  }

	if (p->hi_ewin)
	  {
	     RedrawPagersForDesktop(EoGetDesk(p->hi_ewin), 3);
	     ForceUpdatePagersForDesktop(EoGetDesk(p->hi_ewin));
	     if (prev_desk >= 0 && prev_desk != EoGetDesk(p->hi_ewin))
	       {
		  RedrawPagersForDesktop(prev_desk, 3);
		  ForceUpdatePagersForDesktop(prev_desk);
	       }
	     PagerHideHi(p);
	  }

	mode_context_pager = NULL;
	GrabPointerRelease();
     }

 done:
   /* unallocate the space that was holding the old positions of the */
   /* windows */
   if (gwin_px)
     {
	Efree(gwin_px);
	gwin_px = NULL;
	Efree(gwin_py);
	gwin_py = NULL;
     }

   return used;
}

static void
PagerEventMainWin(XEvent * ev, void *prm)
{
   Pager              *p = (Pager *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	PagerEventMouseDown(p, ev);
	break;
     case ButtonRelease:
	PagerEventMouseUp(p, ev);
	break;
     case MotionNotify:
	PagerEventMotion(p, ev);
	break;
     case EnterNotify:
#if 0				/* Nothing done here */
	PagerHandleMotion(p, ev->xany.window, ev->xcrossing.x, ev->xcrossing.y,
			  PAGER_EVENT_MOUSE_IN);
#endif
	break;
     case LeaveNotify:
	PagerHandleMotion(p, ev->xany.window, ev->xcrossing.x, ev->xcrossing.y,
			  PAGER_EVENT_MOUSE_OUT);
	break;
     case UnmapNotify:
	PagerEventUnmap(p);
	break;
     }
}

static void
PagerEventHiWin(XEvent * ev, void *prm)
{
   Pager              *p = (Pager *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	PagerEventMouseDown(p, ev);
	break;
     case ButtonRelease:
	PagerEventMouseUp(p, ev);
	break;
     case MotionNotify:
	PagerEventMotion(p, ev);
	break;
     case EnterNotify:
#if 0				/* Nothing done here */
	PagerHandleMotion(p, ev->xany.window, ev->xcrossing.x, ev->xcrossing.y,
			  PAGER_EVENT_MOUSE_IN);
#endif
	break;
     case LeaveNotify:
	PagerHandleMotion(p, ev->xany.window, ev->xcrossing.x, ev->xcrossing.y,
			  PAGER_EVENT_MOUSE_OUT);
	break;
     }
}

/*
 * Pagers handling
 */

static void
PagersEnableForDesktop(int desk)
{
   Pager             **pl;
   int                 num;

   pl = PagersForDesktop(desk, &num);
   if (!pl)
      NewPagerForDesktop(desk);
   else
      Efree(pl);
}

static void
PagersDisableForDesktop(int desk)
{
   Pager             **pl;

   int                 i, num;

   pl = PagersForDesktop(desk, &num);
   if (!pl)
      return;

   for (i = 0; i < num; i++)
      PagerClose(pl[i]);
   Efree(pl);
}

static void
PagersShow(int enable)
{
   int                 i;

   if (enable && !Conf.pagers.enable)
     {
	Conf.pagers.enable = 1;
	for (i = 0; i < Conf.desks.num; i++)
	   PagersEnableForDesktop(i);
	UpdatePagerSel();
     }
   else if (!enable && Conf.pagers.enable)
     {
	Conf.pagers.enable = 0;
	for (i = 0; i < Conf.desks.num; i++)
	   PagersDisableForDesktop(i);
     }
}

static void
PagersReArea(void)
{
   Pager             **pl = NULL;
   int                 i, num;

   if (!Conf.pagers.enable)
      return;

   pl = (Pager **) ListItemType(&num, LIST_TYPE_PAGER);
   if (!pl)
      return;

   for (i = 0; i < num; i++)
      PagerReArea(pl[i]);
   Efree(pl);
}

#if 0
static int
PagersCountForDesktop(int desk)
{
   Pager             **pl;
   int                 num;

   pl = PagersForDesktop(desk, &num);
   if (pl)
      Efree(pl);
   return num;
}
#endif

/*
 * Configuration dialog
 */
static char         tmp_show_pagers;
static char         tmp_pager_hiq;
static char         tmp_pager_snap;
static char         tmp_pager_zoom;
static char         tmp_pager_title;
static char         tmp_pager_do_scan;
static int          tmp_pager_scan_speed;
static int          tmp_pager_sel_button;
static int          tmp_pager_win_button;
static int          tmp_pager_menu_button;
static DItem       *pager_scan_speed_label = NULL;
static Dialog      *pager_settings_dialog = NULL;

static void
CB_ConfigurePager(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	PagersShow(tmp_show_pagers);
	if (Conf.pagers.hiq != tmp_pager_hiq)
	   PagerSetHiQ(tmp_pager_hiq);
	Conf.pagers.zoom = tmp_pager_zoom;
	Conf.pagers.title = tmp_pager_title;
	Conf.pagers.sel_button = tmp_pager_sel_button;
	Conf.pagers.win_button = tmp_pager_win_button;
	Conf.pagers.menu_button = tmp_pager_menu_button;
	if ((Conf.pagers.scanspeed != tmp_pager_scan_speed)
	    || ((!tmp_pager_do_scan) && (Conf.pagers.scanspeed > 0))
	    || ((tmp_pager_do_scan) && (Conf.pagers.scanspeed == 0)))
	  {
	     if (tmp_pager_do_scan)
		Conf.pagers.scanspeed = tmp_pager_scan_speed;
	     else
		Conf.pagers.scanspeed = 0;
	     PagerSetSnap(tmp_pager_snap);
	  }
	if (Conf.pagers.snap != tmp_pager_snap)
	   PagerSetSnap(tmp_pager_snap);
     }
   autosave();
}

static void
CB_PagerScanSlide(Dialog * d __UNUSED__, int val __UNUSED__,
		  void *data __UNUSED__)
{
   char                s[256];

   Esnprintf(s, sizeof(s), "%s %03i %s", _("Pager scanning speed:"),
	     tmp_pager_scan_speed, _("lines per second"));
   DialogItemTextSetText(pager_scan_speed_label, s);
   DialogDrawItems(pager_settings_dialog, pager_scan_speed_label, 0, 0, 99999,
		   99999);
}

static void
SettingsPager(void)
{
   Dialog             *d;
   DItem              *table, *di, *radio;
   char                s[256];

   if ((d = FindItem("CONFIGURE_PAGER", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_PAGER");

   tmp_show_pagers = Conf.pagers.enable;
   tmp_pager_hiq = Conf.pagers.hiq;
   tmp_pager_snap = Conf.pagers.snap;
   tmp_pager_zoom = Conf.pagers.zoom;
   tmp_pager_title = Conf.pagers.title;
   tmp_pager_sel_button = Conf.pagers.sel_button;
   tmp_pager_win_button = Conf.pagers.win_button;
   tmp_pager_menu_button = Conf.pagers.menu_button;
   if (Conf.pagers.scanspeed == 0)
      tmp_pager_do_scan = 0;
   else
      tmp_pager_do_scan = 1;
   tmp_pager_scan_speed = Conf.pagers.scanspeed;

   d = pager_settings_dialog = DialogCreate("CONFIGURE_PAGER");
   DialogSetTitle(d, _("Pager Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 2, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	di = DialogAddItem(table, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/pager.png");

	di = DialogAddItem(table, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Desktop & Area\n"
				"Pager Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetColSpan(di, 2);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di, _("Enable pager display"));
   DialogItemCheckButtonSetState(di, tmp_show_pagers);
   DialogItemCheckButtonSetPtr(di, &tmp_show_pagers);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_("Make miniature snapshots of the screen"));
   DialogItemCheckButtonSetState(di, tmp_pager_snap);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_snap);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_
				("Smooth high quality snapshots in snapshot mode"));
   DialogItemCheckButtonSetState(di, tmp_pager_hiq);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_hiq);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_
				("Zoom in on pager windows when mouse is over them"));
   DialogItemCheckButtonSetState(di, tmp_pager_zoom);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_zoom);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_
				("Pop up window title when mouse is over the window"));
   DialogItemCheckButtonSetState(di, tmp_pager_title);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_title);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemCheckButtonSetText(di,
				_("Continuously scan screen to update pager"));
   DialogItemCheckButtonSetState(di, tmp_pager_do_scan);
   DialogItemCheckButtonSetPtr(di, &tmp_pager_do_scan);

   di = pager_scan_speed_label = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 512);
   Esnprintf(s, sizeof(s), "%s %03i %s", _("Pager scanning speed:"),
	     tmp_pager_scan_speed, _("lines per second"));
   DialogItemTextSetText(di, s);

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 1, 256);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 1);
   DialogItemSetColSpan(di, 2);
   DialogItemSliderSetVal(di, tmp_pager_scan_speed);
   DialogItemSliderSetValPtr(di, &tmp_pager_scan_speed);
   DialogItemSetCallback(di, CB_PagerScanSlide, 0, NULL);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 0);
   DialogItemTextSetText(di, _("Mouse button to select and drag windows:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Left"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Middle"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Right"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_pager_win_button);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 0);
   DialogItemTextSetText(di, _("Mouse button to select desktops:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Left"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Middle"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Right"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_pager_sel_button);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetColSpan(di, 2);
   DialogItemSetAlign(di, 0, 0);
   DialogItemTextSetText(di, _("Mouse button to display pager menu:"));

   radio = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Left"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Middle"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Right"));
   DialogItemRadioButtonSetFirst(di, radio);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio, &tmp_pager_menu_button);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetColSpan(di, 2);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigurePager, 1, DIALOG_BUTTON_OK);
   DialogAddButton(d, _("Apply"), CB_ConfigurePager, 0, DIALOG_BUTTON_APPLY);
   DialogAddButton(d, _("Close"), CB_ConfigurePager, 1, DIALOG_BUTTON_CLOSE);
   DialogSetExitFunction(d, CB_ConfigurePager, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigurePager, 0);
   ShowDialog(d);
}

/*
 * Pagers Module
 */

static void
PagersSighan(int sig, void *prm)
{
   static int          pdesk = -1;	/* Last desk */
   int                 desk;

   switch (sig)
     {
     case ESIGNAL_INIT:
	EDirMake(EDirUserCache(), "cached/pager");
	break;
     case ESIGNAL_CONFIGURE:
	break;
     case ESIGNAL_START:
	if (!Conf.pagers.enable)
	   break;
	Conf.pagers.enable = 0;
	Mode.queue_up = 0;
	PagersShow(1);
	Mode.queue_up = DRAW_QUEUE_ENABLE;
	break;
     case ESIGNAL_AREA_CONFIGURED:
	PagersReArea();
	break;
     case ESIGNAL_AREA_SWITCH_DONE:
	UpdatePagerSel();
	RedrawPagersForDesktop(DesksGetCurrent(), 3);
	ForceUpdatePagersForDesktop(DesksGetCurrent());
	break;
     case ESIGNAL_DESK_ADDED:
	NewPagerForDesktop((int)(prm));
	break;
     case ESIGNAL_DESK_REMOVED:
	PagersDisableForDesktop((int)(prm));
	break;
     case ESIGNAL_DESK_SWITCH_DONE:
#if 0
	/* Raise */
	RedrawPagersForDesktop(desk, 3);
	ForceUpdatePagersForDesktop(desk);
	UpdatePagerSel();
	/* Lower */
	RedrawPagersForDesktop(desks.order[0], 3);
	ForceUpdatePagersForDesktop(desks.order[0]);
	UpdatePagerSel();
#endif
	/* DeskGoto */
	desk = DesksGetCurrent();
	if (pdesk >= 0)
	   RedrawPagersForDesktop(pdesk, 0);
	RedrawPagersForDesktop(desk, 3);
	ForceUpdatePagersForDesktop(desk);
	UpdatePagerSel();
	pdesk = desk;
	break;
     case ESIGNAL_BACKGROUND_CHANGE:
	desk = (int)prm;
	if (desk == DesksGetCurrent())
	  {
	     RedrawPagersForDesktop(desk, 2);
	     ForceUpdatePagersForDesktop(desk);
	  }
	else
	   RedrawPagersForDesktop(desk, 1);
	break;
     case ESIGNAL_DESK_CHANGE:
	ForceUpdatePagersForDesktop((int)prm);
	break;
     case ESIGNAL_DESK_RESIZE:
	PagersReArea();
	break;
     case ESIGNAL_EWIN_UNMAP:
	PagerEwinOutsideAreaUpdate((EWin *) prm);
	break;
     case ESIGNAL_EWIN_CHANGE:
	if (Mode.mode != MODE_NONE)
	   break;
	PagerEwinOutsideAreaUpdate((EWin *) prm);
	break;
     }
}

static void
IPC_Pager(const char *params, Client * c __UNUSED__)
{
   const char         *p = params;
   char                prm1[128];
   int                 len, desk;

   if (!p)
      return;

   prm1[0] = '\0';
   len = 0;
   sscanf(p, "%100s %n", prm1, &len);
   p += len;

   if (!strncmp(prm1, "cfg", 3))
     {
	SettingsPager();
     }
   else if (!strcmp(prm1, "on"))
     {
	PagersShow(1);
     }
   else if (!strcmp(prm1, "off"))
     {
	PagersShow(0);
     }
   else if (!strcmp(prm1, "desk"))
     {
	desk = -1;
	prm1[0] = '\0';
	sscanf(p, "%d %100s", &desk, prm1);

	if (desk < 0)
	  {
	     ;
	  }
	else if (!strcmp(prm1, "on"))
	  {
	     PagersEnableForDesktop(desk);
	  }
	else if (!strcmp(prm1, "new"))
	  {
	     NewPagerForDesktop(desk);
	  }
	else if (!strcmp(prm1, "off"))
	  {
	     PagersDisableForDesktop(desk);
	  }
     }
   else if (!strcmp(prm1, "snap"))
     {
	if (!strcmp(p, "on"))
	  {
	     PagerSetSnap(1);
	  }
	else if (!strcmp(p, "off"))
	  {
	     PagerSetSnap(0);
	  }
     }
   else if (!strcmp(prm1, "hiq"))
     {
	if (!strcmp(p, "on"))
	  {
	     PagerSetHiQ(1);
	  }
	else if (!strcmp(p, "off"))
	  {
	     PagerSetHiQ(0);
	  }
     }
}

IpcItem             PagersIpcArray[] = {
   {
    IPC_Pager,
    "pager", "pg",
    "Toggle the status of the Pager and various pager settings",
    "use \"pager <on/off>\" to set the current mode\nuse \"pager ?\" "
    "to get the current mode\n"
    "  pager <#> <on/off/?>   Toggle or test any desktop's pager\n"
    "  pager cfg              Configure pagers\n"
    "  pager hiq <on/off>     Toggle high quality pager\n"
    "  pager scanrate <#>     Toggle number of line update " "per second"
    "  pager snap <on/off>    Toggle snapshotting in the pager\n"
    "  pager title <on/off>   Toggle title display in the pager\n"
    "  pager zoom <on/off>    Toggle zooming in the pager\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(PagersIpcArray)/sizeof(IpcItem))

/*
 * Configuration items
 */
static const CfgItem PagersCfgItems[] = {
   CFG_ITEM_BOOL(Conf.pagers, enable, 1),
   CFG_ITEM_BOOL(Conf.pagers, zoom, 1),
   CFG_ITEM_BOOL(Conf.pagers, title, 1),
   CFG_ITEM_BOOL(Conf.pagers, hiq, 1),
   CFG_ITEM_BOOL(Conf.pagers, snap, 1),
   CFG_ITEM_INT(Conf.pagers, scanspeed, 10),
   CFG_ITEM_INT(Conf.pagers, sel_button, 2),
   CFG_ITEM_INT(Conf.pagers, win_button, 1),
   CFG_ITEM_INT(Conf.pagers, menu_button, 3),

};
#define N_CFG_ITEMS (sizeof(PagersCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
EModule             ModPagers = {
   "pagers", "pg",
   PagersSighan,
   {N_IPC_FUNCS, PagersIpcArray},
   {N_CFG_ITEMS, PagersCfgItems}
};
