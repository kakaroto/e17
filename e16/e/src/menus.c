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
#define DECLARE_STRUCT_MENU 1
#include "E.h"
#include <sys/stat.h>
#include <errno.h>

static void         MenuRedraw(Menu * m);
static void         MenuDrawItem(Menu * m, MenuItem * mi, char shape);

static void         FileMenuUpdate(int val, void *data);
static void         FillFlatFileMenu(Menu * m, MenuStyle * ms, char *name,
				     char *file, Menu * parent);

Menu               *
FindMenuItem(Window win, MenuItem ** mi)
{
   Menu               *menu = NULL;
   Menu              **menus;
   int                 i, j, num;

   EDBUG(6, "FindMenuItem");

   menus = (Menu **) ListItemType(&num, LIST_TYPE_MENU);
   for (i = 0; i < num; i++)
     {
	for (j = 0; j < menus[i]->num; j++)
	  {
	     if ((win == menus[i]->items[j]->win) ||
		 (win == menus[i]->items[j]->icon_win))
	       {
		  *mi = menus[i]->items[j];
		  menu = menus[i];
		  break;
	       }
	  }
     }
   if (menus)
      Efree(menus);

   EDBUG_RETURN(menu);
}

Menu               *
FindMenu(Window win)
{
   Menu               *menu = NULL;
   Menu              **menus;
   int                 i, num;

   EDBUG(6, "FindMenu");

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

   EDBUG_RETURN(menu);
}

EWin               *
FindEwinSpawningMenu(Menu * m)
{
   EWin               *ewin = NULL;
   EWin              **ewins;
   int                 i, num;

   EDBUG(6, "FindEwinSpawningMenu");

   ewins = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (i = 0; i < num; i++)
     {
	if (ewins[i]->shownmenu != m->win)
	   continue;
	ewin = ewins[i];
	break;
     }
   if (ewins)
      Efree(ewins);

   EDBUG_RETURN(ewin);
}

void
MenuHide(Menu * m)
{
   EWin               *ewin;

   EDBUG(5, "MenuHide");

   if (m->win)
      EUnmapWindow(disp, m->win);

   ewin = FindEwinSpawningMenu(m);
   if (ewin)
      ewin->shownmenu = 0;

   ewin = FindEwinByMenu(m);
   if (ewin)
     {
	HideEwin(ewin);
     }

   if (m->sel_item)
     {
	m->sel_item->state = STATE_NORMAL;
	MenuDrawItem(m, m->sel_item, 1);
	m->sel_item = NULL;
     }

   m->stuck = 0;
   m->shown = 0;

   EDBUG_RETURN_;
}

static void
MenuMoveResize(EWin * ewin, int resize)
{
   Menu               *m = ewin->menu;

   if (!m)
      return;

   if ((!m->style->use_item_bg && m->pmm.pmap == 0) ||
       conf.theme.transparency || IclassIsTransparent(m->style->bg_iclass))
      MenuRedraw(m);
   return;
   resize = 0;
}

static void
MenuRefresh(EWin * ewin)
{
   MenuMoveResize(ewin, 0);
}

static void
MenuEwinInit(EWin * ewin, void *ptr)
{
   ewin->menu = (Menu *) ptr;
   ewin->MoveResize = MenuMoveResize;
   ewin->Refresh = MenuRefresh;
}

void
MenuShow(Menu * m, char noshow)
{
   EWin               *ewin;
   int                 x, y;
   int                 wx = 0, wy = 0;	/* wx, wy added to stop menus */
   unsigned int        w, h, mw, mh;	/* from appearing offscreen */
   int                 head_num = 0;

   EDBUG(5, "MenuShow");
   if ((m->num <= 0) || (!m->style))
      EDBUG_RETURN_;

   if (m->shown)
      return;

   if (m->stuck)
     {
	Button             *button;
	EWin               *ewin99;

	if ((button = FindButton(mode.context_win)))
	  {
	     ButtonDrawWithState(button, STATE_NORMAL);
	  }
	else if ((ewin99 = FindEwinByDecoration(mode.context_win)))
	  {
	     int                 i99;

	     for (i99 = 0; i99 < ewin99->border->num_winparts; i99++)
	       {
		  if (mode.context_win == ewin99->bits[i99].win)
		    {
		       ewin99->bits[i99].state = STATE_NORMAL;
		       ChangeEwinWinpart(ewin99, i99);
		       i99 = ewin99->border->num_winparts;
		    }
	       }
	  }
	EDBUG_RETURN_;
     }

   if (!m->win)
      MenuRealize(m);

   ewin = FindEwinByMenu(m);
   if (ewin)
     {
	if ((mode.button) &&
	    FindItem((char *)mode.button, 0, LIST_FINDBY_POINTER,
		     LIST_TYPE_BUTTON))
	  {
	     ButtonDrawWithState(mode.button, STATE_NORMAL);
	  }
#if 0				/* ??? */
	RaiseEwin(ewin);
	ShowEwin(ewin);
	EDBUG_RETURN_;
#else
	MenuHide(m);
#endif
     }

   GetWinXY(m->items[0]->win, &x, &y);
   GetWinWH(m->items[0]->win, &w, &h);
   GetWinWH(m->win, &mw, &mh);

   if (!mode.button)
     {
	int                 dum;
	Window              rt, ch;

	XQueryPointer(disp, m->win, &rt, &ch, &(mode.x), &(mode.y), &dum, &dum,
		      (unsigned int *)&dum);
     }

   wx = 0;
   wy = 0;
   if (conf.menusonscreen)
     {
	Border             *b;

	b = (Border *) FindItem(m->style->border_name, 0, LIST_FINDBY_NAME,
				LIST_TYPE_BORDER);
	if (b)
	  {
	     int                 width;
	     int                 height;
	     int                 x_origin;
	     int                 y_origin;

	     head_num =
		GetPointerScreenGeometry(&x_origin, &y_origin, &width, &height);

	     if (mode.x - x - ((int)mw / 2) > (x_origin + width))
		wx = x_origin + (int)b->border.left;
	     else if (mode.x + ((int)mw / 2) > (int)(x_origin + width))
		wx = (x_origin + width) - (int)mw - (int)b->border.right;
	     else
		wx = mode.x - x - ((int)w / 2);

	     if ((wx - ((int)w / 2)) < x_origin)
		wx = x_origin + (int)b->border.left;

	     if (mode.y + (int)mh > (int)root.h)
		wy = (y_origin + height) - (int)mh - (int)b->border.bottom;
	     else
		wy = mode.y - y - ((int)h / 2);

	     if ((wy - ((int)h / 2) - (int)b->border.top) < y_origin)
		wy = y_origin + (int)b->border.top;
	  }
     }

   if ((mode.x >= 0) && (mode.y >= 0))
     {
	if (conf.menusonscreen)
	   EMoveWindow(disp, m->win, wx, wy);
	else
	   EMoveWindow(disp, m->win, mode.x - x - (w / 2),
		       mode.y - y - (h / 2));
     }
   else if ((mode.x >= 0) && (mode.y < 0))
     {
	if (((-mode.y) + (int)mh) > (int)root.h)
	   mode.y = -((-mode.y) - mode.context_h - mh);
	if (conf.menusonscreen)
	   EMoveWindow(disp, m->win, wx, -mode.y);
	else
	   EMoveWindow(disp, m->win, mode.x - x - (w / 2), -mode.y);
     }
   else if ((mode.x < 0) && (mode.y >= 0))
     {
	if (((-mode.x) + (int)mw) > (int)root.w)
	   mode.x = -((-mode.x) - mode.context_w - mw);
	if (conf.menusonscreen)
	   EMoveWindow(disp, m->win, -mode.x, wy);
	else
	   EMoveWindow(disp, m->win, -mode.x, mode.y - y - (h / 2));
     }
   else
     {
	if (((-mode.x) + (int)mw) > (int)root.w)
	   mode.x = -((-mode.x) - mode.context_w - mw);
	if (((-mode.y) + (int)mh) > (int)root.h)
	   mode.y = -((-mode.y) - mode.context_h - mh);
	EMoveWindow(disp, m->win, -mode.x, -mode.y);
     }

   ewin = AddInternalToFamily(m->win, m->style->border_name, EWIN_TYPE_MENU, m,
			      MenuEwinInit);
   if (ewin)
     {
	ewin->head = head_num;
	if (conf.menuslide)
	   InstantShadeEwin(ewin, 0);
	ICCCM_Cmap(NULL);
	MoveEwin(ewin, ewin->x, ewin->y);
	if (!noshow)
	  {
	     ShowEwin(ewin);
	     if (conf.menuslide)
		UnShadeEwin(ewin);
	  }
     }

   m->stuck = 0;

   if (!FindMenu(m->win))
      AddItem(m, m->name, m->win, LIST_TYPE_MENU);

   {
      Button             *button;
      EWin               *ewin99;

      if ((button = FindButton(mode.context_win)))
	{
	   ButtonDrawWithState(button, STATE_NORMAL);
	}
      else if ((ewin99 = FindEwinByDecoration(mode.context_win)))
	{
	   int                 i99;

	   for (i99 = 0; i99 < ewin99->border->num_winparts; i99++)
	     {
		if (mode.context_win == ewin99->bits[i99].win)
		  {
		     ewin99->bits[i99].state = STATE_NORMAL;
		     ChangeEwinWinpart(ewin99, i99);
		     i99 = ewin99->border->num_winparts;
		  }
	     }
	}
   }

   m->shown = 1;

   EDBUG_RETURN_;
}

