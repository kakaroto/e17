/*
 * Copyright (C) 2000-2005 Carsten Haitzler, Geoff Harrison and various contributors
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
#include <X11/keysym.h>

#define DEBUG_MENU_EVENTS 0

struct
{
   EWin               *context_ewin;
   int                 current_depth;
   Menu               *list[256];
   char                clicked;
   char                just_shown;
   EObj               *cover_win;
} Mode_menus;

struct _menustyle
{
   char               *name;
   TextClass          *tclass;
   ImageClass         *bg_iclass;
   ImageClass         *item_iclass;
   ImageClass         *sub_iclass;
   char                use_item_bg;
   char                iconpos;
   int                 maxx;
   int                 maxy;
   char               *border_name;
   unsigned int        ref_count;
};

struct _menuitem
{
   Menu               *menu;
   ImageClass         *icon_iclass;
   char               *text;
   void               *params;
   Menu               *child;
   char                state;
   PmapMask            pmm[3];
   Window              win;
   Window              icon_win;
   short               icon_w;
   short               icon_h;
   short               text_w;
   short               text_h;
   short               text_x;
   short               text_y;
};

struct _menu
{
   char               *name;
   char               *title;
   MenuStyle          *style;
   EWin               *ewin;
   int                 w, h;
   int                 num;
   MenuItem          **items;
   Window              win;
   PmapMask            pmm;
   char                shown;
   char                stuck;
   char                internal;	/* Don't destroy when reloading */
   char                redraw;
   Menu               *parent;
   MenuItem           *sel_item;
   time_t              last_change;
   void               *data;
   unsigned int        ref_count;
};

#define MENU_ITEM_EVENT_MASK \
	KeyPressMask | KeyReleaseMask | \
	ButtonPressMask | ButtonReleaseMask | \
	EnterWindowMask | LeaveWindowMask	/* | PointerMotionMask */

static void         MenuRedraw(Menu * m);
static void         MenuRealize(Menu * m);
static void         MenuActivateItem(Menu * m, MenuItem * mi);
static void         MenuDrawItem(Menu * m, MenuItem * mi, char shape);

static void         MenuHandleEvents(XEvent * ev, void *m);
static void         MenuItemHandleEvents(XEvent * ev, void *mi);
static void         MenuMaskerHandleEvents(XEvent * ev, void *prm);

static void         MenusHide(void);

static Menu        *active_menu = NULL;
static MenuItem    *active_item = NULL;

static Menu        *
FindMenu(Window win)
{
   Menu               *menu = NULL;
   Menu              **menus;
   int                 i, num;

   menus = (Menu **) ListItemType(&num, LIST_TYPE_MENU);
   for (i = 0; i < num; i++)
     {
	if (menus[i]->win != win)
	   continue;
	menu = menus[i];
	break;
     }
   if (menus)
      Efree(menus);

   return menu;
}

void
MenuHide(Menu * m)
{
   EWin               *ewin;

   MenuActivateItem(m, NULL);

   ewin = m->ewin;
   if (ewin)
     {
	EUnmapWindow(m->win);
	EReparentWindow(m->win, VRoot.win, ewin->client.x, ewin->client.y);
	HideEwin(ewin);
     }
   m->ewin = NULL;

   m->stuck = 0;
   m->shown = 0;
}

static void
MenuEwinMoveResize(EWin * ewin, int resize __UNUSED__)
{
   Menu               *m = ewin->data;

   if (!m || Mode.mode != MODE_NONE)
      return;

   if (TransparencyUpdateNeeded())
      m->redraw = 1;

   if ((!m->style->use_item_bg && m->pmm.pmap == 0) || m->redraw)
      MenuRedraw(m);
}

static void
MenuEwinClose(EWin * ewin)
{
   if ((Menu *) (ewin->data) == active_menu)
     {
	GrabKeyboardRelease();
	active_menu = NULL;
	active_item = NULL;
     }

   ewin->data = NULL;
}

static void
MenuEwinInit(EWin * ewin, void *ptr)
{
   Menu               *m = ptr;

   ewin->data = ptr;

   ewin->MoveResize = MenuEwinMoveResize;
   ewin->Close = MenuEwinClose;

   ewin->props.skip_ext_task = 1;
   ewin->props.skip_ext_pager = 1;
   ewin->props.no_actions = 1;
   ewin->props.skip_focuslist = 1;
   ewin->props.skip_winlist = 1;
   ewin->props.never_focus = 1;
   ewin->client.grav = StaticGravity;

   ewin->client.width.min = ewin->client.width.max = ewin->client.w = m->w;
   ewin->client.height.min = ewin->client.height.max = ewin->client.h = m->h;
   ewin->client.no_resize_h = ewin->client.no_resize_v = 1;

   EoSetSticky(ewin, 1);
   EoSetLayer(ewin, 3);
   EoSetFloating(ewin, 1);
   ewin->ewmh.opacity = OpacityExt(Conf.menus.opacity);
}

static void         MenuShowMasker(Menu * m);

static void
MenuShow(Menu * m, char noshow)
{
   EWin               *ewin;
   int                 x, y;
   int                 wx = 0, wy = 0;	/* wx, wy added to stop menus */
   int                 w, h, mw, mh;	/* from appearing offscreen */
   int                 head_num = 0;

   if ((m->num <= 0) || (!m->style))
      return;

   if (m->shown)
      return;

   if (m->stuck)
      return;

   if (!m->win)
      MenuRealize(m);

   ewin = m->ewin;
   if (ewin)
     {
#if 0				/* FIXME - Why? */
	if ((Mode.button) &&
	    FindItem(Mode.button, 0, LIST_FINDBY_POINTER, LIST_TYPE_BUTTON))
	  {
	     ButtonDrawWithState(Mode.button, STATE_NORMAL);
	  }
#endif
#if 0				/* ??? */
	RaiseEwin(ewin);
	ShowEwin(ewin);
	return;
#else
	MenuHide(m);
#endif
     }

   EGetGeometry(m->items[0]->win, NULL, &x, &y, &w, &h, NULL, NULL);
   mw = m->w;
   mh = m->h;

   wx = 0;
   wy = 0;
   if (Conf.menus.onscreen)
     {
	Border             *b;

	b = FindItem(m->style->border_name, 0, LIST_FINDBY_NAME,
		     LIST_TYPE_BORDER);
	if (b)
	  {
	     int                 width;
	     int                 height;
	     int                 x_origin;
	     int                 y_origin;

	     head_num =
		GetPointerScreenGeometry(&x_origin, &y_origin, &width, &height);

	     if (Mode.x - x - ((int)mw / 2) > x_origin + width)
		wx = x_origin + b->border.left;
	     else if (Mode.x + ((int)mw / 2) > x_origin + width)
		wx = x_origin + width - mw - b->border.right;
	     else
		wx = Mode.x - x - (w / 2);

	     if ((wx - ((int)w / 2)) < x_origin)
		wx = x_origin + b->border.left;

	     if (Mode.y + (int)mh > VRoot.h)
		wy = (y_origin + height) - mh - b->border.bottom;
	     else
		wy = Mode.y - y - (h / 2);

	     if ((wy - ((int)h / 2) - b->border.top) < y_origin)
		wy = y_origin + b->border.top;
	  }
	else
	  {
	     /* We should never get here */
	     wx = Mode.x - x - (w / 2);
	     wy = Mode.y - y - (h / 2);
	  }
     }
   else
     {
	wx = Mode.x - x - (w / 2);
	wy = Mode.y - y - (h / 2);
     }

   EMoveWindow(m->win, wx, wy);

   ewin = AddInternalToFamily(m->win, m->style->border_name, EWIN_TYPE_MENU, m,
			      MenuEwinInit);
   m->ewin = ewin;
   if (ewin)
     {
	ewin->client.event_mask |= KeyPressMask;
	ESelectInput(m->win, ewin->client.event_mask);

	ewin->head = head_num;

	MoveEwinToDesktop(ewin, EoGetDesk(ewin));
	ResizeEwin(ewin, ewin->client.w, ewin->client.h);

	if (Conf.menus.animate)
	   EwinInstantShade(ewin, 0);

	if (!noshow)
	  {
	     ICCCM_Cmap(NULL);
	     EwinFloatAt(ewin, EoGetX(ewin), EoGetY(ewin));
	     ShowEwin(ewin);
	     if (Conf.menus.animate)
		EwinUnShade(ewin);
	  }
     }

   m->stuck = 0;

   if (!FindMenu(m->win))
      AddItem(m, m->name, m->win, LIST_TYPE_MENU);

   Mode_menus.just_shown = 1;

   m->shown = 1;
   if (Mode_menus.current_depth == 0)
     {
	Mode_menus.context_ewin = GetContextEwin();
#if 0
	Eprintf("Mode_menus.context_ewin set %s\n",
		EwinGetName(Mode_menus.context_ewin));
#endif
	ESync();
#if 1				/* ??? */
	Mode_menus.list[0] = m;
	Mode_menus.current_depth = 1;
	MenuShowMasker(m);
#endif
	TooltipsEnable(0);
	GrabKeyboardSet(m->win);
     }
   m->ref_count++;
}

