/* for complex link stuff */
#include "config.h"
#include <Ecore.h>
#ifdef HAVE_EDBUS
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

static int try_again(void *data);
static void *signal_unmarhsall(DBusMessage *msg, DBusError *err);
static void *operator_unmarhsall(DBusMessage *msg, DBusError *err);
static void signal_callback(void *data, void *ret, DBusError *err);
static void operator_callback(void *data, void *ret, DBusError *err);
static void signal_result_free(void *data);
static void operator_result_free(void *data);
static void get_signal(void);
static void get_operator(void);
static void signal_changed(void *data, DBusMessage *msg);
static void operator_changed(void *data, DBusMessage *msg);
static void name_changed(void *data, DBusMessage *msg);

static int
try_again(void *data)
{
   get_signal();
   get_operator();
   try_again_timer = 0;
   return 0;
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
		  e_dbus_signal_handler_del(conn, changed_h);
		  changed_h = e_dbus_signal_handler_add(conn,
							"org.openmoko.qtopia.Phonestatus",
							"/Status",
							"org.openmoko.qtopia.Phonestatus",
							"signalStrengthChanged",
							signal_changed, NULL);
	       }
	     else if (changed_fso_h)
	       {
		  e_dbus_signal_handler_del(conn_system, changed_fso_h);
		  changed_fso_h = e_dbus_signal_handler_add(conn_system,
							    "org.freesmartphone.ogsmd",
							    "/org/freesmartphone/GSM/Device",
							    "org.freesmartphone.GSM.Network",
							    "SignalStrength",
							    signal_changed, NULL);
	       }
	     success = 1;
	  }
	int *str_ret;
	str_ret = ret;
	printf("S%i\n", *str_ret);
	fflush(stdout);
     }
   else
     {
	success = 0;
	if (try_again_timer) ecore_timer_del(try_again_timer);
	try_again_timer = ecore_timer_add(1.0, try_again, NULL);
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
		  e_dbus_signal_handler_del(conn, operatorch_h);
		  operatorch_h = e_dbus_signal_handler_add(conn,
							   "org.openmoko.qtopia.Phonestatus",
							   "/Status",
							   "org.openmoko.qtopia.Phonestatus",
							   "networkOperatorChanged",
							   operator_changed, NULL);
	       }
	     else if (operatorch_fso_h)
	       {
		  e_dbus_signal_handler_del(conn, operatorch_h);
		  operatorch_h = e_dbus_signal_handler_add(conn,
							   "org.freesmartphone.ogsmd",
							   "/org/freesmartphone/GSM/Device",
							   "org.freesmartphone.GSM.Network",
							   "networkOperatorChanged",
							   operator_changed, NULL);
	       }
	     success = 1;
	  }
	printf("O%s\n", ret);
	fflush(stdout);
     }
   else
     {
	success = 0;
	if (try_again_timer) ecore_timer_del(try_again_timer);
	try_again_timer = ecore_timer_add(1.0, try_again, NULL);
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
get_signal(void)
{
   DBusMessage *msg, *msg2;
   
   if (!conn) return;
   msg = dbus_message_new_method_call("org.openmoko.qtopia.Phonestatus", 
				      "/Status", 
				      "org.openmoko.qtopia.Phonestatus", 
				      "signalStrength");
   if (!conn_system) return;
   msg2 = dbus_message_new_method_call("org.freesmartphone.ogsmd",
				       "/org/freesmartphone/GSM/Device",
				       "org.freesmartphone.GSM.Network",
				       "GetSignalStrength");
   if (!msg || !msg2) return;
   e_dbus_method_call_send(conn, msg,
			   signal_unmarhsall,
			   signal_callback,
			   signal_result_free, -1, NULL);
   e_dbus_method_call_send(conn_system, msg2,
			   signal_unmarhsall,
			   signal_callback,
			   signal_result_free, -1, NULL);
   dbus_message_unref(msg);
   dbus_message_unref(msg2);
}

static void
get_operator(void)
{
   DBusMessage *msg, *msg2;
   
   if (!conn) return;
   msg = dbus_message_new_method_call("org.openmoko.qtopia.Phonestatus", 
				      "/Status", 
				      "org.openmoko.qtopia.Phonestatus", 
				      "networkOperator");
   if (!conn_system) return;
   msg2 = dbus_message_new_method_call("org.freesmartphone.ogsmd",
				       "/org/freesmartphone/GSM/Device",
				       "org.freesmartphone.GSM.Network",
				       "networkOperator");
   if (!msg || !msg2) return;
   e_dbus_method_call_send(conn, msg,
			   operator_unmarhsall,
			   operator_callback,
			   operator_result_free, -1, NULL);
   e_dbus_method_call_send(conn_system, msg2,
			   operator_unmarhsall,
			   operator_callback,
			   operator_result_free, -1, NULL);
   dbus_message_unref(msg);
   dbus_message_unref(msg2);
}

static void
signal_changed(void *data, DBusMessage *msg)
{
   DBusError err;
   dbus_int32_t str = -1;
   
   dbus_error_init(&err);
   if (!dbus_message_get_args(msg, &err, DBUS_TYPE_INT32, &str, DBUS_TYPE_INVALID))
     return;
   printf("S%i\n", str);
   fflush(stdout);
}

static void
operator_changed(void *data, DBusMessage *msg)
{
   DBusError err;
   const char *str;
   
   dbus_error_init(&err);
   if (!dbus_message_get_args(msg, &err, DBUS_TYPE_STRING, &str, DBUS_TYPE_INVALID))
     return;
   printf("O%s\n", str);
   fflush(stdout);
}

static void
name_changed(void *data, DBusMessage *msg)
{
   DBusError err;
//   dbus_int32_t str = -1;
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
	     e_dbus_signal_handler_del(conn, changed_h);
	     changed_h = e_dbus_signal_handler_add(conn,
						   "org.openmoko.qtopia.Phonestatus",
						   "/Status",
						   "org.openmoko.qtopia.Phonestatus",
						   "signalStrengthChanged",
						   signal_changed, NULL);
	     get_signal();
	  }
	if (operatorch_h)
	  {
	     e_dbus_signal_handler_del(conn, operatorch_h);
	     operatorch_h = e_dbus_signal_handler_add(conn,
						      "org.openmoko.qtopia.Phonestatus",
						      "/Status",
						      "org.openmoko.qtopia.Phonestatus",
						      "networkOperatorChanged",
						      operator_changed, NULL);
	     get_operator();
	  }
     }
   else if (!strcmp(s1, "org.freesmartphone.ogsmd"))
     {
	if (changed_fso_h)
	  {
	     e_dbus_signal_handler_del(conn_system, changed_fso_h);
	     changed_fso_h = e_dbus_signal_handler_add(conn_system,
						       "org.freesmartphone.ogsmd",
						       "/org/freesmartphone/GSM/Device",
						       "org.freesmartphone.GSM.Network",
						       "SignalStrength",
						       signal_changed, NULL);
	     get_signal();
	  }
	if (operatorch_fso_h)
	  {
	     e_dbus_signal_handler_del(conn, operatorch_h);
	     operatorch_h = e_dbus_signal_handler_add(conn,
						      "org.freesmartphone.ogsmd",
						      "/org/freesmartphone/GSM/Device",
						      "org.freesmartphone.GSM.Network",
						      "networkOperatorChanged",
						      operator_changed, NULL);
	     get_operator();
	  }
     }
   return;
}
#endif

