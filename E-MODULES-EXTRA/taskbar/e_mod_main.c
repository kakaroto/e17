/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include "e.h"
#include "e_mod_main.h"

/***************************************************************************/
 /**/
/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);
static const char *_gc_id_new(void);

/* and actually define the gadcon class that this module provides (just 1) */
static E_Gadcon_Client_Class _gadcon_class = {
   GADCON_CLIENT_CLASS_VERSION,
   "taskbar",
   {
      _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL
   },
   E_GADCON_CLIENT_STYLE_PLAIN
};
 /**/
/***************************************************************************/
/***************************************************************************/
    /**/
/* actual module specifics */

typedef struct _Taskbar Taskbar;
typedef struct _Taskbar_Item Taskbar_Item;

struct _Taskbar
{
   E_Gadcon_Client *gcc;        // The gadcon client
   Evas_Object *o_items;        // Table of items
   Eina_List *items;            // List of items
   E_Zone *zone;                // Current Zone
   Config_Item *config;         // Configuration
   int horizontal;
};

struct _Taskbar_Item
{
   Taskbar *taskbar;            // Parent taskbar
   E_Border *border;            // The border this item points to
   Evas_Object *o_item;         // The edje theme object
   Evas_Object *o_icon;         // The icon
   const char *label;           // label taken from the border
};

static Taskbar *_taskbar_new(Evas *evas, E_Zone *zone, const char *id);
static void _taskbar_free(Taskbar *taskbar);
static void _taskbar_refill(Taskbar *taskbar);
static void _taskbar_refill_all();
static void _taskbar_refill_border(E_Border *border);
static void _taskbar_signal_emit(E_Border *border, char *sig, char *src);

static Taskbar_Item *_taskbar_item_find(Taskbar *taskbar, E_Border *border);
static Taskbar_Item *_taskbar_item_new(Taskbar *taskbar, E_Border *border);

static int _taskbar_item_check_add(Taskbar *taskbar, E_Border *border);
static void _taskbar_item_add(Taskbar *taskbar, E_Border *border);
static void _taskbar_item_remove(Taskbar_Item *item);
static void _taskbar_item_refill(Taskbar_Item *item);
static void _taskbar_item_fill(Taskbar_Item *item);
static void _taskbar_item_free(Taskbar_Item *item);
static void _taskbar_item_signal_emit(Taskbar_Item *item, char *sig, char *src);

static Config_Item *_taskbar_config_item_get(const char *id);

static void _taskbar_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _taskbar_cb_item_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _taskbar_cb_item_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info);

static int _taskbar_cb_event_border_add(void *data, int type, void *event);
static int _taskbar_cb_event_border_remove(void *data, int type, void *event);
static int _taskbar_cb_event_border_iconify(void *data, int type, void *event);
static int _taskbar_cb_event_border_uniconify(void *data, int type, void *event);
static int _taskbar_cb_event_border_icon_change(void *data, int type, void *event);
static int _taskbar_cb_event_border_zone_set(void *data, int type, void *event);
static int _taskbar_cb_event_border_desk_set(void *data, int type, void *event);
static int _taskbar_cb_window_focus_in(void *data, int type, void *event);
static int _taskbar_cb_window_focus_out(void *data, int type, void *event);
static int _taskbar_cb_event_border_property(void *data, int type, void *event);
static int _taskbar_cb_event_desk_show(void *data, int type, void *event);
static int _taskbar_cb_event_border_urgent_change(void *data, int type, void *event);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *taskbar_config = NULL;

/* module setup */
EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "Taskbar"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   conf_item_edd = E_CONFIG_DD_NEW("Taskbar_Config_Item", Config_Item);

#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, show_all, INT);

   conf_edd = E_CONFIG_DD_NEW("Taskbar_Config", Config);

