#include "e.h"
#include "e_mod_main.h"

static void _itask_menu_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _itask_menu_cb_configuration(void *data, E_Menu *m, E_Menu_Item *mi);
static void _itask_menu_cb_post(void *data, E_Menu *m);
static void _itask_menu_cb_config_post(void *data, E_Menu *m);

static void _itask_menu_item_cb(void *data, E_Menu *m, E_Menu_Item *mi);
static void _itask_menu_icon_cb(void *data, E_Menu *m, E_Menu_Item *mi);
static void _itask_menu_remove(Itask *it);

void
itask_menu_button(Itask *it)
{
      Evas_Coord w, h;
      it->o_button = edje_object_add(evas_object_evas_get(it->o_box));
      evas_object_event_callback_add(it->o_button, EVAS_CALLBACK_MOUSE_DOWN, _itask_menu_cb_mouse_down, it);
      if (!e_theme_edje_object_set(it->o_button, "base/theme/modules/itask", "modules/itask/item"))
         edje_object_file_set(it->o_button, itask_theme_path, "modules/itask/item");

      edje_object_part_text_set(it->o_button, "label", "");
      evas_object_show(it->o_button);
      e_box_pack_start(it->o_box, it->o_button);
     Itask_Item *ic;
     ic = E_NEW(Itask_Item,1);
     ic->itask = it;
     ic->o_holder = it->o_button;
     it->menubutton = ic;
}

void
itask_menu_remove(Itask *it){
  //edje_object_part_unswallow(it->menubutton->o_holder, it->menubutton->o_icon);
   evas_object_del(it->menubutton->o_holder);
   E_FREE(it->menubutton);
}

/* taken from e_int_menus */
static void
_itask_menu_icon_cb(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Border *bd;
   Evas_Object *o;

   bd = data;
   E_OBJECT_CHECK(bd);

   o = e_icon_add(m->evas);
   e_icon_object_set(o, e_border_icon_add(bd, m->evas));

   mi->icon_object = o;
}


/* taken from e_int_menus */
static void
_itask_menu_item_cb(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Itask_Item *ic;
   E_Border *bd;


   ic = data;
   bd = ic->border;

   E_OBJECT_CHECK(bd);
   if (bd->iconic)
   {
      if (!bd->lock_user_iconify)
         e_border_uniconify(bd);
   }
   e_desk_show(bd->desk);
   if (!bd->lock_user_stacking)
      e_border_raise(bd);
}


static void
_itask_menu_cb_item_drag_finished(E_Drag *drag, int dropped)
{
   E_Border *bd;

   bd = drag->data;
   if (!dropped)
   {
      e_border_desk_set(bd, e_desk_current_get(bd->zone));
      if(bd->iconic)
         e_border_uniconify(bd);
      else
         e_border_raise(bd);
   }
   e_object_unref(E_OBJECT(bd));
}

static void
_e_int_menus_apps_drag(void *data, E_Menu *m, E_Menu_Item *mi)
{
   E_Border *bd;
   bd = data;

   /* start drag! */
   if (mi->icon_object)
      {
	 E_Drag *drag;
	 Evas_Object *o = NULL;
	 Evas_Coord x, y, w, h;
	 const char *drag_types[] = { "enlightenment/border" };

	 evas_object_geometry_get(mi->icon_object,
				     &x, &y, &w, &h);
	 drag = e_drag_new(m->zone->container, x, y,
			      drag_types, 1, bd, -1, NULL, _itask_menu_cb_item_drag_finished);

         o = e_border_icon_add(bd, e_drag_evas_get(drag));

	 e_drag_object_set(drag, o);
         e_drag_resize(drag, w, h);
         e_object_ref(E_OBJECT(bd));
	 e_drag_start(drag, mi->drag.x + w, mi->drag.y + h);
      }
}

E_Menu *
itask_menu_items_menu(Eina_List *items)
{
  Eina_List *l;
  Itask_Item *ic;
  E_Menu *mn = NULL;
  E_Menu_Item *mi;

  if(eina_list_count(items) == 0)
    return NULL;

  mn = e_menu_new();
  const char *title;

  for(l = items; l ; l = l->next)
  {
     ic = l->data;
     mi = e_menu_item_new(mn);
     title = e_border_name_get(ic->border);

     if ((title) && (title[0]))
        e_menu_item_label_set(mi, title);
     else
        e_menu_item_label_set(mi, N_("No name!!"));

     e_menu_item_label_set(mi, title);
     e_menu_item_callback_set(mi, _itask_menu_item_cb, ic);
     e_menu_item_realize_callback_set(mi, _itask_menu_icon_cb, ic->border);
     e_menu_item_drag_callback_set(mi, _e_int_menus_apps_drag, ic->border);
  }
  return mn;
}

static void
_itask_menu_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Down *ev;
   Itask *it;
   E_Menu *m;
   E_Menu_Item *mi;

   int cx, cy, cw, ch;
   Evas_Coord x, y, w, h;
   Instance *inst;

   int dir;

   it = data;
   inst = it->inst;
   ev = event_info;

   if (ev->button == 3)
   {
      m = e_menu_new();
      e_menu_post_deactivate_callback_set(m, _itask_menu_cb_config_post, NULL);

      m = e_menu_new();
      mi = e_menu_item_new(m);
      e_menu_item_label_set(mi, N_("Settings"));
      e_util_menu_item_theme_icon_set(mi, "enlightenment/configuration");
      e_menu_item_callback_set(mi, _itask_menu_cb_configuration, it);

      m = e_gadcon_client_util_menu_items_append(it->inst->gcc, m, 0);
   }
   else if (ev->button == 1)
   {
   	 if(it->menu_all_window)
	   m = itask_menu_items_menu(it->items);
   	 else
	   m = itask_menu_items_menu(it->items_menu);
   }
   if (m)
   {
   	  e_menu_post_deactivate_callback_set(m, _itask_menu_cb_post, NULL);
      evas_object_geometry_get(it->o_button, &x, &y, &w, &h);
      e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon,
                                        &cx, &cy, &cw, &ch);
      x += cx;
      y += cy;

      dir = E_MENU_POP_DIRECTION_AUTO;
      switch (inst->gcc->gadcon->orient)
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
      e_menu_activate_mouse(m,
                            e_util_zone_current_get(e_manager_current_get()),
                            x, y, w, h,
                            dir, ev->timestamp);

   }
}



static void
_itask_menu_cb_post(void *data, E_Menu *m)
{
	if (!m)
      return;
   e_object_del(E_OBJECT(m));
}



static void
_itask_menu_cb_config_post(void *data, E_Menu *m)
{
	if (!m)
      return;
   e_object_del(E_OBJECT(m));
}



static void
_itask_menu_cb_configuration(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Itask *it;
   Config_Item *ci;
   int ok = 1;
   Eina_List *l;

   it = data;
   ci = it->inst->ci;

   for (l = itask_config->config_dialog; l; l = l->next)
   {
      E_Config_Dialog *cfd;

      cfd = l->data;
      if (cfd->data == ci)
      {
         ok = 0;
         break;
      }
   }
   if (ok)
      _config_itask_module(ci);
}
