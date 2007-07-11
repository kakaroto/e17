#include "E_Notify.h"
#include "E_Notification_Daemon.h"
#include "e_notify_private.h"

/**** daemon api ****/
void
e_notification_signal_notification_closed(E_Notification_Daemon *daemon, unsigned int id, E_Notification_Closed_Reason reason)
{
  e_dbus_message_send(daemon->conn, 
                      e_notify_marshal_notification_closed_signal(id, reason),
                      NULL, -1, NULL);
}

void
e_notification_signal_action_invoked(E_Notification_Daemon *daemon, unsigned int notification_id, const char *action_id)
{
  e_dbus_message_send(daemon->conn, 
                      e_notify_marshal_action_invoked_signal(notification_id, action_id),
                      NULL, -1, NULL);
}
