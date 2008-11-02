/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"

#define GOLDEN_RATIO 1.618033989

typedef struct _Instance Instance;
typedef struct _Calendar Calendar;

struct _Instance 
{
   E_Gadcon_Client *gcc;
   Calendar        *calendar;
   Config_Item     *ci;

   E_Gadcon_Popup  *popup;
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
static Config_Item *_config_item_get(const char *id);
static int _update_date(void *data);
static void _update_calendar_sheet(Instance *inst);
static void _calendar_popup_content_create(Instance *inst);
static void _calendar_popup_resize(Evas_Object *obj, int *w, int *h);
static void _calendar_popup_destroy(Instance *inst);
static void _cb_action(E_Object *obj, const char *params);
static void _cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _menu_cb_post(void *data, E_Menu *m);
static void _calendar_firstweekday_su(void *data, E_Menu *m, E_Menu_Item *mi);
static void _calendar_firstweekday_mo(void *data, E_Menu *m, E_Menu_Item *mi);

static int days_in_month[2][12] =
{
   {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
   {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;
static E_Action *act = NULL;

Config *calendar_conf = NULL;

static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "calendar", 
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL},
   E_GADCON_CLIENT_STYLE_PLAIN
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   Calendar        *calendar;
   Instance        *inst;
   E_Gadcon_Client *gcc;
   char             buf[4096];

   inst = E_NEW(Instance, 1);   
   inst->ci = _config_item_get(id);

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

   calendar_conf->instances = eina_list_append(calendar_conf->instances, inst);
   evas_object_event_callback_add(calendar->o_icon, EVAS_CALLBACK_MOUSE_IN,
				   _cb_mouse_in, inst);
   evas_object_event_callback_add(calendar->o_icon, EVAS_CALLBACK_MOUSE_OUT,
				   _cb_mouse_out, inst);
   evas_object_event_callback_add(calendar->o_icon, EVAS_CALLBACK_MOUSE_DOWN,
				  _cb_mouse_down, inst);

   _update_calendar_sheet(inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst;
   Calendar *calendar;
   
   inst = gcc->data;
   calendar = inst->calendar;

   if (inst->popup) _calendar_popup_destroy(inst);
   if (calendar->o_icon) evas_object_del(calendar->o_icon);
   
   calendar_conf->instances = eina_list_remove(calendar_conf->instances, inst);
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
   return "Calendar";
}

static Evas_Object *
_gc_icon(Evas *evas) 
{
   Evas_Object *o;
   char buf[4096];

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
   Config_Item *ci;
   Eina_List   *l;
   const char *id;
   char buf[128];
   int num = 0;

   /* Create id */
   if (calendar_conf->items)
     {
	const char *p;
	ci = eina_list_last(calendar_conf->items)->data;
	p = strrchr(ci->id, '.');
	if (p) num = atoi(p + 1) + 1;
     }
   snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, num);
   id = buf;
   return id;
}

static Config_Item *
_config_item_get(const char *id) 
{
   Eina_List   *l;
   Config_Item *ci;
   char buf[128];

   if (!id)
     {
	int  num = 0;

	/* Create id */
	if (calendar_conf->items)
	  {
	     const char *p;
	     ci = eina_list_last(calendar_conf->items)->data;
	     p = strrchr(ci->id, '.');
	     if (p) num = atoi(p + 1) + 1;
	  }
	snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, num);
	id = buf;
     }
   else
     {
	for (l = calendar_conf->items; l; l = l->next) 
	  {
	     ci = l->data;
	     if (!ci->id) continue;
	     if (!strcmp(ci->id, id))
	       return ci;
	  }
     }

   ci = E_NEW(Config_Item, 1);
   ci->id = eina_stringshare_add(id);
   ci->firstweekday = 0;

   calendar_conf->items = eina_list_append(calendar_conf->items, ci);
   return ci;
}

static int
_update_date(void *data)
{
   Eina_List *l;
   time_t current_time;
   struct tm *local_time;
   static int prev_day=0;

   if (!calendar_conf->instances) return 1;

   current_time = time (NULL);
   local_time = localtime (&current_time);
   if (prev_day == local_time->tm_mday)
     return 1;
   else
     prev_day = local_time->tm_mday;

   for (l = calendar_conf->instances; l; l = l->next) 
     {
	Instance *inst;

	inst = l->data;
	if (!inst) continue;
	_update_calendar_sheet (inst);
     }

   return 1;
}

static void
_update_calendar_sheet(Instance *inst)
{
   Calendar *calendar;
   char buf[4];
   time_t current_time;
   struct tm *local_time;
   int old_popup_state=0, old_popup_pinned_state=0;

   if (!inst) return;
   calendar = inst->calendar;
   if (!calendar) return;

   current_time = time (NULL);
   local_time = localtime (&current_time);

   strftime (buf, sizeof(buf), "%d", local_time);
   edje_object_part_text_set (calendar->o_icon, "monthday", buf);
   strftime (buf, sizeof(buf), "%a", local_time);
   edje_object_part_text_set (calendar->o_icon, "weekday", buf);

   if (inst->popup)
     {
	old_popup_state = inst->popup->win->visible;
	old_popup_pinned_state = inst->popup->pinned;
     }
   _calendar_popup_content_create(inst);
   if (inst->popup && old_popup_state)
     {
	e_gadcon_popup_show(inst->popup);
	if (old_popup_pinned_state) e_gadcon_popup_toggle_pinned(inst->popup);
     }
}

static void
_calendar_popup_content_create(Instance *inst)
{
   Evas_Object *o, *of, *ob;
   Evas *evas;
   char buf[32];
   time_t current_time, start_time;
   struct tm *local_time, *local_time2;
   int row, col, day;
   int startwd, today, month, year, maxdays;

   if (inst->popup) _calendar_popup_destroy(inst);
   inst->popup = e_gadcon_popup_new(inst->gcc, _calendar_popup_resize);

   current_time = time (NULL);
   local_time = localtime (&current_time);
   today = local_time->tm_mday;
   month = local_time->tm_mon;
   year = local_time->tm_year + 1900;
   if (!(year % 4))
     maxdays = days_in_month[(!(year % 4) && (year % 100))][month];
   else
     maxdays = days_in_month[(!(year % 400))][month];

   start_time = current_time - ((today-1) * 86400);
   local_time2 = localtime (&start_time);
   strftime (buf, sizeof(buf), "%w", local_time2);
   startwd = atoi (buf) - inst->ci->firstweekday;
   if (startwd < 0) startwd = 6;

   evas = inst->popup->win->evas;
   o = e_widget_list_add(evas, 0, 0);
   strftime (buf, sizeof(buf), "%B %Y", local_time);
   of = e_widget_frametable_add(evas, buf, 0);

   /* column titles */
   day = inst->ci->firstweekday;
   for (col = 0; col <= 6; col++)
     {
	switch (day)
	  {
	     case 0: ob = e_widget_label_add(evas, "Su"); break;
	     case 1: ob = e_widget_label_add(evas, "Mo"); break;
	     case 2: ob = e_widget_label_add(evas, "Tu"); break;
	     case 3: ob = e_widget_label_add(evas, "We"); break;
	     case 4: ob = e_widget_label_add(evas, "Th"); break;
	     case 5: ob = e_widget_label_add(evas, "Fr"); break;
	     case 6: ob = e_widget_label_add(evas, "Sa"); break;
	  } 
	e_widget_frametable_object_append(of, ob, col, 0, 1, 1, 1, 0, 0, 0);
	if (day++ >= 6) day = 0;
     }

   /* output days */
   day = 0;
   for (row = 1; row <= 6; row++)
     {
	for (col = 0; col <= 6; col++)
	  {
	     if (!day) if (col == startwd) day = 1;

	     if (day && (day <= maxdays))
	       snprintf(buf, sizeof(buf), "%02d", day++);
	     else
	       buf[0] = 0;
	     ob = e_widget_label_add(evas, buf);
	     // if (day == today) // FIXME: highlight needed
	     e_widget_frametable_object_append(of, ob, col, row, 1, 1, 1, 0, 0, 0);
	}
	if (day > maxdays) break;
     }

   e_widget_list_object_append(o, of, 1, 1, 0.5);
   e_gadcon_popup_content_set(inst->popup, o);
}

static void
_calendar_popup_resize(Evas_Object *obj, int *w, int *h)
{
   if (!(*w)) *w = 0;
   if (!(*h)) *h = 0;
   /* Apply the golden ratio to the popup */
   if ((double) *w / *h > GOLDEN_RATIO) {
	*h = *w / GOLDEN_RATIO;
   } else if ((double) *w / *h < GOLDEN_RATIO - (double) 1) {
	*w = *h * (GOLDEN_RATIO - (double) 1);
   }
}

static void
_calendar_popup_destroy(Instance *inst)
{
   if (!inst->popup) return;
   e_object_del(E_OBJECT(inst->popup));
}

static void
_cb_action(E_Object *obj, const char *params)
{
   Eina_List *l;

   for (l = calendar_conf->instances; l; l = l->next) 
     {
	Instance *inst;

	inst = l->data;
	if (!inst) continue;
	if (!inst->popup) continue;
	if (inst->popup->win->visible)
	  {
	     e_gadcon_popup_toggle_pinned(inst->popup);
	     e_gadcon_popup_hide(inst->popup);
	  }
	else
	  {
	     e_gadcon_popup_toggle_pinned(inst->popup);
	     e_gadcon_popup_show(inst->popup);
	  }
     }
}

static void
_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;

