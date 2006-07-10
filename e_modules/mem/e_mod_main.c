#include <e.h>
#include "e_mod_main.h"

typedef struct _Instance Instance;
typedef struct _Mem Mem;

struct _Instance 
{
   E_Gadcon_Client *gcc;
   Evas_Object *mem_obj;
   Mem *mem;
   Ecore_Timer *check_timer;
};

struct _Mem 
{
   Instance *inst;
   Evas_Object *mem_obj;
};

/* Func Protos for Gadcon */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);

/* Func Protos for Module */
static void _mem_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _mem_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _mem_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _mem_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi);
static void _mem_menu_cb_post(void *data, E_Menu *m);
static Config_Item *_mem_config_item_get(const char *id);
static Mem *_mem_new(Evas *evas);
static void _mem_free(Mem *mem);
static int _mem_cb_check(void *data);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *mem_config = NULL;

static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION,
     "mem", {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon}
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   E_Gadcon_Client *gcc;
   Evas_Object *o;
   Instance *inst;
   Config_Item *ci;
   Mem *mem;
   
   inst = E_NEW(Instance, 1);
   
   ci = _mem_config_item_get(id);
   if (!ci->id) ci->id = evas_stringshare_add(id);
   
   mem = _mem_new(gc->evas);
   mem->inst = inst;
   inst->mem = mem;
   
   o = mem->mem_obj;
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;
   inst->gcc = gcc;
   inst->mem_obj = o;

   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_DOWN, _mem_cb_mouse_down, inst);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_IN, _mem_cb_mouse_in, inst);
   evas_object_event_callback_add(o, EVAS_CALLBACK_MOUSE_OUT, _mem_cb_mouse_out, inst);
   
   if (ci->always_text)
     edje_object_signal_emit(inst->mem_obj, "label_active", "");
   
   _mem_cb_check(inst);
   
   inst->check_timer = ecore_timer_add(ci->poll_time, _mem_cb_check, inst);
   mem_config->instances = evas_list_append(mem_config->instances, inst);
   return gcc;
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
   return D_("Mem");
}

static Evas_Object *
_gc_icon(Evas *evas) 
{
   Evas_Object *o;
   char buf[4096];
   
   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/module.eap", e_module_dir_get(mem_config->module));
   edje_object_file_set(o, buf, "icon");
   return o;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst;
   
   inst = gcc->data;
   if (inst->check_timer) ecore_timer_del(inst->check_timer);
   mem_config->instances = evas_list_remove(mem_config->instances, inst);
   _mem_free(inst->mem);
   free(inst);
   inst = NULL;
}

static void
_mem_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info) 
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;

   inst = data;
   ev = event_info;
   if ((ev->button == 3) && (!mem_config->menu))
     {
        E_Menu *mn;
        E_Menu_Item *mi;
        int x, y, w, h;

        mn = e_menu_new();
        e_menu_post_deactivate_callback_set(mn, _mem_menu_cb_post, inst);
        mem_config->menu = mn;

        mi = e_menu_item_new(mn);
        e_menu_item_label_set(mi, D_("Configuration"));
        e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
        e_menu_item_callback_set(mi, _mem_menu_cb_configure, inst);

        mi = e_menu_item_new(mn);
        e_menu_item_separator_set(mi, 1);

        e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, &w, &h);
        e_menu_activate_mouse(mn,
                              e_util_zone_current_get(e_manager_current_get()),
                              x + ev->output.x, y + ev->output.y, 1, 1, E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
        evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button, EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }   
}

static void
_mem_menu_cb_post(void *data, E_Menu *m) 
{
   if (!mem_config->menu) return;
   e_object_del(E_OBJECT(mem_config->menu));
   mem_config->menu = NULL;
}

static void
_mem_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   Instance *inst;
   Config_Item *ci;
   
   inst = data;
   ci = _mem_config_item_get(inst->gcc->id);
   _config_mem_module(ci);
}

void
_mem_config_updated(const char *id) 
{
   Evas_List *l;
   Config_Item *ci;

   if (!mem_config) return;
   ci = _mem_config_item_get(id);
   for (l = mem_config->instances; l; l = l->next)
     {
        Instance *inst;

        inst = l->data;
        if (!inst->gcc->id) continue;

        if (!strcmp(inst->gcc->id, ci->id))
          {
             if (inst->check_timer) ecore_timer_del(inst->check_timer);
             inst->check_timer = ecore_timer_add((double)ci->poll_time, _mem_cb_check, inst);
             if (ci->always_text)
                edje_object_signal_emit(inst->mem_obj, "label_active", "");
             else
                edje_object_signal_emit(inst->mem_obj, "label_passive", "");

             break;
          }
     }   
}

static Config_Item *
_mem_config_item_get(const char *id) 
{
   Evas_List *l;
   Config_Item *ci;

   for (l = mem_config->items; l; l = l->next)
     {
        ci = l->data;
        if (!ci->id) continue;
        if (!strcmp(ci->id, id)) return ci;
     }
   ci = E_NEW(Config_Item, 1);
   ci->id = evas_stringshare_add(id);
   ci->poll_time = 1.0;
   ci->always_text = 0;
   ci->show_percent = 1;
#ifdef __linux__
   ci->real_ignore_buffers = 0;
   ci->real_ignore_cached = 0;
#endif

   mem_config->items = evas_list_append(mem_config->items, ci);
   return ci;   
}

