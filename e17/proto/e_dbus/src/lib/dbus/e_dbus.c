#include "E_DBus.h"

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Ecore.h>
#include <Ecore_Data.h>

#define NUM_BUS_TYPES 3

/*
 * TODO: 
 *  listen for disconnected signal and clean up?
 *  listen for NameOwnerChanged signals for names we have SignalHandler's for
 *    remap SH to listen for signals from new owner
 */

static int connection_slot = -1;

static int init = 0;
int E_DBUS_EVENT_SIGNAL = 0;

typedef struct E_DBus_Handler_Data E_DBus_Handler_Data;
typedef struct E_DBus_Timeout_Data E_DBus_Timeout_Data;

struct E_DBus_Connection
{
  DBusConnection *conn;
  char *conn_name;

  Ecore_List *fd_handlers;
  Ecore_List *timeouts;

  Ecore_Idler *idler;

  int refcount;
};

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

static int e_dbus_idler(void *data);

static int
_e_dbus_fd_handler(void *data, Ecore_Fd_Handler *fd_handler)
{
  E_DBus_Handler_Data *hd;
  DBusConnection *conn;
  unsigned int condition = 0;

  DEBUG(5, "fd handler (%ld)!\n", (long int)fd_handler);

  hd = data;

  if (!hd->enabled) {
    DEBUG(5, "handler disabled\n");
    if (hd->fd_handler) ecore_main_fd_handler_del(hd->fd_handler);
    hd->fd_handler = NULL;
    return 0;
  }

  conn = hd->cd->conn;

  if (ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_READ)) condition |= DBUS_WATCH_READABLE;
  if (ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_WRITE)) condition |= DBUS_WATCH_WRITABLE;
  if (ecore_main_fd_handler_active_get(fd_handler, ECORE_FD_ERROR)) condition |= DBUS_WATCH_ERROR;

  if (condition & DBUS_WATCH_ERROR) DEBUG(5, "DBUS watch error\n");
  dbus_watch_handle(hd->watch, condition);
  hd = NULL;

  return 1;
}


static void
e_dbus_fd_handler_add(E_DBus_Handler_Data *hd)
{
  unsigned int dflags;
  Ecore_Fd_Handler_Flags eflags;

  if (hd->fd_handler) return;
  DEBUG(5, "fd handler add (%d)\n", hd->fd);

  dflags = dbus_watch_get_flags(hd->watch);
  eflags = ECORE_FD_ERROR;
  if (dflags & DBUS_WATCH_READABLE) eflags |= ECORE_FD_READ;
  if (dflags & DBUS_WATCH_WRITABLE) eflags |= ECORE_FD_WRITE;


  hd->fd_handler = ecore_main_fd_handler_add(hd->fd,
                                             eflags,
                                             _e_dbus_fd_handler,
                                             hd,
                                             NULL,
                                             NULL);

  ecore_list_append(hd->cd->fd_handlers, hd->fd_handler);
}


static void
_e_dbus_handler_data_free(void *data)
{
  E_DBus_Handler_Data *hd = data;
  
  DEBUG(5, "_e_dbus_handler_data_free\n");
  if (hd->fd_handler)
  {
    if (ecore_list_goto(hd->cd->fd_handlers, hd->fd_handler))
      ecore_list_remove(hd->cd->fd_handlers);
    ecore_main_fd_handler_del(hd->fd_handler);
  }
  free(hd);
}

static void
e_dbus_connection_data_watch_add(E_DBus_Connection *cd, DBusWatch *watch)
{
  E_DBus_Handler_Data *hd;

  hd = calloc(1, sizeof(E_DBus_Handler_Data));
  dbus_watch_set_data(watch, hd, _e_dbus_handler_data_free);
  hd->cd = cd;
  hd->watch = watch;

  hd->enabled = dbus_watch_get_enabled(watch);
  hd->fd = dbus_watch_get_fd(hd->watch);
  DEBUG(5, "watch add (enabled: %d)\n", hd->enabled);
  if (hd->enabled) e_dbus_fd_handler_add(hd);
}

static void
_e_dbus_connection_free(void *data)
{
  E_DBus_Connection *cd = data;
  Ecore_Fd_Handler *fd_handler;
  Ecore_Timer *timer;
  DEBUG(5, "_e_dbus_connection free!\n");

  if (cd->conn_name) free(cd->conn_name);

  ecore_list_goto_first(cd->fd_handlers);
  while ((fd_handler = ecore_list_next(cd->fd_handlers)))
    ecore_main_fd_handler_del(fd_handler);
  ecore_list_destroy(cd->fd_handlers);

  ecore_list_goto_first(cd->timeouts);
  while ((timer = ecore_list_next(cd->timeouts)))
    ecore_timer_del(timer);
  ecore_list_destroy(cd->timeouts);

  free(cd);
}


