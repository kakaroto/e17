#include <e.h>
#include "e_mod_config.h"
#include "e_mod_main.h"
#include "e_mod_gadcon.h"

/* local function prototypes */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient);
static char *_gc_label(E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas);
static const char *_gc_id_new(E_Gadcon_Client_Class *client_class);
static void _gc_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _gc_cb_menu_post(void *data, E_Menu  *menu);
static void _menu_cb_configure(void *data, E_Menu * m, E_Menu_Item * mi);

static const E_Gadcon_Client_Class _gc_class =
{
   GADCON_CLIENT_CLASS_VERSION, "eweather",
     { _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new,
          NULL, NULL },
   E_GADCON_CLIENT_STYLE_PLAIN
};

/* local functions */
static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Instance *inst;
   E_Gadcon_Client *gcc;
   char buff[PATH_MAX];

   inst = E_NEW(Instance, 1);
   inst->ci = _weather_config_item_get(inst, id);

   snprintf(buff, sizeof(buff), "%s/eweather.edj", weather_cfg->mod_dir);
   if(gc->location->site == E_GADCON_SITE_DESKTOP)
     {
         inst->obj = eweather_object_add(gc->evas);
         inst->eweather = eweather_object_eweather_get(inst->obj);
     }
   else
     {
        inst->ci->inst = NULL;
        E_FREE(inst);
        return NULL; // what to do, no eweather...
     }
   evas_object_event_callback_add(inst->obj, EVAS_CALLBACK_MOUSE_DOWN,
                                  _gc_cb_mouse_down, inst);
   evas_object_show(inst->obj);

   gcc = e_gadcon_client_new(gc, name, id, style, inst->obj);
   gcc->data = inst;
   inst->gcc = gcc;

   weather_cfg->instances = eina_list_append(weather_cfg->instances, inst);

   eweather_poll_time_set(inst->eweather, inst->ci->poll_time);
   if(!strcmp(inst->ci->plugin, "Yahoo"))
       eweather_code_set(inst->eweather, inst->ci->location);
   else
       eweather_code_set(inst->eweather, inst->ci->google);
   if(inst->ci->plugin)
       eweather_plugin_byname_set(inst->eweather, inst->ci->plugin);
   if(inst->ci->celcius)
       eweather_temp_type_set(inst->eweather, EWEATHER_TEMP_CELCIUS);
    
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;

   inst = gcc->data;
   inst->ci->inst = NULL;
   weather_cfg->instances = eina_list_remove(weather_cfg->instances, inst);

   if (inst->menu)
     {
        e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
        e_object_del(E_OBJECT(inst->menu));
        inst->menu = NULL;
     }
   if (inst->obj)
     {
        evas_object_event_callback_del(inst->obj, EVAS_CALLBACK_MOUSE_DOWN,
                                       _gc_cb_mouse_down);
        evas_object_del(inst->obj);
     }
   E_FREE(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc, E_Gadcon_Orient orient)
{
   e_gadcon_client_aspect_set(gcc, 16, 16);
   e_gadcon_client_min_size_set(gcc, 16, 16);
}

static char *
_gc_label(E_Gadcon_Client_Class *client_class)
{
   return D_("EWeather");
}

static Evas_Object *
_gc_icon(E_Gadcon_Client_Class *client_class, Evas *evas)
{
   Evas_Object *o;
   char buff[PATH_MAX];

   snprintf(buff, sizeof(buff), "%s/eweather.edj", weather_cfg->mod_dir);
   o = edje_object_add(evas);
   edje_object_file_set(o, buff, "icon");
   return o;
}

static const char *
_gc_id_new(E_Gadcon_Client_Class *client_class)
{
   Config_Item *ci;

   ci = _weather_config_item_get(NULL, NULL);
   return ci->id;
}

static void
_gc_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;

   if (!(inst = data)) return;
   ev = event;
   if ((ev->button == 3) && (!inst->menu))
     {
        E_Menu *m;
        E_Menu_Item *mi;
        E_Zone *zone;
        int x = 0, y = 0;

        zone = e_util_zone_current_get(e_manager_current_get());

        m = e_menu_new();

        mi = e_menu_item_new(m);
        e_menu_item_label_set(mi, D_("Settings"));
        e_util_menu_item_theme_icon_set(mi, "configure");
        e_menu_item_callback_set(mi, _menu_cb_configure, inst);

        m = e_gadcon_client_util_menu_items_append(inst->gcc, m, 0);
        e_menu_post_deactivate_callback_set(m, _gc_cb_menu_post, inst);
        inst->menu = m;

        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, NULL, NULL);
        e_menu_activate_mouse(m, zone, x + ev->output.x, y + ev->output.y,
                              1, 1, E_MENU_POP_DIRECTION_AUTO, ev->timestamp);
     }
}

static void
_menu_cb_configure(void *data, E_Menu * m, E_Menu_Item * mi)
{
   Instance *inst;

   inst = data;
   weather_config_dialog(inst->ci);
}

static void
_gc_cb_menu_post(void *data, E_Menu  *menu)
{
   Instance *inst;

   if (!(inst = data)) return;
   if (!inst->menu) return;
   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

/* public functions */
EINTERN void
_gc_register(void)
{
   e_gadcon_provider_register(&_gc_class);
}

EINTERN void
_gc_unregister(void)
{
   e_gadcon_provider_unregister(&_gc_class);
}

EINTERN const char *
_gc_name(void)
{
   return _gc_class.name;
}
