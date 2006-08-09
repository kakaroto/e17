#include <e.h>
#include "e_mod_main.h"

typedef struct _Instance Instance;
typedef struct _Net Net;

struct _Instance
{
  E_Gadcon_Client *gcc;
  Evas_Object *net_obj;
  Net *net;
  Ecore_Timer *check_timer;
  unsigned long old_in;
  unsigned long old_out;
};

struct _Net
{
  Instance *inst;
  Evas_Object *net_obj;
};

/* Func Protos for Gadcon */
static E_Gadcon_Client *_gc_init (E_Gadcon * gc, const char *name,
				  const char *id, const char *style);
static void _gc_shutdown (E_Gadcon_Client * gcc);
static void _gc_orient (E_Gadcon_Client * gcc);
static char *_gc_label (void);
static Evas_Object *_gc_icon (Evas * evas);

/* Func Protos for Module */
static void _net_cb_mouse_down (void *data, Evas * e, Evas_Object * obj,
				void *event_info);
static void _net_cb_mouse_in (void *data, Evas * e, Evas_Object * obj,
			      void *event_info);
static void _net_cb_mouse_out (void *data, Evas * e, Evas_Object * obj,
			       void *event_info);
static void _net_menu_cb_configure (void *data, E_Menu * m, E_Menu_Item * mi);
static void _net_menu_cb_post (void *data, E_Menu * m);
static Config_Item *_net_config_item_get (const char *id);
static Net *_net_new (Evas * evas);
static void _net_free (Net * net);
static void _net_update_rx (void *data, int value);
static void _net_update_tx (void *data, int value);
static int _net_cb_check (void *data);

static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;

Config *net_config = NULL;

/* Define the gadcon class and functions provided by this module */
static const E_Gadcon_Client_Class _gc_class = {
  GADCON_CLIENT_CLASS_VERSION,
  "net", {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon},
  E_GADCON_CLIENT_STYLE_PLAIN
};

static E_Gadcon_Client *
_gc_init (E_Gadcon * gc, const char *name, const char *id, const char *style)
{
  E_Gadcon_Client *gcc;
  Evas_Object *o;
  Instance *inst;
  Config_Item *ci;
  Net *net;

  inst = E_NEW (Instance, 1);

  ci = _net_config_item_get (id);
  if (!ci->id)
    ci->id = evas_stringshare_add (id);

  net = _net_new (gc->evas);
  net->inst = inst;
  inst->net = net;

  o = net->net_obj;
  gcc = e_gadcon_client_new (gc, name, id, style, o);
  gcc->data = inst;
  inst->gcc = gcc;
  inst->net_obj = o;

  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
				  _net_cb_mouse_down, inst);
  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_MOVE,
				  _net_cb_mouse_in, inst);
  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_IN, _net_cb_mouse_in,
				  inst);
  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_OUT,
				  _net_cb_mouse_out, inst);

  if (ci->always_text)
    edje_object_signal_emit (inst->net_obj, "label_active", "");

  _net_cb_check (inst);

  inst->check_timer = ecore_timer_add (ci->poll_time, _net_cb_check, inst);
  net_config->instances = evas_list_append (net_config->instances, inst);

  return gcc;
}

static void
_gc_orient (E_Gadcon_Client * gcc)
{
  e_gadcon_client_aspect_set (gcc, 16, 16);
  e_gadcon_client_min_size_set (gcc, 16, 16);
}

static char *
_gc_label (void)
{
  return D_ ("Net");
}

static Evas_Object *
_gc_icon (Evas * evas)
{
  Evas_Object *o;
  char buf[4096];

  o = edje_object_add (evas);
  snprintf (buf, sizeof (buf), "%s/module.eap",
	    e_module_dir_get (net_config->module));
  edje_object_file_set (o, buf, "icon");
  return o;
}

