#include <e.h>
#include "e_mod_main.h"
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

#define MAX_SONG_LENGTH 255

typedef struct _Instance Instance;
struct _Instance
{
  E_Gadcon_Client *gcc;
  Evas_Object *empris;
  Evas_Object *o_popup;
  Config_Item *ci;
  E_Gadcon_Popup *popup;

  int cur_track;
  
  /* E_DBus_Signal_Handler *cb_tracklist_change = NULL; */
  E_DBus_Signal_Handler *cb_player_track_change;
  E_DBus_Signal_Handler *cb_player_status_change;
};

/* Func Proto Requirements for Gadcon */
static E_Gadcon_Client *_gc_init (E_Gadcon * gc, const char *name,
				  const char *id, const char *style);
static void _gc_shutdown (E_Gadcon_Client * gcc);
static void _gc_orient (E_Gadcon_Client * gcc, E_Gadcon_Orient orient);
static char *_gc_label (E_Gadcon_Client_Class *client_class);
static Evas_Object *_gc_icon (E_Gadcon_Client_Class *client_class, Evas * evas);
static const char *_gc_id_new (E_Gadcon_Client_Class *client_class);

/* Module Protos */
static void _empris_cb_mouse_down (void *data, Evas * e, Evas_Object * obj, void *event_info);
static void _empris_cb_mouse_in (void *data, Evas * e, Evas_Object * obj, void *event_info);
static void _empris_cb_mouse_out (void *data, Evas * e, Evas_Object * obj, void *event_info);
static void _empris_menu_cb_configure (void *data, E_Menu * m, E_Menu_Item * mi);
static void _empris_menu_cb_post (void *data, E_Menu * m);
static void _empris_cb_play (void *data, Evas_Object * obj,  const char *emission, const char *source);
static void _empris_cb_stop (void *data, Evas_Object * obj,  const char *emission, const char *source);
static void _empris_cb_pause (void *data, Evas_Object * obj, const char *emission, const char *source);
static void _empris_cb_next (void *data, Evas_Object * obj,  const char *emission, const char *source);
static void _empris_cb_previous (void *data, Evas_Object * obj, const char *emission, const char *source);
static Config_Item *_empris_config_item_get (const char *id);

static void _set_status(Instance *inst, DBusMessage *msg);
static void _dbus_cb_tracklist_metadata(void *data, DBusMessage *reply, DBusError *error);
static void _dbus_cb_get_status(void *data, DBusMessage *reply, DBusError *error);
static void _dbus_cb_track_change(void *data, DBusMessage *msg);
static void _dbus_cb_current_track(void *data, DBusMessage *reply, DBusError *error);
static void _dbus_cb_list_names(void *data, DBusMessage *msg, DBusError *err);
static void _dbus_cb_name_owner_changed(void *data, DBusMessage *msg);
static void _dbus_cb_status_change(void *data, DBusMessage *msg);
static DBusPendingCall *_dbus_send_msg(const char *path, const char *method, E_DBus_Method_Return_Cb _cb, void *data);

static char * _util_unescape(const char *string, int length);


static E_Config_DD *conf_edd = NULL;
static E_Config_DD *conf_item_edd = NULL;


static E_DBus_Connection *conn = NULL;
static DBusPendingCall *pending_get_name_owner = NULL;
static E_DBus_Signal_Handler *cb_name_owner_changed = NULL;
static const char *bus_name = NULL;
static const char mpris_interface[] = "org.freedesktop.MediaPlayer";
static const char fdo_bus_name[] = "org.freedesktop.DBus";
static const char fdo_interface[] = "org.freedesktop.DBus";
static const char fdo_path[] = "/org/freedesktop/DBus";

static Eina_List *players = NULL;
static Eina_Bool active = EINA_FALSE;

Config *empris_config = NULL;

/* Define the class and gadcon functions this module provides */
static const E_Gadcon_Client_Class _gc_class = {
  GADCON_CLIENT_CLASS_VERSION,
  "empris", {_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon,
	     _gc_id_new, NULL, e_gadcon_site_is_not_toolbar},
  E_GADCON_CLIENT_STYLE_PLAIN
};

