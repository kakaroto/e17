#include "e_mod_main.h"
#include <X11/Xlib.h>

static int            _ngi_taskbar_border_check          (Ngi_Box *box, E_Border *bd);
static Ngi_Item *     _ngi_taskbar_border_find           (Ngi_Box *box, E_Border *bd);

static Eina_Bool      _ngi_taskbar_cb_border_event       (void *data, int type, void *event);
static Eina_Bool      _ngi_taskbar_cb_desk_show          (void *data, int type, void *event);

static void           _ngi_taskbar_item_new              (Ngi_Box *box, E_Border *bd);
static void           _ngi_taskbar_item_set_icon         (Ngi_Item *it);
static void           _ngi_taskbar_item_set_label        (Ngi_Item *it);

static void           _ngi_taskbar_item_cb_mouse_down    (Ngi_Item *it, Ecore_Event_Mouse_Button *ev);
static void           _ngi_taskbar_item_cb_mouse_up      (Ngi_Item *it, Ecore_Event_Mouse_Button *ev);
static void           _ngi_taskbar_item_cb_drag_start    (Ngi_Item *it);

static void           _ngi_taskbar_item_cb_drag_end      (E_Drag *drag, int dropped);

static Eina_Bool      _ngi_taskbar_cb_show_window        (void *data);
static void           _ngi_taskbar_cb_drop_enter         (void *data, const char *type, void *event_info);
static void           _ngi_taskbar_cb_drop_move          (void *data, const char *type, void *event_info);
static void           _ngi_taskbar_cb_drop_end           (void *data, const char *type, void *event_info);
static void           _ngi_taskbar_cb_drop_leave         (void *data, const char *type, void *event_info);

static void           _ngi_taskbar_item_mouse_in         (Ngi_Item *it);
static void           _ngi_taskbar_item_mouse_out        (Ngi_Item *it);

static Ecore_X_Atom ECOMORPH_ATOM_THUMBNAIL = 0;

void
ngi_taskbar_init(void)
{
   Ecore_Event_Handler *h;

   h = ecore_event_handler_add(E_EVENT_BORDER_ADD, _ngi_taskbar_cb_border_event, NULL);
   if (h)
      ngi_config->handlers = eina_list_append(ngi_config->handlers, h);

   h = ecore_event_handler_add(E_EVENT_BORDER_REMOVE, _ngi_taskbar_cb_border_event, NULL);
   if (h)
      ngi_config->handlers = eina_list_append(ngi_config->handlers, h);

   h = ecore_event_handler_add(E_EVENT_BORDER_ICONIFY, _ngi_taskbar_cb_border_event, NULL);
   if (h)
      ngi_config->handlers = eina_list_append(ngi_config->handlers, h);

   h = ecore_event_handler_add(E_EVENT_BORDER_UNICONIFY, _ngi_taskbar_cb_border_event, NULL);
   if (h)
      ngi_config->handlers = eina_list_append(ngi_config->handlers, h);

   h = ecore_event_handler_add(E_EVENT_BORDER_ICON_CHANGE, _ngi_taskbar_cb_border_event, NULL);
   if (h)
      ngi_config->handlers = eina_list_append(ngi_config->handlers, h);

   h = ecore_event_handler_add(E_EVENT_BORDER_PROPERTY, _ngi_taskbar_cb_border_event, NULL);
   if (h)
      ngi_config->handlers = eina_list_append(ngi_config->handlers, h);

   h = ecore_event_handler_add(E_EVENT_BORDER_ZONE_SET, _ngi_taskbar_cb_border_event, NULL);
   if (h)
      ngi_config->handlers = eina_list_append(ngi_config->handlers, h);

   h = ecore_event_handler_add(E_EVENT_BORDER_URGENT_CHANGE, _ngi_taskbar_cb_border_event, NULL);
   if (h)
      ngi_config->handlers = eina_list_append(ngi_config->handlers, h);

   h = ecore_event_handler_add(E_EVENT_BORDER_FOCUS_IN, _ngi_taskbar_cb_border_event, NULL);
   if (h)
      ngi_config->handlers = eina_list_append(ngi_config->handlers, h);

   h = ecore_event_handler_add(E_EVENT_BORDER_FOCUS_OUT, _ngi_taskbar_cb_border_event, NULL);
   if (h)
      ngi_config->handlers = eina_list_append(ngi_config->handlers, h);

   h = ecore_event_handler_add(E_EVENT_DESK_SHOW, _ngi_taskbar_cb_desk_show, NULL);
   if (h)
      ngi_config->handlers = eina_list_append(ngi_config->handlers, h);

   ECOMORPH_ATOM_THUMBNAIL = ecore_x_atom_get("__ECOMORPH_THUMBNAIL");
}

