/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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
#include <math.h>

typedef enum
{
   EWIN_ICON_TYPE_APP,
   EWIN_ICON_TYPE_IMG,
   EWIN_ICON_TYPE_SNAP
} ewin_icon_e;

typedef struct _iconbox Iconbox;

static void         IconboxesConfigSave(void);
static void         UpdateAppIcon(EWin * ewin, int imode);
static Iconbox     *SelectIconboxForEwin(EWin * ewin);

/* Systray stuff */
static void         SystrayInit(Iconbox * obj, Window win, int screen);

typedef union
{
   void               *obj;
   EWin               *ewin;
   Window              win;
} IboxOject;

struct _iconbox
{
   /* user settings */
   char               *name;
   char                type;
   char                orientation;
   char                animate;
   char                scrollbar_side;
   char                arrow_side;
   char                shownames;
   char                nobg;
   int                 iconsize;
   int                 icon_mode;

   char                auto_resize;
   char                draw_icon_base;
   char                scrollbar_hide;
   char                cover_hide;
   int                 auto_resize_anchor;

   /* internally set stuff */
   int                 w, h;
   int                 pos;
   int                 max, max_min;
   char                force_update;
   char                arrow1_hilited;
   char                arrow1_clicked;
   char                arrow2_hilited;
   char                arrow2_clicked;
   char                icon_clicked;
   char                scrollbar_hilited;
   char                scrollbar_clicked;
   char                scrollbox_clicked;

   Pixmap              pmap, mask;

   Window              win;
   Window              cover_win;
   Window              icon_win;
   Window              scroll_win;
   Window              arrow1_win;
   Window              arrow2_win;
   Window              scrollbar_win;
   Window              scrollbarknob_win;
   EWin               *ewin;

   int                 num_objs;
   IboxOject          *objs;

   /* these are theme-settable parameters */
   int                 scroll_thickness;
   int                 arrow_thickness;
   int                 bar_thickness;
   int                 knob_length;

};

typedef struct
{
   char               *title_match;
   char               *name_match;
   char               *class_match;
   char               *icon_file;
} Icondef;

/* Silly hack to avoid name clash warning when using -Wshadow */
#define y1 y1_

static void         IconboxRedraw(Iconbox * ib);
static void         IboxEventScrollWin(XEvent * ev, void *prm);
static void         IboxEventScrollbarWin(XEvent * ev, void *prm);
static void         IboxEventCoverWin(XEvent * ev, void *prm);
static void         IboxEventArrow1Win(XEvent * ev, void *prm);
static void         IboxEventArrow2Win(XEvent * ev, void *prm);
static void         IboxEventIconWin(XEvent * ev, void *prm);

#define IB_ANIM_TIME 0.25

