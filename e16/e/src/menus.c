/*
 * Copyright (C) 2000 Carsten Haitzler, Geoff Harrison and various contributors
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

static MenuStyle   *task_menu_style = NULL;
static MenuStyle   *desk_menu_style = NULL;
static MenuStyle   *group_menu_style = NULL;

static void         FileMenuUpdate(int val, void *data);
static void         FillFlatFileMenu(Menu * m, MenuStyle * ms, char *name,
				     char *file, Menu * parent);

void
ShowTaskMenu(void)
{
   EDBUG(5, "ShowTaskMenu");
   EDBUG_RETURN_;
}

void
ShowAllTaskMenu(void)
{
   static MenuStyle   *ms = NULL;
   static Menu        *m = NULL;

   EDBUG(5, "ShowAllTaskMenu");
   XUngrabPointer(disp, CurrentTime);
   if (!task_menu_style)
     {
	ms = FindItem("TASK_MENU", 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
	if (!ms)
	   ms = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
	if (!ms)
	   EDBUG_RETURN_;
     }
   task_menu_style = ms;
   mode.cur_menu_mode = 1;
   m = NULL;
   m = RefreshAllTaskMenu(all_task_menu);
   all_task_menu = m;
   if (m)
     {
	if (!FindEwinByMenu(m))
	   ShowMenu(m, 0);
	mode.cur_menu[0] = m;
	mode.cur_menu_depth = 1;
	ShowMenuMasker(m);
     }
   else
     {
	mode.cur_menu[0] = NULL;
	mode.cur_menu_depth = 0;
	HideMenuMasker();
     }
   EDBUG_RETURN_;
}

void
ShowDeskMenu(void)
{
   static MenuStyle   *ms = NULL;
   static Menu        *m = NULL;

   EDBUG(5, "ShowDeskMenu");
   XUngrabPointer(disp, CurrentTime);
   if (!desk_menu_style)
     {
	ms = FindItem("DESK_MENU", 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
	if (!ms)
	   ms = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
	if (!ms)
	   EDBUG_RETURN_;
     }
   desk_menu_style = ms;
   mode.cur_menu_mode = 1;
   m = NULL;
   m = RefreshDeskMenu(desk_menu);
   desk_menu = m;
   if (m)
     {
	if (!FindEwinByMenu(m))
	   ShowMenu(m, 0);
	mode.cur_menu[0] = m;
	mode.cur_menu_depth = 1;
	ShowMenuMasker(m);
     }
   else
     {
	mode.cur_menu[0] = NULL;
	mode.cur_menu_depth = 0;
	HideMenuMasker();
     }
   EDBUG_RETURN_;
}

Menu               *
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
	DestroyMenu(task_menu[desk]);
     }
   task_menu[desk] = NULL;
   if (!task_menu_style)
     {
	EDBUG_RETURN(NULL);
     }
   task_menu[desk] = CreateMenuFromDesktopEWins("MENU", task_menu_style, desk);
   if ((was) && (task_menu[desk]))
     {
	ShowMenu(task_menu[desk], 1);
	ewin = FindEwinByMenu(task_menu[desk]);
	if (ewin)
	  {
	     MoveEwin(ewin, lx, ly);
	     ShowEwin(ewin);
	  }
	mode.cur_menu[0] = task_menu[desk];
	mode.cur_menu_depth = 1;
	ShowMenuMasker(task_menu[desk]);
     }
   EDBUG_RETURN(task_menu[desk]);
}

Menu               *
RefreshAllTaskMenu(Menu * m)
{
   char                was = 0;
   int                 lx = 0, ly = 0;
   EWin               *ewin;

   EDBUG(5, "RefreshAllTaskMenu");
   if (m)
     {
	ewin = FindEwinByMenu(m);
	if ((m->win) && (ewin))
	  {
	     lx = ewin->x;
	     ly = ewin->y;
	     was = 1;
	  }
	DestroyMenu(m);
     }
   m = NULL;
   if (!task_menu_style)
     {
	EDBUG_RETURN(NULL);
     }
   m = CreateMenuFromAllEWins("MENU", task_menu_style);
   if ((was) && (m))
     {
	ShowMenu(m, 1);
	ewin = FindEwinByMenu(m);
	if (ewin)
	  {
	     MoveEwin(ewin, lx, ly);
	     ShowEwin(ewin);
	  }
	mode.cur_menu[0] = m;
	mode.cur_menu_depth = 1;
	ShowMenuMasker(m);
     }
   EDBUG_RETURN(m);
}

Menu               *
RefreshDeskMenu(Menu * m)
{
   char                was = 0;
   int                 lx = 0, ly = 0;
   EWin               *ewin;

   EDBUG(5, "RefreshDeskMenu");
   if (m)
     {
	ewin = FindEwinByMenu(m);
	if ((m->win) && (ewin))
	  {
	     lx = ewin->x;
	     ly = ewin->y;
	     was = 1;
	  }
	DestroyMenu(m);
     }
   m = NULL;
   if (!desk_menu_style)
     {
	EDBUG_RETURN(NULL);
     }
   m = CreateMenuFromDesktops("MENU", desk_menu_style);
   if ((was) && (m))
     {
	ShowMenu(m, 1);
	ewin = FindEwinByMenu(m);
	if (ewin)
	  {
	     MoveEwin(ewin, lx, ly);
	     ShowEwin(ewin);
	  }
	mode.cur_menu[0] = m;
	mode.cur_menu_depth = 1;
	ShowMenuMasker(m);
     }
   EDBUG_RETURN(m);
}

void
HideMenu(Menu * m)
{
   EWin               *ewin;

/*  XEvent              ev; */

   EDBUG(5, "HideMenu");
   if (m->win)
      EUnmapWindow(disp, m->win);
   ewin = FindEwinSpawningMenu(m);
   if (ewin)
      ewin->shownmenu = 0;
   ewin = FindEwinByMenu(m);
   if (ewin)
     {
	HideEwin(ewin);
/* FASTER ????
 * ev.xunmap.window = m->win;
 * HandleUnmap(&ev);
 */
     }
   if (m->sel_item)
     {
	m->sel_item->state = STATE_NORMAL;
	DrawMenuItem(m, m->sel_item, 1);
	m->sel_item = NULL;
     }
   m->stuck = 0;
   EDBUG_RETURN_;
}

