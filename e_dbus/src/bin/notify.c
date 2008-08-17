#include <E_DBus.h>
#include <E_Notify.h>

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

  free(notify);
}

int
cb_timer(void *data)
{
  E_Notification *n;
  char buf[1024];
  static int num = 0;
  static const char *icons[] = {
    "xterm",
    "firefox",
    "gvim"
  };

  snprintf(buf, sizeof(buf), "<i>%s</i> says <b>Hello</b> #%d", icons[num%3], num / 3); 
  n = e_notification_full_new(icons[num%3], 0, icons[num%3], "Summary", buf, -1);
  e_notification_send(n, cb_sent, NULL);
  e_notification_unref(n);
  num++;

  return 1;
}

void
cb_action_invoked(void *data, int type, void *event)
{
  E_Notification_Event_Action_Invoked *ev;

  ev = event;
  printf("Action (%d): %s\n", ev->notification_id, ev->action_id);
  free(ev);
}

void
cb_note_closed(void *data, int type, void *event)
{
  E_Notification_Event_Notification_Closed *ev;
  static const char *reasons[] = {
    "Expired",
    "Dismissed",
    "Requested",
    "Undefined"
  };

  ev = event;
  printf("Note %d closed: %s\n", ev->notification_id, reasons[ev->reason]);
  free(ev);
}

int
main(int argc, char **argv)
{
  int ret = 0;
  ecore_init();
  if (e_notification_init())
  {
    ecore_timer_add(1, cb_timer, NULL);
    ecore_main_loop_begin();
    e_notification_shutdown();
  }

  ecore_shutdown();
  return ret;
}
