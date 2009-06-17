/***************************************************************************
 * Corey Donohoe : http://atmos.org/
 * File: main.c
 * Date: January 21, 2004
 *
 * The ecore + libpbb glue for epbb.  This app is VERY simple, and works
 * like the following. 
 * 1. Establish a connection to pbbuttons
 * 2. Initialize ecore/edje/etc
 * 3. Create an Ecore_Evas
 * 4. Setup a battery monitoring timer
 * 5. Setup a client listener callback
 * 6. Add the edje theme to the ecore_evas
 * 7. Wait for events and pipe them through to edje when they arrive
 **************************************************************************/
#include<stdio.h>
#include<pbb.h>
#include<Ecore.h>
#include<Ecore_X.h>
#include<Ecore_Evas.h>
#include<Edje.h>
#include<limits.h>
#include"epbb.h"
#include"../config.h"

#define VOLUME_MAX 100
#define BACKLIGHT_MAX 15

#define UN(ptr) ptr = 0
#define PBBUTTONSD_TIMEOUT 0.25
#define PBBUTTONSD_BATTERY_TIMEOUT 15.0

static Epbb *epbb = NULL;

static int
app_exit_cb(void *data, int ev_type, void *ev)
{
   Ecore_Event_Signal_Exit *e = NULL;

   e = ev;
   ipc_exit();
   ecore_main_loop_quit();
   return (0);
   UN(ev_type);
   UN(data);
}

static void
interp_changed_taglist(struct tagitem *t)
{
   while (t->tag != TAG_END)
   {
      switch (t->tag)
      {
        case TAG_BRIGHTNESS:
           epbb_brightness_set(epbb, (double) t->data / BACKLIGHT_MAX);
           break;
        case TAG_VOLUME:
           epbb_volume_set(epbb, (double) t->data / VOLUME_MAX);
           break;
        case TAG_MUTE:
           epbb_mute_set(epbb);
           break;
        case TAG_POWERSOURCE:
           epbb_battery_source_set(epbb, (int) t->data);
           break;
        case TAG_TIMEREMAINING:
           epbb_battery_remaining_set(epbb, (int) t->data);
           break;
        default:
           printf("%0.4f is changed t->tag\n", (double) t->tag);
           break;
      }
      t++;
   }
}

static void
interp_warning_taglist(struct tagitem *t)
{
   while (t->tag != TAG_END)
   {
      switch (t->tag)
      {
        case TAG_CURRENTBWL:
           epbb_warning_battery_set(epbb, (int) t->data);
           break;
        case TAG_SLEEPINSECONDS:
           epbb_warning_sleep_set(epbb, (int) t->data);
           break;
        case TAG_TIMEREMAINING:
           /* 
              epbb_battery_remaining_set(epbb, (int) t->data); */
           break;
        default:
           printf("%0.4f is warning t->tag\n", (double) t->tag);
           break;
      }
      t++;
   }
}

static int
pbbuttonsd_battery_check(void *data)
{
   struct tagitem tagrequest[2];

   taglist_init(tagrequest);
   taglist_add(tagrequest, TAG_TIMEREMAINING, 0);
   taglist_add(tagrequest, TAG_POWERSOURCE, 0);
   ipc_send(0, READVALUE, tagrequest);
   return (1);
   UN(data);
}

static int
pbbuttonsd_timeout(void *data)
{
   char *msgbuffer[100];
   struct pbbmessage *m = (struct pbbmessage *) msgbuffer;

   if ((ipc_receive(msgbuffer, sizeof(msgbuffer))) == 0)
   {
      switch (m->action)
      {
        case REGFAILED:
           printf("Can't register client at server\n");
           break;
        case CLIENTEXIT:
           ecore_main_loop_quit();
           return (0);
           break;
        case CHANGEVALUE:
           interp_changed_taglist(m->taglist);
           break;
        case CHANGEERROR:
           fprintf(stderr, "ERROR: CHANGEERROR\n");
           break;
        case WARNING:
           interp_warning_taglist(m->taglist);
           break;
        default:
           printf("Received %d action\n", (int) m->action);
           break;
      }
   }
   return (1);
   data = NULL;
}

static void
window_delete_cb(Ecore_Evas * ee)
{
   ecore_main_loop_quit();
   return;
   UN(ee);
}

static void
window_resize_cb(Ecore_Evas * ee)
{
   if (ee)
   {
      int w, h;
      Evas_Object *o = NULL;

      ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
      if ((o = evas_object_name_find(ecore_evas_get(ee), "edje")))
      {
         evas_object_resize(o, w, h);
      }
   }
}

static void
window_show_cb(Ecore_Evas * ee)
{
   if (ee)
   {
      int sw, sh, w, h;

      ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
      ecore_x_window_size_get(0, &sw, &sh);
      ecore_evas_move(ee, (sw - w) / 2, (sh - h) / 2);
   }
   return;
   UN(ee);
}

static void
window_hide_cb(Ecore_Evas * ee)
{
   return;
   UN(ee);
}

int
main(int argc, const char *argv[])
{
   int rc = -1;

   init_libpbb();
   if ((rc = ipc_init(EPBB_IPCNAME, LIBMODE_CLIENT, 1)) != 0)
   {
      switch (rc)
      {
        case E_NOSERVER:
           printf("Server message port not found\n");
           break;
        case E_REGISTER:
           printf("Client list full.\n");
           break;
        default:
           break;
      }
   }
   ecore_init();
   ecore_app_args_set(argc, (const char **) argv);
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, app_exit_cb, NULL);
   ecore_timer_add(PBBUTTONSD_TIMEOUT, pbbuttonsd_timeout, NULL);
   ecore_timer_add(PBBUTTONSD_BATTERY_TIMEOUT, pbbuttonsd_battery_check,
                   NULL);
   edje_init();
   if (ecore_evas_init())
   {
      Ecore_Evas *ee;
      Evas *evas = NULL;
      int w = 300, h = 80;

      ee = ecore_evas_software_x11_new(NULL, 0, 0, 0, w, h);
      ecore_evas_title_set(ee, PACKAGE);
      ecore_evas_name_class_set(ee, PACKAGE, "status");
      ecore_evas_borderless_set(ee, 1);
      ecore_evas_shaped_set(ee, 1);
      ecore_evas_callback_delete_request_set(ee, window_delete_cb);
      ecore_evas_callback_resize_set(ee, window_resize_cb);
      ecore_evas_callback_show_set(ee, window_show_cb);
      ecore_evas_callback_hide_set(ee, window_hide_cb);
      evas = ecore_evas_get(ee);
      evas_image_cache_set(evas, 512 * 1024);
      evas_font_cache_set(evas, 512 * 1024);
      evas_font_path_append(evas, PACKAGE_DATA_DIR "/fonts/");

      epbb = epbb_new(ee);
   }
   ecore_main_loop_begin();
   epbb_free(epbb);
   edje_shutdown();
   ecore_evas_shutdown();
   ecore_shutdown();
   exit_libpbb();
   return (0);
}
