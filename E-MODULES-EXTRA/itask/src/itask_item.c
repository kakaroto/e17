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

static Itask_Item *_itask_item_find(Itask *it, E_Border *bd);

static Itask_Item *_itask_item_get_youngest(Eina_List *list);
static Itask_Item *_itask_item_get_oldest(Eina_List *list);

void
itask_item_add_to_bar(Itask_Item *ic)
{
   Itask *it = ic->itask;
   Eina_List *l;
   Itask_Item *l_ic = NULL;

   ic->in_bar = 1;
   
   if (it->num_items == 0 && it->ci->hide_menu_button){
      itask_menu_button(it);
   }

   it->num_items++;

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
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_IN,   _itask_item_cb_mouse_in,  ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_OUT,  _itask_item_cb_mouse_out, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_DOWN, _itask_item_cb_mouse_down, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_UP,   _itask_item_cb_mouse_up, ic);
   evas_object_event_callback_add(ic->o_holder, EVAS_CALLBACK_MOUSE_MOVE, _itask_item_cb_mouse_move, ic);
   evas_object_show(ic->o_holder);

   if (!ic->o_icon)
     itask_item_set_icon(ic);
   else
     {
	edje_object_part_swallow(ic->o_holder, "icon", ic->o_icon);
	evas_object_pass_events_set(ic->o_icon, 1);
	evas_object_show(ic->o_icon);

	if (ic->border->iconic)
	  itask_icon_signal_emit(ic, "iconify", "");
	if (ic->border->focused)
	  itask_icon_signal_emit(ic, "focused", "");
     }

   itask_item_set_label(ic);

   if (ic->border->client.icccm.class)
     {	
	EINA_LIST_FOREACH(it->items_bar, l, l_ic)
	  {
	     if (l_ic->border->client.icccm.class
		 && !strcmp(l_ic->border->client.icccm.class,
			    ic->border->client.icccm.class))
	       break;
	     l_ic = NULL;
	  }
     }
   
   if (l_ic)
     {
	e_box_pack_before(it->o_box, ic->o_holder, l_ic->o_holder);
     }
   else
     {
	e_box_pack_after(it->o_box, ic->o_holder, it->o_button);
     }

   e_box_freeze(it->o_box);
   e_box_pack_options_set(ic->o_holder,
			  1, 1, /* fill */
			  1, 1, /* expand */
			  0.0, 0.5, /* align */
			  0, 0, /* min */
			  -1, -1 /* max */
			  );
   e_box_thaw(it->o_box);

   it->items_bar = eina_list_append(it->items_bar, ic);
}

void
itask_item_remove_from_bar(Itask_Item *ic)
{
   Itask *it = ic->itask;
   it->num_items--;
   if ((it->num_items == 0) && it->ci->hide_menu_button)
     itask_menu_remove(it);

   ic->in_bar = 0;
   itask_item_del_icon(ic);

   edje_object_part_unswallow(ic->o_holder, ic->o_icon);
   evas_object_del(ic->o_holder);

   it->items_bar = eina_list_remove(it->items_bar, ic);
}

Itask_Item *
itask_item_new(Itask *it, E_Border *bd)
{
   Itask_Item *ic;
   Eina_List *group = NULL;
   if (!itask_item_add_check(it, bd) || _itask_item_find(it, bd))
     return NULL;

   ic = E_NEW(Itask_Item, 1);
   e_object_ref(E_OBJECT(bd));

   ic->itask = it;
   ic->border = bd;
   ic->last_time = ecore_time_get();
   ic->items = NULL;

   it->items = eina_list_append(it->items, ic);

   if (ic->border->client.icccm.class)
     {
	if ((group = eina_hash_find(it->item_groups, ic->border->client.icccm.class)))
	  {
	     group = eina_list_append(group, ic);
	  }
	else
	  {
	     group = eina_list_append(group, ic);
	     eina_hash_add(it->item_groups, ic->border->client.icccm.class, group);
	  }
     }
   return ic;
}