static void
IB_Animate(char iconify, EWin * from, EWin * to)
{
   double              t1, t2, t, i, spd, ii;
   int                 x, y, x1, y1, x2, y2, x3, y3, x4, y4, w, h, fx, fy, fw,
      fh, dx, dy, dw, dh;
   GC                  gc;
   XGCValues           gcv;

   if (Mode.wm.startup)
      return;

   ecore_x_grab();
   spd = 0.00001;
   gcv.subwindow_mode = IncludeInferiors;
   gcv.function = GXxor;
   gcv.fill_style = FillOpaqueStippled;
   gcv.foreground = WhitePixel(disp, VRoot.scr);
   if (gcv.foreground == 0)
      gcv.foreground = BlackPixel(disp, VRoot.scr);
   gc = XCreateGC(disp, VRoot.win,
		  GCFunction | GCForeground | GCSubwindowMode | GCFillStyle,
		  &gcv);
   t1 = GetTime();
   if (iconify)
     {
	fw = EoGetW(from) + 4;
	fh = EoGetH(from) + 4;
	fx = EoGetX(from) + DeskGetX(EoGetDesk(from)) - 2;
	fy = EoGetY(from) + DeskGetY(EoGetDesk(from)) - 2;
	dw = 4;
	dh = 4;
	dx = EoGetX(to) + DeskGetX(EoGetDesk(to)) + (EoGetW(to) / 2) - 2;
	dy = EoGetY(to) + DeskGetY(EoGetDesk(to)) + (EoGetH(to) / 2) - 2;
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

	     XDrawLine(disp, VRoot.win, gc, x1, y1, x2, y2);
	     XDrawLine(disp, VRoot.win, gc, x2, y2, x3, y3);
	     XDrawLine(disp, VRoot.win, gc, x3, y3, x4, y4);
	     XDrawLine(disp, VRoot.win, gc, x4, y4, x1, y1);

	     XDrawLine(disp, VRoot.win, gc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	     XDrawLine(disp, VRoot.win, gc, x2 + 1, y2 + 1, x3 - 1, y3 - 1);
	     XDrawLine(disp, VRoot.win, gc, x3 + 1, y3 + 1, x4 - 1, y4 - 1);
	     XDrawLine(disp, VRoot.win, gc, x4 + 1, y4 + 1, x1 - 1, y1 - 1);

	     XDrawLine(disp, VRoot.win, gc, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	     XDrawLine(disp, VRoot.win, gc, x2 + 2, y2 + 2, x3 - 2, y3 - 2);
	     XDrawLine(disp, VRoot.win, gc, x3 + 2, y3 + 2, x4 - 2, y4 - 2);
	     XDrawLine(disp, VRoot.win, gc, x4 + 2, y4 + 2, x1 - 2, y1 - 2);

	     ecore_x_sync();
	     t2 = GetTime();
	     t = t2 - t1;
	     t1 = t2;
	     spd = t / IB_ANIM_TIME;

	     XDrawLine(disp, VRoot.win, gc, x1, y1, x2, y2);
	     XDrawLine(disp, VRoot.win, gc, x2, y2, x3, y3);
	     XDrawLine(disp, VRoot.win, gc, x3, y3, x4, y4);
	     XDrawLine(disp, VRoot.win, gc, x4, y4, x1, y1);

	     XDrawLine(disp, VRoot.win, gc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	     XDrawLine(disp, VRoot.win, gc, x2 + 1, y2 + 1, x3 - 1, y3 - 1);
	     XDrawLine(disp, VRoot.win, gc, x3 + 1, y3 + 1, x4 - 1, y4 - 1);
	     XDrawLine(disp, VRoot.win, gc, x4 + 1, y4 + 1, x1 - 1, y1 - 1);

	     XDrawLine(disp, VRoot.win, gc, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	     XDrawLine(disp, VRoot.win, gc, x2 + 2, y2 + 2, x3 - 2, y3 - 2);
	     XDrawLine(disp, VRoot.win, gc, x3 + 2, y3 + 2, x4 - 2, y4 - 2);
	     XDrawLine(disp, VRoot.win, gc, x4 + 2, y4 + 2, x1 - 2, y1 - 2);
	  }
     }
   else
     {
	fw = EoGetW(from) + 4;
	fh = EoGetH(from) + 4;
	fx = EoGetX(from) + DeskGetX(EoGetDesk(from)) - 2;
	fy = EoGetY(from) + DeskGetY(EoGetDesk(from)) - 2;
	dw = 4;
	dh = 4;
	dx = EoGetX(to) + DeskGetX(EoGetDesk(to)) + (EoGetW(to) / 2) - 2;
	dy = EoGetY(to) + DeskGetY(EoGetDesk(to)) + (EoGetH(to) / 2) - 2;
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

	     XDrawLine(disp, VRoot.win, gc, x1, y1, x2, y2);
	     XDrawLine(disp, VRoot.win, gc, x2, y2, x3, y3);
	     XDrawLine(disp, VRoot.win, gc, x3, y3, x4, y4);
	     XDrawLine(disp, VRoot.win, gc, x4, y4, x1, y1);

	     XDrawLine(disp, VRoot.win, gc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	     XDrawLine(disp, VRoot.win, gc, x2 + 1, y2 + 1, x3 - 1, y3 - 1);
	     XDrawLine(disp, VRoot.win, gc, x3 + 1, y3 + 1, x4 - 1, y4 - 1);
	     XDrawLine(disp, VRoot.win, gc, x4 + 1, y4 + 1, x1 - 1, y1 - 1);

	     XDrawLine(disp, VRoot.win, gc, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	     XDrawLine(disp, VRoot.win, gc, x2 + 2, y2 + 2, x3 - 2, y3 - 2);
	     XDrawLine(disp, VRoot.win, gc, x3 + 2, y3 + 2, x4 - 2, y4 - 2);
	     XDrawLine(disp, VRoot.win, gc, x4 + 2, y4 + 2, x1 - 2, y1 - 2);

	     ecore_x_sync();
	     t2 = GetTime();
	     t = t2 - t1;
	     t1 = t2;
	     spd = t / IB_ANIM_TIME;

	     XDrawLine(disp, VRoot.win, gc, x1, y1, x2, y2);
	     XDrawLine(disp, VRoot.win, gc, x2, y2, x3, y3);
	     XDrawLine(disp, VRoot.win, gc, x3, y3, x4, y4);
	     XDrawLine(disp, VRoot.win, gc, x4, y4, x1, y1);

	     XDrawLine(disp, VRoot.win, gc, x1 + 1, y1 + 1, x2 - 1, y2 - 1);
	     XDrawLine(disp, VRoot.win, gc, x2 + 1, y2 + 1, x3 - 1, y3 - 1);
	     XDrawLine(disp, VRoot.win, gc, x3 + 1, y3 + 1, x4 - 1, y4 - 1);
	     XDrawLine(disp, VRoot.win, gc, x4 + 1, y4 + 1, x1 - 1, y1 - 1);

	     XDrawLine(disp, VRoot.win, gc, x1 + 2, y1 + 2, x2 - 2, y2 - 2);
	     XDrawLine(disp, VRoot.win, gc, x2 + 2, y2 + 2, x3 - 2, y3 - 2);
	     XDrawLine(disp, VRoot.win, gc, x3 + 2, y3 + 2, x4 - 2, y4 - 2);
	     XDrawLine(disp, VRoot.win, gc, x4 + 2, y4 + 2, x1 - 2, y1 - 2);
	  }
     }
   XFreeGC(disp, gc);
   ecore_x_ungrab();
}

static Iconbox     *
IconboxCreate(const char *name)
{
   Iconbox            *ib;

   ib = Emalloc(sizeof(Iconbox));
   ib->name = Estrdup(name);
   ib->type = (!strncmp(name, "_ST_", 4)) ? 1 : 0;	/* Type 1 is systray */
   ib->orientation = 0;
   ib->scrollbar_side = 1;
   ib->arrow_side = 1;
   ib->nobg = 0;
   ib->shownames = 1;
   ib->iconsize = (ib->type == 0) ? 48 : 24;
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
   ib->win = ECreateWindow(VRoot.win, 0, 0, 128, 32, 0);
   ib->icon_win = ECreateWindow(ib->win, 0, 0, 128, 26, 0);
   EventCallbackRegister(ib->icon_win, 0, IboxEventIconWin, ib);
   ib->cover_win = ECreateWindow(ib->win, 0, 0, 128, 26, 0);
   EventCallbackRegister(ib->cover_win, 0, IboxEventCoverWin, ib);
   ib->scroll_win = ECreateWindow(ib->win, 6, 26, 116, 6, 0);
   EventCallbackRegister(ib->scroll_win, 0, IboxEventScrollWin, ib);
   ib->arrow1_win = ECreateWindow(ib->win, 0, 26, 6, 6, 0);
   EventCallbackRegister(ib->arrow1_win, 0, IboxEventArrow1Win, ib);
   ib->arrow2_win = ECreateWindow(ib->win, 122, 26, 6, 6, 0);
   EventCallbackRegister(ib->arrow2_win, 0, IboxEventArrow2Win, ib);
   ib->scrollbar_win = ECreateWindow(ib->scroll_win, 122, 26, 6, 6, 0);
   EventCallbackRegister(ib->scrollbar_win, 0, IboxEventScrollbarWin, ib);
   ib->scrollbarknob_win = ECreateWindow(ib->scrollbar_win, -20, -20, 4, 4, 0);
   ib->pmap = ecore_x_pixmap_new(ib->icon_win, 128, 32, VRoot.depth);

   ESelectInput(ib->icon_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask | PointerMotionMask);
   ESelectInput(ib->scroll_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask);
   ESelectInput(ib->cover_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask);
   ESelectInput(ib->arrow1_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask);
   ESelectInput(ib->arrow2_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask);
   ESelectInput(ib->scrollbar_win,
		EnterWindowMask | LeaveWindowMask | ButtonPressMask |
		ButtonReleaseMask | PointerMotionMask);

   EMapWindow(ib->icon_win);
   EMapWindow(ib->scroll_win);
   EMapWindow(ib->arrow1_win);
   EMapWindow(ib->arrow2_win);
   EMapWindow(ib->scrollbar_win);
   EMapWindow(ib->scrollbarknob_win);

   ib->ewin = NULL;
   ib->num_objs = 0;
   ib->objs = NULL;

   AddItem(ib, ib->name, 0, LIST_TYPE_ICONBOX);

   if (ib->type == 1)
      SystrayInit(ib, ib->icon_win, VRoot.scr);

   return ib;
}

static void
IconboxDestroy(Iconbox * ib)
{
   int                 i;

   RemoveItem((char *)ib, 0, LIST_FINDBY_POINTER, LIST_TYPE_ICONBOX);

   if (ib->name)
      Efree(ib->name);

   for (i = 0; i < ib->num_objs; i++)
     {
	switch (ib->type)
	  {
	  case 0:
	     EwinDeIconify(ib->objs[i].ewin);
	     break;
#if 0
	  case 1:
	     IconboxWindowFree(ib, ib->objs[i].win);
	     break;
#endif
	  }
     }

   if (ib->objs)
      Efree(ib->objs);

   if (ib->pmap)
      ecore_x_pixmap_del(ib->pmap);

   EDestroyWindow(ib->win);

   Efree(ib);
   IconboxesConfigSave();
}

#if 0				/* Not used */
static              Window
IconboxGetWin(Iconbox * ib)
{
   return ib->win;
}
#endif

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
	ic = ImageclassFind("ICONBOX_VERTICAL", 0);
	if (ic)
	   extra = ic->padding.left + ic->padding.right;
	ewin->client.width.max = ewin->client.width.min =
	   ib->iconsize + ib->scroll_thickness + extra;
	ewin->client.no_resize_h = 1;
	ib->max_min = ewin->client.height.min;
     }
   else
     {
	ic = ImageclassFind("ICONBOX_HORIZONTAL", 0);
	if (ic)
	   extra = ic->padding.left + ic->padding.right;
	ewin->client.height.max = ewin->client.height.min =
	   ib->iconsize + ib->scroll_thickness + extra;
	ewin->client.no_resize_v = 1;
	ib->max_min = ewin->client.width.min;
     }

   ICCCM_MatchSize(ewin);
}

static void
IconboxEwinMoveResize(EWin * ewin, int resize __UNUSED__)
{
   static int          call_depth = 0;	/* Ugly! */
   Iconbox            *ib = ewin->data;

   if (!ib || call_depth > 0)
      return;
   call_depth++;

   RememberImportantInfoForEwin(ewin);

   if (!TransparencyEnabled() &&
       ib->w == ewin->client.w && ib->h == ewin->client.h)
      goto done;

   ib->w = ewin->client.w;
   ib->h = ewin->client.h;
   ib->force_update = 1;
   IconboxRedraw(ib);

 done:
   call_depth--;
}

static void
IconboxEwinRefresh(EWin * ewin)
{
   Iconbox            *ib = ewin->data;

   if (!ib)
      return;

   if (!TransparencyEnabled())
      return;

   ib->force_update = 1;
   IconboxRedraw(ib);
}

static void
IconboxEwinClose(EWin * ewin)
{
   IconboxDestroy(ewin->data);
   ewin->data = NULL;
}

static void
IconboxEwinInit(EWin * ewin, void *ptr)
{
   ewin->data = (Iconbox *) ptr;
   ewin->MoveResize = IconboxEwinMoveResize;
   ewin->Refresh = IconboxEwinRefresh;
   ewin->Close = IconboxEwinClose;

   ewin->props.inhibit_iconify = 1;
   ewin->props.autosave = 1;
}

static void
IconboxShow(Iconbox * ib)
{
   EWin               *ewin = NULL;
   char                pq;

   pq = Mode.queue_up;
   Mode.queue_up = 0;

   HintsSetWindowName(ib->win, "Iconbox");
   HintsSetWindowClass(ib->win, ib->name, "Enlightenment_IconBox");

   ewin = AddInternalToFamily(ib->win, "ICONBOX", EWIN_TYPE_ICONBOX, ib,
			      IconboxEwinInit);
   if (ewin)
     {
	int                 w, h;

	ib->ewin = ewin;

	IB_Reconfigure(ib);

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
	     EwinStick(ewin);
	     ResizeEwin(ewin, w, h);
	     MoveEwin(ewin, VRoot.w - EoGetW(ewin), VRoot.h - EoGetH(ewin));
	  }

	ShowEwin(ewin);
     }
   Mode.queue_up = pq;
}

#if 0
static void
IconboxHide(Iconbox * ib)
{
   if (ib->ewin)
      HideEwin(ib->ewin);
}
#endif

/*
 * Return index, -1 if not found.
 */
static int
IconboxObjectFind(Iconbox * ib, void *obj)
{
   int                 i;

   for (i = 0; i < ib->num_objs; i++)
      if (ib->objs[i].obj == obj)
	 return i;

   return -1;
}

static int
IconboxObjectAdd(Iconbox * ib, void *obj)
{
   /* Not if already there */
   if (IconboxObjectFind(ib, obj) >= 0)
      return -1;

   ib->num_objs++;
   ib->objs = Erealloc(ib->objs, sizeof(EWin *) * ib->num_objs);
   ib->objs[ib->num_objs - 1].obj = obj;

   return 0;			/* Success */
}

static int
IconboxObjectDel(Iconbox * ib, void *obj)
{
   int                 i, j;

   /* Quit if not there */
   i = IconboxObjectFind(ib, obj);
   if (i < 0)
      return -1;

   for (j = i; j < ib->num_objs - 1; j++)
      ib->objs[j] = ib->objs[j + 1];
   ib->num_objs--;
   if (ib->num_objs > 0)
      ib->objs = Erealloc(ib->objs, sizeof(EWin *) * ib->num_objs);
   else
     {
	Efree(ib->objs);
	ib->objs = NULL;
     }

   return 0;			/* Success */
}

static int
IconboxFindEwin(Iconbox * ib, EWin * ewin)
{
   return IconboxObjectFind(ib, ewin);
}

static void
IconboxAddEwin(Iconbox * ib, EWin * ewin)
{
   if (IconboxObjectAdd(ib, ewin) == 0)
      IconboxRedraw(ib);
}

static void
IconboxDelEwin(Iconbox * ib, EWin * ewin)
{
   if (IconboxObjectDel(ib, ewin) == 0)
      IconboxRedraw(ib);
}

#if 0
static int
IconboxFindWindow(Iconbox * ib, Window win)
{
   return IconboxObjectFind(ib, (void *)win);
}
#endif

static int
IconboxWindowAdd(Iconbox * ib, Window win)
{
   if (IconboxObjectAdd(ib, (void *)win))
      return -1;

   XReparentWindow(disp, win, ib->icon_win, 0, 0);
   IconboxRedraw(ib);
   XMapWindow(disp, win);

   return 0;
}

static void
IconboxWindowDel(Iconbox * ib, Window win)
{
   if (IconboxObjectDel(ib, (void *)win) == 0)
      IconboxRedraw(ib);
}

#if 0
static void
IconboxWindowFree(Iconbox * ib __UNUSED__, Window win)
{
   Eprintf("IconboxWindowFree %#lx\n", win);
   if (disp)
     {
	EUnmapWindow(disp, win);
	EReparentWindow(disp, win, VRoot.win, 0, 0);
     }
}
#endif

static void
IconboxesEwinIconify(EWin * ewin)
{
   Iconbox            *ib;

   SoundPlay("SOUND_ICONIFY");

   ib = SelectIconboxForEwin(ewin);
   if (ib)
     {
	if (ib->animate && !ewin->st.showingdesk)
	   IB_Animate(1, ewin, ib->ewin);
	UpdateAppIcon(ewin, ib->icon_mode);
	IconboxAddEwin(ib, ewin);
     }
}

static void
IconboxesEwinDeIconify(EWin * ewin)
{
   Iconbox            *ib;

   SoundPlay("SOUND_DEICONIFY");

   ib = SelectIconboxForEwin(ewin);
   if (ib && ib->animate && !ewin->st.showingdesk)
      IB_Animate(0, ewin, ib->ewin);
}

static void
RemoveMiniIcon(EWin * ewin)
{
   Iconbox            *ib;

   ib = SelectIconboxForEwin(ewin);
   if (ib)
      IconboxDelEwin(ib, ewin);
}

static void
IB_SnapEWin(EWin * ewin)
{
   /* Make snapshot of window */
   int                 w, h, scale;
   Iconbox            *ib;
   Imlib_Image        *im;
   Drawable            draw;

   if (!EwinIsMapped(ewin))
      return;

   w = 40;
   h = 40;
   ib = SelectIconboxForEwin(ewin);
   if (ib)
     {
	w = ib->iconsize;
	h = ib->iconsize;
     }

   /* Oversample for nicer snapshots */
   scale = 4;
   w *= scale;
   h *= scale;

   if (EoGetW(ewin) > EoGetH(ewin))
      h = (w * EoGetH(ewin)) / EoGetW(ewin);
   else
      w = (h * EoGetW(ewin)) / EoGetH(ewin);
   if (w < 4)
      w = 4;
   if (h < 4)
      h = 4;
   if (w > EoGetW(ewin) || h > EoGetH(ewin))
     {
	w = EoGetW(ewin);
	h = EoGetH(ewin);
     }

#if USE_COMPOSITE
   draw = EoGetPixmap(ewin);
   if (draw != None)
     {
	Pixmap              mask;

	mask = EWindowGetShapePixmap(EoGetWin(ewin));
	imlib_context_set_drawable(draw);
	im = imlib_create_scaled_image_from_drawable(mask, 0, 0, EoGetW(ewin),
						     EoGetH(ewin), w, h, 1, 0);
	ecore_x_pixmap_del(ib->pmap);
	imlib_context_set_image(im);
	imlib_image_set_has_alpha(1);	/* Should be set by imlib? */
     }
   else
#endif
     {
	draw = EoGetWin(ewin);
	imlib_context_set_drawable(draw);
	im = imlib_create_scaled_image_from_drawable(None, 0, 0, EoGetW(ewin),
						     EoGetH(ewin), w, h, 1, 1);
	imlib_context_set_image(im);
	imlib_image_set_has_alpha(1);	/* Should be set by imlib? */
     }
   ewin->icon_image = im;
   ewin->icon_type = EWIN_ICON_TYPE_SNAP;
}

static void
IB_GetAppIcon(EWin * ewin)
{
   /* Get the applications icon pixmap/mask */
   int                 x, y;
   unsigned int        w, h, depth, bw;
   Window              rt;
   Imlib_Image         im;

   if (!ewin->client.icon_pmap)
      return;

   w = 0;
   h = 0;
   XGetGeometry(disp, ewin->client.icon_pmap, &rt, &x, &y, &w, &h, &bw, &depth);

   if (w < 1 || h < 1)
      return;

   imlib_context_set_colormap(None);
   imlib_context_set_drawable(ewin->client.icon_pmap);
   im = imlib_create_image_from_drawable(ewin->client.icon_mask, 0, 0, w, h, 1);
   imlib_context_set_image(im);
   imlib_image_set_has_alpha(1);	/* Should be set by imlib? */
   imlib_context_set_colormap(VRoot.cmap);
   imlib_context_set_drawable(VRoot.win);
   ewin->icon_image = im;
   ewin->icon_type = EWIN_ICON_TYPE_APP;
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
   Imlib_Image        *im;

   idef = IB_MatchIcondef(ewin->icccm.wm_name, ewin->icccm.wm_res_name,
			  ewin->icccm.wm_res_class);

   if (!idef)
      return;

   im = ELoadImage(idef->icon_file);
   if (!im)
      return;

   ewin->icon_image = im;
   ewin->icon_type = EWIN_ICON_TYPE_IMG;
}

static void
IB_AddIcondef(char *title, char *name, char *class, char *file)
{
   /* add match for a window pointing to an iconfile */
   /* form: "*term*" "name*" "*class" "path/to_image.png" */
   Icondef            *idef;

   idef = Emalloc(sizeof(Icondef));
   if (!idef)
      return;
   idef->title_match = Estrdup(title);
   idef->name_match = Estrdup(name);
   idef->class_match = Estrdup(class);
   idef->icon_file = Estrdup(file);
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

   ff = ThemeFileFind("icondefs.cfg");
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

   ff = ThemeFileFind("icondefs.cfg");
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
IcondefChecker(int val __UNUSED__, void *data __UNUSED__)
{
   IB_ReLoadIcondefs();
   DoIn("ICONDEF_CHECK", 2.0, IcondefChecker, 0, NULL);
}

#if 0				/* Not used */
static void
IB_SaveIcondefs(void)
{
   /* save the icondefs */
   char                s[1024];
   FILE               *f;

   Esnprintf(s, sizeof(s), "%s/icondefs.cfg", EDirUser());
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

static Iconbox    **
IconboxesList(int *num)
{
   /* list all currently available Iconboxes */
   return (Iconbox **) ListItemType(num, LIST_TYPE_ICONBOX);
}

static Iconbox     *
SelectIconboxForEwin(EWin * ewin)
{
   /* find the appropriate iconbox from all available ones for this app */
   /* if it is to be iconified, or if it is alreayd return which iconbox */
   /* it's in */
   Iconbox           **lst, *ib, *ib_sel = NULL;
   int                 i, num = 0;

   if (!ewin)
      return NULL;

   lst = IconboxesList(&num);
   if (!lst)
      return NULL;

   if (ewin->iconified)
     {
	/* find the iconbox this window got iconifed into */
	for (i = 0; i < num; i++)
	  {
	     ib = lst[i];
	     if (ib->type != 0)
		continue;

	     if (IconboxFindEwin(ib, ewin) < 0)
		continue;
	     ib_sel = ib;
	     break;
	  }
     }
   else
     {
	/* pick the closest iconbox physically on screen to put it in */
	int                 min_dist;
	int                 dx, dy, dist;

	min_dist = 0x7fffffff;
	for (i = 0; i < num; i++)
	  {
	     ib = lst[i];
	     if (ib->ewin == NULL || ib->type != 0)
		continue;

	     dx = (EoGetX(ib->ewin) + (EoGetW(ib->ewin) / 2)) -
		(EoGetX(ewin) + (EoGetW(ewin) / 2));
	     dy = (EoGetY(ib->ewin) + (EoGetH(ib->ewin) / 2)) -
		(EoGetY(ewin) + (EoGetH(ewin) / 2));
	     dist = (dx * dx) + (dy * dy);
	     if ((!EoIsSticky(ib->ewin)) &&
		 (EoGetDesk(ib->ewin) != EoGetDesk(ewin)))
		dist += (VRoot.w * VRoot.w) + (VRoot.h * VRoot.h);
	     if (dist < min_dist)
	       {
		  min_dist = dist;
		  ib_sel = ib;
	       }
	  }
     }

   Efree(lst);

   return ib_sel;
}

static void
IconboxUpdateEwinIcon(Iconbox * ib, EWin * ewin, int icon_mode)
{
   if (ib->icon_mode != icon_mode)
      return;

   if (IconboxFindEwin(ib, ewin) < 0)
      return;

   UpdateAppIcon(ewin, icon_mode);
   IconboxRedraw(ib);
}

static void
IconboxesUpdateEwinIcon(EWin * ewin, int icon_mode)
{
   Iconbox           **ib;
   int                 i, num = 0;

   ib = IconboxesList(&num);
   if (!ib)
      return;

   for (i = 0; i < num; i++)
      IconboxUpdateEwinIcon(ib[i], ewin, icon_mode);
   Efree(ib);
}

static void
UpdateAppIcon(EWin * ewin, int imode)
{
   /* free whatever we had before */
   if (ewin->icon_image)
     {
	imlib_context_set_image(ewin->icon_image);
	imlib_free_image();
	ewin->icon_image = NULL;
     }

   switch (imode)
     {
     case 0:
	/* snap first - if fails try app, then e */
	if (!ewin->icon_image)
	  {
	     if (ewin->shaded)
		EwinInstantUnShade(ewin);
	     RaiseEwin(ewin);
	     IB_SnapEWin(ewin);
	  }
	if (!ewin->icon_image)
	   IB_GetAppIcon(ewin);
	if (!ewin->icon_image)
	   IB_GetEIcon(ewin);
	break;
     case 1:
	/* try app first, then e, then snap */
	if (!ewin->icon_image)
	   IB_GetAppIcon(ewin);
	if (!ewin->icon_image)
	   IB_GetEIcon(ewin);
	if (!ewin->icon_image)
	  {
	     if (ewin->shaded)
		EwinInstantUnShade(ewin);
	     RaiseEwin(ewin);
	     IB_SnapEWin(ewin);
	  }
	break;
     case 2:
	/* try E first, then snap, then app */
	if (!ewin->icon_image)
	   IB_GetEIcon(ewin);
	if (!ewin->icon_image)
	  {
	     if (ewin->shaded)
		EwinInstantUnShade(ewin);
	     RaiseEwin(ewin);
	     IB_SnapEWin(ewin);
	  }
	if (!ewin->icon_image)
	   IB_GetAppIcon(ewin);
	break;
     default:
	break;
     }
}

static void
IconboxFindIconSize(Iconbox * ib, Imlib_Image * im, int type, int *pw, int *ph)
{
   int                 w, h, minsz, maxsz, maxwh;

   imlib_context_set_image(im);
   w = imlib_image_get_width();
   h = imlib_image_get_height();
   maxwh = (w > h) ? w : h;
   if (maxwh <= 1)
      goto done;

   minsz = ib->iconsize / 2;
   maxsz = ib->iconsize;

   switch (type)
     {
     case EWIN_ICON_TYPE_APP:
#if 0				/* Scale app icons too */
	break;
#endif
	goto do_scale;
     case EWIN_ICON_TYPE_IMG:
	goto do_scale;
     case EWIN_ICON_TYPE_SNAP:
	if (ib->draw_icon_base)
	  {
	     ImageClass         *ic;
	     int                 maxpad;

	     ic = ImageclassFind("DEFAULT_ICON_BUTTON", 0);
	     if (ic)
	       {
		  maxpad = ic->padding.left + ic->padding.right;
		  if (maxpad < ic->padding.top + ic->padding.bottom)
		     maxpad = ic->padding.top + ic->padding.bottom;
		  if (maxpad > 4)
		     maxpad = 4;
		  maxsz -= maxpad;
	       }
	  }
	goto do_scale;

     default:
      do_scale:
	if (maxwh < minsz || maxwh > maxsz)
	  {
	     w = (w * maxsz) / maxwh;
	     h = (h * maxsz) / maxwh;
	  }
	break;
     }

 done:
   *pw = w;
   *ph = h;
}

static void
IB_CalcMax(Iconbox * ib)
{
   int                 i, x, y, w, h;

   x = 0;
   y = 0;
   for (i = 0; i < ib->num_objs; i++)
     {
	w = 8;
	h = 8;
	if (ib->type == 0)
	  {
	     EWin               *ewin;

	     ewin = ib->objs[i].ewin;
	     if (!ewin->icon_image)
		UpdateAppIcon(ewin, ib->icon_mode);
	     if (ewin->icon_image)
		IconboxFindIconSize(ib, ewin->icon_image, ewin->icon_type, &w,
				    &h);
	  }
	else
	  {
	     w = h = ib->iconsize;
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
	ic = ImageclassFind("ICONBOX_VERTICAL", 0);
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
	ic = ImageclassFind("ICONBOX_HORIZONTAL", 0);
	x = -ib->pos;
	y = 0;
	if (ic)
	  {
	     x += ic->padding.left;
	     y += ic->padding.top;
	  }
     }

   for (i = 0; i < ib->num_objs; i++)
     {
	int                 w, h, xx, yy;

	w = 8;
	h = 8;
	if (ib->type == 0)
	  {
	     EWin               *ewin;

	     ewin = ib->objs[i].ewin;
	     if (!ewin->icon_image)
		UpdateAppIcon(ewin, ib->icon_mode);
	     if (ewin->icon_image)
	       {
		  IconboxFindIconSize(ib, ewin->icon_image, ewin->icon_type, &w,
				      &h);
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
		  if ((px >= (xx - 1)) && (py >= (yy - 1))
		      && (px < (xx + w + 1)) && (py < (yy + h + 1)))
		     return ewin;
	       }
	  }
	else
	  {
	     w = h = ib->iconsize;
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

	ic = ImageclassFind("ICONBOX_SCROLLBAR_BASE_VERTICAL", 0);
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

	ic = ImageclassFind("ICONBOX_SCROLLKNOB_VERTICAL", 0);
	if ((ic) && (bw > ib->knob_length))
	   EMoveResizeWindow(ib->scrollbarknob_win, 0,
			     (bw - ib->knob_length) / 2, ib->bar_thickness,
			     ib->knob_length);
	else
	   EMoveResizeWindow(ib->scrollbarknob_win, -9999, -9999,
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
		       EMapWindow(ib->arrow1_win);
		       EMapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->arrow1_win,
					 ib->w - ib->scroll_thickness, 0,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(ib->arrow2_win,
					 ib->w - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(ib->scroll_win,
					 ib->w - ib->scroll_thickness,
					 ib->arrow_thickness * 2,
					 ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2));
		    }
		  /* both ends */
		  else if (ib->arrow_side == 1)
		    {
		       EMapWindow(ib->arrow1_win);
		       EMapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->arrow1_win,
					 ib->w - ib->scroll_thickness, 0,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(ib->arrow2_win,
					 ib->w - ib->scroll_thickness,
					 ib->h - ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(ib->scroll_win,
					 ib->w - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2));
		    }
		  /* end */
		  else if (ib->arrow_side == 2)
		    {
		       EMapWindow(ib->arrow1_win);
		       EMapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->arrow1_win,
					 ib->w - ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2),
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(ib->arrow2_win,
					 ib->w - ib->scroll_thickness,
					 ib->h - ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(ib->scroll_win,
					 ib->w - ib->scroll_thickness, 0,
					 ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2));
		    }
		  /* no arrows */
		  else
		    {
		       EUnmapWindow(ib->arrow1_win);
		       EUnmapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->scroll_win,
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
		       EMapWindow(ib->arrow1_win);
		       EMapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->arrow1_win, 0, 0,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(ib->arrow2_win, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(ib->scroll_win, 0,
					 ib->arrow_thickness * 2,
					 ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2));
		    }
		  /* both ends */
		  else if (ib->arrow_side == 1)
		    {
		       EMapWindow(ib->arrow1_win);
		       EMapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->arrow1_win, 0, 0,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(ib->arrow2_win, 0,
					 ib->h - ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(ib->scroll_win, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2));
		    }
		  /* end */
		  else if (ib->arrow_side == 2)
		    {
		       EMapWindow(ib->arrow1_win);
		       EMapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->arrow1_win, 0,
					 ib->h - (ib->arrow_thickness * 2),
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(ib->arrow2_win, 0,
					 ib->h - ib->arrow_thickness,
					 ib->scroll_thickness,
					 ib->arrow_thickness);
		       EMoveResizeWindow(ib->scroll_win, 0, 0,
					 ib->scroll_thickness,
					 ib->h - (ib->arrow_thickness * 2));
		    }
		  /* no arrows */
		  else
		    {
		       EUnmapWindow(ib->arrow1_win);
		       EUnmapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->scroll_win, 0, 0,
					 ib->scroll_thickness, ib->h);
		    }
	       }
	  }
	else
	  {
	     EMoveResizeWindow(ib->scroll_win, -9999, -9999, 2, 2);
	     EMoveResizeWindow(ib->arrow1_win, -9999, -9999, 2, 2);
	     EMoveResizeWindow(ib->arrow2_win, -9999, -9999, 2, 2);
	  }
	EMoveResizeWindow(ib->scrollbar_win,
			  (ib->scroll_thickness - ib->bar_thickness) / 2, bx,
			  ib->bar_thickness, bw);

	ic = ImageclassFind("ICONBOX_SCROLLBAR_BASE_VERTICAL", 0);
	if (ic)
	   ImageclassApply(ic, ib->scroll_win, -1, -1, 0, 0, STATE_NORMAL, 0,
			   ST_ICONBOX);

	ic = ImageclassFind("ICONBOX_SCROLLBAR_KNOB_VERTICAL", 0);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->scrollbar_hilited)
		state = STATE_HILITED;
	     if (ib->scrollbar_clicked)
		state = STATE_CLICKED;
	     ImageclassApply(ic, ib->scrollbar_win, -1, -1, 0, 0, state, 0,
			     ST_ICONBOX);
	  }

	ic = ImageclassFind("ICONBOX_SCROLLKNOB_VERTICAL", 0);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->scrollbar_hilited)
		state = STATE_HILITED;
	     if (ib->scrollbar_clicked)
		state = STATE_CLICKED;
	     ImageclassApply(ic, ib->scrollbarknob_win, -1, -1, 0, 0, state, 0,
			     ST_ICONBOX);
	  }

	ic = ImageclassFind("ICONBOX_ARROW_UP", 0);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->arrow1_hilited)
		state = STATE_HILITED;
	     if (ib->arrow1_clicked)
		state = STATE_CLICKED;
	     ImageclassApply(ic, ib->arrow1_win, -1, -1, 0, 0, state, 0,
			     ST_ICONBOX);
	  }

	ic = ImageclassFind("ICONBOX_ARROW_DOWN", 0);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->arrow2_hilited)
		state = STATE_HILITED;
	     if (ib->arrow2_clicked)
		state = STATE_CLICKED;
	     ImageclassApply(ic, ib->arrow2_win, -1, -1, 0, 0, state, 0,
			     ST_ICONBOX);
	  }
	/* remove this coment when fixed */
     }
   else
     {
	int                 bs, bw, bx;

	ic = ImageclassFind("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 0);
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
	ic = ImageclassFind("ICONBOX_SCROLLKNOB_HORIZONTAL", 0);
	if ((ic) && (bw > ib->knob_length))
	   EMoveResizeWindow(ib->scrollbarknob_win,
			     (bw - ib->knob_length) / 2, 0, ib->knob_length,
			     ib->bar_thickness);
	else
	   EMoveResizeWindow(ib->scrollbarknob_win, -9999, -9999,
			     ib->knob_length, ib->bar_thickness);

	if (show_sb)
	  {
	     if (ib->scrollbar_side == 1)
		/* bottom */
	       {
		  /* start */
		  if (ib->arrow_side == 0)
		    {
		       EMapWindow(ib->arrow1_win);
		       EMapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->arrow1_win, 0,
					 ib->h - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(ib->arrow2_win,
					 ib->arrow_thickness,
					 ib->h - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(ib->scroll_win,
					 ib->arrow_thickness * 2,
					 ib->h - ib->scroll_thickness,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->scroll_thickness);
		    }
		  /* both ends */
		  else if (ib->arrow_side == 1)
		    {
		       EMapWindow(ib->arrow1_win);
		       EMapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->arrow1_win, 0,
					 ib->h - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(ib->arrow2_win,
					 ib->w - ib->arrow_thickness,
					 ib->h - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(ib->scroll_win,
					 ib->arrow_thickness,
					 ib->h - ib->scroll_thickness,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->scroll_thickness);
		    }
		  /* end */
		  else if (ib->arrow_side == 2)
		    {
		       EMapWindow(ib->arrow1_win);
		       EMapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->arrow1_win,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->h - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(ib->arrow2_win,
					 ib->w - ib->arrow_thickness,
					 ib->h - ib->scroll_thickness,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(ib->scroll_win, 0,
					 ib->h - ib->scroll_thickness,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->scroll_thickness);
		    }
		  /* no arrows */
		  else
		    {
		       EUnmapWindow(ib->arrow1_win);
		       EUnmapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->scroll_win, 0,
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
		       EMapWindow(ib->arrow1_win);
		       EMapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->arrow1_win, 0, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(ib->arrow2_win,
					 ib->arrow_thickness, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(ib->scroll_win,
					 ib->arrow_thickness * 2, 0,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->scroll_thickness);
		    }
		  /* both ends */
		  else if (ib->arrow_side == 1)
		    {
		       EMapWindow(ib->arrow1_win);
		       EMapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->arrow1_win, 0, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(ib->arrow2_win,
					 ib->w - ib->arrow_thickness, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(ib->scroll_win,
					 ib->arrow_thickness, 0,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->scroll_thickness);
		    }
		  /* end */
		  else if (ib->arrow_side == 2)
		    {
		       EMapWindow(ib->arrow1_win);
		       EMapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->arrow1_win,
					 ib->w - (ib->arrow_thickness * 2), 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(ib->arrow2_win,
					 ib->w - ib->arrow_thickness, 0,
					 ib->arrow_thickness,
					 ib->scroll_thickness);
		       EMoveResizeWindow(ib->scroll_win, 0, 0,
					 ib->w - (ib->arrow_thickness * 2),
					 ib->scroll_thickness);
		    }
		  /* no arrows */
		  else
		    {
		       EUnmapWindow(ib->arrow1_win);
		       EUnmapWindow(ib->arrow2_win);
		       EMoveResizeWindow(ib->scroll_win, 0, 0, ib->w,
					 ib->scroll_thickness);
		    }
	       }
	  }
	else
	  {
	     EMoveResizeWindow(ib->scroll_win, -9999, -9999, 2, 2);
	     EMoveResizeWindow(ib->arrow1_win, -9999, -9999, 2, 2);
	     EMoveResizeWindow(ib->arrow2_win, -9999, -9999, 2, 2);
	  }

	EMoveResizeWindow(ib->scrollbar_win, bx,
			  (ib->scroll_thickness - ib->bar_thickness) / 2, bw,
			  ib->bar_thickness);

	ic = ImageclassFind("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 0);
	if (ic)
	   ImageclassApply(ic, ib->scroll_win, -1, -1, 0, 0, STATE_NORMAL, 0,
			   ST_ICONBOX);

	ic = ImageclassFind("ICONBOX_SCROLLBAR_KNOB_HORIZONTAL", 0);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->scrollbar_hilited)
		state = STATE_HILITED;
	     if (ib->scrollbar_clicked)
		state = STATE_CLICKED;
	     ImageclassApply(ic, ib->scrollbar_win, -1, -1, 0, 0, state, 0,
			     ST_ICONBOX);
	  }

	ic = ImageclassFind("ICONBOX_SCROLLKNOB_HORIZONTAL", 0);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->scrollbar_hilited)
		state = STATE_HILITED;
	     if (ib->scrollbar_clicked)
		state = STATE_CLICKED;
	     ImageclassApply(ic, ib->scrollbarknob_win, -1, -1, 0, 0, state, 0,
			     ST_ICONBOX);
	  }

	ic = ImageclassFind("ICONBOX_ARROW_LEFT", 0);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->arrow1_hilited)
		state = STATE_HILITED;
	     if (ib->arrow1_clicked)
		state = STATE_CLICKED;
	     ImageclassApply(ic, ib->arrow1_win, -1, -1, 0, 0, state, 0,
			     ST_ICONBOX);
	  }

	ic = ImageclassFind("ICONBOX_ARROW_RIGHT", 0);
	if (ic)
	  {
	     int                 state = STATE_NORMAL;

	     if (ib->arrow2_hilited)
		state = STATE_HILITED;
	     if (ib->arrow2_clicked)
		state = STATE_CLICKED;
	     ImageclassApply(ic, ib->arrow2_win, -1, -1, 0, 0, state, 0,
			     ST_ICONBOX);
	  }
     }
   PropagateShapes(ib->win);
   if (ib->ewin)
     {
	const Border       *b;

	b = ib->ewin->border;
	SyncBorderToEwin(ib->ewin);
	if (ib->ewin->border == b)
	   EwinPropagateShapes(ib->ewin);
     }
}