#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   taskbar_config = e_config_domain_load("module.taskbar", conf_edd);
   if (!taskbar_config)
     {
        Config_Item *config;
        taskbar_config = E_NEW(Config, 1);
        config = E_NEW(Config_Item, 1);

        config->id = eina_stringshare_add("0");
        config->show_all = 0;

        taskbar_config->items = eina_list_append(taskbar_config->items, config);
     }

   taskbar_config->module = m;

   taskbar_config->handlers = eina_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_ADD, _taskbar_cb_event_border_add, NULL));
   taskbar_config->handlers = eina_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_REMOVE, _taskbar_cb_event_border_remove, NULL));
   taskbar_config->handlers = eina_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_ICONIFY, _taskbar_cb_event_border_iconify, NULL));
   taskbar_config->handlers = eina_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_UNICONIFY, _taskbar_cb_event_border_uniconify, NULL));
   taskbar_config->handlers = eina_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_ICON_CHANGE, _taskbar_cb_event_border_icon_change, NULL));
   taskbar_config->handlers = eina_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_DESK_SET, _taskbar_cb_event_border_desk_set, NULL));
   taskbar_config->handlers = eina_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_ZONE_SET, _taskbar_cb_event_border_zone_set, NULL));
   taskbar_config->handlers = eina_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_FOCUS_IN, _taskbar_cb_window_focus_in, NULL));
   taskbar_config->handlers = eina_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_FOCUS_OUT, _taskbar_cb_window_focus_out, NULL));
   taskbar_config->handlers = eina_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_BORDER_PROPERTY, _taskbar_cb_event_border_property, NULL));
   taskbar_config->handlers = eina_list_append
      (taskbar_config->handlers, ecore_event_handler_add(E_EVENT_DESK_SHOW, _taskbar_cb_event_desk_show, NULL));
   taskbar_config->handlers = eina_list_append
      (taskbar_config->handlers, ecore_event_handler_add
       (E_EVENT_BORDER_URGENT_CHANGE, _taskbar_cb_event_border_urgent_change, NULL));

   e_gadcon_provider_register(&_gadcon_class);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   e_gadcon_provider_unregister(&_gadcon_class);

   while (taskbar_config->config_dialog)
      e_object_del(E_OBJECT(taskbar_config->config_dialog));

   while (taskbar_config->handlers)
     {
        ecore_event_handler_del(taskbar_config->handlers->data);
        taskbar_config->handlers = eina_list_remove_list(taskbar_config->handlers, taskbar_config->handlers);
     }
   while (taskbar_config->borders)
     {
        taskbar_config->borders = eina_list_remove_list(taskbar_config->borders, taskbar_config->borders);
     }
   free(taskbar_config);
   taskbar_config = NULL;
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save("module.taskbar", conf_edd, taskbar_config);
   return 1;
}

/**************************************************************/

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Taskbar *taskbar;
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Evas_Coord x, y, w, h;
   int cx, cy, cw, ch;

   taskbar = _taskbar_new(gc->evas, gc->zone, id);

   o = taskbar->o_items;
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = taskbar;
   taskbar->gcc = gcc;

   e_gadcon_canvas_zone_geometry_get(gcc->gadcon, &cx, &cy, &cw, &ch);
   evas_object_geometry_get(o, &x, &y, &w, &h);

   taskbar_config->taskbars = eina_list_append(taskbar_config->taskbars, taskbar);

   // Fill on initial config
   _taskbar_config_updated(taskbar->config);

   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Taskbar *taskbar;

   taskbar = (Taskbar *)gcc->data;
   taskbar_config->taskbars = eina_list_remove(taskbar_config->taskbars, taskbar);
   _taskbar_free(taskbar);
}

/* TODO */
static void
_gc_orient(E_Gadcon_Client *gcc)
{
   Taskbar *taskbar;

   taskbar = (Taskbar *)gcc->data;

   switch (gcc->gadcon->orient)
     {
      case E_GADCON_ORIENT_FLOAT:
      case E_GADCON_ORIENT_HORIZ:
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_BOTTOM:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
      case E_GADCON_ORIENT_CORNER_BL:
      case E_GADCON_ORIENT_CORNER_BR:
	if (!taskbar->horizontal)
	  {
              taskbar->horizontal = 1;
              e_box_orientation_set(taskbar->o_items, taskbar->horizontal);
	      _taskbar_refill(taskbar);
	  }
	break;
      case E_GADCON_ORIENT_VERT:
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_RIGHT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_LB:
      case E_GADCON_ORIENT_CORNER_RB:
	if (taskbar->horizontal)
	  {
              taskbar->horizontal = 0;
              e_box_orientation_set(taskbar->o_items, taskbar->horizontal);
	      _taskbar_refill(taskbar);
	  }
	break;
      default:
	break;
     }
   e_box_align_set(taskbar->o_items, 0.5, 0.5);
}