int
itask_item_realize(Itask_Item *ic)
{
   Itask *it;
   int resize = 1;

   it = ic->itask;
   
   if (eina_list_data_find(it->items_bar, ic))
     return 0;

   if (it->num_items >= it->ci->max_items)
     {
	itask_item_swap_oldest_from_bar(it);
	edje_object_signal_emit(it->o_button, "focused", "");
     }
   itask_item_add_to_bar(ic);

   if (resize)
     itask_update_gc_orient(it);

   return 1;
}

void
itask_item_remove(Itask_Item *ic)
{
   Itask *it;
   Eina_List *group;

   it = ic->itask;

   it->items = eina_list_remove(it->items, ic);

   if (ic->in_bar)
     itask_item_remove_from_bar(ic);
   else
     it->items_menu = eina_list_remove(it->items_menu, ic);

   if (ic->border->client.icccm.class)
     {
	if ((group = eina_hash_find(it->item_groups, ic->border->client.icccm.class)))
	  {
	     group = eina_list_remove(group, ic);

	     if (eina_list_count(group) == 0)
	       {		  
		  eina_hash_del(it->item_groups, ic->border->client.icccm.class, group);
	       }
	     else
	       {
		  eina_hash_modify(it->item_groups, ic->border->client.icccm.class, group);
	       }
	  }
     }

   e_object_unref(E_OBJECT(ic->border));
   free(ic);
}

void
itask_item_swap_oldest_from_bar(Itask *it)
{
   Itask_Item *ic;

   ic = _itask_item_get_oldest(it->items_bar);
   if (ic)
     {
	itask_item_remove_from_bar(ic);

	it->items_menu = eina_list_append(it->items_menu, ic);
	edje_object_signal_emit(it->o_button, "focused", "");
     }
}

void
itask_item_swap_youngest_from_menu(Itask *it)
{
   Itask_Item *ic;
   ic = _itask_item_get_youngest(it->items_menu);
   if (ic) itask_item_swap_to_bar(ic);
}

void
itask_item_swap_to_bar(Itask_Item *ic)
{
   Itask *it = ic->itask;

   ic->last_time = ecore_time_get();
   itask_item_add_to_bar(ic);
   it->items_menu = eina_list_remove(it->items_menu, ic);

   if (!it->items_menu)
     {
	edje_object_signal_emit(it->o_button, "unfocused", "");
	//edje_object_part_text_set(it->o_button, "label", " ");
     }
}

void
itask_item_set_icon(Itask_Item *ic)
{
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
itask_item_del_icon(Itask_Item *ic)
{
   if (ic->o_icon)
     evas_object_del(ic->o_icon);
   if (ic->o_icon2)
     evas_object_del(ic->o_icon2);
   ic->o_icon = NULL;
   ic->o_icon2 = NULL;
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
   if (!m)
     return;
   e_object_del(E_OBJECT(m));
}

static void
_itask_item_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_In *ev;
   Itask_Item *ic;
   ev = event_info;
   ic = data;

   /* TODO a popup that shows the label*/
   /* it = ic->itask;
      E_Container *con;
      if (!(con = e_container_current_get(e_manager_current_get()))) return;

      if (it->item_label_popup)
      {
      e_popup_hide(it->item_label_popup);
      e_object_del(E_OBJECT(it->item_label_popup));
      it->item_label_popup = NULL;
      }

      it->item_label_popup = e_popup_new(e_zone_current_get(con), 0, 0, 100, 100);

      e_popup_show(it->item_label_popup);
   */
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
   /*
     it = ic->itask;
     if (it->item_label_popup)
     {
     e_popup_hide(it->item_label_popup);
     e_object_del(E_OBJECT(it->item_label_popup));
     it->item_label_popup = NULL;
     }*/
   itask_icon_signal_emit(ic, "passive", "");
   if (ic->itask->ci->show_label)
     itask_icon_signal_emit(ic, "label_passive", "");
}