static void
MenuStyleSetName(MenuStyle * ms, const char *name)
{
   if (ms->name)
      Efree(ms->name);
   ms->name = Estrdup(name);
}

static MenuStyle   *
MenuStyleCreate(const char *name)
{
   MenuStyle          *ms;

   ms = Ecalloc(1, sizeof(MenuStyle));
   ms->iconpos = ICON_LEFT;
   MenuStyleSetName(ms, name);

   return ms;
}

MenuItem           *
MenuItemCreate(const char *text, ImageClass * iclass,
	       const char *action_params, Menu * child)
{
   MenuItem           *mi;

   mi = Ecalloc(1, sizeof(MenuItem));

   mi->icon_iclass = iclass;
   if (iclass)
      iclass->ref_count++;

   mi->text = (text) ? Estrdup((text[0]) ? _(text) : "?!?") : NULL;
   mi->params = Estrdup(action_params);
   mi->child = child;
   mi->state = STATE_NORMAL;

   return mi;
}

void
MenuSetName(Menu * m, const char *name)
{
   if (m->name)
      Efree(m->name);
   m->name = Estrdup(name);
   AddItem(m, m->name, m->win, LIST_TYPE_MENU);
}

void
MenuSetTitle(Menu * m, const char *title)
{
   if (m->title)
      Efree(m->title);
   m->title = (title) ? Estrdup(_(title)) : NULL;
}

void
MenuSetData(Menu * m, char *data)
{
   if (m->data)
      Efree(m->data);
   m->data = data;
}

void
MenuSetTimestamp(Menu * m, time_t t)
{
   m->last_change = t;
}

const char         *
MenuGetName(const Menu * m)
{
   return m->name;
}

const char         *
MenuGetData(const Menu * m)
{
   return m->data;
}

time_t
MenuGetTimestamp(const Menu * m)
{
   return m->last_change;
}

void
MenuSetStyle(Menu * m, MenuStyle * ms)
{
   if (m->style)
      m->style->ref_count--;
   if (!ms && m->parent)
      ms = m->parent->style;
   if (!ms)
      ms = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
   m->style = ms;
   if (ms)
      ms->ref_count++;
}

int
MenuIsNotEmpty(const Menu * m)
{
   return m && (m->num > 0) && m->style;
}

Menu               *
MenuCreate(const char *name, const char *title, Menu * parent, MenuStyle * ms)
{
   Menu               *m;

   m = Ecalloc(1, sizeof(Menu));
   if (!m)
      return m;

   m->parent = parent;
   MenuSetName(m, name);
   MenuSetTitle(m, title);
   MenuSetStyle(m, ms);

   return m;
}

void
MenuDestroy(Menu * m)
{
   int                 i, j;
   char                s[4096];

   if (!m)
      return;

   MenuHide(m);

   if (m->win)
      EDestroyWindow(m->win);

   Esnprintf(s, sizeof(s), "__.%s", m->name);
   RemoveTimerEvent(s);
   RemoveItemByPtr(m, LIST_TYPE_MENU);
   if (m->name)
      Efree(m->name);
   if (m->title)
      Efree(m->title);

   for (i = 0; i < m->num; i++)
     {
	if (m->items[i])
	  {
	     if (m->items[i]->child)
	       {
		  if (FindItem(m->items[i]->child, 0, LIST_FINDBY_POINTER,
			       LIST_TYPE_MENU))
		     MenuDestroy(m->items[i]->child);
	       }
	     if (m->items[i]->text)
		Efree(m->items[i]->text);
	     if (m->items[i]->params)
		Efree(m->items[i]->params);
	     for (j = 0; j < 3; j++)
		FreePmapMask(&(m->items[i]->pmm[j]));
	     if (m->items[i]->icon_iclass)
		m->items[i]->icon_iclass->ref_count--;
	     if (m->items[i])
		Efree(m->items[i]);
	  }
     }

   if (m->items)
      Efree(m->items);
   if (m->data)
      Efree(m->data);
   FreePmapMask(&m->pmm);

   Efree(m);
}

/* NB - this doesnt free imageclasses if we created them for the menu
 * FIXME: so it will leak if we create new imageclasses and stop using
 * old ones for menu icons. we need to add some ref counting in menu icon
 * imageclasses to knw to free them when not used
 */
void
MenuEmpty(Menu * m)
{
   int                 i, j;

   for (i = 0; i < m->num; i++)
     {
	if (m->items[i])
	  {
	     if (m->items[i]->child)
		MenuDestroy(m->items[i]->child);
	     if (m->items[i]->text)
		Efree(m->items[i]->text);
	     if (m->items[i]->params)
		Efree(m->items[i]->params);
	     for (j = 0; j < 3; j++)
		FreePmapMask(&(m->items[i]->pmm[j]));
	     if (m->items[i]->win)
		EDestroyWindow(m->items[i]->win);
	     if (m->items[i])
		Efree(m->items[i]);
	  }
     }
   if (m->items)
      Efree(m->items);
   m->items = NULL;
   m->num = 0;
}

void
MenuRepack(Menu * m)
{
   EWin               *ewin;
   unsigned int        w, h;

   m->redraw = 1;
   if (m->win)
      MenuRealize(m);

   ewin = m->ewin;
   if (ewin)
     {
	w = m->w;
	h = m->h;
	ewin->client.height.min = h;
	ewin->client.height.max = h;
	ewin->client.width.min = w;
	ewin->client.width.max = w;
	ResizeEwin(ewin, w, h);
	RaiseEwin(ewin);
     }
}

void
MenuAddItem(Menu * m, MenuItem * item)
{
   m->num++;
   m->items = Erealloc(m->items, sizeof(MenuItem *) * m->num);
   m->items[m->num - 1] = item;
}