static void _empris_popup_destroy (Instance * inst);
static void _empris_popup_create (Instance * inst, const char *dir);


static E_Gadcon_Client *
_gc_init (E_Gadcon * gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Evas *evas;
   Instance *inst;
   char buf[4096];

   inst = E_NEW (Instance, 1);

   inst->ci = _empris_config_item_get (id);
   if (!inst->ci->id)
     inst->ci->id = eina_stringshare_add (id);

   o = edje_object_add (gc->evas);
   snprintf (buf, sizeof (buf), "%s/empris.edj",
	     e_module_dir_get (empris_config->module));
   if (!e_theme_edje_object_set
       (o, "base/theme/modules/empris", "modules/empris/main"))
     edje_object_file_set (o, buf, "modules/empris/main");
   evas_object_show (o);

   gcc = e_gadcon_client_new (gc, name, id, style, o);
   gcc->data = inst;
   inst->gcc = gcc;
   inst->empris = o;

   _empris_popup_create(inst, buf);

   evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
				   _empris_cb_mouse_down, inst);
   evas_object_event_callback_add (inst->empris, EVAS_CALLBACK_MOUSE_IN,
				   _empris_cb_mouse_in, inst);
   evas_object_event_callback_add (inst->empris, EVAS_CALLBACK_MOUSE_OUT,
				   _empris_cb_mouse_out, inst);
   edje_object_signal_callback_add (o, "empris,play", "", _empris_cb_play,
				    inst);
   edje_object_signal_callback_add (o, "empris,stop", "", _empris_cb_stop,
				    inst);
   edje_object_signal_callback_add (o, "empris,pause", "", _empris_cb_pause,
				    inst);
   edje_object_signal_callback_add (o, "empris,next", "", _empris_cb_next,
				    inst);
   edje_object_signal_callback_add (o, "empris,previous", "",
				    _empris_cb_previous, inst);

   inst->cb_player_track_change = e_dbus_signal_handler_add
     (conn, bus_name, "/Player", mpris_interface, "TrackChange",
      _dbus_cb_track_change, inst);

   inst->cb_player_status_change = e_dbus_signal_handler_add
     (conn, bus_name, "/Player", mpris_interface, "StatusChange",
      _dbus_cb_status_change, inst);

   _dbus_send_msg("/TrackList", "GetCurrentTrack", _dbus_cb_current_track, inst);
  
   empris_config->instances = eina_list_append (empris_config->instances, inst);
   return gcc;
}



static void
_gc_shutdown (E_Gadcon_Client * gcc)
{
   Instance *inst;

   inst = gcc->data;

   if (inst->cb_player_track_change)
     e_dbus_signal_handler_del(conn, inst->cb_player_track_change);
   if (inst->cb_player_status_change)
     e_dbus_signal_handler_del(conn, inst->cb_player_status_change);
  
   empris_config->instances = eina_list_remove (empris_config->instances, inst);

   evas_object_event_callback_del (inst->empris, EVAS_CALLBACK_MOUSE_DOWN,
				   _empris_cb_mouse_down);
   evas_object_event_callback_del (inst->empris, EVAS_CALLBACK_MOUSE_IN,
				   _empris_cb_mouse_in);
   evas_object_event_callback_del (inst->empris, EVAS_CALLBACK_MOUSE_OUT,
				   _empris_cb_mouse_out);
   _empris_popup_destroy (inst);
   evas_object_del (inst->empris);
   free (inst);
   inst = NULL;
}

static void
_gc_orient (E_Gadcon_Client * gcc, E_Gadcon_Orient orient)
{
   Instance *inst;
   Evas_Coord mw, mh;

   inst = gcc->data;
   edje_object_size_min_calc (inst->empris, &mw, &mh);
   e_gadcon_client_min_size_set (gcc, mw, mh);
}

static char *
_gc_label (E_Gadcon_Client_Class *client_class)
{
   return D_ ("Empris");
}

