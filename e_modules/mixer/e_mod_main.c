#include <e.h>
#include "e_mod_main.h"
#include "e_mod_types.h"

#ifdef HAVE_LIBASOUND
# include "alsa_mixer.h"
#endif

/* Define to 1 for testing alsa code */
#define DEBUG 0
#define SLIDE_LENGTH 0.5

/* Gadcon Protos */
static E_Gadcon_Client *_gc_init     (E_Gadcon * gc, const char *name, const char *id, const char *style);
static void             _gc_shutdown (E_Gadcon_Client * gcc);
static void             _gc_orient   (E_Gadcon_Client * gcc);
static char            *_gc_label    (void);
static Evas_Object     *_gc_icon     (Evas * evas);

/* Module Protos */
static Config_Item *_mixer_config_item_get   (void *data, const char *id);
static void         _mixer_menu_cb_post      (void *data, E_Menu *m);
static void         _mixer_menu_cb_configure (void *data, E_Menu *m, E_Menu_Item *mi);
static void         _mixer_cb_mouse_down     (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void         _mixer_menu_cb_configure (void *data, E_Menu *m, E_Menu_Item *mi);
static void         _mixer_system_init       (void *data);
static void         _mixer_system_shutdown   (void *data);

static void _mixer_window_simple_pop_up           (Instance *inst);
static void _mixer_window_simple_pop_down         (Instance *inst);
static int  _mixer_window_simple_animator_up_cb   (void *data);
static int  _mixer_window_simple_animator_down_cb (void *data);
static void _mixer_window_simple_changed_cb       (void *data, Evas_Object *obj, void *event_info);
static void _mixer_window_simple_mute_cb          (void *data, Evas_Object *obj, void *event_info);

static int  _mixer_window_simple_mouse_move_cb    (void *data, int type, void *event);
static int  _mixer_window_simple_mouse_down_cb    (void *data, int type, void *event);
static int  _mixer_window_simple_mouse_up_cb      (void *data, int type, void *event);
static int  _mixer_window_simple_mouse_wheel_cb   (void *data, int type, void *event);

/* Private vars */
static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *mixer_config = NULL;

static const E_Gadcon_Client_Class _gc_class =
{
   GADCON_CLIENT_CLASS_VERSION, "mixer",
     {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon},
   E_GADCON_CLIENT_STYLE_PLAIN
};

/* Implementation */
static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Instance        *inst;
   Config_Item     *ci;
   Mixer           *mixer;
   E_Gadcon_Client *gcc;
   char             buf[4096];
   
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
   edje_object_file_set(mixer->base, buf, "e/modules/mixer/main");
   evas_object_show(mixer->base);
   edje_object_signal_emit(mixer->base, "low", "");

   _mixer_system_init(mixer);

   /* Defer this until after the mixer system has been setup */
   ci = _mixer_config_item_get(mixer, id);
   if (!ci->id) ci->id = evas_stringshare_add(id);

   if ((mixer->mix_sys->get_volume) && (ci->card_id != 0) && (ci->channel_id != 0))
     {
	int ret;
	
	ret = mixer->mix_sys->get_volume(ci->card_id, ci->channel_id);
	if (ret < 33)
	  edje_object_signal_emit(mixer->base, "low", "");
	else if ((ret >= 34) && (ret < 66))
	  edje_object_signal_emit(mixer->base, "medium", "");
	else if (ret > 66)
	  edje_object_signal_emit(mixer->base, "high", ""); 
	else if (ret <= 0) 
	  edje_object_signal_emit(mixer->base, "muted", ""); 
     }

   if ((mixer->mix_sys->get_mute) && (ci->card_id != 0) && (ci->channel_id != 0))
     {
        int m;

	m = mixer->mix_sys->get_mute(ci->card_id, ci->channel_id);
	if (m) 
	  edje_object_signal_emit(mixer->base, "muted", "");
     }
   
   gcc = e_gadcon_client_new(gc, name, id, style, mixer->base);
   gcc->data = inst;
   inst->gcc = gcc;

   evas_object_event_callback_add(mixer->base, EVAS_CALLBACK_MOUSE_DOWN, 
				  _mixer_cb_mouse_down, inst);
   
   mixer_config->instances = evas_list_append(mixer_config->instances, inst);
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;
   Mixer    *mixer;
   
   inst = gcc->data;
   if (!inst) return;
   mixer = inst->mixer;
   if (!mixer) return;

   if (mixer->mix_sys) _mixer_system_shutdown(mixer->mix_sys);
   if (mixer->base) evas_object_del(mixer->base);
   
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
   return _("Mixer");
}