static void
MenuRealize(Menu * m)
{
   int                 i, maxh, maxw, nmaxy;
   int                 maxx1, maxx2, w, h, x, y, r, mmw, mmh;
   int                 iw, ih;
   Imlib_Image        *im;
   char                has_i, has_s;

   if (!m->style)
      return;

   if (!m->win)
     {
	m->win = ECreateWindow(VRoot.win, 0, 0, 1, 1, 0);
	EventCallbackRegister(m->win, 0, MenuHandleEvents, m);
     }

   if (m->title)
     {
	HintsSetWindowName(m->win, m->title);
     }

   maxh = maxw = 0;
   maxx1 = 0;
   maxx2 = 0;
   has_i = 0;
   has_s = 0;

   for (i = 0; i < m->num; i++)
     {
	m->items[i]->menu = m;

	if (m->items[i]->child)
	   has_s = 1;
	else
	   has_i = 1;

	m->items[i]->win = ECreateWindow(m->win, 0, 0, 1, 1, 0);
	EventCallbackRegister(m->items[i]->win, 0, MenuItemHandleEvents,
			      m->items[i]);
	ESelectInput(m->items[i]->win, MENU_ITEM_EVENT_MASK);
	EMapWindow(m->items[i]->win);

	if ((m->style) && (m->style->tclass) && (m->items[i]->text))
	  {
	     TextSize(m->style->tclass, 0, 0, 0, m->items[i]->text, &w, &h, 17);
	     if (h > maxh)
		maxh = h;
	     if (w > maxx1)
		maxx1 = w;
	     m->items[i]->text_w = w;
	     m->items[i]->text_h = h;
	  }
	if (m->items[i]->icon_iclass)
	  {
	     im = ELoadImage(m->items[i]->icon_iclass->norm.normal->im_file);
	     if (im)
	       {
		  imlib_context_set_image(im);
		  m->items[i]->icon_win =
		     ECreateWindow(m->items[i]->win, 0, 0,
				   imlib_image_get_width(),
				   imlib_image_get_height(), 0);
		  EMapWindow(m->items[i]->icon_win);
		  m->items[i]->icon_w = imlib_image_get_width();
		  m->items[i]->icon_h = imlib_image_get_height();
		  if (imlib_image_get_height() > maxh)
		     maxh = imlib_image_get_height();
		  if (imlib_image_get_width() > maxx2)
		     maxx2 = imlib_image_get_width();
		  imlib_free_image();
	       }
	     else
		m->items[i]->icon_iclass = NULL;
	  }
     }

   if (((has_i) && (has_s)) || ((!has_i) && (!has_s)))
     {
	if (m->style->item_iclass->padding.top >
	    m->style->sub_iclass->padding.top)
	   maxh += m->style->item_iclass->padding.top;
	else
	   maxh += m->style->sub_iclass->padding.top;
	if (m->style->item_iclass->padding.bottom >
	    m->style->sub_iclass->padding.bottom)
	   maxh += m->style->item_iclass->padding.bottom;
	else
	   maxh += m->style->sub_iclass->padding.bottom;
	maxw = maxx1 + maxx2;
	if (m->style->item_iclass->padding.left >
	    m->style->sub_iclass->padding.left)
	   maxw += m->style->item_iclass->padding.left;
	else
	   maxw += m->style->sub_iclass->padding.left;
	if (m->style->item_iclass->padding.right >
	    m->style->sub_iclass->padding.right)
	   maxw += m->style->item_iclass->padding.right;
	else
	   maxw += m->style->sub_iclass->padding.right;
     }
   else if (has_i)
     {
	maxh += m->style->item_iclass->padding.top;
	maxh += m->style->item_iclass->padding.bottom;
	maxw = maxx1 + maxx2;
	maxw += m->style->item_iclass->padding.left;
	maxw += m->style->item_iclass->padding.right;
     }
   else if (has_s)
     {
	maxh += m->style->sub_iclass->padding.top;
	maxh += m->style->sub_iclass->padding.bottom;
	maxw = maxx1 + maxx2;
	maxw += m->style->sub_iclass->padding.left;
	maxw += m->style->sub_iclass->padding.right;
     }

   mmw = 0;
   mmh = 0;

   nmaxy = 3 * VRoot.h / (4 * maxh + 1);
   if (m->style->maxy && nmaxy > m->style->maxy)
      nmaxy = m->style->maxy;

   r = 0;
   x = 0;
   y = 0;
   for (i = 0; i < m->num; i++)
     {
	if (r == 0 && (m->style->bg_iclass) && (!m->style->use_item_bg))
	  {
	     x += m->style->bg_iclass->padding.left;
	     y += m->style->bg_iclass->padding.top;
	  }
	EMoveResizeWindow(m->items[i]->win, x, y, maxw, maxh);
	if (m->style->iconpos == ICON_LEFT)
	  {
	     m->items[i]->text_x = m->style->item_iclass->padding.left + maxx2;
	     m->items[i]->text_w = maxx1;
	     m->items[i]->text_y = (maxh - m->items[i]->text_h) / 2;
	     if (m->items[i]->icon_win)
		EMoveWindow(m->items[i]->icon_win,
			    m->style->item_iclass->padding.left +
			    ((maxx2 - m->items[i]->icon_w) / 2),
			    ((maxh - m->items[i]->icon_h) / 2));
	  }
	else
	  {
	     m->items[i]->text_x = m->style->item_iclass->padding.left;
	     m->items[i]->text_w = maxx1;
	     m->items[i]->text_y = (maxh - m->items[i]->text_h) / 2;
	     if (m->items[i]->icon_win)
		EMoveWindow(m->items[i]->icon_win,
			    maxw - m->style->item_iclass->padding.right -
			    maxx2 + ((maxx2 - w) / 2), ((maxh - h) / 2));
	  }
	if (m->items[i]->icon_iclass)
	  {
	     iw = 0;
	     ih = 0;
	     EGetGeometry(m->items[i]->icon_win, NULL, NULL, NULL, &iw, &ih,
			  NULL, NULL);
	     ImageclassApply(m->items[i]->icon_iclass, m->items[i]->icon_win,
			     iw, ih, 0, 0, STATE_NORMAL, 0, ST_MENU_ITEM);
	  }
	if (x + maxw > mmw)
	   mmw = x + maxw;
	if (y + maxh > mmh)
	   mmh = y + maxh;
	if ((m->style->maxx) || (nmaxy))
	  {
	     if (nmaxy)
	       {
		  y += maxh;
		  r++;
		  if (r >= nmaxy)
		    {
		       r = 0;
		       x += maxw;
		       y = 0;
		    }
	       }
	     else
	       {
		  x += maxw;
		  r++;
		  if (r >= m->style->maxx)
		    {
		       r = 0;
		       y += maxh;
		       x = 0;
		    }
	       }
	  }
	else
	   y += maxh;
     }

   if ((m->style->bg_iclass) && (!m->style->use_item_bg))
     {
	mmw += m->style->bg_iclass->padding.right;
	mmh += m->style->bg_iclass->padding.bottom;
     }

   m->redraw = 1;
   m->w = mmw;
   m->h = mmh;
   EResizeWindow(m->win, mmw, mmh);
}

static void
MenuRedraw(Menu * m)
{
   int                 i, j, w, h;

   if (m->redraw)
     {
	for (i = 0; i < m->num; i++)
	  {
	     for (j = 0; j < 3; j++)
		FreePmapMask(&(m->items[i]->pmm[j]));

	  }
	m->redraw = 0;
     }

   if (!m->style->use_item_bg)
     {
	w = m->w;
	h = m->h;
	FreePmapMask(&m->pmm);
	ImageclassApplyCopy(m->style->bg_iclass, m->win, w, h, 0, 0,
			    STATE_NORMAL, &m->pmm, 1, ST_MENU);
	ESetWindowBackgroundPixmap(m->win, m->pmm.pmap);
	EShapeCombineMask(m->win, ShapeBounding, 0, 0, m->pmm.mask, ShapeSet);
	EClearWindow(m->win);
	for (i = 0; i < m->num; i++)
	   MenuDrawItem(m, m->items[i], 0);
     }
   else
     {
	for (i = 0; i < m->num; i++)
	   MenuDrawItem(m, m->items[i], 0);
	EShapePropagate(m->win);
     }
}