/* static void
 * _e_int_menus_apps_drag(void *data, E_Menu *m, E_Menu_Item *mi)
 * {
 *    E_Border *bd;
 *    bd = data;
 * 
 *    /\* start drag! *\/
 *    if (mi->icon_object)
 *      {
 * 	E_Drag *drag;
 * 	Evas_Object *o = NULL;
 * 	Evas_Coord x, y, w, h;
 * 	const char *drag_types[] = { "enlightenment/border" };
 * 
 * 	evas_object_geometry_get(mi->icon_object,
 * 				 &x, &y, &w, &h);
 * 	drag = e_drag_new(m->zone->container, x, y,
 * 			  drag_types, 1, bd, -1, NULL, _itask_item_cb_drag_finished);
 * 
 * 	o = e_border_icon_add(bd, e_drag_evas_get(drag));
 * 
 * 	e_drag_object_set(drag, o);
 * 	e_drag_resize(drag, w, h);
 * 	e_object_ref(E_OBJECT(bd));
 * 	e_drag_start(drag, mi->drag.x + w, mi->drag.y + h);
 *      }
 * } */

static void
_itask_item_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   Itask_Item *ic;
   Evas_Coord x, y, w, h;
   E_Menu *mn = NULL;
   int cx, cy, cw, ch, dir;
   Eina_List *group;
   
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
	// E_Border *bd = ic->border;
	if (ic->border)
	  {
	     // TODO:
	     // Disable own menu for now, no time atm to get into why
	     // itasks menu doesnt work anymore
	     //  mn = e_menu_new();
	     //  mn = itask_border_menu_get(ic->border,mn);
	     evas_object_geometry_get(ic->o_holder, &x, &y, &w, &h);
	     e_gadcon_canvas_zone_geometry_get(ic->itask->gcc->gadcon,
					       &cx, &cy, &cw, &ch);
	     x += cx;
	     y += cy;

	     e_int_border_menu_show(ic->border, x, y, 0, ev->timestamp);
	  }
     }
   else if (ev->button == 2) // && itask->option_group_items
     {
	group = eina_hash_find(ic->itask->item_groups, ic->border->client.icccm.class);
	if (group)
	  mn = itask_menu_items_menu(group);
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
			      x, y, w, h,
			      dir, ev->timestamp);
	// edje_object_signal_emit(inst->o_button, "e,state,focused", "e");
	//e_util_evas_fake_mouse_up_later(inst->gcc->gadcon->evas,
	//                                ev->button);
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

static Itask_Item *
_itask_item_find(Itask *it, E_Border *bd)
{
   Eina_List *l;
   Itask_Item *ic;

   EINA_LIST_FOREACH(it->items, l, ic)
     {
	if (ic->border == bd)
	  return ic;
     }
   return NULL;
}

int
itask_item_add_check(Itask *it, E_Border *bd)
{
   if ((it->ci->skip_dialogs && bd->client.netwm.type == ECORE_X_WINDOW_TYPE_DIALOG) ||
       (it->ci->skip_always_below_windows && (bd->layer == 50)) ||
       (bd->user_skip_winlist) ||
       (bd->client.netwm.state.skip_taskbar) ||
       ((it->ci->show_desk) && (!bd->desk->visible)))
     return 0;

   return 1;
}

static Itask_Item *
_itask_item_get_oldest(Eina_List *list)
{
   Eina_List *l;
   Itask_Item *ic, *ic2;

   ic = eina_list_data_get(list);
   if (!ic) return NULL;
   
   EINA_LIST_FOREACH(list, l, ic2)
     {
	if (ic2->last_time < ic->last_time)
	  ic = ic2;
     }
   
   return ic;
}

static Itask_Item *
_itask_item_get_youngest(Eina_List *list)
{
   Eina_List *l;
   Itask_Item *ic, *ic2;

   ic = eina_list_data_get(list);
   if (!ic) return NULL;

   EINA_LIST_FOREACH(list, l, ic2)
     {
	if (ic2->last_time > ic->last_time)
	  ic = ic2;
     }

   return ic;
}