static void
IB_FixPos(Iconbox * ib)
{
   if (ib->orientation)
     {
	ImageClass         *ic;
	int                 v = 0;

	ic = ImageclassFind("ICONBOX_SCROLLBAR_BASE_VERTICAL", 0);
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

	ic = ImageclassFind("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 0);
	v = ib->max - ib->w;
	if (ic)
	   v += ic->padding.left + ic->padding.right;
	if (ib->pos > v)
	   ib->pos = v;
     }
   if (ib->pos < 0)
      ib->pos = 0;

}

static void
IconboxRedraw(Iconbox * ib)
{
   char                pq;
   char                was_shaded = 0;
   int                 i, x, y, w, h;
   ImageClass         *ib_ic_cover, *ib_ic_box;
   int                 ib_x0, ib_y0, ib_xlt, ib_ylt, ib_ww, ib_hh;
   Imlib_Image        *im, *im2;
   int                 ww, hh;

   if (!ib)
      return;
   if (!ib->ewin)
      return;

   if (ib->orientation)
     {
	ib_ic_box = ImageclassFind("ICONBOX_VERTICAL", 0);
     }
   else
     {
	ib_ic_box = ImageclassFind("ICONBOX_HORIZONTAL", 0);
     }

   x = EoGetX(ib->ewin);
   y = EoGetY(ib->ewin);
   w = ib->w;
   h = ib->h;

   if (ib->auto_resize)
     {
	int                 add = 0;

	if (ib->ewin->shaded)
	  {
	     was_shaded = 1;
	     EwinUnShade(ib->ewin);
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
		       ib->ewin->border->border.bottom + add) > VRoot.h)
		     add =
			VRoot.h - (ib->ewin->border->border.top +
				   ib->ewin->border->border.bottom);
	       }
	     y += (((ib->ewin->client.h - add) * ib->auto_resize_anchor) >> 10);
	     h = add;
	     if (ib->ewin->border)
	       {
		  if ((EoGetY(ib->ewin) + ib->ewin->border->border.top +
		       ib->ewin->border->border.bottom + add) > VRoot.h)
		     y = VRoot.h - (ib->ewin->border->border.top +
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
		       ib->ewin->border->border.right + add) > VRoot.w)
		     add =
			VRoot.w - (ib->ewin->border->border.left +
				   ib->ewin->border->border.right);
	       }
	     x += (((ib->ewin->client.w - add) * ib->auto_resize_anchor) >> 10);
	     w = add;
	     if (ib->ewin->border)
	       {
		  if ((EoGetX(ib->ewin) + ib->ewin->border->border.left +
		       ib->ewin->border->border.right + add) > VRoot.w)
		     x = VRoot.w - (ib->ewin->border->border.left +
				    ib->ewin->border->border.right + add);
	       }
	  }
     }

   if (ib->force_update || (x != EoGetX(ib->ewin))
       || (y != EoGetY(ib->ewin)) || (w != ib->ewin->client.w)
       || (h != ib->ewin->client.h))
     {
	ib->w = w;
	ib->h = h;
	MoveResizeEwin(ib->ewin, x, y, w, h);
	EResizeWindow(ib->win, w, h);
	im = imlib_create_image(w, h);
	ib->force_update = 0;
     }

   if (was_shaded)
      EwinShade(ib->ewin);

   pq = Mode.queue_up;
   Mode.queue_up = 0;

   IB_CalcMax(ib);
   IB_FixPos(ib);
   IB_DrawScroll(ib);

   if (ib->orientation)
     {
	ib_ic_cover = ImageclassFind("ICONBOX_COVER_VERTICAL", 0);
	ib_x0 = 0;
	ib_y0 = -ib->pos;
	ib_xlt = (ib->scrollbar_side == 1) ? 0 : ib->scroll_thickness;
	ib_ylt = 0;
	ib_ww = ib->w - ib->scroll_thickness;
	ib_hh = ib->h;
     }
   else
     {
	ib_ic_cover = ImageclassFind("ICONBOX_COVER_HORIZONTAL", 0);
	ib_x0 = -ib->pos;
	ib_y0 = 0;
	ib_xlt = 0;
	ib_ylt = (ib->scrollbar_side == 1) ? 0 : ib->scroll_thickness;
	ib_ww = ib->w;
	ib_hh = ib->h - ib->scroll_thickness;
     }

   EMoveResizeWindow(ib->icon_win, ib_xlt, ib_ylt, ib_ww, ib_hh);

   if (ib_ic_cover && (!(ib->cover_hide)))
     {
	EMoveResizeWindow(ib->cover_win, ib_xlt, ib_ylt, ib_ww, ib_hh);
	EMapWindow(ib->cover_win);
	ImageclassApply(ib_ic_cover, ib->cover_win, -1, -1, 0, 0, STATE_NORMAL,
			0, ST_ICONBOX);
     }
   else
     {
	EMoveResizeWindow(ib->cover_win, -30000, -30000, 2, 2);
	EUnmapWindow(ib->cover_win);
     }

   if (!ib->nobg && ib_ic_box)
     {
	im2 = ImageclassGetImage(ib_ic_box, 0, 0, STATE_NORMAL);
	imlib_context_set_image(im2);
	ww = imlib_image_get_width();
	hh = imlib_image_get_height();
	im = imlib_create_cropped_scaled_image(0, 0, ww, hh, w, h);
	imlib_context_set_image(im2);
	imlib_free_image();
     }
   else
     {
	im = imlib_create_image(w, h);
	imlib_context_set_image(im);
	imlib_image_set_has_alpha(1);
	imlib_context_set_blend(0);
	imlib_context_set_color(0, 0, 0, 0);
	imlib_image_fill_rectangle(0, 0, w, h);
     }

   x = ib_x0;
   y = ib_y0;
   if (ib_ic_box)
     {
	x += ib_ic_box->padding.left;
	y += ib_ic_box->padding.top;
     }

   for (i = 0; i < ib->num_objs; i++)
     {
	EWin               *ewin;

	w = 8;
	h = 8;

	if (ib->type == 0)
	  {
	     if (ib->draw_icon_base)
	       {
		  ImageClass         *ic;

		  /* get the base pixmap */
		  ic = ImageclassFind("DEFAULT_ICON_BUTTON", 0);
		  if (ic)
		    {
		       im2 = ImageclassGetImage(ic, 0, 0, STATE_NORMAL);
		       imlib_context_set_image(im2);
		       ww = imlib_image_get_width();
		       hh = imlib_image_get_height();
		       imlib_context_set_image(im);
		       imlib_context_set_blend(0);
		       imlib_blend_image_onto_image(im2, 1, 0, 0, ww, hh,
						    x, y, ib->iconsize,
						    ib->iconsize);
#if 0
		       imlib_context_set_image(im2);
		       imlib_free_image();
#endif
		    }
	       }

	     ewin = ib->objs[i].ewin;

	     if (!ewin->icon_image)
		UpdateAppIcon(ewin, ib->icon_mode);
	     if (ewin->icon_image)
	       {
		  int                 xoff, yoff;

		  IconboxFindIconSize(ib, ewin->icon_image, ewin->icon_type, &w,
				      &h);

		  if (ib->draw_icon_base)
		    {
		       xoff = (ib->iconsize - w) / 2;
		       yoff = (ib->iconsize - h) / 2;
		    }
		  else if (ib->orientation)
		    {
		       xoff = (ib->iconsize - w) / 2;
		       yoff = 0;
		    }
		  else
		    {
		       xoff = 0;
		       yoff = (ib->iconsize - h) / 2;
		    }

		  imlib_context_set_image(ewin->icon_image);
		  ww = imlib_image_get_width();
		  hh = imlib_image_get_height();
		  imlib_context_set_image(im);
		  imlib_context_set_anti_alias(1);
		  imlib_context_set_blend(1);
		  imlib_blend_image_onto_image(ewin->icon_image, 1,
					       0, 0, ww, hh,
					       x + xoff, y + yoff, w, h);
		  imlib_context_set_anti_alias(0);
	       }
	  }
	else
	  {
	     w = h = ib->iconsize;
	     EMoveResizeWindow(ib->objs[i].win, x, y, w, h);
	  }

	if (ib->orientation)
	   y += (ib->draw_icon_base) ? ib->iconsize : h + 2;
	else
	   x += (ib->draw_icon_base) ? ib->iconsize : w + 2;
     }

   if (ib->pmap != None)
      imlib_free_pixmap_and_mask(ib->pmap);
   ib->pmap = ib->mask = None;
   imlib_context_set_drawable(ib->icon_win);
   imlib_context_set_image(im);
   imlib_image_set_has_alpha(1);
   imlib_render_pixmaps_for_whole_image(&ib->pmap, &ib->mask);
   ESetWindowBackgroundPixmap(ib->icon_win, ib->pmap);
   EShapeCombineMask(ib->icon_win, ShapeBounding, 0, 0, ib->mask, ShapeSet);
   if (ib->nobg && ib->num_objs == 0)
      EMoveWindow(ib->icon_win, -ib->w, -ib->h);
   imlib_free_image();
   EClearWindow(ib->icon_win);

   PropagateShapes(ib->win);
   ICCCM_GetShapeInfo(ib->ewin);
   EwinPropagateShapes(ib->ewin);

   Mode.queue_up = pq;
}

