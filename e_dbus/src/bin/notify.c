#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <stdio.h>

#include <Evas.h>
#include <Ecore.h>
#include <Ecore_Evas.h>
#include <E_DBus.h>
#include <E_Notify.h>

#define LOGO PACKAGE_DATA_DIR "/logo.png"

void
cb_sent(void *data __UNUSED__, void *ret, DBusError *err)
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
}

Eina_Bool
cb_timer(void *data __UNUSED__)
{
  E_Notification *n;
  char buf[1024];
  static int num = 0;
  static const char *icons[] = {
    "xterm",
    "firefox",
    "gvim",
    "e"
  };

  snprintf(buf, sizeof(buf), "<i>%s</i> says <b>Hello</b> #%d", icons[num % 4], num % 4); 
  n = e_notification_full_new(icons[num % 4], 0, (icons[num % 4][0] != 'e') ? icons[num % 4] : NULL, "Summary", buf, -1);

  if (!e_notification_app_icon_get(n))
    {
       Ecore_Evas *ee;
       Evas_Object *img;
       E_Notification_Image *i;
       ee = ecore_evas_buffer_new(1, 1);
       if (ee)
         {
            img = evas_object_image_add(ecore_evas_get(ee));
            evas_object_image_file_set(img, LOGO, NULL);
            if (evas_object_image_load_error_get(img) != EVAS_LOAD_ERROR_NONE)
              evas_object_image_file_set(img, "logo.png", NULL);
            if (evas_object_image_load_error_get(img) != EVAS_LOAD_ERROR_NONE)
              {
                 fprintf(stderr, "ERROR LOADING IMAGE: %s\n", LOGO);
                 evas_object_del(img);
                 img = NULL;
              }
            else
              ecore_evas_manual_render(ee);

            i = e_notification_image_new();
            if (e_notification_image_init(i, img))
              e_notification_hint_image_data_set(n, i);
           evas_object_del(img);
           ecore_evas_free(ee);
         }
    }

  e_notification_send(n, cb_sent, NULL);
  e_notification_unref(n);
  num++;

  return ECORE_CALLBACK_RENEW;
}

void
cb_action_invoked(void *data __UNUSED__, int type __UNUSED__, void *event)
{
  E_Notification_Event_Action_Invoked *ev;

  ev = event;
  printf("Action (%d): %s\n", ev->notification_id, ev->action_id);
  free(ev);
}

void
cb_note_closed(void *data __UNUSED__, int type __UNUSED__, void *event)
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
main()
{
  int ret = 0;
  ecore_init();
  ecore_evas_init();
  if (e_notification_init())
  {
    ecore_timer_add(1, cb_timer, NULL);
    ecore_main_loop_begin();
    e_notification_shutdown();
  }

  ecore_evas_shutdown();
  ecore_shutdown();
  return ret;
}
