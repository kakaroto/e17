#include <e.h>
#include "e_mod_main.h"
// #include "e_mixer.h"
#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

/* Func Proto Requirements for Gadcon */
static E_Gadcon_Client *_gc_init (E_Gadcon * gc, const char *name,
				  const char *id, const char *style);
static void _gc_shutdown (E_Gadcon_Client * gcc);
static void _gc_orient (E_Gadcon_Client * gcc);
static char *_gc_label (void);
static Evas_Object *_gc_icon (Evas * evas);

/* Module Protos */
static void _mixer_cb_mouse_down (void *data, Evas * e, Evas_Object * obj,
				   void *event_info);
static void _mixer_menu_cb_configure (void *data, E_Menu * m,
				       E_Menu_Item * mi);
static void _mixer_menu_cb_post (void *data, E_Menu * m);
static int _mixer_cb_check (void *data);
static Config_Item *_mixer_config_item_get (const char *id);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *mixer_config = NULL;

/* Define the class and gadcon functions this module provides */
static const E_Gadcon_Client_Class _gc_class = {
  GADCON_CLIENT_CLASS_VERSION,
  "mixer", {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon}
};

typedef struct _Instance Instance;
struct _Instance
{
  E_Gadcon_Client *gcc;
  Evas_Object *mixer;
  Evas_Object *slider;
  Ecore_Timer *check_timer;
};

static E_Gadcon_Client *
_gc_init (E_Gadcon * gc, const char *name, const char *id, const char *style)
{
  Evas_Object *o, *bar;
  E_Gadcon_Client *gcc;
  //Evas_List *mixer_system, *mixer_list, *mixer_elem_list;
  //Mixer *mixer;
  //Mixer_Elem *mixer_elem;
  //int l, r;
  Instance *inst;
  char buf[4096];
  Config_Item *ci;

  inst = E_NEW (Instance, 1);

  ci = _mixer_config_item_get (id);
  if (!ci->id)
    ci->id = evas_stringshare_add (id);

  
  o = edje_object_add (gc->evas);
  snprintf (buf, sizeof (buf), "%s/mixer.edj",
	    e_module_dir_get (mixer_config->module));
  if (!e_theme_edje_object_set
      (o, "base/theme/modules/mixer", "modules/mixer/main"))
    edje_object_file_set (o, buf, "modules/mixer/main");

  evas_object_show (o);

/*
  mixer_system = mixer_system_get_list();
  mixer_list = mixer_system_open(evas_list_data(mixer_system));
  mixer = evas_list_data(mixer_list);
  mixer_elem_list = mixer->elems;
  mixer_elem = evas_list_data(mixer_elem_list);
  mixer_get_volume(mixer_elem, l, r);
  ci->volume = l + r / 2;
*/

  bar = e_widget_slider_add(gc->evas, 1, 0, _("%3.0f %%"), 0, 100, 1, 0, NULL, &ci->volume, 100);
  edje_object_part_swallow(o, "mixerbar", bar);
//  evas_object_pass_events_set(bar, 1);
  evas_object_show (bar);

  gcc = e_gadcon_client_new (gc, name, id, style, o);
  gcc->data = inst;
  inst->gcc = gcc;
  inst->mixer = o;
  inst->slider = bar;

  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
				  _mixer_cb_mouse_down, inst);
  evas_object_event_callback_add (bar, EVAS_CALLBACK_MOUSE_DOWN,
				  _mixer_cb_mouse_down, inst);

  mixer_config->instances =
    evas_list_append (mixer_config->instances, inst);

  _mixer_cb_check (inst);
  return gcc;
}

static void
_gc_shutdown (E_Gadcon_Client * gcc)
{
  Instance *inst;

  inst = gcc->data;
  if (inst->check_timer)
    ecore_timer_del (inst->check_timer);
  mixer_config->instances =
    evas_list_remove (mixer_config->instances, inst);

  evas_object_event_callback_del (inst->mixer, EVAS_CALLBACK_MOUSE_DOWN,
				  _mixer_cb_mouse_down);

  /* FIXME: slider doesnt like mouse events */

  evas_object_event_callback_del (inst->slider, EVAS_CALLBACK_MOUSE_DOWN,
				  _mixer_cb_mouse_down);

  evas_object_del (inst->mixer);
  free (inst);
  inst = NULL;
}

static void
_gc_orient (E_Gadcon_Client * gcc)
{
  Instance *inst;
  Evas_Coord mw, mh;

  inst = gcc->data;
  edje_object_size_min_calc (inst->mixer, &mw, &mh);
  e_gadcon_client_min_size_set (gcc, mw, mh);
}

static char *
_gc_label (void)
{
  return D_ ("Mixer");
}

static Evas_Object *
_gc_icon (Evas * evas)
{
  Evas_Object *o;
  char buf[4096];

  o = edje_object_add (evas);
  snprintf (buf, sizeof (buf), "%s/module.eap",
	    e_module_dir_get (mixer_config->module));
  edje_object_file_set (o, buf, "icon");
  return o;
}