static Evas_Object *
_gc_icon (E_Gadcon_Client_Class *client_class, Evas * evas)
{
   Evas_Object *o;
   char buf[4096];

   o = edje_object_add (evas);
   snprintf (buf, sizeof (buf), "%s/e-module-empris.edj",
	     e_module_dir_get (empris_config->module));
   edje_object_file_set (o, buf, "icon");
   return o;
}

static const char *
_gc_id_new (E_Gadcon_Client_Class *client_class)
{
   Config_Item *ci;

   ci = _empris_config_item_get (NULL);
   return ci->id;
}

static void
_empris_cb_mouse_down (void *data, Evas * e, Evas_Object * obj,
		       void *event_info)
{
   Instance *inst;
   Evas_Event_Mouse_Down *ev;

   inst = data;
   ev = event_info;
   if ((ev->button == 3) && (!empris_config->menu))
     {
	E_Menu *m;
	E_Menu_Item *mi;
	int x, y, w, h;

	m = e_menu_new ();

	m = e_gadcon_client_util_menu_items_append (inst->gcc, m, 0);
	e_menu_post_deactivate_callback_set (m, _empris_menu_cb_post, inst);
	empris_config->menu = m;

	mi = e_menu_item_new_relative(m, NULL);
	e_menu_item_label_set (mi, D_ ("Configuration"));
	e_util_menu_item_theme_icon_set(mi, "preferences-system");
	e_menu_item_callback_set (mi, _empris_menu_cb_configure, inst);

	e_gadcon_canvas_zone_geometry_get (inst->gcc->gadcon, &x, &y, &w, &h);
	e_menu_activate_mouse (m,
			       e_util_zone_current_get (e_manager_current_get
							()), x + ev->output.x,
			       y + ev->output.y, 1, 1,
			       E_MENU_POP_DIRECTION_DOWN, ev->timestamp);
	evas_event_feed_mouse_up (inst->gcc->gadcon->evas, ev->button,
				  EVAS_BUTTON_NONE, ev->timestamp, NULL);
     }
   else if (ev->button == 1)
     {
	//e_gadcon_popup_toggle_pinned(inst->popup);
     }
}


static void
_empris_cb_mouse_in (void *data, Evas * e, Evas_Object * obj,
		     void *event_info)
{
   Instance *inst;

   if ((!(inst = data)) || (!inst->ci->show_popup) || (!inst->popup))
     return;

   e_gadcon_popup_show (inst->popup);
}

static void
_empris_cb_mouse_out (void *data, Evas * e, Evas_Object * obj,
		      void *event_info)
{
   Instance *inst;

   if ((!(inst = data)) || ((!inst->ci->show_popup) && (inst->popup)))
     return;

   e_gadcon_popup_hide (inst->popup);
}

static void
_empris_menu_cb_post (void *data, E_Menu * m)
{
   if (!empris_config->menu)
     return;
   e_object_del (E_OBJECT (empris_config->menu));
   empris_config->menu = NULL;
}

static void
_empris_menu_cb_configure (void *data, E_Menu * m, E_Menu_Item * mi)
{
   Instance *inst;

   inst = data;
   _config_empris_module (inst->ci);
}

void
_empris_config_updated (Config_Item * ci)
{
   Eina_List *l;

   if (!empris_config)
     return;
   for (l = empris_config->instances; l; l = l->next)
     {
	Instance *inst;

	inst = l->data;
	if (inst->ci != ci)
	  continue;

	/* TODO choose player */
	
	if (inst->ci->show_popup)
	  {
	     if (!inst->popup)
	       {
		  _empris_popup_create(inst, NULL);
	       }
	  }
	else
	  {
	     if (inst->popup)
	       {
		  _empris_popup_destroy(inst);
	       }
	  }
	break;
     }
}

static void
_empris_cb_play (void *data, Evas_Object * obj, const char *emission, const char *source)
{
   _dbus_send_msg("/Player", "Play", NULL, NULL);
}