MenuStyle          *
MenuStyleCreate(void)
{
   MenuStyle          *ms;

   EDBUG(5, "MenuStyleCreate");
   ms = Emalloc(sizeof(MenuStyle));

   ms->name = NULL;
   ms->tclass = NULL;
   ms->bg_iclass = NULL;
   ms->item_iclass = NULL;
   ms->sub_iclass = NULL;
   ms->use_item_bg = 0;
   ms->iconpos = ICON_LEFT;
   ms->maxx = 0;
   ms->maxy = 0;
   ms->border_name = NULL;
   ms->ref_count = 0;

   EDBUG_RETURN(ms);
}

MenuItem           *
MenuItemCreate(const char *text, ImageClass * iclass, int action_id,
	       char *action_params, Menu * child)
{
   MenuItem           *mi;

   EDBUG(5, "MenuItemCreate");
   mi = Emalloc(sizeof(MenuItem));

   mi->icon_iclass = iclass;
   if (iclass)
      iclass->ref_count++;
   mi->text = Estrdup(text);
   mi->act_id = action_id;
   if (action_params)
     {
	mi->params = Estrdup(action_params);
     }
   else
     {
	mi->params = NULL;
     }
   mi->child = child;
   mi->state = STATE_NORMAL;
   mi->win = 0;
   mi->pmm[0].pmap = 0;
   mi->pmm[1].pmap = 0;
   mi->pmm[2].pmap = 0;
   mi->pmm[0].mask = 0;
   mi->pmm[1].mask = 0;
   mi->pmm[2].mask = 0;
   mi->icon_win = 0;
   mi->icon_w = 0;
   mi->icon_h = 0;
   mi->text_w = 0;
   mi->text_h = 0;
   mi->text_x = 0;
   mi->text_y = 0;

   EDBUG_RETURN(mi);
}

Menu               *
MenuCreate(const char *name)
{
   Menu               *m;

   EDBUG(5, "MenuCreate");
   m = Emalloc(sizeof(Menu));
   m->ref_count = 0;
   m->name = NULL;
   m->title = NULL;
   m->style = NULL;
   m->num = 0;
   m->items = NULL;
   m->win = 0;
   m->pmm.pmap = 0;
   m->pmm.mask = 0;
   m->shown = 0;
   m->stuck = 0;
   m->internal = 0;
   m->parent = NULL;
   m->sel_item = NULL;
   m->data = NULL;
   m->ref_menu = NULL;
   m->last_change = 0;
   MenuAddName(m, name);
   EDBUG_RETURN(m);
}

void
MenuDestroy(Menu * m)
{
   int                 i, j;
   char                s[4096];

   EDBUG(5, "MenuDestroy");

   if (!m)
      EDBUG_RETURN_;

   MenuHide(m);

   if (m->win)
      EDestroyWindow(disp, m->win);

   Esnprintf(s, sizeof(s), "__.%s", m->name);
   RemoveTimerEvent(s);
   RemoveItem((char *)m, m->win, LIST_FINDBY_POINTER, LIST_TYPE_MENU);
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
		  if (FindItem
		      ((char *)m->items[i]->child, 0, LIST_FINDBY_POINTER,
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

   EDBUG_RETURN_;
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

   EDBUG(5, "MenuEmpty");
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
		EDestroyWindow(disp, m->items[i]->win);
	     if (m->items[i])
		Efree(m->items[i]);
	  }
     }
   if (m->items)
      Efree(m->items);
   m->items = NULL;
   m->num = 0;
   EDBUG_RETURN_;
}

void
MenuRepack(Menu * m)
{
   EWin               *ewin;
   unsigned int        w, h;

   EDBUG(5, "MenuRepack");
   ewin = FindEwinByMenu(m);
   if (m->win)
      MenuRealize(m);
   if (ewin)
     {
	GetWinWH(m->win, &w, &h);
	ewin->client.height.min = h;
	ewin->client.height.max = h;
	ewin->client.width.min = w;
	ewin->client.width.max = w;
	ResizeEwin(ewin, w, h);
	RaiseEwin(ewin);
     }
   EDBUG_RETURN_;
}

void
MenuAddItem(Menu * menu, MenuItem * item)
{
   EDBUG(5, "MenuAddItem");
   menu->num++;
   menu->items = Erealloc(menu->items, sizeof(MenuItem *) * menu->num);
   menu->items[menu->num - 1] = item;
   EDBUG_RETURN_;
}

void
MenuAddName(Menu * menu, const char *name)
{
   EDBUG(5, "MenuAddName");
   if (menu->name)
      Efree(menu->name);
   menu->name = Estrdup(name);
   AddItem(menu, menu->name, menu->win, LIST_TYPE_MENU);
   EDBUG_RETURN_;
}

void
MenuAddTitle(Menu * menu, const char *title)
{
   EDBUG(5, "MenuAddTitle");
   if (menu->title)
      Efree(menu->title);
   menu->title = Estrdup(title);
   EDBUG_RETURN_;
}

