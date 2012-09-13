#include "logind.h"
#include <libgen.h>

#define LOGIND_BUS "org.freedesktop.login1"
#define LOGIND_PATH "/org/freedesktop/login1"
#define LOGIND_INTERFACE "org.freedesktop.login1.Manager"

#if HAVE_LIBSYSTEMD_LOGIN
#include <systemd/sd-login.h>
#endif

static const char *_logind_session_id = NULL;

#if !HAVE_LIBSYSTEMD_LOGIN
static void _logind_session_id_done(void *data, DBusMessage *msg, DBusError *error);
#endif

Eina_Bool logind_init(E_DBus_Connection *conn)
{
#if HAVE_LIBSYSTEMD_LOGIN
  char *id;

  DBG("Requesting systemd-login session via library");

  if (sd_pid_get_session (empower_pid, &id) < 0)
    return EINA_FALSE;

  _logind_session_id = eina_stringshare_add(id);
  free(id);
  return EINA_TRUE;
#else
  DBusMessage *msg;
  DBusMessageIter itr;

  DBG("Requesting systemd-login session via dbus");
  msg = dbus_message_new_method_call(LOGIND_BUS, LOGIND_PATH, LOGIND_INTERFACE, "GetSessionByPID");

  if (!msg)
    return EINA_FALSE;

  dbus_message_iter_init_append(msg, &itr);
  dbus_message_iter_append_basic(&itr, DBUS_TYPE_UINT32, &empower_pid);

  e_dbus_message_send(conn, msg, _logind_session_id_done, 5000, NULL);

  return EINA_TRUE;
#endif
}

const char *logind_session_get()
{
  return _logind_session_id;
}

#if !HAVE_LIBSYSTEMD_LOGIN
static void
_logind_session_id_done(void *data, DBusMessage *msg, DBusError *error)
{
  char *s;
  DBusMessageIter itr;

  if (error->message)
  {
    WRN("Failed to get session ID (%s)", error->message);

    _logind_session_id = eina_stringshare_add("");

    return;
  }

  dbus_message_iter_init(msg, &itr);
  dbus_message_iter_get_basic(&itr, &s);

  // The reason basename is used is because polkit uses the sd_pid_get_session
  // function.  This function only returns the session number and not the full
  // path.  This is in contrast to polkit's ConsoleKit support which expects
  // the full path to the session object on the ConsoleKit dbus interface.
  _logind_session_id = eina_stringshare_add(basename(s));

  DBG("Session Received: %s", _logind_session_id);
}
#endif