static void
MenuDrawItem(Menu * m, MenuItem * mi, char shape)
{
   PmapMask           *mi_pmm;

   mi_pmm = &(mi->pmm[(int)(mi->state)]);

   if (!mi_pmm->pmap)
     {
	GC                  gc;
	int                 x, y, w, h;
	int                 item_type;
	ImageClass         *ic;

	EGetGeometry(mi->win, NULL, &x, &y, &w, &h, NULL, NULL);

	mi_pmm->type = 0;
	mi_pmm->pmap = ECreatePixmap(mi->win, w, h, VRoot.depth);
	mi_pmm->mask = None;

	ic = (mi->child) ? m->style->sub_iclass : m->style->item_iclass;
	item_type = (mi->state != STATE_NORMAL) ? ST_MENU_ITEM : ST_MENU;

	if (!m->style->use_item_bg)
	  {
	     gc = ECreateGC(m->pmm.pmap, 0, NULL);
	     XCopyArea(disp, m->pmm.pmap, mi_pmm->pmap, gc, x, y, w, h, 0, 0);
	     if ((mi->state != STATE_NORMAL) || (mi->child))
	       {
		  PmapMask            pmm;

		  ImageclassApplyCopy(ic, mi->win, w, h, 0, 0, mi->state, &pmm,
				      1, item_type);
		  if (pmm.mask)
		    {
		       XSetClipMask(disp, gc, pmm.mask);
		       XSetClipOrigin(disp, gc, 0, 0);
		    }
		  XCopyArea(disp, pmm.pmap, mi_pmm->pmap, gc, 0, 0, w, h, 0, 0);
		  FreePmapMask(&pmm);
	       }
	     EFreeGC(gc);
	  }
	else
	  {
	     ImageclassApplyCopy(ic, mi_pmm->pmap, w, h, 0, 0, mi->state,
				 mi_pmm, 1, item_type);
	  }

	if (mi->text)
	  {
	     TextDraw(m->style->tclass, mi_pmm->pmap, 0, 0, mi->state,
		      mi->text, mi->text_x, mi->text_y, mi->text_w, mi->text_h,
		      17, m->style->tclass->justification);
	  }
     }

   ESetWindowBackgroundPixmap(mi->win, mi_pmm->pmap);
   EShapeCombineMask(mi->win, ShapeBounding, 0, 0, mi_pmm->mask, ShapeSet);
   EClearWindow(mi->win);

   if ((shape) && (m->style->use_item_bg))
      EShapePropagate(m->win);

   if (mi->state == STATE_HILITED)
     {
	active_item = mi;
	if (active_menu != m)
	  {
	     active_menu = m;
	     GrabKeyboardRelease();
	     GrabKeyboardSet(m->win);
	  }
     }
}

static void
MenuShowMasker(Menu * m __UNUSED__)
{
   EObj               *eo = Mode_menus.cover_win;

   if (!eo)
     {
	eo = EobjWindowCreate(EOBJ_TYPE_EVENT, 0, 0, VRoot.w, VRoot.h, 0,
			      "Masker");
	if (!eo)
	   return;

	EobjSetFloating(eo, 1);
	EobjSetLayer(eo, 2);
	EobjListStackLower(eo);
	ESelectInput(eo->win, ButtonPressMask | ButtonReleaseMask |
		     EnterWindowMask | LeaveWindowMask);
	EventCallbackRegister(eo->win, 0, MenuMaskerHandleEvents, NULL);

	Mode_menus.cover_win = eo;
     }

   EobjMap(eo, 0);
}

static void
MenuHideMasker(void)
{
   EObj               *eo = Mode_menus.cover_win;

   if (!eo)
      return;

   EventCallbackUnregister(eo->win, 0, MenuMaskerHandleEvents, NULL);
   EobjWindowDestroy(eo);
   Mode_menus.cover_win = NULL;
}

static void
MenusDestroyLoaded(void)
{
   Menu               *menu;
   Menu              **menus;
   int                 i, num, found_one;

   /* Free all menustyles first (gulp) */
   do
     {
	found_one = 0;
	menus = (Menu **) ListItemType(&num, LIST_TYPE_MENU);
	for (i = 0; i < num; i++)
	  {
	     menu = menus[i];
	     if (menu->internal)
		continue;

	     MenuDestroy(menu);
	     /* Destroying a menu may result in sub-menus being
	      * destroyed too, so we have to re-find all menus
	      * afterwards. Inefficient yes, but it works...
	      */
	     found_one = 1;
	     break;
	  }
	if (menus)
	   Efree(menus);
     }
   while (found_one);
}

/*
 * Internal menus
 */

static Menu        *
RefreshInternalMenu(Menu * m, MenuStyle * ms,
		    Menu * (mcf) (const char *xxx, MenuStyle * ms))
{
   char                was = 0;
   int                 lx = 0, ly = 0;
   EWin               *ewin;

   if (m)
     {
	ewin = m->ewin;
	if ((m->win) && (ewin))
	  {
	     lx = EoGetX(ewin);
	     ly = EoGetY(ewin);
	     was = 1;
	  }
	MenuDestroy(m);
	m = NULL;
     }

   if (!ms)
      return NULL;

   m = mcf("MENU", ms);
   if ((was) && (m))
     {
	m->internal = 1;
	MenuShow(m, 1);
	ewin = m->ewin;
	if (ewin)
	  {
	     MoveEwin(ewin, lx, ly);
	     ShowEwin(ewin);
	  }
     }

   return m;
}

static void
MenusShowNamed(const char *name)
{
   Menu               *m;

   /* Hide any menus currently up */
   if (MenusActive())
      MenusHide();

   m = FindItem(name, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU);
   if (!m)
      return;

   if (!m->ewin)		/* Don't show if already shown */
      MenuShow(m, 0);
}