void
ngi_taskbar_new(Ng *ng, Config_Box *cfg)
{
   Ngi_Box *box = ngi_box_new(ng);
   if (!box)
      return;

   box->cfg = cfg;
   cfg->box = box;

   // "enlightenment/border", "enlightenment/desktop",
   const char *drop[] = {"text/uri-list","text/x-moz-url", "enlightenment/x-file"};
   box->drop_handler = e_drop_handler_add(E_OBJECT(ng->win->popup), box,
                                          _ngi_taskbar_cb_drop_enter, _ngi_taskbar_cb_drop_move,
                                          _ngi_taskbar_cb_drop_leave, _ngi_taskbar_cb_drop_end,
                                          drop, 3, 0, 0, 0, 0);

   ngi_taskbar_fill(box);
}

void
ngi_taskbar_fill(Ngi_Box *box)
{
   E_Border *bd;
   E_Border_List *bl;

   bl = e_container_border_list_first(box->ng->zone->container);

   while ((bd = e_container_border_list_next(bl)))
     {
        if (box->ng->zone == bd->zone)
           _ngi_taskbar_item_new(box, bd);
     }

   e_container_border_list_free(bl);
}

void
ngi_taskbar_remove(Ngi_Box *box)
{
   e_drop_handler_del(box->drop_handler);

   ngi_box_free(box);
}

static int
_ngi_taskbar_border_check(Ngi_Box *box, E_Border *bd)
{
   if (box->cfg->taskbar_show_desktop && bd->desk != e_desk_current_get(box->ng->zone))
      return 0;

   if (box->cfg->taskbar_show_iconified == 0 && bd->iconic)
      return 0;

   if (box->cfg->taskbar_show_iconified == 2 && !bd->iconic)
      return 0;

   if ((box->cfg->taskbar_skip_dialogs && bd->client.netwm.type == ECORE_X_WINDOW_TYPE_DIALOG)
       || (bd->user_skip_winlist)
       || (bd->client.netwm.state.skip_taskbar))
      return 0;

   /* XXX Xitk menu windows act like normal windows... */
   if (bd->client.icccm.transient_for &&
       bd->client.icccm.class &&
       !strcmp(bd->client.icccm.class, "Xitk"))
      return 0;

   return 1;
}

static Ngi_Item *
_ngi_taskbar_border_find(Ngi_Box *box, E_Border *bd)
{
   Eina_List *l;
   Ngi_Item *it;

   EINA_LIST_FOREACH(box->items, l, it)
   if (it->border == bd)
      return it;

   return NULL;
}

/* *************************  DND  CALLBACKS  ************************** */

static void
_ngi_taskbar_cb_drop_enter(void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Enter *ev = (E_Event_Dnd_Enter *)event_info;
   Ngi_Box *box = (Ngi_Box *)data;
   Ng *ng = box->ng;

   ng->pos = (ng->horizontal ? ev->x : ev->y);

   ng->item_active = ngi_box_item_at_position_get(box);

   printf("drop enter %d %p\n", ng->pos, ng->item_active);
   
   if (!ng->item_active)
      return;  /* FIXME set edge_in before ?*/

   ITEM_MOUSE_IN(ng->item_active);

   if (box->dnd_timer)
      ecore_timer_del(box->dnd_timer);
   
   box->dnd_timer = ecore_timer_add(0.5, _ngi_taskbar_cb_show_window, box);

   ng->dnd = 1;
   ngi_mouse_in(ng);
}

static void
_ngi_taskbar_cb_drop_leave(void *data, const char *type, void *event_info)
{
   Ngi_Box *box = (Ngi_Box *)data;
   Ng *ng = box->ng;

   printf("drop leave %d %p\n", ng->pos, ng->item_active);
   
   if (box->dnd_timer)
     {
	ecore_timer_del(box->dnd_timer);
	box->dnd_timer = NULL;
     }
   
   ng->item_active = NULL;
   ngi_mouse_out(ng);
}