static Evas_Object *
_gc_icon(Evas *evas)
{
   Evas_Object *o;
   char         buf[4096];

   snprintf(buf, sizeof(buf), "%s/module.eap",
	     e_module_dir_get(mixer_config->module));

   o = edje_object_add(evas);
   edje_object_file_set(o, buf, "icon");
   return o;
}

static void
_mixer_cb_mouse_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Instance              *inst;
   Evas_Event_Mouse_Down *ev;

   inst = data;
   if (!inst) return;
   
   ev = event_info;
   if ((ev->button == 3) && (!mixer_config->menu))
     {
	E_Menu      *mn;
	E_Menu_Item *mi;
	E_Zone      *zone;
	int          x, y, w, h;

	zone = e_util_zone_current_get(e_manager_current_get());
	
	mn = e_menu_new();
	e_menu_post_deactivate_callback_set(mn, _mixer_menu_cb_post, inst);
	mixer_config->menu = mn;

	mi = e_menu_item_new(mn);
	e_menu_item_label_set(mi, _("Configuration"));
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
   else if (ev->button == 1) 
     {
	if (ev->flags == EVAS_BUTTON_DOUBLE_CLICK) 
	  {
	     /* Call full mixer window. 
	      For now just call simple till this mode is made */
	     _mixer_window_simple_pop_up(inst);
	  }
	else if (ev->flags == EVAS_BUTTON_NONE)
	  {
	     /* Call a simple window */
	     _mixer_window_simple_pop_up(inst);
	  }
     }
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
   Instance    *inst;
   Config_Item *ci;

   inst = data;
   if (!inst) return;
   ci = _mixer_config_item_get(inst->mixer, inst->gcc->id);
   if (!ci) return;
   _config_mixer_module(inst->mixer, ci);
}

static Config_Item *
_mixer_config_item_get(void *data, const char *id)
{
   Mixer         *mixer;
   Mixer_Card    *card;
   Mixer_Channel *chan;
   Evas_List     *l;
   Config_Item   *ci;

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
		  mixer_config->items = evas_list_append(mixer_config->items, ci);
		  return ci;
	       }
	  }
	     
	card = mixer->mix_sys->cards->data;
	if (!card) 
	  {
	     ci->card_id = 0;
	     ci->channel_id = 0;
	     ci->mode = SIMPLE_MODE;
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
		  chan = card->channels->data;
		  ci->channel_id = chan->id;
	       }
	     else 
	       ci->channel_id = 0;
	  }
     }

   ci->mode = SIMPLE_MODE;
   mixer_config->items = evas_list_append(mixer_config->items, ci);
   return ci;
}

static void 
_mixer_system_init(void *data) 
{
   Mixer        *mixer;
   Mixer_System *sys;
   
   mixer = data;
   if (!mixer) return;

   sys = E_NEW(Mixer_System, 1);
   if (!sys) return;
   mixer->mix_sys = sys;

   #ifdef HAVE_LIBASOUND
   sys->get_cards = alsa_get_cards;
   sys->get_card = alsa_get_card;
   sys->get_channels = alsa_card_get_channels;
   sys->get_channel = alsa_card_get_channel;
   sys->free_cards = alsa_free_cards;
   
   sys->get_volume = alsa_get_volume;
   sys->set_volume = alsa_set_volume;
   
   sys->get_mute = alsa_get_mute;
   sys->set_mute = alsa_set_mute;
   #endif
}