void
MenuAddStyle(Menu * menu, const char *style)
{
   EDBUG(5, "MenuAddStyle");
   menu->style = FindItem(style, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
   EDBUG_RETURN_;
}

void
MenuRealize(Menu * m)
{
   int                 i, maxh = 0, maxw =
      0, maxx1, maxx2, w, h, x, y, r, mmw, mmh;
   unsigned int        iw, ih;
   Imlib_Image        *im;
   XSetWindowAttributes att;
   XTextProperty       xtp;
   char                pq, has_i, has_s;

   EDBUG(5, "MenuRealize");
   if (!m->style)
      EDBUG_RETURN_;

   if (!m->win)
      m->win = ECreateWindow(root.win, 0, 0, 1, 1, 0);

   if (m->title)
     {
	xtp.encoding = XA_STRING;
	xtp.format = 8;
	xtp.value = (unsigned char *)(m->title);
	xtp.nitems = strlen((char *)(xtp.value));
	XSetWMName(disp, m->win, &xtp);
     }
   maxh = 0;
   maxx1 = 0;
   maxx2 = 0;
   has_i = 0;
   has_s = 0;
   att.event_mask = PointerMotionMask;
   XChangeWindowAttributes(disp, m->win, CWEventMask, &att);
   att.event_mask =
      ButtonPressMask | ButtonReleaseMask | EnterWindowMask | LeaveWindowMask
      | PointerMotionMask;

   for (i = 0; i < m->num; i++)
     {
	if (m->items[i]->child)
	   has_s = 1;
	else
	   has_i = 1;
	m->items[i]->win = ECreateWindow(m->win, 0, 0, 1, 1, 0);
	XChangeWindowAttributes(disp, m->items[i]->win, CWEventMask, &att);
	EMapWindow(disp, m->items[i]->win);
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
		  EMapWindow(disp, m->items[i]->icon_win);
		  XChangeWindowAttributes(disp, m->items[i]->icon_win,
					  CWEventMask, &att);
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
   x = 0;
   y = 0;
   if ((m->style->bg_iclass) && (!m->style->use_item_bg))
     {
	x = m->style->bg_iclass->padding.left;
	y = m->style->bg_iclass->padding.top;
     }

   r = 0;
   mmw = 0;
   mmh = 0;
   pq = queue_up;
   queue_up = 0;

   for (i = 0; i < m->num; i++)
     {
	EMoveResizeWindow(disp, m->items[i]->win, x, y, maxw, maxh);
	if (m->style->iconpos == ICON_LEFT)
	  {
	     m->items[i]->text_x = m->style->item_iclass->padding.left + maxx2;
	     m->items[i]->text_w = maxx1;
	     m->items[i]->text_y = (maxh - m->items[i]->text_h) / 2;
	     if (m->items[i]->icon_win)
		EMoveWindow(disp, m->items[i]->icon_win,
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
		EMoveWindow(disp, m->items[i]->icon_win,
			    maxw - m->style->item_iclass->padding.right -
			    maxx2 + ((maxx2 - w) / 2), ((maxh - h) / 2));
	  }
	if (m->items[i]->icon_iclass)
	  {
	     iw = 0;
	     ih = 0;
	     GetWinWH(m->items[i]->icon_win, &iw, &ih);
	     IclassApply(m->items[i]->icon_iclass, m->items[i]->icon_win, iw,
			 ih, 0, 0, STATE_NORMAL, 0);
	  }
	if (x + maxw > mmw)
	   mmw = x + maxw;
	if (y + maxh > mmh)
	   mmh = y + maxh;
	if ((m->style->maxx) || (m->style->maxy))
	  {
	     if (m->style->maxy)
	       {
		  y += maxh;
		  r++;
		  if (r >= m->style->maxy)
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
   EResizeWindow(disp, m->win, mmw, mmh);

   queue_up = pq;
   EDBUG_RETURN_;
}

static void
MenuRedraw(Menu * m)
{
   int                 i, w, h;

   if (!m->style->use_item_bg)
     {
	GetWinWH(m->win, &w, &h);
	FreePmapMask(&m->pmm);
	IclassApplyCopy(m->style->bg_iclass, m->win, w, h, 0, 0,
			STATE_NORMAL, &m->pmm, 1);
	ESetWindowBackgroundPixmap(disp, m->win, m->pmm.pmap);
	EShapeCombineMask(disp, m->win, ShapeBounding, 0, 0, m->pmm.mask,
			  ShapeSet);
	for (i = 0; i < m->num; i++)
	   MenuDrawItem(m, m->items[i], 0);
     }
   else
     {
	for (i = 0; i < m->num; i++)
	   MenuDrawItem(m, m->items[i], 0);
	PropagateShapes(m->win);
     }
}

void
MenuDrawItem(Menu * m, MenuItem * mi, char shape)
{
   GC                  gc;
   XGCValues           gcv;
   unsigned int        w, h;
   int                 x, y;
   char                pq;
   PmapMask           *mi_pmm;

   EDBUG(5, "MenuDrawItem");
   pq = queue_up;
   queue_up = 0;

   mi_pmm = &(mi->pmm[(int)(mi->state)]);
   if (conf.theme.transparency || IclassIsTransparent(m->style->bg_iclass))
      FreePmapMask(mi_pmm);
   if (!mi_pmm->pmap)
     {
	if (mi->text)
	  {
	     GetWinWH(mi->win, &w, &h);
	     GetWinXY(mi->win, &x, &y);
	     if (!m->style->use_item_bg)
	       {
		  mi_pmm->type = 0;
		  mi_pmm->pmap = ECreatePixmap(disp, mi->win, w, h, root.depth);
		  gc = XCreateGC(disp, m->pmm.pmap, 0, &gcv);
		  XCopyArea(disp, m->pmm.pmap, mi_pmm->pmap, gc, x, y, w, h, 0,
			    0);
		  mi_pmm->mask = None;
		  if ((mi->state != STATE_NORMAL) || (mi->child))
		    {
		       PmapMask            pmm;

		       if (mi->child)
			  IclassApplyCopy(m->style->sub_iclass, mi->win, w, h,
					  0, 0, mi->state, &pmm, 1);
		       else
			  IclassApplyCopy(m->style->item_iclass, mi->win, w, h,
					  0, 0, mi->state, &pmm, 1);
		       if (pmm.mask)
			 {
			    XSetClipMask(disp, gc, pmm.mask);
			    XSetClipOrigin(disp, gc, 0, 0);
			 }
		       XCopyArea(disp, pmm.pmap, mi_pmm->pmap, gc, 0, 0, w, h,
				 0, 0);
		       FreePmapMask(&pmm);
		    }
		  XFreeGC(disp, gc);
	       }
	     else
	       {
		  if (mi->child)
		     IclassApplyCopy(m->style->sub_iclass, mi->win, w, h, 0, 0,
				     mi->state, mi_pmm, 1);
		  else
		     IclassApplyCopy(m->style->item_iclass, mi->win, w, h, 0, 0,
				     mi->state, mi_pmm, 1);
	       }
	  }
     }

   if ((m->style->tclass) && (mi->text))
     {
	TextDraw(m->style->tclass, mi_pmm->pmap, 0, 0, mi->state,
		 mi->text, mi->text_x, mi->text_y, mi->text_w, mi->text_h, 17,
		 m->style->tclass->justification);
     }

   if (mi->text)
     {
	ESetWindowBackgroundPixmap(disp, mi->win, mi_pmm->pmap);
	EShapeCombineMask(disp, mi->win, ShapeBounding, 0, 0, mi_pmm->mask,
			  ShapeSet);
	XClearWindow(disp, mi->win);
     }
   else
     {
	GetWinWH(mi->win, &w, &h);
	GetWinXY(mi->win, &x, &y);
	if (!m->style->use_item_bg)
	  {
	     if ((mi->state != STATE_NORMAL) || (mi->child))
		IclassApply(m->style->item_iclass, mi->win, w, h, 0, 0,
			    mi->state, 0);
	     else
	       {
		  ESetWindowBackgroundPixmap(disp, mi->win, ParentRelative);
		  EShapeCombineMask(disp, mi->win, ShapeBounding, 0, 0, None,
				    ShapeSet);
		  XClearWindow(disp, mi->win);
	       }
	  }
	else
	  {
	     if (mi->child)
		IclassApply(m->style->sub_iclass, mi->win, w, h, 0, 0,
			    mi->state, 0);
	     else
		IclassApply(m->style->item_iclass, mi->win, w, h, 0, 0,
			    mi->state, 0);
	  }
     }

   if ((shape) && (m->style->use_item_bg))
      PropagateShapes(m->win);

   queue_up = pq;
   EDBUG_RETURN_;
}

Menu               *
MenuCreateFromDirectory(char *name, MenuStyle * ms, char *dir)
{
   Progressbar        *p = NULL;
   Menu               *m, *mm;
   int                 i, num;
   char              **list, s[4096], ss[4096], *ext, cs[4096];
   MenuItem           *mi;
   struct stat         st;
   const char         *chmap =
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_";
   FILE               *f;

   EDBUG(5, "MenuCreateFromDirectory");
   m = MenuCreate(name);
   m->style = ms;
   if (stat(dir, &st) >= 0)
     {
	int                 aa, bb, cc;

	aa = (int)st.st_ino;
	bb = (int)st.st_dev;
	cc = 0;
	if (st.st_mtime > st.st_ctime)
	   cc = st.st_mtime;
	else
	   cc = st.st_ctime;
	Esnprintf(cs, sizeof(cs),
		  "%s/cached/img/.%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
		  EDirUserCache(), chmap[(aa >> 0) & 0x3f],
		  chmap[(aa >> 6) & 0x3f], chmap[(aa >> 12) & 0x3f],
		  chmap[(aa >> 18) & 0x3f], chmap[(aa >> 24) & 0x3f],
		  chmap[(aa >> 28) & 0x3f], chmap[(bb >> 0) & 0x3f],
		  chmap[(bb >> 6) & 0x3f], chmap[(bb >> 12) & 0x3f],
		  chmap[(bb >> 18) & 0x3f], chmap[(bb >> 24) & 0x3f],
		  chmap[(bb >> 28) & 0x3f], chmap[(cc >> 0) & 0x3f],
		  chmap[(cc >> 6) & 0x3f], chmap[(cc >> 12) & 0x3f],
		  chmap[(cc >> 18) & 0x3f], chmap[(cc >> 24) & 0x3f],
		  chmap[(cc >> 28) & 0x3f]);
	/* cached dir listing - use it */
	if (exists(cs))
	  {
	     f = fopen(cs, "r");
	     while (fgets(s, sizeof(s), f))
	       {
		  s[strlen(s) - 1] = 0;
		  word(s, 1, ss);
		  if (!strcmp(ss, "BG"))
		    {
		       Background         *bg;
		       char                ok = 1;
		       char                s2[4096], s3[512];

		       word(s, 3, s3);
		       bg = (Background *) FindItem(s3, 0, LIST_FINDBY_NAME,
						    LIST_TYPE_BACKGROUND);
		       if (!bg)
			 {
			    Imlib_Image        *im;

			    word(s, 2, s2);
			    Esnprintf(ss, sizeof(ss), "%s/%s", dir, s2);
			    im = imlib_load_image(ss);
			    if (im)
			      {
				 Imlib_Image        *im2;
				 XColor              xclr;
				 char                tile = 1, keep_asp = 0;
				 int                 width, height;
				 int                 scalex = 0, scaley = 0;
				 int                 scr_asp, im_asp;
				 int                 w2, h2;
				 int                 maxw = 48, maxh = 48;
				 int                 justx = 512, justy = 512;

				 Esnprintf(s2, sizeof(s2), "%s/cached/img/%s",
					   EDirUserCache(), s3);
				 imlib_context_set_image(im);
				 width = imlib_image_get_width();
				 height = imlib_image_get_height();
				 h2 = maxh;
				 w2 =
				    (imlib_image_get_width() * h2) /
				    imlib_image_get_height();
				 if (w2 > maxw)
				   {
				      w2 = maxw;
				      h2 =
					 (imlib_image_get_height() * w2) /
					 imlib_image_get_width();
				   }
				 im2 = imlib_create_cropped_scaled_image(0, 0,
									 imlib_image_get_width
									 (),
									 imlib_image_get_height
									 (), w2,
									 h2);
				 imlib_free_image_and_decache();
				 imlib_context_set_image(im2);
				 imlib_image_set_format("ppm");
				 imlib_save_image(s2);
				 imlib_free_image_and_decache();

				 scr_asp = (root.w << 16) / root.h;
				 im_asp = (width << 16) / height;
				 if (width == height)
				   {
				      justx = 0;
				      justy = 0;
				      scalex = 0;
				      scaley = 0;
				      tile = 1;
				      keep_asp = 0;
				   }
				 else if ((!(IN_RANGE(scr_asp, im_asp, 16000)))
					  && ((width < 480) && (height < 360)))
				   {
				      justx = 0;
				      justy = 0;
				      scalex = 0;
				      scaley = 0;
				      tile = 1;
				      keep_asp = 0;
				   }
				 else if (IN_RANGE(scr_asp, im_asp, 16000))
				   {
				      justx = 0;
				      justy = 0;
				      scalex = 1024;
				      scaley = 1024;
				      tile = 0;
				      keep_asp = 0;
				   }
				 else if (im_asp > scr_asp)
				   {
				      justx = 512;
				      justy = 512;
				      scalex = 1024;
				      scaley = 0;
				      tile = 0;
				      keep_asp = 1;
				   }
				 else
				   {
				      justx = 512;
				      justy = 512;
				      scalex = 0;
				      scaley = 1024;
				      tile = 0;
				      keep_asp = 1;
				   }
				 ESetColor(&xclr, 0, 0, 0);
				 bg = CreateDesktopBG(s3, &xclr, ss, tile,
						      keep_asp, justx, justy,
						      scalex, scaley, NULL, 0,
						      0, 0, 0, 0);
				 AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);
			      }
			    else
			       ok = 0;
			 }
		       if (ok)
			 {
			    ImageClass         *ic = NULL;
			    char                stmp[4096];

			    ic = CreateIclass();
			    ic->name = Estrdup("`");
			    ic->norm.normal = CreateImageState();
			    Esnprintf(stmp, sizeof(stmp), "%s/cached/img/%s",
				      EDirUserCache(), s3);
			    ic->norm.normal->im_file = Estrdup(stmp);
			    ic->norm.normal->unloadable = 1;
			    IclassPopulate(ic);
			    AddItem(ic, ic->name, 0, LIST_TYPE_ICLASS);
			    mi = MenuItemCreate(NULL, ic, ACTION_BACKGROUND_SET,
						s3, NULL);
			    MenuAddItem(m, mi);
			 }
		    }
		  else if (!strcmp(ss, "EXE"))
		    {
		       word(s, 2, ss);
		       Esnprintf(s, sizeof(s), "%s/%s", dir, ss);
		       mi = MenuItemCreate(NULL, NULL, ACTION_EXEC, s, NULL);
		       MenuAddItem(m, mi);
		    }
		  else if (!strcmp(ss, "DIR"))
		    {
		       char                tmp[4096];

		       word(s, 2, tmp);
		       Esnprintf(s, sizeof(s), "%s/%s:%s", dir, tmp, name);
		       Esnprintf(ss, sizeof(ss), "%s/%s", dir, tmp);
		       mm = MenuCreateFromDirectory(s, ms, ss);
		       mm->parent = m;
		       mi = MenuItemCreate(tmp, NULL, 0, NULL, mm);
		       MenuAddItem(m, mi);
		    }
	       }
	     fclose(f);
	     EDBUG_RETURN(m);
	  }
     }
   list = ls(dir, &num);
   Esnprintf(s, sizeof(s), "Scanning %s", dir);
   if (!init_win_ext)
      p = CreateProgressbar(s, 600, 16);
   if (p)
      ShowProgressbar(p);
   f = fopen(cs, "w");
   for (i = 0; i < num; i++)
     {
	if (p)
	   SetProgressbar(p, (i * 100) / num);
	Esnprintf(ss, sizeof(ss), "%s/%s", dir, list[i]);
	/* skip "dot" files and dirs - senisble */
	if ((*(list[i]) != '.') && (stat(ss, &st) >= 0))
	  {
	     ext = FileExtension(ss);
	     if (S_ISDIR(st.st_mode))
	       {
		  Esnprintf(s, sizeof(s), "%s/%s:%s", dir, list[i], name);
		  Esnprintf(ss, sizeof(ss), "%s/%s", dir, list[i]);
		  mm = MenuCreateFromDirectory(s, ms, ss);
		  mm->parent = m;
		  mi = MenuItemCreate(list[i], NULL, 0, NULL, mm);
		  MenuAddItem(m, mi);
		  if (f)
		     fprintf(f, "DIR %s\n", list[i]);
	       }
/* that's it - people are stupid and have executable images and just */
/* don't get it - so I'm disablign this to save people from their own */
/* stupidity */
/*           else if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
 * {
 * mi = MenuItemCreate(list[i], NULL, ACTION_EXEC, ss, NULL);
 * MenuAddItem(m, mi);
 * if (f)
 * fprintf(f, "EXE %s\n", list[i]);
 * }
 */
	     else if ((!strcmp(ext, "jpg")) || (!strcmp(ext, "JPG"))
		      || (!strcmp(ext, "jpeg")) || (!strcmp(ext, "Jpeg"))
		      || (!strcmp(ext, "JPEG")) || (!strcmp(ext, "Jpg"))
		      || (!strcmp(ext, "gif")) || (!strcmp(ext, "Gif"))
		      || (!strcmp(ext, "GIF")) || (!strcmp(ext, "png"))
		      || (!strcmp(ext, "Png")) || (!strcmp(ext, "PNG"))
		      || (!strcmp(ext, "tif")) || (!strcmp(ext, "Tif"))
		      || (!strcmp(ext, "TIFF")) || (!strcmp(ext, "tiff"))
		      || (!strcmp(ext, "Tiff")) || (!strcmp(ext, "TIFF"))
		      || (!strcmp(ext, "xpm")) || (!strcmp(ext, "Xpm"))
		      || (!strcmp(ext, "XPM")) || (!strcmp(ext, "ppm"))
		      || (!strcmp(ext, "PPM")) || (!strcmp(ext, "pgm"))
		      || (!strcmp(ext, "PGM")) || (!strcmp(ext, "pnm"))
		      || (!strcmp(ext, "PNM")) || (!strcmp(ext, "bmp"))
		      || (!strcmp(ext, "Bmp")) || (!strcmp(ext, "BMP")))
	       {
		  Background         *bg;
		  char                ok = 1;
		  char                s2[4096], s3[512];
		  int                 aa, bb, cc;

		  aa = (int)st.st_ino;
		  bb = (int)st.st_dev;
		  cc = 0;
		  if (st.st_mtime > st.st_ctime)
		     cc = st.st_mtime;
		  else
		     cc = st.st_ctime;
		  Esnprintf(s3, sizeof(s3),
			    ".%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
			    chmap[(aa >> 0) & 0x3f], chmap[(aa >> 6) & 0x3f],
			    chmap[(aa >> 12) & 0x3f], chmap[(aa >> 18) & 0x3f],
			    chmap[(aa >> 24) & 0x3f], chmap[(aa >> 28) & 0x3f],
			    chmap[(bb >> 0) & 0x3f], chmap[(bb >> 6) & 0x3f],
			    chmap[(bb >> 12) & 0x3f], chmap[(bb >> 18) & 0x3f],
			    chmap[(bb >> 24) & 0x3f], chmap[(bb >> 28) & 0x3f],
			    chmap[(cc >> 0) & 0x3f], chmap[(cc >> 6) & 0x3f],
			    chmap[(cc >> 12) & 0x3f], chmap[(cc >> 18) & 0x3f],
			    chmap[(cc >> 24) & 0x3f], chmap[(cc >> 28) & 0x3f]);
		  bg = (Background *) FindItem(s3, 0, LIST_FINDBY_NAME,
					       LIST_TYPE_BACKGROUND);
		  if (!bg)
		    {
		       Imlib_Image        *im;

		       im = imlib_load_image(ss);
		       if (im)
			 {
			    Imlib_Image        *im2;
			    XColor              xclr;
			    char                tile = 1, keep_asp = 0;
			    int                 width, height, scalex =
			       0, scaley = 0;
			    int                 scr_asp, im_asp, w2, h2;
			    int                 maxw = 48, maxh = 48;

			    Esnprintf(s2, sizeof(s2), "%s/cached/img/%s",
				      EDirUserCache(), s3);
			    imlib_context_set_image(im);
			    width = imlib_image_get_width();
			    height = imlib_image_get_height();
			    h2 = maxh;
			    w2 =
			       (imlib_image_get_width() * h2) /
			       imlib_image_get_height();
			    if (w2 > maxw)
			      {
				 w2 = maxw;
				 h2 =
				    (imlib_image_get_height() * w2) /
				    imlib_image_get_width();
			      }
			    im2 = imlib_create_cropped_scaled_image(0, 0,
								    imlib_image_get_width
								    (),
								    imlib_image_get_height
								    (), w2, h2);
			    imlib_free_image_and_decache();
			    imlib_context_set_image(im2);
			    imlib_image_set_format("ppm");
			    imlib_save_image(s2);
			    imlib_free_image_and_decache();

			    scr_asp = (root.w << 16) / root.h;
			    im_asp = (width << 16) / height;
			    if (width == height)
			      {
				 scalex = 0;
				 scaley = 0;
				 tile = 1;
				 keep_asp = 0;
			      }
			    else if ((!(IN_RANGE(scr_asp, im_asp, 16000)))
				     && ((width < 480) && (height < 360)))
			      {
				 scalex = 0;
				 scaley = 0;
				 tile = 1;
				 keep_asp = 0;
			      }
			    else if (IN_RANGE(scr_asp, im_asp, 16000))
			      {
				 scalex = 1024;
				 scaley = 1024;
				 tile = 0;
				 keep_asp = 0;
			      }
			    else if (im_asp > scr_asp)
			      {
				 scalex = 1024;
				 scaley = 0;
				 tile = 0;
				 keep_asp = 1;
			      }
			    else
			      {
				 scalex = 0;
				 scaley = 1024;
				 tile = 0;
				 keep_asp = 1;
			      }
			    ESetColor(&xclr, 0, 0, 0);
			    bg = CreateDesktopBG(s3, &xclr, ss, tile, keep_asp,
						 512, 512, scalex, scaley, NULL,
						 0, 0, 0, 0, 0);
			    AddItem(bg, bg->name, 0, LIST_TYPE_BACKGROUND);
			 }
		       else
			  ok = 0;
		    }
		  if (ok)
		    {
		       ImageClass         *ic = NULL;
		       char                stmp[4096];

		       ic = CreateIclass();
		       ic->name = Estrdup("`");
		       ic->norm.normal = CreateImageState();
		       Esnprintf(stmp, sizeof(stmp), "%s/cached/img/%s",
				 EDirUserCache(), s3);
		       ic->norm.normal->im_file = Estrdup(stmp);
		       ic->norm.normal->unloadable = 1;
		       IclassPopulate(ic);
		       AddItem(ic, ic->name, 0, LIST_TYPE_ICLASS);
		       mi = MenuItemCreate(NULL, ic, ACTION_BACKGROUND_SET, s3,
					   NULL);
		       MenuAddItem(m, mi);
		    }
		  if (f)
		     fprintf(f, "BG %s %s\n", list[i], s3);
	       }
	  }
     }
   if (f)
      fclose(f);
   if (p)
      FreeProgressbar(p);
   if (list)
      freestrlist(list, num);
   EDBUG_RETURN(m);
}

Menu               *
MenuCreateFromFlatFile(char *name, MenuStyle * ms, char *file, Menu * parent)
{
   Menu               *m;
   char                s[4096], *ff = NULL;
   static int          calls = 0;

   EDBUG(5, "MenuCreateFromFlatFile");
   calls++;
   if (calls > 255)
     {
	calls--;
	EDBUG_RETURN(NULL);
     }
   ff = FindFile(file);
   if (!ff)
      EDBUG_RETURN(NULL);
   if (canread(ff))
     {
	m = MenuCreate(name);
	m->style = ms;
	m->last_change = moddate(ff);
	if (parent)
	   FillFlatFileMenu(m, m->style, m->name, ff, parent);
	else
	   FillFlatFileMenu(m, m->style, m->name, ff, m);
	m->data = ff;
	m->ref_menu = parent;
	Esnprintf(s, sizeof(s), "__.%s", m->name);
	DoIn(s, 2.0, FileMenuUpdate, 0, m);
	calls--;
	EDBUG_RETURN(m);
     }
   Efree(ff);
   calls--;
   EDBUG_RETURN(NULL);
}

static void
FillFlatFileMenu(Menu * m, MenuStyle * ms, char *name, char *file,
		 Menu * parent)
{
   FILE               *f;
   char                first = 1;
   char                s[4096];

   f = fopen(file, "r");
   if (!f)
     {
	fprintf(stderr, "Unable to open menu file %s -- %s\n", file,
		strerror(errno));
	return;
     }

   while (fgets(s, 4096, f))
     {
	s[strlen(s) - 1] = 0;
	if ((s[0]) && s[0] != '#')
	  {
	     if (first)
	       {
		  char               *wd;

		  wd = field(s, 0);
		  if (wd)
		    {
		       MenuAddTitle(m, wd);
		       Efree(wd);
		    }
		  first = 0;
	       }
	     else
	       {
		  char               *txt = NULL, *icon = NULL, *act = NULL;
		  char               *params = NULL, *tmp = NULL, wd[4096];

		  MenuItem           *mi;
		  ImageClass         *icc = NULL;
		  Menu               *mm;
		  int                 count = 0;

		  txt = field(s, 0);
		  icon = field(s, 1);
		  act = field(s, 2);
		  params = field(s, 3);
		  tmp = NULL;
		  if (icon)
		    {
		       Esnprintf(wd, sizeof(wd), "__FM.%s", icon);
		       icc =
			  FindItem(wd, 0, LIST_FINDBY_NAME, LIST_TYPE_ICLASS);
		       if (!icc)
			 {
			    icc = CreateIclass();
			    icc->name = Estrdup(wd);
			    icc->norm.normal = CreateImageState();
			    icc->norm.normal->im_file = icon;
			    IclassPopulate(icc);
			    AddItem(icc, icc->name, 0, LIST_TYPE_ICLASS);
			 }
		       else
			 {
			    Efree(icon);
			 }
		    }
		  if ((act) && (!strcmp(act, "exec")) && (params))
		    {
		       word(params, 1, wd);
		       tmp = pathtoexec(wd);
		       if (tmp)
			 {
			    Efree(tmp);

			    mi = MenuItemCreate(txt, icc, ACTION_EXEC, params,
						NULL);
			    MenuAddItem(m, mi);
			 }
		    }
		  else if ((act) && (!strcmp(act, "menu")) && (params))
		    {
		       Esnprintf(wd, sizeof(wd), "__FM.%s.%i", name, count);
		       count++;
		       mm = MenuCreateFromFlatFile(wd, ms, params, parent);
		       if (mm)
			 {
			    mm->parent = m;
			    mi = MenuItemCreate(txt, icc, 0, NULL, mm);
			    MenuAddItem(m, mi);
			 }
		    }
		  else
		    {
		       mi = MenuItemCreate(txt, icc, 0, NULL, NULL);
		       MenuAddItem(m, mi);
		    }
		  if (txt)
		     Efree(txt);
		  if (act)
		     Efree(act);
		  if (params)
		     Efree(params);
	       }
	  }
     }
   fclose(f);
}

static void
FileMenuUpdate(int val, void *data)
{
   Menu               *m, *mm;
   time_t              lastmod = 0;
   char                s[4096];

   m = (Menu *) data;
   if (!m)
      return;
   if (!FindItem((char *)m, m->win, LIST_FINDBY_POINTER, LIST_TYPE_MENU))
      return;
   /* if the menu is up dont update */
   if (((mode.cur_menu_mode) || (clickmenu)) && (mode.cur_menu_depth > 0))
     {
	Esnprintf(s, sizeof(s), "__.%s", m->name);
	DoIn(s, 2.0, FileMenuUpdate, 0, m);
	return;
     }
   mm = m;
   if (m->ref_menu)
      mm = m->ref_menu;
   if (!exists(m->data))
     {
	MenuHide(m);
	MenuEmpty(m);
	return;
     }
   if (m->data)
      lastmod = moddate(m->data);
   if (lastmod > m->last_change)
     {
	m->last_change = lastmod;
	if (m == mm)
	  {
	     Esnprintf(s, sizeof(s), "__.%s", m->name);
	     DoIn(s, 2.0, FileMenuUpdate, 0, m);
	  }
	MenuEmpty(mm);
	FillFlatFileMenu(mm, mm->style, mm->name, mm->data, mm);
	MenuRepack(mm);
	return;
     }
   Esnprintf(s, sizeof(s), "__.%s", m->name);
   DoIn(s, 2.0, FileMenuUpdate, 0, m);
   val = 0;
}

Menu               *
MenuCreateFromGnome(char *name, MenuStyle * ms, char *dir)
{
   Menu               *m, *mm;
   int                 i, num;
   char              **list, s[4096], ss[4096];

   MenuItem           *mi;
   FILE               *f;
   char               *lang, name_buf[20];

   EDBUG(5, "MenuCreateFromGnome");

   if ((lang = setlocale(LC_MESSAGES, NULL)) != NULL)
      Esnprintf(name_buf, sizeof(name_buf), "Name[%s]=", lang);
   else
      name_buf[0] = '\0';

   m = MenuCreate(name);
   m->style = ms;
   list = ls(dir, &num);
   for (i = 0; i < num; i++)
     {
	if ((strcmp(list[i], ".")) && (strcmp(list[i], "..")))
	  {
	     Esnprintf(ss, sizeof(ss), "%s/%s", dir, list[i]);
	     if (isdir(ss))
	       {
		  Esnprintf(s, sizeof(s), "%s/%s:%s", dir, list[i], name);
		  mm = MenuCreateFromGnome(s, ms, ss);
		  mm->parent = m;
		  name = list[i];
		  if (name_buf[0])
		    {
		       Esnprintf(s, sizeof(s), "%s/.directory", ss);
		       if ((f = fopen(s, "r")) != NULL)
			 {
			    while (fgets(s, sizeof(s), f))
			      {
				 if (!strncmp(s, name_buf, strlen(name_buf)))
				   {
				      if (s[strlen(s) - 1] == '\n')
					 s[strlen(s) - 1] = 0;
				      name = &(s[strlen(name_buf)]);
				      break;
				   }
			      }
			    fclose(f);
			 }
		    }
		  mi = MenuItemCreate(name, NULL, 0, NULL, mm);
		  MenuAddItem(m, mi);
	       }
	     else
	       {
		  f = fopen(ss, "r");
		  if (f)
		    {
		       char               *iname = NULL, *exec = NULL, *texec =
			  NULL, *tmp;
		       char               *en_name = NULL;

		       while (fgets(s, sizeof(s), f))
			 {
			    if (s[strlen(s) - 1] == '\n')
			       s[strlen(s) - 1] = 0;
			    if (!strncmp(s, "Name=", strlen("Name=")))
			       en_name = Estrdup(&(s[strlen("Name=")]));
			    else if (name_buf[0]
				     && !strncmp(s, name_buf, strlen(name_buf)))
			       iname = Estrdup(&(s[strlen(name_buf)]));
			    else if (!strncmp
				     (s, "TryExec=", strlen("TryExec=")))
			       texec = Estrdup(&(s[strlen("TryExec=")]));
			    else if (!strncmp(s, "Exec=", strlen("Exec=")))
			       exec = Estrdup(&(s[strlen("Exec=")]));
			 }
		       if (iname)
			 {
			    if (en_name)
			       Efree(en_name);
			 }
		       else
			 {
			    if (en_name)
			       iname = en_name;
			 }
		       fclose(f);
		       if ((iname) && (exec))
			 {
			    tmp = NULL;
			    if (texec)
			       tmp = pathtoexec(texec);
			    if ((tmp) || (!texec))
			      {
				 if (tmp)
				    Efree(tmp);

				 mi = MenuItemCreate(iname, NULL, ACTION_EXEC,
						     exec, NULL);
				 MenuAddItem(m, mi);
			      }
			 }
		       if (iname)
			  Efree(iname);
		       if (exec)
			  Efree(exec);
		       if (texec)
			  Efree(texec);
		    }
	       }
	  }
     }
   if (list)
      freestrlist(list, num);
   EDBUG_RETURN(m);
}

Menu               *
MenuCreateFromThemes(char *name, MenuStyle * ms)
{
   Menu               *m;
   char              **lst;
   int                 i, num;
   char                ss[4096], *s;

   MenuItem           *mi;

   EDBUG(5, "MenuCreateFromThemes");

   m = MenuCreate(name);
   m->style = ms;
   lst = ListThemes(&num);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     s = fullfileof(lst[i]);
	     Esnprintf(ss, sizeof(ss), "restart_theme %s", s);
	     Efree(s);
	     s = fileof(lst[i]);
	     mi = MenuItemCreate(s, NULL, ACTION_EXIT, ss, NULL);
	     MenuAddItem(m, mi);
	     Efree(s);
	  }
	freestrlist(lst, i);
     }
   EDBUG_RETURN(m);
}

static int
BorderNameCompare(void *b1, void *b2)
{
   if (b1 && b2)
      return strcmp(((Border *) b1)->name, ((Border *) b2)->name);

   return 0;
}

Menu               *
MenuCreateFromBorders(char *name, MenuStyle * ms)
{
   Menu               *m;
   Border            **lst;
   int                 i, num;

   MenuItem           *mi;

   EDBUG(5, "MenuCreateFromBorders");

   m = MenuCreate(name);
   m->style = ms;
   lst = (Border **) ListItemType(&num, LIST_TYPE_BORDER);
   if (lst)
      Quicksort((void **)lst, 0, num - 1, BorderNameCompare);
   for (i = 0; i < num; i++)
     {
	/* if its not internal (ie doesnt start with _ ) */
	if (lst[i]->name[0] != '_')
	  {
	     mi = MenuItemCreate(lst[i]->name, NULL, ACTION_SET_WINDOW_BORDER,
				 lst[i]->name, NULL);
	     MenuAddItem(m, mi);
	  }
     }
   if (lst)
      Efree(lst);
   EDBUG_RETURN(m);
}

Menu               *
MenuCreateFromAllEWins(char *name, MenuStyle * ms)
{
   Menu               *m;
   EWin              **lst;
   int                 i, num;
   char                s[256];

   MenuItem           *mi;

   EDBUG(5, "MenuCreateFromEWins");
   m = MenuCreate(name);
   m->style = ms;

   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     if ((!lst[i]->menu) && (!lst[i]->pager) && (!lst[i]->skipwinlist)
		 && (EwinGetTitle(lst[i])) && (!lst[i]->ibox))
	       {
		  Esnprintf(s, sizeof(s), "%i", (int)(lst[i]->client.win));
		  mi = MenuItemCreate(EwinGetTitle(lst[i]), NULL,
				      ACTION_FOCUS_SET, s, NULL);
		  MenuAddItem(m, mi);
	       }
	  }
	Efree(lst);
     }
   EDBUG_RETURN(m);
}

#if 0				/* Not used */
static Menu        *
MenuCreateFromDesktopEWins(char *name, MenuStyle * ms, int desk)
{
   Menu               *m;
   EWin              **lst;
   int                 i, num;
   char                s[256];

   MenuItem           *mi;

   EDBUG(5, "MenuCreateFromDesktopEWins");
   m = MenuCreate(name);
   m->style = ms;

   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     if (((lst[i]->desktop == desk) || (lst[i]->sticky))
		 && (!lst[i]->menu) && (!lst[i]->pager)
		 && (!lst[i]->skipwinlist) && (lst[i]->client.title)
		 && (!lst[i]->ibox))
	       {
		  Esnprintf(s, sizeof(s), "%i", (int)(lst[i]->client.win));
		  mi = MenuItemCreate(lst[i]->client.title, NULL,
				      ACTION_FOCUS_SET, s, NULL);
		  MenuAddItem(m, mi);
	       }
	  }
	Efree(lst);
     }
   EDBUG_RETURN(m);
   desk = 0;
}
#endif

