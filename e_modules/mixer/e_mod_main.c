/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"
#include "e_mod_types.h"
#include "e_mod_keybindings.h"

#if defined(HAVE_ALSA)
# include "alsa_mixer.h"
#elif defined(HAVE_OSS)
# include "oss_mixer.h"
#endif

/* Define to 1 for testing alsa code */
#define DEBUG 0
#define SLIDE_LENGTH 0.5
#define SLIDE_FRAMERATE (1.0 / 75.0)
#define VOL_STEP 4

/* Gadcon Protos */
static E_Gadcon_Client *_gc_init     (E_Gadcon * gc, const char *name, const char *id, const char *style);
static void             _gc_shutdown (E_Gadcon_Client * gcc);
static void             _gc_orient   (E_Gadcon_Client * gcc);
static char            *_gc_label    (void);
static Evas_Object     *_gc_icon     (Evas * evas);
static const char      *_gc_id_new   (void);

/* Module Protos */
static void         _mixer_simple_volume_change (Mixer *mixer, Config_Item *ci, double val);
static void         _mixer_volume_change        (Mixer *mixer, Config_Item *ci, double val);
static void         _mixer_simple_mute_toggle   (Mixer *mixer, Config_Item *ci);
static void         _mixer_mute_toggle          (Mixer *mixer, Config_Item *ci);

static Config_Item *_mixer_config_item_get   (void *data, const char *id);
static void         _mixer_menu_cb_post      (void *data, E_Menu *m);
static void         _mixer_menu_cb_configure (void *data, E_Menu *m, E_Menu_Item *mi);
static void         _mixer_cb_mouse_down     (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void         _mixer_cb_mouse_wheel    (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void         _mixer_system_init       (void *data);
static void         _mixer_system_shutdown   (void *data);

static void _mixer_window_simple_pop_up        (Instance *inst);
static void _mixer_window_simple_pop_down      (Instance *inst);
static int  _mixer_window_simple_timer_up_cb   (void *data);
static int  _mixer_window_simple_timer_down_cb (void *data);
static void _mixer_window_simple_changed_cb    (void *data, Evas_Object *obj, void *event_info);
static void _mixer_window_simple_mute_cb       (void *data, Evas_Object *obj, void *event_info);

static int  _mixer_window_simple_mouse_move_cb  (void *data, int type, void *event);
static int  _mixer_window_simple_mouse_down_cb  (void *data, int type, void *event);
static int  _mixer_window_simple_mouse_up_cb    (void *data, int type, void *event);
static int  _mixer_window_simple_mouse_wheel_cb (void *data, int type, void *event);

static void _mixer_base_send_vol(Mixer *mixer, Config_Item *ci);
static void _mixer_window_gauge_send_vol   (Mixer_Win_Gauge *win, Mixer *mixer, Config_Item *ci);
static void _mixer_window_gauge_free       (Mixer_Win_Gauge *win);
static int  _mixer_window_gauge_visible_cb (void *data);
static void _mixer_window_gauge_pop_down   (Mixer_Win_Gauge *win);
static void _mixer_window_gauge_pop_up     (Instance *inst);
static void _mixer_volume_increase         (Mixer *mixer, Config_Item *ci);
static void _mixer_volume_decrease         (Mixer *mixer, Config_Item *ci);

/* Private vars */
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *mixer_config = NULL;

static const E_Gadcon_Client_Class _gc_class =
{
   GADCON_CLIENT_CLASS_VERSION, "mixer",
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL},
   E_GADCON_CLIENT_STYLE_PLAIN
};

/* Implementation */
static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Instance *inst;
   Mixer *mixer;
   E_Gadcon_Client *gcc;
   char buf[4096];

   inst = E_NEW(Instance, 1);
   if (!inst) return NULL;

   mixer = E_NEW(Mixer, 1);
   if (!mixer) return NULL;
   mixer->inst = inst;
   mixer->evas = gc->evas;
   inst->mixer = mixer;

   snprintf(buf, sizeof(buf), "%s/mixer.edj", 
	    e_module_dir_get(mixer_config->module));

   mixer->base = edje_object_add(gc->evas);
   if (!e_theme_edje_object_set(mixer->base, "base/theme/modules/mixer", 
				"e/modules/mixer/main"))
     edje_object_file_set(mixer->base, buf, "e/modules/mixer/main");
   evas_object_show(mixer->base);
   edje_object_signal_emit(mixer->base, "low", "");

   _mixer_system_init(mixer);

   /* Defer this until after the mixer system has been setup */
   inst->ci = _mixer_config_item_get(mixer, id);
   if ((mixer->mix_sys->get_volume) && (inst->ci->channel_id != 0))
     {
	int ret;
	
	ret = mixer->mix_sys->get_volume(inst->ci->card_id, inst->ci->channel_id);
	if (ret < 33)
	  edje_object_signal_emit(mixer->base, "low", "");
	else if ((ret >= 33) && (ret < 66))
	  edje_object_signal_emit(mixer->base, "medium", "");
	else if (ret >= 66)
	  edje_object_signal_emit(mixer->base, "high", ""); 
     }

   if ((mixer->mix_sys->get_mute) && (inst->ci->channel_id != 0))
     {
        int m;

	m = mixer->mix_sys->get_mute(inst->ci->card_id, inst->ci->channel_id);
	if (m) 
	  edje_object_signal_emit(mixer->base, "muted", "");
	else
	  edje_object_signal_emit(mixer->base, "unmuted", "");
     }

   _mixer_base_send_vol(mixer, inst->ci);

   gcc = e_gadcon_client_new(gc, name, id, style, mixer->base);
   gcc->data = inst;
   inst->gcc = gcc;

   evas_object_event_callback_add(mixer->base, EVAS_CALLBACK_MOUSE_DOWN, 
				  _mixer_cb_mouse_down, inst);
   evas_object_event_callback_add(mixer->base, EVAS_CALLBACK_MOUSE_WHEEL, 
				  _mixer_cb_mouse_wheel, inst->mixer);
   evas_object_propagate_events_set(mixer->base, 0);
   
   mixer_config->instances = evas_list_append(mixer_config->instances, inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;
   Mixer *mixer;

   inst = gcc->data;
   if (!inst) return;
   mixer = inst->mixer;
   if (!mixer) return;

   if (mixer->mix_sys) _mixer_system_shutdown(mixer->mix_sys);
   if (mixer->base) evas_object_del(mixer->base);
   if (mixer->gauge_win) _mixer_window_gauge_free(mixer->gauge_win);

   mixer_config->instances = evas_list_remove(mixer_config->instances, inst);
   E_FREE(mixer);
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
   return D_("Mixer");
}

static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/e-module-mixer.edj",
	     e_module_dir_get(mixer_config->module));

   o = edje_object_add(evas);
   edje_object_file_set(o, buf, "icon");
   return o;
}

