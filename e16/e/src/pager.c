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
   char               *border_name;
   Window              sel_win;
   char                hi_visible;
   Window              hi_win;
   EWin               *hi_ewin;
   int                 hi_win_w, hi_win_h;
};

#define PAGER_EVENT_MOUSE_OUT -1
#define PAGER_EVENT_MOTION     0
#define PAGER_EVENT_MOUSE_IN   1

static void         PagerUpdateTimeout(int val, void *data);
static void         PagerEwinUpdateMini(Pager * p, EWin * ewin);
static void         PagerEwinUpdateFromPager(Pager * p, EWin * ewin);

#define HIQ Conf.pagers.hiq

static void
PagerUpdateTimeout(int val, void *data)
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
   if (p->desktop != desks.current)
      return;
   if (Mode.mode != MODE_NONE)
      return;

   GetAreaSize(&ax, &ay);
   GetCurrentArea(&cx, &cy);
   ww = p->w / ax;
   hh = p->h / ay;
   xx = cx * ww;
   yy = cy * hh;
   phase = p->update_phase;
   y = ((phase & 0xfffffff8) + offsets[phase % 8]) % hh;
   y2 = (y * root.h) / hh;

   ScaleLine(p->pmap, root.win, xx, yy + y, root.w, ww, y2, (root.h / hh));
   XClearArea(disp, p->win, xx, yy + y, ww, 1, False);

   p->update_phase++;
   if (p->update_phase >= p->h)
     {
	int                 i, num;
	EWin               *const *lst;

	lst = EwinListGetForDesktop(p->desktop, &num);
	for (i = 0; i < num; i++)
	   PagerEwinUpdateFromPager(p, lst[i]);

	p->update_phase = 0;
     }
   return;
   val = 0;
}

Pager              *
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
     {
#if !USE_IMLIB2
	if (pImlib_Context->x.shm)
	  {
	     if (!pImlib_Context->x.shmp)
	       {
		  if (XShmPixmapFormat(disp) != ZPixmap)
		    {
		       SettingsPager();
		       DialogOK(_("Warning!"),
				_("\n"
				  "You seem to have an X Server capable of Shared Memory\n"
				  "but it is incapable of doing ZPixmap Shared pixmaps\n"
				  "(The server does not claim to be able to do them).\n"
				  "\n"
				  "The pager in Enlightenment will run slowly in snapshot\n"
				  "mode if you continue to use that mode of the pager\n"
				  "under these conditions.\n" "\n"
				  "It is suggested you change the settings on your pager to\n"
				  "disable snapshots to improve performance.\n"
				  "\n"));
		    }
		  else
		     DialogOK(_("Warning!"),
			      _("\n"
				"Your X Server is capable of doing Shared Memory but you do\n"
				"not have Shared Pixmaps enabled in your Imlib configuration.\n"
				"\n"
				"Please enable Shared Pixmaps in your Imlib configuration\n"
				"then restart Enlightenment to gain better performance for\n"
				"the pagers when snapshot mode is enabled.\n"
				"\n"));
	       }
	  }
	else
	  {
	     SettingsPager();
	     DialogOK(_("Warning!"),
		      _("\n"
			"You seem to be running Enlightenment over a network Connection\n"
			"or on an X Server that does not support Shared Memory, or you\n"
			"have disabled MIT-SHM Shared memory in your Imlib configuration.\n"
			"This means the Enlightenment Pager will perform slowly and use\n"
			"more system resources than it would when Shared Memory is\n"
			"available.\n" "\n"
			"To improve performance please either enable MIT-SHM Shared Memory\n"
			"in your Imlib config, if you disabled it, or disable Pager\n"
			"snapshots.\n" "\n"));
	  }
#endif
	did_dialog = 1;
     }
   GetAreaSize(&ax, &ay);
   p = Emalloc(sizeof(Pager));
   p->name = NULL;
   attr.colormap = root.cmap;
   attr.border_pixel = 0;
   attr.background_pixel = 0;
   attr.save_under = False;
   p->w = ((48 * root.w) / root.h) * ax;
   p->h = 48 * ay;
   p->dw = ((48 * root.w) / root.h);
   p->dh = 48;
   p->win = ECreateWindow(root.win, 0, 0, p->w, p->h, 0);
   p->pmap = ECreatePixmap(disp, p->win, p->w, p->h, root.depth);
   ESetWindowBackgroundPixmap(disp, p->win, p->pmap);
   p->hi_win = ECreateWindow(root.win, 0, 0, 3, 3, 0);
   p->hi_visible = 0;
   p->hi_ewin = NULL;
   XSelectInput(disp, p->hi_win,
		ButtonPressMask | ButtonReleaseMask | PointerMotionMask |
		EnterWindowMask | LeaveWindowMask);
   p->desktop = 0;
   p->visible = 0;
   p->update_phase = 0;
   p->ewin = NULL;
   p->border_name = NULL;
   p->sel_win = ECreateWindow(p->win, 0, 0, p->w / ax, p->h / ay, 0);
   pq = Mode.queue_up;
   Mode.queue_up = 0;
   ic = FindItem("PAGER_SEL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (ic)
      IclassApply(ic, p->sel_win, p->w / ax, p->h / ay, 0, 0, STATE_NORMAL, 0);
   Mode.queue_up = pq;
   return p;
}

