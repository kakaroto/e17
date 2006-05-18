#include <time.h>
#include <e.h>
#include "e_mod_main.h"

/* Func Proto Requirements for Gadcon */
static E_Gadcon_Client *_gc_init     (E_Gadcon *gc, char *name, char *id, char *style);
static void             _gc_shutdown (E_Gadcon_Client *gcc);
static void             _gc_orient   (E_Gadcon_Client *gcc);
static char            *_gc_label    (void);
static Evas_Object     *_gc_icon     (Evas *evas);

/* Module Protos */
static void _tclock_cb_mouse_down (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _tclock_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _tclock_menu_cb_post(void *data, E_Menu *m);
static int  _tclock_cb_check(void *data);
static Config_Item *_tclock_config_item_get(const char *id);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *tclock_config = NULL;

/* Define the class and gadcon functions this module provides */
static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION,
     "tclock",
     {
	_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon
     }
};

typedef struct _Instance Instance;
struct _Instance 
{
   E_Gadcon_Client *gcc;
   Evas_Object *tclock;
   Config_Item *cfg;
   const char *id;
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, char *name, char *id, char *style) 
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;
   char buf[4096];
   Config_Item *ci;
   
   inst = E_NEW(Instance, 1);
   
   ci = _tclock_config_item_get(gc->id);
   if (!ci->id) ci->id = evas_stringshare_add(gc->id);
   inst->id = evas_stringshare_add(ci->id);
   
   o = edje_object_add(gc->evas);
   snprintf(buf, sizeof(buf), "%s/tclock.edj", e_module_dir_get(tclock_config->module));
   if (!e_theme_edje_object_set(o, "base/theme/modules/tclock", "modules/tclock/main"))
      edje_object_file_set(o, buf, "modules/tclock/main");
   evas_object_show(o);
   
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;
   inst->gcc = gcc;
   inst->tclock = o;
   inst->cfg = ci;
   
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN,
				  _tclock_cb_mouse_down, inst);
   tclock_config->instances = evas_list_append(tclock_config->instances, inst);

   _tclock_cb_check(NULL);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst;
   
   inst = gcc->data;
   evas_stringshare_del(inst->id);
   tclock_config->instances = evas_list_remove(tclock_config->instances, inst);   
   evas_object_del(inst->tclock);
   free(inst);
}

static void 
_gc_orient(E_Gadcon_Client *gcc) 
{
   Instance *inst;
   
   inst = gcc->data;
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(void) 
{
   return D_("TClock");
}

static Evas_Object *
_gc_icon(Evas *evas) 
{
   Evas_Object *o;
   char buf[4096];
   
   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/module.eap", 
	    e_module_dir_get(tclock_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static void 
_tclock_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;
   
   inst = data;
   ev = event_info;
   if ((ev->button == 3) && (!tclock_config->menu)) 
     {
	E_Menu *mn;
	E_Menu_Item *mi;
	int x, y, w, h;
	
	mn = e_menu_new();
	e_menu_post_deactivate_callback_set(mn, _tclock_menu_cb_post, inst);
	tclock_config->menu = mn;
	
	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _tclock_menu_cb_configure, inst);
	
	e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
	e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon,
					  &x, &y, &w, &h);
	e_menu_activate_mouse(mn,
			      e_util_zone_current_get(e_manager_current_get()),
			      x + ev->output.x, y + ev->output.y, 1, 1,
			      E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
				 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }   
}

static void
_tclock_menu_cb_post(void *data, E_Menu *m) 
{
   if (!tclock_config->menu)
     return;
   e_object_del(E_OBJECT(tclock_config->menu));
   tclock_config->menu = NULL;
}

static void
_tclock_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   Instance *inst;
   Config_Item *ci;
   
   if (!tclock_config)
     return;
   if (tclock_config->config_dialog)
     return;
   
   inst = data;
   ci = _tclock_config_item_get(inst->gcc->gadcon->id);
   _config_tclock_module(ci);
}

void
_tclock_config_updated(void) 
{
   Evas_List *l;
   
   if (!tclock_config)
     return;

   for (l = tclock_config->instances; l; l = l->next) 
     {
	Instance *inst;
	Config_Item *ci;
	
	inst = l->data;
	ci = _tclock_config_item_get(inst->gcc->gadcon->id);
	if ((inst->id) && (ci->id) && (!strcmp(inst->id, ci->id)))
	  {
	     inst->cfg = ci;
	
	     if (!inst->cfg->show_time) 
	       edje_object_signal_emit(inst->tclock, "time_hidden", "");
	     else 
	       edje_object_signal_emit(inst->tclock, "time_visible", "");	     
	     edje_object_message_signal_process(inst->tclock);
	
	     if (!inst->cfg->show_date) 
	       edje_object_signal_emit(inst->tclock, "date_hidden", "");
	     else  
	       edje_object_signal_emit(inst->tclock, "date_visible", "");
	     edje_object_message_signal_process(inst->tclock);
	
	     if (tclock_config->tclock_check_timer) 
	       ecore_timer_del(tclock_config->tclock_check_timer);
   
	     tclock_config->tclock_check_timer = ecore_timer_add(inst->cfg->poll_time,
								 _tclock_cb_check, NULL);
	     _tclock_cb_check(NULL);
	  }
     }
}

