#include "ckit.h"

#define CONKIT_BUS "org.freedesktop.ConsoleKit"
#define CONKIT_PATH "/org/freedesktop/ConsoleKit/Manager"
#define CONKIT_INTERFACE "org.freedesktop.ConsoleKit.Manager"

static const char *_ckit_session_id = NULL;
static void _ckit_session_id_done(void *data, DBusMessage *msg, DBusError *error);

Eina_Bool ckit_init(E_DBus_Connection *conn)
{
  DBusMessage *msg;
  DBusMessageIter itr;

  DBG("Requesting session id from ConsoleKit");
  msg = dbus_message_new_method_call(CONKIT_BUS, CONKIT_PATH, CONKIT_INTERFACE, "GetSessionForUnixProcess");

  if (!msg)
    return EINA_FALSE;

  dbus_message_iter_init_append(msg, &itr);
  dbus_message_iter_append_basic(&itr, DBUS_TYPE_UINT32, &empower_pid);

  e_dbus_message_send(conn, msg, _ckit_session_id_done, 5000, NULL);

  // Return pointer to the id so the caller can watch for it to be filled
  return EINA_TRUE;
}

const char *ckit_session_get()
{
  return _ckit_session_id;
}

static void
_ckit_session_id_done(void *data, DBusMessage *msg, DBusError *error)
{
  const char *s;
  DBusMessageIter itr;

  if (error->message)
  {
    WRN("Failed to get session ID (%s)", error->message);

    _ckit_session_id = eina_stringshare_add("");

    return;
  }

  dbus_message_iter_init(msg, &itr);
  dbus_message_iter_get_basic(&itr, &s);
  _ckit_session_id = eina_stringshare_add(s);

  DBG("Session Received: %s", _ckit_session_id);
}
