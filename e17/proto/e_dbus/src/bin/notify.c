#include <E_DBus.h>
#include <E_Notify.h>

static dbus_uint32_t msg_num = 0;

void
cb_sent(void *data, void *ret, DBusError *err)
{
  E_Notification_Return_Notify *notify;
  notify = ret;
  if (notify) 
  {
    printf("id: %d\n", notify->notification_id);
  }
  else if (dbus_error_is_set(err))
  {
    printf("Error: %s\n", err->message);
  }
  ecore_main_loop_quit();
}

int
main(int argc, char **argv)
{
  int ret = 0;
  E_Notification *n;
  ecore_init();
  if (e_notification_init())
  {
    n = e_notification_full_new("Elicit", 0, "elicit", "Summary", "The <b>body</b>", -1);
    e_notification_send(n, cb_sent, NULL);
    ecore_main_loop_begin();
    e_notification_shutdown();
  }

  ecore_shutdown();
  return ret;
}
