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
#include <sys/time.h>

typedef struct _awaiticlass
{
   Window              client_win;
   int                 ewin_bit;
   ImageClass         *iclass;
}
AwaitIclass;

#define EWIN_BORDER_PART_EVENT_MASK \
  (KeyPressMask | KeyReleaseMask | ButtonPressMask | ButtonReleaseMask | \
   EnterWindowMask | LeaveWindowMask | PointerMotionMask | ExposureMask)
#define EWIN_BORDER_TITLE_EVENT_MASK \
  (EWIN_BORDER_PART_EVENT_MASK | ExposureMask)

static void         BorderWinpartHandleEvents(XEvent * ev, void *prm);
static void         BorderFrameHandleEvents(XEvent * ev, void *prm);

void
SyncBorderToEwin(EWin * ewin)
{
   const Border       *b;

   EDBUG(4, "SyncBorderToEwin");
   b = ewin->border;
   ICCCM_GetShapeInfo(ewin);
   EwinSetBorder(ewin, b, 1);
   EDBUG_RETURN_;
}

void
EwinBorderUpdateState(EWin * ewin)
{
   EwinBorderDraw(ewin, 0, 0);
}

static void
BorderWinpartRealise(EWin * ewin, int i)
{
   EWinBit            *ewb = &ewin->bits[i];

   EDBUG(4, "BorderWinpartRealise");

   if ((ewb->cx != ewb->x) || (ewb->cy != ewb->y) ||
       (ewb->cw != ewb->w) || (ewb->ch != ewb->h))
     {
	if ((ewb->w < 0) || (ewb->h < 0))
	  {
	     EUnmapWindow(disp, ewb->win);
	  }
	else
	  {
	     EMapWindow(disp, ewb->win);
	     EMoveResizeWindow(disp, ewb->win, ewb->x, ewb->y, ewb->w, ewb->h);
	  }
     }
   EDBUG_RETURN_;
}

static void
BorderWinpartITclassApply(EWin * ewin, int i, int force)
{
   EWinBit            *ewb = &ewin->bits[i];
   ImageState         *is;
   TextState          *ts;
   const char         *title;

   if (ewb->win == None)
      return;

   is = ImageclassGetImageState(ewin->border->part[i].iclass,
				ewin->bits[i].state,
				ewin->active, EoIsSticky(ewin));
   ts = NULL;
   if (ewin->border->part[i].tclass)
      ts = TextGetState(ewin->border->part[i].tclass, ewin->active,
			EoIsSticky(ewin), ewin->bits[i].state);
   if (!force && ewin->bits[i].is == is && ewin->bits[i].ts == ts)
      return;
   ewin->bits[i].is = is;
   ewin->bits[i].ts = ts;

   ImageclassApply(ewin->border->part[i].iclass, ewb->win,
		   ewb->w, ewb->h, ewin->active,
		   EoIsSticky(ewin), ewb->state, ewb->expose, ST_BORDER);

   switch (ewin->border->part[i].flags)
     {
     case FLAG_TITLE:
	title = EwinGetName(ewin);
	if (title)
	   TextclassApply(ewin->border->part[i].iclass, ewb->win,
			  ewb->w, ewb->h, ewin->active,
			  EoIsSticky(ewin), ewb->state, ewb->expose,
			  ewin->border->part[i].tclass, title);
	break;
     case FLAG_MINIICON:
	break;
     default:
	break;
     }
}

static int
BorderWinpartDraw(EWin * ewin, int i)
{
   EWinBit            *ewb = &ewin->bits[i];
   int                 move = 0, resize = 0, ret = 0;

   EDBUG(4, "BorderWinpartDraw");

   if ((ewb->x != ewb->cx) || (ewb->y != ewb->cy))
     {
	move = 1;
	ewb->cx = ewb->x;
	ewb->cy = ewb->y;
	ret = 1;
     }

   if ((ewb->w != ewb->cw) || (ewb->h != ewb->ch))
     {
	resize = 1;
	ewb->cw = ewb->w;
	ewb->ch = ewb->h;
     }

   if ((resize) || (ewb->expose))
     {
	BorderWinpartITclassApply(ewin, i, 1);
	ewb->expose = 0;
	ret = 1;
     }

   EDBUG_RETURN(ret);
}

void
BorderWinpartChange(EWin * ewin, int i, int force)
{
   EDBUG(3, "BorderWinpartChange");

   BorderWinpartITclassApply(ewin, i, force);

   if (!ewin->shapedone || ewin->border->changes_shape)
      EwinPropagateShapes(ewin);
   ewin->shapedone = 1;

   EDBUG_RETURN_;
}

void
EwinBorderDraw(EWin * ewin, int do_shape, int queue_off)
{
   int                 i, pq;

   EDBUG(4, "EwinBorderDraw");

   if (!ewin)
      EDBUG_RETURN_;

   pq = Mode.queue_up;
   if (queue_off)
      Mode.queue_up = 0;

   for (i = 0; i < ewin->border->num_winparts; i++)
      BorderWinpartITclassApply(ewin, i, do_shape);

   if (do_shape || !ewin->shapedone || ewin->border->changes_shape)
      EwinPropagateShapes(ewin);
   ewin->shapedone = 1;

   if (queue_off)
      Mode.queue_up = pq;

   EDBUG_RETURN_;
}

