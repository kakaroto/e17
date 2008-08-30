/**
@file main.c
@brief When entrance starts, and ui specific variables
*/
#include <time.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <locale.h>
#include <Ecore_X_Cursor.h>
#include <Edje.h>
#include <Esmart/Esmart_Text_Entry.h>
#include <Esmart/Esmart_Container.h>
#include <Ecore_Config.h>
#include <Efreet.h>
#include "entrance.h"
#include "entrance_session.h"
#include "entrance_x_session.h"
#include "entrance_ipc.h"

#define WINW 800
#define WINH 600

static Entrance_Session *session = NULL;

int ENTRANCE_DESKTOP_TYPE_XSESSION = -1;

Ecore_Evas *setup_ecore_evas(int wx, int wy, int ww, int wh, int fullscreen);

static int
idler_before_cb(void *data)
{
   edje_thaw();
   return 1;
}

static int
idler_after_cb(void *data)
{
   edje_freeze();
   return 1;
}


/**
 * get the hostname of the machine, surrounded by the before and after
 * strings the config specifies
 * @return - a valid string for the hostname, Localhost on failure or
 * whatever the system provides
 */
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
 * Called when the screensaver starts/stops
 */
static int
screensaver_notify_cb(void *data, int ev_type, void *ev)
{
   Ecore_X_Event_Screensaver_Notify *e;
   Evas_Object *edje = data;

   e = ev;
   if (e->on)
   {
      int size;

      edje_object_play_set(edje, 0);
      edje_freeze();
      size = evas_image_cache_get(evas_object_evas_get(edje));
      evas_image_cache_set(evas_object_evas_get(edje), 0);
      evas_image_cache_set(evas_object_evas_get(edje), size);
      size = evas_font_cache_get(evas_object_evas_get(edje));
      evas_font_cache_set(evas_object_evas_get(edje), 0);
      evas_font_cache_set(evas_object_evas_get(edje), size);
      /* FIXME: it'd be nice to literally delete all the evas objects to nuke 
         even mroe memory use here */
   }
   else
   {
      edje_thaw();
      edje_object_play_set(edje, 1);
   }
   return 1;
}

/**
 * what to do if we SIGINT(^c) it
 * @param data - no clue
 * @param ev_type - kill event ?
 * @param ev - event data
 * @return 1
 * Obviously I want to exit here.
 */
static int
exit_cb(void *data, int ev_type, void *ev)
{
   ecore_main_loop_quit();
   return 1;
}

/**
 * what to do when we receive a window delete event
 * @param ee - the Ecore_Evas that received the event
 */
static void
window_del_cb(Ecore_Evas * ee)
{
   ecore_main_loop_quit();
}

static void
resize_object_on_screen(Evas_Object *o, int s, int screens, int w, int h)
{
   if (screens > 1)
   {
      int sx, sy, sw, sh;

      ecore_x_xinerama_screen_geometry_get(s, &sx, &sy, &sw, &sh);
      evas_object_move(o, sx, sy);
      evas_object_resize(o, sw, sh);
   }
   else
   {
      evas_object_resize(o, w, h);
   }
}

/**
 * handle when the ecore_evas needs to be resized
 * @param ee - The Ecore_Evas we're resizing 
 */
static void
window_resize_cb(Ecore_Evas * ee)
{
   Evas_Object *o = NULL, *ui = NULL;
   int w, h;
   int screens, i;
   char buf[50];

   ecore_evas_geometry_get(ee, NULL, NULL, &w, &h);

   ui = evas_object_name_find(ecore_evas_get(ee), "ui");

   screens = ecore_x_xinerama_screen_count_get();
   if (!screens)
      screens = 1;
   for (i = 0; i < screens; i++)
   {
      snprintf(buf, sizeof(buf), "background%d", i);
      if ((o = evas_object_name_find(ecore_evas_get(ee), buf)))
	 resize_object_on_screen(o, i, screens, w, h);
      if ((i == session->current_screen) && (ui != NULL))
	 resize_object_on_screen(ui, i, screens, w, h);
   }
}

