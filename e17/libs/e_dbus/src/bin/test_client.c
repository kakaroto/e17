#include <E_DBus.h>

#define NUM_LOOPS 10000

static dbus_uint32_t msg_num = 0;

void
cb_reply(void *data, DBusMessage *reply, DBusError *error)
{
  dbus_uint32_t val;
  if (dbus_error_is_set(error))
  {
    printf("Error: %s - %s\n", error->name, error->message);
    return;
  }

  dbus_message_get_args(reply, error, DBUS_TYPE_UINT32, &val, DBUS_TYPE_INVALID);
  printf("Received: %d\n", val);
  if (val == NUM_LOOPS - 1) ecore_main_loop_quit();
}

int
send_message(void *data)
{

  DBusMessage *msg;
  E_DBus_Connection *conn;
  
  conn = data;

  msg = dbus_message_new_method_call(
    "org.e.Repeater",
    "/org/e/Repeater",
    "org.e.Repeater",
    "Repeat"
  );

  dbus_message_append_args(msg, DBUS_TYPE_UINT32, &msg_num, DBUS_TYPE_INVALID);
  msg_num++;
  e_dbus_message_send(conn, msg, cb_reply, -1, NULL);
  dbus_message_unref(msg);
  printf("Sent: %d\n", msg_num);
  return 1;
}

int
main(int argc, char **argv)
{
  E_DBus_Connection *conn;
  int ret = 0;
  ecore_init();
  e_dbus_init();

  conn = e_dbus_bus_get(DBUS_BUS_SESSION);
  if (conn)
    ecore_timer_add(0.0, send_message, conn);
  else
  {
    printf("Error: could not connect to session bus.\n");
    ret = 1;
  }

  ecore_main_loop_begin();

  e_dbus_shutdown();
  ecore_shutdown();
  return ret;
}
