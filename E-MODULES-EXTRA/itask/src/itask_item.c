#include "e.h"
#include "e_mod_main.h"

static void _itask_item_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _itask_item_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _itask_item_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _itask_item_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _itask_item_cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _itask_item_cb_menu_post(void *data, E_Menu *m);
static void _itask_item_cb_drag_finished(E_Drag *drag, int dropped);
static void _itask_item_cb_menu_post(void *data, E_Menu *m);


void
itask_item_new(Itask *it, E_Border *bd)
{
   Itask_Item *ic;

   if (!itask_item_add_check(it, bd))
     return;

   if (itask_item_find(it, bd))
     return;

   e_object_ref(E_OBJECT(bd));

   ic = E_NEW(Itask_Item, 1);
   ic->itask = it;
   ic->border = bd;

   ic->o_holder = edje_object_add(evas_object_evas_get(it->o_box));

   if (it->ci->ibox_style)
     {
	if (!e_theme_edje_object_set(ic->o_holder, "base/theme/modules/itask", "modules/itask/icon"))
	  edje_object_file_set(ic->o_holder, itask_theme_path, "modules/itask/icon");
     }
   else
     {
	if (!e_theme_edje_object_set(ic->o_holder, "base/theme/modules/itask", "modules/itask/item"))
	  edje_object_file_set(ic->o_holder, itask_theme_path, "modules/itask/item");
     }

   itask_item_set_icon(ic);
   itask_item_set_label(ic);

   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_IN,   _itask_item_cb_mouse_in,  ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_OUT,  _itask_item_cb_mouse_out, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_DOWN, _itask_item_cb_mouse_down, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_UP,   _itask_item_cb_mouse_up, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_MOVE, _itask_item_cb_mouse_move, ic);

   it->items = eina_list_append(it->items, ic);

   itask_update(ic->itask);
}

void
itask_item_free(Itask_Item *ic)
{
   if (ic->o_icon) evas_object_del(ic->o_icon);
   if (ic->o_holder) evas_object_del(ic->o_holder);

   e_object_unref(E_OBJECT(ic->border));

   ic->itask->items = eina_list_remove(ic->itask->items, ic);

   itask_update(ic->itask);

   free(ic);
}

void
itask_item_set_icon(Itask_Item *ic)
{
   if (ic->o_icon) evas_object_del(ic->o_icon);

   ic->o_icon = e_border_icon_add(ic->border, evas_object_evas_get(ic->itask->o_box));
   edje_object_part_swallow(ic->o_holder, "icon", ic->o_icon);
   evas_object_pass_events_set(ic->o_icon, 1);
   evas_object_show(ic->o_icon);

   if (ic->border->iconic)
     itask_icon_signal_emit(ic, "iconify", "");
   if (ic->border->focused)
     itask_icon_signal_emit(ic, "focused", "");

}

void
itask_item_set_label(Itask_Item *ic)
{
   const char *title;

   title = e_border_name_get(ic->border);

   if ((!title) || (!title[0]))
     title  = D_("No name!!");

   edje_object_part_text_set(ic->o_holder, "label", title);
}

void
itask_icon_signal_emit(Itask_Item *ic, char *sig, char *src)
{
   if (ic->o_holder)
     edje_object_signal_emit(ic->o_holder, sig, src);
   if (ic->o_icon)
     edje_object_signal_emit(ic->o_icon, sig, src);
}

static void
_itask_item_cb_menu_post(void *data, E_Menu *m)
{
   if (!m) return;
   e_object_del(E_OBJECT(m));
}

static void
_itask_item_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_In *ev;
   Itask_Item *ic;
   ev = event_info;
   ic = data;

   itask_icon_signal_emit(ic, "active", "");
   if (ic->itask->ci->show_label)
     itask_icon_signal_emit(ic, "label_active", "");
}

static void
_itask_item_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Out *ev;
   Itask_Item *ic;

   ev = event_info;
   ic = data;

   itask_icon_signal_emit(ic, "passive", "");
   if (ic->itask->ci->show_label)
     itask_icon_signal_emit(ic, "label_passive", "");
}