static void
_empris_cb_previous (void *data, Evas_Object * obj, const char *emission, const char *source)
{
   _dbus_send_msg("/Player", "Prev", NULL, NULL);
}

static void
_empris_cb_next (void *data, Evas_Object * obj, const char *emission, const char *source)
{
   _dbus_send_msg("/Player", "Next", NULL, NULL);
}

static void
_empris_cb_stop (void *data, Evas_Object * obj, const char *emission, const char *source)
{
   _dbus_send_msg("/Player", "Stop", NULL, NULL);
}

static void
_empris_cb_pause (void *data, Evas_Object * obj, const char *emission, const char *source)
{
   _dbus_send_msg("/Player", "Pause", NULL, NULL);
}

static Config_Item *
_empris_config_item_get (const char *id)
{
   Eina_List *l;
   Config_Item *ci;
   char buf[128];


   if (!id)
     {
	int num = 0;

	/* Create id */
	if (empris_config->items)
	  {
	     const char *p;
	     ci = eina_list_last (empris_config->items)->data;
	     p = strrchr (ci->id, '.');
	     if (p)
	       num = atoi (p + 1) + 1;
	  }
	snprintf (buf, sizeof (buf), "%s.%d", _gc_class.name, num);
	id = buf;
     }
   else
     {
	for (l = empris_config->items; l; l = l->next)
	  {
	     ci = l->data;
	     if (!ci->id)
	       continue;
	     if (!strcmp (ci->id, id))
	       return ci;
	  }
     }

   ci = E_NEW (Config_Item, 1);
   ci->id = eina_stringshare_add (id);
   ci->player = eina_stringshare_add ("");
   ci->show_popup = 1;

   empris_config->items = eina_list_append (empris_config->items, ci);
   return ci;
}

EAPI E_Module_Api e_modapi = {
  E_MODULE_API_VERSION,
  "Empris"
};

EAPI void *
e_modapi_init (E_Module * m)
{
   char buf[4096];

   snprintf (buf, sizeof (buf), "%s/locale", e_module_dir_get (m));
   bindtextdomain (PACKAGE, buf);
   bind_textdomain_codeset (PACKAGE, "UTF-8");

   conf_item_edd = E_CONFIG_DD_NEW ("Empris_Config_Item", Config_Item);
#undef T
#undef D
#define T Config_Item
#define D conf_item_edd
   E_CONFIG_VAL (D, T, id, STR);
   E_CONFIG_VAL (D, T, player, STR);
   E_CONFIG_VAL (D, T, show_popup, UCHAR);

   conf_edd = E_CONFIG_DD_NEW ("Empris_Config", Config);
#undef T
#undef D
#define T Config
#define D conf_edd
   E_CONFIG_LIST (D, T, items, conf_item_edd);

   empris_config = e_config_domain_load ("module.empris", conf_edd);
   if (!empris_config)
     {
	Config_Item *ci;

	empris_config = E_NEW (Config, 1);

	ci = E_NEW (Config_Item, 1);
	ci->id = eina_stringshare_add ("0");
	ci->player = eina_stringshare_add ("");
	ci->show_popup = 1;

	empris_config->items = eina_list_append (empris_config->items, ci);
     }
   empris_config->module = m;

   conn = e_dbus_bus_get(DBUS_BUS_SESSION);

   if (!conn) return NULL;
   
   cb_name_owner_changed = e_dbus_signal_handler_add
     (conn, fdo_bus_name, fdo_path, fdo_interface, "NameOwnerChanged",
      _dbus_cb_name_owner_changed, NULL);

   e_dbus_list_names(conn, _dbus_cb_list_names, NULL);
  
   e_gadcon_provider_register (&_gc_class);

   e_module_delayed_set(m, 1);
  
   return m;
}