static void
cb_main_wakeup(void *data)
{
  E_DBus_Connection *cd;
  DEBUG(5, "wakeup main!\n");

  cd = data;

  if (!cd->idler) cd->idler = ecore_idler_add(e_dbus_idler, cd);
  else DEBUG(1, "already idling\n");
}

static void
cb_dispatch_status(DBusConnection *conn, DBusDispatchStatus new_status, void *data)
{
  E_DBus_Connection *cd;

  DEBUG(5, "dispatch status: %d!\n", new_status);
  cd = data;

  if (new_status == DBUS_DISPATCH_DATA_REMAINS && !cd->idler) cd->idler = ecore_idler_add(e_dbus_idler, cd);

  else if (new_status != DBUS_DISPATCH_DATA_REMAINS && cd->idler) 
  {
    ecore_idler_del(cd->idler);
    cd->idler = NULL;
  }
}

static int
e_dbus_timeout_handler(void *data)
{
  E_DBus_Timeout_Data *td;

  td = data;

  if (dbus_timeout_get_enabled(td->timeout)) 
  {
    DEBUG(5, "timeout_handler (not enabled, ending)\n");
    td->handler = NULL;
    return 0;
  }

  DEBUG(5, "timeout handler!\n");
  dbus_timeout_handle(td->timeout);
  return 1;
}

static void
e_dbus_timeout_data_free(void *timeout_data)
{
  E_DBus_Timeout_Data *td = timeout_data;
  DEBUG(5, "_e_dbus_timeout_data_free\n");
  if (td->handler) ecore_timer_del(td->handler);
  free(td);
}

static dbus_bool_t 
cb_timeout_add(DBusTimeout *timeout, void *data)
{
  E_DBus_Connection *cd;
  E_DBus_Timeout_Data *td;
  
  cd = data;
  DEBUG(5, "timeout add!\n");
  td = calloc(1, sizeof(E_DBus_Timeout_Data));
  td->cd = cd;
  dbus_timeout_set_data(timeout, (void *)td, e_dbus_timeout_data_free);

  td->interval = dbus_timeout_get_interval(timeout);
  td->timeout = timeout;

  if (dbus_timeout_get_enabled(timeout)) td->handler = ecore_timer_add(td->interval, e_dbus_timeout_handler, td);
  ecore_list_append(td->cd->timeouts, td->handler);

  return true;
}

static void
cb_timeout_del(DBusTimeout *timeout, void *data)
{
  E_DBus_Timeout_Data *td;
  DEBUG(5, "timeout del!\n");

  td = (E_DBus_Timeout_Data *)dbus_timeout_get_data(timeout);

  if (td->handler) 
  {
    if (ecore_list_goto(td->cd->timeouts, td->handler))
      ecore_list_remove(td->cd->timeouts);
    ecore_timer_del(td->handler);
    td->handler = NULL;
  }

  /* Note: timeout data gets freed when the timeout itself is freed by dbus */
}

static void
cb_timeout_toggle(DBusTimeout *timeout, void *data)
{
  E_DBus_Timeout_Data *td;
  DEBUG(5, "timeout toggle!\n");

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

  DEBUG(5, "cb_watch_add\n");
  e_dbus_connection_data_watch_add(cd, watch);

  return true;
}

static void
cb_watch_del(DBusWatch *watch, void *data)
{
  E_DBus_Connection *cd;
  E_DBus_Handler_Data *hd;
  cd = data;

  DEBUG(5, "cb_watch_del\n");
  hd = (E_DBus_Handler_Data *)dbus_watch_get_data(watch);

  if (hd->fd_handler) 
  {
    if (ecore_list_goto(hd->cd->fd_handlers, hd->fd_handler))
      ecore_list_remove(hd->cd->fd_handlers);
    ecore_main_fd_handler_del(hd->fd_handler);
    hd->fd_handler = NULL;
  }
}

static void
cb_watch_toggle(DBusWatch *watch, void *data)
{
  E_DBus_Handler_Data *hd;

  DEBUG(5, "cb_watch_toggle\n");
  hd = dbus_watch_get_data(watch);

  if (!hd) return;

  hd->enabled = dbus_watch_get_enabled(watch);

  if (hd->enabled) e_dbus_fd_handler_add(hd);
}

static void
e_dbus_message_free(void *data, void *message)
{
  //dbus_message_unref(message);
}

