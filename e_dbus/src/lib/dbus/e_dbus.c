#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include "e_dbus_private.h"

static E_DBus_Version _version = { VMAJ, VMIN, VMIC, VREV };
EAPI E_DBus_Version *e_dbus_version = &_version;

#define NUM_BUS_TYPES 3

/*
 * TODO: 
 *  listen for disconnected signal and clean up?
 *  listen for NameOwnerChanged signals for names we have SignalHandler's for
 *    remap SH to listen for signals from new owner
 */
int _e_dbus_log_dom = -1;
static int connection_slot = -1;

static int _edbus_init_count = 0;
static int close_connection = 0;
EAPI int E_DBUS_EVENT_SIGNAL = 0;

static E_DBus_Connection *shared_connections[2] = {NULL, NULL};

typedef struct E_DBus_Handler_Data E_DBus_Handler_Data;
typedef struct E_DBus_Timeout_Data E_DBus_Timeout_Data;


struct E_DBus_Handler_Data
{
  int fd;
  Ecore_Fd_Handler *fd_handler;
  E_DBus_Connection *cd;
  DBusWatch *watch;
  int enabled;
};

struct E_DBus_Timeout_Data
{
  Ecore_Timer *handler;
  DBusTimeout *timeout;
  E_DBus_Connection *cd;
  int interval;
};

static Eina_Bool e_dbus_idler(void *data);

static Eina_Bool
e_dbus_fd_handler(void *data, Ecore_Fd_Handler *fd_handler)
{
  E_DBus_Handler_Data *hd;
  unsigned int condition = 0;

  DBG("fd handler (%ld)!", (long int)fd_handler);

  hd = data;

  if (!hd->enabled) {
    DBG("handler disabled");
    if (hd->fd_handler) ecore_main_fd_handler_del(hd->fd_handler);
    hd->fd_handler = NULL;
    return ECORE_CALLBACK_CANCEL;
  }
  if (ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_READ)) condition |= DBUS_WATCH_READABLE;
  if (ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_WRITE)) condition |= DBUS_WATCH_WRITABLE;
  if (ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_ERROR)) condition |= DBUS_WATCH_ERROR;

  if (condition & DBUS_WATCH_ERROR) DBG("DBUS watch error");
  dbus_watch_handle(hd->watch, condition);
  hd = NULL;

  return ECORE_CALLBACK_RENEW;
}

static void
e_dbus_fd_handler_add(E_DBus_Handler_Data *hd)
{
  unsigned int dflags;
  Ecore_Fd_Handler_Flags eflags;

  if (hd->fd_handler) return;
  DBG("fd handler add (%d)", hd->fd);

  dflags = dbus_watch_get_flags(hd->watch);
  eflags = ECORE_FD_ERROR;
  if (dflags & DBUS_WATCH_READABLE) eflags |= ECORE_FD_READ;
  if (dflags & DBUS_WATCH_WRITABLE) eflags |= ECORE_FD_WRITE;


  hd->fd_handler = ecore_main_fd_handler_add(hd->fd,
                                             eflags,
                                             e_dbus_fd_handler,
                                             hd,
                                             NULL,
                                             NULL);

  hd->cd->fd_handlers = eina_list_append(hd->cd->fd_handlers, hd->fd_handler);
}


static void
e_dbus_handler_data_free(void *data)
{
  E_DBus_Handler_Data *hd = data;
  
  DBG("e_dbus_handler_data_free");
  if (hd->fd_handler)
  {
    hd->cd->fd_handlers = eina_list_remove(hd->cd->fd_handlers, hd->fd_handler);
    ecore_main_fd_handler_del(hd->fd_handler);
  }
  free(hd);
}

static void
e_dbus_connection_data_watch_add(E_DBus_Connection *cd, DBusWatch *watch)
{
  E_DBus_Handler_Data *hd;

  hd = calloc(1, sizeof(E_DBus_Handler_Data));
  dbus_watch_set_data(watch, hd, e_dbus_handler_data_free);
  hd->cd = cd;
  hd->watch = watch;

  hd->enabled = dbus_watch_get_enabled(watch);
#if (DBUS_VERSION_MAJOR == 1 && DBUS_VERSION_MINOR == 1 && DBUS_VERSION_MICRO>= 1) || (DBUS_VERSION_MAJOR == 1 && DBUS_VERSION_MINOR > 1) || (DBUS_VERSION_MAJOR > 1)
  hd->fd = dbus_watch_get_unix_fd(hd->watch);
#else
  hd->fd = dbus_watch_get_fd(hd->watch);
#endif
  DBG("watch add (enabled: %d)", hd->enabled);
  if (hd->enabled) e_dbus_fd_handler_add(hd);
}

