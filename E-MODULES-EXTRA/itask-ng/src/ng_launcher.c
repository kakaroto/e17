#include "e_mod_main.h"

/*
   TODO
   - update on icon changes

 */

static void      _ngi_launcher_fill               (Ngi_Box *box);
static void      _ngi_launcher_app_change_cb      (void *data, E_Order *eo);

static void      _ngi_launcher_item_new           (Ngi_Box *box, Efreet_Desktop *dekstop, int instant, Ngi_Item *after);
static void      _ngi_launcher_item_fill          (Ngi_Item *it);
static void      _ngi_launcher_item_cb_drag_start (Ngi_Item *it);
static void      _ngi_launcher_item_cb_mouse_up   (Ngi_Item *it, Ecore_Event_Mouse_Button *ev);
static void      _ngi_launcher_item_cb_mouse_down (Ngi_Item *it, Ecore_Event_Mouse_Button *ev);

static void      _ngi_launcher_cb_drop_enter      (void *data, const char *type, void *event_info);
static void      _ngi_launcher_cb_drop_leave      (void *data, const char *type, void *event_info);
static void      _ngi_launcher_cb_drop_move       (void *data, const char *type, void *event_info);
static void      _ngi_launcher_cb_drop_end        (void *data, const char *type, void *event_info);

void
ngi_launcher_new(Ng *ng, Config_Box *cfg)
{
   char buf[4096];

   Ngi_Box *box = ngi_box_new(ng);
   if (!box)
      return;

   box->cfg = cfg;
   cfg->box = box;

   const char *drop[] =
   { "enlightenment/desktop",
     "enlightenment/border",
     "text/uri-list" };

   box->drop_handler = e_drop_handler_add
         (ng->win->drop_win, box,
         _ngi_launcher_cb_drop_enter, _ngi_launcher_cb_drop_move,
         _ngi_launcher_cb_drop_leave, _ngi_launcher_cb_drop_end,
         drop, 3, 0, 0, 0, 0);

   if (!cfg->launcher_app_dir || strlen(cfg->launcher_app_dir) == 0)
      return;

   if (cfg->launcher_app_dir[0] != '/')
     {
        snprintf(buf, sizeof(buf), "%s/.e/e/applications/bar/%s/.order",
                 e_user_homedir_get(), cfg->launcher_app_dir);
     }
   else
      snprintf(buf, sizeof(buf), "%s", cfg->launcher_app_dir);

   box->apps = e_order_new(buf);
   e_order_update_callback_set(box->apps, _ngi_launcher_app_change_cb, box);

   _ngi_launcher_fill(box);
}

void
ngi_launcher_remove(Ngi_Box *box)
{
   e_drop_handler_del(box->drop_handler);

   if (box->apps)
     {
        e_order_update_callback_set(box->apps, NULL, NULL);
        e_object_del(E_OBJECT(box->apps));
     }

   ngi_box_free(box);
}

static void
_ngi_launcher_fill(Ngi_Box *box)
{
   ngi_freeze(box->ng);

   if (box->apps)
     {
        Efreet_Desktop *desktop;
        Eina_List *l;

        EINA_LIST_FOREACH( box->apps->desktops, l, desktop)
        _ngi_launcher_item_new(box, desktop, 1, NULL);
     }

   ngi_thaw(box->ng);
}