static void
_itask_item_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   Itask_Item *ic;
   Evas_Coord x, y, w, h;
   E_Menu *mn = NULL;
   int cx, cy, cw, ch, dir;

   ev = event_info;
   ic = data;

   if (ev->button == 1)
     {
	ic->drag.x = ev->output.x;
	ic->drag.y = ev->output.y;
	ic->drag.start = 1;
	ic->drag.dnd = 0;
     }
   else if (ev->button == 3)
     {
	if (ic->border)
	  {
	     evas_object_geometry_get(ic->o_holder, &x, &y, &w, &h);
	     e_gadcon_canvas_zone_geometry_get(ic->itask->gcc->gadcon,
					       &cx, &cy, &cw, &ch);
	     x += cx;
	     y += cy;

	     e_int_border_menu_show(ic->border, x, y, 0, ev->timestamp);
	  }
     }
   else if (ev->button == 2)
     {
     }
   if (mn)
     {
	e_menu_post_deactivate_callback_set(mn, _itask_item_cb_menu_post, NULL);

	evas_object_geometry_get(ic->o_holder, &x, &y, &w, &h);
	e_gadcon_canvas_zone_geometry_get(ic->itask->gcc->gadcon,
					  &cx, &cy, &cw, &ch);
	x += cx;
	y += cy;

	dir = E_MENU_POP_DIRECTION_AUTO;
	switch (ic->itask->gcc->gadcon->orient)
	  {
	   case E_GADCON_ORIENT_TOP:
	   case E_GADCON_ORIENT_CORNER_TL:
	   case E_GADCON_ORIENT_CORNER_TR:
	      dir = E_MENU_POP_DIRECTION_DOWN;
	      break;
	   case E_GADCON_ORIENT_BOTTOM:
	   case E_GADCON_ORIENT_CORNER_BL:
	   case E_GADCON_ORIENT_CORNER_BR:
	      dir = E_MENU_POP_DIRECTION_UP;
	      break;
	   case E_GADCON_ORIENT_LEFT:
	   case E_GADCON_ORIENT_CORNER_LT:
	   case E_GADCON_ORIENT_CORNER_LB:
	      dir = E_MENU_POP_DIRECTION_RIGHT;
	      break;
	   case E_GADCON_ORIENT_RIGHT:
	   case E_GADCON_ORIENT_CORNER_RT:
	   case E_GADCON_ORIENT_CORNER_RB:
	      dir = E_MENU_POP_DIRECTION_LEFT;
	      break;
	   case E_GADCON_ORIENT_FLOAT:
	   case E_GADCON_ORIENT_HORIZ:
	   case E_GADCON_ORIENT_VERT:
	   default:
	      dir = E_MENU_POP_DIRECTION_AUTO;
	      break;
	  }
	e_menu_activate_mouse(mn,
			      e_util_zone_current_get(e_manager_current_get()),
			      x, y, w, h, dir, ev->timestamp);
     }
}

static void
_itask_item_cb_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   Itask_Item *ic;
   E_Desk *current_desk;

   ev = event_info;
   ic = data;

   if ((ev->button == 1) && (!ic->drag.dnd))
     {
	current_desk = e_desk_current_get(ic->border->zone);

	if ((ic->itask->ci->iconify_focused) &&
	    (ic->border == e_border_focused_get()))
	  e_border_iconify(ic->border);
	else
   	  {
	     if ((e_desk_current_get(ic->border->zone) != ic->border->desk) &&
		 (!ic->border->sticky))
	       e_desk_show(ic->border->desk);

	     if (ic->border->shaded)
	       e_border_unshade(ic->border, E_DIRECTION_UP);

	     if (ic->border->iconic)
	       e_border_uniconify(ic->border);
	     else
	       e_border_raise(ic->border);

	     e_border_focus_set(ic->border, 1, 1);
   	  }
     }
   ic->drag.start = 0;
}

static void
_itask_item_cb_mouse_move(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Move *ev;
   Itask_Item *ic;
   int dx, dy;

   ev = event_info;
   ic = data;
   if (!ic->drag.start)
     return;

   dx = ev->cur.output.x - ic->drag.x;
   dy = ev->cur.output.y - ic->drag.y;

   if (((dx * dx) + (dy * dy)) >
       (e_config->drag_resist * e_config->drag_resist))
     {
	E_Drag *d;
	Evas_Object *o;
	Evas_Coord x, y, w, h;
	const char *drag_types[] =
	  { "enlightenment/border" };

	ic->drag.dnd = 1;
	ic->drag.start = 0;

	evas_object_geometry_get(ic->o_icon, &x, &y, &w, &h);
	d = e_drag_new(ic->itask->gcc->gadcon->zone->container,
		       x, y, drag_types, 1,
		       ic->border, -1, NULL, _itask_item_cb_drag_finished);
	o = e_border_icon_add(ic->border, e_drag_evas_get(d));
	e_drag_object_set(d, o);

	e_drag_resize(d, w, h);
	e_drag_start(d, ic->drag.x, ic->drag.y);
	evas_event_feed_mouse_up(ic->itask->gcc->gadcon->evas,
				 1, EVAS_BUTTON_NONE,
				 ecore_x_current_time_get(), NULL);
	e_object_ref(E_OBJECT(ic->border));
     }
}

static void
_itask_item_cb_drag_finished(E_Drag *drag, int dropped)
{
   E_Border *bd;

   bd = drag->data;
   if (!dropped)
     {
	e_border_desk_set(bd, e_desk_current_get(bd->zone));
	if (bd->iconic)
	  e_border_uniconify(bd);
	else
	  e_border_raise(bd);
     }
   e_object_unref(E_OBJECT(bd));
}

Itask_Item *
itask_item_find(Itask *it, E_Border *bd)
{
   Eina_List *l;
   Itask_Item *ic;

   EINA_LIST_FOREACH(it->items, l, ic)
     if (ic->border == bd) return ic;

   return NULL;
}

int
itask_item_add_check(Itask *it, E_Border *bd)
{
   if ((it->ci->skip_dialogs && (bd->client.netwm.type == ECORE_X_WINDOW_TYPE_DIALOG)) ||
       (it->ci->skip_always_below_windows && (bd->layer == 50)) ||
       (bd->user_skip_winlist) ||
       (bd->client.netwm.state.skip_taskbar) ||
       ((it->ci->show_desk) && (!bd->desk->visible)))
     return 0;

   return 1;
}