static const char *
_gc_id_new(void)
{
   int num = 0;
   char buf[128];
   Config_Item *ci;

   /* Create id */
   if (mixer_config->items)
     {
	const char *p;

	ci = evas_list_last(mixer_config->items)->data;
	p = strrchr(ci->id, '.');
	if (p) num = atoi(p + 1) + 1;
     }
   snprintf(buf, sizeof(buf), "%s.%d", _gc_class.name, num);
   return strdup(buf);
}

void
mixer_vol_increase(Instance *inst)
{
   Mixer_Win_Gauge *win;

   _mixer_window_gauge_pop_up(inst);

   if ((!inst) || (!inst->mixer)) return;

   win = inst->mixer->gauge_win;
   _mixer_volume_increase(inst->mixer, inst->ci);
   _mixer_window_gauge_send_vol(win, inst->mixer, inst->ci);
   _mixer_base_send_vol(inst->mixer, inst->ci);
   if (win) edje_object_signal_emit(win->pulsar, "vol,increase", "e");
}

void
mixer_vol_decrease(Instance *inst)
{
   Mixer_Win_Gauge *win;

   _mixer_window_gauge_pop_up(inst);

   if ((!inst) || (!inst->mixer)) return;

   win = inst->mixer->gauge_win;
   _mixer_volume_decrease(inst->mixer, inst->ci);
   _mixer_window_gauge_send_vol(win, inst->mixer, inst->ci);
   _mixer_base_send_vol(inst->mixer, inst->ci);
   if (win) edje_object_signal_emit(win->pulsar, "vol,decrease", "e");
}

void
mixer_mute_toggle(Instance *inst)
{
   if ((!inst) || (!inst->mixer)) return;
   _mixer_simple_mute_toggle(inst->mixer, inst->ci);
}

static void
_mixer_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;
   E_Menu *mn;
   E_Menu_Item *mi;
   E_Zone *zone;
   int x, y, w, h;

   if (!(inst = data)) return;

   ev = event_info;
   if ((ev->button == 3) && (!mixer_config->menu))
     {
	zone = e_util_zone_current_get(e_manager_current_get());

	mn = e_menu_new();
	e_menu_post_deactivate_callback_set(mn, _mixer_menu_cb_post, inst);
	mixer_config->menu = mn;

	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, D_("Configuration"));
	e_util_menu_item_edje_icon_set(mi, "enlightenment/configuration");
	e_menu_item_callback_set(mi, _mixer_menu_cb_configure, inst);

	mi = e_menu_item_new(mn);
	e_menu_item_separator_set(mi, 1);

	e_gadcon_client_util_menu_items_append(inst->gcc, mn, 0);
	e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &x, &y, &w, &h);
	e_menu_activate_mouse(mn,zone, x + ev->output.x, y + ev->output.y, 
			      1, 1, E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
				 EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
   else if (ev->button == 2)
     {
	if ((inst->ci->use_app) && (inst->ci->app != NULL))
	  ecore_exe_run(inst->ci->app, NULL);
	else
	  _mixer_window_simple_pop_up(inst);	
     }
   else if (ev->button == 1) 
     _mixer_window_simple_pop_up(inst);
}