static void
_ngi_launcher_app_change_cb(void *data, E_Order *eo)
{
   Ngi_Box *box = (Ngi_Box *)data;
   Ng *ng = box->ng;
   Ngi_Item *it = NULL;

   /* Efreet_Desktop *app; */
   Eina_List *apps, *items;
   if (!box->apps)
      return;

   EINA_LIST_FOREACH(box->items, items, it)
   if (!it->app || !it->usable)
      return;

   /* apps = box->apps->desktops;
    * items = box->items;
    *
    * if (eina_list_count(items) == 0) return;
    *
    * if (eina_list_count(items) - eina_list_count(apps) == 1 )
    *   {
    *   while(items)
    *     {
    *        app = apps ? (Efreet_Desktop*) apps->data : NULL;
    *        it  = (Ngi_Item*) items->data;
    *        if (!app || app != it->app)
    *          {
    *       ngi_item_remove(it);
    *       break;
    *          }
    *        if (apps) apps = apps->next;
    *        items = items->next;
    *     }
    *   }
    * else if (eina_list_count(apps) - eina_list_count(items) == 1 )
    *   {
    *   while(apps)
    *     {
    *        app = (Efreet_Desktop*) apps->data;
    *        it  = items ? (Ngi_Item*) items->data : NULL;
    *
    *        if (!it || app != it->app)
    *          {
    *       _ngi_launcher_item_new(box, app, 0, it);
    *       break;
    *          }
    *        apps = apps->next;
    *        if (items) items = items->next;
    *     }
    *   }
    *
    * apps = box->apps->desktops;
    * items = box->items;
    *
    * while(apps && items)
    *   {
    *   app = (Efreet_Desktop*) apps->data;
    *   it  = (Ngi_Item*) items->data;
    *   items = items->next;
    *   if (!it->usable) continue;
    *
    *   if (app != it->app) break;
    *
    *   apps = apps->next;
    *   }
    * if (!apps && !items) return;
    *
    * ngi_freeze(ng);
    * for(items = box->items; items;)
    *   {
    *
    *   it = (Ngi_Item*) items->data;
    *   items = items->next;
    *   if (it->usable);
    *   ngi_item_free(it);
    *   }
    *
    * _ngi_launcher_fill(box); */

   apps = box->apps->desktops;

   ngi_freeze(ng);

   EINA_LIST_FREE(box->items, it)
   ngi_item_free(it);

   _ngi_launcher_fill(box);

   ngi_thaw(ng);
}

/* ******************************* LAUNCHER DROP HANDLER ******************** */

static void
_ngi_launcher_pos_set(Ngi_Box *box, int x, int y)
{
   if (box->ng->horizontal)
      box->ng->pos = x + box->ng->size;
   else
      box->ng->pos = y + box->ng->size;
}

static void
_ngi_launcher_cb_drop_enter(void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Enter *ev = (E_Event_Dnd_Enter *)event_info;
   Ngi_Box *box = (Ngi_Box *)data;
   Ngi_Item *it;

   _ngi_launcher_pos_set(box, ev->x, ev->y);

   if (!strcmp(type, "text/uri-list"))
     {
        box->ng->item_active = ngi_box_item_at_position_get(box);
     }
   else /* border || desktop */
     {
        it = ngi_box_item_at_position_get(box);
        if (it && it->border)
           it = NULL;

        box->item_drop = ngi_item_new(box);
        ngi_box_item_show(box->ng, box->item_drop, 1);
        if (it)
           box->items = eina_list_prepend_relative(box->items, box->item_drop, it);
        else
           box->items = eina_list_append(box->items, box->item_drop);
     }

   box->ng->dnd = 1;
   box->ng->zoom_out = 1;
   box->ng->state = unzoomed;

   ngi_mouse_in(box->ng);
}

static void
_ngi_launcher_cb_drop_move (void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Move *ev = (E_Event_Dnd_Move *)event_info;
   Ngi_Item *it, *next = NULL;
   Ngi_Box *box = (Ngi_Box *)data;
   Ng *ng = box->ng;
   Eina_List *l;

   _ngi_launcher_pos_set(box, ev->x, ev->y);

   it = ngi_box_item_at_position_get(box);

   if (!strcmp(type, "text/uri-list"))
     {
        /*  if (it && it != box->item_active)
            {
            box->item_active = it;
            ngi_item_over_signal_emit(it, "e,action,start");
            }*/
     }
   else
     {
        l = eina_list_data_find_list(box->items, box->item_drop);
        if (l && l->next)
           next = (Ngi_Item *)l->next->data;

        if (it != box->item_drop && (!next || it != next))
          {
             box->items = eina_list_remove(box->items, box->item_drop);

             if (it)
                box->items = eina_list_prepend_relative(box->items, box->item_drop, it);
             else
                box->items = eina_list_append(box->items, box->item_drop);
          }
     }

   ngi_thaw(ng);
}