static void
_mixer_cb_mouse_down (void *data, Evas * e, Evas_Object * obj,
		       void *event_info)
{
  Instance *inst;
  Evas_Event_Mouse_Down *ev;

  inst = data;
  ev = event_info;
  if ((ev->button == 3) && (!mixer_config->menu))
    {
      E_Menu *mn;
      E_Menu_Item *mi;
      int x, y, w, h;

      mn = e_menu_new ();
      e_menu_post_deactivate_callback_set (mn, _mixer_menu_cb_post, inst);
      mixer_config->menu = mn;

      mi = e_menu_item_new (mn);
      e_menu_item_label_set (mi, D_ ("Configuration"));
      e_util_menu_item_edje_icon_set (mi, "enlightenment/configuration");
      e_menu_item_callback_set (mi, _mixer_menu_cb_configure, inst);

      mi = e_menu_item_new (mn);
      e_menu_item_separator_set (mi, 1);

      e_gadcon_client_util_menu_items_append (inst->gcc, mn, 0);
      e_gadcon_canvas_zone_geometry_get (inst->gcc->gadcon, &x, &y, &w, &h);
      e_menu_activate_mouse (mn,
			     e_util_zone_current_get (e_manager_current_get
						      ()), x + ev->output.x,
			     y + ev->output.y, 1, 1,
			     E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
      evas_event_feed_mouse_up (inst->gcc->gadcon->evas, ev->button,
				EVAS_BUTTON_NONE, ev->timestamp, NULL);
    }
}

static void
_mixer_menu_cb_post (void *data, E_Menu * m)
{
  if (!mixer_config->menu)
    return;
  e_object_del (E_OBJECT (mixer_config->menu));
  mixer_config->menu = NULL;
}

static void
_mixer_menu_cb_configure (void *data, E_Menu * m, E_Menu_Item * mi)
{
  Instance *inst;
  Config_Item *ci;

  inst = data;
  ci = _mixer_config_item_get (inst->gcc->id);
  _config_mixer_module (ci);
}

void
_mixer_config_updated (const char *id)
{
  Evas_List *l;
  Config_Item *ci;

  if (!mixer_config)
    return;
  ci = _mixer_config_item_get (id);
  /* FIXME: stuff here */
}

static int
_mixer_cb_check (void *data)
{
  Instance *inst;
  Config_Item *ci;
  time_t current_time;
  struct tm *local_time;
  char buf[1024];

  inst = data;
  ci = _mixer_config_item_get (inst->gcc->id);
  /* FIXME: stuff here */

  return 1;
}

static Config_Item *
_mixer_config_item_get (const char *id)
{
  Evas_List *l;
  Config_Item *ci;

  for (l = mixer_config->items; l; l = l->next)
    {
      ci = l->data;
      if (!ci->id)
	continue;
      if (!strcmp (ci->id, id))
	return ci;
    }

  ci = E_NEW (Config_Item, 1);
  ci->id = evas_stringshare_add (id);
  ci->volume = 10;
  /* FIXME: stuff here */

  mixer_config->items = evas_list_append (mixer_config->items, ci);
  return ci;
}

EAPI E_Module_Api e_modapi = {
  E_MODULE_API_VERSION,
  "Mixer"
};

EAPI void *
e_modapi_init (E_Module * m)
{
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");

  conf_item_edd = E_CONFIG_DD_NEW ("Mixer_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
  E_CONFIG_VAL (D, T, id, STR);
  E_CONFIG_VAL (D, T, volume, INT);

  conf_edd = E_CONFIG_DD_NEW ("Mixer_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
  E_CONFIG_LIST (D, T, items, conf_item_edd);

  mixer_config = e_config_domain_load ("module.mixer", conf_edd);
  if (!mixer_config)
    {
      Config_Item *ci;

      mixer_config = E_NEW (Config, 1);

      ci = E_NEW (Config_Item, 1);
      ci->id = evas_stringshare_add ("0");
      ci->volume = 10;

      mixer_config->items = evas_list_append (mixer_config->items, ci);
    }
  mixer_config->module = m;

  e_gadcon_provider_register (&_gc_class);
  return m;
}

EAPI int
e_modapi_shutdown (E_Module * m)
{
  mixer_config->module = NULL;
  e_gadcon_provider_unregister (&_gc_class);

  if (mixer_config->config_dialog)
    e_object_del (E_OBJECT (mixer_config->config_dialog));
  if (mixer_config->menu)
    {
      e_menu_post_deactivate_callback_set (mixer_config->menu, NULL, NULL);
      e_object_del (E_OBJECT (mixer_config->menu));
      mixer_config->menu = NULL;
    }

  while (mixer_config->items)
    {
      Config_Item *ci;

      ci = mixer_config->items->data;
      mixer_config->items =
	evas_list_remove_list (mixer_config->items, mixer_config->items);
      if (ci->id)
	evas_stringshare_del (ci->id);
      free (ci);
      ci = NULL;
    }

  free (mixer_config);
  mixer_config = NULL;
  E_CONFIG_DD_FREE (conf_item_edd);
  E_CONFIG_DD_FREE (conf_edd);
  return 1;
}

EAPI int
e_modapi_save (E_Module * m)
{
  Evas_List *l;

  for (l = mixer_config->instances; l; l = l->next)
    {
      Instance *inst;
      Config_Item *ci;

      inst = l->data;
      ci = _mixer_config_item_get (inst->gcc->id);
      if (ci->id)
	evas_stringshare_del (ci->id);
      ci->id = evas_stringshare_add (inst->gcc->id);
    }
  e_config_domain_save ("module.mixer", conf_edd, mixer_config);
  return 1;
}

EAPI int
e_modapi_about (E_Module * m)
{
  e_module_dialog_show (m, D_ ("Mixer"),
			D_ ("Mixer module lets you change volume."));
  return 1;
}