static void 
_mixer_cb_mouse_wheel(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Evas_Event_Mouse_Wheel *ev;
   Mixer *mixer;
   int vol, dir = 0;
   int step = 4;
   double val;

   ev = event_info;
   if (!(mixer = data)) return;
   if (!mixer) return;
   if (!mixer->mix_sys) return;
   if (!mixer->mix_sys->get_volume) return;

   dir = -ev->z;
   vol = mixer->mix_sys->get_volume(mixer->inst->ci->card_id, mixer->inst->ci->channel_id);
   val = ((double)vol + dir * step);
   _mixer_simple_volume_change(mixer, mixer->inst->ci, val);
}

static void
_mixer_menu_cb_post(void *data, E_Menu *m)
{
   if (!mixer_config->menu) return;
   e_object_del(E_OBJECT(mixer_config->menu));
   mixer_config->menu = NULL;
}

static void
_mixer_menu_cb_configure(void *data, E_Menu *m, E_Menu_Item *mi) 
{
   Instance *inst;

   if (!(inst = data)) return;
   _config_mixer_module(inst->mixer, inst->ci);
}

static Config_Item *
_mixer_config_item_get(void *data, const char *id)
{
   Mixer *mixer;
   Mixer_Card *card;
   Mixer_Channel *chan;
   Evas_List *l = NULL;
   Config_Item *ci;

   mixer = data;
   for (l = mixer_config->items; l; l = l->next)
     {
	ci = l->data;
	if (!ci->id) continue;
	if (!strcmp(ci->id, id)) return ci;
     }

   ci = E_NEW(Config_Item, 1);
   ci->id = evas_stringshare_add(id);
   if ((!mixer) || (!mixer->mix_sys)) 
     {
	ci->card_id = 0;
	ci->channel_id = 0;
	ci->mode = SIMPLE_MODE;
	ci->app = evas_stringshare_add("");
     }
   else if (mixer->mix_sys)
     {   
	if (!mixer->mix_sys->cards) 
	  {
	     if (mixer->mix_sys->get_cards)
	       mixer->mix_sys->cards = mixer->mix_sys->get_cards();
	     else 
	       {
		  ci->card_id = 0;
		  ci->channel_id = 0;
		  ci->mode = SIMPLE_MODE;
		  ci->app = evas_stringshare_add("");
		  mixer_config->items = evas_list_append(mixer_config->items, ci);
		  return ci;
	       }
	  }

	card = evas_list_nth(mixer->mix_sys->cards, 0);
	if (!card) 
	  {
	     ci->card_id = 0;
	     ci->channel_id = 0;
	     ci->mode = SIMPLE_MODE;
	     ci->app = evas_stringshare_add("");
	     mixer_config->items = evas_list_append(mixer_config->items, ci);
	     return ci;
	  }
	else 
	  {
	     ci->card_id = card->id;
	     if (!card->channels) 
	       {
		  if (mixer->mix_sys->get_channels) 
		    card->channels = mixer->mix_sys->get_channels(card);
	       }
	     if (card->channels) 
	       {
		  chan = evas_list_nth(card->channels, 0);
		  ci->channel_id = chan->id;
	       }
	     else 
	       ci->channel_id = 0;
	  }
     }

   mixer_config->items = evas_list_append(mixer_config->items, ci);
   return ci;
}

static void 
_mixer_system_init(void *data) 
{
   Mixer *mixer;
   Mixer_System *sys;

   if (!(mixer = data)) return;

   sys = E_NEW(Mixer_System, 1);
   if (!sys) return;
   mixer->mix_sys = sys;

#if defined(HAVE_ALSA)
   sys->get_cards = alsa_get_cards;
   sys->get_card = alsa_get_card;
   sys->get_channels = alsa_card_get_channels;
   sys->get_channel = alsa_card_get_channel;
   sys->free_cards = alsa_free_cards;
   sys->get_volume = alsa_get_volume;
   sys->set_volume = alsa_set_volume;
   sys->get_mute = alsa_get_mute;
   sys->set_mute = alsa_set_mute;
#elif defined(HAVE_OSS)
   sys->get_cards = oss_get_cards;
   sys->get_card = oss_get_card;
   sys->get_channels = oss_card_get_channels; 
   sys->get_channel = oss_card_get_channel;
   sys->free_cards = oss_free_cards;
   sys->get_volume = oss_get_volume;
   sys->set_volume = oss_set_volume;
   sys->get_mute = oss_get_mute;
   sys->set_mute = oss_set_mute;   
#endif
}

static void 
_mixer_system_shutdown(void *data) 
{
   Mixer_System *sys;

   if (!(sys = data)) return;
   if (sys->free_cards) sys->free_cards(sys->cards);
   E_FREE(sys);
}

EAPI E_Module_Api e_modapi =
{
   E_MODULE_API_VERSION, "Mixer"
};