static void
PagerEwinMoveResize(EWin * ewin, int resize)
{
   Pager              *p = ewin->pager;
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
   EFreePixmap(disp, p->pmap);
   FreePmapMask(&p->bgpmap);
   EResizeWindow(disp, p->win, w, h);
   p->w = w;
   p->h = h;
   p->dw = w / ax;
   p->dh = h / ay;
   p->pmap = ECreatePixmap(disp, p->win, p->w, p->h, root.depth);
   if (p->visible)
      PagerRedraw(p, 1);
   ESetWindowBackgroundPixmap(disp, p->win, p->pmap);
   XClearWindow(disp, p->win);
   if (p->ewin)
     {
	double              aspect;

	aspect = ((double)root.w) / ((double)root.h);
	p->ewin->client.w_inc = ax * 4;
	p->ewin->client.h_inc = ay * 8;
	p->ewin->client.aspect_min = aspect * ((double)ax / (double)ay);
	p->ewin->client.aspect_max = aspect * ((double)ax / (double)ay);
     }
   pq = Mode.queue_up;
   Mode.queue_up = 0;
   ic = FindItem("PAGER_SEL", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   if (ic)
     {
	cx = desks.desk[p->desktop].current_area_x;
	cy = desks.desk[p->desktop].current_area_y;
	EMoveResizeWindow(disp, p->sel_win, cx * p->dw, cy * p->dh, p->dw,
			  p->dh);
	IclassApply(ic, p->sel_win, p->dw, p->dh, 0, 0, STATE_NORMAL, 0);
     }
   Mode.queue_up = pq;

   lst = EwinListGetForDesktop(p->desktop, &num);
   for (i = 0; i < num; i++)
      PagerEwinUpdateMini(p, lst[i]);

   return;
   resize = 0;
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
   PagerDestroy(ewin->pager);
   ewin->pager = NULL;
}

static void
PagerEwinInit(EWin * ewin, void *ptr)
{
   ewin->pager = (Pager *) ptr;
   ewin->MoveResize = PagerEwinMoveResize;
   ewin->Refresh = PagerEwinRefresh;
   ewin->Close = PagerEwinClose;
}

void
PagerShow(Pager * p)
{
   EWin               *ewin = NULL;
   XClassHint         *xch;
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
   xch = XAllocClassHint();
   xch->res_name = s;
   xch->res_class = (char *)"Enlightenment_Pager";
   XSetClassHint(disp, p->win, xch);
   XFree(xch);
   pq = Mode.queue_up;
   Mode.queue_up = 0;
   MatchToSnapInfoPager(p);
   ewin = AddInternalToFamily(p->win, (p->border_name) ? p->border_name :
			      "PAGER", EWIN_TYPE_PAGER, p, PagerEwinInit);
   if (ewin)
     {
	int                 ax, ay, w, h;
	Snapshot           *sn;
	double              aspect;

#if 1				/* Do we need this? */
	ewin->client.event_mask |=
	   ButtonPressMask | ButtonReleaseMask | PointerMotionMask;
	XSelectInput(disp, p->win, ewin->client.event_mask);
#endif

	aspect = ((double)root.w) / ((double)root.h);
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
	sn = FindSnapshot(ewin);

	/* get the size right damnit! */
	w = ewin->client.w;
	h = ewin->client.h;
	ewin->client.w = 1;
	ewin->client.h = 1;
	if (sn)
	  {
	     ResizeEwin(ewin, w, h);
	  }
	else
	  {
	     /* no snapshots ? first time ? make a row on the bottom left up */
	     MoveResizeEwin(ewin, 0, root.h - (Conf.desks.num -
					       p->desktop) * ewin->h, w, h);
	  }
	PagerRedraw(p, 1);

	/* show the pager ewin */
	ShowEwin(ewin);
	if (((sn) && (sn->use_sticky) && (sn->sticky)) || (!sn))
	   MakeWindowSticky(ewin);
	RememberImportantInfoForEwin(ewin);
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

void
PagerDestroy(Pager * p)
{
   char                s[4096];

   RemoveItem("PAGER", p->win, LIST_FINDBY_ID, LIST_TYPE_PAGER);
   Esnprintf(s, sizeof(s), "__.%x", (unsigned)p->win);
   RemoveTimerEvent(s);
   if (p->name)
      Efree(p->name);
   EDestroyWindow(disp, p->win);
   if (p->hi_win)
      EDestroyWindow(disp, p->hi_win);
   if (p->pmap)
      EFreePixmap(disp, p->pmap);
   FreePmapMask(&p->bgpmap);
   if (p->border_name)
      Efree(p->border_name);
   Efree(p);
}

Pager             **
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

void
RedrawPagersForDesktop(int d, char newbg)
{
   Pager             **pl;
   int                 i, num;

   if (!Conf.pagers.enable)
      return;

   pl = PagersForDesktop(d, &num);
   if (pl)
     {
	for (i = 0; i < num; i++)
	   PagerRedraw(pl[i], newbg);
	Efree(pl);
     }
}

void
ForceUpdatePagersForDesktop(int d)
{
   Pager             **pl;
   int                 i, num;

   if (!Conf.pagers.enable)
      return;

   pl = PagersForDesktop(d, &num);
   if (pl)
     {
	for (i = 0; i < num; i++)
	   PagerForceUpdate(pl[i]);
	Efree(pl);
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
   cx = desks.desk[p->desktop].current_area_x;
   cy = desks.desk[p->desktop].current_area_y;

   w = ((ewin->w) * (p->w / ax)) / root.w;
   h = ((ewin->h) * (p->h / ay)) / root.h;

   if (w < 1)
      w = 1;
   if (h < 1)
      h = 1;
   if ((ewin->mini_w != w) || (ewin->mini_h != h))
     {
	FreePmapMask(&ewin->mini_pmm);

	ewin->mini_w = w;
	ewin->mini_h = h;

	if ((ewin->desktop != desks.current) || (ewin->area_x != cx)
	    || (ewin->area_y != cy) || (!Conf.pagers.snap))
	  {
	     ImageClass         *ic = NULL;

	     ic = FindItem("PAGER_WIN", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
	     if (ic)
		IclassApplyCopy(ic, ewin->win, w, h, 0, 0, STATE_NORMAL,
				&ewin->mini_pmm, 1);
	  }
	else
	  {
	     ewin->mini_pmm.type = 0;
	     ewin->mini_pmm.pmap =
		ECreatePixmap(disp, p->win, w, h, root.depth);
	     ScaleRect(ewin->mini_pmm.pmap, ewin->win, 0, 0, 0, 0, ewin->w,
		       ewin->h, w, h);
	  }
     }

   if (ewin == p->hi_ewin)
      PagerUpdateHiWin(p, ewin);
}

static void
PagerEwinUpdateFromPager(Pager * p, EWin * ewin)
{
   int                 x, y, w, h, ax, ay, cx, cy;
   static GC           gc = 0;
   XGCValues           gcv;

   if (!Conf.pagers.snap)
     {
	PagerEwinUpdateMini(p, ewin);
	return;
     }
   if (!Conf.pagers.enable)
      return;

   GetAreaSize(&ax, &ay);
   cx = desks.desk[p->desktop].current_area_x;
   cy = desks.desk[p->desktop].current_area_y;
   x = ((ewin->x + (cx * root.w)) * (p->w / ax)) / root.w;
   y = ((ewin->y + (cy * root.h)) * (p->h / ay)) / root.h;
   w = ((ewin->w) * (p->w / ax)) / root.w;
   h = ((ewin->h) * (p->h / ay)) / root.h;
   if (!gc)
      gc = XCreateGC(disp, p->pmap, 0, &gcv);

   if ((ewin->mini_w != w) || (ewin->mini_h != h))
      FreePmapMask(&ewin->mini_pmm);

   if (!ewin->mini_pmm.pmap)
     {
	ewin->mini_w = w;
	ewin->mini_h = h;
	ewin->mini_pmm.type = 0;
	ewin->mini_pmm.pmap = ECreatePixmap(disp, p->win, w, h, root.depth);
     }
   XCopyArea(disp, p->pmap, ewin->mini_pmm.pmap, gc, x, y, w, h, 0, 0);

   if (ewin == p->hi_ewin)
      PagerUpdateHiWin(p, ewin);
}

void
PagerRedraw(Pager * p, char newbg)
{
   int                 x, y, ax, ay, cx, cy;
   GC                  gc;
   XGCValues           gcv;
   EWin               *const *lst;
   int                 i, num;

   if (!Conf.pagers.enable || Mode.mode == MODE_DESKSWITCH)
      return;

   if (Mode.queue_up)
     {
	DrawQueue          *dq;

	dq = Emalloc(sizeof(DrawQueue));
	dq->win = p->win;
	dq->iclass = NULL;
	dq->w = p->w;
	dq->h = p->h;
	dq->active = 0;
	dq->sticky = 0;
	dq->state = 0;
	dq->expose = 0;
	dq->tclass = NULL;
	dq->text = NULL;
	dq->shape_propagate = 0;
	dq->pager = NULL;
	dq->redraw_pager = p;
	dq->newbg = newbg;
	dq->d = NULL;
	dq->di = NULL;
	dq->x = 0;
	dq->y = 0;
	AddItem(dq, "DRAW", dq->win, LIST_TYPE_DRAW);
	return;
     }

   p->update_phase = 0;
   GetAreaSize(&ax, &ay);
   cx = desks.desk[p->desktop].current_area_x;
   cy = desks.desk[p->desktop].current_area_y;
   gc = XCreateGC(disp, p->pmap, 0, &gcv);
   if (gc)
     {
	if ((newbg > 0) && (newbg < 3))
	  {
	     FreePmapMask(&p->bgpmap);

	     if (!Conf.pagers.snap)
	       {
		  ImageClass         *ic = NULL;

		  ic = FindItem("PAGER_BACKGROUND", 0, LIST_FINDBY_NAME,
				LIST_TYPE_ICLASS);
		  if (ic)
		     IclassApplyCopy(ic, p->win, p->w / ax, p->h / ay, 0, 0,
				     STATE_NORMAL, &p->bgpmap, 0);
	       }
	     else
	       {
		  if (desks.desk[p->desktop].bg)
		    {
		       char                s[4096];
		       char               *uniq;
		       Imlib_Image        *im;
		       Pixmap              mask;

		       uniq =
			  BackgroundGetUniqueString(desks.desk[p->desktop].bg);
		       Esnprintf(s, sizeof(s), "%s/cached/pager/%s.%i.%i.%s",
				 EDirUserCache(),
				 BackgroundGetName(desks.desk[p->desktop].bg),
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
			       ECreatePixmap(disp, p->win, p->w / ax, p->h / ay,
					     root.depth);
			    p->bgpmap.mask = None;
			    BackgroundApply(desks.desk[p->desktop].bg,
					    p->bgpmap.pmap, 0);
			    imlib_context_set_drawable(p->bgpmap.pmap);
			    im =
			       imlib_create_image_from_drawable(0, 0, 0,
								(p->w / ax),
								(p->h / ay), 0);
			    imlib_context_set_image(im);
			    imlib_image_set_format("ppm");
			    imlib_save_image(s);
			    imlib_free_image_and_decache();
			 }
		    }
		  else
		    {
		       p->bgpmap.type = 0;
		       p->bgpmap.pmap =
			  ECreatePixmap(disp, p->win, p->w / ax, p->h / ay,
					root.depth);
		       p->bgpmap.mask = None;
		       XSetForeground(disp, gc, BlackPixel(disp, root.scr));
		       XDrawRectangle(disp, p->bgpmap.pmap, gc, 0, 0, p->dw,
				      p->dh);
		       XSetForeground(disp, gc, WhitePixel(disp, root.scr));
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

	lst = EwinListGetForDesktop(p->desktop, &num);
	for (i = num - 1; i >= 0; i--)
	  {
	     EWin               *ewin;
	     int                 wx, wy, ww, wh;

	     ewin = lst[i];
	     if (!ewin->iconified && ewin->visible)
	       {
		  wx = ((ewin->x + (cx * root.w)) * (p->w / ax)) / root.w;
		  wy = ((ewin->y + (cy * root.h)) * (p->h / ay)) / root.h;
		  ww = ((ewin->w) * (p->w / ax)) / root.w;
		  wh = ((ewin->h) * (p->h / ay)) / root.h;
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
		       XSetForeground(disp, gc, BlackPixel(disp, root.scr));
		       XDrawRectangle(disp, p->pmap, gc, wx - 1, wy - 1, ww + 1,
				      wh + 1);
		       XSetForeground(disp, gc, WhitePixel(disp, root.scr));
		       XFillRectangle(disp, p->pmap, gc, wx, wy, ww, wh);
		    }
	       }
	  }

	if (newbg < 2)
	  {
	     ESetWindowBackgroundPixmap(disp, p->win, p->pmap);
	     XClearWindow(disp, p->win);
	  }

	XFreeGC(disp, gc);
     }
}

void
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

	dq = Emalloc(sizeof(DrawQueue));
	dq->win = p->win;
	dq->iclass = NULL;
	dq->w = p->w;
	dq->h = p->h;
	dq->active = 0;
	dq->sticky = 0;
	dq->state = 0;
	dq->expose = 0;
	dq->tclass = NULL;
	dq->text = NULL;
	dq->shape_propagate = 0;
	dq->pager = p;
	dq->redraw_pager = NULL;
	dq->d = NULL;
	dq->di = NULL;
	dq->x = 0;
	dq->y = 0;
	AddItem(dq, "DRAW", dq->win, LIST_TYPE_DRAW);
	return;
     }

   if ((p->desktop != desks.current) || (!Conf.pagers.snap))
     {
	PagerRedraw(p, 0);
	return;
     }

   p->update_phase = 0;
   GetAreaSize(&ax, &ay);
   cx = desks.desk[p->desktop].current_area_x;
   cy = desks.desk[p->desktop].current_area_y;
   ww = p->w / ax;
   hh = p->h / ay;
   xx = cx * ww;
   yy = cy * hh;

   ScaleRect(p->pmap, root.win, 0, 0, xx, yy, root.w, root.h, ww, hh);
   XClearWindow(disp, p->win);

   lst = EwinListGetForDesktop(p->desktop, &num);
   for (i = 0; i < num; i++)
      PagerEwinUpdateFromPager(p, lst[i]);
}

void
PagerReArea(void)
{
   Pager             **pl = NULL;
   int                 i, pnum, w, h, ax, ay;

   if (!Conf.pagers.enable)
      return;

   pl = (Pager **) ListItemType(&pnum, LIST_TYPE_PAGER);
   GetAreaSize(&ax, &ay);
   if (pl)
     {
	for (i = 0; i < pnum; i++)
	  {
	     w = pl[i]->dw * ax;
	     h = pl[i]->dh * ay;
	     if (pl[i]->ewin)
	       {
		  double              aspect;

		  aspect = ((double)root.w) / ((double)root.h);
		  pl[i]->ewin->client.w_inc = ax * 4;
		  pl[i]->ewin->client.h_inc = ay * 8;
		  pl[i]->ewin->client.aspect_min =
		     aspect * ((double)ax / (double)ay);
		  pl[i]->ewin->client.aspect_max =
		     aspect * ((double)ax / (double)ay);
		  MoveResizeEwin(pl[i]->ewin, pl[i]->ewin->x, pl[i]->ewin->y, w,
				 h);
	       }
	  }
	Efree(pl);
     }
}

void
PagerEwinOutsideAreaUpdate(EWin * ewin)
{
   if (!Conf.pagers.enable)
      return;

   if (ewin->sticky)
     {
	int                 i;

	for (i = 0; i < Conf.desks.num; i++)
	   RedrawPagersForDesktop(i, 0);
	ForceUpdatePagersForDesktop(ewin->desktop);
	return;
     }
   else if (ewin->desktop != desks.current)
     {
	RedrawPagersForDesktop(ewin->desktop, 0);
	ForceUpdatePagersForDesktop(ewin->desktop);
	return;
     }

   if ((ewin->x < 0) || (ewin->y < 0) || ((ewin->x + ewin->w) > root.w)
       || ((ewin->y + ewin->h) > root.h))
      RedrawPagersForDesktop(ewin->desktop, 3);
   ForceUpdatePagersForDesktop(ewin->desktop);
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
   cx = desks.desk[p->desktop].current_area_x;
   cy = desks.desk[p->desktop].current_area_y;
   lst = EwinListGetForDesktop(p->desktop, &num);
   for (i = 0; i < num; i++)
     {
	EWin               *ewin;

	ewin = lst[i];
	if ((ewin->visible) && (!ewin->iconified))
	  {
	     wx = ((ewin->x + (cx * root.w)) * (p->w / ax)) / root.w;
	     wy = ((ewin->y + (cy * root.h)) * (p->h / ay)) / root.h;
	     ww = ((ewin->w) * (p->w / ax)) / root.w;
	     wh = ((ewin->h) * (p->h / ay)) / root.h;
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

	pw_menu = MenuCreate("__DESK_WIN_MENU");
	MenuAddTitle(pw_menu, _("Window Options"));
	MenuAddStyle(pw_menu, "DEFAULT");

	Esnprintf(s, sizeof(s), "%i", (unsigned)ewin->client.win);
	mi = MenuItemCreate(_("Iconify"), NULL, ACTION_ICONIFY, s, NULL);
	MenuAddItem(pw_menu, mi);

	mi = MenuItemCreate(_("Close"), NULL, ACTION_KILL, s, NULL);
	MenuAddItem(pw_menu, mi);

	mi = MenuItemCreate(_("Annihilate"), NULL, ACTION_KILL_NASTY, s, NULL);
	MenuAddItem(pw_menu, mi);

	mi = MenuItemCreate(_("Stick / Unstick"), NULL, ACTION_STICK, s, NULL);
	MenuAddItem(pw_menu, mi);

	ActionsCall(ACTION_SHOW_MENU, NULL, "named __DESK_WIN_MENU");
	return;
     }

   PagerAreaAt(p, x, y, &ax, &ay);
   if (p_menu)
      MenuDestroy(p_menu);
   p_menu = MenuCreate("__DESK_MENU");
   MenuAddTitle(p_menu, _("Desktop Options"));
   MenuAddStyle(p_menu, "DEFAULT");

   mi = MenuItemCreate(_("Pager Settings..."), NULL, ACTION_CONFIG, "pager",
		       NULL);
   MenuAddItem(p_menu, mi);

   mi = MenuItemCreate(_("Snapshotting On"), NULL, ACTION_SET_PAGER_SNAP, "1",
		       NULL);
   MenuAddItem(p_menu, mi);

   mi = MenuItemCreate(_("Snapshotting Off"), NULL, ACTION_SET_PAGER_SNAP, "0",
		       NULL);
   MenuAddItem(p_menu, mi);

   if (Conf.pagers.snap)
     {
	mi = MenuItemCreate(_("High Quality On"), NULL, ACTION_SET_PAGER_HIQ,
			    "1", NULL);
	MenuAddItem(p_menu, mi);

	mi = MenuItemCreate(_("High Quality Off"), NULL, ACTION_SET_PAGER_HIQ,
			    "0", NULL);
	MenuAddItem(p_menu, mi);
     }

   ActionsCall(ACTION_SHOW_MENU, NULL, "named __DESK_MENU");
}

void
PagerHide(Pager * p)
{
   if (p->ewin)
      HideEwin(p->ewin);
}

static void
PagerTitle(Pager * p, char *title)
{
   XTextProperty       xtp;

   if (!Conf.pagers.enable)
      return;

   xtp.encoding = XA_STRING;
   xtp.format = 8;
   xtp.value = (unsigned char *)(title);
   xtp.nitems = strlen((char *)(xtp.value));
   XSetWMName(disp, p->win, &xtp);
}

void
UpdatePagerSel(void)
{
   Pager             **pl;
   Pager              *p;
   int                 i, pnum, cx, cy;
   ImageClass         *ic;

   if (!Conf.pagers.enable)
      return;

   pl = (Pager **) ListItemType(&pnum, LIST_TYPE_PAGER);
   if (pl)
     {
	for (i = 0; i < pnum; i++)
	  {
	     p = pl[i];
	     if (p->desktop != desks.current)
		EUnmapWindow(disp, p->sel_win);
	     else
	       {
		  cx = desks.desk[p->desktop].current_area_x;
		  cy = desks.desk[p->desktop].current_area_y;
		  EMoveWindow(disp, p->sel_win, cx * p->dw, cy * p->dh);
		  EMapWindow(disp, p->sel_win);
		  ic = FindItem("PAGER_SEL", 0, LIST_FINDBY_NAME,
				LIST_TYPE_ICLASS);
		  if (ic)
		     IclassApply(ic, p->sel_win, p->dw, p->dh, 0, 0,
				 STATE_NORMAL, 0);
	       }
	  }
	Efree(pl);
     }
}

static void
PagerShowTt(EWin * ewin)
{
   static EWin        *tt_ewin = NULL;
   ToolTip            *tt;

   if (!Conf.pagers.title || (ewin == tt_ewin))
      return;

   if (Mode.cur_menu_depth)	/* Don't show Tooltip when menu is up */
      return;

   tt = FindItem("PAGER", 0, LIST_FINDBY_NAME, LIST_TYPE_TOOLTIP);
   if (tt)
     {
	if (ewin)
	   ShowToolTip(tt, EwinGetIconName(ewin), NULL, Mode.x, Mode.y);
	else
	   HideToolTip(tt);
     }

   tt_ewin = ewin;
}

void
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

void
PagerHideHi(Pager * p)
{
   if (p->hi_visible)
     {
	p->hi_visible = 0;
	EUnmapWindow(disp, p->hi_win);
     }
   p->hi_ewin = NULL;

   PagerShowTt(NULL);
}

static void
PagerShowHi(Pager * p, EWin * ewin, int x, int y, int w, int h)
{
   char                pq;
   ImageClass         *ic = NULL;

   if (Mode.cur_menu_depth)	/* Don't show HiWin when menu is up */
      return;

   pq = Mode.queue_up;

   p->hi_win_w = 2 * w;
   p->hi_win_h = 2 * h;

   ic = FindItem("PAGER_WIN", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
   EMoveResizeWindow(disp, p->hi_win, x, y, w, h);
   EMapRaised(disp, p->hi_win);
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
		  ESetWindowBackgroundPixmap(disp, p->hi_win, pmap);
		  imlib_free_pixmap_and_mask(pmap);
		  EMoveResizeWindow(disp, p->hi_win, xx, yy, ww, hh);
		  XClearWindow(disp, p->hi_win);
		  {
		     int                 px, py;

		     PointerAt(&px, &py);
		     if ((px < x) || (py < y) || (px >= (x + w))
			 || (py >= (y + h)))
		       {
			  imlib_free_image_and_decache();
			  EUnmapWindow(disp, p->hi_win);
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
		  ESetWindowBackgroundPixmap(disp, p->hi_win, pmap);
		  imlib_free_pixmap_and_mask(pmap);
		  EMoveResizeWindow(disp, p->hi_win, xx, yy, ww, hh);
		  XClearWindow(disp, p->hi_win);
		  {
		     int                 px, py;

		     PointerAt(&px, &py);
		     if ((px < x) || (py < y) || (px >= (x + w))
			 || (py >= (y + h)))
		       {
			  imlib_free_image_and_decache();
			  EUnmapWindow(disp, p->hi_win);
			  goto done;
		       }
		  }
	       }
	  }
	EMoveResizeWindow(disp, p->hi_win, x - (w / 2), y - (h / 2),
			  w * 2, h * 2);
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
		  IclassApply(ic, p->hi_win, ww, hh, 0, 0, STATE_NORMAL, 0);
		  EMoveResizeWindow(disp, p->hi_win, xx, yy, ww, hh);
		  XClearWindow(disp, p->hi_win);
		  {
		     int                 px, py;

		     PointerAt(&px, &py);
		     if ((px < x) || (py < y) || (px >= (x + w))
			 || (py >= (y + h)))
		       {
			  EUnmapWindow(disp, p->hi_win);
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
		  IclassApply(ic, p->hi_win, ww, hh, 0, 0, STATE_NORMAL, 0);
		  EMoveResizeWindow(disp, p->hi_win, xx, yy, ww, hh);
		  XClearWindow(disp, p->hi_win);
		  {
		     int                 px, py;

		     PointerAt(&px, &py);
		     if ((px < x) || (py < y) || (px >= (x + w))
			 || (py >= (y + h)))
		       {
			  EUnmapWindow(disp, p->hi_win);
			  goto done;
		       }
		  }
	       }
	  }
	EMoveResizeWindow(disp, p->hi_win, x - (w / 2), y - (h / 2), w * 2,
			  h * 2);
     }
   else
     {
	Pixmap              pmap;
	GC                  gc = 0;
	XGCValues           gcv;
	int                 xx, yy, ww, hh, i;

	pmap = ECreatePixmap(disp, p->hi_win, w * 2, h * 2, root.depth);
	ESetWindowBackgroundPixmap(disp, p->hi_win, pmap);
	if (!gc)
	   gc = XCreateGC(disp, pmap, 0, &gcv);
	if (w > h)
	  {
	     for (i = w; i < (w * 2); i++)
	       {
		  ww = i;
		  hh = (i * h) / w;
		  xx = x + ((w - ww) / 2);
		  yy = y + ((h - hh) / 2);
		  XSetForeground(disp, gc, BlackPixel(disp, root.scr));
		  XFillRectangle(disp, pmap, gc, 0, 0, ww, hh);
		  XSetForeground(disp, gc, WhitePixel(disp, root.scr));
		  XFillRectangle(disp, pmap, gc, 1, 1, ww - 2, hh - 2);
		  EMoveResizeWindow(disp, p->hi_win, xx, yy, ww, hh);
		  XClearWindow(disp, p->hi_win);
		  {
		     int                 px, py;

		     PointerAt(&px, &py);
		     if ((px < x) || (py < y) || (px >= (x + w))
			 || (py >= (y + h)))
		       {
			  EFreePixmap(disp, pmap);
			  EUnmapWindow(disp, p->hi_win);
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
		  XSetForeground(disp, gc, BlackPixel(disp, root.scr));
		  XFillRectangle(disp, pmap, gc, 0, 0, ww, hh);
		  XSetForeground(disp, gc, WhitePixel(disp, root.scr));
		  XFillRectangle(disp, pmap, gc, 1, 1, ww - 2, hh - 2);
		  EMoveResizeWindow(disp, p->hi_win, xx, yy, ww, hh);
		  XClearWindow(disp, p->hi_win);
		  {
		     int                 px, py;

		     PointerAt(&px, &py);
		     if ((px < x) || (py < y) || (px >= (x + w))
			 || (py >= (y + h)))
		       {
			  EFreePixmap(disp, pmap);
			  EUnmapWindow(disp, p->hi_win);
			  goto done;
		       }
		  }
	       }
	  }
	EFreePixmap(disp, pmap);
	EMoveResizeWindow(disp, p->hi_win, x - (w / 2), y - (h / 2), w * 2,
			  h * 2);
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
   else if ((in == PAGER_EVENT_MOTION) && ewin != p->hi_ewin)
     {
	int                 wx, wy, ww, wh, ax, ay, cx, cy, px, py;

	PagerHideHi(p);
	GetAreaSize(&ax, &ay);
	cx = desks.desk[p->desktop].current_area_x;
	cy = desks.desk[p->desktop].current_area_y;
	wx = ((ewin->x + (cx * root.w)) * (p->w / ax)) / root.w;
	wy = ((ewin->y + (cy * root.h)) * (p->h / ay)) / root.h;
	ww = ((ewin->w) * (p->w / ax)) / root.w;
	wh = ((ewin->h) * (p->h / ay)) / root.h;
	XTranslateCoordinates(disp, p->win, root.win, 0, 0, &px, &py, &cw);
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

void
NewPagerForDesktop(int desk)
{

   Pager              *p;
   char                s[1024];

   p = PagerCreate();
   if (p)
     {
	p->desktop = desk;
	Esnprintf(s, sizeof(s), "%i", desk);
	PagerTitle(p, s);
	PagerShow(p);
     }
}

void
EnableSinglePagerForDesktop(int desk)
{
   Pager             **pl;
   int                 num;

   pl = PagersForDesktop(desk, &num);
   if (!pl)
      NewPagerForDesktop(desk);
   else
      Efree(pl);
}

void
EnableAllPagers(void)
{
   int                 i;

   if (!Conf.pagers.enable)
     {
	Conf.pagers.enable = 1;
	for (i = 0; i < Conf.desks.num; i++)
	   EnableSinglePagerForDesktop(i);
	UpdatePagerSel();
     }
   return;
}

int
PagerForDesktop(int desk)
{
   Pager             **pl;
   int                 num;

   pl = PagersForDesktop(desk, &num);
   if (pl)
      Efree(pl);
   return num;
}

void
DisablePagersForDesktop(int desk)
{
   Pager             **pl;

   int                 i, num;

   pl = PagersForDesktop(desk, &num);
   if (pl)
     {
	for (i = 0; i < num; i++)
	  {
	     if (pl[i]->ewin)
		ICCCM_Delete(pl[i]->ewin);
	  }
	Efree(pl);
     }
}

void
DisableAllPagers(void)
{
   int                 i;

   if (Conf.pagers.enable)
     {
	for (i = 0; i < Conf.desks.num; i++)
	   DisablePagersForDesktop(i);
	Conf.pagers.enable = 0;
     }
   return;
}

void
PagerSetHiQ(char onoff)
{
   Pager             **pl;
   EWin               *const *lst;
   int                 i, num;

   HIQ = onoff;

   lst = EwinListGetAll(&num);
   for (i = 0; i < num; i++)
     {
	lst[i]->mini_w = 0;
	lst[i]->mini_h = 0;
     }

   pl = (Pager **) ListItemType(&num, LIST_TYPE_PAGER);
   if (pl)
     {
	for (i = 0; i < num; i++)
	  {
	     PagerHideHi(pl[i]);
	     PagerRedraw(pl[i], 2);
	     PagerForceUpdate(pl[i]);
	  }
	Efree(pl);
     }
}

void
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
   if (pl)
     {
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
	Efree(pl);
     }
}

Window
PagerGetWin(Pager * p)
{
   return (p) ? p->win : 0;
}

Window
PagerGetHiWin(Pager * p)
{
   return (p) ? p->hi_win : 0;
}

Pager              *
FindPager(Window win)
{
   Pager              *p, *pr = NULL;
   Pager             **ps;
   int                 i, num;

   EDBUG(6, "FindDialog");

   ps = (Pager **) ListItemType(&num, LIST_TYPE_PAGER);
   for (i = 0; i < num; i++)
     {
	p = ps[i];
	if ((p->win == win) || (p->hi_win == win))
	  {
	     pr = p;
	     break;
	  }
     }
   if (ps)
      Efree(ps);

   EDBUG_RETURN(pr);
}

/*
 * Pager event handlers
 */

static int         *gwin_px, *gwin_py;

void
PagerEventUnmap(Pager * p)
{
   PagerHideHi(p);
   if (p == Mode.context_pager)
     {
	Mode.context_pager = NULL;
	Mode.mode = MODE_NONE;
     }
}

int
PagersEventMotion(XEvent * ev)
{
   int                 used = 0;
   Pager              *p;

   int                 x, y, dx, dy;
   int                 ax, ay, cx, cy, i, num;
   EWin              **gwins;

   switch (Mode.mode)
     {
     case MODE_NONE:
	p = FindPager(ev->xmotion.window);
	if (p == NULL)
	   break;
	used = 1;
	PagerHandleMotion(p, ev->xmotion.window, ev->xmotion.x,
			  ev->xmotion.y, PAGER_EVENT_MOTION);
	break;

     case MODE_PAGER_DRAG_PENDING:
     case MODE_PAGER_DRAG:
	Mode.mode = MODE_PAGER_DRAG;
	p = Mode.context_pager;
	if (p == NULL)
	   break;

	used = 1;
	cx = desks.desk[p->desktop].current_area_x;
	cy = desks.desk[p->desktop].current_area_y;
	GetAreaSize(&ax, &ay);
	dx = Mode.x - Mode.px;
	dy = Mode.y - Mode.py;
	if (!FindItem
	    ((char *)p->hi_ewin, 0, LIST_FINDBY_POINTER, LIST_TYPE_EWIN))
	   p->hi_ewin = NULL;
	if ((p->hi_ewin) && (!p->hi_ewin->pager) && (!p->hi_ewin->fixedpos))
	  {
	     Window              dw;
	     int                 px, py;

	     GetWinXY(p->hi_win, &x, &y);
	     XRaiseWindow(disp, p->hi_win);
	     x += dx;
	     y += dy;
	     EMoveWindow(disp, p->hi_win, x, y);
	     XTranslateCoordinates(disp, p->win, root.win, 0, 0, &px, &py, &dw);
	     x -= px + (cx * (p->w / ax));
	     y -= py + (cy * (p->h / ay));
	     MoveEwin(p->hi_ewin, (x * root.w * ax) / p->w,
		      (y * root.h * ay) / p->h);
	  }

	gwins =
	   ListWinGroupMembersForEwin(p->hi_ewin, ACTION_MOVE,
				      Mode.nogroup, &num);
	for (i = 0; i < num; i++)
	  {
	     if ((gwins[i] != p->hi_ewin) && (!gwins[i]->pager)
		 && (!gwins[i]->fixedpos))
	       {
		  GetWinXY(gwins[i]->win, &x, &y);
		  x += (dx * root.w * ax) / p->w;
		  y += (dy * root.h * ay) / p->h;
		  MoveEwin(gwins[i], x, y);
	       }
	  }
	if (gwins)
	   Efree(gwins);
	break;
     }

   return used;
}

int
PagersEventMouseDown(XEvent * ev)
{
   Window              win = ev->xbutton.window;
   int                 i, num;
   Pager              *p;
   EWin               *ewin, **gwins;

   p = FindPager(win);
   if (!p)
      return 0;

   gwins =
      ListWinGroupMembersForEwin(p->hi_ewin, ACTION_MOVE, Mode.nogroup, &num);
   gwin_px = calloc(num, sizeof(int));
   gwin_py = calloc(num, sizeof(int));

   for (i = 0; i < num; i++)
     {
	gwin_px[i] = gwins[i]->x;
	gwin_py[i] = gwins[i]->y;
     }

   if (gwins)
      Efree(gwins);

   if (win == p->hi_win)
     {
	int                 hx, hy;
	Window              dw;

	XTranslateCoordinates(disp, p->hi_win, p->win, 0, 0, &hx, &hy, &dw);
	ev->xbutton.x += hx;
	ev->xbutton.y += hy;
     }

   if ((int)ev->xbutton.button == Conf.pagers.menu_button)
     {
	if ((ev->xbutton.x >= 0) && (ev->xbutton.y >= 0)
	    && (ev->xbutton.x < p->w) && (ev->xbutton.y < p->h))
	  {
	     PagerHideHi(p);
	     PagerMenuShow(p, ev->xbutton.x, ev->xbutton.y);
	  }
     }
   else if ((int)ev->xbutton.button == Conf.pagers.win_button)
     {
	ewin = EwinInPagerAt(p, ev->xbutton.x, ev->xbutton.y);
	if ((ewin) && (!ewin->pager))
	  {
	     Window              dw;
	     int                 wx, wy, ww, wh, ax, ay, cx, cy, px, py;

	     PagerHideHi(p);
	     GetAreaSize(&ax, &ay);
	     cx = desks.desk[p->desktop].current_area_x;
	     cy = desks.desk[p->desktop].current_area_y;
	     wx = ((ewin->x + (cx * root.w)) * (p->w / ax)) / root.w;
	     wy = ((ewin->y + (cy * root.h)) * (p->h / ay)) / root.h;
	     ww = ((ewin->w) * (p->w / ax)) / root.w;
	     wh = ((ewin->h) * (p->h / ay)) / root.h;
	     XTranslateCoordinates(disp, p->win, root.win, 0, 0, &px, &py, &dw);
	     EMoveResizeWindow(disp, p->hi_win, px + wx, py + wy, ww, wh);
	     ESetWindowBackgroundPixmap(disp, p->hi_win, ewin->mini_pmm.pmap);
	     EMapRaised(disp, p->hi_win);
	     GrabThePointer(p->hi_win);
	     p->hi_visible = 1;
	     p->hi_ewin = ewin;
	     p->hi_win_w = ww;
	     p->hi_win_h = wh;
	     Mode.mode = MODE_PAGER_DRAG_PENDING;
	     Mode.context_pager = p;
	  }
     }

   return 1;
}

int
PagersEventMouseUp(XEvent * ev)
{
   Window              win = ev->xbutton.window;
   int                 used = 0;
   int                 i, num;
   Pager              *p;
   EWin               *ewin, **gwins;
   int                 x, y, pax, pay;
   int                 mode_was;

   p = FindPager(win);
   if (p == NULL)
      goto done;

   mode_was = Mode.mode;
   Mode.mode = MODE_NONE;

   if (((int)ev->xbutton.button == Conf.pagers.sel_button))
     {
	if (win != Mode.last_bpress)
	   goto done;
	PagerAreaAt(p, ev->xbutton.x, ev->xbutton.y, &pax, &pay);
	GotoDesktop(p->desktop);
	if (p->desktop != desks.current)
	   SoundPlay("SOUND_DESKTOP_SHUT");
	SetCurrentArea(pax, pay);
     }
   else if (((int)ev->xbutton.button == Conf.pagers.win_button))
     {
	int                 prev_desk = -1;

	if (win == p->hi_win)
	  {
	     int                 hx, hy;
	     Window              dw;

	     XTranslateCoordinates(disp, p->hi_win, p->win, 0, 0, &hx, &hy,
				   &dw);
	     ev->xbutton.x += hx;
	     ev->xbutton.y += hy;
	  }

	if (!FindItem((char *)p->hi_ewin, 0,
		      LIST_FINDBY_POINTER, LIST_TYPE_EWIN))
	   p->hi_ewin = NULL;

	switch (mode_was)
	  {
	  case MODE_PAGER_DRAG:
	     if (!p->hi_ewin)
		break;

	     /* Remember old desk for the dragged window */
	     prev_desk = p->hi_ewin->desktop;

	     /* Find which pager or iconbox we are in (if any) */
	     ewin = GetEwinPointerInClient();
	     if ((ewin) && (ewin->pager))
	       {
		  /* Pointer is in pager */
		  Pager              *pp;
		  int                 w, h, ax, ay, cx, cy, px, py;
		  int                 wx, wy, base_x = 0, base_y = 0;
		  Window              dw;

		  pp = ewin->pager;
		  cx = desks.desk[pp->desktop].current_area_x;
		  cy = desks.desk[pp->desktop].current_area_y;
		  GetAreaSize(&ax, &ay);
		  GetWinXY(p->hi_win, &x, &y);
		  GetWinWH(p->hi_win, &w, &h);
		  XTranslateCoordinates(disp, pp->win, root.win, 0, 0, &px,
					&py, &dw);
		  wx = ((x - px) -
			(cx * (pp->w / ax))) * (root.w / (pp->w / ax));
		  wy = ((y - py) -
			(cy * (pp->h / ay))) * (root.h / (pp->h / ay));
		  if (((x + w) <= px) || ((y + h) <= py)
		      || (x >= (px + pp->w)) || (y >= (py + pp->h)))
		    {
		       int                 nx, ny;

		       nx = (int)ev->xbutton.x_root -
			  desks.desk[desks.current].x -
			  ((int)p->hi_ewin->w / 2);
		       ny = (int)ev->xbutton.y_root -
			  desks.desk[desks.current].y -
			  ((int)p->hi_ewin->h / 2);

		       MoveEwinToDesktopAt(p->hi_ewin, desks.current, nx, ny);
		    }
		  else
		    {
		       gwins =
			  ListWinGroupMembersForEwin(p->hi_ewin,
						     ACTION_MOVE,
						     Mode.nogroup, &num);
		       /* get get the location of the base win so we can move the */
		       /* rest of the windows in the group to the correct offset */
		       for (i = 0; i < num; i++)
			  if (gwins[i] == p->hi_ewin)
			    {
			       base_x = gwin_px[i];
			       base_y = gwin_py[i];
			    }
		       for (i = 0; i < num; i++)
			 {
			    x = wx + gwin_px[i] - base_x;
			    y = wy + gwin_py[i] - base_y;
			    MoveEwinToDesktopAt(gwins[i], pp->desktop, x, y);
			 }
		       if (gwins)
			  Efree(gwins);
		    }
	       }
	     else if ((ewin) && (ewin->ibox))
	       {
		  /* Pointer is in iconbox */

		  /* Don't iconify an iconbox by dragging */
		  if (p->hi_ewin->ibox)
		     break;

		  /* Iconify after moving back to pre-drag position */
		  gwins = ListWinGroupMembersForEwin(p->hi_ewin, ACTION_MOVE,
						     Mode.nogroup, &num);
		  for (i = 0; i < num; i++)
		    {
		       if (!gwins[i]->pager)
			 {
			    MoveEwin(gwins[i], gwin_px[i], gwin_py[i]);
			    IconboxIconifyEwin(ewin->ibox, gwins[i]);
			 }
		    }
		  if (gwins)
		     Efree(gwins);
	       }
	     else
	       {
		  /* Pointer is not in pager or iconbox */
		  int                 nx, ny, base_x = 0, base_y = 0, ax, ay;

		  /* Move window(s) to pointer location */
		  nx = (int)ev->xbutton.x_root -
		     desks.desk[desks.current].x - ((int)p->hi_ewin->w / 2);
		  ny = (int)ev->xbutton.y_root -
		     desks.desk[desks.current].y - ((int)p->hi_ewin->h / 2);
		  GetAreaSize(&ax, &ay);

		  gwins = ListWinGroupMembersForEwin(p->hi_ewin, ACTION_MOVE,
						     Mode.nogroup, &num);
		  for (i = 0; i < num; i++)
		     if (gwins[i] == p->hi_ewin)
		       {
			  base_x = gwin_px[i];
			  base_y = gwin_py[i];
		       }
		  for (i = 0; i < num; i++)
		    {
		       x = nx + gwin_px[i] - base_x;
		       y = ny + gwin_py[i] - base_y;
		       MoveEwinToDesktopAt(gwins[i], desks.current, x, y);
		    }
		  if (gwins)
		     Efree(gwins);
	       }
	     break;

	  default:
	     if ((ev->xbutton.x < 0) || (ev->xbutton.y < 0) ||
		 (ev->xbutton.x >= p->w) || (ev->xbutton.y >= p->h))
		break;

	     PagerAreaAt(p, ev->xbutton.x, ev->xbutton.y, &pax, &pay);
	     GotoDesktop(p->desktop);
	     SetCurrentArea(pax, pay);
	     ewin = EwinInPagerAt(p, ev->xbutton.x, ev->xbutton.y);
	     if (ewin)
	       {
		  RaiseEwin(ewin);
		  FocusToEWin(ewin, FOCUS_SET);
	       }
	     break;
	  }

	if (p->hi_ewin)
	  {
	     RedrawPagersForDesktop(p->hi_ewin->desktop, 3);
	     ForceUpdatePagersForDesktop(p->hi_ewin->desktop);
	     if (prev_desk >= 0 && prev_desk != p->hi_ewin->desktop)
	       {
		  RedrawPagersForDesktop(prev_desk, 3);
		  ForceUpdatePagersForDesktop(prev_desk);
	       }
	     PagerHideHi(p);
	  }

	Mode.context_pager = NULL;
	UnGrabTheButtons();
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

int
PagersEventMouseIn(XEvent * ev)
{
   Pager              *p;
   Window              win = ev->xcrossing.window;

   p = FindPager(win);
   if (p)
     {
#if 0				/* Nothing done here */
	PagerHandleMotion(p, win, ev->xcrossing.x, ev->xcrossing.y,
			  PAGER_EVENT_MOUSE_IN);
#endif
	return 1;
     }

   return 0;
}

int
PagersEventMouseOut(XEvent * ev)
{
   Pager              *p;
   Window              win = ev->xcrossing.window;

   p = FindPager(win);
   if (p)
     {
	PagerHandleMotion(p, win, ev->xcrossing.x, ev->xcrossing.y,
			  PAGER_EVENT_MOUSE_OUT);
	return 1;
     }

   return 0;
}