Menu               *
MenuCreateFromDesktops(char *name, MenuStyle * ms)
{
   Menu               *m, *mm;
   EWin              **lst;
   int                 j, i, num;
   char                s[256];

   MenuItem           *mi;

   EDBUG(5, "MenuCreateFromDesktops");
   m = MenuCreate(name);
   m->style = ms;
   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (j = 0; j < conf.desks.num; j++)
     {
	mm = MenuCreate("__SUBMENUDESK_E");
	mm->style = ms;
	Esnprintf(s, sizeof(s), "%i", j);
	mi = MenuItemCreate(_("Go to this Desktop"), NULL, ACTION_GOTO_DESK, s,
			    NULL);
	MenuAddItem(mm, mi);
	for (i = 0; i < num; i++)
	  {
	     if (((lst[i]->desktop == j) || (lst[i]->sticky)) && (!lst[i]->menu)
		 && (!lst[i]->pager) && (!lst[i]->skipwinlist)
		 && (EwinGetTitle(lst[i])) && (!lst[i]->ibox))
	       {
		  Esnprintf(s, sizeof(s), "%i", (int)(lst[i]->client.win));
		  mi = MenuItemCreate(EwinGetTitle(lst[i]), NULL,
				      ACTION_FOCUS_SET, s, NULL);
		  MenuAddItem(mm, mi);
	       }
	  }
	mm->parent = m;
	Esnprintf(s, sizeof(s), _("Desktop %i"), j);
	mi = MenuItemCreate(s, NULL, 0, NULL, mm);
	MenuAddItem(m, mi);
     }
   if (lst)
      Efree(lst);
   EDBUG_RETURN(m);
}