EAPI void *
e_modapi_init(E_Module *m)
{
   char buf[4096];

   snprintf(buf, sizeof(buf), "%s/locale", e_module_dir_get(m));
   bindtextdomain(PACKAGE, buf);
   bind_textdomain_codeset(PACKAGE, "UTF-8");

   conf_item_edd = E_CONFIG_DD_NEW("Mixer_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, card_id, INT);
   E_CONFIG_VAL(D, T, channel_id, INT);
   E_CONFIG_VAL(D, T, mode, INT);
   E_CONFIG_VAL(D, T, app, STR);
   E_CONFIG_VAL(D, T, use_app, INT);
   E_CONFIG_VAL(D, T, show_popup, INT);
   E_CONFIG_VAL(D, T, popup_speed, DOUBLE);

   conf_edd = E_CONFIG_DD_NEW("Mixer_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_VAL(D, T, decrease_vol_key.context, INT);
   E_CONFIG_VAL(D, T, decrease_vol_key.modifiers, INT);
   E_CONFIG_VAL(D, T, decrease_vol_key.key, STR);
   E_CONFIG_VAL(D, T, decrease_vol_key.action, STR);
   E_CONFIG_VAL(D, T, decrease_vol_key.params, STR);
   E_CONFIG_VAL(D, T, decrease_vol_key.any_mod, UCHAR);
   E_CONFIG_VAL(D, T, increase_vol_key.context, INT);
   E_CONFIG_VAL(D, T, increase_vol_key.modifiers, INT);
   E_CONFIG_VAL(D, T, increase_vol_key.key, STR);
   E_CONFIG_VAL(D, T, increase_vol_key.action, STR);
   E_CONFIG_VAL(D, T, increase_vol_key.params, STR);
   E_CONFIG_VAL(D, T, increase_vol_key.any_mod, UCHAR);
   E_CONFIG_VAL(D, T, mute_key.context, INT);
   E_CONFIG_VAL(D, T, mute_key.modifiers, INT);
   E_CONFIG_VAL(D, T, mute_key.key, STR);
   E_CONFIG_VAL(D, T, mute_key.action, STR);
   E_CONFIG_VAL(D, T, mute_key.params, STR);
   E_CONFIG_VAL(D, T, mute_key.any_mod, UCHAR);
   E_CONFIG_LIST(D, T, items, conf_item_edd);

   mixer_config = e_config_domain_load("module.mixer", conf_edd);
   if (!mixer_config) 
     {
	Config_Item *ci;

	mixer_config = E_NEW(Config, 1);
	ci = E_NEW(Config_Item, 1);
	ci->id = evas_stringshare_add("0");
	ci->card_id = 0;
	ci->channel_id = 0;
	ci->mode = SIMPLE_MODE;
	ci->app = evas_stringshare_add("");
	ci->use_app = 0;
	ci->show_popup = 1;
	ci->popup_speed = 2.0;
	mixer_config->items = evas_list_append(mixer_config->items, ci);
     }
   mixer_register_module_actions();
   mixer_register_module_keybindings();

   mixer_config->module = m;

   e_gadcon_provider_register(&_gc_class);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
   mixer_unregister_module_actions();
   mixer_unregister_module_keybindings();

   mixer_config->module = NULL;
   e_gadcon_provider_unregister(&_gc_class);

   if (mixer_config->config_dialog)
     e_object_del(E_OBJECT(mixer_config->config_dialog));

   if (mixer_config->menu)
     {
	e_menu_post_deactivate_callback_set(mixer_config->menu, NULL, NULL);
	e_object_del(E_OBJECT(mixer_config->menu));
	mixer_config->menu = NULL;
     }

   while (mixer_config->items)
     {
	Config_Item *ci;

	ci = mixer_config->items->data;
	mixer_config->items = evas_list_remove_list(mixer_config->items, 
						    mixer_config->items);
	if (ci->id) evas_stringshare_del(ci->id);
	if (ci->app) evas_stringshare_del(ci->app);
	E_FREE(ci);
     }

   E_FREE(mixer_config);
   E_CONFIG_DD_FREE(conf_item_edd);
   E_CONFIG_DD_FREE(conf_edd);
   return 1;
}

EAPI int
e_modapi_save(E_Module *m)
{
   e_config_domain_save("module.mixer", conf_edd, mixer_config);
   return 1;
}

/* Changes the volume for the main channel*/
static void 
_mixer_simple_volume_change(Mixer *mixer, Config_Item *ci, double val)
{
   if (!mixer) return;
   if (!ci) return;
   _mixer_volume_change(mixer, ci, val);
}

/* Changes the volume of the given channel*/
static void 
_mixer_volume_change(Mixer *mixer, Config_Item *ci, double val)
{
   int m, ret;

   if (!mixer) return;
   if (!mixer->mix_sys) return;
   if (!mixer->mix_sys->set_volume) return;
   if (!ci) return;

   if (ci->channel_id != 0) 
     {
	ret = mixer->mix_sys->set_volume(ci->card_id, ci->channel_id, val);
	if (ret)
	  {
	     m = mixer->mix_sys->get_mute(ci->card_id, ci->channel_id);
	     if (m) 
	       {
		  edje_object_signal_emit(mixer->base, "muted", "");
	       }
	     else 
	       edje_object_signal_emit(mixer->base, "unmuted", "");

	     if (val < 33)
	       edje_object_signal_emit(mixer->base, "low", "");
	     else if ((val >= 33) && (val < 66))
	       edje_object_signal_emit(mixer->base, "medium", "");
	     else if (val >= 66)
	       edje_object_signal_emit(mixer->base, "high", ""); 

	  }
	_mixer_base_send_vol(mixer, ci);
     }
}

/* Mutes the main channel */
static void        
_mixer_simple_mute_toggle(Mixer *mixer, Config_Item *ci)
{
   if (!mixer) return;
   if (!ci) return;
   _mixer_mute_toggle(mixer, ci);
}

/* Mute the given channel */
static void         
_mixer_mute_toggle(Mixer *mixer, Config_Item *ci)
{
   int m;
   Mixer_Win_Simple *win;

   if (!ci) return;
   if (!mixer) return;
   if (!mixer->mix_sys) return;
   if (!mixer->mix_sys->get_mute) return;
   if (!mixer->mix_sys->set_mute) return;
   if (!mixer->mix_sys->get_volume) return;

   win = mixer->simple_win;
   m = mixer->mix_sys->get_mute(ci->card_id, ci->channel_id);
   m = m ? 0 : 1;
   mixer->mix_sys->set_mute(ci->card_id, ci->channel_id, m);
   if (m) 
     edje_object_signal_emit(mixer->base, "muted", "");
   else 
     edje_object_signal_emit(mixer->base, "unmuted", "");
   _mixer_base_send_vol(mixer, ci);
}

/* Makes the simple window containing the slider pop up */
static void
_mixer_window_simple_pop_up(Instance *inst)
{
   E_Container *con;
   Mixer_Win_Simple *win;
   Evas_Coord ox, oy, ow, oh;
   Evas_Coord mw, mh;
   int cx, cy, cw, ch;

   if ((!inst) || (!inst->mixer)) return;
   if (!(con = e_container_current_get(e_manager_current_get()))) return;

   evas_object_geometry_get(inst->mixer->base, &ox, &oy, &ow, &oh); 

   if (!(win = inst->mixer->simple_win))
     {
        win = E_NEW(Mixer_Win_Simple, 1);
        inst->mixer->simple_win = win;
        win->mixer = inst->mixer;
        win->window = e_popup_new(e_zone_current_get(con), 0, 0, 0, 0);

        win->bg_obj = edje_object_add(win->window->evas);
        e_theme_edje_object_set(win->bg_obj, "base/theme/menus",
				"e/widgets/menu/default/background");
	edje_object_part_text_set(win->bg_obj, "e.text.title", D_("Volume"));
	edje_object_signal_emit(win->bg_obj, "e,action,show,title", "e");
        edje_object_message_signal_process(win->bg_obj);
        evas_object_show(win->bg_obj);

        win->vbox = e_box_add(win->window->evas);
        e_box_freeze(win->vbox);
        e_box_orientation_set(win->vbox, 0);

        win->slider = e_slider_add(win->window->evas);
        e_slider_value_range_set(win->slider, 0.0, 100.0);
        e_slider_orientation_set(win->slider, 0);
        /* TODO: Fix this in e_slider... */
//        e_slider_direction_set(win->slider, 1);
        evas_object_show(win->slider);
        e_slider_min_size_get(win->slider, &mw, &mh);
        e_box_pack_start(win->vbox, win->slider);
        e_box_pack_options_set(win->slider, 1, 1, 1, 1, 0.5, 0.5,
                               mw, mh, 9999, 9999);
        evas_object_smart_callback_add(win->slider, "changed",
                                       _mixer_window_simple_changed_cb, win);

        win->check = e_widget_check_add(win->window->evas, D_("Mute"), &win->mute);
        evas_object_show(win->check);
        e_widget_min_size_get(win->check, &mw, &mh);
        e_box_pack_end(win->vbox, win->check);
        e_box_pack_options_set(win->check, 0, 0, 0, 0, 0.5, 0.5, mw, mh, mw, mh);
        evas_object_smart_callback_add(win->check, "changed",
                                       _mixer_window_simple_mute_cb, win);

        e_box_min_size_get(win->vbox, &mw, &mh);
        if (mw < ow)  mw = ow;
        if (mh < 150) mh = 150;
        edje_extern_object_min_size_set(win->vbox, mw, mh);
        edje_object_part_swallow(win->bg_obj, "e.swallow.content", win->vbox);
        edje_object_size_min_calc(win->bg_obj, &win->w, &win->h);
        evas_object_move(win->bg_obj, 0, 0);
        evas_object_resize(win->bg_obj, win->w, win->h);
     }
   
   if (win->input_window == 0)
     {
        Ecore_X_Window root, parent;
        int root_x, root_y, root_w, root_h;

        root = win->window->evas_win;
        while ((parent = ecore_x_window_parent_get(root)) != 0)
	  root = parent;

        ecore_x_window_geometry_get(root, &root_x, &root_y, &root_w, &root_h);
        win->input_window = ecore_x_window_input_new(root, root_x, root_y, root_w, root_h);
        ecore_x_window_show(win->input_window);
        /* TODO: Fixme... */
        //ecore_x_pointer_confine_grab(win->input_window);
        ecore_x_keyboard_grab(win->input_window);

        win->mouse_move_handler = 
	  ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE,
				  _mixer_window_simple_mouse_move_cb, win);
        win->mouse_down_handler = 
	  ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_DOWN,
				  _mixer_window_simple_mouse_down_cb, win);
        win->mouse_up_handler = 
	  ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP,
				  _mixer_window_simple_mouse_up_cb, win);
        win->mouse_wheel_handler = 
	  ecore_event_handler_add(ECORE_X_EVENT_MOUSE_WHEEL,
				  _mixer_window_simple_mouse_wheel_cb, win);

        win->first_mouse_up = 1;

        evas_event_feed_mouse_move(win->window->evas,
                                   -100000, -100000, ecore_time_get(), NULL);
        evas_event_feed_mouse_in(win->window->evas,
                                 ecore_time_get(), NULL);
     }
   
   if (inst->mixer->mix_sys->get_volume) 
     {
        int vol;

	if (inst->ci->channel_id != 0) 
	  {
	     edje_object_signal_emit(e_slider_edje_object_get(win->slider), 
				     "e,state,enabled", "e");
	     vol = inst->mixer->mix_sys->get_volume(inst->ci->card_id, inst->ci->channel_id);
	     e_slider_value_set(win->slider, vol);
	     if (vol < 33)
	       edje_object_signal_emit(inst->mixer->base, "low", "");
	     else if ((vol >= 33) && (vol < 66))
	       edje_object_signal_emit(inst->mixer->base, "medium", "");
	     else if (vol >= 66)
	       edje_object_signal_emit(inst->mixer->base, "high", "");

	     if (inst->mixer->mix_sys->get_mute) 
	       {
		  int m;

		  m = inst->mixer->mix_sys->get_mute(inst->ci->card_id, inst->ci->channel_id);
		  e_widget_check_checked_set(win->check, m);
		  if (m) 
		    edje_object_signal_emit(inst->mixer->base, "muted", ""); 
	       }
	        _mixer_base_send_vol(inst->mixer, inst->ci);
	  }
     }
   
   e_gadcon_canvas_zone_geometry_get(inst->gcc->gadcon, &cx, &cy, &cw, &ch);
   win->x = cx + ox;
   win->y = cy + oy;

   if ((win->y - con->y) < (con->h / 2))
     win->to_top = 0;
   else
     win->to_top = 1;

   switch (inst->gcc->gadcon->orient)
     {
      case E_GADCON_ORIENT_LEFT:
      case E_GADCON_ORIENT_CORNER_LT:
      case E_GADCON_ORIENT_CORNER_LB:
	win->x += ox + ow;
	if (win->to_top) win->y += oh;
	break;
      case E_GADCON_ORIENT_RIGHT:
      case E_GADCON_ORIENT_CORNER_RT:
      case E_GADCON_ORIENT_CORNER_RB:
	win->x -= (win->w - ((cx + cw) - (ox + ow)) + 5);
	if (win->to_top) win->y += oh;
	break;
      case E_GADCON_ORIENT_TOP:
      case E_GADCON_ORIENT_CORNER_TL:
      case E_GADCON_ORIENT_CORNER_TR:
	win->x += ((ow - win->w) / 2);
	if (win->x < cx) win->x = cx;
	if ((win->x + win->w) > (cx + cw)) win->x = cx + cw - win->w;
	if (!win->to_top) win->y += oh;
	break;
      default:
	win->x += ((ow - win->w) / 2);
	if (win->x < cx) win->x = cx;
	if ((win->x + win->w) > (cx + cw)) win->x = cx + cw - win->w;
	if (!win->to_top) win->y += (cy + ch);
	break;
     }

   e_popup_move_resize(win->window, win->x, win->y, win->w, 0);
   e_popup_show(win->window);

   win->start_time = ecore_time_get();
   if (win->slide_timer) ecore_timer_del(win->slide_timer);
   win->slide_timer = ecore_timer_add(SLIDE_FRAMERATE,
                                      _mixer_window_simple_timer_up_cb, win);
   win->popped_up = 1;
}

