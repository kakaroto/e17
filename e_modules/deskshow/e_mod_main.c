/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>5n-3f0^-2{2
 */
#include <e.h>
#include "e_mod_main.h"

/* gadcon requirements */
static E_Gadcon_Client *_gc_init (E_Gadcon * gc, const char *name,
				  const char *id, const char *style);
static void _gc_shutdown (E_Gadcon_Client * gcc);
static void _gc_orient (E_Gadcon_Client * gcc);
static char *_gc_label (void);
static Evas_Object *_gc_icon (Evas * evas);
static int _deskshow_cb_event_desk_show(void *data, int type, void *event);

/* and actually define the gadcon class that this module provides (just 1) */
static const E_Gadcon_Client_Class _gadcon_class = {
  GADCON_CLIENT_CLASS_VERSION,
  "deskshow",
  {
   _gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon}
};

/* actual module specifics */
typedef struct _Instance Instance;
struct _Instance
{
  E_Gadcon_Client *gcc;
  Evas_Object *o_button;
};

static void _button_cb_mouse_down (void *data, Evas * e, Evas_Object * obj,
				   void *event_info);

static E_Module *desk_module = NULL;
Evas_List *handlers;

static E_Gadcon_Client *
_gc_init (E_Gadcon * gc, const char *name, const char *id, const char *style)
{
  Evas_Object *o;
  E_Gadcon_Client *gcc;
  Instance *inst;
  char buf[4096];

  inst = E_NEW (Instance, 1);

  snprintf (buf, sizeof (buf), "%s/deskshow.edj",
	    e_module_dir_get (desk_module));
  o = edje_object_add (gc->evas);
  if (!e_theme_edje_object_set
      (o, "base/theme/modules/deskshow", "modules/deskshow/main"))
    edje_object_file_set (o, buf, "modules/deskshow/main");
  evas_object_show (o);
  edje_object_signal_emit (o, "passive", "");

  gcc = e_gadcon_client_new (gc, name, id, style, o);
  gcc->data = inst;

  inst->gcc = gcc;
  inst->o_button = o;

  e_gadcon_client_util_menu_attach (gcc);

  evas_object_event_callback_add (o, EVAS_CALLBACK_MOUSE_DOWN,
				  _button_cb_mouse_down, inst);
   handlers = evas_list_append(handlers,
	 ecore_event_handler_add(E_EVENT_DESK_SHOW,
	    _deskshow_cb_event_desk_show, inst));
   handlers = evas_list_append(handlers,
	 ecore_event_handler_add(E_EVENT_DESK_DESKSHOW,
	    _deskshow_cb_event_desk_show, inst));
  return gcc;
}

static void
_gc_shutdown (E_Gadcon_Client * gcc)
{
  Instance *inst;

  inst = gcc->data;
  evas_object_del (inst->o_button);
  free (inst);
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
  return _("Deskshow");
}

static Evas_Object *
_gc_icon (Evas * evas)
{
  Evas_Object *o;
  char buf[4096];

  o = edje_object_add (evas);
  snprintf (buf, sizeof (buf), "%s/module.eap",
	    e_module_dir_get (desk_module));
  edje_object_file_set (o, buf, "icon");
  return o;
}

static void
_button_cb_mouse_down (void *data, Evas * e, Evas_Object * obj,
		       void *event_info)
{
  Instance *inst;
  Evas_Event_Mouse_Down *ev;

  inst = data;
  ev = event_info;
  if (ev->button == 1)
    {
      E_Border *bd;
      E_Border_List *bl;
      E_Zone *zone;
      E_Desk *desk;

      zone = e_util_zone_current_get (e_manager_current_get ());
      if (zone)
	{
	   desk = e_desk_current_get(zone);
	   if (desk->deskshow_toggle)
	     edje_object_signal_emit(inst->o_button, "passive", "");
	   else
	     edje_object_signal_emit(inst->o_button, "active", "");
	   e_desk_deskshow(zone);
	}

//      evas_event_feed_mouse_up(inst->gcc->gadcon->evas, ev->button,
//                               EVAS_BUTTON_NONE, ev->timestamp, NULL);
    }
}

static int
_deskshow_cb_event_desk_show(void *data, int type, void *event)
{
   E_Event_Desk_Show *ev;
   E_Desk *desk;
   Instance *inst;

   inst = data;
   ev = event;
   desk = ev->desk;

   if (desk->deskshow_toggle)
     edje_object_signal_emit(inst->o_button, "active", "");
   else
     edje_object_signal_emit(inst->o_button, "passive", "");
}

/* module setup */
EAPI E_Module_Api e_modapi = {
  E_MODULE_API_VERSION,
  "Deskshow"
};

EAPI void *
e_modapi_init (E_Module * m)
{
   desk_module = m;
   e_gadcon_provider_register (&_gadcon_class);
   return desk_module;
}

EAPI int
e_modapi_shutdown (E_Module * m)
{
  desk_module = NULL;
  while (handlers) 
    {
       ecore_event_handler_del(handlers->data);
       handlers = evas_list_remove_list(handlers, handlers);
    }
  e_gadcon_provider_unregister (&_gadcon_class);
  return 1;
}

EAPI int
e_modapi_save (E_Module * m)
{
  return 1;
}

EAPI int
e_modapi_about (E_Module * m)
{
  e_module_dialog_show (m, _("Enlightenment Deskshow Module"),
			_
			("This module will allow you to show your desktop if it is covered with windows."));
  return 1;
}