#if 0				/* Not finished */
Menu               *
MenuCreateMoveToDesktop(char *name, MenuStyle * ms)
{
   Menu               *m;
   int                 i;
   char                s1[256], s2[256];

   MenuItem           *mi;

   EDBUG(5, "MenuCreateDesktops");
   m = MenuCreate(name);
   m->style = ms;
   for (i = 0; i < mode.numdesktops; i++)
     {
	Esnprintf(s1, sizeof(s1), _("Desktop %i"), i);
	Esnprintf(s2, sizeof(s2), "%i", i);
	mi = MenuItemCreate(s1, NULL, ACTION_MOVE_TO_DESK, s2, NULL);
	MenuAddItem(m, mi);
     }
   EDBUG_RETURN(m);
}
#endif

static Menu        *
MenuCreateFromGroups(char *name, MenuStyle * ms)
{
   Menu               *m, *mm;
   Group             **lst;
   int                 i, j, num;
   char                s[256];

   MenuItem           *mi;

   EDBUG(5, "MenuCreateFromEWins");
   m = MenuCreate(name);
   m->style = ms;
   lst = (Group **) ListItemType(&num, LIST_TYPE_GROUP);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     mm = MenuCreate("__SUBMENUGROUP_E");
	     mm->style = ms;
	     Esnprintf(s, sizeof(s), "%li", lst[i]->members[0]->client.win);
	     mi = MenuItemCreate(_("Show/Hide this group"), NULL,
				 ACTION_SHOW_HIDE_GROUP, s, NULL);
	     MenuAddItem(mm, mi);
	     mi = MenuItemCreate(_("Iconify this group"), NULL, ACTION_ICONIFY,
				 s, NULL);
	     MenuAddItem(mm, mi);

	     for (j = 0; j < lst[i]->num_members; j++)
	       {
		  Esnprintf(s, sizeof(s), "%li",
			    lst[i]->members[j]->client.win);
		  mi =
		     MenuItemCreate(EwinGetTitle(lst[i]->members[j]), NULL,
				    ACTION_FOCUS_SET, s, NULL);
		  MenuAddItem(mm, mi);
	       }
	     mm->parent = m;
	     Esnprintf(s, sizeof(s), _("Group %i"), i);
	     mi = MenuItemCreate(s, NULL, 0, NULL, mm);
	     MenuAddItem(m, mi);
	  }
	Efree(lst);
     }
   EDBUG_RETURN(m);
}