static void
_ngi_taskbar_cb_drop_move(void *data, const char *type, void *event_info)
{
   E_Event_Dnd_Move *ev = (E_Event_Dnd_Move *)event_info;
   Ngi_Box *box = (Ngi_Box *)data;
   Ng *ng = box->ng;
   Ngi_Item *it;

   ng->pos = (ng->horizontal ? ev->x : ev->y);
   
   it = ngi_box_item_at_position_get(box);

   if (!it || !ng->item_active)
      return;

   if (ng->item_active != it)
     {
        if (box->dnd_timer)
           ecore_timer_del(box->dnd_timer);
	box->dnd_timer = NULL;
	
	if (it)
	  {
	     ITEM_MOUSE_OUT(ng->item_active);
	     ITEM_MOUSE_IN(it);

	     ng->item_active = it;

	     box->dnd_timer = ecore_timer_add(0.5, _ngi_taskbar_cb_show_window, box);
	  }	
     }

   ngi_animate(ng);
}

static void
_ngi_taskbar_cb_drop_end(void *data, const char *type, void *event_info)
{
   Ngi_Box *box = (Ngi_Box *)data;
   Ng *ng = box->ng;

   if (!ng->item_active)
      return;

   if (box->dnd_timer)
     {
	ecore_timer_del(box->dnd_timer);
	box->dnd_timer = NULL;
     }

   ng->item_active = NULL;
   ngi_mouse_out(ng);
}

static Eina_Bool
_ngi_taskbar_cb_show_window(void *data)
{
   Ngi_Box *box = (Ngi_Box *)data;
   E_Desk *desk;
   Ng *ng = box->ng;
   Ngi_Item *it = ng->item_active;

   if (!it)
      return EINA_FALSE;

   desk = e_desk_current_get(it->border->zone);

   if (desk != it->border->desk && !it->border->sticky)
     {
        e_desk_show(it->border->desk);
     }

   if (it->border->shaded)
     {
        e_border_unshade(it->border, E_DIRECTION_UP);
     }

   if (!it->border->visible)
     {
        e_border_show(it->border);
     }

   if (it->border->iconic)
     {
        e_border_uniconify(it->border);
     }
   else
     {
        e_border_raise(it->border);
     }

   e_border_focus_set(it->border, 1, 1);

   box->dnd_timer = NULL;
   return EINA_FALSE;
}

/* ************************** BORDER CALLBACKS ************************* */

static Eina_Bool
_ngi_taskbar_cb_border_event(void *data, int type, void *event)
{
   E_Event_Border_Add *ev = event;
   Ng *ng;
   Ngi_Box *box;
   Ngi_Item *it;
   Eina_List *l, *ll;
   E_Border *bd = ev->border;

   EINA_LIST_FOREACH (ngi_config->instances, l, ng)
   {
      EINA_LIST_FOREACH (ng->boxes, ll, box)
      {
         if (box->cfg->type != taskbar)
            continue;

         it = _ngi_taskbar_border_find(box, bd);

         if (type == E_EVENT_BORDER_FOCUS_IN)
           {
              if (!it)
                 continue;

              ngi_item_signal_emit(it, "e,state,taskbar_item_focused");
           }
         else if (type == E_EVENT_BORDER_FOCUS_OUT)
           {
              if (!it)
                 continue;

              if (it->border->iconic && box->cfg->taskbar_show_iconified != 2)
                 ngi_item_signal_emit(it, "e,state,taskbar_item_iconify");
              else
                 ngi_item_signal_emit(it, "e,state,taskbar_item_normal");
           }
         else if (type == E_EVENT_BORDER_ICONIFY)
           {
              if (!it)
                 continue;

              ngi_item_signal_emit(it, "e,state,taskbar_item_iconify");
           }
         else if (type == E_EVENT_BORDER_UNICONIFY)
           {
              if (!it)
                 continue;

              ngi_item_signal_emit(it, "e,state,taskbar_item_normal");
           }
         else if (type == E_EVENT_BORDER_ICON_CHANGE)
           {
              if (!it)
                 continue;

              _ngi_taskbar_item_set_icon(it);
           }
         else if (type == E_EVENT_BORDER_ADD)
           {
              _ngi_taskbar_item_new(box, bd);
           }
         else if (type == E_EVENT_BORDER_REMOVE)
           {
              if (!it)
                 continue;

              ngi_item_remove(it);
           }
         else if (type == E_EVENT_BORDER_ZONE_SET)
           {
              if (ng->zone == bd->zone)
                {
                   e_border_zone_set(bd, ng->zone);
                   _ngi_taskbar_item_new(box, bd);
                }
              else if (it)
                {
                   ngi_item_remove(it);
                }
           }
         else if (type == E_EVENT_BORDER_URGENT_CHANGE)
           {
              if (!it)
                 continue;

              if (!ng->cfg->autohide_show_urgent)
                 continue;

              if (bd->client.icccm.urgent)
                {
                   it->urgent = 1;
                   ngi_item_signal_emit(it, "e,state,taskbar_item_urgent");
                   ngi_item_signal_emit(it, "e,action,start");
                   ng->show_bar++;
                   ngi_animate(ng);
                }
              else
                {
                   it->urgent = 0;
                   if (it->border->iconic && box->cfg->taskbar_show_iconified != 2)
                      ngi_item_signal_emit(it, "e,state,taskbar_item_iconify");
                   else if (bd->focused)
                      ngi_item_signal_emit(it, "e,state,taskbar_item_focused");
                   else
                      ngi_item_signal_emit(it, "e,state,taskbar_item_normal");

                   if (ng->show_bar > 0)
                      ng->show_bar--;

                   ngi_animate(ng);
                }
           }
         else if (type == E_EVENT_BORDER_PROPERTY)
           {
              /* XXX workaround for e sending event after the
               * border_remove event
               */
              if (bd->already_unparented)
                 return EINA_TRUE;

              if (it && it->usable)
                {
                   if (!_ngi_taskbar_border_check(box, bd))
                      ngi_item_remove(it);
                   else
                      _ngi_taskbar_item_set_label(it);
                }
              else if (!it)
                {
                   _ngi_taskbar_item_new(box, ev->border);
                }
           }
      }
   }
   return EINA_TRUE;
}