static E_DBus_Connection *
e_dbus_connection_new(DBusConnection *conn)
{
  E_DBus_Connection *cd;
  const char *conn_name;

  cd = calloc(1, sizeof(E_DBus_Connection));
  if (!cd) return NULL;

  cd->conn = conn;
  conn_name = dbus_bus_get_unique_name(conn);
  if (conn_name)
  {
    DBG("Connected! Name: %s", conn_name);
    cd->conn_name = strdup(conn_name);
  }
  else
    DBG("Not connected");

  cd->shared_type = (unsigned int)-1;
  cd->fd_handlers = NULL;
  cd->timeouts = NULL;

  return cd;
}

static void
e_dbus_connection_free(void *data)
{
  E_DBus_Connection *cd = data;
  Ecore_Fd_Handler *fd_handler;
  Ecore_Timer *timer;
  DBG("e_dbus_connection free!");

  EINA_LIST_FREE(cd->fd_handlers, fd_handler)
    ecore_main_fd_handler_del(fd_handler);

  EINA_LIST_FREE(cd->timeouts, timer)
    ecore_timer_del(timer);

  if (cd->shared_type != (unsigned int)-1)
    shared_connections[cd->shared_type] = NULL;

  e_dbus_signal_handlers_free_all(cd);

  if (cd->conn_name) free(cd->conn_name);

  if (cd->idler) ecore_idle_enterer_del(cd->idler);

  free(cd);
}


static void
cb_main_wakeup(void *data)
{
  E_DBus_Connection *cd;
  DBG("wakeup main!");

  cd = data;

  if (!cd->idler) cd->idler = ecore_idle_enterer_add(e_dbus_idler, cd);
  else DBG("already idling");
}

static void
e_dbus_loop_wakeup(void)
{
  static int dummy_event = 0;

  /* post a dummy event to get the mainloop back to normal - this is
   * needed because idlers are very special things that won't re-evaluate
   * timers and other stuff while idelrs run - idle_exiters and enterers
   * can do this safely, but not idlers. idelrs were meant to be used
   * very sparingly for very special cases */
  if (dummy_event == 0) dummy_event = ecore_event_type_new();
  ecore_event_add(dummy_event, NULL, NULL, NULL);
}

static void
cb_dispatch_status(DBusConnection *conn __UNUSED__, DBusDispatchStatus new_status, void *data)
{
  E_DBus_Connection *cd;

  DBG("dispatch status: %d!", new_status);
  cd = data;

  if (new_status == DBUS_DISPATCH_DATA_REMAINS && !cd->idler) cd->idler = ecore_idle_enterer_add(e_dbus_idler, cd);

  else if (new_status != DBUS_DISPATCH_DATA_REMAINS && cd->idler) 
  {
    ecore_idle_enterer_del(cd->idler);
    cd->idler = NULL;
    e_dbus_loop_wakeup();
  }
}

static Eina_Bool
e_dbus_timeout_handler(void *data)
{
  E_DBus_Timeout_Data *td;

  td = data;

  if (dbus_timeout_get_enabled(td->timeout)) 
  {
    DBG("timeout_handler (not enabled, ending)");
    td->handler = NULL;
    return ECORE_CALLBACK_CANCEL;
  }

  DBG("timeout handler!");
  dbus_timeout_handle(td->timeout);
  return ECORE_CALLBACK_RENEW;
}

static void
e_dbus_timeout_data_free(void *timeout_data)
{
  E_DBus_Timeout_Data *td = timeout_data;
  DBG("e_dbus_timeout_data_free");
  if (td->handler) ecore_timer_del(td->handler);
  free(td);
}

static dbus_bool_t 
cb_timeout_add(DBusTimeout *timeout, void *data)
{
  E_DBus_Connection *cd;
  E_DBus_Timeout_Data *td;
  
  cd = data;
  DBG("timeout add!");
  td = calloc(1, sizeof(E_DBus_Timeout_Data));
  td->cd = cd;
  dbus_timeout_set_data(timeout, (void *)td, e_dbus_timeout_data_free);

  td->interval = dbus_timeout_get_interval(timeout);
  td->timeout = timeout;

  if (dbus_timeout_get_enabled(timeout)) td->handler = ecore_timer_add(td->interval, e_dbus_timeout_handler, td);
  td->cd->timeouts = eina_list_append(td->cd->timeouts, td->handler);

  return true;
}

static void
cb_timeout_del(DBusTimeout *timeout, void *data __UNUSED__)
{
  E_DBus_Timeout_Data *td;
  DBG("timeout del!");

  td = (E_DBus_Timeout_Data *)dbus_timeout_get_data(timeout);

  if (td->handler) 
  {
    td->cd->timeouts = eina_list_remove(td->cd->timeouts, td->handler);
    ecore_timer_del(td->handler);
    td->handler = NULL;
  }

  /* Note: timeout data gets freed when the timeout itself is freed by dbus */
}