void
EwinBorderUpdateInfo(EWin * ewin)
{
   int                 i;

   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	if (ewin->border->part[i].flags == FLAG_TITLE)
	   BorderWinpartITclassApply(ewin, i, 1);
     }
}

static void
BorderWinpartCalc(EWin * ewin, int i)
{
   int                 x, y, w, h, ox, oy, max, min;
   int                 topleft, bottomright;

   EDBUG(4, "BorderWinpartCalc");
   topleft = ewin->border->part[i].geom.topleft.originbox;
   bottomright = ewin->border->part[i].geom.bottomright.originbox;
   if (topleft >= 0)
      BorderWinpartCalc(ewin, topleft);
   if (bottomright >= 0)
      BorderWinpartCalc(ewin, bottomright);
   x = y = 0;
   if (topleft == -1)
     {
	x = ((ewin->border->part[i].geom.topleft.x.percent *
	      EoGetW(ewin)) >> 10) +
	   ewin->border->part[i].geom.topleft.x.absolute;
	y = ((ewin->border->part[i].geom.topleft.y.percent *
	      EoGetH(ewin)) >> 10) +
	   ewin->border->part[i].geom.topleft.y.absolute;
     }
   else if (topleft >= 0)
     {
	x = ((ewin->border->part[i].geom.topleft.x.percent *
	      ewin->bits[topleft].w) >> 10) +
	   ewin->border->part[i].geom.topleft.x.absolute +
	   ewin->bits[topleft].x;
	y = ((ewin->border->part[i].geom.topleft.y.percent *
	      ewin->bits[topleft].h) >> 10) +
	   ewin->border->part[i].geom.topleft.y.absolute +
	   ewin->bits[topleft].y;
     }
   ox = oy = 0;
   if (bottomright == -1)
     {
	ox = ((ewin->border->
	       part[i].geom.bottomright.x.percent * EoGetW(ewin)) >> 10) +
	   ewin->border->part[i].geom.bottomright.x.absolute;
	oy = ((ewin->border->
	       part[i].geom.bottomright.y.percent * EoGetH(ewin)) >> 10) +
	   ewin->border->part[i].geom.bottomright.y.absolute;
     }
   else if (bottomright >= 0)
     {
	ox = ((ewin->border->part[i].geom.bottomright.x.percent *
	       ewin->bits[bottomright].w) >> 10) +
	   ewin->border->part[i].geom.bottomright.x.absolute +
	   ewin->bits[bottomright].x;
	oy = ((ewin->border->part[i].geom.bottomright.y.percent *
	       ewin->bits[bottomright].h) >> 10) +
	   ewin->border->part[i].geom.bottomright.y.absolute +
	   ewin->bits[bottomright].y;
     }
   /*
    * calculate height before width, because we may need it in order to
    * determine the font size. But we might do it the other way around for
    * side borders :-)
    */

   h = (oy - y) + 1;
   max = ewin->border->part[i].geom.height.max;
   min = ewin->border->part[i].geom.height.min;

   /*
    * If the title bar max size is set to zero, then set the title bar size to
    * just a little bit more than the size of the title text.
    */

   if (max == 0 && ewin->border->part[i].flags == FLAG_TITLE)
     {
	int                 dummywidth, wmax, wmin;
	ImageClass         *iclass;
	TextClass          *tclass;

	/*
	 * calculate width before height, because we need it in order to
	 * determine the font size.
	 */

	w = (ox - x) + 1;
	wmax = ewin->border->part[i].geom.width.max;
	wmin = ewin->border->part[i].geom.width.min;
	if (w > wmax)
	  {
	     w = wmax;
	     x = ((x + ox) - w) >> 1;
	  }
	else if (w < wmin)
	  {
	     w = wmin;
	  }
	iclass = ewin->border->part[i].iclass;
	tclass = ewin->border->part[i].tclass;
	TextSize(tclass, ewin->active, EoIsSticky(ewin), ewin->bits[i].state,
		 EwinGetName(ewin), &max, &dummywidth,
		 w - (iclass->padding.top + iclass->padding.bottom));
	max += iclass->padding.left + iclass->padding.right;
	if (h > max)
	  {
	     y = y + (((h - max) * tclass->justification) >> 10);
	     h = max;
	  }
	if (h < min)
	  {
	     h = min;
	  }
     }
   else
     {
	if (h > max)
	  {
	     h = max;
	     y = ((y + oy) - h) >> 1;
	  }
	else if (h < min)
	  {
	     h = min;
	  }
	/*
	 * and now the width.
	 */

	w = (ox - x) + 1;
	max = ewin->border->part[i].geom.width.max;
	min = ewin->border->part[i].geom.width.min;

	/*
	 * If the title bar max size is set to zero, then set the title bar
	 * size to just a little bit more than the size of the title text.
	 */

	if (max == 0 && ewin->border->part[i].flags == FLAG_TITLE)
	  {
	     int                 dummyheight;

	     ImageClass         *iclass;
	     TextClass          *tclass;

	     iclass = ewin->border->part[i].iclass;
	     tclass = ewin->border->part[i].tclass;
	     TextSize(tclass, ewin->active, EoIsSticky(ewin),
		      ewin->bits[i].state, EwinGetName(ewin), &max,
		      &dummyheight,
		      h - (iclass->padding.top + iclass->padding.bottom));
	     max += iclass->padding.left + iclass->padding.right;

	     if (w > max)
	       {
		  x = x + (((w - max) * tclass->justification) >> 10);
		  w = max;
	       }
	  }
	if (w > max)
	  {
	     w = max;
	     x = ((x + ox) - w) >> 1;
	  }
	else if (w < min)
	  {
	     w = min;
	  }
     }
   if ((ewin->shaded) && (!ewin->border->part[i].keep_for_shade))
     {
	ewin->bits[i].x = -100;
	ewin->bits[i].y = -100;
	ewin->bits[i].w = -1;
	ewin->bits[i].h = -1;
     }
   else
     {
	ewin->bits[i].x = x;
	ewin->bits[i].y = y;
	ewin->bits[i].w = w;
	ewin->bits[i].h = h;
     }
   EDBUG_RETURN_;
}

