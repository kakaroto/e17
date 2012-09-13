#include "pka.h"
#include "marshal.h"
#include "ckit.h"
#include "logind.h"

#define POLKIT_BUS "org.freedesktop.PolicyKit1"
#define POLKIT_PATH "/org/freedesktop/PolicyKit1/Authority"
#define POLKIT_INTERFACE "org.freedesktop.PolicyKit1.Authority"

#define AGENT_BUS "auth.empower"
#define AGENT_PATH "/auth/empower/Agent"
#define AGENT_INTERFACE "org.freedesktop.PolicyKit1.AuthenticationAgent"

static E_DBus_Connection *_pka_system_conn = NULL;
static E_DBus_Object     *_pka_system_obj = NULL;
static DBusPendingCall   *_pka_register_call = NULL;
static Eina_List         *_pka_pending_auths = NULL;
const char               *_pka_agent_path = AGENT_PATH;

static Empower_Auth_State _pka_state;

// Storage for any session ID's retrieved from consolekit/logind
//const char               *_pka_session_id = NULL;
static const char   *_pka_session_ids[2] = {NULL, NULL};
static unsigned int  _pka_session_idx = 0;

// Method Call Callbacks
static void               _pka_register_done(void *data, DBusMessage *msg, DBusError *error);
static void               _pka_unregister_done(void *data, DBusMessage *msg, DBusError *error);

// Agent Interface Callbacks
static DBusMessage       *_pka_message_beginauthentication(E_DBus_Object *obj, DBusMessage *msg);
static DBusMessage       *_pka_message_cancelauthentication(E_DBus_Object *obj, DBusMessage *msg);

// Helper Callbacks
static Eina_Bool          _pka_helper_stdout(void *data, int type, void *event);
static Eina_Bool          _pka_helper_stderr(void *data, int type, void *event);

// Misc
static void               _pka_beginauthentication_finish(Empower_Auth_Info *info);
static Eina_Bool          _pka_session_idler(void *data);

Eina_Bool pka_init()
{
  E_DBus_Interface *iface;

  _pka_state = INVALID;

  if (!e_dbus_init())
  {
    ERR("Failed to initialize e_dbus!");
    return EINA_FALSE;
  }

  _pka_system_conn = e_dbus_bus_get(DBUS_BUS_SYSTEM);
  if (!_pka_system_conn)
  {
    ERR("Cannot connect to system bus!");
    return EINA_FALSE;
  }

  e_dbus_request_name(_pka_system_conn, AGENT_BUS, 0, NULL, NULL);

  _pka_system_obj = e_dbus_object_add(_pka_system_conn, AGENT_PATH, NULL);
  if (!_pka_system_obj)
  {
    ERR("Cannot create DBUS object "AGENT_PATH);
    return EINA_FALSE;
  }

  DBG("Creating AuthenticationAgent Interface");
  iface = e_dbus_interface_new(AGENT_INTERFACE);
  if (!iface)
  {
    ERR("Cannot add "AGENT_INTERFACE" to "AGENT_PATH);
    return EINA_FALSE;
  }
  e_dbus_object_interface_attach(_pka_system_obj, iface);

  if (!e_dbus_interface_method_add(iface, "BeginAuthentication", "sssa{ss}sa(sa{sv})", "", _pka_message_beginauthentication))
  {
    ERR("Failed to create BeginAuthentication method on "AGENT_INTERFACE);
    return EINA_FALSE;
  }

  if (!e_dbus_interface_method_add(iface, "CancelAuthentication", "s", "", _pka_message_cancelauthentication))
  {
    ERR("Failed to create BeginAuthentication method on "AGENT_INTERFACE);
    return EINA_FALSE;
  }

  // FIXME: Eventually ckit support can be dropped as it's deprecated.  This
  //        interface can then hopefully be simplified as we won't need to
  //        check for session ids from multiple places.
  ckit_init(_pka_system_conn);
  logind_init(_pka_system_conn);

  ecore_idle_enterer_add(_pka_session_idler, NULL);

  return EINA_TRUE;
}

Eina_Bool pka_shutdown()
{
  DBusMessage *msg;
  DBusMessageIter itr;
  Empower_Subject subject;

  // Handle case where we unregister before we're done registering
  if (_pka_register_call)
  {
    dbus_pending_call_cancel(_pka_register_call);
    _pka_register_call = NULL;
    return EINA_FALSE;
  }

  if (_pka_session_idx > (sizeof(_pka_session_ids)/sizeof(_pka_session_ids[0])))
    return EINA_TRUE;

  msg = dbus_message_new_method_call(POLKIT_BUS, POLKIT_PATH, POLKIT_INTERFACE, "UnregisterAuthenticationAgent");

  if (!msg)
    return EINA_FALSE;

  subject.kind = EMPOWER_SUBJECT_SESSION;
  subject.details.session.id = _pka_session_ids[_pka_session_idx];

  dbus_message_iter_init_append(msg, &itr);
  marshal_subject(&itr, &subject);
  dbus_message_iter_append_basic(&itr, DBUS_TYPE_STRING, &_pka_agent_path);

  _pka_register_call = e_dbus_message_send(_pka_system_conn, msg, _pka_unregister_done, -1, NULL);

  return EINA_TRUE;
}