   if (!(inst = data)) return;
   e_gadcon_popup_show(inst->popup);
}

static void 
_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   
   if (!(inst = data)) return;
   e_gadcon_popup_hide(inst->popup);
}

static void
_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;

   inst = data;
   ev = event_info;
   if (ev->button == 1)
     {
	e_gadcon_popup_toggle_pinned(inst->popup);
     }
   if ((ev->button == 3) && (!calendar_conf->menu))
     {
	E_Menu *mn;
	E_Menu_Item *mi;
	int cx, cy, cw, ch;

	mn = e_menu_new();
	calendar_conf->menu_firstweekday = mn;
	
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Sunday"));
	e_menu_item_radio_set(mi, 1);
	e_menu_item_radio_group_set(mi, 1);
	if (!inst->ci->firstweekday) e_menu_item_toggle_set(mi, 1);
	e_menu_item_callback_set(mi, _calendar_firstweekday_su, inst);

	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Monday"));
	e_menu_item_radio_set(mi, 1);
	e_menu_item_radio_group_set(mi, 1);
	if (inst->ci->firstweekday == 1) e_menu_item_toggle_set(mi, 1);
	e_menu_item_callback_set(mi, _calendar_firstweekday_mo, inst);

	mn = e_menu_new();
	calendar_conf->menu = mn;

	e_menu_post_deactivate_callback_set(mn, _menu_cb_post, inst);

	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("First Day of Week"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_submenu_set(mi, calendar_conf->menu_firstweekday);

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
   if (calendar_conf->menu_firstweekday)
     e_object_del(E_OBJECT(calendar_conf->menu_firstweekday));
   calendar_conf->menu_firstweekday = NULL;
}

static void
_calendar_firstweekday_su(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Instance *inst;
 
   inst = data;

   inst->ci->firstweekday = 0;
   e_config_save_queue();

   _update_calendar_sheet(inst);
}

static void
_calendar_firstweekday_mo(void *data, E_Menu *m, E_Menu_Item *mi)
{
   Instance *inst;
 
   inst = data;

   inst->ci->firstweekday = 1;
   e_config_save_queue();

   _update_calendar_sheet(inst);
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
   E_CONFIG_VAL(D, T, firstweekday, INT);
   
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
	ci->id = eina_stringshare_add("0");
	ci->firstweekday = 0;
	
	calendar_conf->items = eina_list_append(calendar_conf->items, ci);
     }
   
   calendar_conf->module = m;
   e_gadcon_provider_register(&_gc_class);
   /* add module supplied action */
   act = e_action_add("calendar");
   if (act)
     {
	act->func.go = _cb_action;
	e_action_predef_name_set(D_("Calendar"), D_("Monthview Popup (Show/Hide)"), "calendar",
				 "<none>", NULL, 0);
     }
   calendar_conf->timer = ecore_timer_add(1, _update_date, calendar_conf);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   if (calendar_conf->timer) ecore_timer_del(calendar_conf->timer);
   calendar_conf->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);
   /* remove module-supplied action */
   if (act)
     {
	e_action_predef_name_del(D_("Calendar"), D_("Monthview Popup (Show/Hide)"));
	e_action_del("calendar");
	act = NULL;
     }

   while (calendar_conf->items) 
     {
	Config_Item *ci;
	
	ci = calendar_conf->items->data;
	if (ci->id)
	  eina_stringshare_del(ci->id);
	calendar_conf->items = eina_list_remove_list(calendar_conf->items, calendar_conf->items);
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