static void 
_mixer_system_shutdown(void *data) 
{
   Mixer_System *sys;
   
   sys = data;
   if (!sys) return;

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
   conf_item_edd = E_CONFIG_DD_NEW("Mixer_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL(D, T, id, STR);
   E_CONFIG_VAL(D, T, card_id, INT);
   E_CONFIG_VAL(D, T, channel_id, INT);
   E_CONFIG_VAL(D, T, mode, INT);

   conf_edd = E_CONFIG_DD_NEW("Mixer_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
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
	mixer_config->items = evas_list_append(mixer_config->items, ci);
     }
   
   mixer_config->module = m;

   e_gadcon_provider_register(&_gc_class);
   return m;
}

EAPI int
e_modapi_shutdown(E_Module *m)
{
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
   Evas_List *l;

   for (l = mixer_config->instances; l; l = l->next)
     {
	Instance *inst;
	Config_Item *ci;

	inst = l->data;
	ci = _mixer_config_item_get(inst->mixer, inst->gcc->id);
	if (ci->id) evas_stringshare_del(ci->id);
	ci->id = evas_stringshare_add(inst->gcc->id);
     }
   e_config_domain_save("module.mixer", conf_edd, mixer_config);
   return 1;
}

EAPI int
e_modapi_about(E_Module *m)
{
   e_module_dialog_show(m, _("Mixer"), 
			_("Mixer module lets you change volume."));
   return 1;
}

/* Makes the simple window containing the slider pop up */
static void
_mixer_window_simple_pop_up(Instance *inst)
{
   E_Container *con;
   Config_Item *ci;
   Mixer_Win_Simple *win;
   Evas_Coord ox, oy, ow, oh;
   Evas_Coord mw, mh;
   int cx, cy, cw, ch;
   
   if (!inst || !inst->mixer) return;
   if (!(con = e_container_current_get(e_manager_current_get()))) return;
   
   ci = _mixer_config_item_get(inst->mixer, inst->gcc->id);
   if (!ci) return;
   
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
	edje_object_part_text_set(win->bg_obj, "e.text.title", _("Volume"));
	edje_object_signal_emit(win->bg_obj, "e,action,show,title", "e");
        edje_object_message_signal_process(win->bg_obj);
        evas_object_show(win->bg_obj);
        
        win->vbox = e_box_add(win->window->evas);
        e_box_freeze(win->vbox);
        e_box_orientation_set(win->vbox, 0);
        
        win->slider = e_slider_add(win->window->evas);
        e_slider_value_range_set(win->slider, 0.0, 1.0);
        e_slider_orientation_set(win->slider, 0);
        /* TODO: Fix this in e_slider... */
        //e_slider_direction_set(win->slider, 1);
        evas_object_show(win->slider);
        e_slider_min_size_get(win->slider, &mw, &mh);
        e_box_pack_start(win->vbox, win->slider);
        e_box_pack_options_set(win->slider, 1, 1, 1, 1, 0.5, 0.5,
                               mw, mh, 9999, 9999);
        evas_object_smart_callback_add(win->slider, "changed",
                                       _mixer_window_simple_changed_cb, win);
        
        win->check = e_widget_check_add(win->window->evas, _("Mute"), &win->mute);
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
        
        win->mouse_move_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_MOVE,
                                                        _mixer_window_simple_mouse_move_cb, win);
        win->mouse_down_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_DOWN,
                                                        _mixer_window_simple_mouse_down_cb, win);
        win->mouse_up_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_BUTTON_UP,
                                                        _mixer_window_simple_mouse_up_cb, win);
        win->mouse_wheel_handler = ecore_event_handler_add(ECORE_X_EVENT_MOUSE_WHEEL,
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
        double v;
        
	if ((ci->card_id != 0) && (ci->channel_id != 0)) 
	  {
	     edje_object_signal_emit(e_slider_edje_object_get(win->slider), 
				     "e,state,enabled", "e");
	     vol = inst->mixer->mix_sys->get_volume(ci->card_id, ci->channel_id);
	     v = (1.0 - ((double)vol / 100));
	     e_slider_value_set(win->slider, v);
	     if (vol < 33)
	       edje_object_signal_emit(inst->mixer->base, "low", "");
	     else if ((vol >= 34) && (vol < 66))
	       edje_object_signal_emit(inst->mixer->base, "medium", "");
	     else if (vol > 66)
	       edje_object_signal_emit(inst->mixer->base, "high", "");
	     else if (vol <= 0) 
	       {
		  edje_object_signal_emit(inst->mixer->base, "muted", ""); 
		  edje_object_signal_emit(e_slider_edje_object_get(win->slider), 
					  "e,state,disabled", "e");
	       }
	  }	
     }

   if (inst->mixer->mix_sys->get_mute) 
     {
        int m;

	m = inst->mixer->mix_sys->get_mute(ci->card_id, ci->channel_id);
	e_widget_check_checked_set(win->check, m);
	if (m) 
	  {
	     edje_object_signal_emit(inst->mixer->base, "muted", ""); 
	     edje_object_signal_emit(e_slider_edje_object_get(win->slider), 
				     "e,state,disabled", "e");
	  }
	else 
	  {
	     edje_object_signal_emit(e_slider_edje_object_get(win->slider), 
				     "e,state,enabled", "e");
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
          win->x += cw;
          if (win->to_top) win->y += oh;
          break;
        case E_GADCON_ORIENT_RIGHT:
        case E_GADCON_ORIENT_CORNER_RT:
        case E_GADCON_ORIENT_CORNER_RB:
          win->x -= win->w;
          if (win->to_top) win->y += oh;
          break;
        default:
          win->x += (ow - win->w) / 2;
          if (win->x < cx) win->x = cx;
          if ((win->x + win->w) > (cx + cw)) win->x = cx + cw - win->w;
          if (!win->to_top) win->y += ch;
          break;
     }
   
   e_popup_move(win->window, win->x, win->y);
   e_popup_resize(win->window, win->w, 0);
   e_popup_show(win->window);
   
   win->start_time = ecore_time_get();
   if (win->slide_animator) ecore_animator_del(win->slide_animator);
   win->slide_animator = ecore_animator_add(_mixer_window_simple_animator_up_cb, win);
   win->popped_up = 1;
}

