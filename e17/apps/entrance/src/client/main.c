/**
@file main.c
@brief When entrance starts, and ui specific variables
*/
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <Edje.h>
#include <Esmart/container.h>
#include "entrance.h"
#include "entrance_session.h"
#include "EvasTextEntry.h"

#define WINW 800
#define WINH 600

static Entrance_Session *session = NULL;

/* Callbacks for entrance */
static char *
get_my_hostname(void)
{
   char buf[255];               /* some standard somewhere limits hostname
                                   lengths to this */
   char *dot;
   char message[PATH_MAX];

   char *result = NULL;

   if (!(gethostname(buf, 255)))
   {
      /* Ensure that hostname is in short form */
      dot = strstr(buf, ".");
      if (dot)
         *dot = '\0';

      snprintf(message, PATH_MAX, "%s %s %s", session->config->before.string,
               buf, session->config->after.string);
   }
   else
      snprintf(message, PATH_MAX, "%s Localhost %s",
               session->config->before.string, session->config->after.string);
   result = strdup(message);
   return (result);
}

/**
 * exit_cb - waht to do if we kill it or something?
 * @data - no clue
 * @ev_type - kill event ?
 * @ev - event data
 * Obviously I want to exit here.
 */
static int
exit_cb(void *data, int ev_type, void *ev)
{
   ecore_main_loop_quit();
//    exit(0);
   return 1;
}

/**
 * window_del_cb - what to do when we receive a window delete event
 * @ee - the Ecore_Evas that received the event
 */
static void
window_del_cb(Ecore_Evas * ee)
{
   ecore_main_loop_quit();
//    exit(0);
}

/**
 * window_resize_cb - handle when the ecore_evas needs to be resized
 * @ee - The Ecore_Evas we're resizing 
 */
static void
window_resize_cb(Ecore_Evas * ee)
{
   Evas_Object *o = NULL;

   if ((o = evas_object_name_find(ecore_evas_get(ee), "ui")))
   {
      int w, h;

      ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);
      evas_object_resize(o, w, h);
   }
}

/**
 * focus_swap - swap key input focus between the password and user
 * entries
 * @o - the object we want to swap focus with
 * @selecto - whether to focus on o, or the other entry
 */
static void
focus_swap(Evas_Object * o, int selecto)
{
   Evas_Object *oo = NULL;
   Evas_Text_Entry *e = NULL;

   if ((e = evas_object_smart_data_get(o)))
   {
      if (!strcmp(e->edje.part, "EntrancePassEntry"))
      {
         if ((oo =
              evas_object_name_find(evas_object_evas_get(o),
                                    "EntranceUserEntry")))
         {
            evas_text_entry_text_set(oo, "");
         }
         evas_text_entry_text_set(o, "");
      }
      else if (!strcmp(e->edje.part, "EntranceUserEntry"))
      {
         oo =
            evas_object_name_find(evas_object_evas_get(o),
                                  "EntrancePassEntry");
      }
   }
   if (oo)
   {
      selecto ? evas_text_entry_focus_set(oo,
                                          0) : evas_text_entry_focus_set(o,
                                                                         0);

      selecto ? evas_text_entry_focus_set(o,
                                          1) : evas_text_entry_focus_set(oo,
                                                                         1);
   }
}

/**
 * interp_return_key - when Enter is hit on the keyboard we end up here
 * @data - The smart object that is this Entry
 * @str - The string that was in the buffer when Enter was pressed
 */
static void
interp_return_key(void *data, const char *str)
{
   Evas_Object *o = NULL;
   Evas_Text_Entry *e = NULL;

   o = (Evas_Object *) data;

   if ((e = evas_object_smart_data_get(o)))
   {
      if (!strcmp(e->edje.part, "EntranceUserEntry"))
      {
         if (!entrance_auth_set_user(session->auth, str))
         {
            edje_object_signal_emit(e->edje.o, "EntranceUserAuth", "");
            focus_swap(o, 0);
         }
         else
         {
            evas_text_entry_text_set(o, "");
            entrance_session_user_reset(session);
            edje_object_signal_emit(e->edje.o, "EntranceUserFail", "");
            focus_swap(o, 1);
         }
      }
      if (!strcmp(e->edje.part, "EntrancePassEntry"))
      {
         if (session->auth->user && strlen(session->auth->user) > 0)
         {
            entrance_auth_set_pass(session->auth, str);
            if (!entrance_session_auth_user(session))
            {
               edje_object_signal_emit(e->edje.o, "EntranceUserAuthSuccess",
                                       "");
               session->authed = 1;
            }
            else
            {
               entrance_session_user_reset(session);
               edje_object_signal_emit(e->edje.o, "EntranceUserAuthFail", "");
               focus_swap(o, 0);
            }
         }
      }
   }
}