/**
 * swap key input focus between the password and user entries
 * @param o - the object we want to swap focus with
 * @param selecto - whether to focus on o, or the other entry
 */
static void
focus_swap(Evas_Object * o, int selecto)
{
   Evas_Object *oo = NULL;

   if (!strcmp(esmart_text_entry_edje_part_get(o), "entrance.entry.pass"))
   {
      if ((oo =
           evas_object_name_find(evas_object_evas_get(o),
                                 "entrance.entry.user")))
      {
         esmart_text_entry_text_set(oo, "");
      }
      esmart_text_entry_text_set(o, "");
   }
   else
      if (!strcmp(esmart_text_entry_edje_part_get(o), "entrance.entry.user"))
   {
      oo =
         evas_object_name_find(evas_object_evas_get(o),
                               "entrance.entry.pass");
   }
   if (oo)
   {
      selecto ? evas_object_focus_set(oo, 0) : evas_object_focus_set(oo, 1);
      selecto ? evas_object_focus_set(o, 1) : evas_object_focus_set(o, 0);
   }
}

/**
 * when Enter is hit on the keyboard we end up here
 * @param data - The smart object that is this Entry
 * @param str - The string that was in the buffer when Enter was pressed
 */
static void
interp_return_key(void *data, const char *str)
{
   Evas_Object *o = NULL;

   o = (Evas_Object *) data;

   if (!strcmp(esmart_text_entry_edje_part_get(o), "entrance.entry.user"))
   {
      if (!entrance_session_user_set(session, str))
      {
         edje_object_signal_emit(esmart_text_entry_edje_object_get(o),
                                 "entrance,user,success", "");
         focus_swap(o, 0);
      }
      else
      {
         esmart_text_entry_text_set(o, "");
         entrance_session_user_reset(session);
         edje_object_signal_emit(esmart_text_entry_edje_object_get(o),
                                 "entrance,user,fail", "");
         focus_swap(o, 1);
      }
   }
   if (!strcmp(esmart_text_entry_edje_part_get(o), "entrance.entry.pass"))
   {
      if (!entrance_session_pass_set(session, str))
      {
         session->authed = 1;
         edje_object_signal_emit(esmart_text_entry_edje_object_get(o),
                                 "entrance,user,auth,success", "");
      }
      else
      {
         esmart_text_entry_text_set(o, "");
         entrance_session_user_reset(session);
         edje_object_signal_emit(esmart_text_entry_edje_object_get(o),
                                 "entrance,user,auth,fail", "");
         focus_swap(o, 0);
      }
   }
}

/**
 * an edje signal emission 
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
focus(void *data, Evas_Object * o, const char *emission, const char *source)
{
   Evas_Object *oo = NULL;      /* in nexus this looks in infinity ! */

   if ((oo = (Evas_Object *) data))
   {
      if (!strcmp(emission, "In"))
      {
         if (!evas_object_focus_get(oo))
         {
            evas_object_focus_set(oo, 1);
         }
      }
      else if (!strcmp(emission, "Out"))
      {
         if (evas_object_focus_get(oo))
         {
            evas_object_focus_set(oo, 1);
            evas_object_focus_set(oo, 0);
         }
      }
      else
         fprintf(stderr, "Unknown signal Emission(%s)", emission);
   }
}

