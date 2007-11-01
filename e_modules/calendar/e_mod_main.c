/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"

#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/sysctl.h>
#include <sys/resource.h>
#endif

typedef struct _Instance Instance;
typedef struct _Calendar Calendar;

struct _Instance 
{
   E_Gadcon_Client *gcc;
   Calendar        *calendar;
   Ecore_Timer     *timer;
};

struct _Calendar 
{
   Instance *inst;
   Evas_Object *o_icon;
};

static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);
static const char *_gc_id_new(void);
static int _update_calendar_sheet(void *data);
static void _button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _menu_cb_post(void *data, E_Menu *m);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *calendar_conf = NULL;

static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "calendar", 
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL}
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   Calendar        *calendar;
   Instance        *inst;
   E_Gadcon_Client *gcc;
   char             buf[4096];

   inst = E_NEW(Instance, 1);   

   calendar = E_NEW(Calendar, 1);
   calendar->inst = inst;

   snprintf(buf, sizeof(buf), "%s/calendar.edj", 
	    e_module_dir_get(calendar_conf->module));
   
   calendar->o_icon = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(calendar->o_icon, 
				"base/theme/modules/calendar", "modules/calendar/main"))
     edje_object_file_set(calendar->o_icon, buf, "modules/calendar/main");
   evas_object_show(calendar->o_icon);
   
   gcc = e_gadcon_client_new(gc, name, id, style, calendar->o_icon);
   gcc->data = inst;
   inst->gcc = gcc;
   inst->calendar = calendar;

   calendar_conf->instances = evas_list_append(calendar_conf->instances, inst);
   evas_object_event_callback_add(calendar->o_icon, EVAS_CALLBACK_MOUSE_DOWN,
				  _button_cb_mouse_down, inst);

   _update_calendar_sheet(inst);
   inst->timer = ecore_timer_add(1, _update_calendar_sheet, inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst;
   Calendar *calendar;
   
   inst = gcc->data;
   calendar = inst->calendar;

   if (inst->timer)
     ecore_timer_del(inst->timer);
   if (calendar->o_icon)
     evas_object_del(calendar->o_icon);
   
   calendar_conf->instances = evas_list_remove(calendar_conf->instances, inst);
   E_FREE(calendar);
   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc) 
{
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(void) 
{
   return _("Calendar");
}

static Evas_Object *
_gc_icon(Evas *evas) 
{
   Evas_Object *o;
   char         buf[4096];

   if (!calendar_conf->module) return NULL;
   
   snprintf(buf, sizeof(buf), "%s/e-module-calendar.edj", 
	    e_module_dir_get(calendar_conf->module));
   
   o = edje_object_add(evas);
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(void)
{
   const char *id;
   char buf[128];
   int num = 0;

   /* Create id */
   if (calendar_conf->items)
     {
	const char *p;
	p = strrchr(id, '.');
	if (p) num = atoi(p + 1) + 1;
     }
   snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, num);
   id = buf;
   return id;
}

static int
_update_calendar_sheet(void *data)
{
    Instance *inst;
    Calendar *calendar;
    char buf[4];
    time_t current_time;
    struct tm *local_time;

    inst = data;
    if (!inst) return 1;
    calendar = inst->calendar;
    if (!calendar) return 1;

    current_time = time (NULL);
    local_time = localtime (&current_time);
    strftime (buf, sizeof(buf), "%d", local_time);
    edje_object_part_text_set (calendar->o_icon, "monthday", buf);
    strftime (buf, sizeof(buf), "%a", local_time);
    edje_object_part_text_set (calendar->o_icon, "weekday", buf);
    return 1;
}

static void
_button_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;

   inst = data;
   ev = event_info;
   if ((ev->button == 3) && (!calendar_conf->menu))
     {
	E_Menu *mn;
	E_Menu_Item *mi;
	int cx, cy, cw, ch;

	mn = e_menu_new();
	calendar_conf->menu = mn;

	e_menu_post_deactivate_callback_set(mn, _menu_cb_post, inst);

	e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
	e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy, &cw, &ch);
	e_menu_activate_mouse(mn,
			      e_util_zone_current_get(e_manager_current_get()),
			      cx + ev->output.x, cy + ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
        evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
				 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }	
}

static void
_menu_cb_post(void *data, E_Menu *m)
{
   if (!calendar_conf->menu) return;
   e_object_del(E_OBJECT(calendar_conf->menu));
   calendar_conf->menu = NULL;
}

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION, "Calendar"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   conf_item_edd = E_CONFIG_DD_NEW("Calendar_Config_Item", Config_Item);
   conf_edd = E_CONFIG_DD_NEW("Calendar_Config", Config);
   
   #undef T
   #define T Config_Item
   #undef D
   #define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   
   #undef T
   #define T Config
   #undef D
   #define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);
   
   calendar_conf = e_config_domain_load("module.calendar", conf_edd);
   if (!calendar_conf) 
     {
	Config_Item *ci;
	
	calendar_conf = E_NEW(Config, 1);
	ci = E_NEW(Config_Item, 1);
	ci->id = evas_stringshare_add("0");
	
	calendar_conf->items = evas_list_append(calendar_conf->items, ci);
     }
   
   calendar_conf->module = m;
   e_gadcon_provider_register(&_gc_class);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   calendar_conf->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);

   while (calendar_conf->items) 
     {
	Config_Item *ci;
	
	ci = calendar_conf->items->data;
	if (ci->id)
	  evas_stringshare_del(ci->id);
	calendar_conf->items = evas_list_remove_list(calendar_conf->items, calendar_conf->items);
	E_FREE(ci);
     }

   E_FREE(calendar_conf);
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m) 
{
   e_config_domain_save("module.calendar", conf_edd, calendar_conf);
   return 1;
}