static Eina_Bool
_ngi_taskbar_cb_desk_show(void *data, int type, void *event)
{
   /* E_Event_Desk_Show *ev = (E_Event_Desk_Show*) event; */
   Ng *ng;
   Ngi_Box *box;
   Ngi_Item *it;
   Eina_List *l, *ll, *lll;
   E_Desk *desk;
   int changed = 0;

   EINA_LIST_FOREACH (ngi_config->instances, l, ng)
   {
      EINA_LIST_FOREACH (ng->boxes, ll, box)
      {
         if (box->cfg->type != taskbar || !box->cfg->taskbar_show_desktop)
            continue;

         desk = e_desk_current_get(ng->zone);

         EINA_LIST_FOREACH (box->items, lll, it)
         {
            if (it->border->desk == desk)
              {
                 changed = 1;
                 it->scale = 1.0;
              }
            else
              {
                 changed = 1;
                 it->scale = 0.0;
              }
         }
         if (changed)
            ngi_thaw(ng);

         changed = 0;
      }
   }
   return EINA_TRUE;
}

/* ***************************  TASKBAR  ITEM  *************************** */

static void
_ngi_taskbar_item_new(Ngi_Box *box, E_Border *bd)
{
   Ngi_Item *it, *l_it = NULL, *ll_it = NULL;
   Eina_List *l;
   Ng *ng = box->ng;

   if (!_ngi_taskbar_border_check(box, bd))
      return;

   if (_ngi_taskbar_border_find(box, bd))
      return;

   it = ngi_item_new(box);
   it->type = taskbar_item;

   e_object_ref(E_OBJECT(bd));

   it->border = bd;

   /* it->cb_free       = _ngi_taskbar_item_cb_free; */
   it->cb_mouse_in = _ngi_taskbar_item_mouse_in;
   it->cb_mouse_out = _ngi_taskbar_item_mouse_out;
   it->cb_mouse_down = _ngi_taskbar_item_cb_mouse_down;
   it->cb_mouse_up = _ngi_taskbar_item_cb_mouse_up;
   it->cb_drag_start = _ngi_taskbar_item_cb_drag_start;

   _ngi_taskbar_item_set_icon(it);
   _ngi_taskbar_item_set_label(it);

   if (box->cfg->taskbar_group_apps)
      if (bd->client.icccm.class && bd->client.icccm.class[0])
        {
           it->class = eina_stringshare_add(bd->client.icccm.class);

           EINA_LIST_FOREACH (box->items, l, l_it)
           {
              if (box->cfg->taskbar_append_right)
                {
                   if (ll_it && l_it->class && ll_it->class &&
                       ll_it->class == it->class &&
                       l_it->class != it->class)
                      break;
                }
              else
                {
                   if (l_it->class && l_it->class == it->class)
                      break;

                   ll_it = l_it;
                   l_it = NULL;
                }
           }
        }

   if (l_it)
     {
        if (box->cfg->taskbar_append_right)
           box->items = eina_list_append_relative(box->items, it, l_it);
        else
           box->items = eina_list_prepend_relative(box->items, it, l_it);
     }
   else
     {
        if (box->cfg->taskbar_append_right)
           box->items = eina_list_append(box->items, it);
        else
           box->items = eina_list_prepend(box->items, it);
     }

   it->usable = 1;
   it->urgent = bd->client.icccm.urgent;

   ngi_box_item_show(ng, it, 0);

   if (!bd->iconic || box->cfg->taskbar_show_iconified == 2)
      ngi_item_signal_emit(it, "e,state,taskbar_item_normal");
}