void
EwinBorderCalcSizes(EWin * ewin)
{
   int                 i;
   char                reshape;

   EDBUG(4, "EwinBorderCalcSizes");

   if (!ewin)
      EDBUG_RETURN_;
   if (!ewin->border)
      EDBUG_RETURN_;

   for (i = 0; i < ewin->border->num_winparts; i++)
      ewin->bits[i].w = -2;
   for (i = 0; i < ewin->border->num_winparts; i++)
      if (ewin->bits[i].w == -2)
	 BorderWinpartCalc(ewin, i);
   for (i = 0; i < ewin->border->num_winparts; i++)
      BorderWinpartRealise(ewin, i);

   reshape = 0;
   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	reshape |= BorderWinpartDraw(ewin, i);
	ewin->bits[i].no_expose = 1;
     }

   if ((reshape) || (Mode.have_place_grab))
     {
	if (Mode.have_place_grab)
	  {
	     char                pq;

	     pq = Mode.queue_up;
	     Mode.queue_up = 0;
	     EwinPropagateShapes(ewin);
	     Mode.queue_up = pq;
	  }
	else
	   EwinPropagateShapes(ewin);
	ewin->shapedone = 1;
     }

   EDBUG_RETURN_;
}

void
HonorIclass(char *s, int id)
{
   AwaitIclass        *a;
   EWin               *ewin;

   EDBUG(4, "HonorIclass");

   a = RemoveItem(s, 0, LIST_FINDBY_NAME, LIST_TYPE_AWAIT_ICLASS);
   if (!a)
      EDBUG_RETURN_;

   ewin = FindItem(NULL, a->client_win, LIST_FINDBY_ID, LIST_TYPE_EWIN);
   if (ewin)
     {
	if (a->ewin_bit < ewin->border->num_winparts)
	  {
	     if ((ewin->border->part[a->ewin_bit].iclass->external)
		 && (!ewin->bits[a->ewin_bit].win) && (id))
	       {
		  ewin->bits[a->ewin_bit].win = id;
		  BorderWinpartRealise(ewin, a->ewin_bit);
		  EMapWindow(disp, id);
		  ewin->shapedone = 0;
		  if (!ewin->shapedone)
		    {
		       EwinPropagateShapes(ewin);
		    }
		  else
		    {
		       if (ewin->border->changes_shape)
			  EwinPropagateShapes(ewin);
		    }
		  ewin->shapedone = 1;
	       }
	  }
     }
   if (a->iclass)
      a->iclass->ref_count--;

   Efree(a);

   EDBUG_RETURN_;
}

static void
BorderIncRefcount(const Border * b)
{
   ((Border *) b)->ref_count++;
}

static void
BorderDecRefcount(const Border * b)
{
   ((Border *) b)->ref_count--;
}

void
EwinBorderSelect(EWin * ewin)
{
   const Border       *b;

   /* Quit if we allready have a border that isn't the fallback one */
   b = ewin->border;
   if (b && strcmp(b->name, "__FALLBACK_BORDER"))
      return;

   ICCCM_GetShapeInfo(ewin);

   if ((!ewin->client.mwm_decor_title) && (!ewin->client.mwm_decor_border))
      b = (Border *) FindItem("BORDERLESS", 0, LIST_FINDBY_NAME,
			      LIST_TYPE_BORDER);
   else
      b = MatchEwinByFunction(ewin,
			      (void
			       *(*)(EWin *, WindowMatch *))(MatchEwinBorder));
   if (Conf.dock.enable && ewin->docked)
      b = (Border *) FindItem("BORDERLESS", 0, LIST_FINDBY_NAME,
			      LIST_TYPE_BORDER);
   if (!b)
      b = (Border *) FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);

   if (!b)
      b = FindItem("__FALLBACK_BORDER", 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);

   ewin->normal_border = ewin->border = b;
}

