#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "private.h"
#include <E_DBus.h>

#define DBUS_NAME "org.enlightenment.enjoy"
#define DBUS_IFACE "org.enlightenment.enjoy.Control"
#define DBUS_PATH "/org/enlightenment/enjoy/Control"

static E_DBus_Connection *conn = NULL;
static E_DBus_Object *dbus_obj = NULL;
static E_DBus_Interface *dbus_iface = NULL;

typedef struct _Enjoy_DBus_Method Enjoy_DBus_Method;
struct _Enjoy_DBus_Method {
   const char *name;
   const char *par;
   const char *ret;
   E_DBus_Method_Cb cb;
};

static DBusMessage *
_cb_dbus_quit(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   enjoy_quit();
   return dbus_message_new_method_return(msg);
}

static DBusMessage *
_cb_dbus_version(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply = dbus_message_new_method_return(msg);
   DBusMessageIter iter, siter;
   dbus_message_iter_init_append(reply, &iter);
   dbus_message_iter_open_container(&iter, DBUS_TYPE_STRUCT, NULL, &siter);

#define APPEND_UINT16(val)                                              \
   do {                                                                 \
      unsigned short _tmp_val = val;                                    \
      dbus_message_iter_append_basic(&siter, DBUS_TYPE_UINT16, &_tmp_val); \
   } while (0)
   APPEND_UINT16(VMAJ);
   APPEND_UINT16(VMIN);
   APPEND_UINT16(VMIC);
#undef APPEND_UINT16

   dbus_message_iter_close_container(&iter, &siter);
   return reply;
}

/* Avoid duplicating MPRIS -- see src/plugins/mpris */
static const Enjoy_DBus_Method control_methods[] = {
  {"Quit", "", "", _cb_dbus_quit},
  {"Version", "", "(qqq)", _cb_dbus_version},
  /* TODO: DB management */
  {NULL, NULL, NULL, NULL}
};

static void
_dbus_methods_add(E_DBus_Interface *iface, const Enjoy_DBus_Method desc[])
{
   const Enjoy_DBus_Method *itr = desc;
   for (; itr->name; itr++)
     e_dbus_interface_method_add(iface, itr->name, itr->par, itr->ret, itr->cb);
}

static void
_cb_dbus_request_name(void *data __UNUSED__, DBusMessage *msg, DBusError *err)
{
   DBusError new_err;
   dbus_uint32_t msgtype;
   E_DBus_Interface *iface;

   if (dbus_error_is_set(err))
     {
        ERR("Could not get DBus name: %s", err->message);
        goto error;
     }

   dbus_error_init(&new_err);
   dbus_message_get_args
     (msg, &new_err, DBUS_TYPE_UINT32, &msgtype, DBUS_TYPE_INVALID);
   if (msgtype != DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER)
     {
        ERR("Could not get the DBus name: reply=%d", msgtype);
        goto error;
     }

   INF("Got DBus name - unique instance running.");

   dbus_obj = e_dbus_object_add(conn, DBUS_PATH, NULL);
   if (!dbus_obj)
     {
        ERR("Could not create Control DBus object.");
        goto error;
     }
   dbus_iface = e_dbus_interface_new(DBUS_IFACE);
   e_dbus_object_interface_attach(dbus_obj, dbus_iface);
   _dbus_methods_add(dbus_iface, control_methods);

   /* will run after other events run, in the main loop */
   ecore_event_add(ENJOY_EVENT_STARTED, NULL, NULL, NULL);
   return;

 error:
   ecore_main_loop_quit();
   return;
}

Eina_Bool
enjoy_dbus_init(void)
{
   e_dbus_init();
   conn = e_dbus_bus_get(DBUS_BUS_SESSION);
   if (!conn)
     {
        ERR("Could not get DBus session bus");
        return EINA_FALSE;
     }

   e_dbus_request_name
     (conn, DBUS_NAME, DBUS_NAME_FLAG_DO_NOT_QUEUE,
      _cb_dbus_request_name, NULL);
}

void
enjoy_dbus_shutdown(void)
{
   if (dbus_obj) e_dbus_object_free(dbus_obj);
   if (dbus_iface) e_dbus_interface_unref(dbus_iface);
   conn = NULL;
   e_dbus_shutdown();
}