static void
_ngi_launcher_cb_drop_leave(void *data, const char *type, void *event_info)
{
   Ngi_Box *box = (Ngi_Box *)data;
   Ng *ng = box->ng;

   if (!strcmp(type, "text/uri-list"))
     {
        ng->item_active = NULL;
     }
   else
     {
        box->items = eina_list_remove(box->items, box->item_drop);
        ngi_item_free(box->item_drop);
        box->item_drop = NULL;
     }

   ngi_reposition(ng);
   ngi_input_extents_calc(ng, 1);
   ng->dnd = 0;
   ngi_mouse_out(ng);
}

static void
_ngi_launcher_cb_drop_end  (void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Drop *ev = (E_Event_Dnd_Drop *)event_info;
   Ngi_Box *box = (Ngi_Box *)data;
   Efreet_Desktop *app = NULL;
   Eina_List *l, *fl = NULL;
   Ngi_Item *it, *it2 = NULL;
   E_Border *bd;
   char *file;
   Ng *ng = box->ng;

   if (!strcmp(type, "enlightenment/desktop"))
     {
        app = (Efreet_Desktop *)ev->data;
     }
   else if (!strcmp(type, "enlightenment/border"))
     {
        bd = (E_Border *)ev->data;
        app = bd->desktop;

        if (!app)
          {
             app = e_desktop_border_create(bd);
             efreet_desktop_save(app);
             e_desktop_edit(e_container_current_get(e_manager_current_get()), app);
          }
     }
   else if (!strcmp(type, "text/uri-list"))
     {
        it = ng->item_active;

        Eina_List *ll = NULL;

        EINA_LIST_FOREACH (ev->data, l, file)
        {
           if (!strncmp(file, "file:///", 8))
              file = file + 7;

           ll = eina_list_append(ll, strdup(file));
        }

        ng->item_active = NULL;
        e_exec(ng->zone, it->app, NULL, ll, "itask-ng"); /* FIXME ? */

        ngi_item_signal_emit(it, "e,action,start");

        //ngi_item_over_signal_emit(it, "e,action,start");
        return;
     }

   for(l = box->items; l; l = l->next)
     {
        if (box->item_drop == l->data && l->next && l->next->data)
          {
             it2 = (Ngi_Item *)l->next->data;
          }
     }

   if (it2 && it2->app && app)
     {
        e_order_prepend_relative(box->apps, app, it2->app);
     }
   else if (app)
     {
        e_order_append(box->apps, app);
     }
   else if (fl)
     {
        char *file;
        app = NULL;
        EINA_LIST_FOREACH(fl, l, file)
        {
           if (!strncmp(file, "file:///", 8))
              file = file + 7;

           app = efreet_desktop_get(strdup(file));

           if (it2 && it2->app && app)
             {
                e_order_prepend_relative(box->apps, app, it2->app);
             }
           else if (app)
             {
                e_order_append(box->apps, app);
             }
        }
     }

   if (app)
     {
        _ngi_launcher_item_new(box, app, 1, it2);
     }

   box->items = eina_list_remove(box->items, box->item_drop);
   ngi_item_free(box->item_drop);
   box->item_drop = NULL;

   ngi_reposition(ng);
   ngi_input_extents_calc(ng, 1);

   printf("mouse_out drop end\n");
   ng->dnd = 0;
   ngi_mouse_out(ng);
}

/* ******************************* LAUNCHER ITEM ********************************** */