EAPI int
e_modapi_shutdown (E_Module * m)
{
   char *player;

   empris_config->module = NULL;
   e_gadcon_provider_unregister (&_gc_class);

   if (conn)
     {
	if (cb_name_owner_changed) e_dbus_signal_handler_del(conn, cb_name_owner_changed);
	e_dbus_connection_close(conn);
     }

   EINA_LIST_FREE(players, player)
     eina_stringshare_del(player);
  
   if (empris_config->config_dialog)
     e_object_del (E_OBJECT (empris_config->config_dialog));
   if (empris_config->menu)
     {
	e_menu_post_deactivate_callback_set (empris_config->menu, NULL, NULL);
	e_object_del (E_OBJECT (empris_config->menu));
	empris_config->menu = NULL;
     }

   while (empris_config->items)
     {
	Config_Item *ci;

	ci = empris_config->items->data;
	empris_config->items =
	  eina_list_remove_list (empris_config->items, empris_config->items);
	if (ci->id)
	  eina_stringshare_del (ci->id);
	free (ci);
	ci = NULL;
     }

   free (empris_config);
   empris_config = NULL;
   E_CONFIG_DD_FREE (conf_item_edd);
   E_CONFIG_DD_FREE (conf_edd);
   return 1;
}

EAPI int
e_modapi_save (E_Module * m)
{
   e_config_domain_save ("module.empris", conf_edd, empris_config);
   return 1;
}

static void
_empris_popup_destroy (Instance * inst)
{
   if (inst->popup)
     e_object_del (E_OBJECT (inst->popup));
   inst->popup = NULL;

   if (inst->o_popup)
     evas_object_del(inst->o_popup);
   inst->o_popup = NULL;
}

static void
_empris_popup_create (Instance * inst, const char *dir)
{
   Evas *evas;
   Evas_Object *o_popup;
   char buf[4096];
  
   if (inst->ci->show_popup)
     {
	inst->popup = e_gadcon_popup_new (inst->gcc);
	evas = inst->popup->win->evas;
	o_popup = edje_object_add (evas);
	if (!e_theme_edje_object_set
	    (o_popup, "base/theme/modules/empris", "modules/empris/popup"))
	  {
	     if (dir)
	       {
		  edje_object_file_set (o_popup, dir, "modules/empris/popup");
	       }
	     else
	       {
		  snprintf(buf, sizeof(buf), "%s/empris.edj",
			   e_module_dir_get(empris_config->module));
		  edje_object_file_set(o_popup, buf, "modules/empris/popup");
	       }
	  }
	evas_object_show (o_popup);
	e_gadcon_popup_content_set (inst->popup, o_popup);
	edje_object_size_min_calc (o_popup, NULL, NULL);
	inst->o_popup = o_popup;
	edje_object_signal_callback_add (o_popup, "empris,play", "",
					 _empris_cb_play, inst);
	edje_object_signal_callback_add (o_popup, "empris,stop", "",
					 _empris_cb_stop, inst);
	edje_object_signal_callback_add (o_popup, "empris,pause", "",
					 _empris_cb_pause, inst);
	edje_object_signal_callback_add (o_popup, "empris,next", "",
					 _empris_cb_next, inst);
	edje_object_signal_callback_add (o_popup, "empris,previous", "",
					 _empris_cb_previous, inst);
     }
   else
     {
	inst->popup = NULL;
	inst->o_popup = NULL;
     }
}


static int
_dbus_check_msg(DBusMessage *reply, DBusError *error)
{
   if (error && dbus_error_is_set(error))
     {
	printf("Error: %s - %s\n", error->name, error->message);
	return 0;
     }
   return (dbus_message_get_type(reply) == DBUS_MESSAGE_TYPE_METHOD_RETURN);
}

static DBusPendingCall *
_dbus_send_msg(const char *path, const char *method,
	       E_DBus_Method_Return_Cb _cb, void *data)
{
   DBusMessage *msg;
   DBusPendingCall *pnd;

   if (!active) return NULL;
  
   msg = dbus_message_new_method_call(bus_name, path,
				      mpris_interface,
				      method);

   pnd = e_dbus_message_send(conn, msg, _cb, -1, data);
   dbus_message_unref(msg);

   return pnd;
}