void
MenuShowMasker(Menu * m)
{
   EWin               *ewin;

   ewin = FindEwinByMenu(m);
   if ((ewin) && (!mode.menu_cover_win))
     {
	Window              parent;
	Window              wl[2];

	parent = desks.desk[ewin->desktop].win;
	mode.menu_cover_win = ECreateEventWindow(parent, 0, 0, root.w, root.h);
	mode.menu_win_covered = ewin->win;
	wl[0] = mode.menu_win_covered;
	wl[1] = mode.menu_cover_win;
	XSelectInput(disp, mode.menu_cover_win,
		     ButtonPressMask | ButtonReleaseMask | EnterWindowMask |
		     LeaveWindowMask);
	XRestackWindows(disp, wl, 2);
	EMapWindow(disp, mode.menu_cover_win);
     }
}

void
MenuHideMasker(void)
{
   if (mode.menu_cover_win)
     {
	EDestroyWindow(disp, mode.menu_cover_win);
	mode.menu_cover_win = 0;
	mode.menu_win_covered = 0;
     }
}

void
ShowNamedMenu(const char *name)
{
   Menu               *m;

   EDBUG(5, "ShowNamedMenu");

   m = FindItem(name, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU);
   if (m)
     {
	mode.cur_menu_mode = 1;
	XUngrabPointer(disp, CurrentTime);
	if (!FindEwinByMenu(m))	/* Don't show if already shown */
	   MenuShow(m, 0);
	mode.cur_menu[0] = m;
	mode.cur_menu_depth = 1;
	MenuShowMasker(m);
	m->ref_count++;
     }
   else
     {
	mode.cur_menu[0] = NULL;
	mode.cur_menu_depth = 0;
	MenuHideMasker();
     }

   EDBUG_RETURN_;
}