static void
_ngi_launcher_item_new(Ngi_Box *box, Efreet_Desktop *desktop, int instant, Ngi_Item *after)
{
   Ngi_Item *it;
   Ng *ng = box->ng;

   it = ngi_item_new(box);
   it->type = launcher_item;

   it->app = desktop;

   if (!desktop->name)
      return;  /* TODO remove later, when e stabilised...*/

   efreet_desktop_ref(desktop);

   /* it->cb_free       = _ngi_launcher_item_cb_free; */
   it->cb_mouse_in = ngi_item_mouse_in;
   it->cb_mouse_out = ngi_item_mouse_out;
   it->cb_mouse_down = _ngi_launcher_item_cb_mouse_down;
   it->cb_mouse_up = _ngi_launcher_item_cb_mouse_up;
   it->cb_drag_start = _ngi_launcher_item_cb_drag_start;

   _ngi_launcher_item_fill(it);

   if (after)
      box->items = eina_list_prepend_relative(box->items, it, after);
   else
      box->items = eina_list_append(box->items, it);

   it->usable = 1;

   ngi_box_item_show(ng, it, instant);
   ngi_item_signal_emit(it, "e,state,launcher_item_normal");
}

static void
_ngi_launcher_item_fill(Ngi_Item *it)
{
   ngi_item_del_icon(it);

   it->o_icon = e_util_desktop_icon_add(it->app, 128, it->box->ng->evas);
   edje_object_part_swallow(it->obj, "e.swallow.content", it->o_icon);
   evas_object_pass_events_set(it->o_icon, 1);
   evas_object_show(it->o_icon);

   it->o_icon2 = e_util_desktop_icon_add(it->app, 128, it->box->ng->evas);
   edje_object_part_swallow(it->over, "e.swallow.content", it->o_icon2);
   evas_object_pass_events_set(it->o_icon2, 1);

   if (it->app->name)
      it->label = eina_stringshare_add(it->app->name);
   else if (it->app->generic_name)
      it->label = eina_stringshare_add(it->app->generic_name);
   else
      it->label = eina_stringshare_add("");
}

static void
_ngi_launcher_item_cb_drag_del(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Ng *ng = data;

   ng->show_bar--;
   ngi_thaw(ng);
}

static void
_ngi_launcher_item_cb_drag_start(Ngi_Item *it)
{
   E_Drag *d;
   Evas_Object *o;
   Evas_Coord x, y, w, h, px, py;
   Ng *ng = it->box->ng;

   if (!it->usable)
      return;  /* FIXME needed? */

   if (it->box->cfg->launcher_lock_dnd)
      return;

   evas_object_geometry_get(it->o_icon, &x, &y, &w, &h);

   if (!ngi_config->use_composite)
     {
	x -= ng->win->rect.x;
	y -= ng->win->rect.y;
     }
   
   const char *drag_types[] = { "enlightenment/desktop" };

   d = e_drag_new(ng->zone->container, x, y, drag_types, 1,
                  it->app, -1, NULL, NULL);

   o = e_util_desktop_icon_add(it->app, MIN(w, h), e_drag_evas_get(d));

   evas_object_hide(it->obj);

   e_order_remove(it->box->apps, it->app);

   e_drag_object_set(d, o);
   e_drag_resize(d, w, h);
   evas_object_event_callback_add(o, EVAS_CALLBACK_DEL, _ngi_launcher_item_cb_drag_del, ng);

   ecore_x_pointer_xy_get(ng->win->input, &px, &py);

   e_drag_start(d, px, py);

   ng->item_drag = NULL;

   ng->show_bar++;

   ngi_thaw(ng);
}

static void
_ngi_launcher_menu_cb_lock_dnd(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Ngi_Box *box = (Ngi_Box *)data;
   if (!box)
      return;

   box->cfg->launcher_lock_dnd = e_menu_item_toggle_get(mi);
}

static void
_ngi_launcher_menu_cb_edit_icon(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Ngi_Item *it = (Ngi_Item *)data;
   if (!it)
      return;

   e_desktop_edit(it->box->ng->zone->container, it->app);
}

static void
_ngi_launcher_menu_cb_configure_bar(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Ng *ng = (Ng *)data;
   if (!ng)
      return;

   ngi_configure_module(ng->cfg);
}

