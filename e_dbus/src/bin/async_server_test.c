#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Ecore.h>
#include "E_DBus.h"

#define DBUS_NAME "com.profusion"
#define OBJECT_PATH "/com/profusion/test"
#define IFACE_NAME "com.Profusion.Test"

static E_DBus_Connection *conn = NULL;
static E_DBus_Object *obj_path = NULL;

typedef struct _EDBus_Method
  {
     char *member;
     char *signature;
     char *reply_signature;
     E_DBus_Method_Cb func;
  } EDBus_Method;

static void
obj_register(char *path_name, char *iface_name, EDBus_Method *methods)
{
   obj_path = e_dbus_object_add(conn, path_name, NULL);
   E_DBus_Interface *iface = e_dbus_interface_new(iface_name);
   const EDBus_Method *_method;

   e_dbus_object_interface_attach(obj_path, iface);
   e_dbus_interface_unref(iface);

   for (_method = methods; _method != NULL && _method->member != NULL; _method++)
     e_dbus_interface_method_add(iface, _method->member,
                                        _method->signature,
                                        _method->reply_signature,
                                        _method->func);
}

static void
_dbus_error_check(DBusError *error)
{
   if (dbus_error_is_set(error))
     {
        printf("dbus error\nName: %s\nDescription: %s\n", error->name,
               error->message);
        ecore_main_loop_quit();
     }
}

Eina_Bool
_resp_async(void *data)
{
   DBusMessage *msg = data;
   DBusMessage *reply;
   DBusError new_err;
   char *string;
   int size;

   dbus_error_init(&new_err);
   dbus_message_get_args(msg, &new_err, DBUS_TYPE_STRING, &string,
   DBUS_TYPE_INVALID);
   _dbus_error_check(&new_err);

   reply = dbus_message_new_method_return(msg);
   size = strlen(string);
   dbus_message_append_args(reply, DBUS_TYPE_INT32, &size, DBUS_TYPE_INVALID);
   e_dbus_message_send(conn, reply, NULL, -1, NULL);

   dbus_message_unref(msg);
   dbus_message_unref(reply);

   return ECORE_CALLBACK_CANCEL;
}

static DBusMessage *
_async(E_DBus_Object *obj, DBusMessage *msg)
{
   dbus_message_ref(msg);
   printf("received a string_len_async call\n");
   printf("response will be send in 5 seconds\n");
   ecore_timer_add (5, _resp_async, msg);
   return NULL;
}

static void
_cb_dbus_request_name(void *context, DBusMessage *msg, DBusError *err)
{
   DBusError new_err;
   dbus_uint32_t msgtype;
   EDBus_Method table_methods[] =
     {
        { "string_len_async", "s", "d", _async},
        { NULL, NULL, NULL, NULL }
     };

   _dbus_error_check(err);
   dbus_error_init(&new_err);
   dbus_message_get_args(msg, &new_err, DBUS_TYPE_UINT32, &msgtype,
                         DBUS_TYPE_INVALID);
   if (msgtype != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
     {
        printf("Could not get the DBus name: reply=%d", msgtype);
        ecore_main_loop_quit();
     }

   obj_register(OBJECT_PATH, IFACE_NAME, table_methods);
}

int
main(int argc, char *argv[])
{
   e_dbus_init();

   conn = e_dbus_bus_get(DBUS_BUS_SESSION);

   e_dbus_request_name(conn, DBUS_NAME, DBUS_NAME_FLAG_DO_NOT_QUEUE,
                       _cb_dbus_request_name, NULL);

   ecore_main_loop_begin();

   e_dbus_shutdown();
   return 0;
}