/* Makes the simple window containing the slider pop up */
static void
_mixer_window_simple_pop_down(Instance *inst)
{
   Mixer_Win_Simple *win;

   if (!(win = inst->mixer->simple_win) || (!win->popped_up)) return;

   if (win->input_window != 0)
     {
        /* TODO: Fixme... */
        //ecore_x_pointer_ungrab();
        ecore_x_keyboard_ungrab();
        ecore_x_window_del(win->input_window);
        ecore_event_handler_del(win->mouse_move_handler);
        ecore_event_handler_del(win->mouse_down_handler);
        ecore_event_handler_del(win->mouse_up_handler);
        ecore_event_handler_del(win->mouse_wheel_handler);
        win->input_window = 0;
        win->mouse_up_handler = NULL;
     }

   win->start_time = ecore_time_get();
   if (win->slide_timer) ecore_timer_del(win->slide_timer);
   win->slide_timer = ecore_timer_add(SLIDE_FRAMERATE,
                                      _mixer_window_simple_timer_down_cb, win);
   win->popped_up = 0;
}

/* Makes the window slide when it pops up */
static int 
_mixer_window_simple_timer_up_cb(void *data)
{
   Mixer_Win_Simple *win;
   double progress;
   int prev_h, h;

   if (!(win = data)) return 1;

   progress = (ecore_time_get() - win->start_time) / SLIDE_LENGTH;
   progress = E_CLAMP(progress, 0.0, 1.0);
   progress = 1.0 - (1.0 - progress) * (1.0 - progress);
   h = progress * win->h;
   prev_h = win->window->h;

   if (win->to_top)
     {
        e_popup_move_resize(win->window, win->x, win->y - h, win->w, h);
        evas_object_move(win->bg_obj, 0, 0);
     }
   else
     {
        e_popup_resize(win->window, win->w, h);
        evas_object_move(win->bg_obj, 0, h - win->h);
     }

   if (h >= win->h)
     {
        win->slide_timer = NULL;
        return 0;
     }
   else
     return 1;
}

