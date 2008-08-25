#include <e.h>
/* for complex link stuff */
#include "config.h"
#include <Ecore.h>
#include <E_DBus.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

static E_DBus_Connection *conn = NULL;
static E_DBus_Connection *conn_system = NULL;
static E_DBus_Signal_Handler *changed_h = NULL;
static E_DBus_Signal_Handler *changed_fso_h = NULL;
static E_DBus_Signal_Handler *operatorch_h = NULL;
static E_DBus_Signal_Handler *operatorch_fso_h = NULL;
static E_DBus_Signal_Handler *namech_h = NULL;
static E_DBus_Signal_Handler *namech_system_h = NULL;

static Ecore_Timer *try_again_timer = NULL;
static int success = 0;

/***************************************************************************/
typedef struct _Instance Instance;

struct _Instance
{
   E_Gadcon_Client *gcc;
   Evas_Object *obj;
   int strength;
   char *operator;
};

/***************************************************************************/
/**/
/* gadcon requirements */
static E_Gadcon_Client *_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style);
static void _gc_shutdown(E_Gadcon_Client *gcc);
static void _gc_orient(E_Gadcon_Client *gcc);
static char *_gc_label(void);
static Evas_Object *_gc_icon(Evas *evas);
static const char *_gc_id_new(void);
/* and actually define the gadcon class that this module provides (just 1) */
static const E_Gadcon_Client_Class _gadcon_class =
{
   GADCON_CLIENT_CLASS_VERSION,
     "illume-gsm",
     {
	_gc_init, _gc_shutdown, _gc_orient, _gc_label, _gc_icon, _gc_id_new, NULL
     },
   E_GADCON_CLIENT_STYLE_PLAIN
};
static E_Module *mod = NULL;
/**/
/***************************************************************************/

static int try_again(void *data);
static void *signal_unmarhsall(DBusMessage *msg, DBusError *err);
static void *operator_unmarhsall(DBusMessage *msg, DBusError *err);
static void signal_callback(void *data, void *ret, DBusError *err);
static void operator_callback(void *data, void *ret, DBusError *err);
static void signal_result_free(void *data);
static void operator_result_free(void *data);
static void get_signal(void *data);
static void get_operator(void *data);
static void signal_changed(void *data, DBusMessage *msg);
static void operator_changed(void *data, DBusMessage *msg);
static void name_changed(void *data, DBusMessage *msg);

static int
try_again(void *data)
{
   get_signal(data);
   get_operator(data);
   try_again_timer = 0;
   return 0;
}

/* called from the module core */
void
_e_mod_gad_gsm_init(E_Module *m)
{
   mod = m;
   e_gadcon_provider_register(&_gadcon_class);
}

void
_e_mod_gad_gsm_shutdown(void)
{
   e_gadcon_provider_unregister(&_gadcon_class);
   mod = NULL;
}

/* internal calls */
static Evas_Object *
_theme_obj_new(Evas *e, const char *custom_dir, const char *group)
{
   Evas_Object *o;
   
   o = edje_object_add(e);
   if (!e_theme_edje_object_set(o, "base/theme/modules/illume", group))
     {
	if (custom_dir)
	  {
	     char buf[PATH_MAX];
	     
	     snprintf(buf, sizeof(buf), "%s/illume.edj", custom_dir);
	     if (edje_object_file_set(o, buf, group))
	       {
		  printf("OK FALLBACK %s\n", buf);
	       }
	  }
     }
   return o;
}