static void
_ngi_launcher_menu_cb_configure_launcher(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Ngi_Box *box = (Ngi_Box *)data;
   char path[4096];

   if (!box)
      return;

   snprintf(path, sizeof(path), "%s/.e/e/applications/bar/%s/.order",
            e_user_homedir_get(), box->cfg->launcher_app_dir);

   e_configure_registry_call("internal/ibar_other",
                             e_container_current_get(e_manager_current_get()),
                             path);
}

static void
_ngi_launcher_menu_cb_menu_end(void *data, E_Menu *m)
{
   Ng *ng = (Ng *)data;

   e_object_del(E_OBJECT(m));

   ngi_thaw(ng);
}

static void
_ngi_launcher_item_cb_mouse_down(Ngi_Item *it, Ecore_Event_Mouse_Button *ev)
{
   Evas_Coord x, y, w, h;
   Efreet_Desktop *app;
   E_Menu *m;
   E_Menu_Item *mi;
   Ngi_Box *box = it->box;

   if (!it->usable)
      return;

   app = it->app;

   if (ev->buttons == 3 && app)
     {
        evas_object_geometry_get(it->obj, &x, &y, &w, &h);

        x += box->ng->win->popup->x + box->ng->zone->x;
        y += box->ng->win->popup->y + box->ng->zone->y;

        int dir = E_MENU_POP_DIRECTION_AUTO;

        ITEM_MOUSE_OUT(box->ng->item_active); /* FIXME check this again */

        switch(box->ng->cfg->orient)
          {
           case E_GADCON_ORIENT_TOP:
              dir = E_MENU_POP_DIRECTION_DOWN;
              y += h;
              break;

           case E_GADCON_ORIENT_BOTTOM:
              dir = E_MENU_POP_DIRECTION_UP;
              break;

           case E_GADCON_ORIENT_LEFT:
              dir = E_MENU_POP_DIRECTION_RIGHT;
              x += w;
              break;

           case E_GADCON_ORIENT_RIGHT:
              dir = E_MENU_POP_DIRECTION_LEFT;
              break;
          } /* switch */

        m = e_menu_new();

        mi = e_menu_item_new(m);
        e_menu_item_label_set(mi, D_("Configure Bar"));
        e_menu_item_callback_set(mi, _ngi_launcher_menu_cb_configure_bar, box->ng);

        mi = e_menu_item_new(m);
        e_menu_item_label_set(mi, D_("Lock Dragging"));
        e_menu_item_check_set(mi, 1);
        e_menu_item_toggle_set(mi, box->cfg->launcher_lock_dnd);
        e_menu_item_callback_set(mi, _ngi_launcher_menu_cb_lock_dnd, box);

        mi = e_menu_item_new(m);
        e_menu_item_separator_set(mi, 1);

        mi = e_menu_item_new(m);
        e_menu_item_label_set(mi, D_("Applications"));
        e_menu_item_callback_set(mi, _ngi_launcher_menu_cb_configure_launcher, box);

        mi = e_menu_item_new(m);
        e_menu_item_label_set(mi, D_("Edit Icon"));
        e_menu_item_callback_set(mi, _ngi_launcher_menu_cb_edit_icon, it);

        e_menu_post_deactivate_callback_set(m, _ngi_launcher_menu_cb_menu_end, box->ng);

        e_menu_activate_mouse(m, box->ng->zone, x, y, 1, 1,
                              dir, ev->timestamp);
     }
   else
      it->mouse_down = 1;
}

static void
_ngi_launcher_item_cb_mouse_up(Ngi_Item *it, Ecore_Event_Mouse_Button *ev)
{
   if (!it->mouse_down)
      return;  /* FIXME remove */

   if (!it->usable)
      return;

   if (ev->buttons == 1)
     {
        if (!it->border)
          {
             e_exec(it->box->ng->zone, it->app, NULL, NULL, "itask-ng");

             ngi_item_signal_emit(it, "e,action,start");
          }
        else
          {
             ngi_taskbar_item_border_show(it, 1);
          }
     }

   it->mouse_down = 0;
}