EAPI E_Module_Api e_modapi = 
{
   E_MODULE_API_VERSION,
     "Mem"
};

EAPI void *
e_modapi_init(E_Module *m) 
{
   bindtextdomain(PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   conf_item_edd = E_CONFIG_DD_NEW("Mem_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, poll_time, DOUBLE);
   E_CONFIG_VAL(D, T, always_text, INT);
   E_CONFIG_VAL(D, T, show_percent, INT);
#ifdef __linux__
   E_CONFIG_VAL(D, T, real_ignore_buffers, INT);
   E_CONFIG_VAL(D, T, real_ignore_cached, INT);
#endif

   conf_edd = E_CONFIG_DD_NEW("Mem_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   mem_config = e_config_domain_load("module.mem", conf_edd);
   if (!mem_config)
     {
        Config_Item *ci;

        mem_config = E_NEW(Config, 1);

        ci = E_NEW(Config_Item, 1);
        ci->id = evas_stringshare_add("0");
        ci->poll_time = 1.0;
        ci->always_text = 0;
	ci->show_percent = 1;
#ifdef __linux__
	ci->real_ignore_buffers = 0;
	ci->real_ignore_cached = 0;
#endif
        mem_config->items = evas_list_append(mem_config->items, ci);
     }
   mem_config->module = m;
   e_gadcon_provider_register(&_gc_class);
   return 1;   
}

EAPI int
e_modapi_shutdown(E_Module *m) 
{
   mem_config->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);

   if (mem_config->config_dialog)
      e_object_del(E_OBJECT(mem_config->config_dialog));
   if (mem_config->menu)
     {
        e_menu_post_deactivate_callback_set(mem_config->menu, NULL, NULL);
        e_object_del(E_OBJECT(mem_config->menu));
        mem_config->menu = NULL;
     }
   while (mem_config->items)
     {
        Config_Item *ci;

        ci = mem_config->items->data;
        mem_config->items = evas_list_remove_list(mem_config->items, mem_config->items);
        if (ci->id) evas_stringshare_del(ci->id);
        free(ci);
	ci = NULL;
     }
   free(mem_config);
   mem_config = NULL;
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;   
}

EAPI int
e_modapi_save(E_Module *m) 
{
   Evas_List *l;

   for (l = mem_config->instances; l; l = l->next)
     {
        Instance *inst;
        Config_Item *ci;

        inst = l->data;
        ci = _mem_config_item_get(inst->gcc->id);
        if (ci->id) evas_stringshare_del(ci->id);
        ci->id = evas_stringshare_add(inst->gcc->id);
     }
   e_config_domain_save("module.mem", conf_edd, mem_config);
   return 1;   
}

EAPI int
e_modapi_about(E_Module *m) 
{
   e_module_dialog_show(m, D_("Enlightenment Mem Monitor Module"), 
			D_("This module is used to monitor memory."));
   return 1;   
}

static Mem *
_mem_new(Evas *evas)
{
   Mem *mem;
   char buf[4096];

   mem = E_NEW(Mem, 1);

   mem->mem_obj = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/mem.edj", e_module_dir_get(mem_config->module));
   if (!e_theme_edje_object_set(mem->mem_obj, "base/theme/modules/mem", "modules/mem/main"))
      edje_object_file_set(mem->mem_obj, buf, "modules/mem/main");
   evas_object_show(mem->mem_obj);

   return mem;
}

static void
_mem_free(Mem *m)
{
   evas_object_del(m->mem_obj);
   free(m);
   m = NULL;
}

static void
_mem_cb_mouse_in(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;

   inst = data;
   edje_object_signal_emit(inst->mem_obj, "label_active", "");
}

static void
_mem_cb_mouse_out(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Config_Item *ci;

   inst = data;
   ci = _mem_config_item_get(inst->gcc->id);
   if (!ci->always_text)
      edje_object_signal_emit(inst->mem_obj, "label_passive", "");
}

static int
_mem_cb_check(void *data)
{
   Instance *inst;
   Config_Item *ci;
   Edje_Message_Float msg;
   int real, swap, total_real, total_swap;
   char real_str[100];
   char swap_str[100];

   inst = data;
   ci = _mem_config_item_get(inst->gcc->id);
   _mem_get_values(ci, &real, &swap, &total_real, &total_swap);
   
   if (!ci->show_percent)
     {
	snprintf(real_str, sizeof(real_str), "Real: %d/%d MB", (real / 1024), (total_real / 1024));
	snprintf(swap_str, sizeof(swap_str), "Swap: %d/%d MB", (swap / 1024), (total_swap / 1024));
     }
   else
     {
	double tr;
	
	tr = (((double)real / (double)total_real) * 100);
	snprintf(real_str, sizeof(real_str), "Real: %1.2f%%", tr);
	tr = (((double)swap / (double)total_swap) * 100);
	snprintf(swap_str, sizeof(swap_str), "Swap: %1.2f%%", tr);
     }
   edje_object_part_text_set(inst->mem_obj, "real_label", real_str);
   edje_object_part_text_set(inst->mem_obj, "swap_label", swap_str);

   double tr = ((double)real / (double)total_real);
   msg.val = tr;
   edje_object_message_send(inst->mem_obj, EDJE_MESSAGE_FLOAT, 1, &msg);

   double ts = ((double)swap / (double)total_swap);
   msg.val = ts;
   edje_object_message_send(inst->mem_obj, EDJE_MESSAGE_FLOAT, 2, &msg);

   return 1;
}