/* Makes the simple window containing the slider pop up */
static void
_mixer_window_simple_pop_down(Instance *inst)
{
   Mixer_Win_Simple *win;
   
   if (!(win = inst->mixer->simple_win) || !win->popped_up) return;
   
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
   if (win->slide_animator) ecore_animator_del(win->slide_animator);
   win->slide_animator = ecore_animator_add(_mixer_window_simple_animator_down_cb, win);
   win->popped_up = 0;
}

/* Makes the window slide when it pops up */
static int 
_mixer_window_simple_animator_up_cb(void *data)
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
   
   if (win->to_top) e_popup_move(win->window, win->x, win->y - h);
   e_popup_resize(win->window, win->w, h);
   
   if (h >= win->h)
     {
        win->slide_animator = NULL;
        return 0;
     }
   else
     return 1;
}

/* Makes the simple window pops down in animation */
static int 
_mixer_window_simple_animator_down_cb(void *data)
{
   Mixer_Win_Simple *win;
   double progress;
   int prev_h, h;
   
   if (!(win = data)) return 1;
   
   progress = (ecore_time_get() - win->start_time) / SLIDE_LENGTH;
   progress = E_CLAMP(progress, 0.0, 1.0);
   h = (1.0 - progress) * (1.0 - progress) * win->h;
   prev_h = win->window->h;
   
   e_popup_resize(win->window, win->w, h);
   if (win->to_top) e_popup_move(win->window, win->x, win->y - h);
   
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
   Mixer            *mixer;
   Config_Item      *ci;
   double            val;
   int               m;
   
   if (!(win = data)) return;
   
   mixer = win->mixer;
   if (!mixer) return;
   if (!mixer->mix_sys) return;
   if (!mixer->mix_sys->get_mute) return;
   if (!mixer->mix_sys->set_volume) return;
   
   ci = _mixer_config_item_get(mixer, mixer->inst->gcc->id);
   if (!ci) return;

   m = mixer->mix_sys->get_mute(ci->card_id, ci->channel_id);
   if (m) return;
   
   val = ((1.0 - (e_slider_value_get(obj))) * 100);
   if ((ci->card_id != 0) && (ci->channel_id != 0)) 
     {
	int ret;
	
	ret = mixer->mix_sys->set_volume(ci->card_id, ci->channel_id, val);
	if (ret)
	  {
	     if (val < 33)
	       edje_object_signal_emit(mixer->base, "low", "");
	     else if ((val >= 34) && (val < 66))
	       edje_object_signal_emit(mixer->base, "medium", "");
	     else if (val > 66)
	       edje_object_signal_emit(mixer->base, "high", ""); 
	     else if (val <= 0)
	       edje_object_signal_emit(mixer->base, "muted", "");
	  }
     }
}