/* Makes the simple window pops down in animation */
static int 
_mixer_window_simple_timer_down_cb(void *data)
{
   Mixer_Win_Simple *win;
   double progress;
   int prev_h, h;

   if (!(win = data)) return 1;

   progress = (ecore_time_get() - win->start_time) / SLIDE_LENGTH;
   progress = E_CLAMP(progress, 0.0, 1.0);
   h = (1.0 - progress) * (1.0 - progress) * win->h;
   prev_h = win->window->h;

   if (win->to_top)
     {
        e_popup_move_resize(win->window, win->x, win->y - h, win->w, h);
        evas_object_move(win->bg_obj, 0, 0);
     }
   else
     {
        e_popup_resize(win->window, win->w, h);
        evas_object_move(win->bg_obj, 0, h - win->h);
     }

   if (h <= 0)
     {
        e_object_del(E_OBJECT(win->window));
        win->mixer->simple_win = NULL;
        E_FREE(win);
        return 0;
     }
   else
     return 1;
}

/* Called when the value of the slider of the simple window is changed */
static void 
_mixer_window_simple_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Mixer_Win_Simple *win;
   Mixer *mixer;
   double val;

   if (!(win = data)) return;
   if (!(mixer = win->mixer)) return;
   if (!mixer->mix_sys) return;
   if (!mixer->mix_sys->set_volume) return;

   val = e_slider_value_get(obj);
   _mixer_simple_volume_change(mixer, mixer->inst->ci, val);
}

