#ifndef E_NOTIFY_H
#define E_NOTIFY_H

#include <Ecore_Data.h>
#include <Evas.h>
#include <E_DBus.h>

#define E_NOTIFICATION_INTERFACE "org.freedesktop.Notifications"
#define E_NOTIFICATION_DESTINATION "org.freedesktop.Notifications"
#define E_NOTIFICATION_PATH "/org/freedesktop/Notifications"

typedef struct E_Notification_Context E_Notification_Daemon;

/* notifications */

typedef struct E_Notification_Context E_Notification_Context;

typedef struct E_Notification_Image E_Notification_Image;
typedef struct E_Notification E_Notification;
typedef struct E_Notification_Action E_Notification_Action;


typedef enum E_Notification_Urgency E_Notification_Urgency;
typedef enum E_Notification_Hint_Type E_Notification_Hint_Type;
typedef enum E_Notification_Closed_Reason E_Notification_Closed_Reason;

/* method returns */
typedef struct E_Notification_Return_Notify E_Notification_Return_Notify;
typedef struct E_Notification_Return_Get_Capabilities E_Notification_Return_Get_Capabilities;
typedef struct E_Notification_Return_Get_Server_Information E_Notification_Return_Get_Server_Information;

/* daemon events */
typedef struct E_Notification_Event_Daemon E_Notification_Event_Daemon;
typedef struct E_Notification_Event_Daemon E_Notification_Event_Notify;
typedef struct E_Notification_Event_Daemon E_Notification_Event_Close;

/* signal events */
typedef struct E_Notification_Event_Action_Invoked E_Notification_Event_Action_Invoked;
typedef struct E_Notification_Event_Notification_Closed E_Notification_Event_Notification_Closed;

/* gui */
typedef struct E_Notification_View E_Notification_View;

/* enums */

enum E_Notification_Urgency
{
  E_NOTIFICATION_URGENCY_LOW,
  E_NOTIFICATION_URGENCY_NORMAL,
  E_NOTIFICATION_URGENCY_CRITICAL
};

enum E_Notification_Closed_Reason
{
  E_NOTIFICATION_CLOSED_EXPIRED,
  E_NOTIFICATION_CLOSED_DISMISSED,
  E_NOTIFICATION_CLOSED_REQUESTED,
  E_NOTIFICATION_CLOSED_UNDEFINED
};

enum E_Notification_Hint_Type
{
  E_NOTIFICATION_HINT_URGENCY        = 0x1,
  E_NOTIFICATION_HINT_CATEGORY       = 0x2,
  E_NOTIFICATION_HINT_DESKTOP        = 0x4,
  E_NOTIFICATION_HINT_SOUND_FILE     = 0x8,
  E_NOTIFICATION_HINT_SUPPRESS_SOUND = 0x10,
  E_NOTIFICATION_HINT_XY             = 0x20,
  E_NOTIFICATION_HINT_IMAGE_DATA     = 0x40
};

struct E_Notification_Context
{
  E_DBus_Connection *conn;
};

struct E_Notification_Image
{
  int   width;
  int   height;
  int   rowstride;
  char  has_alpha;
  int   bits_per_sample;
  int   channels;
  int  *data;
};

struct E_Notification
{
  int id;
  char *app_name;
  unsigned int replaces_id;
  char *app_icon;
  char *summary;
  char *body;
  int expire_timeout;

  Ecore_List *actions;

  struct
  {
    char urgency;
    char *category;
    char *desktop;
    char *sound_file;
    char suppress_sound;
    int x, y;
    E_Notification_Image *image_data;
  } hints;

  int hint_flags;

  int refcount;
};

struct E_Notification_Action 
{
  char *id;
  char *name;
};

/* daemon event data */
struct E_Notification_Event_Daemon
{
  E_Notification *notification;
};

/* client method returns */
struct E_Notification_Return_Notify
{
  unsigned int notification_id;
  E_Notification *notification;
};

struct E_Notification_Return_Get_Capabilities
{
  Ecore_List *capabilities;
};

struct E_Notification_Return_Get_Server_Information
{
  const char *name;
  const char *vendor;
  const char *version;
};

/* signals */
struct E_Notification_Event_Notification_Closed
{
  int notification_id;
  E_Notification_Closed_Reason reason;
};