static char *
_gc_label(void)
{
   return D_("Taskbar");
}

static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-taskbar.edj", e_module_dir_get(taskbar_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(void)
{
   Config_Item *config;

   config = _taskbar_config_item_get(NULL);
   return config->id;
}

/***************************************************************************/

static Taskbar *
_taskbar_new(Evas *evas, E_Zone *zone, const char *id)
{
   Taskbar *taskbar;

   taskbar = E_NEW(Taskbar, 1);
   taskbar->config = _taskbar_config_item_get(id);
   taskbar->o_items = e_box_add(evas);
   taskbar->horizontal = 1;

   e_box_homogenous_set(taskbar->o_items, 1);
   e_box_orientation_set(taskbar->o_items, taskbar->horizontal);
   e_box_align_set(taskbar->o_items, 0.5, 0.5);
   taskbar->zone = zone;
   return taskbar;
}

static void
_taskbar_free(Taskbar *taskbar)
{
   while (taskbar->items)
     {
        _taskbar_item_free(taskbar->items->data);
        taskbar->items = eina_list_remove_list(taskbar->items, taskbar->items);
     }
   evas_object_del(taskbar->o_items);
   free(taskbar);
}

static void 
_taskbar_refill(Taskbar *taskbar)
{
   Eina_List *l;
   E_Border *border;
   Taskbar_Item *item;
   Evas_Coord w, h;

   while (taskbar->items)
     {
        item = taskbar->items->data;
        _taskbar_item_remove(item);
     }
   for (l = taskbar_config->borders; l; l = l->next)
     {
        border = l->data;
        _taskbar_item_check_add(taskbar, border);
     }
   if (taskbar->items) 
     {
        item = taskbar->items->data;
	edje_object_size_min_calc(item->o_item, &w, &h);
	if (!taskbar->gcc->resizable)
	  {
	     if (taskbar->horizontal)
                e_gadcon_client_aspect_set(taskbar->gcc, w*eina_list_count(taskbar->items), h);
	     else
                e_gadcon_client_aspect_set(taskbar->gcc, w, h*eina_list_count(taskbar->items));
          }
     }
}

static void 
_taskbar_refill_all()
{
   Eina_List *l;
   Taskbar *taskbar;

   for (l = taskbar_config->taskbars; l; l = l->next)
     {
        taskbar = l->data;
        _taskbar_refill(taskbar);
     }
}


static void
_taskbar_refill_border(E_Border *border)
{
   Eina_List *l;
   Eina_List *m;
   Taskbar *taskbar;
   Taskbar_Item *item;

   for (l = taskbar_config->taskbars; l; l = l->next)
     {
        taskbar = l->data;
	for (m = taskbar->items; m; m = m->next)
	   {
               item = m->data;
	       if (item->border == border)
	          _taskbar_item_refill(item);
	   }
     }
}

static void 
_taskbar_signal_emit(E_Border *border, char *sig, char *src)
{
   Eina_List *l;
   Eina_List *m;
   Taskbar *taskbar;
   Taskbar_Item *item;

   for (l = taskbar_config->taskbars; l; l = l->next)
     {
        taskbar = l->data;
	for (m = taskbar->items; m; m = m->next)
	   {
               item = m->data;
	       if (item->border == border)
	          _taskbar_item_signal_emit(item, sig, src);
	   }
     }
}