/* Called when the "mute" checkbox is toggled */
static void 
_mixer_window_simple_mute_cb(void *data, Evas_Object *obj, void *event_info)
{
   Mixer_Win_Simple *win;
   Mixer *mixer;

   if (!(win = data)) return;
   mixer = win->mixer;
   _mixer_simple_mute_toggle(mixer, mixer->inst->ci);
}

/* Called when the mouse moves over the input window */
static int
_mixer_window_simple_mouse_move_cb(void *data, int type, void *event)
{
   Mixer_Win_Simple *win;
   Ecore_X_Event_Mouse_Move *xev = event;

   if (!(win = data) || (win->slide_timer)) return 1;

   evas_event_feed_mouse_move(win->window->evas,
                              xev->x - win->window->x, xev->y - win->window->y,
                              xev->time, NULL);
   return 1;
}

/* Called when the input window is pressed by the mouse */
static int
_mixer_window_simple_mouse_down_cb(void *data, int type, void *event)
{
   Mixer_Win_Simple *win;
   Ecore_X_Event_Mouse_Button_Down *xev = event;

   if (!(win = data) || (win->slide_timer)) return 1;

   evas_event_feed_mouse_down(win->window->evas,
                              xev->button, EVAS_BUTTON_NONE,
                              xev->time, NULL);
   return 1;
}

/* Called when the input window is released by the mouse */
static int
_mixer_window_simple_mouse_up_cb(void *data, int type, void *event)
{
   Mixer_Win_Simple *win;
   Ecore_X_Event_Mouse_Button_Up *xev = event;
   int inside;

   if (!(win = data) || (win->slide_timer)) return 1;

   evas_event_feed_mouse_up(win->window->evas,
                            xev->button, EVAS_BUTTON_NONE,
                            xev->time, NULL);

   inside = E_INSIDE(xev->x, xev->y, win->window->x, win->window->y,
                     win->window->w, win->window->h);
   
   if ((xev->button == 1) && (!inside))
     _mixer_window_simple_pop_down(win->mixer->inst);

   if ((xev->button == 1) && (win->first_mouse_up))
     win->first_mouse_up = 0;
   
   return 1;
}

/* Called when the mouse wheel is used over the input window */
static int
_mixer_window_simple_mouse_wheel_cb(void *data, int type, void *event)
{
   Mixer_Win_Simple *win;
   Ecore_X_Event_Mouse_Wheel *xev = event;

   if (!(win = data) || (win->slide_timer)) return 1;

   evas_event_feed_mouse_wheel(win->window->evas,
			       xev->direction, xev->z, xev->time, NULL);
   return 1;
}

static void
_mixer_window_gauge_free(Mixer_Win_Gauge *win)
{
   if (!win) return;

   e_object_del(E_OBJECT(win->window));
   evas_object_del(win->bg_obj);
   evas_object_del(win->pulsar);
   win->mixer->gauge_win = NULL;
   if (win->timer) ecore_timer_del(win->timer);
   win->timer = NULL;
   E_FREE(win);
}