Evas_Object *
_ngi_taskbar_border_icon_add(E_Border *bd, Evas *evas)
{
   Evas_Object *o;

   E_OBJECT_CHECK_RETURN(bd, NULL);
   E_OBJECT_TYPE_CHECK_RETURN(bd, E_BORDER_TYPE, NULL);

   o = NULL;
   if (bd->internal)
     {
        o = edje_object_add(evas);
        if (!bd->internal_icon)
           e_util_edje_icon_set(o, "enlightenment/e");
        else
          {
             if (!bd->internal_icon_key)
               {
                  char *ext;
                  ext = strrchr(bd->internal_icon, '.');
                  if ((ext) && ((!strcmp(ext, ".edj"))))
                    {
                       if (!edje_object_file_set(o, bd->internal_icon, "icon"))
                          e_util_edje_icon_set(o, "enlightenment/e");
                    }
                  else if (ext)
                    {
                       evas_object_del(o);
                       o = e_icon_add(evas);
                       e_icon_file_set(o, bd->internal_icon);
                    }
                  else
                    {
                       if (!e_util_edje_icon_set(o, bd->internal_icon))
                          e_util_edje_icon_set(o, "enlightenment/e");
                    }
               }
             else
               {
                  edje_object_file_set(o, bd->internal_icon,
                                       bd->internal_icon_key);
               }
          }

        return o;
     }

   if ((bd->client.netwm.icons) &&
       ((e_config->use_app_icon) ||
        (bd->remember && (bd->remember->prop.icon_preference == E_ICON_PREF_NETWM))))
     {
        int i, size, tmp, found = 0;
        o = e_icon_add(evas);

        size = bd->client.netwm.icons[0].width;

        for (i = 1; i < bd->client.netwm.num_icons; i++)
          {
             if ((tmp = bd->client.netwm.icons[i].width) > size)
               {
                  size = tmp;
                  found = i;
               }
          }

        e_icon_data_set(o, bd->client.netwm.icons[found].data,
                        bd->client.netwm.icons[found].width,
                        bd->client.netwm.icons[found].height);
        e_icon_alpha_set(o, 1);

        return o;
     }

   if (bd->desktop)
     {
        o = e_util_desktop_icon_add(bd->desktop, 256, evas);
        if (o)
           return o;

        o = e_util_desktop_icon_add(bd->desktop, 128, evas);
        if (o)
           return o;

        o = e_util_desktop_icon_add(bd->desktop, 48, evas);
        if (o)
           return o;

        o = e_util_desktop_icon_add(bd->desktop, 32, evas);
        if (o)
           return o;
     }

   if (bd->client.netwm.icons)
     {
        int i, size, tmp, found = 0;
        o = e_icon_add(evas);

        size = bd->client.netwm.icons[0].width;

        for (i = 1; i < bd->client.netwm.num_icons; i++)
          {
             if ((tmp = bd->client.netwm.icons[i].width) > size)
               {
                  size = tmp;
                  found = i;
               }
          }

        e_icon_data_set(o, bd->client.netwm.icons[found].data,
                        bd->client.netwm.icons[found].width,
                        bd->client.netwm.icons[found].height);
        e_icon_alpha_set(o, 1);
        return o;
     }

   o = e_border_icon_add(bd, evas);
   if (o)
      return o;

   o = edje_object_add(evas);
   e_util_edje_icon_set(o, "enlightenment/unknown");

   return o;
}

