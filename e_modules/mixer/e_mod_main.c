#include <e.h>
#include "e_mod_main.h"
#include "alsa_mixer.h"

/* Define to 1 for testing alsa code */
#define DEBUG 0
#define SLIDE_LENGTH 0.5

typedef struct _Instance Instance;
typedef struct _Mixer Mixer;
typedef struct _Mixer_Win Mixer_Win;
typedef struct _Mixer_Win_Simple Mixer_Win_Simple;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Mixer           *mixer;
};

struct _Mixer
{
   Instance    *inst;
   Evas        *evas;
   Mixer_Win   *win;
   Mixer_Win_Simple *simple_win;
   
   Evas_Object *base;
};

struct _Mixer_Win 
{
   Mixer       *mixer;
   E_Win       *window;

   Evas_Object *bg_obj;
};

struct _Mixer_Win_Simple
{
   Mixer       *mixer;
   E_Win       *window;
   
   Evas_Object *event_obj;
   Evas_Object *bg_obj;
   Evas_Object *slider;
   
   int         x, y, w, h;
   int         to_top;
   int         popped_up;
   double      start_time;
   Ecore_Animator *slide_animator;
};
   
/* Gadcon Protos */
static E_Gadcon_Client *_gc_init     (E_Gadcon * gc, const char *name, 
				      const char *id, const char *style);
static void             _gc_shutdown (E_Gadcon_Client * gcc);
static void             _gc_orient   (E_Gadcon_Client * gcc);
static char            *_gc_label    (void);
static Evas_Object     *_gc_icon     (Evas * evas);

/* Module Protos */
static Config_Item *_mixer_config_item_get   (const char *id);
static void         _mixer_menu_cb_post      (void *data, E_Menu *m);
static void         _mixer_menu_cb_configure (void *data, E_Menu *m, 
					      E_Menu_Item *mi);
static void         _mixer_cb_mouse_down     (void *data, Evas *e, 
					      Evas_Object *obj, 
					      void *event_info);
static void         _mixer_menu_cb_configure (void *data, E_Menu *m, 
					      E_Menu_Item *mi);

static void _mixer_window_simple_pop_up           (Instance *inst);
static void _mixer_window_simple_pop_down         (Instance *inst);
static int  _mixer_window_simple_animator_up_cb   (void *data);
static int  _mixer_window_simple_animator_down_cb (void *data);
static void _mixer_window_simple_mouse_up_cb      (void *data, Evas *e, Evas_Object *obj, void *event_info);
static void _mixer_window_simple_changed_cb       (void *data, Evas_Object *obj, void *event_info);
static void _mixer_window_simple_resize_cb        (E_Win *win);

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

#if DEBUG
   Evas_List *c;

   c = alsa_get_cards();
   if (c) 
     {
	Evas_List *l;
	
	for (l = c; l; l = l->next) 
	  {
	     Alsa_Card *card;
		  
	     card = c->data;
	     if (!card) continue;
	     printf("\nFound Card: %s\tId: %i\n\n", card->real, card->id);
	  }
	alsa_free_cards(c);
     }
#endif
   
   inst = E_NEW(Instance, 1);
   if (!inst) return NULL;
   
   ci = _mixer_config_item_get(id);
   if (!ci->id) ci->id = evas_stringshare_add(id);
   
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

   if (mixer->win) 
     {
	if (mixer->win->bg_obj) evas_object_del(mixer->win->bg_obj);
	e_object_del(E_OBJECT(mixer->win->window));
	E_FREE(mixer->win);
     }
   
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
	#if DEBUG
	e_menu_item_callback_set(mi, _mixer_menu_cb_configure, inst);
	#endif
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
	     /* Call full mixer window */
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
   ci = _mixer_config_item_get(inst->gcc->id);
   if (!ci) return;
   _config_mixer_module(ci);
}