static DBusPendingCall *
_dbus_send_msg_int(const char *path, const char *method,
		   E_DBus_Method_Return_Cb _cb, void *data, int num)
{
   DBusMessage *msg;
   DBusPendingCall *pnd;

   if (!active) return NULL;
  
   msg = dbus_message_new_method_call(bus_name, path,
				      mpris_interface,
				      method);

   dbus_message_append_args(msg,
			    DBUS_TYPE_INT32, &num,
			    DBUS_TYPE_INVALID);
   pnd = e_dbus_message_send(conn, msg, _cb, -1, data);
   dbus_message_unref(msg);

   return pnd;
}

static void
_dbus_cb_current_track(void *data, DBusMessage *reply, DBusError *error)
{
   Instance *inst = data;
   DBusMessage *msg;
   int num;

   if (!_dbus_check_msg(reply, error)) return;

   dbus_message_get_args(reply, error,
			 DBUS_TYPE_INT32, (dbus_int32_t*) &(num),
			 DBUS_TYPE_INVALID);

   /* XXX inst->pnd*/

   _dbus_send_msg_int("/TrackList", "GetMetadata",
		      _dbus_cb_tracklist_metadata, inst, num);

}

static void
_dbus_cb_status_change(void *data, DBusMessage *msg)
{
   DBusMessageIter iter, array;

   dbus_message_iter_init(msg, &iter);
   
   if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRUCT)
     {
	_set_status(data, msg);
     }
   else if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_INT32)
     {
	/* XXX audacious.. */
	_dbus_send_msg("/Player", "GetStatus", _dbus_cb_get_status, data);
     }

   _dbus_send_msg("/TrackList", "GetCurrentTrack", _dbus_cb_current_track, data);
}

static void
_dbus_cb_track_change(void *data, DBusMessage *msg)
{
   _dbus_cb_tracklist_metadata(data, msg, NULL);
}

static void
_dbus_cb_tracklist_metadata(void *data, DBusMessage *reply, DBusError *error)
{
   DBusMessageIter array, item, iter, iter_val;
   Instance *inst = data;
   int type, cnt = 0;
   char *key, *tmp, *location = NULL;
   int title = 0;
   Evas_Object *empris = inst->empris;
   Evas_Object *o_popup = inst->o_popup;

   if (error)
     {
	if (!_dbus_check_msg(reply, error))
	  {
	     printf("dbus garbage!\n");
	     goto error;
	  }
     }

   edje_object_part_text_set (empris, "empris.artist", "");
   edje_object_part_text_set (o_popup, "empris.artist", "");
   
   edje_object_part_text_set (empris, "empris.title", "");
   edje_object_part_text_set (o_popup, "empris.title", "");

   edje_object_part_text_set (empris, "empris.album", "");
   edje_object_part_text_set (o_popup, "empris.album", "");


   dbus_message_iter_init(reply, &array);
   if(dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_ARRAY)
     {
	dbus_message_iter_recurse(&array, &item);

	while(dbus_message_iter_get_arg_type(&item) == DBUS_TYPE_DICT_ENTRY)
	  {
	     dbus_message_iter_recurse(&item, &iter);
	     if (dbus_message_iter_get_arg_type(&iter) == DBUS_TYPE_STRING)
	       {
		  dbus_message_iter_get_basic(&iter, &key);
	       }
	     else
	       {
		  printf("not string{n");
		  goto error;
	       }
	  
	     dbus_message_iter_next(&iter);
	     if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_VARIANT)
	       {
		  printf("not variant\n");
		  goto error;
	       }
	  
	     if (!strcmp(key, "artist"))
	       {
		  dbus_message_iter_recurse (&iter, &iter_val);
		  dbus_message_iter_get_basic (&iter_val, &tmp);
		  if (tmp && tmp[0])
		    {
		       edje_object_part_text_set (empris, "empris.artist", tmp);
		       edje_object_part_text_set (o_popup, "empris.artist", tmp);
		    }
	       }
	     else if (!strcmp(key, "title"))
	       {
		  dbus_message_iter_recurse (&iter, &iter_val);
		  dbus_message_iter_get_basic (&iter_val, &tmp);
		  if (tmp && tmp[0])
		    {
		       edje_object_part_text_set (empris, "empris.title", tmp);
		       edje_object_part_text_set (o_popup, "empris.title", tmp);
		       title = 1;
		    }
	       }
	     else if (!strcmp(key, "location"))
	       {
		  dbus_message_iter_recurse (&iter, &iter_val);
		  dbus_message_iter_get_basic (&iter_val, &tmp);
		  if (tmp && tmp[0])
		    {
		       location = strdup(tmp);
		    }	     
	       }
	     else if (!strcmp(key, "album"))
	       {
		  dbus_message_iter_recurse (&iter, &iter_val);
		  dbus_message_iter_get_basic (&iter_val, &tmp);
		  if (tmp && tmp[0])
		    {
		       edje_object_part_text_set (empris, "empris.album", tmp);
		       edje_object_part_text_set (o_popup, "empris.album", tmp);		  
		    }
	       }
	     /* else if (!strcmp(key, "mtime"))
	      *   {
	      *     dbus_message_iter_recurse (&iter, &iter_val);
	      *     dbus_message_iter_get_basic (&iter_val, &(t->length));
	      *   } */
	     dbus_message_iter_next(&item);
	  }
     }

   if (!title && location)
     {
	char *tmp = _util_unescape(ecore_file_file_get(location), 0);

	if (tmp)
	  {
	     edje_object_part_text_set (empris, "empris.title", tmp);
	     edje_object_part_text_set (o_popup, "empris.title", tmp);
	     free(tmp);
	  }
	free(location);
     }
   
 error:
   return;
}