static void
_ngi_taskbar_item_set_icon(Ngi_Item *it)
{
   ngi_item_del_icon(it);

   it->o_icon = _ngi_taskbar_border_icon_add(it->border, it->box->ng->win->evas);
   edje_object_part_swallow(it->obj, "e.swallow.content", it->o_icon);
   evas_object_pass_events_set(it->o_icon, 1);
   evas_object_show(it->o_icon);

   it->o_icon2 = _ngi_taskbar_border_icon_add(it->border, it->box->ng->win->evas);
   edje_object_part_swallow(it->over, "e.swallow.content", it->o_icon2);
   evas_object_pass_events_set(it->o_icon2, 1);
   evas_object_show(it->o_icon2);

   if (it->border->iconic && it->box->cfg->taskbar_show_iconified != 2)
      ngi_item_signal_emit(it, "e,state,taskbar_item_iconify");
}

static void
_ngi_taskbar_item_set_label(Ngi_Item *it)
{
   const char *title;

   if (it->label)
     {
        eina_stringshare_del(it->label);
        it->label = NULL;
     }

   title = e_border_name_get(it->border);

   if ((!title) || (!title[0]))
      title = D_("No name!");

   unsigned int max_len = 36;

   if (strlen(title) > max_len)
     {
        char *abbv;
        const char *left, *right;

        abbv = (char *)calloc(E_CLIENTLIST_MAX_CAPTION_LEN + 4, sizeof(char));
        left = title;
        right = title + (strlen(title) - (max_len / 2));

        strncpy(abbv, left, max_len / 2);
        strncat(abbv, "...", 3);
        strncat(abbv, right, max_len / 2);

        it->label = eina_stringshare_add(abbv);
     }
   else
     {
        it->label = eina_stringshare_add(title);
     }
}

/* **************************  ITEM  CALLBACKS  ************************** */

static void
_ngi_taskbar_item_cb_mouse_down(Ngi_Item *it, Ecore_Event_Mouse_Button *ev)
{
   Evas_Coord x, y, w, h;
   E_Border *bd;
   Ng *ng;

   if (!it->usable)
      return;

   ng = it->box->ng;
   bd = it->border;

   if (ev->buttons == 3 && bd)
     {
        int dir = E_MENU_POP_DIRECTION_AUTO;
        evas_object_geometry_get(it->obj, &x, &y, &w, &h);

        x += ng->win->popup->x + ng->zone->x;
        y += ng->win->popup->y + ng->zone->y;

        ITEM_MOUSE_OUT(it);

        switch(ng->cfg->orient)
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
        ngi_border_menu_show(it->box, it->border, x, y, dir, ev->timestamp);
     }
   else if (ev->buttons == 1 && ev->double_click && !it->border->iconic)
     {
        e_border_iconify(it->border);
     }
   else
      it->mouse_down = 1;
}

void
ngi_taskbar_item_border_show(Ngi_Item *it, int to_desk)
{
   E_Border *bd;
   E_Desk *desk;
   bd = it->border;

   if (to_desk)
     {
        desk = e_desk_current_get(it->border->zone);

        if (desk != it->border->desk && !it->border->sticky)
          {
             e_desk_show(it->border->desk);
          }
     }

   if (bd->shaded)
     {
        e_border_unshade(it->border, E_DIRECTION_UP);
     }

   if (bd->iconic)
     {
        e_border_uniconify(it->border);

        if (it->border_was_fullscreen) /* ditto */
          {
             e_border_fullscreen(bd, (E_Fullscreen)e_config->fullscreen_policy);
             it->border_was_fullscreen = 0;
          }
     }
   else
     {
        e_border_raise(bd);
     }

   e_border_focus_set(bd, 1, 1);
}

static void
_ngi_taskbar_item_cb_mouse_up(Ngi_Item *it, Ecore_Event_Mouse_Button *ev)
{
   if (!it->mouse_down)
      return;

   if (!it->usable)
      return;

   it->mouse_down = 0;

   /* remove border icon urgent state */
   if (it->urgent)
     {
        it->urgent = 0;

        if (it->border->iconic && it->box->cfg->taskbar_show_iconified != 2)
           ngi_item_signal_emit(it, "e,state,taskbar_item_iconify");
        else if (it->border->focused)
           ngi_item_signal_emit(it, "e,state,taskbar_item_focused");
        else
           ngi_item_signal_emit(it, "e,state,taskbar_item_normal");

        if (it->box->ng->show_bar > 0)
           it->box->ng->show_bar--;
     }

   if (ev->buttons == 1)
     {
        ngi_taskbar_item_border_show(it, 1);
     }
}