/**
 * Set the "entrance.date" part's text
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 * Attempt to set the Part named "entrance.date" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
static void
set_date(void *data, Evas_Object * o, const char *emission,
         const char *source)
{
   if (edje_object_part_exists(o, "entrance.date"))
   {
      struct tm *now;
      char buf[PATH_MAX];
      time_t _t = time(NULL);

      now = localtime(&_t);
      strftime(buf, PATH_MAX, session->config->date.string, now);
      edje_object_part_text_set(o, "entrance.date", buf);
   }
}

/**
 * Set the "entrance.time" part's text
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 * Attempt to set the Part named "entrance.time" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
static void
set_time(void *data, Evas_Object * o, const char *emission,
         const char *source)
{
   if (edje_object_part_exists(o, "entrance.time"))
   {
      struct tm *now;
      char buf[PATH_MAX];
      time_t _t = time(NULL);

      now = localtime(&_t);
      strftime(buf, PATH_MAX, session->config->time.string, now);
      edje_object_part_text_set(o, "entrance.time", buf);
   }
}

/**
 * Executed when an EntranceAuthSuccessDone signal is emitted
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 * Ensure that the session is authed, and quit the main ecore_loop
 */
static void
done_cb(void *data, Evas_Object * o, const char *emission, const char *source)
{
   if (!session->authed)
      syslog(LOG_CRIT,
             "Theme attempted to launch session without finishing authentication. Please fix your theme.");
   else
   {
      /* 
       * Request cookie here and call ecore_main_loop_quit, after we
       * receive the cookie back from server
       */
      entrance_session_setup_user_session(session);
   }
}

/**
 * Executed when a Session is selected
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 * Attempt to set the Part named "entrance.time" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
void
session_item_selected_cb(void *data, Evas_Object * o, const char *emission,
                         const char *source)
{
   if (session && data)
   {
      Entrance_X_Session *exs = (Entrance_X_Session *) data;

      entrance_session_x_session_set(session, exs);
      if (!(session->session_selected))
         session->session_selected = 1;
      edje_object_signal_emit(session->edje, "entrance,xsession,selected",
                              "");
   }
}

/**
 * Executed when a Session is selected
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 * Attempt to set the Part named "entrance.time" to the results of
 * localtime.  This way the interval is configurable via a program in
 * the theme and not statically bound to a value.  
 */
void
user_selected_cb(void *data, Evas_Object * o, const char *emission,
                 const char *source)
{
   if (session && data)
   {
      entrance_session_user_set(session, ((Entrance_User *) data)->name);
   }
}

/**
 * Executed when a Session is unselected
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 * Set the current EntranceFace part back to nothing
 */
void
user_unselected_cb(void *data, Evas_Object * o, const char *emission,
                   const char *source)
{
   if (session && data)
   {
      entrance_session_user_reset(session);
      /* 
         edje_object_signal_emit(o, "entrance,user,auth,fail", ""); */
   }
}

