#ifndef E_NOTIFICATION_DAEMON_H
#define E_NOTIFICATION_DAEMON_H

#define E_NOTIFICATION_DAEMON_VERSION "0.9"

#include <E_Notify.h>

#ifdef EAPI
#undef EAPI
#endif
#ifdef _MSC_VER
# ifdef BUILDING_DLL
#  define EAPI __declspec(dllexport)
# else
#  define EAPI __declspec(dllimport)
# endif
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif


typedef struct E_Notification_Daemon E_Notification_Daemon;

/* daemon callbacks */
typedef int (*E_Notification_Daemon_Callback_Notify) (E_Notification_Daemon *daemon, E_Notification *notification);
typedef void (*E_Notification_Daemon_Callback_Close_Notification) (E_Notification_Daemon *daemon, unsigned int notification_id);

/* gui */
typedef struct E_Notification_View E_Notification_View;

struct E_Notification_Daemon
{
  E_DBus_Connection *conn;
  E_DBus_Interface *iface;
  E_DBus_Object *obj;

  char *name;
  char *vendor;

  struct 
  {
    E_Notification_Daemon_Callback_Notify notify;
    E_Notification_Daemon_Callback_Close_Notification close_notification;
  } func;
  void *data;

  int state;
};

#ifdef __cplusplus
extern "C" {
#endif

/* daemon */
   EAPI E_Notification_Daemon *e_notification_daemon_add(const char *name, const char *vendor);
   EAPI void e_notification_daemon_free(E_Notification_Daemon *d);
/* TODO
   void e_notification_daemon_close(E_Notification_Daemon *d,
   E_Notification *n, unsigned int reason);
   void e_notification_daemon_action_invoke(E_Notification_Daemon *d,
   E_Notification *n, const char *action_id);
*/

   EAPI void  e_notification_daemon_data_set(E_Notification_Daemon *daemon, void *data);
   EAPI void *e_notification_daemon_data_get(E_Notification_Daemon *daemon);

   EAPI void e_notification_daemon_callback_notify_set(E_Notification_Daemon *daemon, E_Notification_Daemon_Callback_Notify func);
   EAPI void e_notification_daemon_callback_close_notification_set(E_Notification_Daemon *daemon, E_Notification_Daemon_Callback_Close_Notification func);

   EAPI void e_notification_daemon_signal_notification_closed(E_Notification_Daemon *daemon, unsigned int id, E_Notification_Closed_Reason reason);
   EAPI void e_notification_daemon_signal_action_invoked(E_Notification_Daemon *daemon, unsigned int notification_id, const char *action_id);

/***** gui *****/
/* TODO
   E_Notification_View *e_notification_view_add(E_Notification_Daemon *d, E_Notification *n);
   void e_notification_view_close(E_Notification_View *nv);
   Evas_Object * e_notification_view_icon_get(Evas *evas, E_Notification *n);
*/

#ifdef __cplusplus
}
#endif

#endif