static Taskbar_Item *
_taskbar_item_find(Taskbar *taskbar, E_Border *border)
{
   Eina_List *l;
   Taskbar_Item *item;
   if (!taskbar->items)
      return NULL;

   for (l = taskbar->items; l; l = l->next)
     {
        item = l->data;

        if (item->border == border)
           return item;
     }
   return NULL;
}

static Taskbar_Item *
_taskbar_item_new(Taskbar *taskbar, E_Border *border)
{
   Taskbar_Item *item;
   char buf[4096];

   item = E_NEW(Taskbar_Item, 1);
   e_object_ref(E_OBJECT(border));
   item->taskbar = taskbar;
   item->border = border;
   item->o_item = edje_object_add(evas_object_evas_get(taskbar->o_items));
   snprintf(buf, sizeof(buf), "%s/taskbar.edj", e_module_dir_get(taskbar_config->module));
   if (!e_theme_edje_object_set(item->o_item, "base/theme/modules/taskbar", "modules/taskbar/item"))
      edje_object_file_set(item->o_item, buf, "modules/taskbar/item");
   evas_object_event_callback_add(item->o_item, EVAS_CALLBACK_MOUSE_DOWN, _taskbar_cb_item_mouse_down, item);
   evas_object_event_callback_add(item->o_item, EVAS_CALLBACK_MOUSE_UP, _taskbar_cb_item_mouse_up, item);
   evas_object_show(item->o_item);

   _taskbar_item_fill(item);
   return item;
}

static int
_taskbar_item_check_add(Taskbar *taskbar, E_Border *border)
{
   if (border->user_skip_winlist)
      return 1;
   if (border->client.netwm.state.skip_taskbar)
      return 1;
   if (_taskbar_item_find(taskbar, border))
      return 1;
   if (!taskbar->config)
      return 1;
   if (!(taskbar->config->show_all))
     {
        if (border->zone != taskbar->zone)
           return 1;
        if ((border->desk != e_desk_current_get(border->zone)) && !border->sticky)
           return 1;
     }

   _taskbar_item_add(taskbar, border);
   return 0;
}

static void
_taskbar_item_add(Taskbar *taskbar, E_Border *border)
{
   Taskbar_Item *item;

   item = _taskbar_item_new(taskbar, border);
   e_box_pack_end(taskbar->o_items, item->o_item);
   e_box_pack_options_set(item->o_item,
			       1, 1, /* fill */
			       1, 1, /* expand */
			       0.5, 0.5, /* align */
			       1, 1, /* min */
			       9999, 9999 /* max */
			       );
   taskbar->items = eina_list_append(taskbar->items, item);
}

static void
_taskbar_item_remove(Taskbar_Item *item)
{
   item->taskbar->items = eina_list_remove(item->taskbar->items, item);
   e_box_unpack(item->o_item);
   evas_object_del(item->o_item);
   _taskbar_item_free(item);
}

static void
_taskbar_item_free(Taskbar_Item *item)
{
   if (item->o_icon)
      evas_object_del(item->o_icon);
   e_object_unref(E_OBJECT(item->border));
   evas_object_del(item->o_item);
   free(item);
}

static void
_taskbar_item_refill(Taskbar_Item *item)
{
   if (item->o_icon)
      evas_object_del(item->o_icon);
   _taskbar_item_fill(item);
}

static void
_taskbar_item_fill(Taskbar_Item *item)
{
   item->o_icon = e_border_icon_add(item->border, evas_object_evas_get(item->taskbar->o_items));
   edje_object_part_swallow(item->o_item, "item", item->o_icon);
   evas_object_pass_events_set(item->o_icon, 1);
   evas_object_show(item->o_icon);

   item->label = e_border_name_get(item->border);
   edje_object_part_text_set(item->o_item, "label", item->label);

   if (item->border->iconic)
      _taskbar_item_signal_emit(item, "iconified", "");
   else
      _taskbar_item_signal_emit(item, "uniconified", "");
   if (item->border->focused)
      _taskbar_item_signal_emit(item, "focused", "");
   else
      _taskbar_item_signal_emit(item, "unfocused", "");
   if (item->border->client.icccm.urgent)
      _taskbar_item_signal_emit(item, "urgent", "");
   else
      _taskbar_item_signal_emit(item, "not_urgent", "");
}