void
ShowInternalMenu(Menu ** pm, MenuStyle ** pms, const char *style,
		 Menu * (mcf) (const char *name, MenuStyle * ms))
{
   Menu               *m = *pm;
   MenuStyle          *ms = *pms;

   /* Hide any menus currently up */
   if (MenusActive())
      MenusHide();

   if (!ms)
     {
	ms = FindItem(style, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
	if (!ms)
	   ms = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
	if (!ms)
	   return;
	*pms = ms;
     }

   *pm = m = RefreshInternalMenu(m, ms, mcf);
   if (!m)
      return;

   if (!m->ewin)
      MenuShow(m, 0);
}

int
MenusActive(void)
{
   return Mode_menus.current_depth;
}

static void
MenusHide(void)
{
   int                 i;

   while (RemoveTimerEvent("SUBMENU_SHOW"))
      ;

   for (i = 0; i < Mode_menus.current_depth; i++)
     {
	if (!Mode_menus.list[i]->stuck)
	   MenuHide(Mode_menus.list[i]);
     }
   MenuHideMasker();
   Mode_menus.current_depth = 0;
   Mode_menus.clicked = 0;
   TooltipsEnable(1);

#if 0
   /* If all done properly this shouldn't be necessary... */
   GrabKeyboardRelease();
   active_menu = NULL;
   active_item = NULL;
#endif
}

/*
 * Menu event handlers
 */

static MenuItem    *
MenuFindNextItem(Menu * m, MenuItem * mi, int inc)
{
   int                 i;

   if (mi == NULL)
     {
	if (m->num > 0)
	   return m->items[0];
	else
	   return NULL;
     }

   for (i = 0; i < m->num; i++)
      if (m->items[i] == mi)
	{
	   i = (i + inc + m->num) % m->num;
	   return m->items[i];
	}

   return NULL;
}

static MenuItem    *
MenuFindParentItem(Menu * m)
{
   int                 i;
   Menu               *mp;

   mp = m->parent;
   if (mp == NULL)
      return NULL;

   for (i = 0; i < mp->num; i++)
      if (mp->items[i]->child == m)
	 return mp->items[i];

   return NULL;
}

static              KeySym
MenuKeyPressConversion(KeySym key)
{
   if (key == Conf.menus.key.left)
      return XK_Left;
   if (key == Conf.menus.key.right)
      return XK_Right;
   if (key == Conf.menus.key.up)
      return XK_Up;
   if (key == Conf.menus.key.down)
      return XK_Down;
   if (key == Conf.menus.key.escape)
      return XK_Escape;
   if (key == Conf.menus.key.ret)
      return XK_Return;

   /* The key does not correspond to any set, use the default behavior 
    * associated to the key */
   return key;
}

static void
MenuEventKeyPress(Menu * m, XEvent * ev)
{
   KeySym              key;
   MenuItem           *mi;
   EWin               *ewin;

   mi = NULL;
   if (active_menu)
     {
	m = active_menu;
	mi = active_item;
     }

   /* NB! m != NULL */

   key = XLookupKeysym(&ev->xkey, 0);
   switch (MenuKeyPressConversion(key))
     {
     case XK_Escape:
	MenusHide();
	break;
     case XK_Down:
      check_next:
	mi = MenuFindNextItem(m, mi, 1);
	goto check_activate;
     case XK_Up:
	mi = MenuFindNextItem(m, mi, -1);
	goto check_activate;
     case XK_Left:
	mi = MenuFindParentItem(m);
	m = m->parent;
	goto check_menu;
     case XK_Right:
	if (mi == NULL)
	   goto check_next;
	m = mi->child;
	if (!m || m->num <= 0)
	   break;
	ewin = m->ewin;
	if (ewin == NULL || !EwinIsMapped(ewin))
	   break;
	mi = m->items[0];
	goto check_menu;
      check_menu:
	if (!m)
	   break;
	goto check_activate;
      check_activate:
	if (!mi)
	   break;
	if (active_menu && active_item && active_menu != m)
	   MenuActivateItem(active_menu, NULL);
	MenuActivateItem(m, mi);
	break;
     case XK_Return:
	if (!mi)
	   break;
	if (!mi->params)
	   break;
	MenusHide();
	EobjsRepaint();
	EFunc(Mode_menus.context_ewin, mi->params);
	break;
     }
}

static void
MenuItemEventMouseDown(MenuItem * mi, XEvent * ev __UNUSED__)
{
   Menu               *m;
   EWin               *ewin;

   Mode_menus.just_shown = 0;

   m = mi->menu;
   mi->state = STATE_CLICKED;
   MenuDrawItem(m, mi, 1);

   if (mi->child && mi->child->shown == 0)
     {
	int                 mx, my, mw, mh;
	EWin               *ewin2;

	ewin = m->ewin;
	if (ewin)
	  {
	     EGetGeometry(mi->win, NULL, &mx, &my, &mw, &mh, NULL, NULL);
#if 1				/* Whatgoesonhere ??? */
	     MenuShow(mi->child, 1);
	     ewin2 = mi->child->ewin;
	     if (ewin2)
	       {
		  MoveEwin(ewin2,
			   EoGetX(ewin) + ewin->border->border.left + mx + mw,
			   EoGetY(ewin) + ewin->border->border.top + my -
			   ewin2->border->border.top);
		  RaiseEwin(ewin2);
		  ShowEwin(ewin2);
		  if (Conf.menus.animate)
		     EwinUnShade(ewin2);
		  Mode_menus.list[Mode_menus.current_depth++] = mi->child;
	       }
#else
	     ewin2 = mi->child->ewin;
	     if (!ewin2)
		MenuShow(mi->child, 1);
#endif
	  }
     }

   return;
}

static void
MenuItemEventMouseUp(MenuItem * mi, XEvent * ev __UNUSED__)
{
   Menu               *m;

   if (Mode_menus.just_shown)
     {
	Mode_menus.just_shown = 0;
	return;
     }

   m = mi->menu;

   if ((m) && (mi->state))
     {
	mi->state = STATE_HILITED;
	MenuDrawItem(m, mi, 1);
	if ((mi->params) /* && (!Mode_menus.just_shown) */ )
	  {
	     MenusHide();
	     EobjsRepaint();
	     EFunc(Mode_menus.context_ewin, mi->params);
	     return;
	  }
     }
}

#if 0				/* Was in HandleMotion() */
void
MenusHandleMotion(void)
{
#define SCROLL_RATIO 2/3
   if ((MenusActive() || (Mode_menus.clicked)))
     {
	int                 i, offx = 0, offy = 0, xdist = 0, ydist = 0;
	EWin               *ewin;
	EWin               *menus[256];
	int                 fx[256];
	int                 fy[256];
	int                 tx[256];
	int                 ty[256];
	static int          menu_scroll_dist = 4;
	int                 my_width, my_height, x_org, y_org, head_num = 0;

	head_num = ScreenGetGeometry(Mode.x, Mode.y, &x_org, &y_org,
				     &my_width, &my_height);

	if (Mode.x > ((x_org + my_width) - (menu_scroll_dist + 1)))
	  {
	     xdist = -(menu_scroll_dist + (Mode.x - (x_org + my_width)));
	  }
	else if (Mode.x < (menu_scroll_dist + x_org))
	  {
	     xdist = x_org + menu_scroll_dist - (Mode.x);
	  }

	if (Mode.y > (VRoot.h - (menu_scroll_dist + 1)))
	  {
	     ydist = -(menu_scroll_dist + (Mode.y - (y_org + my_height)));
	  }
	else if (Mode.y < (menu_scroll_dist + y_org))
	  {
	     ydist = y_org + menu_scroll_dist - (Mode.y);
	  }

	/* That's a hack to avoid unwanted events:
	 * If the user entered the border area, he has to
	 * leave it first, before he can scroll menus again ...
	 */
	if ((xdist != 0) || (ydist != 0) || Mode.doingslide)
	  {
	     /* -10 has no meaning, only makes sure that the if's */
	     /* above can't be fulfilled ... */
	     menu_scroll_dist = -10;
	  }
	else
	  {
	     menu_scroll_dist = 13;
	  }

	if (Mode_menus.current_depth > 0)
	  {
	     int                 x1, y1, x2, y2;

	     x1 = x_org + my_width;
	     x2 = x_org - 1;
	     y1 = y_org + my_height;
	     y2 = y_org - 1;
	     /* work out the minimum and maximum extents of our */
	     /* currently active menus */
	     for (i = 0; i < Mode_menus.current_depth; i++)
	       {
		  if (Mode_menus.list[i])
		    {
		       ewin = Mode_menus.list[i]->ewin;
		       if (ewin)
			 {
			    if (EoGetX(ewin) < x1)
			       x1 = EoGetX(ewin);
			    if (EoGetY(ewin) < y1)
			       y1 = EoGetY(ewin);
			    if ((EoGetX(ewin) + EoGetW(ewin) - 1) > x2)
			       x2 = EoGetX(ewin) + EoGetW(ewin) - 1;
			    if ((EoGetY(ewin) + EoGetH(ewin) - 1) > y2)
			       y2 = EoGetY(ewin) + EoGetH(ewin) - 1;
			 }
		    }
	       }

	     if (xdist < 0)
	       {
		  offx = (x_org + my_width) - x2;
	       }
	     else if (xdist > 0)
	       {
		  offx = x_org - x1;
	       }
	     if (ydist < 0)
	       {
		  offy = (y_org + my_height) - y2;
	       }
	     else if (ydist > 0)
	       {
		  offy = y_org - y1;
	       }

	     if ((xdist < 0) && (offx <= 0))
		xdist = offx;
	     if ((xdist > 0) && (offx >= 0))
		xdist = offx;
	     if ((ydist < 0) && (offy <= 0))
		ydist = offy;
	     if ((ydist > 0) && (offy >= 0))
		ydist = offy;

	     /* only if any active menus are partially off screen then scroll */
	     if ((((xdist > 0) && (x1 < x_org))
		  || ((xdist < 0) && (x2 >= (x_org + my_width))))
		 || (((ydist > 0) && (y1 < y_org))
		     || ((ydist < 0) && (y2 >= (y_org + my_height)))))
	       {
		  /* If we would scroll too far, limit scrolling to 2/3s of screen */
		  if (ydist < -my_width)
		     ydist = -my_width * SCROLL_RATIO;
		  if (ydist > my_width)
		     ydist = my_width * SCROLL_RATIO;

		  if (xdist < -my_height)
		     xdist = -my_height * SCROLL_RATIO;
		  if (xdist > my_height)
		     xdist = my_height * SCROLL_RATIO;

		  if (Mode_menus.current_depth)
		    {
#ifdef HAS_XINERAMA
		       ewin = Mode_menus.list[0]->ewin;
		       if (ewin->head == head_num)
			 {
#endif
			    for (i = 0; i < Mode_menus.current_depth; i++)
			      {
				 menus[i] = NULL;
				 if (Mode_menus.list[i])
				   {
				      ewin = Mode_menus.list[i]->ewin;
				      if (ewin)
					{
					   menus[i] = ewin;
					   fx[i] = EoGetX(ewin);
					   fy[i] = EoGetY(ewin);
					   tx[i] = EoGetX(ewin) + xdist;
					   ty[i] = EoGetY(ewin) + ydist;
					}
				   }
			      }
			    SlideEwinsTo(menus, fx, fy, tx, ty,
					 Mode_menus.current_depth,
					 Conf.shadespeed);
			    if (((xdist != 0) || (ydist != 0))
				&& (Conf.menus.warp))
			       XWarpPointer(disp, None, None, 0, 0, 0, 0, xdist,
					    ydist);
#ifdef HAS_XINERAMA
			 }
#endif
		    }
	       }
	  }
     }
}
#endif

struct _mdata
{
   Menu               *m;
   MenuItem           *mi;
};

static void
MenusSetEvents(int on)
{
   int                 i, j;
   Menu               *m;
   long                event_mask;

   event_mask = (on) ? MENU_ITEM_EVENT_MASK : 0;

   for (i = 0; i < Mode_menus.current_depth; i++)
     {
	m = Mode_menus.list[i];
	if (!m)
	   continue;

	for (j = 0; j < m->num; j++)
	   ESelectInput(m->items[j]->win, event_mask);
     }
}

static void
SubmenuShowTimeout(int val __UNUSED__, void *dat)
{
   int                 mx, my, my2, xo, yo, mw;
   Menu               *m;
   MenuItem           *mi;
   EWin               *ewin2, *ewin;
   struct _mdata      *data;

   data = (struct _mdata *)dat;
   if (!data)
      return;
   if (!data->m)
      return;

   m = data->m;
   ewin = EwinFindByClient(m->win);
   if (!ewin)
      return;
   if (!EoIsShown(ewin))
      return;

   mi = data->mi;
   MenuShow(mi->child, 1);
   ewin2 = EwinFindByClient(mi->child->win);
   if (!ewin2)
      return;

   EGetGeometry(mi->win, NULL, &mx, &my, &mw, NULL, NULL, NULL);
   my2 = 0;
   if (mi->child->num > 0 && mi->child->items[0])
      EGetGeometry(mi->child->items[0]->win, NULL, NULL, &my2, NULL, NULL, NULL,
		   NULL);

   /* Sub-menu offsets relative to parent menu origin */
   xo = ewin->border->border.left + mx + mw;
   yo = ewin->border->border.top + my - (ewin2->border->border.top + my2);

   if (Conf.menus.onscreen)
     {
	EWin               *menus[256];
	int                 fx[256];
	int                 fy[256];
	int                 tx[256];
	int                 ty[256];
	int                 i, ww, hh;
	int                 xdist = 0, ydist = 0;

	/* Size of new submenu (may be shaded atm.) */
	ww = mi->child->w + ewin2->border->border.left +
	   ewin2->border->border.right;
	hh = mi->child->h + ewin2->border->border.top +
	   ewin2->border->border.bottom;

	if (EoGetX(ewin) + xo + ww > VRoot.w)
	   xdist = VRoot.w - (EoGetX(ewin) + xo + ww);
	if (EoGetY(ewin) + yo + hh > VRoot.h)
	   ydist = VRoot.h - (EoGetY(ewin) + yo + hh);

	if ((xdist != 0) || (ydist != 0))
	  {
	     for (i = 0; i < Mode_menus.current_depth; i++)
	       {
		  menus[i] = NULL;
		  if (Mode_menus.list[i])
		    {
		       ewin = Mode_menus.list[i]->ewin;
		       if (ewin)
			 {
			    menus[i] = ewin;
			    fx[i] = EoGetX(ewin);
			    fy[i] = EoGetY(ewin);
			    tx[i] = EoGetX(ewin) + xdist;
			    ty[i] = EoGetY(ewin) + ydist;
			 }
		    }
	       }

	     /* Disable menu item events while sliding */
	     MenusSetEvents(0);
	     SlideEwinsTo(menus, fx, fy, tx, ty, Mode_menus.current_depth,
			  Conf.shadespeed);
	     MenusSetEvents(1);

	     if (Conf.menus.warp)
		XWarpPointer(disp, None, mi->win, 0, 0, 0, 0,
			     mi->text_w / 2, mi->text_h / 2);
	  }
     }

   Mode.move.check = 0;		/* Bypass on-screen checks */
   MoveEwin(ewin2, EoGetX(ewin) + xo, EoGetY(ewin) + yo);
   Mode.move.check = 1;
   EwinFloatAt(ewin2, EoGetX(ewin2), EoGetY(ewin2));
   RaiseEwin(ewin2);
   ShowEwin(ewin2);

   if (Conf.menus.animate)
      EwinUnShade(ewin2);

   if (Mode_menus.list[Mode_menus.current_depth - 1] != mi->child)
      Mode_menus.list[Mode_menus.current_depth++] = mi->child;
}

static void
MenuActivateItem(Menu * m, MenuItem * mi)
{
   static struct _mdata mdata;
   int                 i, j;

   if (m->sel_item)
     {
	m->sel_item->state = STATE_NORMAL;
	MenuDrawItem(m, m->sel_item, 1);
     }

   m->sel_item = mi;

   if (mi == NULL)
      return;

   mi->state = STATE_HILITED;
   MenuDrawItem(m, mi, 1);

   RemoveTimerEvent("SUBMENU_SHOW");

   for (i = 0; i < Mode_menus.current_depth; i++)
     {
	if (Mode_menus.list[i] == m)
	  {
	     if ((!mi->child) ||
		 ((mi->child) && (Mode_menus.list[i + 1] != mi->child)))
	       {
		  for (j = i + 1; j < Mode_menus.current_depth; j++)
		     MenuHide(Mode_menus.list[j]);
		  Mode_menus.current_depth = i + 1;
		  i = Mode_menus.current_depth;
		  break;
	       }
	  }
     }

   if ((mi->child) && (!mi->child->shown) && MenusActive())
     {
	EWin               *ewin;

	mi->child->parent = m;
	ewin = m->ewin;
	if (ewin)
	  {
	     mdata.m = m;
	     mdata.mi = mi;
	     DoIn("SUBMENU_SHOW", 0.2, SubmenuShowTimeout, 0, &mdata);
	  }
     }
}

static void
MenuItemEventMouseIn(MenuItem * mi, XEvent * ev)
{
   Window              win = ev->xcrossing.window;
   Menu               *m;

   m = mi->menu;

   if ((win == mi->icon_win) && (ev->xcrossing.detail == NotifyAncestor))
      goto done;
   if ((win == mi->win) && (ev->xcrossing.detail == NotifyInferior))
      goto done;

   MenuActivateItem(m, mi);

 done:
   return;
}

static void
MenuItemEventMouseOut(MenuItem * mi, XEvent * ev)
{
   Window              win = ev->xcrossing.window;
   Menu               *m;

   m = mi->menu;

   if ((win == mi->icon_win) && (ev->xcrossing.detail == NotifyAncestor))
      goto done;
   if ((win == mi->win) && (ev->xcrossing.detail == NotifyInferior))
      goto done;

   MenuActivateItem(m, NULL);

 done:
   return;
}

static void
MenuHandleEvents(XEvent * ev, void *prm)
{
   Menu               *m = (Menu *) prm;

#if DEBUG_MENU_EVENTS
   Eprintf("MenuHandleEvents %d\n", ev->type);
#endif
   switch (ev->type)
     {
     case KeyPress:
	MenuEventKeyPress(m, ev);
	break;
     case ButtonRelease:
	break;
     case EnterNotify:
	GrabKeyboardSet(m->win);
	break;
     }
}

static void
MenuItemHandleEvents(XEvent * ev, void *prm)
{
   MenuItem           *mi = (MenuItem *) prm;

#if DEBUG_MENU_EVENTS
   Eprintf("MenuItemHandleEvents %d\n", ev->type);
#endif
   switch (ev->type)
     {
     case ButtonPress:
	MenuItemEventMouseDown(mi, ev);
	break;
     case ButtonRelease:
	MenuItemEventMouseUp(mi, ev);
	break;
     case EnterNotify:
	MenuItemEventMouseIn(mi, ev);
	break;
     case LeaveNotify:
	MenuItemEventMouseOut(mi, ev);
	break;
     }
}

static void
MenuMaskerHandleEvents(XEvent * ev, void *prm __UNUSED__)
{
#if DEBUG_MENU_EVENTS
   Eprintf("MenuMaskerHandleEvents %d\n", ev->type);
#endif
   switch (ev->type)
     {
     case ButtonRelease:
	MenusHide();
	break;
     }
}

/*
 * Configuration load/save
 */
#include "conf.h"

int
MenuStyleConfigLoad(FILE * ConfigFile)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   int                 i1;
   MenuStyle          *ms = NULL;
   int                 fields;

   while (GetLine(s, sizeof(s), ConfigFile))
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
		Alert(_("CONFIG: missing required data in \"%s\"\n"), s);
	  }

	switch (i1)
	  {
	  case CONFIG_CLOSE:
	     AddItem(ms, ms->name, 0, LIST_TYPE_MENU_STYLE);
	     goto done;
	  case CONFIG_CLASSNAME:
	     ms = MenuStyleCreate(s2);
	     break;
	  case CONFIG_TEXT:
	     ms->tclass = TextclassFind(s2, 1);
	     if (ms->tclass)
		ms->tclass->ref_count++;
	     break;
	  case MENU_BG_ICLASS:
	     ms->bg_iclass = ImageclassFind(s2, 0);
	     if (ms->bg_iclass)
		ms->bg_iclass->ref_count++;
	     break;
	  case MENU_ITEM_ICLASS:
	     ms->item_iclass = ImageclassFind(s2, 0);
	     if (ms->item_iclass)
		ms->item_iclass->ref_count++;
	     break;
	  case MENU_SUBMENU_ICLASS:
	     ms->sub_iclass = ImageclassFind(s2, 0);
	     if (ms->sub_iclass)
		ms->sub_iclass->ref_count++;
	     break;
	  case MENU_USE_ITEM_BACKGROUND:
	     ms->use_item_bg = atoi(s2);
	     if (ms->use_item_bg)
	       {
		  if (ms->bg_iclass)
		    {
		       ms->bg_iclass->ref_count--;
		       ms->bg_iclass = NULL;
		    }
	       }
	     break;
	  case MENU_MAX_COLUMNS:
	     ms->maxx = atoi(s2);
	     break;
	  case MENU_MAX_ROWS:
	     ms->maxy = atoi(s2);
	     break;
	  case CONFIG_BORDER:
	     {
		/* FIXME!!!  I don't think this file is loaded in the
		 * right order!
		 */
		Border             *b;

		if (ms->border_name)
		   Efree(ms->border_name);

		ms->border_name = Estrdup(s2);

		b = FindItem(ms->border_name, 0, LIST_FINDBY_NAME,
			     LIST_TYPE_BORDER);
		if (b)
		   b->ref_count++;
	     }
	     break;
	  default:
	     break;
	  }
     }
   err = -1;

 done:
   return err;
}