static Config_Item *
_mixer_config_item_get(const char *id)
{
   Evas_List   *l;
   Config_Item *ci;

   for (l = mixer_config->items; l; l = l->next)
     {
	ci = l->data;
	if (!ci->id) continue;
	if (!strcmp(ci->id, id)) return ci;
     }

   ci = E_NEW(Config_Item, 1);
   ci->id = evas_stringshare_add(id);
   ci->card_id = 0;

   mixer_config->items = evas_list_append(mixer_config->items, ci);
   return ci;
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
	ci = _mixer_config_item_get(inst->gcc->id);
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
   Mixer_Win_Simple *win;
   Evas_Coord ox, oy, ow, oh;
   Evas_Coord sw, sh;
   int cx, cy, cw, ch;
   
   if (!inst || !inst->mixer)
     return;
   if (!(con = e_container_current_get(e_manager_current_get())))
     return;
   
   evas_object_geometry_get(inst->mixer->base, &ox, &oy, &ow, &oh); 
   
   if (!(win = inst->mixer->simple_win))
     {
        win = E_NEW(Mixer_Win_Simple, 1);
        inst->mixer->simple_win = win;
        win->mixer = inst->mixer;
        win->window = e_win_new(con);
        e_win_placed_set(win->window, 1);
        e_win_borderless_set(win->window, 1);
        e_win_layer_set(win->window, 255);
        e_win_resize_callback_set(win->window, _mixer_window_simple_resize_cb);
        win->window->data = win;
        
        win->event_obj = evas_object_rectangle_add(e_win_evas_get(win->window));
        evas_object_color_set(win->event_obj, 255, 255, 255, 0);
        evas_object_show(win->event_obj);
        evas_object_event_callback_add(win->event_obj, EVAS_CALLBACK_MOUSE_UP,
                                       _mixer_window_simple_mouse_up_cb, win);
	
        win->bg_obj = edje_object_add(e_win_evas_get(win->window));
        e_theme_edje_object_set(win->bg_obj, "base/theme/menus",
				"e/widgets/menu/default/background");
	edje_object_part_text_set(win->bg_obj, "e.text.title", "Volume");
	edje_object_signal_emit(win->bg_obj, "e,action,show,title", "e");
        edje_object_message_signal_process(win->bg_obj);
        evas_object_repeat_events_set(win->bg_obj, 1);
        evas_object_show(win->bg_obj);
        
        win->slider = e_slider_add(e_win_evas_get(win->window));
        e_slider_value_range_set(win->slider, 0.0, 1.0);
        e_slider_orientation_set(win->slider, 0);
        /* TODO: Fix this in e_slider... */
        //e_slider_direction_set(win->slider, 1);
        /* TODO: this has no effect: Bug in Evas ? */
        evas_object_repeat_events_set(win->slider, 1);
        evas_object_show(win->slider);
        evas_object_smart_callback_add(win->slider, "changed",
                                       _mixer_window_simple_changed_cb, win);
        
        e_slider_min_size_get(win->slider, &sw, &sh);
        if (sw < ow)     sw = ow;
        if (sh < 150)    sh = 150;
        edje_extern_object_min_size_set(win->slider, sw, sh);
        edje_object_part_swallow(win->bg_obj, "e.swallow.content", win->slider);
        
        
        edje_object_size_min_calc(win->bg_obj, &win->w, &win->h);
        evas_object_move(win->bg_obj, 0, 0);
        evas_object_resize(win->bg_obj, win->w, win->h);
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
          if (win->to_top)
            win->y += oh;
          break;
        case E_GADCON_ORIENT_RIGHT:
        case E_GADCON_ORIENT_CORNER_RT:
        case E_GADCON_ORIENT_CORNER_RB:
          win->x -= win->w;
          if (win->to_top)
            win->y += oh;
          break;
        default:
          win->x += (ow - win->w) / 2;
          if (win->x < cx)
            win->x = cx;
          if ((win->x + win->w) > (cx + cw))
            win->x = cx + cw - win->w;
          if (!win->to_top)
            win->y += ch;
          break;
     }
   
   e_win_move(win->window, win->x, win->y);
   e_win_resize(win->window, win->w, 0);
   e_win_show(win->window);
   
   win->start_time = ecore_time_get();
   if (win->slide_animator)
      ecore_animator_del(win->slide_animator);
   win->slide_animator = ecore_animator_add(_mixer_window_simple_animator_up_cb, win);
   win->popped_up = 1;
}

/* Makes the simple window containing the slider pop up */
static void
_mixer_window_simple_pop_down(Instance *inst)
{
   Mixer_Win_Simple *win;
   
   if (!(win = inst->mixer->simple_win) || !win->popped_up)
     return;
   
   win->start_time = ecore_time_get();
   if (win->slide_animator)
      ecore_animator_del(win->slide_animator);
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
   
   if (!(win = data))
     return 1;
   
   progress = (ecore_time_get() - win->start_time) / SLIDE_LENGTH;
   progress = E_CLAMP(progress, 0.0, 1.0);
   progress = 1.0 - (1.0 - progress) * (1.0 - progress);
   h = progress * win->h;
   prev_h = win->window->h;
   
   if (win->to_top)
     e_win_move(win->window, win->x, win->y - h);
   e_win_resize(win->window, win->w, h);
   
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
   
   if (!(win = data))
     return 1;
   
   progress = (ecore_time_get() - win->start_time) / SLIDE_LENGTH;
   progress = E_CLAMP(progress, 0.0, 1.0);
   h = (1.0 - progress) * (1.0 - progress) * win->h;
   prev_h = win->window->h;
   
   if (win->to_top)
     e_win_move(win->window, win->x, win->y - h);
   e_win_resize(win->window, win->w, h);
   
   if (h <= 0)
     {
        e_win_hide(win->window);
        win->slide_animator = NULL;
        return 0;
     }
   else
     return 1;
}

/* Called when the background object of the simple window is released */
static void 
_mixer_window_simple_mouse_up_cb(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   Mixer_Win_Simple *win;
   
   if (!(win = data)) return;
   
   printf("Mouse up\n");
   _mixer_window_simple_pop_down(win->mixer->inst);
}

/* Called when the value of the slider of the simple window is changed */
static void 
_mixer_window_simple_changed_cb(void *data, Evas_Object *obj, void *event_info)
{
   Mixer_Win_Simple *win;
   
   if (!(win = data))
     return;
   
   printf("Slider value: %f\n", e_slider_value_get(obj));
}

/* Called when the simple window is resized */
static void 
_mixer_window_simple_resize_cb(E_Win *win)
{
   Mixer_Win_Simple *simple_win;
   
   if (!win || !(simple_win = win->data))
     return;
   
   evas_object_move(simple_win->event_obj, 0, 0);
   evas_object_resize(simple_win->event_obj, win->w, win->h);
}