void
ShowMenu(Menu * m, char noshow)
{
   EWin               *ewin;
   int                 x, y;
   int                 wx = 0, wy = 0;	/* wx, wy added to stop menus */
   unsigned int        w, h, mw, mh;	/* from appearing offscreen */
   int                 head_num = 0;

   EDBUG(5, "ShowMenu");
   if ((m->num <= 0) || (!m->style))
      EDBUG_RETURN_;
   if (m->stuck)
     {
	Button             *button;
	EWin               *ewin99;

	if ((button = FindButton(mode.context_win)))
	  {
	     button->state = STATE_NORMAL;
	     DrawButton(button);
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
      RealizeMenu(m);
   ewin = FindEwinByMenu(m);
   if (ewin)
     {
	if ((mode.button)
	    && FindItem((char *)mode.button, 0, LIST_FINDBY_POINTER,
			LIST_TYPE_BUTTON))
	  {
	     mode.button->state = STATE_NORMAL;
	     DrawButton(mode.button);
	  }
	RaiseEwin(ewin);
	EDBUG_RETURN_;
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
   if (mode.menusonscreen)
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
	if (mode.menusonscreen)
	   EMoveWindow(disp, m->win, wx, wy);
	else
	   EMoveWindow(disp, m->win, mode.x - x - (w / 2),
		       mode.y - y - (h / 2));
     }
   else if ((mode.x >= 0) && (mode.y < 0))
     {
	if (((-mode.y) + (int)mh) > (int)root.h)
	   mode.y = -((-mode.y) - mode.context_h - mh);
	if (mode.menusonscreen)
	   EMoveWindow(disp, m->win, wx, -mode.y);
	else
	   EMoveWindow(disp, m->win, mode.x - x - (w / 2), -mode.y);
     }
   else if ((mode.x < 0) && (mode.y >= 0))
     {
	if (((-mode.x) + (int)mw) > (int)root.w)
	   mode.x = -((-mode.x) - mode.context_w - mw);
	if (mode.menusonscreen)
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
   ewin =
      AddInternalToFamily(m->win, 1, m->style->border_name, EWIN_TYPE_MENU, m);
   if (ewin)
     {
	DesktopRemoveEwin(ewin);
	ewin->head = head_num;
	DesktopAddEwinToTop(ewin);
	if (ewin->desktop != 0)
	   MoveEwin(ewin, ewin->x - desks.desk[ewin->desktop].x,
		    ewin->y - desks.desk[ewin->desktop].y);
	RestackEwin(ewin);
	if (mode.menuslide)
	   InstantShadeEwin(ewin);
	ICCCM_Cmap(NULL);
	if (!noshow)
	  {
	     ShowEwin(ewin);
	     if (mode.menuslide)
		UnShadeEwin(ewin);
	  }
	ewin->menu = m;
	ewin->dialog = NULL;
     }
   m->stuck = 0;
   if (!FindMenu(m->win))
      AddItem(m, m->name, m->win, LIST_TYPE_MENU);
   {
      Button             *button;
      EWin               *ewin99;

      if ((button = FindButton(mode.context_win)))
	{
	   button->state = STATE_NORMAL;
	   DrawButton(button);
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
   EDBUG_RETURN_;
}

MenuStyle          *
CreateMenuStyle(void)
{
   MenuStyle          *ms;

   EDBUG(5, "CreateMenuStyle");
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
CreateMenuItem(char *text, ImageClass * iclass, int action_id,
	       char *action_params, Menu * child)
{
   MenuItem           *mi;

   EDBUG(5, "CreateMenuItem");
   mi = Emalloc(sizeof(MenuItem));

   mi->icon_iclass = iclass;
   if (iclass)
      iclass->ref_count++;
   mi->text = duplicate(text);
   mi->act_id = action_id;
   if (action_params)
     {
	mi->params = duplicate(action_params);
     }
   else
     {
	mi->params = NULL;
     }
   mi->child = child;
   mi->state = STATE_NORMAL;
   mi->win = 0;
   mi->pmap[0] = 0;
   mi->pmap[1] = 0;
   mi->pmap[2] = 0;
   mi->mask[0] = 0;
   mi->mask[1] = 0;
   mi->mask[2] = 0;
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
CreateMenu(void)
{
   Menu               *m;

   EDBUG(5, "CreateMenu");
   m = Emalloc(sizeof(Menu));
   m->ref_count = 0;
   m->name = NULL;
   m->title = NULL;
   m->style = NULL;
   m->num = 0;
   m->items = NULL;
   m->win = 0;
   m->pmap = 0;
   m->mask = 0;
   m->stuck = 0;
   m->parent = NULL;
   m->sel_item = NULL;
   m->data = NULL;
   m->ref_menu = NULL;
   m->last_change = 0;
   EDBUG_RETURN(m);
}

void
DestroyMenu(Menu * m)
{
   int                 i, j;
   char                s[4096];

   EDBUG(5, "DestroyMenu");
   if (!m)
      EDBUG_RETURN_;
   HideMenu(m);
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
		     DestroyMenu(m->items[i]->child);
	       }
	     if (m->items[i]->text)
		Efree(m->items[i]->text);
	     if (m->items[i]->params)
		Efree(m->items[i]->params);
	     for (j = 0; j < 3; j++)
	       {
		  if (m->items[i]->pmap[j])
		     Imlib_free_pixmap(id, m->items[i]->pmap[j]);
		  if (m->items[i]->mask[j])
		     Imlib_free_pixmap(id, m->items[i]->mask[j]);
	       }
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

   Efree(m);

   EDBUG_RETURN_;
}

/* NB - this doesnt free imageclasses if we created them for the menu
 * FIXME: so it will leak if we create new imageclasses and stop using
 * old ones for menu icons. we need to add some ref counting in menu icon
 * imageclasses to knw to free them when not used
 */
void
EmptyMenu(Menu * m)
{
   int                 i, j;

   EDBUG(5, "EmptyMenu");
   for (i = 0; i < m->num; i++)
     {
	if (m->items[i])
	  {
	     if (m->items[i]->child)
		DestroyMenu(m->items[i]->child);
	     if (m->items[i]->text)
		Efree(m->items[i]->text);
	     if (m->items[i]->params)
		Efree(m->items[i]->params);
	     for (j = 0; j < 3; j++)
	       {
		  if (m->items[i]->pmap[j])
		     Imlib_free_pixmap(id, m->items[i]->pmap[j]);
		  if (m->items[i]->mask[j])
		     Imlib_free_pixmap(id, m->items[i]->mask[j]);
	       }
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
RepackMenu(Menu * m)
{
   EWin               *ewin;
   unsigned int        w, h;

   EDBUG(5, "RepackMenu");
   ewin = FindEwinByMenu(m);
   if (m->win)
      RealizeMenu(m);
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
AddItemToMenu(Menu * menu, MenuItem * item)
{
   EDBUG(5, "AddItemToMenu");
   menu->num++;
   menu->items = Erealloc(menu->items, sizeof(MenuItem *) * menu->num);
   menu->items[menu->num - 1] = item;
   EDBUG_RETURN_;
}

void
AddTitleToMenu(Menu * menu, char *title)
{
   EDBUG(5, "AddTitleToMenu");
   if (menu->title)
      Efree(menu->title);
   menu->title = duplicate(title);
   EDBUG_RETURN_;
}

void
RealizeMenu(Menu * m)
{
   int                 i, maxh = 0, maxw =
      0, maxx1, maxx2, w, h, x, y, r, mmw, mmh;
   unsigned int        iw, ih;
   ImlibImage         *im;
   XSetWindowAttributes att;
   XTextProperty       xtp;
   char                pq, has_i, has_s;

   EDBUG(5, "RealizeMenu");
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
		  m->items[i]->icon_win =
		     ECreateWindow(m->items[i]->win, 0, 0, im->rgb_width,
				   im->rgb_height, 0);
		  EMapWindow(disp, m->items[i]->icon_win);
		  XChangeWindowAttributes(disp, m->items[i]->icon_win,
					  CWEventMask, &att);
		  m->items[i]->icon_w = im->rgb_width;
		  m->items[i]->icon_h = im->rgb_height;
		  if (im->rgb_height > maxh)
		     maxh = im->rgb_height;
		  if (im->rgb_width > maxx2)
		     maxx2 = im->rgb_width;
		  Imlib_destroy_image(id, im);
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
   if (!m->style->use_item_bg)
     {
	IclassApplyCopy(m->style->bg_iclass, m->win, mmw, mmh, 0, 0,
			STATE_NORMAL, &(m->pmap), &(m->mask));
	ESetWindowBackgroundPixmap(disp, m->win, m->pmap);
	EShapeCombineMask(disp, m->win, ShapeBounding, 0, 0, m->mask, ShapeSet);
	for (i = 0; i < m->num; i++)
	   DrawMenuItem(m, m->items[i], 0);
     }
   else
     {
	for (i = 0; i < m->num; i++)
	   DrawMenuItem(m, m->items[i], 0);
	PropagateShapes(m->win);
     }
   queue_up = pq;
   EDBUG_RETURN_;
}

void
DrawMenuItem(Menu * m, MenuItem * mi, char shape)
{
   GC                  gc;
   XGCValues           gcv;
   unsigned int        w, h;
   int                 x, y;
   char                pq;
   Pixmap              pmap, mask;

   EDBUG(5, "DrawMenuItem");
   pq = queue_up;
   queue_up = 0;
   if (!mi->pmap[(int)(mi->state)])
     {
	if (mi->text)
	  {
	     GetWinWH(mi->win, &w, &h);
	     GetWinXY(mi->win, &x, &y);
	     if (!m->style->use_item_bg)
	       {
		  mi->pmap[(int)(mi->state)] =
		     ECreatePixmap(disp, mi->win, w, h, id->x.depth);
		  gc = XCreateGC(disp, m->pmap, 0, &gcv);
		  XCopyArea(disp, m->pmap, mi->pmap[(int)(mi->state)], gc, x, y,
			    w, h, 0, 0);
		  mi->mask[(int)(mi->state)] = None;
		  if ((mi->state != STATE_NORMAL) || (mi->child))
		    {
		       pmap = 0;
		       mask = 0;
		       if (mi->child)
			  IclassApplyCopy(m->style->sub_iclass, mi->win, w, h,
					  0, 0, mi->state, &pmap, &mask);
		       else
			  IclassApplyCopy(m->style->item_iclass, mi->win, w, h,
					  0, 0, mi->state, &pmap, &mask);
		       if (mask)
			 {
			    XSetClipMask(disp, gc, mask);
			    XSetClipOrigin(disp, gc, 0, 0);
			 }
		       XCopyArea(disp, pmap, mi->pmap[(int)(mi->state)], gc, 0,
				 0, w, h, 0, 0);
		       Imlib_free_pixmap(id, pmap);
		       if (mask)
			  Imlib_free_pixmap(id, mask);
		    }
		  XFreeGC(disp, gc);
	       }
	     else
	       {
		  if (mi->child)
		     IclassApplyCopy(m->style->sub_iclass, mi->win, w, h, 0, 0,
				     mi->state, &(mi->pmap[(int)(mi->state)]),
				     &(mi->mask[(int)(mi->state)]));
		  else
		     IclassApplyCopy(m->style->item_iclass, mi->win, w, h, 0, 0,
				     mi->state, &(mi->pmap[(int)(mi->state)]),
				     &(mi->mask[(int)(mi->state)]));
	       }
	  }
     }
   if ((m->style->tclass) && (mi->text))
     {
	TextDraw(m->style->tclass, mi->pmap[(int)(mi->state)], 0, 0, mi->state,
		 mi->text, mi->text_x, mi->text_y, mi->text_w, mi->text_h, 17,
		 m->style->tclass->justification);
     }
   if (mi->text)
     {
	ESetWindowBackgroundPixmap(disp, mi->win, mi->pmap[(int)(mi->state)]);
	EShapeCombineMask(disp, mi->win, ShapeBounding, 0, 0,
			  mi->mask[(int)(mi->state)], ShapeSet);
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
CreateMenuFromDirectory(char *name, MenuStyle * ms, char *dir)
{
   Progressbar        *p = NULL;
   Menu               *m, *mm;
   int                 i, num;
   char              **list, s[4096], ss[4096], *ext, cs[4096];
   MenuItem           *mi;
   struct stat         st;
   const char         *chmap =
#ifndef __EMX__
      "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ-_";

#else
      "0123456789abcdefghijklmnopqrstuvwxyz€‚ƒ„…†‡ˆŠ‹ŒŽ‘’“”•–—˜™-_";

#endif
   FILE               *f;

   EDBUG(5, "CreateMenuFromDirectory");
   m = CreateMenu();
   m->name = duplicate(name);
   m->style = ms;
   if (stat(dir, &st) >= 0)
     {
	int                 aa, bb, cc;

#ifndef __EMX__
	aa = (int)st.st_ino;
#else
	list = ls(dir, &num);
	aa = (int)num;
	freestrlist(list, num);
#endif
	bb = (int)st.st_dev;
	cc = 0;
	if (st.st_mtime > st.st_ctime)
	   cc = st.st_mtime;
	else
	   cc = st.st_ctime;
	Esnprintf(cs, sizeof(cs),
		  "%s/cached/img/.%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
		  UserCacheDir(), chmap[(aa >> 0) & 0x3f],
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
#ifndef __EMX__
	     f = fopen(cs, "r");
#else
	     f = fopen(cs, "rt");
#endif
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
			    ImlibImage         *im;

			    word(s, 2, s2);
			    Esnprintf(ss, sizeof(ss), "%s/%s", dir, s2);
			    im = Imlib_load_image(id, ss);
			    if (im)
			      {
				 ImlibImage         *im2;
				 ImlibColor          icl;
				 char                tile = 1, keep_asp = 0;
				 int                 width, height;
				 int                 scalex = 0, scaley = 0;
				 int                 scr_asp, im_asp;
				 int                 w2, h2;
				 int                 maxw = 48, maxh = 48;
				 int                 justx = 512, justy = 512;

				 Esnprintf(s2, sizeof(s2), "%s/cached/img/%s",
					   UserCacheDir(), s3);
				 width = im->rgb_width;
				 height = im->rgb_height;
				 h2 = maxh;
				 w2 = (im->rgb_width * h2) / im->rgb_height;
				 if (w2 > maxw)
				   {
				      w2 = maxw;
				      h2 = (im->rgb_height * w2) /
					 im->rgb_width;
				   }
				 im2 = Imlib_clone_scaled_image(id, im, w2, h2);
				 Imlib_save_image_to_ppm(id, im2, s2);
				 Imlib_changed_image(id, im2);
				 Imlib_changed_image(id, im);
				 Imlib_kill_image(id, im2);
				 Imlib_kill_image(id, im);
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
				 icl.r = 0;
				 icl.g = 0;
				 icl.b = 0;
				 bg = CreateDesktopBG(s3, &icl, ss, tile,
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
			    ic->name = duplicate("`");
			    ic->norm.normal = CreateImageState();
			    Esnprintf(stmp, sizeof(stmp), "%s/cached/img/%s",
				      UserCacheDir(), s3);
			    ic->norm.normal->im_file = duplicate(stmp);
			    ic->norm.normal->unloadable = 1;
			    IclassPopulate(ic);
			    AddItem(ic, ic->name, 0, LIST_TYPE_ICLASS);
			    mi = CreateMenuItem(NULL, ic, ACTION_BACKGROUND_SET,
						s3, NULL);
			    AddItemToMenu(m, mi);
			 }
		    }
		  else if (!strcmp(ss, "EXE"))
		    {
		       word(s, 2, ss);
		       Esnprintf(s, sizeof(s), "%s/%s", dir, ss);
		       mi = CreateMenuItem(NULL, NULL, ACTION_EXEC, s, NULL);
		       AddItemToMenu(m, mi);
		    }
		  else if (!strcmp(ss, "DIR"))
		    {
		       char                tmp[4096];

		       word(s, 2, tmp);
		       Esnprintf(s, sizeof(s), "%s/%s:%s", dir, tmp, name);
		       Esnprintf(ss, sizeof(ss), "%s/%s", dir, tmp);
		       mm = CreateMenuFromDirectory(s, ms, ss);
		       mm->parent = m;
		       AddItem(mm, mm->name, mm->win, LIST_TYPE_MENU);
		       mi = CreateMenuItem(tmp, NULL, 0, NULL, mm);
		       AddItemToMenu(m, mi);
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
		  mm = CreateMenuFromDirectory(s, ms, ss);
		  mm->parent = m;
		  AddItem(mm, mm->name, mm->win, LIST_TYPE_MENU);
		  mi = CreateMenuItem(list[i], NULL, 0, NULL, mm);
		  AddItemToMenu(m, mi);
		  if (f)
		     fprintf(f, "DIR %s\n", list[i]);
	       }
/* that's it - people are stupid and have executable images and just */
/* don't get it - so I'm disablign this to save people from their own */
/* stupidity */
/*           else if (st.st_mode & (S_IXUSR | S_IXGRP | S_IXOTH))
 * {
 * mi = CreateMenuItem(list[i], NULL, ACTION_EXEC, ss, NULL);
 * AddItemToMenu(m, mi);
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

#ifndef __EMX__
		  aa = (int)st.st_ino;
#else
		  aa = (int)st.st_nlink;
#endif

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
		       ImlibImage         *im;

		       im = Imlib_load_image(id, ss);
		       if (im)
			 {
			    ImlibImage         *im2;
			    ImlibColor          icl;
			    char                tile = 1, keep_asp = 0;
			    int                 width, height, scalex =
			       0, scaley = 0;
			    int                 scr_asp, im_asp, w2, h2;
			    int                 maxw = 48, maxh = 48;

			    Esnprintf(s2, sizeof(s2), "%s/cached/img/%s",
				      UserCacheDir(), s3);
			    width = im->rgb_width;
			    height = im->rgb_height;
			    h2 = maxh;
			    w2 = (im->rgb_width * h2) / im->rgb_height;
			    if (w2 > maxw)
			      {
				 w2 = maxw;
				 h2 = (im->rgb_height * w2) / im->rgb_width;
			      }
			    im2 = Imlib_clone_scaled_image(id, im, w2, h2);
			    Imlib_save_image_to_ppm(id, im2, s2);
			    Imlib_changed_image(id, im2);
			    Imlib_changed_image(id, im);
			    Imlib_kill_image(id, im2);
			    Imlib_kill_image(id, im);
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
			    icl.r = 0;
			    icl.g = 0;
			    icl.b = 0;
			    bg = CreateDesktopBG(s3, &icl, ss, tile, keep_asp,
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
		       ic->name = duplicate("`");
		       ic->norm.normal = CreateImageState();
		       Esnprintf(stmp, sizeof(stmp), "%s/cached/img/%s",
				 UserCacheDir(), s3);
		       ic->norm.normal->im_file = duplicate(stmp);
		       ic->norm.normal->unloadable = 1;
		       IclassPopulate(ic);
		       AddItem(ic, ic->name, 0, LIST_TYPE_ICLASS);
		       mi = CreateMenuItem(NULL, ic, ACTION_BACKGROUND_SET, s3,
					   NULL);
		       AddItemToMenu(m, mi);
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
CreateMenuFromFlatFile(char *name, MenuStyle * ms, char *file, Menu * parent)
{
   Menu               *m;
   char                s[4096], *ff = NULL;
   static int          calls = 0;

   EDBUG(5, "CreateMenuFromFlatFile");
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
	m = CreateMenu();
	m->name = duplicate(name);
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

#ifndef __EMX__
   f = fopen(file, "r");
#else
   f = fopen(file, "rt");
#endif
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
		       AddTitleToMenu(m, wd);
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
			    icc->name = duplicate(wd);
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

			    mi = CreateMenuItem(txt, icc, ACTION_EXEC, params,
						NULL);
			    AddItemToMenu(m, mi);
			 }
		    }
		  else if ((act) && (!strcmp(act, "menu")) && (params))
		    {
		       Esnprintf(wd, sizeof(wd), "__FM.%s.%i", name, count);
		       count++;
		       mm = CreateMenuFromFlatFile(wd, ms, params, parent);
		       if (mm)
			 {
			    mm->parent = m;
			    AddItem(mm, mm->name, mm->win, LIST_TYPE_MENU);
			    mi = CreateMenuItem(txt, icc, 0, NULL, mm);
			    AddItemToMenu(m, mi);
			 }
		    }
		  else
		    {
		       mi = CreateMenuItem(txt, icc, 0, NULL, NULL);
		       AddItemToMenu(m, mi);
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
	HideMenu(m);
	EmptyMenu(m);
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
	EmptyMenu(mm);
	FillFlatFileMenu(mm, mm->style, mm->name, mm->data, mm);
	RepackMenu(mm);
	return;
     }
   Esnprintf(s, sizeof(s), "__.%s", m->name);
   DoIn(s, 2.0, FileMenuUpdate, 0, m);
   val = 0;
}

Menu               *
CreateMenuFromGnome(char *name, MenuStyle * ms, char *dir)
{
   Menu               *m, *mm;
   int                 i, num;
   char              **list, s[4096], ss[4096];

   MenuItem           *mi;
   FILE               *f;
   char               *lang, name_buf[20];

   EDBUG(5, "CreateMenuFromGnome");

   if ((lang = setlocale(LC_MESSAGES, NULL)) != NULL)
      Esnprintf(name_buf, sizeof(name_buf), "Name[%s]=", lang);
   else
      name_buf[0] = '\0';

   m = CreateMenu();
   m->name = duplicate(name);
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
		  mm = CreateMenuFromGnome(s, ms, ss);
		  mm->parent = m;
		  AddItem(mm, mm->name, mm->win, LIST_TYPE_MENU);
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
		  mi = CreateMenuItem(name, NULL, 0, NULL, mm);
		  AddItemToMenu(m, mi);
	       }
	     else
	       {
#ifndef __EMX__
		  f = fopen(ss, "r");
#else
		  f = fopen(ss, "rt");
#endif
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
			       en_name = duplicate(&(s[strlen("Name=")]));
			    else if (name_buf[0]
				     && !strncmp(s, name_buf, strlen(name_buf)))
			       iname = duplicate(&(s[strlen(name_buf)]));
			    else if (!strncmp
				     (s, "TryExec=", strlen("TryExec=")))
			       texec = duplicate(&(s[strlen("TryExec=")]));
			    else if (!strncmp(s, "Exec=", strlen("Exec=")))
			       exec = duplicate(&(s[strlen("Exec=")]));
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

				 mi = CreateMenuItem(iname, NULL, ACTION_EXEC,
						     exec, NULL);
				 AddItemToMenu(m, mi);
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
CreateMenuFromThemes(char *name, MenuStyle * ms)
{
   Menu               *m;
   char              **lst;
   int                 i, num;
   char                ss[4096], *s;

   MenuItem           *mi;

   EDBUG(5, "CreateMenuFromThemes");

   m = CreateMenu();
   m->name = duplicate(name);
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
	     mi = CreateMenuItem(s, NULL, ACTION_EXIT, ss, NULL);
	     AddItemToMenu(m, mi);
	     Efree(s);
	  }
	freestrlist(lst, i);
     }
   EDBUG_RETURN(m);
}

int
BorderNameCompare(Border * b1, Border * b2)
{
   if (b1 && b2)
      return strcmp(b1->name, b2->name);

   return 0;
}

Menu               *
CreateMenuFromBorders(char *name, MenuStyle * ms)
{
   Menu               *m;
   Border            **lst;
   int                 i, num;

   MenuItem           *mi;

   EDBUG(5, "CreateMenuFromBorders");

   m = CreateMenu();
   m->name = duplicate(name);
   m->style = ms;
   lst = (Border **) ListItemType(&num, LIST_TYPE_BORDER);
   if (lst)
      Quicksort((void **)lst, 0, num - 1,
		(int (*)(void *, void *))&BorderNameCompare);
   for (i = 0; i < num; i++)
     {
	/* if its not internal (ie doesnt start with _ ) */
	if (lst[i]->name[0] != '_')
	  {
	     mi = CreateMenuItem(lst[i]->name, NULL, ACTION_SET_WINDOW_BORDER,
				 lst[i]->name, NULL);
	     AddItemToMenu(m, mi);
	  }
     }
   if (lst)
      Efree(lst);
   EDBUG_RETURN(m);
}

Menu               *
CreateMenuFromAllEWins(char *name, MenuStyle * ms)
{
   Menu               *m;
   EWin              **lst;
   int                 i, num;
   char                s[256];

   MenuItem           *mi;

   EDBUG(5, "CreateMenuFromEWins");
   m = CreateMenu();
   m->name = duplicate(name);
   m->style = ms;

   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     if ((!lst[i]->menu) && (!lst[i]->pager) && (!lst[i]->skipwinlist)
		 && (lst[i]->client.title) && (!lst[i]->ibox))
	       {
		  Esnprintf(s, sizeof(s), "%i", (int)(lst[i]->client.win));
		  mi = CreateMenuItem(lst[i]->client.title, NULL,
				      ACTION_FOCUS_SET, s, NULL);
		  AddItemToMenu(m, mi);
	       }
	  }
	Efree(lst);
     }
   EDBUG_RETURN(m);
}

Menu               *
CreateMenuFromDesktopEWins(char *name, MenuStyle * ms, int desk)
{
   Menu               *m;
   EWin              **lst;
   int                 i, num;
   char                s[256];

   MenuItem           *mi;

   EDBUG(5, "CreateMenuFromEWins");
   m = CreateMenu();
   m->name = duplicate(name);
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
		  mi = CreateMenuItem(lst[i]->client.title, NULL,
				      ACTION_FOCUS_SET, s, NULL);
		  AddItemToMenu(m, mi);
	       }
	  }
	Efree(lst);
     }
   EDBUG_RETURN(m);
   desk = 0;
}

Menu               *
CreateMenuFromDesktops(char *name, MenuStyle * ms)
{
   Menu               *m, *mm;
   EWin              **lst;
   int                 j, i, num;
   char                s[256];

   MenuItem           *mi;

   EDBUG(5, "CreateMenuFromEWins");
   m = CreateMenu();
   m->name = duplicate(name);
   m->style = ms;
   lst = (EWin **) ListItemType(&num, LIST_TYPE_EWIN);
   for (j = 0; j < mode.numdesktops; j++)
     {
	mm = CreateMenu();
	mm->name = duplicate("__SUBMENUDESK_E");
	mm->style = ms;
	Esnprintf(s, sizeof(s), "%i", j);
	mi = CreateMenuItem(_("Go to this Desktop"), NULL, ACTION_GOTO_DESK, s,
			    NULL);
	AddItemToMenu(mm, mi);
	for (i = 0; i < num; i++)
	  {
	     if (((lst[i]->desktop == j) || (lst[i]->sticky)) && (!lst[i]->menu)
		 && (!lst[i]->pager) && (!lst[i]->skipwinlist)
		 && (lst[i]->client.title) && (!lst[i]->ibox))
	       {
		  Esnprintf(s, sizeof(s), "%i", (int)(lst[i]->client.win));
		  mi = CreateMenuItem(lst[i]->client.title, NULL,
				      ACTION_FOCUS_SET, s, NULL);
		  AddItemToMenu(mm, mi);
	       }
	  }
	mm->parent = m;
	Esnprintf(s, sizeof(s), _("Desktop %i"), j);
	mi = CreateMenuItem(s, NULL, 0, NULL, mm);
	AddItemToMenu(m, mi);
     }
   if (lst)
      Efree(lst);
   EDBUG_RETURN(m);
}

void
ShowMenuMasker(Menu * m)
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
HideMenuMasker(void)
{
   if (mode.menu_cover_win)
     {
	EDestroyWindow(disp, mode.menu_cover_win);
	mode.menu_cover_win = 0;
	mode.menu_win_covered = 0;
     }
}

Menu               *
CreateMenuFromGroups(char *name, MenuStyle * ms)
{
   Menu               *m, *mm;
   Group             **lst;
   int                 i, j, num;
   char                s[256];

   MenuItem           *mi;

   EDBUG(5, "CreateMenuFromEWins");
   m = CreateMenu();
   m->name = duplicate(name);
   m->style = ms;
   lst = (Group **) ListItemType(&num, LIST_TYPE_GROUP);
   if (lst)
     {
	for (i = 0; i < num; i++)
	  {
	     mm = CreateMenu();
	     mm->name = duplicate("__SUBMENUGROUP_E");
	     mm->style = ms;
	     Esnprintf(s, sizeof(s), "%i", lst[i]->members[0]->client.win);
	     mi = CreateMenuItem(_("Show/Hide this group"), NULL,
				 ACTION_SHOW_HIDE_GROUP, s, NULL);
	     AddItemToMenu(mm, mi);
	     mi = CreateMenuItem(_("Iconify this group"), NULL, ACTION_ICONIFY,
				 s, NULL);
	     AddItemToMenu(mm, mi);

	     for (j = 0; j < lst[i]->num_members; j++)
	       {
		  Esnprintf(s, sizeof(s), "%i", lst[i]->members[j]->client.win);
		  mi = CreateMenuItem(lst[i]->members[j]->client.title, NULL,
				      ACTION_FOCUS_SET, s, NULL);
		  AddItemToMenu(mm, mi);
	       }
	     mm->parent = m;
	     Esnprintf(s, sizeof(s), _("Group %i"), i);
	     mi = CreateMenuItem(s, NULL, 0, NULL, mm);
	     AddItemToMenu(m, mi);
	  }
	Efree(lst);
     }
   EDBUG_RETURN(m);
}

Menu               *
RefreshGroupMenu(Menu * m)
{
   char                was = 0;
   int                 lx = 0, ly = 0;

   EWin               *ewin;

   EDBUG(5, "RefreshGroupMenu");
   if (m)
     {
	ewin = FindEwinByMenu(m);
	if ((m->win) && (ewin))
	  {
	     lx = ewin->x;
	     ly = ewin->y;
	     was = 1;
	  }
	DestroyMenu(m);
     }
   m = NULL;
   if (!group_menu_style)
     {
	EDBUG_RETURN(NULL);
     }
   m = CreateMenuFromGroups("MENU", group_menu_style);
   if ((was) && (m))
     {
	ShowMenu(m, 1);
	ewin = FindEwinByMenu(m);
	if (ewin)
	  {
	     MoveEwin(ewin, lx, ly);
	     ShowEwin(ewin);
	  }
	mode.cur_menu[0] = m;
	mode.cur_menu_depth = 1;
	ShowMenuMasker(m);
     }
   EDBUG_RETURN(m);
}

void
ShowGroupMenu(void)
{
   static MenuStyle   *ms = NULL;
   static Menu        *m = NULL;

   EDBUG(5, "ShowGroupMenu");
   XUngrabPointer(disp, CurrentTime);
   if (!group_menu_style)
     {
	ms = FindItem("GROUP_MENU", 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
	if (!ms)
	   ms = FindItem("DEFAULT", 0, LIST_FINDBY_NAME, LIST_TYPE_MENU_STYLE);
	if (!ms)
	   EDBUG_RETURN_;
     }
   group_menu_style = ms;
   mode.cur_menu_mode = 1;
   m = NULL;
   m = RefreshGroupMenu(group_menu);
   group_menu = m;
   if (m)
     {
	if (!FindEwinByMenu(m))
	   ShowMenu(m, 0);
	mode.cur_menu[0] = m;
	mode.cur_menu_depth = 1;
	ShowMenuMasker(m);
     }
   else
     {
	mode.cur_menu[0] = NULL;
	mode.cur_menu_depth = 0;
	HideMenuMasker();
     }
   EDBUG_RETURN_;
}