static void
_ngi_taskbar_item_cb_drag_del(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Ng *ng = data;

   ng->show_bar--;
   ngi_thaw(ng);
}

static void
_ngi_taskbar_item_cb_drag_start(Ngi_Item *it)
{
   E_Drag *d;
   Evas_Object *o;
   Evas_Coord x, y, w, h, px, py;
   Ng *ng = it->box->ng;

   if (!it->usable)
      return;

   if (!it->border)
      return;

   edje_object_signal_emit(ng->label, "e,state,label_hide", "e");

   evas_object_geometry_get(it->o_icon, &x, &y, &w, &h);

   const char *drag_types[] = { "enlightenment/border" };

   d = e_drag_new(ng->zone->container, x, y, drag_types, 1,
                  it->border, -1, NULL, _ngi_taskbar_item_cb_drag_end);

   o = _ngi_taskbar_border_icon_add(it->border,e_drag_evas_get(d));
   e_object_ref(E_OBJECT(it->border));
   e_drag_object_set(d, o);
   e_drag_resize(d, w, h);
   evas_object_event_callback_add(o, EVAS_CALLBACK_DEL, _ngi_taskbar_item_cb_drag_del, ng);
   ng->show_bar++;

   ecore_x_pointer_xy_get(ng->win->evas_win, &px, &py);

   e_drag_start(d, px, py);
}

static void
_ngi_taskbar_item_cb_drag_end(E_Drag *drag, int dropped)
{
   E_Border *bd = (E_Border *)drag->data;

   if (dropped)
     {
        e_object_unref(E_OBJECT(bd));
        return;
     }

   /* TODO _ngi_taskbar_border_find
      ngi_taskbar_item_border_show()*/
   E_Desk *desk = e_desk_current_get(bd->zone);

   if (desk != bd->desk)
      e_border_desk_set(bd, desk);

   if (bd->iconic)
      e_border_uniconify(bd);
   else
      e_border_raise(bd);

   if (bd->shaded)
      e_border_unshade(bd, E_DIRECTION_UP);

   e_object_unref(E_OBJECT(bd));
}

#define TEXT_DIST 36

static int
_ngi_taskbar_zoom_function(Ng *ng)
{
   double zoom;

   double f, x, ff, sqrt_ffxx, sqrt_ff_1;

   Config_Item *cfg = ng->cfg;

   f = 5.5;
   x = 0.0;

   ff = f * f;
   sqrt_ffxx = sqrt(ff - x * x);
   sqrt_ff_1 = sqrt(ff - 1.0);

   zoom = (cfg->zoomfactor - 1.0) * (cfg->zoomfactor - 1.0) *
      ((sqrt_ff_1 - sqrt_ffxx) / (sqrt_ff_1 - f)) + 1.0;

   return (int)(zoom * (double)ng->size) + TEXT_DIST;
}

static Eina_Bool
_ngi_taskbar_cb_preview_hide(void *data)
{
   Ng *ng = data;

   ng->effect_timer = NULL;

   ecore_x_client_message32_send(e_manager_current_get()->root,
                                 ECORE_X_ATOM_NET_WM_ICON_GEOMETRY,
                                 SubstructureNotifyMask,
                                 0, 0, 0, 0, 0);
   return EINA_FALSE;
}

static void
_ngi_taskbar_item_mouse_in(Ngi_Item *it)
{
   Ng *ng = it->box->ng;
   if (ng->cfg->ecomorph_features)
     {
        if (ng->effect_timer)
           ecore_timer_del(ng->effect_timer);

        ng->effect_timer = NULL;

        ecore_x_client_message32_send(e_manager_current_get()->root,
                                      ECORE_X_ATOM_NET_WM_ICON_GEOMETRY,
                                      SubstructureNotifyMask,
                                      it->border->win, it->pos,
                                      _ngi_taskbar_zoom_function(ng),
                                      ng->cfg->orient,0);
     }
}

static void
_ngi_taskbar_item_mouse_out(Ngi_Item *it)
{
   Ng *ng = it->box->ng;
   if (ng->cfg->ecomorph_features)
     {
        if (ng->effect_timer)
           ecore_timer_del(ng->effect_timer);

        ng->effect_timer = ecore_timer_add(0.2, _ngi_taskbar_cb_preview_hide, ng);
     }
}