static void
IB_Scroll(Iconbox * ib, int dir)
{
   ib->pos += dir;
   IB_FixPos(ib);
   IconboxRedraw(ib);
}

static void
IB_ShowMenu(Iconbox * ib, int x __UNUSED__, int y __UNUSED__)
{
   static Menu        *p_menu = NULL;
   MenuItem           *mi;
   char                s[1024];

   if (p_menu)
      MenuDestroy(p_menu);
   p_menu = MenuCreate("__IBOX_MENU", _("Iconbox Options"), NULL, NULL);

   Esnprintf(s, sizeof(s), "ibox cfg %s", ib->name);
   mi = MenuItemCreate(_("This Iconbox Settings..."), NULL, s, NULL);
   MenuAddItem(p_menu, mi);

   Esnprintf(s, sizeof(s), "wop %#lx cl", ib->win);
   mi = MenuItemCreate(_("Close Iconbox"), NULL, s, NULL);
   MenuAddItem(p_menu, mi);

   mi = MenuItemCreate(_("Create New Iconbox"), NULL, "ibox new", NULL);
   MenuAddItem(p_menu, mi);

   EFunc("menus show __IBOX_MENU");
}

static void
IB_CompleteRedraw(Iconbox * ib)
{
   IB_Reconfigure(ib);
   IconboxRedraw(ib);
}