static void
_set_status(Instance *inst, DBusMessage *msg)
{
   DBusMessageIter iter, array;
   int status;
  
   dbus_message_iter_init(msg, &iter);

   if (dbus_message_iter_get_arg_type(&iter) != DBUS_TYPE_STRUCT)
     {
	edje_object_part_text_set (inst->empris, "empris.status",
				   D_ ("Unknown"));
	edje_object_part_text_set (inst->o_popup, "empris.status",
				   D_ ("Unknown"));

	printf("no dbus struct\n");
	return;
     }

   dbus_message_iter_recurse(&iter, &array);
   dbus_message_iter_get_basic(&array, &status);
   /* dbus_message_iter_next(&array); */
   /* dbus_message_iter_get_basic(&array, &(p->status.random));
    * dbus_message_iter_next(&array);
    * dbus_message_iter_get_basic(&array, &(p->status.repeat));
    * dbus_message_iter_next(&array);
    * dbus_message_iter_get_basic(&array, &(p->status.loop));
    * DBG("status %d", p->status.playing); */

   if (status == 0)
     {
	edje_object_part_text_set (inst->empris, "empris.status",
				   D_ ("Stopped"));
	edje_object_part_text_set (inst->o_popup, "empris.status",
				   D_ ("Stopped"));
     }
   else if (status == 2)
     {
	edje_object_part_text_set (inst->empris, "empris.status",
				   D_ ("Playing"));
	edje_object_part_text_set (inst->o_popup, "empris.status",
				   D_ ("Playing"));
     }
   else if (status == 1)
     {
	edje_object_part_text_set (inst->empris, "empris.status",
				   D_ ("Paused"));
	edje_object_part_text_set (inst->o_popup, "empris.status",
				   D_ ("Paused"));
     }
   else
     {
	edje_object_part_text_set (inst->empris, "empris.status",
				   D_ ("Unknown"));
	edje_object_part_text_set (inst->o_popup, "empris.status",
				   D_ ("Unknown"));
     }
}

static void
_dbus_cb_get_status(void *data, DBusMessage *reply, DBusError *error)
{
   if (!_dbus_check_msg(reply, error)) return;

   _set_status(data, reply);
}