void
EwinBorderDetach(EWin * ewin)
{
   const Border       *b = ewin->border;
   int                 i;

   if (!b)
      return;

   EventCallbackUnregister(EoGetWin(ewin), 0, BorderFrameHandleEvents, ewin);
   for (i = 0; i < b->num_winparts; i++)
     {
	EventCallbackUnregister(ewin->bits[i].win, 0,
				BorderWinpartHandleEvents, &ewin->bits[i]);
	if (ewin->bits[i].win)
	   EDestroyWindow(disp, ewin->bits[i].win);
     }
   if (ewin->bits)
      Efree(ewin->bits);
   ewin->bits = NULL;
   BorderDecRefcount(b);

   ewin->border = NULL;
}

void
EwinBorderSetTo(EWin * ewin, const Border * b)
{
   int                 i;
   char                s[1024];

   AwaitIclass        *await;

   EDBUG(4, "EwinBorderSetTo");

   if (ewin->border == b)
      EDBUG_RETURN_;

   if (b == NULL)
     {
	b = ewin->border;
	ewin->border = NULL;
     }

   if (ewin->border)
      EwinBorderDetach(ewin);

   ewin->border = b;
   BorderIncRefcount(b);
   HintsSetWindowBorder(ewin);

   EventCallbackRegister(EoGetWin(ewin), 0, BorderFrameHandleEvents, ewin);

   if (b->num_winparts > 0)
      ewin->bits = Emalloc(sizeof(EWinBit) * b->num_winparts);

   for (i = 0; i < b->num_winparts; i++)
     {
	ewin->bits[i].ewin = ewin;	/* Reference to associated Ewin */
	if (b->part[i].iclass->external)
	  {
	     ewin->bits[i].win = 0;
	     Esnprintf(s, sizeof(s), "request imageclass %s",
		       b->part[i].iclass->name);
	     CommsBroadcast(s);
	     await = Emalloc(sizeof(AwaitIclass));
	     await->client_win = ewin->client.win;
	     await->ewin_bit = i;

	     await->iclass = b->part[i].iclass;
	     if (await->iclass)
		await->iclass->ref_count++;

	     AddItem(await, b->part[i].iclass->name, 0, LIST_TYPE_AWAIT_ICLASS);
	  }
	else
	  {
	     ewin->bits[i].win =
		ECreateWindow(EoGetWin(ewin), -10, -10, 1, 1, 0);
	     ECursorApply(b->part[i].ec, ewin->bits[i].win);
	     EMapWindow(disp, ewin->bits[i].win);
	     EventCallbackRegister(ewin->bits[i].win, 0,
				   BorderWinpartHandleEvents, &ewin->bits[i]);
	     /*
	      * KeyPressMask KeyReleaseMask ButtonPressMask 
	      * ButtonReleaseMask
	      * EnterWindowMask LeaveWindowMask PointerMotionMask 
	      * PointerMotionHintMask Button1MotionMask 
	      * Button2MotionMask
	      * Button3MotionMask Button4MotionMask Button5MotionMask
	      * ButtonMotionMask KeymapStateMask ExposureMask 
	      * VisibilityChangeMask StructureNotifyMask 
	      * ResizeRedirectMask 
	      * SubstructureNotifyMask SubstructureRedirectMask 
	      * FocusChangeMask PropertyChangeMas ColormapChangeMask 
	      * OwnerGrabButtonMask
	      */
	     if (b->part[i].flags & FLAG_TITLE)
		XSelectInput(disp, ewin->bits[i].win,
			     EWIN_BORDER_TITLE_EVENT_MASK);
	     else
		XSelectInput(disp, ewin->bits[i].win,
			     EWIN_BORDER_PART_EVENT_MASK);
	     ewin->bits[i].x = -10;
	     ewin->bits[i].y = -10;
	     ewin->bits[i].w = -10;
	     ewin->bits[i].h = -10;
	     ewin->bits[i].cx = -99;
	     ewin->bits[i].cy = -99;
	     ewin->bits[i].cw = -99;
	     ewin->bits[i].ch = -99;
	     ewin->bits[i].state = 0;
	     ewin->bits[i].expose = 0;
	     ewin->bits[i].no_expose = 0;
	     ewin->bits[i].left = 0;
	     ewin->bits[i].is = NULL;
	  }
     }

   {
      Window             *wl;
      int                 j = 0;

      wl = Emalloc((b->num_winparts + 1) * sizeof(Window));
      for (i = b->num_winparts - 1; i >= 0; i--)
	{
	   if (b->part[i].ontop)
	      wl[j++] = ewin->bits[i].win;
	}
      wl[j++] = ewin->win_container;
      for (i = b->num_winparts - 1; i >= 0; i--)
	{
	   if (!b->part[i].ontop)
	      wl[j++] = ewin->bits[i].win;
	}
      XRestackWindows(disp, wl, j);
      Efree(wl);
   }

   if (!ewin->shaded)
      EMoveWindow(disp, ewin->win_container, b->border.left, b->border.top);

   EwinBorderCalcSizes(ewin);
   EwinPropagateShapes(ewin);

   EDBUG_RETURN_;
}