static int
MenuConfigLoad(FILE * fs)
{
   int                 err = 0;
   char                s[FILEPATH_LEN_MAX];
   char                s2[FILEPATH_LEN_MAX];
   char                s3[FILEPATH_LEN_MAX];
   char                s4[FILEPATH_LEN_MAX];
   char                s5[FILEPATH_LEN_MAX];
   char               *txt = NULL;
   const char         *params = NULL;
   int                 i1, i2;
   Menu               *m = NULL, *mm;
   MenuItem           *mi;
   MenuStyle          *ms;
   ImageClass         *ic = NULL;
   int                 fields;
   int                 act = 0;

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
		  continue;
	       }
	  }

	switch (i1)
	  {
	  case CONFIG_MENU:
	     err = -1;
	     i2 = atoi(s2);
	     if (i2 != CONFIG_OPEN)
		goto done;
	     m = NULL;
	     ic = NULL;
	     _EFREE(txt);
	     act = 0;
	     break;
	  case CONFIG_CLOSE:
	     if (m)
		MenuRealize(m);
	     err = 0;
	     break;

	  case MENU_PREBUILT:
	     sscanf(s, "%i %4000s %4000s %4000s %4000s", &i1, s2, s3, s4, s5);
	     if (!strcmp(s4, "dirscan"))
	       {
		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (!ms)
		     ms = FindItem("DEFAULT", 0, LIST_FINDBY_NAME,
				   LIST_TYPE_MENU_STYLE);

		  if (ms)
		    {
		       SoundPlay("SOUND_SCANNING");
		       m = MenuCreateFromDirectory(s2, NULL, ms, s5);
		    }
	       }
	     else if (!strcmp(s4, "gnome"))
	       {
		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (ms)
		     m = MenuCreateFromGnome(s2, NULL, ms, s5);
	       }
	     else if (!strcmp(s4, "borders"))
	       {
		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (ms)
		     m = MenuCreateFromBorders(s2, ms);
	       }
	     else if (!strcmp(s4, "themes"))
	       {
		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (ms)
		     m = MenuCreateFromThemes(s2, ms);
	       }
	     else if (!strcmp(s4, "file"))
	       {
		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (ms)
		     m = MenuCreateFromFlatFile(s2, NULL, ms, s5);
	       }
	     else if (!strcmp(s4, "windowlist"))
	       {
		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (ms)
		     m = MenuCreateFromAllEWins(s2, ms);
	       }
	     else if (!strcmp(s4, "desktopwindowlist"))
	       {
		  ms = FindItem(s3, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		  if (ms)
		     m = MenuCreateFromDesktops(s2, ms);
	       }
	     break;
	  case CONFIG_CLASSNAME:
	     if (!m)
		m = MenuCreate(s2, NULL, NULL, NULL);
	     else
		MenuSetName(m, s2);
	     break;
	  case MENU_USE_STYLE:
	     {
		ms = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
		if (ms)
		   MenuSetStyle(m, ms);
	     }
	     break;
	  case MENU_TITLE:
	     if (m)
		MenuSetTitle(m, atword(s, 2));
	     break;
	  case MENU_ITEM:
#if 0				/* FIXME - Why ? */
	     if ((txt) || (ic))
	       {
		  mi = MenuItemCreate(txt, ic, NULL, NULL);
		  MenuAddItem(m, mi);
	       }
#endif
	     ic = NULL;
	     if (strcmp("NULL", s2))
		ic = ImageclassFind(s2, 0);
	     _EFDUP(txt, atword(s, 3));
	     break;
	  case MENU_ACTION:
	     if ((txt) || (ic))
	       {
		  char                ok = 1;

		  /* if its an execute line then check to see if the exec is 
		   * on your system before adding the menu entry */
		  if (!strcmp(s2, "exec"))
		    {
		       char                buf[1024];
		       char               *path;

		       params = atword(s, 3);
		       if (params)
			 {
			    sscanf(atword(s, 3), "%1000s", buf);
			    path = pathtoexec(buf);
			    if (path)
			       Efree(path);
			    else
			       ok = 0;
			 }
		    }
		  if (ok)
		    {
		       params = atword(s, 2);
		       mi = MenuItemCreate(txt, ic, params, NULL);
		       MenuAddItem(m, mi);
		    }
		  ic = NULL;
		  _EFREE(txt);
	       }
	     break;
	  case MENU_SUBMENU:
	     sscanf(s, "%i %4000s %4000s", &i1, s2, s3);
	     ic = NULL;
	     if (strcmp("NULL", s3))
		ic = ImageclassFind(s3, 0);
	     mm = FindItem(s2, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU);
	     /* if submenu empty - dont put it in - only if menu found */
	     if (MenuIsNotEmpty(mm))
	       {
		  mi = MenuItemCreate(atword(s, 4), ic, NULL, mm);
		  MenuAddItem(m, mi);
	       }
	     break;
	  default:
	     break;
	  }
     }

 done:
   if (err)
      ConfigAlertLoad(_("Menu"));
   _EFREE(txt);

   return err;
}