static int 
_tclock_cb_check(void *data) 
{
   Instance *inst;
   Evas_List *l;
   time_t current_time;
   struct tm *local_time;
   char buf[1024];

   memset(buf, 0, sizeof(buf));
   current_time = time(NULL);
   local_time = localtime(&current_time);

   for (l = tclock_config->instances; l; l = l->next) 
     {
	Instance *inst;
	Config_Item *ci;
	
	inst = l->data;
	ci = _tclock_config_item_get(inst->gcc->gadcon->id);
	inst->cfg = ci;
	
	if (!inst->cfg->show_time) 
	  edje_object_signal_emit(inst->tclock, "time_hidden", "");
	else 
	  edje_object_signal_emit(inst->tclock, "time_visible", "");
	edje_object_message_signal_process(inst->tclock);
	
	if (!inst->cfg->show_date) 
	  edje_object_signal_emit(inst->tclock, "date_hidden", "");
	else 
	  edje_object_signal_emit(inst->tclock, "date_visible", "");
	edje_object_message_signal_process(inst->tclock);
	
	strftime(buf, 1024, inst->cfg->time_format, local_time);
	edje_object_part_text_set(inst->tclock, "tclock_time", buf);
	strftime(buf, 1024, inst->cfg->date_format, local_time);
	edje_object_part_text_set(inst->tclock, "tclock_date", buf);
     }
   return 1;
}

static Config_Item *
_tclock_config_item_get(const char *id) 
{
   Evas_List *l;
   Config_Item *ci;
   
   for (l = tclock_config->items; l; l = l->next) 
     {
	ci = l->data;
	if ((ci->id) && (!strcmp(ci->id, id)))
	  return ci;
     }
   ci = E_NEW(Config_Item, 1);
   ci->id = evas_stringshare_add(id);
   ci->poll_time = 1.0;
   ci->resolution = RESOLUTION_SECOND;
   ci->show_date = 1;
   ci->show_time = 1;
   ci->time_format = strdup("%T");
   ci->date_format = strdup("%d/%m/%y");
   
   tclock_config->items = evas_list_append(tclock_config->items, ci);
   return ci;
}

EAPI E_Module_Api e_modapi = {
   E_MODULE_API_VERSION,
   "TClock"
};

EAPI void *
e_modapi_init(E_Module *m)
{   
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   conf_item_edd = E_CONFIG_DD_NEW("TClock_Config_Item", Config_Item);
   #undef T
   #undef D
   #define T Config_Item
   #define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, poll_time, DOUBLE);
   E_CONFIG_VAL(D, T, resolution, INT);
   E_CONFIG_VAL(D, T, show_date, INT);
   E_CONFIG_VAL(D, T, show_time, INT);
   E_CONFIG_VAL(D, T, date_format, STR);
   E_CONFIG_VAL(D, T, time_format, STR);
   
   conf_edd = E_CONFIG_DD_NEW("TClock_Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);
   
   tclock_config = e_config_domain_load("module.tclock", conf_edd);
   if (!tclock_config) 
     {
	Config_Item *ci;
	
	tclock_config = E_NEW(Config, 1);

	ci = E_NEW(Config_Item, 1);
	ci->poll_time = 1.0;
	ci->resolution = RESOLUTION_SECOND;
	ci->show_date = 1;
	ci->show_time = 1;
	ci->time_format = strdup("%T");
	ci->date_format = strdup("%d/%m/%y");
	ci->id = evas_stringshare_add("0");
	
	E_CONFIG_LIMIT(ci->poll_time, 1.0, 60.0);
	E_CONFIG_LIMIT(ci->resolution, 0, 1);
	E_CONFIG_LIMIT(ci->show_date, 0, 1);
	E_CONFIG_LIMIT(ci->show_time, 0, 1);
	
	tclock_config->items = evas_list_append(tclock_config->items, ci);
	tclock_config->tclock_check_timer = ecore_timer_add(ci->poll_time,
						       _tclock_cb_check, NULL);
     }
   
   tclock_config->module = m;

   e_gadcon_provider_register(&_gc_class);
   return 1;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   tclock_config->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);
   
   if (tclock_config->config_dialog)
     e_object_del(E_OBJECT(tclock_config->config_dialog));
   
   if (tclock_config->tclock_check_timer)
     ecore_timer_del(tclock_config->tclock_check_timer);
   if (tclock_config->menu) 
     {
	e_menu_post_deactivate_callback_set(tclock_config->menu, NULL, NULL);
	e_object_del(E_OBJECT(tclock_config->menu));
	tclock_config->menu = NULL;
     }

   while (tclock_config->items) 
     {
	Config_Item *ci;
	
	ci = tclock_config->items->data;
	if (ci->id) evas_stringshare_del(ci->id);
	tclock_config->items = evas_list_remove_list(tclock_config->items, tclock_config->items);
	free(ci);
     }
   
   free(tclock_config);
   tclock_config = NULL;
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_info(E_Module *m)
{
   char buf[4096];
   
   snprintf(buf, sizeof(buf), "%s/module_icon.png", e_module_dir_get(m));
   m->icon_file = strdup(buf);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   Evas_List *l;
   
   for (l = tclock_config->instances; l; l = l->next) 
     {
	Instance *inst;
	Config_Item *ci;
	
	inst = l->data;
	ci = _tclock_config_item_get(inst->gcc->gadcon->id);
	if (ci->id) evas_stringshare_del(ci->id);
	ci->id = evas_stringshare_add(inst->gcc->gadcon->id);
     }   
   e_config_domain_save("module.tclock", conf_edd, tclock_config);
   return 1;
}

EAPI int
e_modapi_about(E_Module *m)
{
   e_module_dialog_show(D_("Simple Digital Clock"), 
			D_("Displays a digital clock on the desktop"));
   return 1;
}