struct E_Notification_Event_Action_Invoked
{
  int notification_id;
  char *action_id;
};



/* daemon events */
extern int E_NOTIFICATION_EVENT_NOTIFY;
extern int E_NOTIFICATION_EVENT_CLOSE;

/* client events */
extern int E_NOTIFICATION_EVENT_CLOSED;
extern int E_NOTIFICATION_EVENT_ACTION_INVOKED;


int e_notify_init();
void e_notifications_shutdown();

/* daemon */
E_Notification_Daemon *e_notification_daemon_add(DBusConnection *conn);
void e_notification_daemon_free(E_Notification_Daemon *d);
void e_notification_daemon_close(E_Notification_Daemon *d,
                                 E_Notification *n, unsigned int reason);
void e_notification_daemon_action_invoke(E_Notification_Daemon *d,
                                 E_Notification *n, const char *action_id);

/* client */
/*
E_Notification_Context *e_notification_client_add();
void e_notification_client_shutdown(E_Notification_Context *t);
void e_notifications_get_capabilities(E_Notification_Context *nc);
void e_notifications_get_server_info(E_Notification_Context *nc);
void e_notifications_close_notification(E_Notification_Context *nc, int id);
*/


/* Notifications */

E_Notification *e_notification_new();
void e_notification_ref(E_Notification *n);
void e_notification_unref(E_Notification *n);
void e_notification_free(E_Notification *n);

E_Notification *e_notification_full_new(const char *app_name,
                                        unsigned int replaces_id,
                                        const char *app_icon,
                                        const char *summary,
                                        const char *body,
                                        int expire_timeout);

/* notification mutators */
void e_notification_app_name_set(E_Notification *n, const char *app_name);
void e_notification_app_icon_set(E_Notification *n, const char *app_icon);
void e_notification_summary_set(E_Notification *n, const char *summary);
void e_notification_body_set(E_Notification *n, const char *body);
void e_notification_replaces_id_set(E_Notification *n, int replaces_id);
void e_notification_timeout_set(E_Notification *n, int timeout);

/* notification accessors */
const char *e_notification_app_name_get(E_Notification *n);
const char *e_notification_app_icon_get(E_Notification *n);
const char *e_notification_summary_get(E_Notification *n);
const char *e_notification_body_get(E_Notification *n);
int e_notification_timeout_get(E_Notification *note);
int e_notification_replaces_id_get(E_Notification *note);

/* actions */
void e_notification_action_add(E_Notification *n, const char *action_id, const char *action_name);
Ecore_List *e_notification_actions_get(E_Notification *n);

/* hint mutators */
void e_notification_hint_urgency_set(E_Notification *n, char urgency);
void e_notification_hint_category_set(E_Notification *n, const char *category);
void e_notification_hint_desktop_set(E_Notification *n, const char *desktop);
void e_notification_hint_sound_file_set(E_Notification *n, const char *sound_file);
void e_notification_hint_suppress_sound_set(E_Notification *n, char suppress_sound);
void e_notification_hint_xy_set(E_Notification *n, int x, int y);
void e_notification_hint_image_data_set(E_Notification *n, E_Notification_Image *image);

/* hint accessors */
char  e_notification_hint_urgency_get(E_Notification *n);
const char *e_notification_hint_category_get(E_Notification *n);
const char *e_notification_hint_desktop_get(E_Notification *n);
const char *e_notification_hint_sound_file_get(E_Notification *n);
char  e_notification_hint_suppress_sound_get(E_Notification *n);
void  e_notification_hint_x_get(E_Notification *n, int *x, int *y);
E_Notification_Image *e_notification_hint_image_data_get(E_Notification *n);

/* image hint */
E_Notification_Image *e_notification_image_new();
void e_notification_image_free(E_Notification_Image *img);
Evas_Object *e_notification_image_object_add(Evas *evas, E_Notification_Image *img);

/***** gui *****/
E_Notification_View *e_notification_view_add(E_Notification_Daemon *d, E_Notification *n);
void e_notification_view_close(E_Notification_View *nv);
Evas_Object * e_notification_view_icon_get(Evas *evas, E_Notification *n);

void e_notification_send(E_Notification *n, E_DBus_Callback_Func func, void *data);

#endif