/**
 * focus - an edje signal emission 
 * @data - the data passed when the callback was added
 * @o - the evas object(Edje) that created the signal
 * @emission - the signal "type" that was emitted
 * @source - the signal originated from this "part"
 */
static void
focus(void *data, Evas_Object * o, const char *emission, const char *source)
{
   Evas_Object *oo = NULL;      /* in nexus this looks in infinity ! */

   if ((oo = (Evas_Object *) data))
   {
      if (!strcmp(emission, "In"))
      {
         if (!evas_text_entry_is_focused(oo))
         {
            evas_text_entry_focus_set(oo, 1);
         }
      }
      else if (!strcmp(emission, "Out"))
      {
         if (evas_text_entry_is_focused(oo))
         {
            evas_text_entry_focus_set(oo, 1);
            evas_text_entry_focus_set(oo, 0);
         }
      }
      else
         fprintf(stderr, "Unknown signal Emission(%s)", emission);
   }
}

/**
 * set_date - an edje signal emission 
 * @data - the data passed when the callback was added
 * @o - the evas object(Edje) that created the signal
 * @emission - the signal "type" that was emitted
 * @source - the signal originated from this "part"
 * Attempt to set the Part named "EntranceDate" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
static void
set_date(void *data, Evas_Object * o, const char *emission,
         const char *source)
{
   if (edje_object_part_exists(o, "EntranceDate"))
   {
      struct tm *now;
      char buf[PATH_MAX];
      time_t _t = time(NULL);

      now = localtime(&_t);
      strftime(buf, PATH_MAX, session->config->date.string, now);
      edje_object_part_text_set(o, "EntranceDate", buf);
   }
}

/**
 * set_time - an edje signal emission 
 * @data - the data passed when the callback was added
 * @o - the evas object(Edje) that created the signal
 * @emission - the signal "type" that was emitted
 * @source - the signal originated from this "part"
 * Attempt to set the Part named "EntranceTime" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
static void
set_time(void *data, Evas_Object * o, const char *emission,
         const char *source)
{
   if (edje_object_part_exists(o, "EntranceTime"))
   {
      struct tm *now;
      char buf[PATH_MAX];
      time_t _t = time(NULL);

      now = localtime(&_t);
      strftime(buf, PATH_MAX, session->config->time.string, now);
      edje_object_part_text_set(o, "EntranceTime", buf);
   }
}

/**
 * done_cb - Executed when an EntranceAuthSuccessDone signal is emitted
 * @data - the data passed when the callback was added
 * @o - the evas object(Edje) that created the signal
 * @emission - the signal "type" that was emitted
 * @source - the signal originated from this "part"
 * Attempt to set the Part named "EntranceTime" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
static void
done_cb(void *data, Evas_Object * o, const char *emission, const char *source)
{
   if (session->authed)
   {
      entrance_session_start_user_session(session);
   }
   else
   {
      exit(0);
   }
}

/**
 * session_item_selected_cb - Executed when a Session is selected
 * @data - the data passed when the callback was added
 * @o - the evas object(Edje) that created the signal
 * @emission - the signal "type" that was emitted
 * @source - the signal originated from this "part"
 * Attempt to set the Part named "EntranceTime" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
void
session_item_selected_cb(void *data, Evas_Object * o, const char *emission,
                         const char *source)
{
   if (session && data)
   {
      entrance_session_xsession_set(session, (char *) data);
   }
}

/**
 * session_item_selected_cb - Executed when a Session is selected
 * @data - the data passed when the callback was added
 * @o - the evas object(Edje) that created the signal
 * @emission - the signal "type" that was emitted
 * @source - the signal originated from this "part"
 * Attempt to set the Part named "EntranceTime" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
void
user_selected_cb(void *data, Evas_Object * o, const char *emission,
                 const char *source)
{
   if (session && data)
   {
      entrance_session_user_set(session, (char *) data);
   }
}

/**
 * session_item_selected_cb - Executed when a Session is selected
 * @data - the data passed when the callback was added
 * @o - the evas object(Edje) that created the signal
 * @emission - the signal "type" that was emitted
 * @source - the signal originated from this "part"
 * Attempt to set the Part named "EntranceTime" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
void
user_unselected_cb(void *data, Evas_Object * o, const char *emission,
                   const char *source)
{
   if (session && data)
   {
      entrance_session_user_reset(session);
   }
}

/**
 * reboot_cb - Executed when an EntranceSystemReboot signal is emitted
 * @data - the data passed when the callback was added
 * @o - the evas object(Edje) that created the signal
 * @emission - the signal "type" that was emitted
 * @source - the signal originated from this "part"
 * Attempt to set the Part named "EntranceTime" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
static void
reboot_cb(void *data, Evas_Object * o, const char *emission,
          const char *source)
{
   if (session->config->reboot.allow)
   {
      pid_t pid;

      switch (pid = fork())
      {
        case 0:
           if (execl
               ("/bin/sh", "/bin/sh", "-c", "/sbin/shutdown -r now", NULL))
           {
              syslog(LOG_CRIT,
                     "Reboot failed: Unable to execute /sbin/shutdown");
              exit(0);
           }
        case -1:
           syslog(LOG_CRIT,
                  "Reboot failed: could not fork to execute shutdown script");
           break;
        default:
           syslog(LOG_INFO, "The system is being rebooted");
           exit(EXITCODE);
      }
   }
}

/**
 * shutdown_cb - Executed when an EntranceSystemHalt signal is emitted
 * @data - the data passed when the callback was added
 * @o - the evas object(Edje) that created the signal
 * @emission - the signal "type" that was emitted
 * @source - the signal originated from this "part"
 * Attempt to set the Part named "EntranceTime" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
static void
shutdown_cb(void *data, Evas_Object * o, const char *emission,
            const char *source)
{
   pid_t pid;

   if (session->config->halt.allow)
   {
      switch (pid = fork())
      {
        case 0:
           if (execl
               ("/bin/sh", "/bin/sh", "-c", "/sbin/shutdown -h now", NULL))
           {
              syslog(LOG_CRIT,
                     "Shutdown failed: Unable to execute /sbin/shutdown");
              exit(0);
           }
        case -1:
           syslog(LOG_CRIT,
                  "Shutdown failed: could not fork to execute shutdown script");
           break;
        default:
           syslog(LOG_INFO, "The system is being shut down");
           exit(EXITCODE);
      }
   }
}

/**
 * timer_cb - we handle this iteration outside of the theme
 */