static void
_taskbar_item_signal_emit(Taskbar_Item *item, char *sig, char *src)
{
   if (item->o_item)
      edje_object_signal_emit(item->o_item, sig, src);
   if (item->o_icon)
      edje_object_signal_emit(item->o_icon, sig, src);
}

static Config_Item *
_taskbar_config_item_get(const char *id)
{
   Eina_List *l;
   Config_Item *config;
   char buf[128];

   if (!id)
     {
        int num = 0;

        /* Create id */
        if (taskbar_config->items)
          {
             const char *p;

             config = eina_list_last(taskbar_config->items)->data;
             p = strrchr(config->id, '.');
             if (p)
                num = atoi(p + 1) + 1;
          }
        snprintf(buf, sizeof(buf), "%s.%d", _gadcon_class.name, num);
        id = buf;
     }
   else
     {
        for (l = taskbar_config->items; l; l = l->next)
          {
             config = l->data;
             if (!config->id)
                continue;
             if (!strcmp(config->id, id))
                return config;
          }
     }

   config = E_NEW(Config_Item, 1);
   config->id = eina_stringshare_add(id);
   config->show_all = 0;

   taskbar_config->items = eina_list_append(taskbar_config->items, config);

   return config;
}

void
_taskbar_config_updated(Config_Item *config)
{
   Eina_List *l;

   if (!taskbar_config)
      return;
   for (l = taskbar_config->taskbars; l; l = l->next)
     {
        Taskbar *taskbar;

        taskbar = l->data;
        if (taskbar->config != config)
           continue;
        _taskbar_refill(taskbar);
     }
}

static void
_taskbar_cb_menu_configure(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Taskbar *taskbar;

   taskbar = (Taskbar *) data;
   _config_taskbar_module(taskbar->config);
}

static void
_taskbar_cb_item_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{

   Evas_Event_Mouse_Down *ev;
   Taskbar_Item *item;

   item = (Taskbar_Item *) data;
   ev = event_info;
   if (ev->button == 3) 
     {

        int cx, cy, cw, ch;

        e_gadcon_canvas_zone_geometry_get(item->taskbar->gcc->gadcon, &cx, &cy, &cw, &ch);

	e_int_border_menu_create(item->border);
        E_Menu *mn;
	E_Menu_Item *mi;

	mn = e_menu_new();

	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _taskbar_cb_menu_configure, item->taskbar);


	e_gadcon_client_util_menu_items_append(item->taskbar->gcc, mn, 0);
	     
        mi = e_menu_item_new(item->border->border_menu);
        e_menu_item_label_set(mi, D_("Taskbar"));
        e_menu_item_submenu_set(mi, mn);
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
			     
	e_menu_activate_mouse(item->border->border_menu,
                                   e_util_zone_current_get(e_manager_current_get()),
                                   cx + ev->output.x, cy + ev->output.y, 1, 1, E_MENU_POP_DIRECTION_DOWN, ev->timestamp);


        evas_event_feed_mouse_up(item->taskbar->gcc->gadcon->evas, ev->button, EVAS_BUTTON_NONE, ev->timestamp, NULL);

     }

}

