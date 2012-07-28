#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Ecore.h>
#include "E_DBus.h"

#define DBUS_NAME "com.profusion"
#define OBJECT_PATH "/com/profusion/test"
#define IFACE_NAME "com.Profusion.Test"

static E_DBus_Connection *conn = NULL;

static void
_cb_resp(void *data, DBusMessage *msg, DBusError *error)
{
   DBusError new_error;
   int size;

   if (dbus_error_is_set(error))
     {
        printf("dbus error\nName: %s\nDescription: %s\n", error->name,
               error->message);
        ecore_main_loop_quit();
        return;
     }

   dbus_error_init(&new_error);
   dbus_message_get_args(msg, &new_error, DBUS_TYPE_INT32, &size,
                         DBUS_TYPE_INVALID);
   if (dbus_error_is_set(&new_error))
     printf("dbus error\nName: %s\nDescription: %s\n", new_error.name,
            new_error.message);
   else printf("size = %d\n", size);

   ecore_main_loop_quit();
}

int
main(int argc, char *argv[])
{
   char *string = "lalala";
   DBusMessage *msg;

   e_dbus_init();
   conn = e_dbus_bus_get(DBUS_BUS_SESSION);

   msg = dbus_message_new_method_call(DBUS_NAME, OBJECT_PATH,
                                      IFACE_NAME, "string_len_async");
   dbus_message_append_args(msg, DBUS_TYPE_STRING, &string, DBUS_TYPE_INVALID);
   e_dbus_message_send(conn, msg, _cb_resp, -1, NULL);
   dbus_message_unref(msg);

   ecore_main_loop_begin();

   e_dbus_shutdown();
   return 0;
}