void
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

void
MenusHideByWindow(Window win)
{
   Menu               *m;
   int                 i, ok;

   m = FindMenu(win);
   if (m)
     {
	MenuHide(m);
	ok = 0;
	for (i = 0; i < mode.cur_menu_depth; i++)
	  {
	     if (ok)
		MenuHide(mode.cur_menu[i]);
	     if (mode.cur_menu[i] == m)
		ok = 1;
	  }
	MenuHideMasker();
     }
}

/*
 * Internal menus
 */

#if 0				/* Not used */
static Menu        *task_menu[ENLIGHTENMENT_CONF_NUM_DESKTOPS];
#endif

void
MenusInit(void)
{
#if 0				/* Not used */
   int                 i;

   for (i = 0; i < ENLIGHTENMENT_CONF_NUM_DESKTOPS; task_menu[i++] = NULL);
#endif
}

static Menu        *
RefreshInternalMenu(Menu * m, MenuStyle * ms,
		    Menu * (mcf) (char *xxx, MenuStyle * ms))
{
   char                was = 0;
   int                 lx = 0, ly = 0;
   EWin               *ewin;

   EDBUG(5, "RefreshInternalMenu");

   if (m)
     {
	ewin = FindEwinByMenu(m);
	if ((m->win) && (ewin))
	  {
	     lx = ewin->x;
	     ly = ewin->y;
	     was = 1;
	  }
	MenuDestroy(m);
	m = NULL;
     }

   if (!ms)
      EDBUG_RETURN(NULL);

   m = mcf("MENU", ms);
   if ((was) && (m))
     {
	m->internal = 1;
	MenuShow(m, 1);
	ewin = FindEwinByMenu(m);
	if (ewin)
	  {
	     MoveEwin(ewin, lx, ly);
	     ShowEwin(ewin);
	  }
	mode.cur_menu[0] = m;
	mode.cur_menu_depth = 1;
	MenuShowMasker(m);
     }

   EDBUG_RETURN(m);
}