static void
cb_timeout_toggle(DBusTimeout *timeout, void *data __UNUSED__)
{
  E_DBus_Timeout_Data *td;
  DBG("timeout toggle!");

  td = (E_DBus_Timeout_Data *)dbus_timeout_get_data(timeout);

  if (dbus_timeout_get_enabled(td->timeout))
  {
    td->interval = dbus_timeout_get_interval(timeout);
    td->handler = ecore_timer_add(td->interval, e_dbus_timeout_handler, td);
  }
  else
  {
    ecore_timer_del(td->handler);
    td->handler = NULL;
  }


}

static dbus_bool_t 
cb_watch_add(DBusWatch *watch, void *data)
{
  E_DBus_Connection *cd;
  cd = data;

  DBG("cb_watch_add");
  e_dbus_connection_data_watch_add(cd, watch);

  return true;
}

static void
cb_watch_del(DBusWatch *watch, void *data __UNUSED__)
{
  E_DBus_Handler_Data *hd;

  DBG("cb_watch_del");
  hd = (E_DBus_Handler_Data *)dbus_watch_get_data(watch);

  if (hd->fd_handler) 
  {
    hd->cd->fd_handlers = eina_list_remove(hd->cd->fd_handlers, hd->fd_handler);
    ecore_main_fd_handler_del(hd->fd_handler);
    hd->fd_handler = NULL;
  }
}

static void
cb_watch_toggle(DBusWatch *watch, void *data __UNUSED__)
{
  E_DBus_Handler_Data *hd;

  DBG("cb_watch_toggle");
  hd = dbus_watch_get_data(watch);

  if (!hd) return;

  hd->enabled = dbus_watch_get_enabled(watch);

  if (hd->enabled) e_dbus_fd_handler_add(hd);
}

static void
e_dbus_message_free(void *data __UNUSED__, void *message)
{
  dbus_message_unref(message);
}

static DBusHandlerResult
e_dbus_filter(DBusConnection *conn __UNUSED__, DBusMessage *message, void *user_data)
{
  E_DBus_Connection *cd = user_data;
  DBG("-----------------");
  DBG("Message!");

  DBG("type: %s", dbus_message_type_to_string(dbus_message_get_type(message)));
  DBG("path: %s", dbus_message_get_path(message));
  DBG("interface: %s", dbus_message_get_interface(message));
  DBG("member: %s", dbus_message_get_member(message));
  DBG("sender: %s", dbus_message_get_sender(message));

  switch (dbus_message_get_type(message))
  {
    case DBUS_MESSAGE_TYPE_METHOD_CALL:
      DBG("signature: %s", dbus_message_get_signature(message));
      break;
    case DBUS_MESSAGE_TYPE_METHOD_RETURN:
      DBG("reply serial %d", dbus_message_get_reply_serial(message));
      break;
    case DBUS_MESSAGE_TYPE_ERROR:
      DBG("error: %s", dbus_message_get_error_name(message));
      break;
    case DBUS_MESSAGE_TYPE_SIGNAL:
      dbus_message_ref(message);

      if (cd->signal_dispatcher)
	cd->signal_dispatcher(cd, message);

      ecore_event_add(E_DBUS_EVENT_SIGNAL, message, e_dbus_message_free, NULL);
      /* don't need to handle signals, they're for everyone who wants them */
      return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
      break;
    default:
      break;
  }
  DBG("-----------------");

  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}

int e_dbus_idler_active = 0;

static Eina_Bool
e_dbus_idler(void *data)
{
  E_DBus_Connection *cd;
  cd = data;

  if (DBUS_DISPATCH_COMPLETE == dbus_connection_get_dispatch_status(cd->conn))
  {
    DBG("done dispatching!");
    cd->idler = NULL;
    return ECORE_CALLBACK_CANCEL;
  }
  e_dbus_idler_active++;
  dbus_connection_ref(cd->conn);
  DBG("dispatch!");
  dbus_connection_dispatch(cd->conn);
  dbus_connection_unref(cd->conn);
  e_dbus_idler_active--;
  e_dbus_signal_handlers_clean(cd);
  if (!e_dbus_idler_active && close_connection)
  {
    do
    {
      e_dbus_connection_close(cd);
    } while (--close_connection);
  }
  e_dbus_loop_wakeup();
  return ECORE_CALLBACK_RENEW;
}