static void
IconboxesShow(void)
{
   int                 i, num;
   Iconbox           **ibl;

   IcondefChecker(0, NULL);
   ibl = IconboxesList(&num);
   if (ibl)
     {
	for (i = 0; i < num; i++)
	   IconboxShow(ibl[i]);
	Efree(ibl);
     }
   else if (Conf.startup.firsttime)
     {
	Iconbox            *ib;

	ib = IconboxCreate("_IB_0");
	IconboxShow(ib);
	IconboxesConfigSave();
     }
}

#if 0
static void
IconboxesDestroy(void)
{
   int                 i, num;
   Iconbox           **ibl;

   ibl = IconboxesList(&num);
   if (!ibl)
      return;

   for (i = 0; i < num; i++)
      IconboxDestroy(ibl[i]);
   Efree(ibl);
}
#endif

static void
IboxEventScrollWin(XEvent * ev, void *prm)
{
   Iconbox            *ib = (Iconbox *) prm;
   int                 x, y, w, h;

   switch (ev->type)
     {
     case ButtonPress:
	if (ev->xbutton.button == 1)
	   ib->scrollbox_clicked = 1;
	else if (ev->xbutton.button == 3)
	   IB_ShowMenu(ib, ev->xbutton.x, ev->xbutton.y);
	break;
     case ButtonRelease:
	if (!ib->scrollbox_clicked)
	   break;

	ib->scrollbox_clicked = 0;
	GetWinXY(ib->scrollbar_win, &x, &y);
	GetWinWH(ib->scrollbar_win, (unsigned int *)&w, (unsigned int *)&h);
	if (ev->xbutton.x < x)
	   IB_Scroll(ib, -8);
	if (ev->xbutton.x > (x + w))
	   IB_Scroll(ib, 8);
	break;
     }
}