static int
_mixer_window_gauge_visible_cb(void *data)
{
   Mixer_Win_Gauge *win;

   win = data;
   _mixer_window_gauge_pop_down(win);
   if (win->timer) ecore_timer_del(win->timer);
   win->timer = NULL;
   return 0;
}

static void
_mixer_window_gauge_pop_down(Mixer_Win_Gauge *win)
{
   if (!win) return;
   e_popup_hide(win->window);
}

static void
_mixer_window_gauge_pop_up(Instance *inst)
{
   E_Container *con;
   Mixer_Win_Gauge *win;
   char buf[4096];

   if ((!inst) || (!inst->mixer) || (!inst->gcc)) return;
   if (!(con = e_container_current_get(e_manager_current_get()))) return;
   if (!(inst->ci->show_popup)) return;

   if (!(win = inst->mixer->gauge_win))
     {
	win = E_NEW(Mixer_Win_Gauge, 1);
	win->mixer = inst->mixer;
        inst->mixer->gauge_win = win;
	win->window = e_popup_new(e_zone_current_get(con), 0, 0, 0, 0);
        win->bg_obj = edje_object_add(win->window->evas);
	e_theme_edje_object_set(win->bg_obj, "base/theme/menus",
				"e/widgets/menu/default/background");
	edje_object_part_text_set(win->bg_obj, "e.text.title", D_("Volume"));
	edje_object_signal_emit(win->bg_obj, "e,action,show,title", "e");
        edje_object_message_signal_process(win->bg_obj);

	win->pulsar = edje_object_add(win->window->evas);
	snprintf(buf, sizeof(buf), "%s/mixer.edj", 
	      e_module_dir_get(mixer_config->module));
	edje_object_file_set(win->pulsar, buf, "e/modules/mixer/pulsar");
	evas_object_show(win->pulsar);
	evas_object_move(win->pulsar, -10, 0);
        evas_object_resize(win->pulsar, 300, 90);

	evas_object_show(win->bg_obj);
        evas_object_move(win->bg_obj, 0, 0);
        evas_object_resize(win->bg_obj, 300, 90);
	e_popup_resize(win->window, 300, 90);
     }
   else
     win = inst->mixer->gauge_win;

   e_popup_move(win->window, (con->w/2.0)-150, (con->h/2.0)-45);
   e_popup_show(win->window);

   if (win->timer) ecore_timer_del(win->timer);
   win->timer = ecore_timer_add(inst->ci->popup_speed, 
				_mixer_window_gauge_visible_cb, win);
}

static void
_mixer_window_gauge_send_vol(Mixer_Win_Gauge *win, Mixer *mixer, Config_Item *ci)
{
   Edje_Message_Int *msg;
   int vol;

   if ((!ci) || (!ci->show_popup) || (!win)) return;
   if ((!mixer) || (!mixer->mix_sys) || (!mixer->mix_sys->get_volume)) return;
   msg = malloc(sizeof(Edje_Message_Int));
   vol = (int)mixer->mix_sys->get_volume(ci->card_id, ci->channel_id);
   msg->val = vol;
   edje_object_message_send(win->pulsar, EDJE_MESSAGE_INT, 0, msg);
   free(msg);
}

static void
_mixer_base_send_vol(Mixer *mixer, Config_Item *ci)
{
   Edje_Message_Int *msg;
   int vol;

   if (!ci) return;
   if ((!mixer) || (!mixer->mix_sys) || (!mixer->mix_sys->get_volume)) return;
   msg = malloc(sizeof(Edje_Message_Int));
   vol = (int)mixer->mix_sys->get_volume(ci->card_id, ci->channel_id);
   msg->val = vol;
   edje_object_message_send(mixer->base, EDJE_MESSAGE_INT, 0, msg);
   free(msg);
   edje_object_signal_emit(mixer->base, "volume,change", "");
}

/* Increase the volume by VOL_STEP */
static void
_mixer_volume_increase(Mixer *mixer, Config_Item *ci)
{
   double vol;

   if (!mixer) return;
   if (!mixer->mix_sys) return;
   if (!mixer->mix_sys->get_volume) return;
   if (!mixer->mix_sys->set_volume) return;
   if (!ci) return;

   if (ci->channel_id != 0) 
     {
	vol = mixer->mix_sys->get_volume(ci->card_id, ci->channel_id);
	mixer->mix_sys->set_volume(ci->card_id, ci->channel_id, vol+VOL_STEP);
     }
}

/* Decrease the volume by VOL_STEP */
static void
_mixer_volume_decrease(Mixer *mixer, Config_Item *ci)
{
   double vol;

   if (!mixer) return;
   if (!mixer->mix_sys) return;
   if (!mixer->mix_sys->set_volume) return;
   if (!ci) return;

   if (ci->channel_id != 0) 
     {
	vol = mixer->mix_sys->get_volume(ci->card_id, ci->channel_id);
	vol = vol - VOL_STEP < 0 ? 0 : vol - VOL_STEP;
	mixer->mix_sys->set_volume(ci->card_id, ci->channel_id, vol);
     }
}