static void
_gc_shutdown (E_Gadcon_Client * gcc)
{
  Instance *inst;
  Net *net;

  inst = gcc->data;
  net = inst->net;

  if (inst->check_timer)
    ecore_timer_del (inst->check_timer);
  net_config->instances = evas_list_remove (net_config->instances, inst);

  evas_object_event_callback_del (net->net_obj, EVAS_CALLBACK_MOUSE_DOWN,
				  _net_cb_mouse_down);
  evas_object_event_callback_del (net->net_obj, EVAS_CALLBACK_MOUSE_MOVE,
				  _net_cb_mouse_in);
  evas_object_event_callback_del (net->net_obj, EVAS_CALLBACK_MOUSE_IN,
				  _net_cb_mouse_in);
  evas_object_event_callback_del (net->net_obj, EVAS_CALLBACK_MOUSE_OUT,
				  _net_cb_mouse_out);

  _net_free (net);
  free (inst);
  inst = NULL;
}

static void
_net_cb_mouse_down (void *data, Evas * e, Evas_Object * obj, void *event_info)
{
  Instance *inst;
  Evas_Event_Mouse_Down *ev;

  inst = data;
  ev = event_info;
  if ((ev->button == 3) && (!net_config->menu))
    {
      E_Menu *mn;
      E_Menu_Item *mi;
      int x, y, w, h;

      mn = e_menu_new ();
      e_menu_post_deactivate_callback_set (mn, _net_menu_cb_post, inst);
      net_config->menu = mn;

      mi = e_menu_item_new (mn);
      e_menu_item_label_set (mi, D_ ("Configuration"));
      e_util_menu_item_edje_icon_set (mi, "enlightenment/configuration");
      e_menu_item_callback_set (mi, _net_menu_cb_configure, inst);

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
_net_menu_cb_post (void *data, E_Menu * m)
{
  if (!net_config->menu)
    return;
  e_object_del (E_OBJECT (net_config->menu));
  net_config->menu = NULL;
}

static void
_net_menu_cb_configure (void *data, E_Menu * m, E_Menu_Item * mi)
{
  Instance *inst;
  Config_Item *ci;

  inst = data;
  ci = _net_config_item_get (inst->gcc->id);
  _config_net_module (ci);
}

void
_net_config_updated (const char *id)
{
  Evas_List *l;
  Config_Item *ci;

  if (!net_config)
    return;
  ci = _net_config_item_get (id);
  for (l = net_config->instances; l; l = l->next)
    {
      Instance *inst;

      inst = l->data;
      if (!inst->gcc->id)
	continue;
      if (!strcmp (inst->gcc->id, ci->id))
	{
	  if (inst->check_timer)
	    ecore_timer_del (inst->check_timer);
	  inst->check_timer =
	    ecore_timer_add ((double) ci->poll_time, _net_cb_check, inst);
	  if (ci->always_text)
	    edje_object_signal_emit (inst->net_obj, "label_active", "");
	  else
	    edje_object_signal_emit (inst->net_obj, "label_passive", "");

	  break;
	}
    }
}

static Config_Item *
_net_config_item_get (const char *id)
{
  Evas_List *l;
  Config_Item *ci;

  for (l = net_config->items; l; l = l->next)
    {
      ci = l->data;
      if (!ci->id)
	continue;
      if (!strcmp (ci->id, id))
	return ci;
    }
  ci = E_NEW (Config_Item, 1);
  ci->id = evas_stringshare_add (id);
  ci->device = evas_stringshare_add ("eth0");
  ci->poll_time = 1.0;
  ci->max = 1500.0;
  ci->always_text = 0;
  net_config->items = evas_list_append (net_config->items, ci);
  return ci;
}

/* Module routines */
EAPI E_Module_Api e_modapi = {
  E_MODULE_API_VERSION,
  "Net"
};

EAPI void *
e_modapi_init (E_Module * m)
{
  bindtextdomain (PACKAGE, LOCALEDIR);
  bind_textdomain_codeset (PACKAGE, "UTF-8");

  conf_item_edd = E_CONFIG_DD_NEW ("Net_Config_Item", Config_Item);

#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
  E_CONFIG_VAL (D, T, id, STR);
  E_CONFIG_VAL (D, T, device, STR);
  E_CONFIG_VAL (D, T, max, DOUBLE);
  E_CONFIG_VAL (D, T, poll_time, DOUBLE);
  E_CONFIG_VAL (D, T, always_text, INT);

  conf_edd = E_CONFIG_DD_NEW ("Net_Config", Config);

#undef T
#undef D
#define T Config
#define D conf_edd
  E_CONFIG_LIST (D, T, items, conf_item_edd);

  net_config = e_config_domain_load ("module.net", conf_edd);
  if (!net_config)
    {
      Config_Item *ci;

      net_config = E_NEW (Config, 1);
      ci = E_NEW (Config_Item, 1);

      ci->id = evas_stringshare_add ("0");
      ci->device = evas_stringshare_add ("eth0");
      ci->poll_time = 1.0;
      ci->max = 1500.0;
      ci->always_text = 0;
      net_config->items = evas_list_append (net_config->items, ci);
    }
  net_config->module = m;
  e_gadcon_provider_register (&_gc_class);
  return m;
}

EAPI int
e_modapi_shutdown (E_Module * m)
{
  net_config->module = NULL;
  e_gadcon_provider_unregister (&_gc_class);

  if (net_config->config_dialog)
    e_object_del (E_OBJECT (net_config->config_dialog));
  if (net_config->menu)
    {
      e_menu_post_deactivate_callback_set (net_config->menu, NULL, NULL);
      e_object_del (E_OBJECT (net_config->menu));
      net_config->menu = NULL;
    }
  while (net_config->items)
    {
      Config_Item *ci;

      ci = net_config->items->data;
      net_config->items =
	evas_list_remove_list (net_config->items, net_config->items);
      if (ci->id)
	evas_stringshare_del (ci->id);
      free (ci);
      ci = NULL;
    }
  free (net_config);
  net_config = NULL;
  E_CONFIG_DD_FREE (conf_item_edd);
  E_CONFIG_DD_FREE (conf_edd);
  return 1;
}

EAPI int
e_modapi_save (E_Module * m)
{
  Evas_List *l;

  for (l = net_config->instances; l; l = l->next)
    {
      Instance *inst;
      Config_Item *ci;

      inst = l->data;
      ci = _net_config_item_get (inst->gcc->id);
      if (ci->id)
	evas_stringshare_del (ci->id);
      ci->id = evas_stringshare_add (inst->gcc->id);
    }
  e_config_domain_save ("module.net", conf_edd, net_config);
  return 1;
}

EAPI int
e_modapi_about (E_Module * m)
{
  e_module_dialog_show (m, D_ ("Enlightenment Network Monitor Module"),
			D_
			("This module is used to monitor a network device."));
  return 1;
}

static Net *
_net_new (Evas * evas)
{
  Net *net;
  char buf[4096];

  net = E_NEW (Net, 1);
  net->net_obj = edje_object_add (evas);
  snprintf (buf, sizeof (buf), "%s/net.edj",
	    e_module_dir_get (net_config->module));
  if (!e_theme_edje_object_set
      (net->net_obj, "base/theme/modules/net", "modules/net/main"))
    edje_object_file_set (net->net_obj, buf, "modules/net/main");
  evas_object_show (net->net_obj);
  return net;
}

static void
_net_free (Net * n)
{
  evas_object_del (n->net_obj);
  free (n);
  n = NULL;
}

static void
_net_update_rx (void *data, int value)
{
  Edje_Message_Int_Set *val;
  Instance *inst;

  inst = data;
  val = malloc (sizeof (Edje_Message_Int_Set) + (1 * sizeof (int)));
  val->count = 1;
  val->val[0] = value;
  edje_object_message_send (inst->net_obj, EDJE_MESSAGE_INT_SET, 1, val);
  free (val);
  val = NULL;
}

static void
_net_update_tx (void *data, int value)
{
  Edje_Message_Int_Set *val;
  Instance *inst;

  inst = data;
  val = malloc (sizeof (Edje_Message_Int_Set) + (1 * sizeof (int)));
  val->count = 1;
  val->val[0] = value;
  edje_object_message_send (inst->net_obj, EDJE_MESSAGE_INT_SET, 2, val);
  free (val);
  val = NULL;
}

static void
_net_cb_mouse_in (void *data, Evas * e, Evas_Object * obj, void *event_info)
{
  Instance *inst;

  inst = data;
  edje_object_signal_emit (inst->net_obj, "label_active", "");
}

static void
_net_cb_mouse_out (void *data, Evas * e, Evas_Object * obj, void *event_info)
{
  Instance *inst;
  Config_Item *ci;

  inst = data;
  ci = _net_config_item_get (inst->gcc->id);
  if (!ci->always_text)
    edje_object_signal_emit (inst->net_obj, "label_passive", "");
}

static int
_net_cb_check (void *data)
{
  Instance *inst;
  Config_Item *ci;
  FILE *stat;
  char dev[64];
  char buf[256];
  unsigned long in = 0;
  unsigned long out = 0;
  unsigned long dummy = 0;
  int found;
  long bytes_in, bytes_out;
  char in_str[100];
  char out_str[100];
  double i, o;

  inst = data;
  ci = _net_config_item_get (inst->gcc->id);

  stat = fopen ("/proc/net/dev", "r");
  if (!stat)
    return 1;

  found = 0;
  while (fgets (buf, 256, stat))
    {
      int i = 0;

      for (; buf[i] != 0; i++)
	if (buf[i] == ':')
	  buf[i] = ' ';

      if (sscanf (buf, "%s %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu %lu "
		  "%lu %lu %lu %lu\n", dev, &in, &dummy, &dummy,
		  &dummy, &dummy, &dummy, &dummy, &dummy, &out, &dummy,
		  &dummy, &dummy, &dummy, &dummy, &dummy, &dummy) < 17)
	continue;
      if (!ci->device)
	continue;
      if (!strcmp (dev, ci->device))
	{
	  found = 1;
	  break;
	}
    }
  fclose (stat);

  if (!found)
    return 1;

  bytes_in = in - inst->old_in;
  bytes_out = out - inst->old_out;
  bytes_in = bytes_in / ci->poll_time;
  bytes_out = bytes_out / ci->poll_time;

  if (bytes_in < 0)
    bytes_in = 0;
  if (bytes_out < 0)
    bytes_out = 0;

  inst->old_in = in;
  inst->old_out = out;

  edje_object_part_text_set (inst->net_obj, "net_label", ci->device);

  if (bytes_in <= 0)
    edje_object_part_text_set (inst->net_obj, "rx_label", "Rx: 0 B");
  else
    {
      if (bytes_in > 1048576)
	snprintf (in_str, sizeof (in_str), "Rx: %ld Mb",
		  (bytes_in / 1048576));
      else if (bytes_in > 1024 && bytes_in < 1048576)
	snprintf (in_str, sizeof (in_str), "Rx: %ld Kb", (bytes_in / 1024));
      else
	snprintf (in_str, sizeof (in_str), "Rx: %ld B", bytes_in);

      edje_object_part_text_set (inst->net_obj, "rx_label", in_str);
    }

  if (bytes_out <= 0)
    edje_object_part_text_set (inst->net_obj, "tx_label", "Tx: 0 B");
  else
    {
      if (bytes_out > 1048576)
	snprintf (out_str, sizeof (out_str), "Tx: %ld Mb",
		  (bytes_out / 1048576));
      else if (bytes_out > 1024 && bytes_out < 1048576)
	snprintf (out_str, sizeof (out_str), "Tx: %ld Kb",
		  (bytes_out / 1024));
      else
	snprintf (out_str, sizeof (out_str), "Tx: %ld B", bytes_out);

      edje_object_part_text_set (inst->net_obj, "tx_label", out_str);
    }

  i = 0.0;
  if (bytes_in != 0.0)
    i = ((double) bytes_in / (double) ci->max);

  o = 0.0;
  if (bytes_out != 0.0)
    o = ((double) bytes_out / (double) ci->max);

  if (i < 0.0)
    i = 0.0;
  if (o < 0.0)
    o = 0.0;
  if ((i > 0.0) && (i < 1.0))
    i = 1.0;
  if ((o > 0.0) && (o < 1.0))
    o = 1.0;

  _net_update_rx (inst, i);
  _net_update_tx (inst, o);

  return 1;
}
