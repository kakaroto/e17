#include <Ecore.h>
#include "E_DBus.h"
#include <stdio.h>
#include <string.h>
#include <dbus/dbus.h>

void
copy_message(DBusMessageIter *from, DBusMessageIter *to)
{
  int type;
  printf("  copy message\n");
  while((type = dbus_message_iter_get_arg_type(from)) != DBUS_TYPE_INVALID)
  {
    printf("  copy type: %c\n", type);
    if (dbus_type_is_basic(type))
    {
      /* XXX is int64 big enough to hold all basic types? */
      dbus_int64_t val;
      dbus_message_iter_get_basic(from, &val);
      dbus_message_iter_append_basic(to, type, &val);
    }
    else if (dbus_type_is_container(type))
    {
      int subtype;

      subtype = dbus_message_iter_get_element_type(from);
      if (type == DBUS_TYPE_ARRAY && dbus_type_is_fixed(subtype))
      {
        int n;
        void *val;
        dbus_message_iter_get_fixed_array(from, &val, &n);
        dbus_message_iter_append_fixed_array(to, subtype, val, n);
      }
      else
      {
        DBusMessageIter fsub, tsub;
        char *sig;
        dbus_message_iter_recurse(from, &fsub);
        sig = dbus_message_iter_get_signature(&fsub);
        dbus_message_iter_open_container(to, type, sig, &tsub);
        copy_message(&fsub, &tsub);
        dbus_message_iter_close_container(to, &tsub);
      }
    }
    dbus_message_iter_next(from);
  }
}

DBusMessage *
cb_repeat(E_DBus_Object *obj, DBusMessage *msg)
{
  DBusMessage *reply;
  DBusMessageIter from, to;

  printf("\n\nREPEAT\n--------\n");
  reply = dbus_message_new_method_return(msg);
  dbus_message_iter_init(msg, &from);
  dbus_message_iter_init_append(reply, &to);

  copy_message(&from, &to);
  return reply;
}

void
cb_request_name(void *data, DBusMessage *msg, DBusError *err)
{
  // XXX check that this actually succeeded and handle errors...
  printf("request name\n");
}

int
_setup(E_DBus_Connection *conn)
{
  E_DBus_Object *repeater;
  E_DBus_Interface *iface;
  e_dbus_request_name(conn, "org.e.Repeater", 0, cb_request_name, NULL);
  repeater = e_dbus_object_add(conn, "/org/e/Repeater", NULL);
  iface = e_dbus_interface_new("org.e.Repeater");
  e_dbus_interface_method_add(iface, "Repeat", NULL, NULL, cb_repeat);
  e_dbus_object_interface_attach(repeater, iface);
  return 1;
}

int
main (int argc, char ** argv)
{
  E_DBus_Connection *conn;
  ecore_init();
  e_dbus_init();

  conn = e_dbus_bus_get(DBUS_BUS_SESSION);
 
  if (conn)
  {
    if (_setup(conn)) ecore_main_loop_begin();
    e_dbus_connection_close(conn);
  }

  e_dbus_shutdown();
  ecore_shutdown();

  return 0;
}
