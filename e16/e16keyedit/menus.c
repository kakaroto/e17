#include "first.h"
#include "x.h"
#include "str.h"
#include "mem.h"
#include "util.h"
#include <Imlib2.h>
#include "events.h"
#include "ev_x.h"
#include "ev_filter.h"
#include "menus.h"

static E_Menu *open_menus = NULL;
static E_Menu_List *menus = NULL;

static void         e_menu_idle_handler(void *data);
static void         e_menu_event_handler(Eevent * ev);

E_Menu *
e_menu_new(void)
{
   static char         have_idle_handler = 0;
   E_Menu             *m;
   E_Menu_List        *l;
   
   
   if (!have_idle_handler)
     {
	e_event_filter_handler_add(EV_MOUSE_MOVE, e_menu_event_handler);
	e_event_filter_handler_add(EV_MOUSE_DOWN, e_menu_event_handler);
	e_event_filter_handler_add(EV_MOUSE_UP, e_menu_event_handler);
	e_event_filter_handler_add(EV_MOUSE_WHEEL, e_menu_event_handler);
	e_event_filter_handler_add(EV_KEY_DOWN, e_menu_event_handler);
	e_event_filter_handler_add(EV_KEY_UP, e_menu_event_handler);
	e_event_filter_handler_add(EV_MOUSE_IN, e_menu_event_handler);
	e_event_filter_handler_add(EV_MOUSE_OUT, e_menu_event_handler);
     }
   m = NEW(E_Menu, 1);
   m->name = NULL;
   m->win = e_window_override_new(0, 0, 0, 1, 1);
   m->pmap = 0;
   m->mask = 0;
   m->x = 0;
   m->y = 0;
   m->w = 0;
   m->h = 0;
   m->visible = 0;
   m->map_time = 0.0;
   m->items = NULL;
   m->next = NULL;
   m->needs_redraw = 1;
   l = NEW(E_Menu_List, 1);
   l->menu = m;
   l->next = menus;
   menus = l;   
   return m;
}


static void
e_menu_idle_handler(void *data)
{
   E_Menu_List *l;
   
   for (l = menus; l; l = l->next)
     {
	if (l->menu->needs_redraw)
	   e_menu_redraw(l->menu);
     }
}

static void
e_menu_event_handler(Eevent * ev)
{
   E_Menu_List *l;
   
   l = menus;
   while (l)
     {
	E_Menu *m;
	
	m = l->menu;
	l = l->next;
	e_menu_handle_event(m, ev);
     }
}

void
e_menu_handle_event(E_Menu *m, Eevent * ev)
{
   switch (ev->type)
     {
     case EV_MOUSE_MOVE:
	  {
	     Ev_Mouse_Move      *e;
	     
	     e = ev->event;
	  }
	break;
     case EV_MOUSE_DOWN:
	  {
	     Ev_Mouse_Down      *e;
	     
	     e = ev->event;
	  }
	break;
     case EV_MOUSE_UP:
	  {
	     Ev_Mouse_Up      *e;
	     
	     e = ev->event;
	  }
	break;
     case EV_MOUSE_WHEEL:
	  {
	     Ev_Wheel      *e;
	     
	     e = ev->event;
	  }
	break;
     case EV_KEY_DOWN:
	  {
	     Ev_Key_Down      *e;
	     
	     e = ev->event;
	  }
	break;
     case EV_KEY_UP:
	  {
	     Ev_Key_Up      *e;
	     
	     e = ev->event;
	  }
	break;
     case EV_MOUSE_IN:
	  {
	     Ev_Window_Enter      *e;
	     
	     e = ev->event;
	  }
	break;
     case EV_MOUSE_OUT:
	  {
	     Ev_Window_Leave      *e;
	     
	     e = ev->event;
	  }
	break;
     default:
	break;
     }
}

void
e_menu_show_at(E_Menu *m, int x, int y)
{
   m->visible = 1;
   e_window_move(m->win, x, y);
   e_menu_redraw(m);
   e_window_show(m->win);
}

void
e_menu_show_at_mouse_xy(E_Menu *m, int x, int y)
{
}

void
e_menu_show_at_submen(E_Menu *m, E_Menu *parent_m, int entry)
{
}

void
e_menu_hide(E_Menu *m)
{
   if (!m->visible)
      return;
   m->visible = 0;
   e_window_hide(m->win);
   if (m->next)
      e_menu_hide(m->next);
}

void
e_menu_free(E_Menu *m)
{
   IF_FREE(m->name);
   e_window_destroy(m->win);
   if (m->pmap)
      e_pixmap_free(m->pmap);
   if (m->mask)
      e_pixmap_free(m->mask);
   FREE(m);
}

E_Menu *
e_menu_find(char *name)
{
   E_Menu_List *l;
   
   for (l = menus; l; l = l->next)
     {
	if ((l->menu->name) && (e_string_cmp(l->menu->name, name)))
	   return l->menu;
     }
   return NULL;
}

void
e_menu_add_entry(E_Menu *m, char *text, char *icon, char *submenu, int onoff, void (*func) (void *data), void *data, void (*func_free) (void *data))
{
   E_Menu_Item *mi, *ptr;
   
   mi = NEW(E_Menu_Item, 1);
   mi->state = MENU_ITEM_STATE_NORMAL;
   mi->icon = e_string_dup(icon);
   mi->text = e_string_dup(text);
   mi->submenu = e_string_dup(submenu);
   if (onoff)
      MENU_ITEM_SET_OFF(mi);
   mi->func = func;
   mi->func_free = func_free;
   mi->data = data;
   mi->next = NULL;
   
   if (!m->items)
      m->items = mi;
   else
     {
	for (ptr = m->items; ptr; ptr = ptr->next)
	  {
	     if (!ptr->next)
	       {
		  ptr->next = mi;
		  break;
	       }
	  }
     }
   m->needs_redraw = 1;
}

void
e_menu_del_entry(E_Menu *m, E_Menu_Item *mi)
{
}

void
e_menu_redraw(E_Menu *m)
{
   if (!m->needs_redraw)
      return;
   m->needs_redraw = 0;
}