static void
_dbus_cb_name_owner_changed(void *data, DBusMessage *msg)
{
   DBusError err;
   Eina_List *l;
   const char *tmp;
   const char *name, *from, *to;

   if (!conn) return;

   dbus_error_init(&err);
   if (!dbus_message_get_args(msg, &err,
			      DBUS_TYPE_STRING, &name,
			      DBUS_TYPE_STRING, &from,
			      DBUS_TYPE_STRING, &to,
			      DBUS_TYPE_INVALID))
     {
	printf("could not get NameOwnerChanged arguments: %s: %s\n",
	    err.name, err.message);
	dbus_error_free(&err);
	return;
     }

   if (strncmp(name, "org.mpris.", 10) != 0)
     return;

   printf("NameOwnerChanged from=[%s] to=[%s]\n", from, to);

   tmp = eina_stringshare_add(name);

   if (to[0] == '\0')
     {
	players = eina_list_remove(players, tmp);

	/* vanished player was current? */
	if (tmp == bus_name)
	  {
	     /* make another player current */
	     if (eina_list_count(players) > 0)
	       {
		  bus_name = players->data;
		  printf("use::%s", bus_name);
		  active = EINA_TRUE;
	       }
	     else
	       {
		  active = EINA_FALSE;
	       }
	  }

	eina_stringshare_del(tmp);
     }
   else
     {
	/* new player appeared? */
	if (!eina_list_data_find(players, tmp))
	  {
	     eina_stringshare_ref(tmp);
	     players = eina_list_append(players, tmp);
	  }

	/* no active player - make player current */
	if (!active)
	  {
	     bus_name = tmp;
	     active = EINA_TRUE;
	  }
     }

   eina_stringshare_del(tmp);
}

static void
_dbus_cb_list_names(void *data, DBusMessage *msg, DBusError *err)
{
   DBusMessageIter array, iter, item, iter_val;
   char *name;

   if (!_dbus_check_msg(msg, err)) return;

   dbus_message_iter_init(msg, &array);
   if(dbus_message_iter_get_arg_type(&array) == DBUS_TYPE_ARRAY)
     {
	dbus_message_iter_recurse(&array, &item);

	while(dbus_message_iter_get_arg_type(&item) == DBUS_TYPE_STRING)
	  {
	     dbus_message_iter_get_basic(&item, &name);
	     if (strncmp(name, "org.mpris.", 10) == 0)
	       {
		  printf("found %s", name);

		  players = eina_list_append(players, eina_stringshare_add(name));
	       }

	     dbus_message_iter_next(&item);
	  }
     }

   if (eina_list_count(players) > 0)
     {
	bus_name = players->data;
	printf("use::%s", bus_name);
	active = EINA_TRUE;

	
     }
}


/* taken from curl:
 *
 * Copyright (C) 1998 - 2010, Daniel Stenberg, <daniel@haxx.se>, et
 * al.
 *
 * Unescapes the given URL escaped string of given length. Returns a
 * pointer to a malloced string with length given in *olen.
 * If length == 0, the length is assumed to be strlen(string).
 * If olen == NULL, no output length is stored.
 */
#define ISXDIGIT(x) (isxdigit((int) ((unsigned char)x)))

static char *
_util_unescape(const char *string, int length)
{
   int alloc = (length?length:(int)strlen(string))+1;
   char *ns = malloc(alloc);
   unsigned char in;
   int strindex=0;
   unsigned long hex;

   if( !ns )
     return NULL;

   while(--alloc > 0) {
      in = *string;
      if(('%' == in) && ISXDIGIT(string[1]) && ISXDIGIT(string[2])) {
	 /* this is two hexadecimal digits following a '%' */
	 char hexstr[3];
	 char *ptr;
	 hexstr[0] = string[1];
	 hexstr[1] = string[2];
	 hexstr[2] = 0;

	 hex = strtoul(hexstr, &ptr, 16);
	 in = (unsigned char)(hex & (unsigned long) 0xFF);
	 // in = ultouc(hex); /* this long is never bigger than 255 anyway */

	 string+=2;
	 alloc-=2;
      }

      ns[strindex++] = in;
      string++;
   }
   ns[strindex]=0; /* terminate it */

   return ns;
}

#undef ISXDIGIT