void
EwinSetBorder(EWin * ewin, const Border * b, int apply)
{
   if (!b)
      return;

   if (apply)
     {
	if (ewin->border != b)
	  {
	     EwinBorderSetTo(ewin, b);
	     ICCCM_MatchSize(ewin);
	     MoveResizeEwin(ewin, EoGetX(ewin), EoGetY(ewin),
			    ewin->client.w, ewin->client.h);
	  }
     }
   else
     {
	if (ewin->border)
	   BorderDecRefcount(ewin->border);
	ewin->border = b;
	if (b)
	   BorderIncRefcount(b);
     }

   if (!ewin->st.fullscreen)
      ewin->normal_border = b;
}

void
EwinSetBorderByName(EWin * ewin, const char *name, int apply)
{
   Border             *b;

   b = (Border *) FindItem(name, 0, LIST_FINDBY_NAME, LIST_TYPE_BORDER);

   EwinSetBorder(ewin, b, apply);
}

Border             *
BorderCreate(const char *name)
{
   Border             *b;

   EDBUG(5, "BorderCreate");

   b = Ecalloc(1, sizeof(Border));
   if (!b)
      EDBUG_RETURN(NULL);

   b->name = Estrdup(name);
   b->group_border_name = NULL;
   b->shadedir = 2;

   EDBUG_RETURN(b);
}

void
BorderDestroy(Border * b)
{
   int                 i;

   EDBUG(3, "BorderDestroy");

   if (!b)
      EDBUG_RETURN_;

   if (b->ref_count > 0)
     {
	DialogOK(_("Border Error!"), _("%u references remain\n"), b->ref_count);
	EDBUG_RETURN_;
     }

   while (RemoveItemByPtr(b, LIST_TYPE_BORDER));

   for (i = 0; i < b->num_winparts; i++)
     {
	if (b->part[i].iclass)
	   b->part[i].iclass->ref_count--;
	if (b->part[i].tclass)
	   b->part[i].tclass->ref_count--;
	if (b->part[i].aclass)
	   ActionclassDecRefcount(b->part[i].aclass);
	if (b->part[i].ec)
	   ECursorDecRefcount(b->part[i].ec);
     }

   if (b->num_winparts > 0)
      Efree(b->part);

   if (b->name)
      Efree(b->name);
   if (b->group_border_name)
      Efree(b->group_border_name);
   if (b->aclass)
      ActionclassDecRefcount(b->aclass);

   EDBUG_RETURN_;
}

void
BorderWinpartAdd(Border * b, ImageClass * iclass, ActionClass * aclass,
		 TextClass * tclass, ECursor * ec, char ontop, int flags,
		 char isregion, int wmin, int wmax, int hmin, int hmax,
		 int torigin, int txp, int txa, int typ, int tya, int borigin,
		 int bxp, int bxa, int byp, int bya, char keep_for_shade)
{
   int                 n;

   EDBUG(6, "BorderWinpartAdd");

   b->num_winparts++;
   n = b->num_winparts;

   isregion = 0;

   b->part = Erealloc(b->part, n * sizeof(WinPart));

   if (!iclass)
      iclass = ImageclassFind(NULL, 0);

   b->part[n - 1].iclass = iclass;
   if (iclass)
      iclass->ref_count++;

   b->part[n - 1].aclass = aclass;
   if (aclass)
      ActionclassDecRefcount(aclass);

   b->part[n - 1].tclass = tclass;
   if (tclass)
      tclass->ref_count++;

   b->part[n - 1].ec = ec;
   if (ec)
      ECursorIncRefcount(ec);

   b->part[n - 1].ontop = ontop;
   b->part[n - 1].flags = flags;
   b->part[n - 1].keep_for_shade = keep_for_shade;
   b->part[n - 1].geom.width.min = wmin;
   b->part[n - 1].geom.width.max = wmax;
   b->part[n - 1].geom.height.min = hmin;
   b->part[n - 1].geom.height.max = hmax;
   b->part[n - 1].geom.topleft.originbox = torigin;
   b->part[n - 1].geom.topleft.x.percent = txp;
   b->part[n - 1].geom.topleft.x.absolute = txa;
   b->part[n - 1].geom.topleft.y.percent = typ;
   b->part[n - 1].geom.topleft.y.absolute = tya;
   b->part[n - 1].geom.bottomright.originbox = borigin;
   b->part[n - 1].geom.bottomright.x.percent = bxp;
   b->part[n - 1].geom.bottomright.x.absolute = bxa;
   b->part[n - 1].geom.bottomright.y.percent = byp;
   b->part[n - 1].geom.bottomright.y.absolute = bya;

   EDBUG_RETURN_;
}

