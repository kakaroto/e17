#ifndef E_NOTIFICATION_DAEMON_H
#define E_NOTIFICATION_DAEMON_H

/* daemon events */
extern int E_NOTIFICATION_EVENT_NOTIFY;
extern int E_NOTIFICATION_EVENT_CLOSE;


typedef struct E_Notification_Context E_Notification_Daemon;

/* daemon events */
typedef struct E_Notification_Event_Daemon E_Notification_Event_Daemon;
typedef struct E_Notification_Event_Daemon E_Notification_Event_Notify;
typedef struct E_Notification_Event_Daemon E_Notification_Event_Close;

/* gui */
typedef struct E_Notification_View E_Notification_View;

struct E_Notification_Context
{
  E_DBus_Connection *conn;
};

/* daemon event data */
struct E_Notification_Event_Daemon
{
  E_Notification *notification;
};

/* daemon */
E_Notification_Daemon *e_notification_daemon_add(DBusConnection *conn);
void e_notification_daemon_free(E_Notification_Daemon *d);
void e_notification_daemon_close(E_Notification_Daemon *d,
                                 E_Notification *n, unsigned int reason);
void e_notification_daemon_action_invoke(E_Notification_Daemon *d,
                                 E_Notification *n, const char *action_id);


/***** gui *****/
E_Notification_View *e_notification_view_add(E_Notification_Daemon *d, E_Notification *n);
void e_notification_view_close(E_Notification_View *nv);
Evas_Object * e_notification_view_icon_get(Evas *evas, E_Notification *n);

#endif