/*
 * Menus Module
 */

static void
MenusSighan(int sig, void *prm __UNUSED__)
{
   switch (sig)
     {
     case ESIGNAL_CONFIGURE:
	ConfigFileLoad("menus.cfg", NULL, MenuConfigLoad, 1);
	break;

     case ESIGNAL_AREA_SWITCH_START:
     case ESIGNAL_DESK_SWITCH_START:
	MenusHide();
	break;

     case ESIGNAL_EWIN_UNMAP:
	if ((EWin *) prm == Mode_menus.context_ewin)
	   MenusHide();
	break;
     }
}

static void
MenusIpc(const char *params, Client * c __UNUSED__)
{
   const char         *p;
   char                cmd[128], prm[4096];
   int                 i, len, num;

   cmd[0] = prm[0] = '\0';
   p = params;
   if (p)
     {
	len = 0;
	sscanf(p, "%100s %4000s %n", cmd, prm, &len);
	p += len;
     }

   if (!p || cmd[0] == '?')
     {
	IpcPrintf("Menus - depth=%d, clicked=%d\n",
		  Mode_menus.current_depth, Mode_menus.clicked);
     }
   else if (!strncmp(cmd, "list", 2))
     {
	Menu              **lst;

	lst = (Menu **) ListItemType(&num, LIST_TYPE_MENU);
	for (i = 0; i < num; i++)
	  {
	     IpcPrintf("%s\n", MenuGetName(lst[i]));
	  }
	if (lst)
	   Efree(lst);

     }
   else if (!strncmp(cmd, "reload", 2))
     {
	MenusDestroyLoaded();
	ConfigFileLoad("menus.cfg", NULL, MenuConfigLoad, 1);
     }
   else if (!strncmp(cmd, "show", 2))
     {
	/* FIXME - Menus should have update function? */
	void                ShowAllTaskMenu(void);
	void                ShowDeskMenu(void);
	void                ShowGroupMenu(void);

	if (!strcmp(prm, "deskmenu"))
	  {
	     SoundPlay("SOUND_MENU_SHOW");
	     ShowDeskMenu();
	  }
	else if (!strcmp(prm, "taskmenu"))
	  {
	     SoundPlay("SOUND_MENU_SHOW");
	     ShowAllTaskMenu();
	  }
	else if (!strcmp(prm, "groupmenu"))
	  {
	     SoundPlay("SOUND_MENU_SHOW");
	     ShowGroupMenu();
	  }
	else if (!strcmp(prm, "named"))
	  {
	     SoundPlay("SOUND_MENU_SHOW");
	     MenusShowNamed(p);
	  }
	else
	  {
	     SoundPlay("SOUND_MENU_SHOW");
	     MenusShowNamed(prm);
	  }
     }
}