void
EwinBorderMinShadeSize(EWin * ewin, int *mw, int *mh)
{
   int                 i, pw, ph, w, h, min_w, min_h;
   int                 leftborderwidth, rightborderwidth;
   int                 topborderwidth, bottomborderwidth;

   min_w = 32;
   min_h = 32;

   if (!ewin)
      goto done;

   pw = EoGetW(ewin);
   ph = EoGetH(ewin);

   for (i = 0; i < ewin->border->num_winparts; i++)
      ewin->bits[i].w = -2;
   for (i = 0; i < ewin->border->num_winparts; i++)
      if (ewin->bits[i].w == -2)
	 BorderWinpartCalc(ewin, i);

   switch (ewin->border->shadedir)
     {
     case 0:
     case 1:
	/* get the correct width, based on the borderparts that */
	/*are remaining visible */
	leftborderwidth = rightborderwidth = 0;
	for (i = 0; i < ewin->border->num_winparts; i++)
	  {
	     if (!ewin->border->part[i].keep_for_shade)
		continue;

	     w = ewin->border->border.left - ewin->bits[i].x;
	     if (leftborderwidth < w)
		leftborderwidth = w;

	     w = ewin->bits[i].x + ewin->bits[i].w -
		(EoGetW(ewin) - ewin->border->border.right);
	     if (rightborderwidth < w)
		rightborderwidth = w;
	  }
	EoSetW(ewin, rightborderwidth + leftborderwidth);
	break;
     case 2:
     case 3:
	topborderwidth = bottomborderwidth = 0;
	for (i = 0; i < ewin->border->num_winparts; i++)
	  {
	     if (!ewin->border->part[i].keep_for_shade)
		continue;

	     h = ewin->border->border.top - ewin->bits[i].y;
	     if (topborderwidth < h)
		topborderwidth = h;

	     h = ewin->bits[i].y + ewin->bits[i].h -
		(EoGetH(ewin) - ewin->border->border.bottom);
	     if (bottomborderwidth < h)
		bottomborderwidth = h;
	  }
	EoSetH(ewin, bottomborderwidth + topborderwidth);
	break;
     default:
	break;
     }

   for (i = 0; i < ewin->border->num_winparts; i++)
      ewin->bits[i].w = -2;
   for (i = 0; i < ewin->border->num_winparts; i++)
      if (ewin->bits[i].w == -2)
	 BorderWinpartCalc(ewin, i);

   min_w = 0;
   min_h = 0;
   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	if (!ewin->border->part[i].keep_for_shade)
	   continue;

	w = ewin->bits[i].x + ewin->bits[i].w;
	if (min_w < w)
	   min_w = w;

	h = ewin->bits[i].y + ewin->bits[i].h;
	if (min_h < h)
	   min_h = h;
     }

   EoSetW(ewin, pw);
   EoSetH(ewin, ph);

 done:
   *mw = min_w;
   *mh = min_h;
}

int
BorderWinpartIndex(EWin * ewin, Window win)
{
   int                 i;

   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	if (win == ewin->bits[i].win)
	   return i;
     }

   return -1;			/* Not found */
}

void
EwinBorderEventsConfigure(EWin * ewin, int mode)
{
   int                 i;
   long                emask;

   emask = (mode) ? ~((long)0) : ~(EnterWindowMask | LeaveWindowMask);

   for (i = 0; i < ewin->border->num_winparts; i++)
     {
	if (ewin->border->part[i].flags & FLAG_TITLE)
	   XSelectInput(disp, ewin->bits[i].win,
			EWIN_BORDER_TITLE_EVENT_MASK & emask);
	else
	   XSelectInput(disp, ewin->bits[i].win,
			EWIN_BORDER_PART_EVENT_MASK & emask);
     }
}

/*
 * Border event handlers
 */
#define DEBUG_BORDER_EVENTS 0

static void
BorderWinpartEventExpose(EWinBit * wbit, XEvent * ev __UNUSED__)
{
   EWin               *ewin = wbit->ewin;
   int                 part = wbit - ewin->bits;

   wbit->no_expose = 0;
   wbit->expose = 1;
   if (BorderWinpartDraw(ewin, part) && IsPropagateEwinOnQueue(ewin))
      EwinPropagateShapes(ewin);
}

static void
BorderWinpartEventMouseDown(EWinBit * wbit, XEvent * ev)
{
   EWin               *ewin = wbit->ewin;
   int                 part = wbit - ewin->bits;

   GrabPointerSet(wbit->win, 0, 0);

   wbit->state = STATE_CLICKED;
#if DEBUG_BORDER_EVENTS
   Eprintf("BorderWinpartEventMouseDown %#lx %d\n", wbit->win, wbit->state);
#endif
   BorderWinpartChange(ewin, part, 0);

   if (ewin->border->part[part].aclass)
      EventAclass(ev, ewin, ewin->border->part[part].aclass);

   FocusHandleClick(ewin, wbit->win);
}

static void
BorderWinpartEventMouseUp(EWinBit * wbit, XEvent * ev)
{
   EWin               *ewin = wbit->ewin;
   int                 part = wbit - ewin->bits;

   GrabPointerRelease();

   if ((wbit->state == STATE_CLICKED) && (!wbit->left))
      wbit->state = STATE_HILITED;
   else
      wbit->state = STATE_NORMAL;
#if DEBUG_BORDER_EVENTS
   Eprintf("BorderWinpartEventMouseUp %#lx %d\n", wbit->win, wbit->state);
#endif
   BorderWinpartChange(ewin, part, 0);

   if (wbit->win == Mode.last_bpress && !wbit->left &&
       ewin->border->part[part].aclass)
      EventAclass(ev, ewin, ewin->border->part[part].aclass);

   wbit->left = 0;
}