static DBusHandlerResult
e_dbus_filter(DBusConnection *conn, DBusMessage *message, void *user_data)
{
  DEBUG(3, "-----------------\nMessage!\n\n");

  DEBUG(3, "type: %s\n", dbus_message_type_to_string(dbus_message_get_type(message)));
  DEBUG(3, "path: %s\n", dbus_message_get_path(message));
  DEBUG(3, "interface: %s\n", dbus_message_get_interface(message));
  DEBUG(3, "member: %s\n", dbus_message_get_member(message));
  DEBUG(3, "sender: %s\n", dbus_message_get_sender(message));

  switch (dbus_message_get_type(message))
  {
    case DBUS_MESSAGE_TYPE_METHOD_CALL:
      DEBUG(3, "signature: %s\n", dbus_message_get_signature(message));
      break;
    case DBUS_MESSAGE_TYPE_METHOD_RETURN:
      DEBUG(3, "reply serial %d\n", dbus_message_get_reply_serial(message));
      break;
    case DBUS_MESSAGE_TYPE_ERROR:
      DEBUG(3, "error: %s\n", dbus_message_get_error_name(message));
      break;
    case DBUS_MESSAGE_TYPE_SIGNAL:
      ecore_event_add(E_DBUS_EVENT_SIGNAL, message, e_dbus_message_free, NULL);
      return DBUS_HANDLER_RESULT_HANDLED;
      break;
    default:
      break;
  }
  DEBUG(3, "-----------------\n\n");


  return DBUS_HANDLER_RESULT_NOT_YET_HANDLED;
}


static int
e_dbus_idler(void *data)
{
  E_DBus_Connection *cd;
  cd = data;

  if (DBUS_DISPATCH_COMPLETE == dbus_connection_get_dispatch_status(cd->conn))
  {
    DEBUG(5, "done dispatching!\n");
    cd->idler = NULL;
    return 0;
  }
  dbus_connection_ref(cd->conn);
  DEBUG(5, "dispatch!\n");
  dbus_connection_dispatch(cd->conn);
  dbus_connection_unref(cd->conn);
  return 1;
}

/* XXX should this be separated from the watch/timeout setup code? what happens when this is called more than once in a single process? */
DBusConnection *
e_dbus_bus_get(DBusBusType type)
{
  DBusError err;
  DBusConnection *conn;

  dbus_error_init(&err);

  conn = dbus_bus_get_private(type, &err);
  if (dbus_error_is_set(&err))
  {
    fprintf(stderr, "Error connecting to bus: %s\n", err.message);
    dbus_error_free(&err);
    return NULL;
  }

  if (!e_dbus_connection_setup(conn))
  {
    fprintf(stderr, "Error setting up dbus connection.\n");
    dbus_connection_unref(conn);
    return NULL;
  }

  dbus_error_free(&err);
  return conn;
}

/**
 * Integrate a DBus connection with the ecore main loop
 *
 * @param conn - a dbus connection
 */
int
e_dbus_connection_setup(DBusConnection *conn)
{
  E_DBus_Connection *cd;

  cd = calloc(1, sizeof(E_DBus_Connection));
  if (!cd) return 0;
  cd->conn = conn;

  cd->fd_handlers = ecore_list_new();
  cd->timeouts = ecore_list_new();
  cd->conn_name = strdup(dbus_bus_get_unique_name(cd->conn));
  DEBUG(1, "Connected! Name: %s\n", cd->conn_name);

  /* connection_setup */
  dbus_connection_set_exit_on_disconnect(cd->conn, FALSE);
  dbus_connection_allocate_data_slot(&connection_slot);

  dbus_connection_set_data(cd->conn, connection_slot, (void *)cd, _e_dbus_connection_free);
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
  dbus_connection_add_filter(cd->conn, e_dbus_filter, NULL, NULL);

  cb_dispatch_status(cd->conn, dbus_connection_get_dispatch_status(cd->conn), cd);

  return 1;
}


void
e_dbus_connection_close(DBusConnection *conn)
{
  E_DBus_Connection *cd;
  cd = dbus_connection_get_data(conn, connection_slot);

  DEBUG(5, "_e_dbus_connection_close\n");

  dbus_connection_free_data_slot(&connection_slot);
  dbus_connection_remove_filter(cd->conn, e_dbus_filter, NULL);
  dbus_connection_set_watch_functions (conn,
                                       NULL,
                                       NULL,
                                       NULL,
                                       NULL, NULL);

  dbus_connection_set_timeout_functions (conn,
                                         NULL,
                                         NULL,
                                         NULL,
                                         NULL, NULL);

  dbus_connection_set_dispatch_status_function (conn, NULL, NULL, NULL);

  dbus_connection_close(conn);
  dbus_connection_unref(conn);
}

int
e_dbus_init(void)
{
  int i;
  if (++init != 1) return init;

  E_DBUS_EVENT_SIGNAL = ecore_event_type_new();
  e_dbus_signal_init();
  return init;
}

void
e_dbus_shutdown(void)
{
  if (--init) return;
  e_dbus_signal_shutdown();
}
