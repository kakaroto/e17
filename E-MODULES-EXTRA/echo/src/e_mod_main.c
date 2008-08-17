#include <e.h>
#include "e_mod_main.h"
#include "e_mod_system.h"

/* gadcon function protos */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);
static const char *_gc_id_new(void);

/* local function protos */
static void _echo_cfg_free(void);
static void _echo_cfg_new(void);
static int _echo_cfg_timer(void *data);
static void _echo_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _echo_cb_menu_post(void *data, E_Menu *menu);
static void _echo_cb_menu_cfg(void *data, E_Menu *menu, E_Menu_Item *mi);
static void _echo_cb_popup_new(void *data);
static void _echo_cb_popup_del(void *data);
static void _echo_cb_popup_resize(Evas_Object *obj, int *w, int *h);
static void _echo_cb_volume_left_change(void *data, Evas_Object *obj, void *event);
static void _echo_cb_volume_right_change(void *data, Evas_Object *obj, void *event);
static void _echo_cb_mute_change(void *data, Evas_Object *obj, void *event);
static void _echo_cb_update_inst(void *data);
static void _echo_cb_signal_volume_left_up(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _echo_cb_signal_volume_left_down(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _echo_cb_signal_volume_right_up(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _echo_cb_signal_volume_right_down(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _echo_cb_signal_volume_mute_toggle(void *data, Evas_Object *obj, const char *emission, const char *source);
static void _echo_cb_volume_increase(E_Object *obj, const char *params);
static void _echo_cb_volume_decrease(E_Object *obj, const char *params);
static void _echo_cb_volume_mute(E_Object *obj, const char *params);

typedef struct _Instance Instance;
struct _Instance 
{
   E_Gadcon_Client *gcc;
   E_Gadcon_Popup *popup;
   E_Menu *menu;

   Evas_Object *o_base;
   Evas_Object *o_lslider, *o_rslider;
   int mute, left_vol, right_vol;
};

/* local variables */
static Evas_List *instances = NULL;
static E_Config_DD *conf_edd = NULL;
static E_Action *incr_act = NULL;
static E_Action *decr_act = NULL;
static E_Action *mute_act = NULL;
E_Module *echo_mod = NULL;
Config *echo_cfg = NULL;

/* Gadcon Api Functions */
static const E_Gadcon_Client_Class _gc_class = 
{
   GADCON_CLIENT_CLASS_VERSION, "echo", 
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL},
   E_GADCON_CLIENT_STYLE_PLAIN
};

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style) 
{
   Instance *inst = NULL;
   char buf[PATH_MAX];

   snprintf(buf, sizeof(buf), "%s/e-module-echo.edj", echo_mod->dir);

   inst = E_NEW(Instance, 1);
   inst->o_base = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(inst->o_base, "base/theme/modules/echo", 
                                "modules/echo/main"))
     edje_object_file_set(inst->o_base, buf, "modules/echo/main");

   inst->gcc = e_gadcon_client_new(gc, name, id, style, inst->o_base);
   inst->gcc->data = inst;

   evas_object_event_callback_add(inst->o_base, EVAS_CALLBACK_MOUSE_DOWN, 
                                  _echo_cb_mouse_down, inst);
   edje_object_signal_callback_add(inst->o_base, "e,action,volume,left,up", "", 
                                   _echo_cb_signal_volume_left_up, inst);
   edje_object_signal_callback_add(inst->o_base, "e,action,volume,left,down", "", 
                                   _echo_cb_signal_volume_left_down, inst);
   edje_object_signal_callback_add(inst->o_base, "e,action,volume,right,up", "", 
                                   _echo_cb_signal_volume_right_up, inst);
   edje_object_signal_callback_add(inst->o_base, "e,action,volume,right,down", "", 
                                   _echo_cb_signal_volume_right_down, inst);
   edje_object_signal_callback_add(inst->o_base, "e,action,volume,mute,toggle", "", 
                                   _echo_cb_signal_volume_mute_toggle, inst);

   inst->left_vol = inst->right_vol = -1;
   if (echo_sys->get_volume)
     echo_sys->get_volume(echo_cfg->channel.name, 
                          &(inst->left_vol), &(inst->right_vol));

   if (echo_sys->get_mute)
     echo_sys->get_mute(echo_cfg->channel.name, &(inst->mute));

   _echo_cb_update_inst(inst);

   instances = evas_list_append(instances, inst);
   return inst->gcc;
}

static void 
_gc_shutdown(E_Gadcon_Client *gcc) 
{
   Instance *inst = NULL;

   if (!(inst = gcc->data)) return;
   instances = evas_list_remove(instances, inst);
   if (inst->menu) 
     {
        e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
        e_object_del(E_OBJECT(inst->menu));
        inst->menu = NULL;
     }
   if (inst->popup) _echo_cb_popup_del(inst);
   if (inst->o_base) 
     {
        evas_object_event_callback_del(inst->o_base, EVAS_CALLBACK_MOUSE_DOWN, 
                                       _echo_cb_mouse_down);
     	edje_object_signal_callback_del(inst->o_base, 
                                        "e,action,volume,left,up", "", 
                                        _echo_cb_signal_volume_left_up);
	edje_object_signal_callback_del(inst->o_base, 
                                        "e,action,volume,left,down", "", 
                                        _echo_cb_signal_volume_left_down);
     	edje_object_signal_callback_del(inst->o_base, 
                                        "e,action,volume,right,up", "", 
                                        _echo_cb_signal_volume_right_up);
      	edje_object_signal_callback_del(inst->o_base, 
                                        "e,action,volume,right,down", "", 
                                        _echo_cb_signal_volume_right_down);
     	edje_object_signal_callback_del(inst->o_base, 
                                        "e,action,volume,mute,toggle", "", 
                                        _echo_cb_signal_volume_mute_toggle);
        evas_object_del(inst->o_base);
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
   return D_("Echo");
}

static Evas_Object *
_gc_icon(Evas *evas) 
{
   Evas_Object *o = NULL;
   char buf[PATH_MAX];

   snprintf(buf, sizeof(buf), "%s/e-module-echo.edj", echo_mod->dir);
   o = edje_object_add(evas);
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(void) 
{
   static char buf[PATH_MAX];

   snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, 
            evas_list_count(instances));
   return buf;
}

/* Module Api Functions */
EAPI E_Module_Api e_modapi = {E_MODULE_API_VERSION, "Echo"};

EAPI void *
e_modapi_init(E_Module *m) 
{
   /* register config panel entry */
   e_configure_registry_category_add("extensions", 90, D_("Extensions"), NULL, 
                                     "enlightenment/extensions");
   e_configure_registry_item_add("extensions/echo", 30, D_("Echo"), NULL, 
                                 "enlightenment/e", e_int_config_echo_module);

   conf_edd = E_CONFIG_DD_NEW("Config", Config);
   #undef T
   #undef D
   #define T Config
   #define D conf_edd
   E_CONFIG_VAL(D, T, version, INT);
   E_CONFIG_VAL(D, T, lock_sliders, INT);
   E_CONFIG_VAL(D, T, card.id, INT);
   E_CONFIG_VAL(D, T, channel.id, INT);
   E_CONFIG_VAL(D, T, channel.name, STR);

   echo_cfg = e_config_domain_load("module.echo", conf_edd);
   if (echo_cfg) 
     {
        if ((echo_cfg->version >> 16) < MOD_CFG_FILE_EPOCH) 
          {
             _echo_cfg_free();
	     ecore_timer_add(1.0, _echo_cfg_timer,
			     D_("Echo Module Configuration data needed "
                                "upgrading. Your old configuration<br> has been"
                                " wiped and a new set of defaults initialized. "
                                "This<br>will happen regularly during "
                                "development, so don't report a<br>bug. "
                                "This simply means the Echo module needs "
                                "new configuration<br>data by default for "
                                "usable functionality that your old<br>"
                                "configuration simply lacks. This new set of "
                                "defaults will fix<br>that by adding it in. "
                                "You can re-configure things now to your<br>"
                                "liking. Sorry for the inconvenience.<br>"));
          }
        else if (echo_cfg->version > MOD_CFG_FILE_VERSION) 
          {
             _echo_cfg_free();
	     ecore_timer_add(1.0, _echo_cfg_timer, 
			     D_("Your Echo Module Configuration is NEWER "
                                "than the Echo Module version. This is "
                                "very<br>strange. This should not happen unless"
                                " you downgraded<br>the Echo Module or "
                                "copied the configuration from a place where"
                                "<br>a newer version of the Echo Module "
                                "was running. This is bad and<br>as a "
                                "precaution your configuration has been now "
                                "restored to<br>defaults. Sorry for the "
                                "inconvenience.<br>"));
          }
     }
   if (!echo_cfg) _echo_cfg_new();

   if (!e_mod_system_init(echo_cfg->card.id)) return NULL;

   incr_act = e_action_add("volume_increase");
   if (incr_act) 
     {
        incr_act->func.go = _echo_cb_volume_increase;
        e_action_predef_name_set(D_("Echo"), D_("Increase Volume"), 
                                 "volume_increase", NULL, NULL, 0);
     }
   decr_act = e_action_add("volume_decrease");
   if (decr_act) 
     {
        decr_act->func.go = _echo_cb_volume_decrease;
        e_action_predef_name_set(D_("Echo"), D_("Decrease Volume"), 
                                 "volume_decrease", NULL, NULL, 0);
     }
   mute_act = e_action_add("volume_mute");
   if (mute_act) 
     {
        mute_act->func.go = _echo_cb_volume_mute;
        e_action_predef_name_set(D_("Echo"), D_("Mute Volume"), "volume_mute", 
                                 NULL, NULL, 0);
     }

   echo_mod = m;
   e_gadcon_provider_register(&_gc_class);
   return m;
}

EAPI int 
e_modapi_shutdown(E_Module *m) 
{
   if (echo_cfg->cfd) e_object_del(E_OBJECT(echo_cfg->cfd));

   if (incr_act) 
     {
        e_action_predef_name_del(D_("Echo"), D_("Increase Volume"));
        e_action_del("volume_increase");
        incr_act = NULL;
     }
   if (decr_act) 
     {
        e_action_predef_name_del(D_("Echo"), D_("Decrease Volume"));
        e_action_del("volume_decrease");
        decr_act = NULL;
     }
   if (mute_act) 
     {
        e_action_predef_name_del(D_("Echo"), D_("Mute Volume"));
        e_action_del("volume_mute");
        mute_act = NULL;
     }

   e_configure_registry_item_del("extensions/echo");
   e_configure_registry_category_del("extensions");

   e_mod_system_shutdown();
   e_gadcon_provider_unregister(&_gc_class);
   echo_mod = NULL;
   _echo_cfg_free();
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int 
e_modapi_save(E_Module *m) 
{
   return e_config_domain_save("module.echo", conf_edd, echo_cfg);
}

EAPI void 
_echo_cb_config_updated(void) 
{
   Instance *inst = NULL;

   if (!(inst = evas_list_data(instances))) return;

   inst->left_vol = inst->right_vol = -1;
   if (echo_sys->get_volume)
     echo_sys->get_volume(echo_cfg->channel.name, 
                          &(inst->left_vol), &(inst->right_vol));

   if (echo_sys->get_mute)
     echo_sys->get_mute(echo_cfg->channel.name, &(inst->mute));

   _echo_cb_update_inst(inst);
}

/* local functions */
static void 
_echo_cfg_free(void) 
{
   if (echo_cfg->channel.name) evas_stringshare_del(echo_cfg->channel.name);
   E_FREE(echo_cfg);
}

static void 
_echo_cfg_new(void) 
{
   echo_cfg = E_NEW(Config, 1);
   echo_cfg->version = (MOD_CFG_FILE_EPOCH << 16);

#define IFMODCFG(v) \
   if ((echo_cfg->version & 0xffff) < v) {
#define IFMODCFGEND }

   IFMODCFG(0x008d);
   echo_cfg->lock_sliders = 1;
   echo_cfg->card.id = 0;
   echo_cfg->channel.id = 0;
   echo_cfg->channel.name = NULL;
   if (e_mod_system_init(echo_cfg->card.id))
     {
	char *name = NULL;
	
        if (echo_sys->get_channel_name)
	  name = echo_sys->get_channel_name(echo_cfg->card.id);
	if (name)
	  {
	     echo_cfg->channel.name = evas_stringshare_add(name);
	     free(name);
	  }
        e_mod_system_shutdown();
     }
   IFMODCFGEND;

   echo_cfg->version = MOD_CFG_FILE_VERSION;

   e_modapi_save(echo_mod);
}

static int 
_echo_cfg_timer(void *data) 
{
   e_util_dialog_show(D_("Echo Configuration Updated"), data);
   return 0;
}

static void 
_echo_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Instance *inst = NULL;
   E_Zone *zone = NULL;
   E_Menu *mn = NULL;
   E_Menu_Item *mi = NULL;
   Evas_Event_Mouse_Down *ev;
   int x, y;

   if (!(inst = data)) return;
   ev = event;
   if ((ev->button == 3) && (!inst->menu)) 
     {
        zone = e_util_zone_current_get(e_manager_current_get());

        mn = e_menu_new();
        e_menu_post_deactivate_callback_set(mn, _echo_cb_menu_post, inst);
        inst->menu = mn;

        mi = e_menu_item_new(mn);
        e_menu_item_label_set(mi, D_("Configuration"));
        e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
        e_menu_item_callback_set(mi, _echo_cb_menu_cfg, inst);

        e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
        e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, NULL, NULL);
        e_menu_activate_mouse(mn, zone, x + ev->output.x, y + ev->output.y, 
                              1, 1, E_MENU_POP_DIRECTION_AUTO, ev->timestamp);
        evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button, 
                                 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
   else if (ev->button == 1) 
     {
        if (!inst->popup) _echo_cb_popup_new(inst);
        else _echo_cb_popup_del(inst);
     }
}

static void 
_echo_cb_menu_post(void *data, E_Menu *menu) 
{
   Instance *inst = NULL;

   if (!(inst = data)) return;
   if (!inst->menu) return;
   e_menu_post_deactivate_callback_set(inst->menu, NULL, NULL);
   e_object_del(E_OBJECT(inst->menu));
   inst->menu = NULL;
}

static void 
_echo_cb_menu_cfg(void *data, E_Menu *menu, E_Menu_Item *mi) 
{
   Instance *inst = NULL;
   E_Container *con = NULL;

   if (!(inst = data)) return;
   if (inst->popup) _echo_cb_popup_del(inst);

   con = e_container_current_get(e_manager_current_get());
   e_int_config_echo_module(con, NULL);
}

static void 
_echo_cb_popup_new(void *data) 
{
   Instance *inst = NULL;
   Evas *evas;
   Evas_Object *of = NULL, *ow = NULL, *os = NULL;

   if (!(inst = data)) return;
   if (echo_cfg->cfd) return;

   inst->left_vol = inst->right_vol = -1;
   if (echo_sys->get_volume)
     echo_sys->get_volume(echo_cfg->channel.name, 
                          &(inst->left_vol), &(inst->right_vol));

   if ((echo_sys->can_mute) && (echo_sys->can_mute(echo_cfg->channel.name)))
     {
        if (echo_sys->get_mute)
          echo_sys->get_mute(echo_cfg->channel.name, &(inst->mute));
     }

   inst->popup = e_gadcon_popup_new(inst->gcc, _echo_cb_popup_resize);
   evas = inst->popup->win->evas;

   of = e_widget_frametable_add(evas, (char *)echo_cfg->channel.name, 0);

   os = e_slider_add(evas);
   inst->o_lslider = os;
   evas_object_show(os);
   e_slider_orientation_set(os, 0);
   e_slider_value_set(os, inst->left_vol);
   e_slider_value_range_set(os, 0.0, 100.0);
   e_slider_value_format_display_set(os, "%3.0f");
   e_widget_frametable_object_append(of, os, 0, 0, 1, 5, 1, 1, 1, 1);
   evas_object_smart_callback_add(os, "changed", 
                                  _echo_cb_volume_left_change, inst);

   if (inst->right_vol >= 0) 
     {
        os = e_slider_add(evas);
        inst->o_rslider = os;
        evas_object_show(os);
        e_slider_orientation_set(os, 0);
        e_slider_value_set(os, inst->right_vol);
        e_slider_value_range_set(os, 0.0, 100.0);
        e_slider_value_format_display_set(os, "%3.0f");
        e_widget_frametable_object_append(of, os, 1, 0, 1, 5, 1, 1, 1, 1);
        evas_object_smart_callback_add(os, "changed", 
                                       _echo_cb_volume_right_change, inst);
     }

   if ((echo_sys->can_mute) && (echo_sys->can_mute(echo_cfg->channel.name))) 
     {
        ow = e_widget_check_add(evas, D_("Mute"), &(inst->mute));
        evas_object_show(ow);
        e_widget_frametable_object_append(of, ow, 0, 6, 2, 1, 1, 1, 1, 0);
        evas_object_smart_callback_add(ow, "changed", 
                                       _echo_cb_mute_change, inst);
     }
   e_gadcon_popup_content_set(inst->popup, of);
   e_gadcon_popup_show(inst->popup);
}

static void 
_echo_cb_popup_del(void *data) 
{
   Instance *inst = NULL;

   if (!(inst = data)) return;
   if (!inst->popup) return;
   e_object_del(E_OBJECT(inst->popup));
   inst->popup = NULL;
}

static void 
_echo_cb_popup_resize(Evas_Object *obj, int *w, int *h) 
{
   int mw, mh;

   e_widget_min_size_get(obj, &mw, &mh);
   if (mh < 150) mh = 150;
   if (mw < 60) mw = 60;
   if (*w) *w = (mw + 8);
   if (*h) *h = (mh + 8);
}

static void 
_echo_cb_volume_left_change(void *data, Evas_Object *obj, void *event) 
{
   Instance *inst = NULL;
   double val;

   if (!(inst = data)) return;
   val = e_slider_value_get(obj);
   inst->left_vol = (int)val;
   if (echo_cfg->lock_sliders) 
     {
        inst->right_vol = (int)val;
        e_slider_value_set(inst->o_rslider, val);
     }
   if (echo_sys->set_volume)
     echo_sys->set_volume(echo_cfg->channel.name, 
                          inst->left_vol, inst->right_vol);

   _echo_cb_update_inst(inst);
}

static void 
_echo_cb_volume_right_change(void *data, Evas_Object *obj, void *event) 
{
   Instance *inst = NULL;
   double val;

   if (!(inst = data)) return;
   val = e_slider_value_get(obj);
   inst->right_vol = (int)val;
   if (echo_cfg->lock_sliders) 
     {
        inst->left_vol = (int)val;
        e_slider_value_set(inst->o_lslider, val);
     }
   if (echo_sys->set_volume)
     echo_sys->set_volume(echo_cfg->channel.name, 
                          inst->left_vol, inst->right_vol);

   _echo_cb_update_inst(inst);
}

static void 
_echo_cb_mute_change(void *data, Evas_Object *obj, void *event) 
{
   Instance *inst = NULL;
   int mute = 0;

   if (!(inst = data)) return;
   mute = e_widget_check_checked_get(obj);
   if (echo_sys->set_mute)
     echo_sys->set_mute(echo_cfg->channel.name, mute);

   _echo_cb_update_inst(inst);
}

static void 
_echo_cb_update_inst(void *data) 
{
   Instance *inst = NULL;
   Edje_Message_Int *left_vol_msg, *right_vol_msg;

   if (!(inst = data)) return;
   left_vol_msg = malloc(sizeof(Edje_Message_Int));
   right_vol_msg = malloc(sizeof(Edje_Message_Int));

   left_vol_msg->val = inst->left_vol;
   right_vol_msg->val = inst->left_vol;

   if (inst->right_vol == -1) 
     {
        if (inst->left_vol == 0)
          edje_object_signal_emit(inst->o_base, "e,action,volume,none", "");
        else if ((inst->left_vol > 0) && (inst->left_vol < 33))
          edje_object_signal_emit(inst->o_base, "e,action,volume,low", "");
        else if ((inst->left_vol >= 33) && (inst->left_vol < 66))
          edje_object_signal_emit(inst->o_base, "e,action,volume,medium", "");
        else if (inst->left_vol >= 66)
          edje_object_signal_emit(inst->o_base, "e,action,volume,high", "");
     }
   else 
     {
        if (inst->left_vol == 0)
          edje_object_signal_emit(inst->o_base, "e,action,volume,left,none", "");
        else if ((inst->left_vol > 0) && (inst->left_vol < 33))
          edje_object_signal_emit(inst->o_base, "e,action,volume,left,low", "");
        else if ((inst->left_vol >= 33) && (inst->left_vol < 66))
          edje_object_signal_emit(inst->o_base, "e,action,volume,left,medium", "");
        else if (inst->left_vol >= 66)
          edje_object_signal_emit(inst->o_base, "e,action,volume,left,high", "");

        if (inst->right_vol == 0)
          edje_object_signal_emit(inst->o_base, "e,action,volume,right,none", "");
        else if ((inst->right_vol > 0) && (inst->right_vol < 33))
          edje_object_signal_emit(inst->o_base, "e,action,volume,right,low", "");
        else if ((inst->right_vol >= 33) && (inst->right_vol < 66))
          edje_object_signal_emit(inst->o_base, "e,action,volume,right,medium", "");
        else if (inst->right_vol >= 66)
          edje_object_signal_emit(inst->o_base, "e,action,volume,right,high", "");

	right_vol_msg->val = inst->right_vol;
     }
   
   if ((echo_sys->can_mute) && (echo_sys->can_mute(echo_cfg->channel.name))) 
     {
        if (echo_sys->get_mute)
	  echo_sys->get_mute(echo_cfg->channel.name, &(inst->mute));
        if (inst->mute)
          edje_object_signal_emit(inst->o_base, "e,action,volume,mute", "");
        else
          edje_object_signal_emit(inst->o_base, "e,action,volume,unmute", "");
     }
   else
     edje_object_signal_emit(inst->o_base, "e,action,volume,unmute", "");

   edje_object_message_send(inst->o_base, EDJE_MESSAGE_INT, 0, left_vol_msg);
   edje_object_message_send(inst->o_base, EDJE_MESSAGE_INT, 1, right_vol_msg);
   edje_object_signal_emit(inst->o_base, "e,action,volume,change", "");
}

static void
_echo_cb_signal_volume_left_up(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Instance *inst;

   if (!(inst = data)) return;
   inst->left_vol = inst->right_vol = -1;
   if (echo_sys->get_volume)
     echo_sys->get_volume(echo_cfg->channel.name, 
		          &(inst->left_vol), &(inst->right_vol));
   if (echo_sys->set_volume)
     echo_sys->set_volume(echo_cfg->channel.name, inst->left_vol + 5, 
                          inst->right_vol);

   _echo_cb_update_inst(inst);
}

static void
_echo_cb_signal_volume_left_down(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Instance *inst;

   if (!(inst = data)) return;
   inst->left_vol = inst->right_vol = -1;
   if (echo_sys->get_volume)
     echo_sys->get_volume(echo_cfg->channel.name, 
		          &(inst->left_vol), &(inst->right_vol));
   if (echo_sys->set_volume)
     echo_sys->set_volume(echo_cfg->channel.name, inst->left_vol - 5, 
                          inst->right_vol);

   _echo_cb_update_inst(inst);
}

static void
_echo_cb_signal_volume_right_up(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Instance *inst;

   if (!(inst = data)) return;
   inst->left_vol = inst->right_vol = -1;
   if (echo_sys->get_volume)
     echo_sys->get_volume(echo_cfg->channel.name, 
		          &(inst->left_vol), &(inst->right_vol));
   if (echo_sys->set_volume)
     echo_sys->set_volume(echo_cfg->channel.name, inst->left_vol, 
                          inst->right_vol + 5);

   _echo_cb_update_inst(inst);
}

static void
_echo_cb_signal_volume_right_down(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Instance *inst;

   if (!(inst = data)) return;
   inst->left_vol = inst->right_vol = -1;
   if (echo_sys->get_volume)
     echo_sys->get_volume(echo_cfg->channel.name, 
		          &(inst->left_vol), &(inst->right_vol));
   if (echo_sys->set_volume)
     echo_sys->set_volume(echo_cfg->channel.name, inst->left_vol, 
                          inst->right_vol - 5);

   _echo_cb_update_inst(inst);
}

static void
_echo_cb_signal_volume_mute_toggle(void *data, Evas_Object *obj, const char *emission, const char *source)
{
   Instance *inst;

   if (!(inst = data)) return;
   if ((echo_sys->can_mute) && (echo_sys->can_mute(echo_cfg->channel.name)))
     {
	if (echo_sys->set_mute)
	  echo_sys->set_mute(echo_cfg->channel.name, !(inst->mute));
     }

   _echo_cb_update_inst(inst);
}

static void 
_echo_cb_volume_increase(E_Object *obj, const char *params) 
{
   Instance *inst = NULL;

   if (!(inst = evas_list_data(instances))) return;
   inst->left_vol = inst->right_vol = -1;
   if (echo_sys->get_volume)
     echo_sys->get_volume(echo_cfg->channel.name, 
		          &(inst->left_vol), &(inst->right_vol));
   if (echo_sys->set_volume) 
     {
        if (inst->right_vol != -1)
          echo_sys->set_volume(echo_cfg->channel.name, inst->left_vol + 5, 
                               inst->right_vol + 5);
        else
          echo_sys->set_volume(echo_cfg->channel.name, inst->left_vol + 5, 
                               inst->right_vol);
     }

   _echo_cb_update_inst(inst);
}

static void 
_echo_cb_volume_decrease(E_Object *obj, const char *params) 
{
   Instance *inst = NULL;

   if (!(inst = evas_list_data(instances))) return;
   inst->left_vol = inst->right_vol = -1;
   if (echo_sys->get_volume)
     echo_sys->get_volume(echo_cfg->channel.name, 
		          &(inst->left_vol), &(inst->right_vol));
   if (echo_sys->set_volume) 
     {
        if (inst->right_vol != -1)
          echo_sys->set_volume(echo_cfg->channel.name, inst->left_vol - 1, 
                               inst->right_vol - 1);
        else
          echo_sys->set_volume(echo_cfg->channel.name, inst->left_vol - 1, 
                               inst->right_vol);
     }

   _echo_cb_update_inst(inst);
}

static void 
_echo_cb_volume_mute(E_Object *obj, const char *params) 
{
   Instance *inst = NULL;

   if (!(inst = evas_list_data(instances))) return;
   if ((echo_sys->can_mute) && (echo_sys->can_mute(echo_cfg->channel.name)))
     {
	if (echo_sys->set_mute)
	  echo_sys->set_mute(echo_cfg->channel.name, !(inst->mute));
     }

   _echo_cb_update_inst(inst);
}