static void
BorderWinpartEventEnter(EWinBit * wbit, XEvent * ev)
{
   EWin               *ewin = wbit->ewin;
   int                 part = wbit - ewin->bits;

#if DEBUG_BORDER_EVENTS
   Eprintf("BorderWinpartEventEnter %#lx %d\n", wbit->win, wbit->state);
#endif
   if (wbit->state == STATE_CLICKED)
      wbit->left = 0;
#if 0				/* Hmmm.. */
   else
#endif
     {
	wbit->state = STATE_HILITED;
	BorderWinpartChange(ewin, part, 0);
	if (ewin->border->part[part].aclass)
	   EventAclass(ev, ewin, ewin->border->part[part].aclass);
     }
}

static void
BorderWinpartEventLeave(EWinBit * wbit, XEvent * ev)
{
   EWin               *ewin = wbit->ewin;
   int                 part = wbit - ewin->bits;

#if DEBUG_BORDER_EVENTS
   Eprintf("BorderWinpartEventLeave %#lx %d\n", wbit->win, wbit->state);
#endif
   if (wbit->state == STATE_CLICKED)
      wbit->left = 1;
   else
     {
	wbit->state = STATE_NORMAL;
	BorderWinpartChange(ewin, part, 0);
	if (ewin->border->part[part].aclass)
	   EventAclass(ev, ewin, ewin->border->part[part].aclass);
     }
}

#if 0				/* FIXME */
/* This was in HandleMouseUp() */

{

#if 0
   if ((Mode.last_bpress) && (Mode.last_bpress != win))
     {
	ev->xbutton.window = Mode.last_bpress;
	BordersEventMouseOut2(ev);
	ev->xbutton.window = win;
     }
#endif
}

static void
BorderWinpartEventLeave2(EWinBit * wbit, XEvent * ev, EWin * ewin, int j)
{
   ewin->bits[j].left = 0;
   ewin->bits[j].state = STATE_NORMAL;
   BorderWinpartChange(ewin, j, 0);
   return;
   ev = NULL;
}
#endif

static void
BorderFrameHandleEvents(XEvent * ev, void *prm)
{
   EWin               *ewin = (EWin *) prm;

   switch (ev->type)
     {
     case EnterNotify:
     case LeaveNotify:
	if (ewin->border->aclass)
	   EventAclass(ev, ewin, ewin->border->aclass);
	break;
     }
}

static void
BorderWinpartHandleEvents(XEvent * ev, void *prm)
{
   EWinBit            *wbit = (EWinBit *) prm;

   switch (ev->type)
     {
#if 0
     case KeyPress:
	break;
#endif
     case ButtonPress:
	BorderWinpartEventMouseDown(wbit, ev);
	break;
     case ButtonRelease:
	BorderWinpartEventMouseUp(wbit, ev);
	break;
     case EnterNotify:
	BorderWinpartEventEnter(wbit, ev);
	break;
     case LeaveNotify:
	BorderWinpartEventLeave(wbit, ev);
	break;
     case Expose:
	BorderWinpartEventExpose(wbit, ev);
	break;
     }
}

/*
 * Configuration load/save
 */
#include "conf.h"

static int
BorderPartLoad(FILE * fs, char type, Border * b)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   ImageClass         *iclass = 0;
   ActionClass        *aclass = 0;
   TextClass          *tclass = 0;
   ECursor            *ec = NULL;
   char                ontop = 1;
   int                 flags = FLAG_BUTTON;
   char                isregion = 0, keepshade = 1;
   int                 wmin = 0, wmax = 0, hmin = 0, hmax = 0, torigin =
      0, txp = 0, txa = 0, typ = 0, tya = 0, borigin = 0;
   int                 bxp = 0, bxa = 0, byp = 0, bya = 0;
   int                 fields;

   type = 0;
   while (GetLine(s, sizeof(s), fs))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
		  i1 = CONFIG_INVALID;
	       }
	  }
	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     BorderWinpartAdd(b, iclass, aclass, tclass, ec, ontop, flags,
			      isregion, wmin, wmax, hmin, hmax, torigin, txp,
			      txa, typ, tya, borigin, bxp, bxa, byp, bya,
			      keepshade);
	     goto done;
	  case CONFIG_IMAGECLASS:
	  case BORDERPART_ICLASS:
	     iclass = ImageclassFind(s2, 1);
	     break;
	  case CONFIG_ACTIONCLASS:
	  case BORDERPART_ACLASS:
	     aclass = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ACLASS);
	     break;
	  case CONFIG_TEXT:
	  case BORDERPART_TEXTCLASS:
	     tclass = TextclassFind(s2, 1);
	     break;
	  case CONFIG_CURSOR:
	     ec = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ECURSOR);
	     break;
	  case BORDERPART_ONTOP:
	     ontop = atoi(s2);
	     break;
	  case BORDERPART_FLAGS:
	     flags = atoi(s2);
	     break;
	  case BORDERPART_ISREGION:
	     isregion = atoi(s2);
	     break;
	  case BORDERPART_WMIN:
	     wmin = atoi(s2);
	     if (!wmax)
		wmax = wmin;
	     break;
	  case BORDERPART_WMAX:
	     wmax = atoi(s2);
	     break;
	  case BORDERPART_HMIN:
	     hmin = atoi(s2);
	     if (!hmax)
		hmax = hmin;
	     break;
	  case BORDERPART_HMAX:
	     hmax = atoi(s2);
	     break;
	  case BORDERPART_TORIGIN:
	     torigin = atoi(s2);
	     break;
	  case BORDERPART_TXP:
	     txp = atoi(s2);
	     break;
	  case BORDERPART_TXA:
	     txa = atoi(s2);
	     break;
	  case BORDERPART_TYP:
	     typ = atoi(s2);
	     break;
	  case BORDERPART_TYA:
	     tya = atoi(s2);
	     break;
	  case BORDERPART_BORIGIN:
	     borigin = atoi(s2);
	     break;
	  case BORDERPART_BXP:
	     bxp = atoi(s2);
	     break;
	  case BORDERPART_BXA:
	     bxa = atoi(s2);
	     break;
	  case BORDERPART_BYP:
	     byp = atoi(s2);
	     break;
	  case BORDERPART_BYA:
	     bya = atoi(s2);
	     break;
	  case BORDERPART_KEEPSHADE:
	     keepshade = atoi(s2);
	     break;
	  default:
	     break;
	  }
     }
   err = -1;
 done:
   return err;
}

