#include <E_DBus.h>
#include <E_Notify.h>

static dbus_uint32_t msg_num = 0;

void
cb_sent(void *data, void *ret)
{
  E_Notification_Return_Notify *notify;
  notify = ret;
  printf("id: %d\n", notify->notification_id);
  ecore_main_loop_quit();
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
  {
    E_Notification *n;
    E_Notification_Context *cl;

    cl = malloc(sizeof(E_Notification_Context));
    cl->conn = conn;
    n = e_notification_full_new("Elicit", 0, "elicit", "Summary", "The <b>body</b>", -1);
    e_notification_send(cl, n, cb_sent, NULL);
    ecore_main_loop_begin();
  }
  else
  {
    printf("Error: could not connect to session bus.\n");
    ret = 1;
  }

  e_dbus_shutdown();
  ecore_shutdown();
  return ret;
}
