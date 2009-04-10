#ifndef E_MOD_POPUP_H
#define E_MOD_POPUP_H

struct _Popup_Data
{
  E_Notification *notif;
  E_Popup *win;
  Evas *e;
  Evas_Object *theme;
  const char  *app_name;
  Evas_Object *app_icon;
  Ecore_Timer *timer;
};

int   notification_popup_notify    (E_Notification *n, 
                                    unsigned int replaces_id, 
                                    unsigned int id,
                                    char *appname);
void  notification_popup_shutdown  (void);
void  notification_popup_close     (unsigned int id);

#endif