int
BorderConfigLoad(FILE * fs)
{
   int                 err = 0;
   Border             *b = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   char                added = 0;
   int                 fields;

   while (GetLine(s, sizeof(s), fs))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	  {
	     i1 = CONFIG_INVALID;
	  }
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }
	else if (i1 != CONFIG_INVALID)
	  {
	     if (fields != 2)
	       {
		  Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
		  i1 = CONFIG_INVALID;
	       }
	  }
	if (atoi(s2) == CONFIG_OPEN)
	  {
	     err = BorderPartLoad(fs, i1, b);
	     if (err)
		break;
	  }
	else
	  {
	     switch (i1)
	       {
	       case CONFIG_CLOSE:
		  if (!added)
		     AddItem(b, b->name, 0, LIST_TYPE_BORDER);
		  goto done;
	       case BORDER_INIT:
		  AddItem(b, b->name, 0, LIST_TYPE_BORDER);
		  added = 1;
		  break;
	       case CONFIG_CLASSNAME:
	       case BORDER_NAME:
		  if (ConfigSkipIfExists(fs, s2, LIST_TYPE_BORDER))
		     goto done;
		  b = BorderCreate(s2);
		  break;
	       case BORDER_GROUP_NAME:
		  b->group_border_name = Estrdup(s2);
		  break;
	       case BORDER_LEFT:
		  b->border.left = atoi(s2);
		  break;
	       case BORDER_RIGHT:
		  b->border.right = atoi(s2);
		  break;
	       case BORDER_TOP:
		  b->border.top = atoi(s2);
		  break;
	       case BORDER_BOTTOM:
		  b->border.bottom = atoi(s2);
		  break;
	       case SHADEDIR:
		  b->shadedir = atoi(s2);
		  break;
	       case BORDER_CHANGES_SHAPE:
		  b->changes_shape = atoi(s2);
		  break;
	       case CONFIG_ACTIONCLASS:
		  b->aclass =
		     FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ACLASS);
		  break;
	       default:
		  break;
	       }
	  }
     }
   err = -1;

 done:
   return err;
}

void
BordersSetupFallback(void)
{
   /*
    * This function creates simple internal data members to be used in 
    * emergencies - ie when all else fails - ie a button is told to use an
    * imageclass that doesn't exist, or no DEFAULT border is defined... at 
    * least E won't barf on us then.
    */
   Border             *b;
   ImageClass         *ic;
   ActionClass        *ac;

   ac = FindItem("__FALLBACK_ACTION", 0, LIST_FINDBY_NAME, LIST_TYPE_ACLASS);
   ic = FindItem("__FALLBACK_ICLASS", 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);

   /* create a fallback border in case no border is found */
   b = BorderCreate("__FALLBACK_BORDER");
   AddItem(b, b->name, 0, LIST_TYPE_BORDER);

   b->border.left = 8;
   b->border.right = 8;
   b->border.top = 8;
   b->border.bottom = 8;
   BorderWinpartAdd(b, ic, ac, NULL, NULL, 1, FLAG_BUTTON, 0, 8, 99999, 8,
		    99999, -1, 0, 0, 0, 0, -1, 1024, -1, 0, 7, 1);
   BorderWinpartAdd(b, ic, ac, NULL, NULL, 1, FLAG_BUTTON, 0, 8, 99999, 8,
		    99999, -1, 0, 0, 1024, -8, -1, 1024, -1, 1024, -1, 1);
   BorderWinpartAdd(b, ic, ac, NULL, NULL, 1, FLAG_BUTTON, 0, 8, 99999, 8,
		    99999, -1, 0, 0, 0, 8, -1, 0, 7, 1024, -9, 1);

   BorderWinpartAdd(b, ic, ac, NULL, NULL, 1, FLAG_BUTTON, 0, 8, 99999, 8,
		    99999, -1, 1024, -8, 0, 8, -1, 1024, -1, 1024, -9, 1);
}