/**
 * Executed when an EntranceSystemReboot signal is emitted
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
reboot_cb(void *data, Evas_Object * o, const char *emission,
          const char *source)
{
   if ((session->config->reboot) && (!session->testing))
   {
      pid_t pid;

      entrance_session_free(session);
      session = NULL;
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
   else if (session->testing)
   {
      syslog(LOG_INFO, "Reboot Unsupported in testing mode");
   }
}

/**
 * Executed when an EntranceSystemHalt signal is emitted
 * @param data - the data passed when the callback was added
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 */
static void
shutdown_cb(void *data, Evas_Object * o, const char *emission,
            const char *source)
{
   pid_t pid;

   if ((session->config->halt) && (!session->testing))
   {
      entrance_session_free(session);
      session = NULL;
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
   else if (session->testing)
   {
      syslog(LOG_INFO, "Shutdown Unsupported in testing mode");
   }
}

/**
 * Executed when an SessionDefaultSet signal is emitted
 * @param data - the Entrance_Session in context
 * @param o - the evas object(Edje) that created the signal
 * @param emission - the signal "type" that was emitted
 * @param source - the signal originated from this "part"
 * Save out the current Entrance_Session's EntranceSession as the user's new
 * default session to be executed when they log in.
 */
static void
_user_session_set(void *data, Evas_Object * o, const char *emission,
                  const char *source)
{
   Entrance_Session *e = NULL;

   if ((e = (Entrance_Session *) data))
   {
      entrance_session_user_session_default_set(e);
   }
}

static void
_container_scroll(void *data, Evas_Object * o, const char *emission,
                  const char *source)
{
   double sx = 0.0, sy = 0.0;
   Evas_Object *container = NULL;

   if ((container = data))
   {
      double container_length = 0.0;

      container_length = esmart_container_elements_length_get(container);
      edje_object_part_drag_value_get(session->edje, source, &sx, &sy);
      switch (esmart_container_direction_get(container))
      {
        case CONTAINER_DIRECTION_HORIZONTAL:
           esmart_container_scroll_offset_set(container,
                                              (int) (sx * container_length));
           break;
        case CONTAINER_DIRECTION_VERTICAL:
           esmart_container_scroll_offset_set(container,
                                              (int) (sy * container_length));
           break;
        default:
           fprintf(stderr, "Unknown Container Orientation\n");
           break;
      }
   }
}

/**
 * print the "Help" associated with the app, shows cli args etc
 * @param argv the argv that was passed from the application
 */
static void
entrance_help(char **argv)
{
   printf("Entrance - The Enlightened Display Manager\n");
   printf("Usage: %s [OPTION]...\n\n", argv[0]);
   printf
      ("---------------------------------------------------------------------------\n");
   printf("  -c, --config=CONFIG          Specify a custom config file\n");
   printf
      ("  -d, --display=DISPLAY        Specify which display Entrance should use\n");
   printf("  -h, --help                   Display this help message\n");
   printf
      ("  -g, --geometry=WIDTHxHEIGHT  Specify the size of the Entrance window.\n");
   printf
      ("                               Use of this option disables fullscreen mode.\n");
   printf
      ("  -t, --theme=THEME            Specify the theme to load. You may specify\n");
   printf
      ("                               either the name of an installed theme, or an\n");
   printf
      ("                               arbitrary path to an edj file (use ./ for\n");
   printf("                               the current directory).\n");
   printf
      ("  -T, --test                   Enable testing mode. This will cause xterm\n");
   printf
      ("                               to be executed instead of the selected\n");
   printf
      ("                               session upon authentication, and uses a\n");
   printf
      ("                               geometry of 800x600 (-g overrides this)\n");
   printf
      ("===========================================================================\n\n");
   printf
      ("Note: To automatically launch an X server that will be managed, please use\n");
   printf
      ("      entranced instead of entrance. Entrance requires an existing X server\n");
   printf("      to run. Run entranced --help for more information.\n\n");
   exit(0);
}

/**
 * we handle this iteration outside of the theme, update date and time
 * @param data a pointer to the main edje in entrance
 * @return 1 so the ecore_timer keeps going and going and ...
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
 * When the pointer switches screens, this moves the widgets to the new screen
 * @param data the integer value of the new screen
 */
static void
screen_switch_cb(void *data, Evas_Object * obj, const char *signal,
                 const char *source)
{
   int screen;
   Evas_Coord x, y, w, h;

   screen = (int) data;
   if (session->current_screen == screen)
      return;

   evas_object_geometry_get(obj, &x, &y, &w, &h);
   evas_object_move(session->edje, x, y);
   evas_object_resize(session->edje, w, h);
   session->current_screen = screen;
}

/**
 * main - where it all starts !
 * @param argc - the number of arguments entrance was called with
 * @param argv - the args entrance was called with 
 * <p>Entrance works like this:</p>
 * <ol>
 * <li> Init Ecore </li>
 * <li> Parse command line arguments </li>
 * <li> Create a New Entrance_Session(Parses config for you) </li>
 * <li> Init Ecore_X </li>
 * <li> Init Ecore_Evas </li>
 * <li> Init Edje </li>
 * <li> Detect Ecore_Evas type from config, software or gl</li>
 * <li> Set the cursor specified in the config </li>
 * <li> Add key modifiers, setup caches and paths </li>
 * <li> Load theme specified in config, or from cli(cli overrides
 * config)</li>
 * <li> Swallow the username and password entries into the edje </li>
 * <li> Detect theme part presence, swallow/setup as appropriate </li>
 * <li> Setup signal callbacks that our main edje might emit </li>
 * <li> Show the main edje </li>
 * <li> Emit an "In" signal on the main entry for lazy themers </li>
 * <li> Tell the Entrance_Sesssion that the Ecore_Evas belongs to it</li>
 * <li> Run.............. until ecore_main_(loop_quit is called</li>
 * <li> If the user is authenticated, try to run their session</li>
 * <li>Shut down edje, ecore_evas, ecore_x, ecore</li>
 * </ol>
 */
int
main(int argc, char *argv[])
{
   char buf[PATH_MAX];
   char *str = NULL;
   char *display = NULL;
   Ecore_Evas *e = NULL;
   int c;
   struct option d_opt[] = {
      {"help", 0, 0, 'h'},
      {"display", 1, 0, 'd'},
      {"geometry", 1, 0, 'g'},
      {"theme", 1, 0, 't'},
      {"test", 0, 0, 'T'},
      {"config", 1, 0, 'c'},
      {0, 0, 0, 0}
   };
   int g_x = WINW, g_y = WINH;
   char *theme = NULL;
   char *config = NULL;
   int fullscreen = 1;
   pid_t server_pid = 0;
   int testing = 0;

   /* Basic ecore initialization */
   if (!ecore_init())
      return (-1);
   if (ecore_config_init("entrance") != ECORE_CONFIG_ERR_SUCC)
   {
      ecore_shutdown();
      return -1;
   }

   if (!efreet_init())
   {
      ecore_shutdown();
      ecore_config_shutdown();
      return -1;
   }

   ENTRANCE_DESKTOP_TYPE_XSESSION = efreet_desktop_type_alias(EFREET_DESKTOP_TYPE_APPLICATION, "XSession");

   ecore_app_args_set(argc, (const char **) argv);

   /* Set locale to user's environment */
   if (!(setlocale(LC_ALL, NULL)))
      fprintf(stderr,
              "Locale set failed!\n"
              "Please make sure you have your locale files installed for \"%s\"\n",
              getenv("LANG"));


   /* Parse command-line options */
   while (1)
   {
      c = getopt_long(argc, argv, "hd:g:t:Tc:z:", d_opt, NULL);
      if (c == -1)
         break;
      switch (c)
      {
        case 'h':
           entrance_help(argv);
        case 'd':
           display = strdup(optarg);
           break;
        case 'g':
           atog(optarg, &g_x, &g_y);

           if (!g_x || !g_y)
           {
              syslog(LOG_CRIT,
                     "Invalid argument '%s' given for geometry. Exiting.",
                     optarg);
              return (-1);
           }

           fullscreen = 0;
           break;
        case 't':
           /* Allow arbitrary paths to theme files */
           theme = theme_normalize_path(theme, optarg);
           break;
        case 'T':
           testing = 1;
           fullscreen = 0;
           break;
        case 'c':
           config = strdup(optarg);
           break;
        case 'z':
           /* printf("entrance: main: z optarg = %s\n", optarg); */
           server_pid = (pid_t) atoi(optarg);
           break;
        default:
           entrance_help(argv);
      }
   }

   if (!testing)
      if (!entrance_ipc_init(server_pid))
         return -1;

   session = entrance_session_new(config, display, testing);
   if (testing)
      session->testing = 1;

   if (config)
      free(config);

#if 0
   printf("entrance: main: XAUTHORITY = %s\n", getenv("XAUTHORITY"));
#endif

#if 1
   if (!ecore_x_init(display))
   {
      if (display)
         syslog(LOG_CRIT,
                "Cannot initialize requested display \"%s\". Exiting.",
                display);
      else if ((str = getenv("DISPLAY")))
         syslog(LOG_CRIT,
                "Cannot initialize default display \"%s\". Exiting.", str);
      else
         syslog(LOG_CRIT, "No DISPLAY variable set! Exiting.");
      return (-1);
   }
#endif
   if (!testing)
   {
      Ecore_X_Window *roots;
      int num, i;

      num = 0;
      roots = ecore_x_window_root_list(&num);
      if (roots)
      {
         for (i = 0; i < num; i++)
         {
            ecore_x_window_background_color_set(roots[i], 0, 0, 0);
            ecore_x_window_cursor_show(roots[i], 0);
         }
         free(roots);
         ecore_x_sync();
      }
   }
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, exit_cb, NULL);
   ecore_idle_enterer_add(idler_before_cb, NULL);

   ecore_x_screensaver_event_listen_set(1);

   /* Load our theme as an edje */
   if (!theme)
   {
      snprintf(buf, PATH_MAX, "%s/themes/%s", PACKAGE_DATA_DIR,
               session->config->theme);
   }
   else
   {
      snprintf(buf, PATH_MAX, "%s", theme);
   }
   if (session->config->theme)
      free(session->config->theme);
   session->config->theme = strdup(buf);

   if (ecore_evas_init())
   {
      edje_init();
      edje_freeze();
      edje_frametime_set(1.0 / 30.0);

      e = setup_ecore_evas(0, 0, g_x, g_y, fullscreen);
      if (!e)
      {
         /* Note: The actual error will be logged in setup_ecore_evas() */
         exit(EXITCODE);
      }

      ecore_idle_enterer_add(idler_after_cb, NULL);

      entrance_session_ecore_evas_set(session, e);
      entrance_ipc_session_set(session);
      entrance_session_run(session);

      if (!testing)
      {
         Ecore_X_Window *roots;
         int num, i;

         num = 0;
         roots = ecore_x_window_root_list(&num);
         if (roots)
         {
            for (i = 0; i < num; i++)
            {
               ecore_x_window_cursor_show(roots[i], 1);
            }
            free(roots);
            ecore_x_sync();
         }
      }
      if (session->authed)
      {
         entrance_session_start_user_session(session);
      }
      else
      {
         entrance_session_free(session);
      }
      if (!testing)
         entrance_ipc_shutdown();
      edje_shutdown();
      ecore_evas_shutdown();
      ecore_x_shutdown();
      efreet_shutdown();
      ecore_config_shutdown();
      ecore_shutdown();
   }
   else
   {
      fprintf(stderr, "Fatal error: Could not initialize ecore_evas!\n");
      exit(EXITCODE);
   }
   return (0);
}