/* Called when the "mute" checkbox is toggled */
static void 
_mixer_window_simple_mute_cb(void *data, Evas_Object *obj, void *event_info)
{
   Mixer_Win_Simple *win;
   Mixer            *mixer;
   Config_Item      *ci;
   double            val;
   int               m;
   
   if (!(win = data)) return;
   
   mixer = win->mixer;
   if (!mixer) return;
   if (!mixer->mix_sys) return;
   if (!mixer->mix_sys->set_mute) return;
   if (!mixer->mix_sys->get_volume) return;
   
   ci = _mixer_config_item_get(mixer, mixer->inst->gcc->id);
   if (!ci) return;

   m = e_widget_check_checked_get(win->check);
   mixer->mix_sys->set_mute(ci->card_id, ci->channel_id, m);
   if (m) 
     {
	edje_object_signal_emit(mixer->base, "muted", "");
	e_slider_value_set(win->slider, 1.0);
	edje_object_signal_emit(e_slider_edje_object_get(win->slider), 
				"e,state,disabled", "e");
     }
   else 
     {
	edje_object_signal_emit(mixer->base, "medium", "");
	e_slider_value_set(win->slider, 0.5);
	edje_object_signal_emit(e_slider_edje_object_get(win->slider), 
				"e,state,enabled", "e");	
     }
}

/* Called when the mouse moves over the input window */
static int
_mixer_window_simple_mouse_move_cb(void *data, int type, void *event)
{
   Mixer_Win_Simple *win;
   Ecore_X_Event_Mouse_Move *xev = event;
   
   if (!(win = data)) return 1;
   
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
   
   if (!(win = data) || !E_INSIDE(xev->x, xev->y,
                                  win->window->x, win->window->y,
                                  win->window->w, win->window->h))
     return 1;
   
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
   
   if (!(win = data)) return 1;
   
   if (E_INSIDE(xev->x, xev->y, win->window->x, win->window->y,
                win->window->w, win->window->h))
     {
        evas_event_feed_mouse_up(win->window->evas,
                                   xev->button, EVAS_BUTTON_NONE,
                                   xev->time, NULL);
     }
   else if (xev->button == 1) //&& !win->first_mouse_up)
     _mixer_window_simple_pop_down(win->mixer->inst);
   
   if ((xev->button == 1) && win->first_mouse_up)
     win->first_mouse_up = 0;
   
   return 1;
}

/* Called when the mouse wheel is used over the input window */
static int
_mixer_window_simple_mouse_wheel_cb(void *data, int type, void *event)
{
   Mixer_Win_Simple *win;
   Ecore_X_Event_Mouse_Wheel *xev = event;
   
   if (!(win = data) || !E_INSIDE(xev->x, xev->y,
                                  win->window->x, win->window->y,
                                  win->window->w, win->window->h))
     return 1;
   
   evas_event_feed_mouse_wheel(win->window->evas,
                              xev->direction, xev->z,
                              xev->time, NULL);
   
   return 1;
}