static E_Gadcon_Client *
_gc_init(E_Gadcon *gc, const char *name, const char *id, const char *style)
{
   Evas_Object *o;
   E_Gadcon_Client *gcc;
   Instance *inst;
   
   inst = E_NEW(Instance, 1);
   o = _theme_obj_new(gc->evas, e_module_dir_get(mod),
		      "e/modules/illume/gadget/gsm");
   evas_object_show(o);
   gcc = e_gadcon_client_new(gc, name, id, style, o);
   gcc->data = inst;
   inst->gcc = gcc;
   inst->obj = o;
   e_gadcon_client_util_menu_attach(gcc);
   
   inst->strength = -1;
   inst->operator = NULL;
   
   int sleeptime = 8;

   ecore_init();
   ecore_string_init();
   e_dbus_init();
   
   conn = e_dbus_bus_get(DBUS_BUS_SESSION);
   conn_system = e_dbus_bus_get(DBUS_BUS_SYSTEM);

   if (conn)
     {
	namech_h = e_dbus_signal_handler_add(conn,
					     "org.freedesktop.DBus",
					     "/org/freedesktop/DBus",
					     "org.freedesktop.DBus",
					     "NameOwnerChanged",
					     name_changed, inst);
	changed_h = e_dbus_signal_handler_add(conn,
					      "org.openmoko.qtopia.Phonestatus",
					      "/Status",
					      "org.openmoko.qtopia.Phonestatus",
					      "signalStrengthChanged",
					      signal_changed, inst);
	operatorch_h = e_dbus_signal_handler_add(conn,
						 "org.openmoko.qtopia.Phonestatus",
						 "/Status",
						 "org.openmoko.qtopia.Phonestatus",
						 "networkOperatorChanged",
						 operator_changed, inst);
     }
   if (conn_system)
     {
	namech_system_h = e_dbus_signal_handler_add(conn_system,
						    "org.freedesktop.DBus",
						    "/org/freedesktop/DBus",
						    "org.freedesktop.DBus",
						    "NameOwnerChanged",
						    name_changed, inst);
	changed_fso_h = e_dbus_signal_handler_add(conn_system,
						  "org.freesmartphone.ogsmd",
						  "/org/freesmartphone/GSM/Device",
						  "org.freesmartphone.GSM.Network",
						  "SignalStrength",
						  signal_changed, inst);
	operatorch_fso_h = e_dbus_signal_handler_add(conn,
						     "org.freesmartphone.ogsmd",
						     "/org/freesmartphone/GSM/Device",
						     "org.freesmartphone.GSM.Network",
						     "networkOperatorChanged",
						     operator_changed, inst);
     }
   get_signal(inst);
   get_operator(inst);
   
   return gcc;
}

static void
_gc_shutdown(E_Gadcon_Client *gcc)
{
   Instance *inst;
   
   if (conn) e_dbus_connection_close(conn);
   if (conn_system) e_dbus_connection_close(conn_system);
   e_dbus_shutdown();
   ecore_string_shutdown();
   ecore_shutdown();
   
   inst = gcc->data;
   evas_object_del(inst->obj);
   free(inst);
}

static void
_gc_orient(E_Gadcon_Client *gcc)
{
   Instance *inst;
   Evas_Coord mw, mh, mxw, mxh;
   
   inst = gcc->data;
   mw = 0, mh = 0;
   edje_object_size_min_get(inst->obj, &mw, &mh);
   edje_object_size_max_get(inst->obj, &mxw, &mxh);
   if ((mw < 1) || (mh < 1))
     edje_object_size_min_calc(inst->obj, &mw, &mh);
   if (mw < 4) mw = 4;
   if (mh < 4) mh = 4;
   if ((mxw > 0) && (mxh > 0))
     e_gadcon_client_aspect_set(gcc, mxw, mxh);
   e_gadcon_client_min_size_set(gcc, mw, mh);
}

static char *
_gc_label(void)
{
   return "GSM (Illume)";
}

static Evas_Object *
_gc_icon(Evas *evas)
{
/* FIXME: need icon
   Evas_Object *o;
   char buf[4096];
   
   o = edje_object_add(evas);
   snprintf(buf, sizeof(buf), "%s/e-module-clock.edj",
	    e_module_dir_get(clock_module));
   edje_object_file_set(o, buf, "icon");
   return o;
 */
   return NULL;
}

static const char *
_gc_id_new(void)
{
   return _gadcon_class.name;
}

static void
update_operator(char *op, void *data)
{
   Instance *inst = data;
   char *poperator;
   
   poperator = inst->operator;
   strcpy(inst->operator, op);
   
   if (inst->operator != poperator)
     {
	Edje_Message_String msg;
	
	msg.str = inst->operator;
	edje_object_message_send(inst->obj, EDJE_MESSAGE_STRING, 1, &msg);
     }
   if ((poperator) && (inst->operator != poperator))
     free(poperator);
}

static void
update_signal(int signal, void *data)
{
   Instance *inst = data;
   int pstrength;
   
   pstrength = inst->strength;
   inst->strength = signal;

   if (inst->strength != pstrength)
     {
	Edje_Message_Float msg;
	double level;
	
	level = (double)inst->strength / 100.0;
	if (level < 0.0) level = 0.0;
	else if (level > 1.0) level = 1.0;
	msg.val = level;
	edje_object_message_send(inst->obj, EDJE_MESSAGE_FLOAT, 1, &msg);
	if ((pstrength == -1) && (inst->strength >= 0))
	  edje_object_signal_emit(inst->obj, "e,state,active", "e");
	else if ((pstrength >= 0) && (inst->strength == -1))
	  edje_object_signal_emit(inst->obj, "e,state,passive", "e");
     }
}