static void
IboxEventScrollbarWin(XEvent * ev, void *prm)
{
   Iconbox            *ib = (Iconbox *) prm;
   static int          px, py;
   int                 dx, dy, bs, x, y;
   ImageClass         *ic;

   switch (ev->type)
     {
     case ButtonPress:
	if (ev->xbutton.button == 1)
	  {
	     px = ev->xbutton.x_root;
	     py = ev->xbutton.y_root;
	     ib->scrollbar_clicked = 1;
	  }
	else if (ev->xbutton.button == 3)
	   IB_ShowMenu(ib, ev->xbutton.x, ev->xbutton.y);
	break;

     case ButtonRelease:
	if (ib->scrollbar_clicked)
	   ib->scrollbar_clicked = 0;
	break;

     case EnterNotify:
	ib->scrollbar_hilited = 1;
	break;

     case LeaveNotify:
	ib->scrollbar_hilited = 0;
	break;

     case MotionNotify:
	if (!ib->scrollbar_clicked)
	   break;

	dx = ev->xmotion.x_root - px;
	dy = ev->xmotion.y_root - py;
	px = ev->xmotion.x_root;
	py = ev->xmotion.y_root;

	if (ib->orientation)
	  {
	     ic = ImageclassFind("ICONBOX_SCROLLBAR_BASE_VERTICAL", 0);
	     GetWinXY(ib->scrollbar_win, &x, &y);
	     bs = ib->h - (ib->arrow_thickness * 2);
	     if (ic)
	       {
		  bs -= (ic->padding.top + ic->padding.bottom);
		  y -= ic->padding.top;
	       }
	     if (bs < 1)
		bs = 1;
	     ib->pos = ((y + dy + 1) * ib->max) / bs;
	     IB_FixPos(ib);
	     IconboxRedraw(ib);
	  }
	else
	  {
	     ic = ImageclassFind("ICONBOX_SCROLLBAR_BASE_HORIZONTAL", 0);
	     GetWinXY(ib->scrollbar_win, &x, &y);
	     bs = ib->w - (ib->arrow_thickness * 2);
	     if (ic)
	       {
		  bs -= (ic->padding.left + ic->padding.right);
		  x -= ic->padding.left;
	       }
	     if (bs < 1)
		bs = 1;
	     ib->pos = ((x + dx + 1) * ib->max) / bs;
	     IB_FixPos(ib);
	     IconboxRedraw(ib);
	  }
     }
   IB_DrawScroll(ib);
}

static void
IboxEventCoverWin(XEvent * ev, void *prm)
{
   Iconbox            *ib = (Iconbox *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	IB_ShowMenu(ib, ev->xbutton.x, ev->xbutton.y);
	break;
     case ButtonRelease:
	break;
     }
}

static void
IboxEventArrow1Win(XEvent * ev, void *prm)
{
   Iconbox            *ib = (Iconbox *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	if (ev->xbutton.button == 1)
	   ib->arrow1_clicked = 1;
	else if (ev->xbutton.button == 3)
	   IB_ShowMenu(ib, ev->xbutton.x, ev->xbutton.y);
	break;

     case ButtonRelease:
	if (!ib->arrow1_clicked)
	   break;
	ib->arrow1_clicked = 0;
	IB_Scroll(ib, -8);
	break;

     case EnterNotify:
	ib->arrow1_hilited = 1;
	break;

     case LeaveNotify:
	ib->arrow1_hilited = 0;
	break;
     }
   IB_DrawScroll(ib);
}

static void
IboxEventArrow2Win(XEvent * ev, void *prm)
{
   Iconbox            *ib = (Iconbox *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	if (ev->xbutton.button == 1)
	   ib->arrow2_clicked = 1;
	else if (ev->xbutton.button == 3)
	   IB_ShowMenu(ib, ev->xbutton.x, ev->xbutton.y);
	break;

     case ButtonRelease:
	if (!ib->arrow2_clicked)
	   break;
	ib->arrow2_clicked = 0;
	IB_Scroll(ib, -8);
	break;

     case EnterNotify:
	ib->arrow2_hilited = 1;
	break;

     case LeaveNotify:
	ib->arrow2_hilited = 0;
	break;
     }
   IB_DrawScroll(ib);
}

static void
IboxEventIconWin(XEvent * ev, void *prm)
{
   Iconbox            *ib = (Iconbox *) prm;
   static EWin        *name_ewin = NULL;
   ToolTip            *tt;
   EWin               *ewin;
   EWin              **gwins;
   int                 j, numg;
   char                iconified;

   switch (ev->type)
     {
     case ButtonPress:
	if (ev->xbutton.button == 1)
	   ib->icon_clicked = 1;
	else if (ev->xbutton.button == 3)
	   IB_ShowMenu(ib, ev->xbutton.x, ev->xbutton.y);
	break;

     case ButtonRelease:
	if (!ib->icon_clicked)
	   break;

	ib->icon_clicked = 0;
	ewin = IB_FindIcon(ib, ev->xbutton.x, ev->xbutton.y);
	if (ewin)
	  {
	     tt = FindItem("ICONBOX", 0, LIST_FINDBY_NAME, LIST_TYPE_TOOLTIP);
	     if (tt)
		TooltipHide(tt);
	     gwins = ListWinGroupMembersForEwin(ewin, GROUP_ACTION_ICONIFY,
						Mode.nogroup, &numg);
	     iconified = ewin->iconified;

	     if (gwins)
	       {
		  for (j = 0; j < numg; j++)
		    {
		       if ((gwins[j]->iconified) && (iconified))
			  EwinDeIconify(gwins[j]);
		    }
		  Efree(gwins);
	       }
	  }
	break;

     case MotionNotify:
     case EnterNotify:
	if (ev->type == MotionNotify)
	  {
	     ewin = IB_FindIcon(ib, ev->xmotion.x, ev->xmotion.y);
	     Mode.x = ev->xmotion.x_root;
	     Mode.y = ev->xmotion.y_root;
	  }
	else
	  {
	     ewin = IB_FindIcon(ib, ev->xcrossing.x, ev->xcrossing.y);
	     Mode.x = ev->xcrossing.x_root;
	     Mode.y = ev->xcrossing.y_root;
	  }

	if (ib->shownames && ewin != name_ewin)
	  {
	     tt = FindItem("ICONBOX", 0, LIST_FINDBY_NAME, LIST_TYPE_TOOLTIP);
	     if (tt)
	       {
		  const char         *name;

		  TooltipHide(tt);
		  if (ewin)
		    {

		       name = EwinGetIconName(ewin);
		       if (name)
			  TooltipShow(tt, name, NULL, Mode.x, Mode.y);
		    }
	       }
	     name_ewin = ewin;
	  }
	break;

     case LeaveNotify:
	tt = FindItem("ICONBOX", 0, LIST_FINDBY_NAME, LIST_TYPE_TOOLTIP);
	if (tt)
	  {
	     TooltipHide(tt);
	     name_ewin = NULL;
	  }
	break;
     }
}

#if 0
static void
IboxEventScrolWin(XEvent * ev, void *prm)
{
   MenuItem           *mi = (MenuItem *) prm;

   switch (ev->type)
     {
     case ButtonPress:
	break;
     case ButtonRelease:
	break;
     case EnterNotify:
	break;
     case LeaveNotify:
	break;
     }
}
#endif

/*
 * Configuration dialog
 */
