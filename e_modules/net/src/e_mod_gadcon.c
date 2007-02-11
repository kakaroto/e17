#include <e.h>
#include "e_mod_main.h"
#include "e_mod_gadcon.h"
#include "e_mod_net.h"
#include "e_mod_config.h"

static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "net", 
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon},
   E_GADCON_CLIENT_STYLE_PLAIN
};
  
static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   Instance *inst;
   E_Gadcon_Client *gcc;
   Config_Item *ci;
   char buf[PATH_MAX];

   snprintf(buf, sizeof(buf), "%s/net.edj", e_module_dir_get(cfg->mod));

   ci = _config_item_get(id);
   if (!ci->id) evas_stringshare_add(id);
   
   inst = E_NEW(Instance, 1);
   inst->o_net = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(inst->o_net, "base/theme/modules/net",
				"modules/net/main"))
     edje_object_file_set(inst->o_net, buf, "modules/net/main");
   evas_object_show(inst->o_net);

   gcc = e_gadcon_client_new(gc, name, id, style, inst->o_net);
   gcc->data = inst;
   inst->gcc = gcc;
   inst->timer = ecore_timer_add(ci->poll_time, _cb_poll, inst);
   
   evas_object_event_callback_add(inst->o_net, EVAS_CALLBACK_MOUSE_DOWN, 
				  _cb_mouse_down, inst);
   evas_object_event_callback_add(inst->o_net, EVAS_CALLBACK_MOUSE_IN, 
				  _cb_mouse_in, inst);
   evas_object_event_callback_add(inst->o_net, EVAS_CALLBACK_MOUSE_OUT, 
				  _cb_mouse_out, inst);
   
   cfg->instances = evas_list_append(cfg->instances, inst);
   return gcc;
}

static void 
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst;
   
   inst = gcc->data;
   cfg->instances = evas_list_remove(cfg->instances, inst);
   if (inst->timer) ecore_timer_del(inst->timer);
   if (inst->o_net) 
     {
	evas_object_event_callback_del(inst->o_net, EVAS_CALLBACK_MOUSE_DOWN,
				       _cb_mouse_down);
	evas_object_event_callback_del(inst->o_net, EVAS_CALLBACK_MOUSE_IN,
				       _cb_mouse_in);
	evas_object_event_callback_del(inst->o_net, EVAS_CALLBACK_MOUSE_OUT,
				       _cb_mouse_out);
	evas_object_del(inst->o_net);
     }
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
   return _("Net");
}

static Evas_Object *
_gc_icon(Evas *evas) 
{
   Evas_Object *o;
   char buf[PATH_MAX];
   
   snprintf(buf, sizeof(buf), "%s/module.edj", e_module_dir_get(cfg->mod));
   o = edje_object_add(evas);
   edje_object_file_set(o, buf, "icon");
   return o;
}

EAPI void 
_gc_register(void) 
{
   e_gadcon_provider_register(&_gc_class);
}

EAPI void 
_gc_unregister(void) 
{
   e_gadcon_provider_unregister(&_gc_class);
}