static void *
signal_unmarhsall(DBusMessage *msg, DBusError *err)
{
   dbus_int32_t str = -1;
   
   if (dbus_message_get_args(msg, NULL, DBUS_TYPE_INT32, &str, DBUS_TYPE_INVALID))
     {
	int *str_ret;
	
	str_ret = malloc(sizeof(int));
	if (str_ret)
	  {
	     *str_ret = str;
	     return str_ret;
	  }
     }
   return NULL;
}

static void *
operator_unmarhsall(DBusMessage *msg, DBusError *err)
{
   const char *str;

   if (dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
     {
	char *str_ret;
	
	str_ret = malloc(strlen(str)+1);
	if (str_ret)
	  {
	     strcpy(str_ret, str);
	     return str_ret;
	  }
     }
   return NULL;
}

static void
signal_callback(void *data, void *ret, DBusError *err)
{
   if (ret)
     {
	if (!success)
	  {
	     if (changed_h)
	       {
		  if (conn)
		    {
		       e_dbus_signal_handler_del(conn, changed_h);
		       changed_h = e_dbus_signal_handler_add(conn,
							     "org.openmoko.qtopia.Phonestatus",
							     "/Status",
							     "org.openmoko.qtopia.Phonestatus",
							     "signalStrengthChanged",
							     signal_changed, data);
		    }
	       }
	     else if (changed_fso_h)
	       {
		  if (conn_system)
		    {
		       e_dbus_signal_handler_del(conn_system, changed_fso_h);
		       changed_fso_h = e_dbus_signal_handler_add(conn_system,
								 "org.freesmartphone.ogsmd",
								 "/org/freesmartphone/GSM/Device",
								 "org.freesmartphone.GSM.Network",
								 "SignalStrength",
								 signal_changed, data);
		    }
	       }
	     success = 1;
	  }
	int *str_ret;
	str_ret = ret;
	update_signal(*str_ret, data);
     }
   else
     {
	success = 0;
	if (try_again_timer) ecore_timer_del(try_again_timer);
	try_again_timer = ecore_timer_add(1.0, try_again, data);
     }
}

static void
operator_callback(void *data, void *ret, DBusError *err)
{
   if (ret)
     {
	if (!success)
	  {
	     if (operatorch_h)
	       {
		  if (conn)
		    {
		       e_dbus_signal_handler_del(conn, operatorch_h);
		       operatorch_h = e_dbus_signal_handler_add(conn,
								"org.openmoko.qtopia.Phonestatus",
								"/Status",
								"org.openmoko.qtopia.Phonestatus",
								"networkOperatorChanged",
								operator_changed, data);
		    }
	       }
	     else if (operatorch_fso_h)
	       {
		  if (conn_system)
		    {
		       e_dbus_signal_handler_del(conn_system, operatorch_h);
		       operatorch_h = e_dbus_signal_handler_add(conn_system,
								"org.freesmartphone.ogsmd",
								"/org/freesmartphone/GSM/Device",
								"org.freesmartphone.GSM.Network",
								"networkOperatorChanged",
								operator_changed, data);
		    }
	       }
	     success = 1;
	  }
	update_operator(ret, data);
     }
   else
     {
	success = 0;
	if (try_again_timer) ecore_timer_del(try_again_timer);
	try_again_timer = ecore_timer_add(1.0, try_again, data);
     }
}

static void
signal_result_free(void *data)
{
   free(data);
}

static void
operator_result_free(void *data)
{
   free(data);
}

static void
get_signal(void *data)
{
   DBusMessage *msg;
   
   if (conn)
     {
	msg = dbus_message_new_method_call("org.openmoko.qtopia.Phonestatus",
					   "/Status",
					   "org.openmoko.qtopia.Phonestatus",
					   "signalStrength");
	if (msg)
	  {
	     e_dbus_method_call_send(conn, msg,
				     signal_unmarhsall,
				     signal_callback,
				     signal_result_free, -1, data);
	     dbus_message_unref(msg);
	  }
     }
   if (conn_system)
     {
	msg = dbus_message_new_method_call("org.freesmartphone.ogsmd",
					   "/org/freesmartphone/GSM/Device",
					   "org.freesmartphone.GSM.Network",
					   "GetSignalStrength");
	if (msg)
	  {
	     e_dbus_method_call_send(conn_system, msg,
				     signal_unmarhsall,
				     signal_callback,
				     signal_result_free, -1, data);
	     dbus_message_unref(msg);
	  }
     }
}

static void
get_operator(void *data)
{
   DBusMessage *msg, *msg2;
   
   if (conn)
     {
	msg = dbus_message_new_method_call("org.openmoko.qtopia.Phonestatus",
					   "/Status",
					   "org.openmoko.qtopia.Phonestatus",
					   "networkOperator");
	if (msg)
	  {
	     e_dbus_method_call_send(conn, msg,
				     operator_unmarhsall,
				     operator_callback,
				     operator_result_free, -1, data);
	     dbus_message_unref(msg);
	  }
     }
   if (conn_system)
     {
	msg = dbus_message_new_method_call("org.freesmartphone.ogsmd",
					   "/org/freesmartphone/GSM/Device",
					   "org.freesmartphone.GSM.Network",
					   "networkOperator");
	if (msg)
	  {
	     e_dbus_method_call_send(conn_system, msg,
				     operator_unmarhsall,
				     operator_callback,
				     operator_result_free, -1, data);
	     dbus_message_unref(msg);
	  }
     }
}

static void
signal_changed(void *data, DBusMessage *msg)
{
   DBusError err;
   dbus_int32_t str = -1;
   
   dbus_error_init(&err);
   if (!dbus_message_get_args(msg, &err, DBUS_TYPE_INT32, &str, DBUS_TYPE_INVALID))
     return;
   update_signal(str, data);
}

static void
operator_changed(void *data, DBusMessage *msg)
{
   DBusError err;
   char *str;
   
   dbus_error_init(&err);
   if (!dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
     return;
   update_operator(str, data);
}

static void
name_changed(void *data, DBusMessage *msg)
{
   DBusError err;
   const char *s1, *s2, *s3;
   
   dbus_error_init(&err);
   if (!dbus_message_get_args(msg, &err,
			      DBUS_TYPE_STRING, &s1,
			      DBUS_TYPE_STRING, &s2,
			      DBUS_TYPE_STRING, &s3,
			      DBUS_TYPE_INVALID))
     return;
   if (!strcmp(s1, "org.openmoko.qtopia.Phonestatus"))
     {
	if (changed_h)
	  {
	     if (conn)
	       {
		  e_dbus_signal_handler_del(conn, changed_h);
		  changed_h = e_dbus_signal_handler_add(conn,
							"org.openmoko.qtopia.Phonestatus",
							"/Status",
							"org.openmoko.qtopia.Phonestatus",
							"signalStrengthChanged",
							signal_changed, data);
		  get_signal(data);
	       }
	  }
	if (operatorch_h)
	  {
	     if (conn)
	       {
		  e_dbus_signal_handler_del(conn, operatorch_h);
		  operatorch_h = e_dbus_signal_handler_add(conn,
							   "org.openmoko.qtopia.Phonestatus",
							   "/Status",
							   "org.openmoko.qtopia.Phonestatus",
							   "networkOperatorChanged",
							   operator_changed, data);
		  get_operator(data);
	       }
	  }
     }
   else if (!strcmp(s1, "org.freesmartphone.ogsmd"))
     {
	if (changed_fso_h)
	  {
	     if (conn_system)
	       {
		  e_dbus_signal_handler_del(conn_system, changed_fso_h);
		  changed_fso_h = e_dbus_signal_handler_add(conn_system,
							    "org.freesmartphone.ogsmd",
							    "/org/freesmartphone/GSM/Device",
							    "org.freesmartphone.GSM.Network",
							    "SignalStrength",
							    signal_changed, data);
		  get_signal(data);
	       }
	  }
	if (operatorch_fso_h)
	  {
	     if (conn_system)
	       {
		  e_dbus_signal_handler_del(conn_system, operatorch_h);
		  operatorch_h = e_dbus_signal_handler_add(conn,
							   "org.freesmartphone.ogsmd",
							   "/org/freesmartphone/GSM/Device",
							   "org.freesmartphone.GSM.Network",
							   "networkOperatorChanged",
							   operator_changed, data);
		  get_operator(data);
	       }
	  }
     }
   return;
}