static void
_taskbar_cb_item_mouse_up(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Up *ev;
   Taskbar_Item *item;

   ev = event_info;
   item = data;
   if (ev->button == 1)
     {
        if (!item->border->sticky && item->taskbar->config->show_all)
           e_desk_show(item->border->desk);
        if (evas_key_modifier_is_set(ev->modifiers, "Alt"))
          {
             if (item->border->iconic)
                e_border_uniconify(item->border);
             else
                e_border_iconify(item->border);
          }
        else if (evas_key_modifier_is_set(ev->modifiers, "Control"))
          {
             if (item->border->maximized)
                e_border_unmaximize(item->border, e_config->maximize_policy);
             else
                e_border_maximize(item->border, e_config->maximize_policy);
          }
        else if (evas_key_modifier_is_set(ev->modifiers, "Shift"))
          {
             if (item->border->shaded)
                e_border_unshade(item->border, item->border->shade.dir);
             else
                e_border_shade(item->border, item->border->shade.dir);
          }
        else if (evas_key_modifier_is_set(ev->modifiers, "Super"))
          {
             e_border_act_close_begin(item->border);
          }
        else
          {
             if (item->border->iconic)
               {
                  e_border_uniconify(item->border);
                  e_border_focus_set(item->border, 1, 1);
               }
             else
               {
                  if (item->border->focused)
                    {
                       e_border_iconify(item->border);
                    }
                  else
                    {
                       e_border_raise(item->border);
                       e_border_focus_set(item->border, 1, 1);
                    }
               }
          }
     }
   else if (ev->button == 2)
     {
        if (!item->border->sticky && item->taskbar->config->show_all)
           e_desk_show(item->border->desk);
        e_border_raise(item->border);
        e_border_focus_set(item->border, 1, 1);
        if (item->border->maximized)
           e_border_unmaximize(item->border, e_config->maximize_policy);
        else
           e_border_maximize(item->border, e_config->maximize_policy);
     }
}

/************ BORDER CALLBACKS *********************/

static int
_taskbar_cb_event_border_add(void *data, int type, void *event)
{
   E_Event_Border_Add *ev;

   ev = event;
   taskbar_config->borders = eina_list_append(taskbar_config->borders, ev->border);
   _taskbar_refill_all();
   return 1;
}

static int
_taskbar_cb_event_border_remove(void *data, int type, void *event)
{
   E_Event_Border_Remove *ev;

   ev = event;
   taskbar_config->borders = eina_list_remove(taskbar_config->borders, ev->border);
   _taskbar_refill_all();
   return 1;
}

static int
_taskbar_cb_event_border_iconify(void *data, int type, void *event)
{
   E_Event_Border_Iconify *ev;

   ev = event;
   _taskbar_signal_emit(ev->border, "iconified", "");
   return 1;
}

static int
_taskbar_cb_event_border_uniconify(void *data, int type, void *event)
{
   E_Event_Border_Uniconify *ev;

   ev = event;
   _taskbar_signal_emit(ev->border, "uniconified", "");
   return 1;
}

static int
_taskbar_cb_window_focus_in(void *data, int type, void *event)
{
   E_Event_Border_Focus_In *ev;

   ev = event;
   _taskbar_signal_emit(ev->border, "focused", "");
   return 1;
}

static int
_taskbar_cb_window_focus_out(void *data, int type, void *event)
{
   E_Event_Border_Focus_Out *ev;

   ev = event;
   _taskbar_signal_emit(ev->border, "unfocused", "");
   return 1;
}

static int
_taskbar_cb_event_border_urgent_change(void *data, int type, void *event)
{
   E_Event_Border_Urgent_Change *ev;

   ev = event;
   if (ev->border->client.icccm.urgent)
      _taskbar_signal_emit(ev->border, "urgent", "");
   else
      _taskbar_signal_emit(ev->border, "not_urgent", "");
   return 1;
}

static int
_taskbar_cb_event_border_property(void *data, int type, void *event)
{
   E_Event_Border_Property *ev;
   E_Border *border;

  ev = event;
  border = ev->border;
   if (border)
      _taskbar_refill_border(border);
   return 1;
}

static int
_taskbar_cb_event_border_icon_change(void *data, int type, void *event)
{
   E_Event_Border_Icon_Change *ev;
   E_Border *border;

  ev = event;
  border = ev->border;
   if (border)
      _taskbar_refill_border(border);
   return 1;
}

static int
_taskbar_cb_event_border_zone_set(void *data, int type, void *event)
{
   _taskbar_refill_all();
   return 1;
}

static int
_taskbar_cb_event_border_desk_set(void *data, int type, void *event)
{
   _taskbar_refill_all();
   return 1;
}

static int
_taskbar_cb_event_desk_show(void *data, int type, void *event)
{
   _taskbar_refill_all();
   return 1;
}

/***************************************************************************/
 /**/

 /**/
/***************************************************************************/