IpcItem             MenusIpcArray[] = {
   {
    MenusIpc,
    "menus", "mnu",
    "Menu functions",
    "  menus list               Show existing menus\n"
    "  menus reload             Reload menus.cfg without restarting\n"
    "  menus show <name>        Show named menu\n"}
   ,
};
#define N_IPC_FUNCS (sizeof(MenusIpcArray)/sizeof(IpcItem))

static const CfgItem MenusCfgItems[] = {
   CFG_ITEM_BOOL(Conf.menus, animate, 0),
   CFG_ITEM_BOOL(Conf.menus, onscreen, 1),
   CFG_ITEM_BOOL(Conf.menus, warp, 1),
   CFG_ITEM_INT(Conf.menus, opacity, 220),
   CFG_ITEM_INT(Conf.menus, key.left, XK_Left),
   CFG_ITEM_INT(Conf.menus, key.right, XK_Right),
   CFG_ITEM_INT(Conf.menus, key.up, XK_Up),
   CFG_ITEM_INT(Conf.menus, key.down, XK_Down),
   CFG_ITEM_INT(Conf.menus, key.escape, XK_Escape),
   CFG_ITEM_INT(Conf.menus, key.ret, XK_Return),
};
#define N_CFG_ITEMS (sizeof(MenusCfgItems)/sizeof(CfgItem))

/*
 * Module descriptor
 */
EModule             ModMenus = {
   "menus", "menu",
   MenusSighan,
   {N_IPC_FUNCS, MenusIpcArray},
   {N_CFG_ITEMS, MenusCfgItems}
};
