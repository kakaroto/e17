#include <Eina.h>
#include <E_DBus.h>
#include <Ecore.h>
#include "plugin.h"

static int _fso_log_domain = -1;

#ifdef CRITICAL
#undef CRITICAL
#endif
#ifdef ERR
#undef ERR
#endif
#ifdef WRN
#undef WRN
#endif
#ifdef INF
#undef INF
#endif
#ifdef DBG
#undef DBG
#endif

#define CRITICAL(...) EINA_LOG_DOM_CRIT(_fso_log_domain, __VA_ARGS__)
#define ERR(...)      EINA_LOG_DOM_ERR(_fso_log_domain, __VA_ARGS__)
#define WRN(...)      EINA_LOG_DOM_WARN(_fso_log_domain, __VA_ARGS__)
#define INF(...)      EINA_LOG_DOM_INFO(_fso_log_domain, __VA_ARGS__)
#define DBG(...)      EINA_LOG_DOM_DBG(_fso_log_domain, __VA_ARGS__)

#define FSO_OUSAGED_SERVICE "org.freesmartphone.ousaged"
#define FSO_OUSAGED_OBJECT_PATH "/org/freesmartphone/Usage"
#define FSO_OUSAGED_INTERFACE "org.freesmartphone.Usage"

static E_DBus_Connection *conn = NULL;

typedef struct _FSO_Cb_Data
{
   void (*func)(void *data, Eina_Bool error);
   void *data;
} FSO_Cb_Data;

static void
fso_request_resource_cb(void *data, DBusMessage *replymsg __UNUSED__, DBusError *error)
{
   FSO_Cb_Data *d = data;
   Eina_Bool e = EINA_FALSE;

   DBG("Request sent to fsousaged to enable resource.");

   if (error && dbus_error_is_set(error))
     {
        ERR("Error requesting FSO resource: %s - %s",
            error->name, error->message);
        e = EINA_TRUE;
     }

   if ((d) && (d->func))
     d->func(d->data, e);
   free(d);
}

static void
fso_release_resource_cb(void *data, DBusMessage *replymsg __UNUSED__, DBusError *error)
{
   FSO_Cb_Data *d = data;
   Eina_Bool e = EINA_FALSE;

   DBG("Request sent to fsousaged to disable resource.");

   if (error && dbus_error_is_set(error))
     {
        ERR("Error releasing FSO resource: %s - %s",
            error->name, error->message);
        e = EINA_TRUE;
     }

   if ((d) && (d->func))
     d->func(d->data, e);
   free(d);
}

static void
fso_request_resource(const char *resource, void (*func)(void *data, Eina_Bool error), const void *data)
{
   FSO_Cb_Data *d = NULL;
   DBusMessage *msg = dbus_message_new_method_call
     (FSO_OUSAGED_SERVICE, FSO_OUSAGED_OBJECT_PATH, FSO_OUSAGED_INTERFACE,
      "RequestResource");
   dbus_message_append_args
     (msg, DBUS_TYPE_STRING, &resource, DBUS_TYPE_INVALID);

   if (func)
     {
        d = malloc(sizeof(FSO_Cb_Data));
        if (d)
          {
             d->func = func;
             d->data = (void *)data;
          }
     }

   e_dbus_message_send(conn, msg, fso_request_resource_cb, -1, d);
   dbus_message_unref(msg);
}


static void
fso_release_resource(const char *resource, void (*func)(void *data, Eina_Bool error), const void *data)
{
   FSO_Cb_Data *d = NULL;
   DBusMessage *msg = dbus_message_new_method_call
     (FSO_OUSAGED_SERVICE, FSO_OUSAGED_OBJECT_PATH, FSO_OUSAGED_INTERFACE,
      "ReleaseResource");
   dbus_message_append_args
     (msg, DBUS_TYPE_STRING, &resource, DBUS_TYPE_INVALID);

   if (func)
     {
        d = malloc(sizeof(FSO_Cb_Data));
        if (d)
          {
             d->func = func;
             d->data = (void *)data;
          }
     }

   e_dbus_message_send(conn, msg, fso_release_resource_cb, -1, d);
   dbus_message_unref(msg);
}

static Eina_Bool
fso_enable(Enjoy_Plugin *p __UNUSED__)
{
   fso_request_resource("CPU", NULL, NULL);
   return EINA_TRUE;
}

static void
_cb_fso_release_resource_done(void *data __UNUSED__, Eina_Bool error __UNUSED__)
{
   enjoy_quit_thaw();
}

static Eina_Bool
fso_disable(Enjoy_Plugin *p __UNUSED__)
{
   enjoy_quit_freeze();
   fso_release_resource("CPU", _cb_fso_release_resource_done, NULL);
   return EINA_TRUE;
}

static const Enjoy_Plugin_Api api = {
  ENJOY_PLUGIN_API_VERSION,
  fso_enable,
  fso_disable
};

static Eina_Bool
fso_init(void)
{
   if (_fso_log_domain < 0)
     {
        _fso_log_domain = eina_log_domain_register
          ("enjoy-fso", EINA_COLOR_LIGHTCYAN);
        if (_fso_log_domain < 0)
          {
             EINA_LOG_CRIT("Could not register log domain 'enjoy-fso'");
             return EINA_FALSE;
          }
     }

   if (!ENJOY_ABI_CHECK())
     {
        ERR("ABI versions differ: enjoy=%u, fso=%u",
            enjoy_abi_version(), ENJOY_ABI_VERSION);
        goto error;
     }

   if (conn) return EINA_TRUE;

   e_dbus_init();
   conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
   if (!conn)
     {
        ERR("Could not get DBus session bus");
        goto error;
     }

   enjoy_plugin_register("sys/fso", &api, ENJOY_PLUGIN_PRIORITY_NORMAL);

   return EINA_TRUE;

 error:
   eina_log_domain_unregister(_fso_log_domain);
   _fso_log_domain = -1;
   return EINA_FALSE;
}

static void
fso_shutdown(void)
{
   if (!conn) return;

   e_dbus_shutdown();
   conn = NULL;
   if (_fso_log_domain >= 0)
     {
        eina_log_domain_unregister(_fso_log_domain);
        _fso_log_domain = -1;
     }
}

EINA_MODULE_INIT(fso_init);
EINA_MODULE_SHUTDOWN(fso_shutdown);