static void
ShowInternalMenu(Menu ** pm, MenuStyle ** pms, char *style,
		 Menu * (mcf) (char *name, MenuStyle * ms))
{
   Menu               *m = *pm;
   MenuStyle          *ms = *pms;

   EDBUG(5, "ShowInternalMenu");

   XUngrabPointer(disp, CurrentTime);

   if (!ms)
     {
	ms = FindItem(style, 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
	if (!ms)
	   ms = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
	if (!ms)
	   EDBUG_RETURN_;
	*pms = ms;
     }

   mode.cur_menu_mode = 1;

   *pm = m = RefreshInternalMenu(m, ms, mcf);
   if (m)
     {
	if (!FindEwinByMenu(m))
	   MenuShow(m, 0);
	mode.cur_menu[0] = m;
	mode.cur_menu_depth = 1;
	MenuShowMasker(m);
     }
   else
     {
	mode.cur_menu[0] = NULL;
	mode.cur_menu_depth = 0;
	MenuHideMasker();
     }

   EDBUG_RETURN_;
}

#if 0				/* Not used */
static Menu        *
RefreshTaskMenu(int desk)
{
   char                was = 0;
   int                 lx = 0, ly = 0;
   EWin               *ewin;

   EDBUG(5, "RefreshTaskMenu");
   if (task_menu[desk])
     {
	ewin = FindEwinByMenu(task_menu[desk]);
	if ((task_menu[desk]->win) && (ewin))
	  {
	     lx = ewin->x;
	     ly = ewin->y;
	     was = 1;
	  }
	MenuDestroy(task_menu[desk]);
     }
   task_menu[desk] = NULL;
   if (!task_menu_style)
     {
	EDBUG_RETURN(NULL);
     }
   task_menu[desk] = MenuCreateFromDesktopEWins("MENU", task_menu_style, desk);
   if ((was) && (task_menu[desk]))
     {
	task_menu[desk]->internal = 1;
	MenuShow(task_menu[desk], 1);
	ewin = FindEwinByMenu(task_menu[desk]);
	if (ewin)
	  {
	     MoveEwin(ewin, lx, ly);
	     ShowEwin(ewin);
	  }
	mode.cur_menu[0] = task_menu[desk];
	mode.cur_menu_depth = 1;
	MenuShowMasker(task_menu[desk]);
     }
   EDBUG_RETURN(task_menu[desk]);
}

void
ShowTaskMenu(void)
{
   EDBUG(5, "ShowTaskMenu");
   EDBUG_RETURN_;
}
#endif

void
ShowAllTaskMenu(void)
{
   static MenuStyle   *ms = NULL;
   static Menu        *m = NULL;

   EDBUG(5, "ShowAllTaskMenu");
   ShowInternalMenu(&m, &ms, "TASK_MENU", MenuCreateFromAllEWins);
   EDBUG_RETURN_;
}

void
ShowDeskMenu(void)
{
   static MenuStyle   *ms = NULL;
   static Menu        *m = NULL;

   EDBUG(5, "ShowDeskMenu");
   ShowInternalMenu(&m, &ms, "DESK_MENU", MenuCreateFromDesktops);
   EDBUG_RETURN_;
}

void
ShowGroupMenu(void)
{
   static MenuStyle   *ms = NULL;
   static Menu        *m = NULL;

   EDBUG(5, "ShowGroupMenu");
   ShowInternalMenu(&m, &ms, "GROUP_MENU", MenuCreateFromGroups);
   EDBUG_RETURN_;
}

void
MenusHide(void)
{
   int                 i;

   for (i = 0; i < mode.cur_menu_depth; i++)
     {
	if (!mode.cur_menu[i]->stuck)
	   MenuHide(mode.cur_menu[i]);
     }
   MenuHideMasker();
   mode.cur_menu_depth = 0;
   mode.cur_menu_mode = 0;
   clickmenu = 0;
}

Window
MenuWindow(Menu * m)
{
   return m->win;
}

/*
 * Menu event handlers
 */

int
MenusEventMouseDown(XEvent * ev)
{
   Menu               *m;
   MenuItem           *mi;
   EWin               *ewin;

   m = FindMenuItem(ev->xbutton.window, &mi);
   if (m == NULL)
      return 0;
   if (mi == NULL)
      goto exit;

   mode.cur_menu_mode = 1;

   mi->state = STATE_CLICKED;
   MenuDrawItem(m, mi, 1);

   if (mi->child && mi->child->shown == 0)
     {
	int                 mx, my;
	unsigned int        mw, mh;
	EWin               *ewin2;

	mode.cur_menu[0] = m;
	mode.cur_menu_depth = 1;
	MenuShowMasker(m);
	XUngrabPointer(disp, CurrentTime);
	ewin = FindEwinByMenu(m);
	if (ewin)
	  {
	     GetWinXY(mi->win, &mx, &my);
	     GetWinWH(mi->win, &mw, &mh);
#if 1				/* Whatgoesonhere ??? */
	     MenuShow(mi->child, 1);
	     ewin2 = FindEwinByMenu(mi->child);
	     if (ewin2)
	       {
		  MoveEwin(ewin2,
			   ewin->x + ewin->border->border.left + mx + mw,
			   ewin->y + ewin->border->border.top + my -
			   ewin2->border->border.top);
		  RaiseEwin(ewin2);
		  ShowEwin(ewin2);
		  if (conf.menuslide)
		     UnShadeEwin(ewin2);
		  mode.cur_menu[mode.cur_menu_depth++] = mi->child;
	       }
#else
	     ewin2 = FindEwinByMenu(mi->child);
	     if (!ewin2)
		MenuShow(mi->child, 1);
#endif
	  }
     }

 exit:
   return 1;
}

int
MenusEventMouseUp(XEvent * ev)
{
   Menu               *m;
   MenuItem           *mi;

   m = FindMenuItem(ev->xbutton.window, &mi);
   if ((m) && (mi->state))
     {
	mi->state = STATE_HILITED;
	MenuDrawItem(m, mi, 1);
	if ((mi->act_id) && (!mode.justclicked))
	  {
	     ActionsCall(mi->act_id, NULL, mi->params);
	     if (clickmenu)
	       {
		  MenusHide();
		  return 1;
	       }
	  }
     }

   if ((mode.cur_menu_mode) && (!clickmenu))
     {
	if (!m)
	  {
	     EWin               *ewin;
	     Window              ww;

	     ww = WindowAtXY(mode.x, mode.y);
	     if ((ewin = FindEwinByChildren(ww)))
	       {
		  int                 i;

		  for (i = 0; i < ewin->border->num_winparts; i++)
		    {
		       if (ww == ewin->bits[i].win)
			 {
			    if ((ewin->border->part[i].flags & FLAG_TITLE)
				&& (ewin->menu))
			      {
				 ewin->menu->stuck = 1;
				 i = ewin->border->num_winparts;
			      }
			 }
		    }
	       }
	  }
	MenusHide();
	return 1;
     }

   if ((mode.cur_menu_mode) && (!mode.justclicked))
     {
	MenusHide();
	return 1;
     }

   return 0;
}

struct _mdata
{
   Menu               *m;
   MenuItem           *mi;
   EWin               *ewin;
};

static void
SubmenuShowTimeout(int val, void *dat)
{
   int                 mx, my;
   unsigned int        mw, mh;
   EWin               *ewin2, *ewin;
   struct _mdata      *data;

   data = (struct _mdata *)dat;
   if (!data)
      return;
   if (!data->m)
      return;
   if (!FindEwinByMenu(data->m))
      return;

   GetWinXY(data->mi->win, &mx, &my);
   GetWinWH(data->mi->win, &mw, &mh);
   MenuShow(data->mi->child, 1);
   ewin2 = FindEwinByMenu(data->mi->child);
   if (ewin2)
     {
	MoveEwin(ewin2,
		 data->ewin->x + data->ewin->border->border.left + mx + mw,
		 data->ewin->y + data->ewin->border->border.top + my -
		 ewin2->border->border.top);
	RaiseEwin(ewin2);
	ShowEwin(ewin2);
	if (conf.menuslide)
	   UnShadeEwin(ewin2);
	if (mode.cur_menu[mode.cur_menu_depth - 1] != data->mi->child)
	   mode.cur_menu[mode.cur_menu_depth++] = data->mi->child;
	if (conf.menusonscreen)
	  {
	     EWin               *menus[256];
	     int                 fx[256];
	     int                 fy[256];
	     int                 tx[256];
	     int                 ty[256];
	     int                 i;
	     int                 xdist = 0, ydist = 0;

	     if (ewin2->x + ewin2->w > root.w)
		xdist = root.w - (ewin2->x + ewin2->w);
	     if (ewin2->y + ewin2->h > root.h)
		ydist = root.h - (ewin2->y + ewin2->h);
	     if ((xdist != 0) || (ydist != 0))
	       {
		  for (i = 0; i < mode.cur_menu_depth; i++)
		    {
		       menus[i] = NULL;
		       if (mode.cur_menu[i])
			 {
			    ewin = FindEwinByMenu(mode.cur_menu[i]);
			    if (ewin)
			      {
				 menus[i] = ewin;
				 fx[i] = ewin->x;
				 fy[i] = ewin->y;
				 tx[i] = ewin->x + xdist;
				 ty[i] = ewin->y + ydist;
			      }
			 }
		    }
		  SlideEwinsTo(menus, fx, fy, tx, ty, mode.cur_menu_depth,
			       conf.shadespeed);
		  if (conf.warpmenus)
		     XWarpPointer(disp, None, None, 0, 0, 0, 0, xdist, ydist);
	       }
	  }
     }
   val = 0;
}

int
MenusEventMouseIn(XEvent * ev)
{
   static struct _mdata mdata;
   Window              win = ev->xcrossing.window;
   Menu               *m;
   MenuItem           *mi;
   int                 i;

   int                 j;

   m = FindMenuItem(win, &mi);
   if (m == NULL)
      return 0;
   if (mi == NULL)
      goto exit;

   PagerHideAllHi();

   if ((win == mi->icon_win) && (ev->xcrossing.detail == NotifyAncestor))
      goto exit;
   if ((win == mi->win) && (ev->xcrossing.detail == NotifyInferior))
      goto exit;

   mi->state = STATE_HILITED;
   MenuDrawItem(m, mi, 1);

   RemoveTimerEvent("SUBMENU_SHOW");

   for (i = 0; i < mode.cur_menu_depth; i++)
     {
	if (mode.cur_menu[i] == m)
	  {
	     if ((!mi->child) ||
		 ((mi->child) && (mode.cur_menu[i + 1] != mi->child)))
	       {
		  for (j = i + 1; j < mode.cur_menu_depth; j++)
		     MenuHide(mode.cur_menu[j]);
		  mode.cur_menu_depth = i + 1;
		  i = mode.cur_menu_depth;
		  break;
	       }
	  }
     }

   if ((mi->child) && (!mi->child->shown) && (mode.cur_menu_mode))
     {
	EWin               *ewin;

	ewin = FindEwinByMenu(m);
	if (ewin)
	  {
	     mdata.m = m;
	     mdata.mi = mi;
	     mdata.ewin = ewin;
	     DoIn("SUBMENU_SHOW", 0.2, SubmenuShowTimeout, 0, &mdata);
	  }
     }

 exit:
   return 1;
}

int
MenusEventMouseOut(XEvent * ev)
{
   Window              win = ev->xcrossing.window;
   Menu               *m;
   MenuItem           *mi;

   m = FindMenuItem(win, &mi);
   if (m == NULL)
      return 0;
   if (mi == NULL)
      goto exit;

   if ((win == mi->icon_win) && (ev->xcrossing.detail == NotifyAncestor))
      goto exit;
   if ((win == mi->win) && (ev->xcrossing.detail == NotifyInferior))
      goto exit;

   mi->state = STATE_NORMAL;
   MenuDrawItem(m, mi, 1);

 exit:
   return 1;
}