int
timer_cb(void *data)
{
   Evas_Object *o = NULL;

   if ((o = (Evas_Object *) data))
   {
      set_date(NULL, o, NULL, NULL);
      set_time(NULL, o, NULL, NULL);
   }
   return (1);
}

/**
 * main - where it all starts !
 */
int
main(int argc, char *argv[])
{
   int i = 0;
   char buf[PATH_MAX];
   char *str = NULL;
   Ecore_X_Window ew;
   Evas *evas = NULL;
   Ecore_Evas *e = NULL;
   Ecore_Timer *timer = NULL;
   Evas_Object *o = NULL, *edje = NULL;
   Evas_Coord x, y, w, h;
   char *entries[] = { "EntranceUserEntry", "EntrancePassEntry" };
   int entries_count = 2;

   openlog("entrance", LOG_NOWAIT, LOG_DAEMON);
   if (argv[1])
      snprintf(buf, PATH_MAX, "%s", argv[1]);
   /* Basic ecore initialization */
   if (!ecore_init())
      return (-1);
   ecore_app_args_set(argc, (const char **) argv);
#if 0
   if (!ecore_x_init(buf))
   {
      if ((str = getenv("DISPLAY")))
         syslog(LOG_CRIT,
                "Cannot initialize default display \"%s\". Exiting.", str);
      else
         syslog(LOG_CRIT, "No DISPLAY variable set! Exiting.");
      return (-1);
   }
#endif
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, exit_cb, NULL);

   session = entrance_session_new();
   if (ecore_evas_init())
   {
      /* init edje and set frametime to 60 fps ? */
      edje_init();
      edje_frametime_set(1.0 / 60.0);

      /* setup our ecore_evas */
      /* testing mode decides entrance window size * * Use rendering engine
         specified in config. On systems with * hardware acceleration, GL
         should improve performance appreciably */
      if (!strcmp(session->config->engine, "software"))
         e = ecore_evas_software_x11_new(NULL, 0, 0, 0, WINW, WINH);
#ifdef HAVE_ECORE_GL_X11
      else if (!strcmp(session->config->engine, "gl"))
         e = ecore_evas_gl_x11_new(NULL, 0, 0, 0, WINW, WINH);
#endif
      else
      {
         fprintf(stderr,
                 "Warning: Invalid Evas engine specified in config. Defaulting to software engine.\n");
         e = ecore_evas_software_x11_new(NULL, 0, 0, 0, WINW, WINH);
      }

      ew = ecore_evas_software_x11_window_get(e);
      ecore_evas_title_set(e, "Entrance");
      ecore_evas_callback_delete_request_set(e, window_del_cb);
      ecore_evas_callback_resize_set(e, window_resize_cb);
      ecore_evas_cursor_set(e, session->config->pointer, 12, 0, 0);
      ecore_evas_move(e, 0, 0);

      /* Evas specific callbacks */
      evas = ecore_evas_get(e);
      evas_image_cache_set(evas, 8 * 1024 * 1024);
      evas_font_cache_set(evas, 1 * 1024 * 1024);
      evas_font_path_append(evas, PACKAGE_DATA_DIR "/fonts");
      evas_font_path_append(evas, PACKAGE_DATA_DIR "/data/fonts");
      evas_key_modifier_add(evas, "Control_L");
      evas_key_modifier_add(evas, "Control_R");
      evas_key_modifier_add(evas, "Shift_L");
      evas_key_modifier_add(evas, "Shift_R");
      evas_key_modifier_add(evas, "Alt_L");
      evas_key_modifier_add(evas, "Alt_R");

      /* Load our theme as an edje */
      edje = edje_object_add(evas);
      snprintf(buf, PATH_MAX, "%s/themes/%s", PACKAGE_DATA_DIR,
               session->config->theme);
      if (!edje_object_file_set(edje, buf, "Main"))
      {
         fprintf(stderr, "Failed to set %s\n", buf);
         exit(1);
      }
      evas_object_move(edje, 0, 0);
      evas_object_resize(edje, WINW, WINH);
      evas_object_name_set(edje, "ui");
      evas_object_layer_set(edje, 0);
      entrance_session_edje_object_set(session, edje);

      for (i = 0; i < entries_count; i++)
      {
         if (edje_object_part_exists(edje, entries[i]))
         {
            edje_object_part_geometry_get(edje, entries[i], &x, &y, &w, &h);
            o = evas_text_entry_new(evas);
            evas_object_move(o, x, y);
            evas_object_resize(o, w, h);
            evas_object_layer_set(o, 1);
            evas_text_entry_max_chars_set(o, 32);
            evas_text_entry_is_password_set(o, i);
            evas_object_name_set(o, entries[i]);
            evas_text_entry_edje_part_set(o, edje, entries[i]);

            evas_text_entry_return_key_callback_set(o, interp_return_key, o);

            edje_object_signal_callback_add(edje, "In", entries[i], focus, o);

            edje_object_signal_callback_add(edje, "Out", entries[i], focus,
                                            o);
            edje_object_part_swallow(edje, entries[i], o);
            evas_object_show(o);
         }
         o = NULL;
      }

      /* See if we have a EntranceHostname part, set it */
      if (edje_object_part_exists(edje, "EntranceHostname"))
      {
         if ((str = get_my_hostname()))
         {
            edje_object_part_text_set(edje, "EntranceHostname", str);
            free(str);
         }
      }
      if (edje_object_part_exists(edje, "EntranceTime"))
      {
         edje_object_signal_callback_add(edje, "Go", "EntranceTime", set_time,
                                         o);
         edje_object_signal_emit(edje, "Go", "EntranceTime");
         timer = ecore_timer_add(0.5, timer_cb, edje);
      }
      if (edje_object_part_exists(edje, "EntranceDate"))
      {
         edje_object_signal_callback_add(edje, "Go", "EntranceDate", set_date,
                                         o);
         edje_object_signal_emit(edje, "Go", "EntranceDate");
         if (!timer)
            timer = ecore_timer_add(0.5, timer_cb, edje);
      }
      if (edje_object_part_exists(edje, "EntranceSession"))
      {
         entrance_session_xsession_set(session, "Default");
      }
      if (edje_object_part_exists(edje, "EntranceSessionList"))
      {
         entrance_session_list_add(session);
      }
      if (edje_object_part_exists(edje, "EntranceUserList"))
      {
         entrance_session_user_list_add(session);
      }
      edje_object_signal_callback_add(edje, "EntranceUserAuthSuccessDone", "",
                                      done_cb, e);
      edje_object_signal_callback_add(edje, "EntranceSystemReboot", "",
                                      reboot_cb, e);
      edje_object_signal_callback_add(edje, "EntranceSystemHalt", "",
                                      shutdown_cb, e);
      /* 
       * It's useful to delay showing of your edje till all your
       * callbacks have been added, otherwise show might not trigger all
       * the desired events 
       */
      evas_object_show(edje);
      /* set focus to user input */
      edje_object_signal_emit(edje, "In", "EntranceUserEntry");

#if (X_TESTING == 0)
      ecore_evas_resize(e, WINW, WINH);
      ecore_evas_fullscreen_set(e, 1);
#elif (X_TESTING == 1)
      ecore_evas_resize(e, WINW, WINH);
#endif

      entrance_session_ecore_evas_set(session, e);
      entrance_session_run(session);
      entrance_session_free(session);
      closelog();
   }
   return (0);
}