EAPI E_DBus_Connection *
e_dbus_bus_get(DBusBusType type)
{
  DBusError err;
  E_DBus_Connection *econn;
  DBusConnection *conn;

  /* each app only needs a single connection to either bus */
  if (type == DBUS_BUS_SYSTEM || type == DBUS_BUS_SESSION)
  {
    if (shared_connections[type]) 
    {
      e_dbus_connection_ref(shared_connections[type]);
      return shared_connections[type];
    }
  }

  dbus_error_init(&err);

  conn = dbus_bus_get_private(type, &err);
  if (dbus_error_is_set(&err))
  {
    ERR("Error connecting to bus: %s", err.message);
    dbus_error_free(&err);
    return NULL;
  }

  econn = e_dbus_connection_setup(conn);
  if (!econn)
  {
    ERR("Error setting up dbus connection.");
    dbus_connection_close(conn);
    dbus_connection_unref(conn);
    return NULL;
  }

  if (type == DBUS_BUS_SYSTEM || type == DBUS_BUS_SESSION)
  {
    econn->shared_type = type;
    shared_connections[type] = econn;
  }
  dbus_error_free(&err);
  e_dbus_connection_ref(econn);
  return econn;
}

EAPI E_DBus_Connection *
e_dbus_connection_setup(DBusConnection *conn)
{
  E_DBus_Connection *cd;

  cd = e_dbus_connection_new(conn);
  if (!cd) return NULL;

  /* connection_setup */
  dbus_connection_set_exit_on_disconnect(cd->conn, EINA_FALSE);
  dbus_connection_allocate_data_slot(&connection_slot);

  dbus_connection_set_data(cd->conn, connection_slot, (void *)cd, e_dbus_connection_free);
  dbus_connection_set_watch_functions(cd->conn,
                                      cb_watch_add,
                                      cb_watch_del,
                                      cb_watch_toggle,
                                      cd,
                                      NULL);

  dbus_connection_set_timeout_functions(cd->conn,
                                      cb_timeout_add,
                                      cb_timeout_del,
                                      cb_timeout_toggle,
                                      cd,
                                      NULL);

  dbus_connection_set_wakeup_main_function(cd->conn, cb_main_wakeup, cd, NULL);
  dbus_connection_set_dispatch_status_function(cd->conn, cb_dispatch_status, cd, NULL);
  dbus_connection_add_filter(cd->conn, e_dbus_filter, cd, NULL);

  cb_dispatch_status(cd->conn, dbus_connection_get_dispatch_status(cd->conn), cd);

  return cd;
}


EAPI void
e_dbus_connection_close(E_DBus_Connection *conn)
{
  DBG("e_dbus_connection_close");

  if (e_dbus_idler_active)
  {
    close_connection++;
    return;
  }
  if (--(conn->refcount) != 0) return;

  dbus_connection_free_data_slot(&connection_slot);
  dbus_connection_remove_filter(conn->conn, e_dbus_filter, conn);
  dbus_connection_set_watch_functions (conn->conn,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL, NULL);

  dbus_connection_set_timeout_functions (conn->conn,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL, NULL);

  dbus_connection_set_dispatch_status_function (conn->conn, NULL, NULL, NULL);

  /* Idler functin must be cancelled when dbus connection is  unreferenced */
  if (conn->idler)
    {
      ecore_idle_enterer_del(conn->idler);
      conn->idler = NULL;
    }

  dbus_connection_close(conn->conn);
  dbus_connection_unref(conn->conn);

  // Note: the E_DBus_Connection gets freed when the dbus_connection is cleaned up by the previous unref
}

EAPI void
e_dbus_connection_ref(E_DBus_Connection *conn)
{
  conn->refcount++;
}

DBusConnection *
e_dbus_connection_dbus_connection_get(E_DBus_Connection *conn)
{
  return conn->conn;
}

EAPI int
e_dbus_init(void)
{
  if (++_edbus_init_count != 1)
    return _edbus_init_count;
  
  if (!eina_init())
    {
      fprintf(stderr,"E-dbus: Enable to initialize eina\n");
      return --_edbus_init_count;
    }

  _e_dbus_log_dom = eina_log_domain_register("e_dbus", E_DBUS_COLOR_DEFAULT);
  if (_e_dbus_log_dom < 0)
    {
      EINA_LOG_ERR("Unable to create an 'e_dbus' log domain");
      eina_shutdown();
      return --_edbus_init_count;
    }
  if (!ecore_init())
    {
      ERR("E-dbus: Unable to initialize ecore");
      eina_log_domain_unregister(_e_dbus_log_dom);
      _e_dbus_log_dom = -1;
      eina_shutdown();
      return --_edbus_init_count;
    }


  E_DBUS_EVENT_SIGNAL = ecore_event_type_new();
  e_dbus_object_init();

  return _edbus_init_count;
}

EAPI int
e_dbus_shutdown(void)
{
  if (--_edbus_init_count)
    return _edbus_init_count;

  e_dbus_object_shutdown();
  ecore_shutdown();
  eina_log_domain_unregister(_e_dbus_log_dom);
  _e_dbus_log_dom = -1;
  eina_shutdown();

  return _edbus_init_count;
}