static char        *tmp_ib_name = NULL;
static char         tmp_ib_nobg;
static char         tmp_ib_shownames;
static int          tmp_ib_vert;
static int          tmp_ib_side;
static int          tmp_ib_arrows;
static int          tmp_ib_iconsize;
static int          tmp_ib_mode;
static char         tmp_ib_auto_resize;
static char         tmp_ib_draw_icon_base;
static char         tmp_ib_scrollbar_hide;
static char         tmp_ib_cover_hide;
static int          tmp_ib_autoresize_anchor;
static char         tmp_ib_animate;

static void
CB_ConfigureIconbox(Dialog * d __UNUSED__, int val, void *data __UNUSED__)
{
   if (val < 2)
     {
	Iconbox            *ib;

	if (!tmp_ib_name)
	   return;
	ib = FindItem(tmp_ib_name, 0, LIST_FINDBY_NAME, LIST_TYPE_ICONBOX);
	if (!ib)
	   return;

	ib->nobg = tmp_ib_nobg;
	ib->shownames = tmp_ib_shownames;
	ib->orientation = tmp_ib_vert;
	ib->scrollbar_side = tmp_ib_side;
	ib->arrow_side = tmp_ib_arrows;
	ib->iconsize = tmp_ib_iconsize;
	ib->icon_mode = tmp_ib_mode;
	ib->auto_resize = tmp_ib_auto_resize;
	ib->draw_icon_base = tmp_ib_draw_icon_base;
	ib->scrollbar_hide = tmp_ib_scrollbar_hide;
	ib->cover_hide = tmp_ib_cover_hide;
	ib->auto_resize_anchor = tmp_ib_autoresize_anchor;
	ib->animate = tmp_ib_animate;
	IB_CompleteRedraw(ib);

	IconboxesConfigSave();
     }
}

static void
IconboxConfigure(Iconbox * ib)
{
   Dialog             *d;
   DItem              *table, *di, *radio1, *radio2, *radio3, *radio4, *table2;

   if (!ib)
      return;

   if ((d =
	FindItem("CONFIGURE_ICONBOX", 0, LIST_FINDBY_NAME, LIST_TYPE_DIALOG)))
     {
	SoundPlay("SOUND_SETTINGS_ACTIVE");
	ShowDialog(d);
	return;
     }
   SoundPlay("SOUND_SETTINGS_ICONBOX");

   tmp_ib_nobg = ib->nobg;
   tmp_ib_shownames = ib->shownames;
   tmp_ib_vert = ib->orientation;
   tmp_ib_side = ib->scrollbar_side;
   tmp_ib_arrows = ib->arrow_side;
   tmp_ib_iconsize = ib->iconsize;
   tmp_ib_mode = ib->icon_mode;
   tmp_ib_auto_resize = ib->auto_resize;
   tmp_ib_draw_icon_base = ib->draw_icon_base;
   tmp_ib_scrollbar_hide = ib->scrollbar_hide;
   tmp_ib_cover_hide = ib->cover_hide;
   tmp_ib_autoresize_anchor = ib->auto_resize_anchor;
   tmp_ib_animate = ib->animate;
   if (tmp_ib_name)
      Efree(tmp_ib_name);
   tmp_ib_name = Estrdup(ib->name);

   d = DialogCreate("CONFIGURE_ICONBOX");
   DialogSetTitle(d, _("Iconbox Settings"));

   table = DialogInitItem(d);
   DialogItemTableSetOptions(table, 1, 0, 0, 0);

   if (Conf.dialogs.headers)
     {
	table2 = DialogAddItem(table, DITEM_TABLE);
	DialogItemTableSetOptions(table2, 2, 0, 0, 0);

	di = DialogAddItem(table2, DITEM_IMAGE);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemImageSetFile(di, "pix/iconbox.png");

	di = DialogAddItem(table2, DITEM_TEXT);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemTextSetText(di,
			      _("Enlightenment Iconbox\n" "Settings Dialog\n"));

	di = DialogAddItem(table, DITEM_SEPARATOR);
	DialogItemSetPadding(di, 2, 2, 2, 2);
	DialogItemSetFill(di, 1, 0);
	DialogItemSeparatorSetOrientation(di, 0);
     }

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Transparent background"));
   DialogItemCheckButtonSetState(di, tmp_ib_nobg);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_nobg);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Hide border around inner Iconbox"));
   DialogItemCheckButtonSetState(di, tmp_ib_cover_hide);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_cover_hide);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Show icon names"));
   DialogItemCheckButtonSetState(di, tmp_ib_shownames);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_shownames);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Draw base image behind Icons"));
   DialogItemCheckButtonSetState(di, tmp_ib_draw_icon_base);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_draw_icon_base);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Hide scrollbar when not needed"));
   DialogItemCheckButtonSetState(di, tmp_ib_scrollbar_hide);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_scrollbar_hide);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di, _("Automatically resize to fit Icons"));
   DialogItemCheckButtonSetState(di, tmp_ib_auto_resize);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_auto_resize);

   di = DialogAddItem(table, DITEM_CHECKBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemCheckButtonSetText(di,
				_("Animate when iconifying to this Iconbox"));
   DialogItemCheckButtonSetState(di, tmp_ib_animate);
   DialogItemCheckButtonSetPtr(di, &tmp_ib_animate);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 0);
   DialogItemTextSetText(di,
			 _
			 ("Alignment of anchoring when automatically resizing:"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 0, 1024);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 8);
   DialogItemSliderSetVal(di, tmp_ib_autoresize_anchor);
   DialogItemSliderSetValPtr(di, &tmp_ib_autoresize_anchor);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 0);
   DialogItemTextSetText(di,
			 _
			 ("Icon image display policy (if one operation fails, try the next):"));

   radio4 = di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di,
				_
				("Snapshot Windows, Use application icon, Use Enlightenment Icon"));
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di,
				_
				("Use application icon, Use Enlightenment Icon, Snapshot Window"));
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di,
				_("Use Enlightenment Icon, Snapshot Window"));
   DialogItemRadioButtonSetFirst(di, radio4);
   DialogItemRadioButtonGroupSetVal(di, 2);
   DialogItemRadioButtonGroupSetValPtr(radio4, &tmp_ib_mode);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   di = DialogAddItem(table, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Icon size"));

   di = DialogAddItem(table, DITEM_SLIDER);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSliderSetBounds(di, 4, 128);
   DialogItemSliderSetUnits(di, 1);
   DialogItemSliderSetJump(di, 8);
   DialogItemSliderSetVal(di, tmp_ib_iconsize);
   DialogItemSliderSetValPtr(di, &tmp_ib_iconsize);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   table2 = DialogAddItem(table, DITEM_TABLE);
   DialogItemTableSetOptions(table2, 3, 0, 0, 0);

   di = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Orientation:"));

   di = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Scrollbar side:"));

   di = DialogAddItem(table2, DITEM_TEXT);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 0, 0);
   DialogItemSetAlign(di, 0, 512);
   DialogItemTextSetText(di, _("Scrollbar arrows:"));

   radio1 = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Horizontal"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio2 = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Left / Top"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 0);

   radio3 = di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Start"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 0);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Vertical"));
   DialogItemRadioButtonSetFirst(di, radio1);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio1, &tmp_ib_vert);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Right / Bottom"));
   DialogItemRadioButtonSetFirst(di, radio2);
   DialogItemRadioButtonGroupSetVal(di, 1);
   DialogItemRadioButtonGroupSetValPtr(radio2, &tmp_ib_side);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("Both ends"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 1);

   di = DialogAddItem(table2, DITEM_NONE);
   di = DialogAddItem(table2, DITEM_NONE);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("End"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 2);

   di = DialogAddItem(table2, DITEM_NONE);
   di = DialogAddItem(table2, DITEM_NONE);

   di = DialogAddItem(table2, DITEM_RADIOBUTTON);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemRadioButtonSetText(di, _("None"));
   DialogItemRadioButtonSetFirst(di, radio3);
   DialogItemRadioButtonGroupSetVal(di, 3);
   DialogItemRadioButtonGroupSetValPtr(radio3, &tmp_ib_arrows);

   di = DialogAddItem(table, DITEM_SEPARATOR);
   DialogItemSetPadding(di, 2, 2, 2, 2);
   DialogItemSetFill(di, 1, 0);
   DialogItemSeparatorSetOrientation(di, 0);

   DialogAddButton(d, _("OK"), CB_ConfigureIconbox, 1);
   DialogAddButton(d, _("Apply"), CB_ConfigureIconbox, 0);
   DialogAddButton(d, _("Close"), CB_ConfigureIconbox, 1);
   DialogSetExitFunction(d, CB_ConfigureIconbox, 2);
   DialogBindKey(d, "Escape", DialogCallbackClose, 0);
   DialogBindKey(d, "Return", CB_ConfigureIconbox, 0);
   ShowDialog(d);
}

/*
 * Configuration load/save
 */
#include "conf.h"