Empower_Auth_State pka_state()
{
  return _pka_state;
}

Empower_Auth_Info *
pka_pending()
{
  Empower_Auth_Info *info;

  info = eina_list_nth(_pka_pending_auths, 0);

  if ((info == NULL) || (info->helper.exe != NULL))
    return NULL;

  return info;
}

Eina_Bool
pka_exec(Empower_Auth_Info *info)
{
  char buf[4096];
  Empower_Identity *id;

  id = eina_list_nth(info->identities, 0);

  // Run helper agent provided with polkit.
  // TODO: Somehow determine polkit-agent-helper-1 location at runtime.
  snprintf(buf, sizeof(buf), "/usr/lib/polkit-1/polkit-agent-helper-1 %s %s", id->details.user.name, info->cookie);
  DBG("Executing: %s\n", buf);
  info->helper.exe = ecore_exe_pipe_run(buf,
                                        (ECORE_EXE_PIPE_READ |
                                         ECORE_EXE_PIPE_WRITE |
                                         ECORE_EXE_PIPE_ERROR |
                                         ECORE_EXE_PIPE_READ_LINE_BUFFERED |
                                         ECORE_EXE_PIPE_ERROR_LINE_BUFFERED),
                                         info);

  info->helper.stdout = ecore_event_handler_add(ECORE_EXE_EVENT_DATA, _pka_helper_stdout, info);
  info->helper.stderr = ecore_event_handler_add(ECORE_EXE_EVENT_ERROR, _pka_helper_stderr, info);

  return EINA_TRUE;
}

Eina_Bool
pka_cancel(Empower_Auth_Info *info)
{
  _pka_beginauthentication_finish(info);
  return EINA_FALSE;
}

static void _pka_register_done(void *data, DBusMessage *msg, DBusError *error)
{
  _pka_register_call = NULL;

  if (error->message)
  {
    ERR("Failed to register Autentication Agent with session '%s', (%s)", _pka_session_ids[_pka_session_idx], error->message);

    // Try the next session ID just in case it was a session error.
    _pka_session_idx++;
    _pka_state = INVALID;

    return;
  }

  DBG("Registration Successful!");

  _pka_state = REGISTERED;
}

static void _pka_unregister_done(void *data, DBusMessage *msg, DBusError *error)
{
  unsigned int i;

  e_dbus_object_free(_pka_system_obj);
  e_dbus_connection_close(_pka_system_conn);
  free(_pka_system_conn);

  e_dbus_shutdown();

  for (i = 0; i < (sizeof(_pka_session_ids)/sizeof(_pka_session_ids[0])); ++i)
  {
    if (_pka_session_ids[i])
      eina_stringshare_del(_pka_session_ids[i]);
  }

  _pka_state = UNREGISTERED;
}

static DBusMessage* _pka_message_beginauthentication(E_DBus_Object *obj, DBusMessage *msg)
{
  DBusMessageIter itr, arr_iter;
  Empower_Identity *id;
  Empower_Auth_Info *info;

  DBG("Recieved a call to BeginAuthentication");

  dbus_message_ref(msg);

  info = calloc(1, sizeof(Empower_Auth_Info));

  info->msg = msg;

  // Get authentication details
  dbus_message_iter_init(msg, &itr);
  dbus_message_iter_get_basic(&itr, &info->action);
  dbus_message_iter_next(&itr);
  dbus_message_iter_get_basic(&itr, &info->message);
  dbus_message_iter_next(&itr);
  dbus_message_iter_get_basic(&itr, &info->icon);
  dbus_message_iter_next(&itr);
  info->details = unmarshal_dict_string_string(&itr);
  dbus_message_iter_next(&itr);
  dbus_message_iter_get_basic(&itr, &info->cookie);
  dbus_message_iter_next(&itr);

  info->identities = NULL;
  dbus_message_iter_recurse(&itr, &arr_iter);
  do
  {
    id = calloc(1, sizeof(Empower_Identity));

    unmarshal_identity(&arr_iter, id);

    info->identities = eina_list_append(info->identities, id);
  } while (dbus_message_iter_next(&arr_iter));

  _pka_pending_auths = eina_list_append(_pka_pending_auths, info);

  // Return NULL here as we need to show a GUI and respond later.
  return NULL;
}