int
main(int argc, char **argv)
{
#ifdef HAVE_EDBUS
   int sleeptime = 8;

   ecore_init();
   ecore_string_init();
   ecore_app_args_set(argc, (const char **)argv);
   e_dbus_init();
   
   if (argc > 1) sleeptime = atoi(argv[1]);
   conn = e_dbus_bus_get(DBUS_BUS_SESSION);
   conn_system = e_dbus_bus_get(DBUS_BUS_SYSTEM);
  
   namech_h = e_dbus_signal_handler_add(conn,
					"org.freedesktop.DBus",
					"/org/freedesktop/DBus",
					"org.freedesktop.DBus",
					"NameOwnerChanged",
					name_changed, NULL);
   namech_system_h = e_dbus_signal_handler_add(conn_system,
					       "org.freedesktop.DBus",
					       "/org/freedesktop/DBus",
					       "org.freedesktop.DBus",
					       "NameOwnerChanged",
					       name_changed, NULL);
   changed_h = e_dbus_signal_handler_add(conn,
					 "org.openmoko.qtopia.Phonestatus",
					 "/Status",
					 "org.openmoko.qtopia.Phonestatus",
					 "signalStrengthChanged",
					 signal_changed, NULL);
   operatorch_h = e_dbus_signal_handler_add(conn,
					    "org.openmoko.qtopia.Phonestatus",
					    "/Status",
					    "org.openmoko.qtopia.Phonestatus",
					    "networkOperatorChanged",
					    operator_changed, NULL);
   changed_fso_h = e_dbus_signal_handler_add(conn_system,
					     "org.freesmartphone.ogsmd",
					     "/org/freesmartphone/GSM/Device",
					     "org.freesmartphone.GSM.Network",
					     "SignalStrength",
					     signal_changed, NULL);
   operatorch_fso_h = e_dbus_signal_handler_add(conn,
						"org.freesmartphone.ogsmd",
						"/org/freesmartphone/GSM/Device",
						"org.freesmartphone.GSM.Network",
						"networkOperatorChanged",
						operator_changed, NULL);
   
   get_signal();
   get_operator();
   
   ecore_main_loop_begin();
   
   if (conn) e_dbus_connection_close(conn);
   if (conn_system) e_dbus_connection_close(conn_system);
   e_dbus_shutdown();
   ecore_string_shutdown();
   ecore_shutdown();
#endif   
   return 0;
}