Ecore_Evas *
setup_ecore_evas(int wx, int wy, int ww, int wh, int fullscreen)
{
   Ecore_Evas *e;
   Ecore_X_Window ew;
   Evas *evas = NULL;
   Evas_Object *edje;
   Evas_Object *background;
   char *bg_file;
   Ecore_Timer *timer = NULL;
   Evas_Object *o = NULL;
   Evas_Coord x, y, w, h;
   char *entries[] = { "entrance.entry.user", "entrance.entry.pass" };
   int entries_count = 2;
   const char *container_orientation = NULL;
   int i;
   char *str = NULL;
   int screens;

   /* setup our ecore_evas */
   /* testing mode decides entrance window size * * Use rendering engine
      specified in config. On systems with * hardware acceleration, GL should 
      improve performance appreciably */
   switch (session->config->engine)
   {
     case 1:
        if (ecore_evas_engine_type_supported_get(ECORE_EVAS_ENGINE_OPENGL_X11))
        {
           e = ecore_evas_gl_x11_new(NULL, 0, wx, wy, ww, wh);
           break;
        }
        syslog(LOG_INFO,
               "Warning: Evas GL engine: engine not supported. Defaulting to software engine.\n");

     case 0:
        e = ecore_evas_software_x11_new(NULL, 0, wx, wy, ww, wh);
        break;

     default:
        syslog(LOG_INFO,
               "Warning: Invalid Evas engine specified in config. Defaulting to software engine.\n");
        e = ecore_evas_software_x11_new(NULL, 0, wx, wy, ww, wh);
        break;
   }

   if (!e)
   {
      syslog(LOG_INFO,
             "Critical error: No Evas engine available. Exiting.\n");
      return (NULL);
   }

   ew = ecore_evas_software_x11_window_get(e);
   if (session->testing)
      ecore_evas_title_set(e, "Entrance - Testing Mode");
   else
      ecore_evas_title_set(e, "Entrance");
   ecore_evas_callback_delete_request_set(e, window_del_cb);
   ecore_evas_callback_resize_set(e, window_resize_cb);
   ecore_x_window_cursor_set(ew, ECORE_X_CURSOR_WATCH);
   ecore_evas_move(e, wx, wy);

   if (fullscreen)
   {
      Ecore_X_Window root;
      int w = 800, h = 600;

      root = ecore_x_window_root_first_get();
      ecore_x_window_size_get(root, &w, &h);
      ecore_evas_move_resize(e, 0, 0, w, h);
   }

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

   if (!strlen(session->config->background))
      bg_file = session->config->theme;
   else
      bg_file = session->config->background;

   screens = ecore_x_xinerama_screen_count_get();
   if (!screens)
      screens = 1;
   for (i = 0; i < screens; i++)
   {
      int sx, sy, sw, sh;
      char buf[50];

      if (screens > 1)
      {
         ecore_x_xinerama_screen_geometry_get(i, &sx, &sy, &sw, &sh);
      }
      else
      {
         ecore_evas_geometry_get(e, &sx, &sy, &sw, &sh);
      }
      /* Load background first, from theme file */
      background = edje_object_add(evas);

      if (!edje_object_file_set(background, bg_file, "Background"))
         if (!edje_object_file_set
             (background, bg_file, "e/desktop/background"))
            syslog(LOG_INFO, "Failed to load background from %s", bg_file);

      evas_object_move(background, sx, sy);
      evas_object_resize(background, sw, sh);
      snprintf(buf, sizeof(buf), "background%d", i);
      evas_object_name_set(background, buf);
      evas_object_layer_set(background, 0);
      evas_object_show(background);
      if (screens > 1)
      {
         edje_object_signal_callback_add(background, "mouse,in", "*",
                                         screen_switch_cb, (void *) i);
      }

      /* show the widgets on the first screen */
      if (i == 0)
      {
         int j;

         /* Load theme */
         edje = edje_object_add(evas);
         if (!edje_object_file_set(edje, session->config->theme, "Main"))
         {
            syslog(LOG_CRIT, "Failed to load theme %s\n", buf);
            entrance_session_free(session);
            return NULL;
         }
         evas_object_move(edje, sx, sy);
         evas_object_resize(edje, sw, sh);
         evas_object_name_set(edje, "ui");
         evas_object_layer_set(edje, 1);
         entrance_session_edje_object_set(session, edje);

         /* Setup the entries */
         for (j = 0; j < entries_count; j++)
         {
            if (edje_object_part_exists(edje, entries[j]))
            {
               edje_object_part_geometry_get(edje, entries[j], &x, &y, &w,
                                             &h);
               o = esmart_text_entry_new(evas);
               evas_object_move(o, x, y);
               evas_object_resize(o, w, h);
               evas_object_layer_set(o, 2);
               esmart_text_entry_max_chars_set(o, 32);
               esmart_text_entry_is_password_set(o, j);
               evas_object_name_set(o, entries[j]);
               esmart_text_entry_edje_part_set(o, edje, entries[j]);

               esmart_text_entry_return_key_callback_set(o, interp_return_key,
                                                         o);

               edje_object_signal_callback_add(edje, "In", entries[j], focus,
                                               o);

               edje_object_signal_callback_add(edje, "Out", entries[j], focus,
                                               o);
               edje_object_part_swallow(edje, entries[j], o);
               evas_object_show(o);
            }
            o = NULL;
         }

         /* See if we have a EntranceHostname part, set it */
         if (edje_object_part_exists(edje, "entrance.hostname"))
         {
            if ((str = get_my_hostname()))
            {
               edje_object_part_text_set(edje, "entrance.hostname", str);
               free(str);
            }
         }
         /* See if we have an EntranceTime part, setup a timer to
            automatically update the Time */
         if (edje_object_part_exists(edje, "entrance.time"))
         {
            edje_object_signal_callback_add(edje, "Go", "entrance.time",
                                            set_time, o);
            edje_object_signal_emit(edje, "Go", "entrance.time");
            timer = ecore_timer_add(0.5, timer_cb, edje);
         }
         /* See if we have an EntranceDate part, setup a timer if one isn't
            already running to automatically update the Date */
         if (edje_object_part_exists(edje, "entrance.date"))
         {
            edje_object_signal_callback_add(edje, "Go", "entrance.date",
                                            set_date, o);
            edje_object_signal_emit(edje, "Go", "entrance.date");
            if (!timer)
               timer = ecore_timer_add(0.5, timer_cb, edje);
         }
         /* See if we have an EntranceSession part, set it to the first
            element in the config's session list */
         if (edje_object_part_exists(edje, "entrance.xsessions.selected"))
         {
            entrance_session_x_session_set(session,
                                           entrance_session_x_session_default_get
                                           (session));
         }
         /* See if we have an EntranceSessionList part, tell the session to
            load the session list if it exists. */
         if (edje_object_part_exists(edje, "entrance.xsessions.list"))
         {
            entrance_session_xsession_list_add(session);
            if ((container_orientation =
                 edje_object_data_get(edje,
                                      "entrance.xsessions.list.orientation")))
            {
               entrance_session_list_direction_set(session,
                                                   session->session_container,
                                                   container_orientation);
            }
            edje_object_signal_callback_add(edje, "drag",
                                            "entrance.xsessions.list.scroller",
                                            _container_scroll,
                                            session->session_container);
         }
         /* See if we have an EntranceUserList part, tell the session to load
            the user list if it exists. */
         if (edje_object_part_exists(edje, "entrance.users.list"))
         {
            entrance_session_user_list_add(session);
            if ((container_orientation =
                 edje_object_data_get(edje,
                                      "entrance.users.list.orientation")))
            {
               entrance_session_list_direction_set(session,
                                                   session->user_container,
                                                   container_orientation);
            }
            edje_object_signal_callback_add(edje, "drag",
                                            "entrance.users.list.scroller",
                                            _container_scroll,
                                            session->user_container);
         }

	 /**
	  * Setup Edje callbacks for signal emissions from our main edje
	  * It's useful to delay showing of your edje till all your
	  * callbacks have been added, otherwise show might not trigger all
	  * the desired events 
	  */
         edje_object_signal_callback_add(edje,
                                         "entrance,user,auth,success,done",
                                         "", done_cb, e);
         edje_object_signal_callback_add(edje, "entrance,system,reboot", "",
                                         reboot_cb, e);
         edje_object_signal_callback_add(edje, "entrance,system,halt", "",
                                         shutdown_cb, e);
         edje_object_signal_callback_add(edje, "entrance,user,xsession,set",
                                         "", _user_session_set, session);
         evas_object_show(edje);
         /* set focus to user input by default */
         edje_object_signal_emit(edje, "In", "entrance.entry.user");
         ecore_event_handler_add(ECORE_X_EVENT_SCREENSAVER_NOTIFY,
                                 screensaver_notify_cb, edje);

         session->current_screen = 0;
      }
   }

   ecore_x_window_cursor_set(ew, ECORE_X_CURSOR_ARROW);
   ecore_evas_cursor_set(e, session->config->pointer, 12, 0, 0);

   ecore_evas_show(e);

   return e;
}