static void _pka_beginauthentication_finish(Empower_Auth_Info *info)
{
  DBusMessage *reply;
  Empower_Identity *id;

  // If the helper was running, clean it up.
  if (info->helper.exe)
  {
    ecore_exe_quit(info->helper.exe);
    ecore_event_handler_del(info->helper.stdout);
    ecore_event_handler_del(info->helper.stderr);
    ecore_exe_free(info->helper.exe);
    info->helper.exe = NULL;
  }

  // Queue actual reply to the BeginAuthentication
  reply = dbus_message_new_method_return(info->msg);
  e_dbus_message_send(_pka_system_conn, reply, NULL, -1, NULL);

  // Remove this authentication from the list and free it
  _pka_pending_auths = eina_list_remove(_pka_pending_auths, info);
  eina_hash_free(info->details);
  EINA_LIST_FREE(info->identities, id)
  {
    eina_stringshare_del(id->details.user.name);
    free(id);
  }
  free(info);

  // Unref reply as this isn't done automagically
  dbus_message_unref(reply);
}

static DBusMessage* _pka_message_cancelauthentication(E_DBus_Object *obj, DBusMessage *msg)
{
  Empower_Auth_Info *info;

  DBG("Received CancelAuthentication");

  info = pka_pending();
  if (info)
    pka_cancel(info);

  return dbus_message_new_method_return(msg);
}

static Eina_Bool _pka_helper_stdout(void *data, int type, void *event)
{
  Ecore_Exe_Event_Data *ev;
  char msg[4096];
  int size;
  Empower_Auth_Info *info;

  ev = (Ecore_Exe_Event_Data *)event;
  info = data;

  size = ev->size;
  if (size > sizeof(msg))
    size = sizeof(msg)-1;

  strncpy(msg, ev->data, size);

  DBG("HELPER OUT: %s", msg);

  if (strstr(msg, "Password:"))
  {
    size = snprintf(msg, sizeof(msg), "%s\n", info->password);
    ecore_exe_send(info->helper.exe, msg, size);

    // Clear password for security
    memset(msg, 0, sizeof(msg));
    memset(info->password, 0, strlen(info->password));
    free(info->password);
    info->password = NULL;
  }
  else
  {
    _pka_beginauthentication_finish(info);
  }

  return EINA_FALSE;
}

static Eina_Bool _pka_helper_stderr(void *data, int type, void *event)
{
  Ecore_Exe_Event_Data *ev;
  char msg[4096];
  int size;

  ev = (Ecore_Exe_Event_Data *)event;

  size = ev->size;
  if (size > sizeof(msg))
    size = sizeof(msg)-1;

  strncpy(msg, ev->data, size);

  DBG("HELPER ERR: %s", msg);
  return EINA_FALSE;
}

static Eina_Bool _pka_session_idler(void *data)
{
  Empower_Subject subject;
  const char *s;
  DBusMessage *newmsg;
  DBusMessageIter itr;

  // If we've registered then we can stop trying different session IDs.
  if (_pka_state == REGISTERED)
    return ECORE_CALLBACK_CANCEL;

  // If we've run out of session IDs, throw an error and claim unregistered to
  // cause Empower to exit.
  if (_pka_session_idx >= (sizeof(_pka_session_ids)/sizeof(_pka_session_ids[0])))
  {
    ERR("Could not register Empower as an authentication agent");
    _pka_state = UNREGISTERED;

    return ECORE_CALLBACK_CANCEL;
  }

  _pka_session_ids[0] = ckit_session_get();
  _pka_session_ids[1] = logind_session_get();

  if ((_pka_state == INVALID) && (_pka_session_ids[_pka_session_idx] != NULL))
  {
    DBG("Attempting to register with session id %s", _pka_session_ids[_pka_session_idx]);
    newmsg = dbus_message_new_method_call(POLKIT_BUS, POLKIT_PATH, POLKIT_INTERFACE, "RegisterAuthenticationAgent");

    if (!newmsg)
    {
      ERR("Failed to allocate memory!");
      return ECORE_CALLBACK_CANCEL;
    }

    subject.kind = EMPOWER_SUBJECT_SESSION;
    subject.details.session.id = _pka_session_ids[_pka_session_idx];
    s = eina_stringshare_add(setlocale(LC_CTYPE, NULL));

    dbus_message_iter_init_append(newmsg, &itr);
    marshal_subject(&itr, &subject);
    dbus_message_iter_append_basic(&itr, DBUS_TYPE_STRING, &s);
    dbus_message_iter_append_basic(&itr, DBUS_TYPE_STRING, &_pka_agent_path);

    _pka_register_call = e_dbus_message_send(_pka_system_conn, newmsg, _pka_register_done, -1, NULL);

    eina_stringshare_del(s);

    _pka_state = REGISTERING;
  }

  return ECORE_CALLBACK_RENEW;
}

