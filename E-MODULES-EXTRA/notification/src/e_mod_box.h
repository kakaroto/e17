#ifndef E_MOD_BOX_H
#define E_MOD_BOX_H

struct _Notification_Box
{
  const char     *id;
  Instance       *inst;
  Evas_Object    *o_box;
  Evas_Object    *o_empty;
  Eina_List      *icons;
};

struct _Notification_Box_Icon
{
  Notification_Box *n_box;
  unsigned int      n_id;
  const char       *label;
  Evas_Object      *o_holder;
  Evas_Object      *o_icon;
  Evas_Object      *o_holder2;
  Evas_Object      *o_icon2;
  E_Border         *border;
  E_Notification   *notif;

  int             popup;
  Ecore_Timer    *mouse_in_timer;
};

int                notification_box_notify              (E_Notification *n, 
                                                         unsigned int replaces_id, 
                                                         unsigned int id);
void               notification_box_shutdown            (void);
void               notification_box_del                 (const char *id);
void               notification_box_show                (Notification_Box *b);
void               notification_box_hide                (Notification_Box *b);
Notification_Box  *notification_box_get                 (const char *id, Evas *evas);
Config_Item       *notification_box_config_item_get     (const char *id);
void               notification_box_orient_set          (Notification_Box *b, int horizontal);

void               notification_box_cb_obj_moveresize   (void *data, 
                                                         Evas *e, 
                                                         Evas_Object *obj, 
                                                         void *event_info);
int                notification_box_cb_border_remove    (void *data, 
                                                         int type, 
                                                         void *event);


#endif