static void
IconboxesConfigLoad(void)
{
   int                 err = 0;
   FILE               *fs;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1, i2;
   int                 fields;
   Iconbox            *ib;

   Esnprintf(s, sizeof(s), "%s.ibox", EGetSavePrefix());
   fs = fopen(s, "r");
   if (!fs)
      return;

   ib = NULL;
   while (fgets(s, sizeof(s), fs))
     {
	s2[0] = 0;
	i1 = CONFIG_INVALID;
	fields = sscanf(s, "%i %4000s", &i1, s2);

	if (fields < 1)
	   i1 = CONFIG_INVALID;
	else if (i1 == CONFIG_CLOSE)
	  {
	     if (fields != 1)
		Alert(_("CONFIG: ignoring extra data in \"%s\"\n"), s);
	  }

	switch (i1)
	  {
	  case CONFIG_IBOX:
	     err = -1;
	     i2 = atoi(s2);
	     if (i2 != CONFIG_OPEN)
		goto done;
	     break;
	  case CONFIG_CLOSE:
	     ib = NULL;
	     err = 0;
	     break;

	  case CONFIG_CLASSNAME:	/* __NAME %s */
	     ib = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_ICONBOX);
	     if (ib)
		IconboxDestroy(ib);
	     ib = IconboxCreate(s2);
	     break;
	  case TEXT_ORIENTATION:	/* __ORIENTATION [ __HORIZONTAL | __VERTICAL ] */
	     if (ib)
		ib->orientation = (char)atoi(s2);
	     break;
	  case CONFIG_TRANSPARENCY:	/* __TRANSPARENCY [ __ON | __OFF ] */
	     if (ib)
		ib->nobg = (char)atoi(s2);
	     break;
	  case CONFIG_SHOW_NAMES:	/* __SHOW_NAMES [ __ON | __OFF ] */
	     if (ib)
		ib->shownames = (char)atoi(s2);
	     break;
	  case CONFIG_ICON_SIZE:	/* __ICON_SIZE %i */
	     if (ib)
		ib->iconsize = (int)atoi(s2);
	     break;
	  case CONFIG_ICON_MODE:	/* __ICON_MODE [ 0 | 1 | 2 | 3 | 4 ] */
	     if (ib)
		ib->icon_mode = (int)atoi(s2);
	     break;
	  case CONFIG_SCROLLBAR_SIDE:	/* __SCROLLBAR_SIDE [ __BAR_LEFT/__BAR_TOP | __BAR_RIGHT/__BAR_BOTTOM ] */
	     if (ib)
		ib->scrollbar_side = (char)atoi(s2);
	     break;
	  case CONFIG_SCROLLBAR_ARROWS:	/* __SCROLLBAR_ARROWS [ __START | __BOTH | __FINISH | __NEITHER ] */
	     if (ib)
		ib->arrow_side = (char)atoi(s2);
	     break;
	  case CONFIG_AUTOMATIC_RESIZE:	/* __AUTOMATIC_RESIZE [ __ON | __OFF ] */
	     if (ib)
		ib->auto_resize = (char)atoi(s2);
	     break;
	  case CONFIG_SHOW_ICON_BASE:	/* __SHOW_ICON_BASE [ __ON | __OFF ] */
	     if (ib)
		ib->draw_icon_base = (char)atoi(s2);
	     break;
	  case CONFIG_SCROLLBAR_AUTOHIDE:	/* __SCROLLBAR_AUTOHIDE [ __ON | __OFF ] */
	     if (ib)
		ib->scrollbar_hide = (char)atoi(s2);
	     break;
	  case CONFIG_COVER_HIDE:	/* __COVER_HIDE [ __ON | __OFF ] */
	     if (ib)
		ib->cover_hide = (char)atoi(s2);
	     break;
	  case CONFIG_RESIZE_ANCHOR:	/* __COVER_HIDE 0-1024 */
	     if (ib)
		ib->auto_resize_anchor = atoi(s2);
	     break;
	  case CONFIG_IB_ANIMATE:	/* __COVER_HIDE 0-1024 */
	     if (ib)
		ib->animate = (char)atoi(s2);
	     break;
	  default:
	     Eprintf("Warning: Iconbox configuration, ignoring: %s\n", s);
	     break;
	  }
     }
   if (err)
      Eprintf("Error: Iconbox configuration file load problem.\n");

 done:
   fclose(fs);
}

static void
IconboxesConfigSave(void)
{
   char                s[FILEPATH_LEN_MAX];
   FILE               *fs;
   int                 i, num;
   Iconbox           **iblist;

   iblist = IconboxesList(&num);

   Esnprintf(s, sizeof(s), "%s.ibox", EGetSavePrefix());
   fs = fopen(s, "w");
   if (!fs)
      return;

   /* We should check for errors... */
   for (i = num - 1; i >= 0; i--)
     {
	fprintf(fs, "19 999\n");
	fprintf(fs, "100 %s\n", iblist[i]->name);
	fprintf(fs, "200 %i\n", iblist[i]->orientation);
	fprintf(fs, "2001 %i\n", iblist[i]->nobg);
	fprintf(fs, "2002 %i\n", iblist[i]->shownames);
	fprintf(fs, "2003 %i\n", iblist[i]->iconsize);
	fprintf(fs, "2004 %i\n", iblist[i]->icon_mode);
	fprintf(fs, "2005 %i\n", iblist[i]->scrollbar_side);
	fprintf(fs, "2006 %i\n", iblist[i]->arrow_side);
	fprintf(fs, "2007 %i\n", iblist[i]->auto_resize);
	fprintf(fs, "2008 %i\n", iblist[i]->draw_icon_base);
	fprintf(fs, "2009 %i\n", iblist[i]->scrollbar_hide);
	fprintf(fs, "2010 %i\n", iblist[i]->cover_hide);
	fprintf(fs, "2011 %i\n", iblist[i]->auto_resize_anchor);
	fprintf(fs, "2012 %i\n", iblist[i]->animate);
	fprintf(fs, "1000\n");
     }

   fclose(fs);

   if (iblist)
      Efree(iblist);
}

/*
 * Iconboxes Module
 */

static void
IconboxesSighan(int sig, void *prm)
{
   EWin               *ewin;

   switch (sig)
     {
     case ESIGNAL_CONFIGURE:
	IconboxesConfigLoad();
	IconboxesShow();
	break;
     case ESIGNAL_START:
	/* We should create one if enabled and none existing */
	break;
#if 0
     case ESIGNAL_EXIT:
	IconboxesDestroy();
	break;
#endif
     case ESIGNAL_EWIN_ICONIFY:
	ewin = (EWin *) prm;
	IconboxesEwinIconify(ewin);
	break;
     case ESIGNAL_EWIN_DEICONIFY:
	ewin = (EWin *) prm;
	RemoveMiniIcon(ewin);
	IconboxesEwinDeIconify(ewin);
	break;
     case ESIGNAL_EWIN_DESTROY:
	ewin = (EWin *) prm;
	if (ewin->iconified > 0)
	   RemoveMiniIcon(ewin);
	break;
     case ESIGNAL_EWIN_CHANGE_ICON:
	ewin = (EWin *) prm;
	if (ewin->iconified)
	   IconboxesUpdateEwinIcon(ewin, 1);
	break;
     }
}

static void
IconboxesConfigure(const char *params)
{
   Iconbox            *ib;

   if (!params || !params[0])
      params = "DEFAULT";

   ib = FindItem(params, 0, LIST_FINDBY_NAME, LIST_TYPE_ICONBOX);
   if (ib)
      IconboxConfigure(ib);
}

/*
 * IPC functions
 */
static void
IboxIpc(const char *params, Client * c __UNUSED__)
{
   const char         *p;
   char                cmd[128], prm[128];
   int                 len, num;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %100s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
	/* List iconboxes */
     }
   else if (!strncmp(cmd, "cfg", 3))
     {
	IconboxesConfigure(prm);
     }
   else if (!strncmp(cmd, "new", 3))
     {
	Iconbox            *ib, **ibl;

	if (!prm[0])
	  {
	     ibl = IconboxesList(&num);
	     if (ibl)
		Efree(ibl);
	     Esnprintf(prm, sizeof(prm), "_IB_%i", num);
	  }
	ib = IconboxCreate(prm);
	IconboxShow(ib);
	IconboxesConfigSave();
     }
}

IpcItem             IconboxesIpcArray[] = {
   {
    IboxIpc,
    "iconbox", "ibox",
    "Iconbox functions",
    "  iconbox new <name>   Create new iconbox\n"
    "  iconbox cfg          Configure iconboxes\n"}
};
#define N_IPC_FUNCS (sizeof(IconboxesIpcArray)/sizeof(IpcItem))

#if 0
/*
 * Configuration items
 */
static const CfgItem IconboxesCfgItems[] = {
   CFG_ITEM_BOOL(Conf.iconboxes, enable, 1),
};
#define N_CFG_ITEMS (sizeof(IconboxesCfgItems)/sizeof(CfgItem))
#endif

/*
 * Module descriptor
 */
EModule             ModIconboxes = {
   "iconboxes", "ibox",
   IconboxesSighan,
   {N_IPC_FUNCS, IconboxesIpcArray},
   {0, NULL}
};

/*
 * System tray functions
 */

/* Selection atoms */
static Atom         E_XA_MANAGER = 0;

/* XEmbed atoms */
static Atom         E_XA__XEMBED = 0;

/* Systray atoms */
static Atom         _NET_SYSTEM_TRAY_Sx = 0;
static Atom         _NET_SYSTEM_TRAY_OPCODE = 0;
static Atom         _NET_SYSTEM_TRAY_MESSAGE_DATA = 0;

#define SYSTEM_TRAY_REQUEST_DOCK    0
#define SYSTEM_TRAY_BEGIN_MESSAGE   1
#define SYSTEM_TRAY_CANCEL_MESSAGE  2

#define XEMBED_EMBEDDED_NOTIFY      0

static void
SystrayEventClientMessage(void *ib, XClientMessageEvent * ev)
{
   Window              win;

   Eprintf("ev->type=%ld ev->data.l: %#lx %#lx %#lx %#lx\n", ev->message_type,
	   ev->data.l[0], ev->data.l[1], ev->data.l[2], ev->data.l[3]);

   if (ev->message_type == _NET_SYSTEM_TRAY_OPCODE)
     {
	win = ev->data.l[2];
	if (win == None)
	   goto done;

	if (IconboxWindowAdd(ib, win))
	   goto done;

	/* Should proto be set according to clients _XEMBED_INFO? */
	ecore_x_client_message32_send(win, E_XA__XEMBED, NoEventMask,
				      CurrentTime, XEMBED_EMBEDDED_NOTIFY, 0,
				      ev->window, /*proto */ 1);
     }
   else if (ev->message_type == _NET_SYSTEM_TRAY_MESSAGE_DATA)
     {
	Eprintf("got message\n");
     }
 done:
   ;
}

static void
SystrayEvent(XEvent * ev, void *prm)
{
   Eprintf("SystrayEvent %2d %#lx\n", ev->type, ev->xany.window);

   switch (ev->type)
     {
     case DestroyNotify:
	IconboxWindowDel(prm, ev->xdestroywindow.window);
	break;
     case ClientMessage:
	SystrayEventClientMessage(prm, &(ev->xclient));
	break;
     }
}

static void
SystrayInit(Iconbox * obj, Window win, int screen)
{
   char                buf[32];

   Esnprintf(buf, sizeof(buf), "_NET_SYSTEM_TRAY_S%d", screen);

   E_XA_MANAGER = XInternAtom(disp, "MANAGER", False);
   E_XA__XEMBED = XInternAtom(disp, "_XEMBED", False);
   _NET_SYSTEM_TRAY_Sx = XInternAtom(disp, buf, False);
   _NET_SYSTEM_TRAY_OPCODE =
      XInternAtom(disp, "_NET_SYSTEM_TRAY_OPCODE", False);
   _NET_SYSTEM_TRAY_MESSAGE_DATA =
      XInternAtom(disp, "_NET_SYSTEM_TRAY_MESSAGE_DATA", False);

   /* Acquire selection */
   XSetSelectionOwner(disp, _NET_SYSTEM_TRAY_Sx, win, CurrentTime);

   if (XGetSelectionOwner(disp, _NET_SYSTEM_TRAY_Sx) != win)
     {
	Eprintf("Failed to acquire selection %s\n", buf);
	return;
     }

   Eprintf("Window %#lx is now system tray\n", win);

   ESelectInput(win, SubstructureRedirectMask | ResizeRedirectMask |
		SubstructureNotifyMask);
   EventCallbackRegister(win, 0, SystrayEvent, obj);

   ecore_x_client_message32_send(VRoot.win, E_XA_MANAGER, StructureNotifyMask,
				 CurrentTime, _NET_SYSTEM_TRAY_Sx, win, 0, 0);
}
