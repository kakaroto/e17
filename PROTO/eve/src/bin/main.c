#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <string.h>

#include <Ecore.h>
#include <Ecore_Data.h>
#include <Ecore_Getopt.h>
#include <Ecore_Evas.h>
#include <Edje.h>
#include <EWebKit.h>

#include "eve_navigator.h"

static const Ecore_Getopt options = {
  "eve",
  "%prog [options] [url]",
  "0.1.0",
  "(C) 2008 Enlightenment",
  "BSD with advertisement clause",
  "Web Browser using Enlightenment Foundation Libraries and WebKit",
  1,
  {
    ECORE_GETOPT_STORE_STR('e', "engine", "ecore-evas engine to use"),
    ECORE_GETOPT_CALLBACK_NOARGS
    ('E', "list-engines", "list ecore-evas engines",
     ecore_getopt_callback_ecore_evas_list_engines, NULL),
    ECORE_GETOPT_STORE_DEF_BOOL('F', "fullscreen", "fullscreen mode", 0),
    ECORE_GETOPT_CALLBACK_ARGS
    ('g', "geometry", "geometry to use in x:y:w:h form.", "X:Y:W:H",
     ecore_getopt_callback_geometry_parse, NULL),
    ECORE_GETOPT_STORE_STR
    ('t', "theme", "path to read the theme file from"),
    ECORE_GETOPT_COUNT('v', "verbose", "be more verbose"),
    ECORE_GETOPT_VERSION('V', "version"),
    ECORE_GETOPT_COPYRIGHT('R', "copyright"),
    ECORE_GETOPT_LICENSE('L', "license"),
    ECORE_GETOPT_HELP('h', "help"),
    ECORE_GETOPT_SENTINEL
  }
};

struct app
{
   Ecore_Evas *ee;
   Evas *evas;
   Evas_Object *navigator;
};

static void
_eve_navitagator_on_del(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   ecore_main_loop_quit();
}

static void
_eve_navigator_on_key_down(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   struct app *app = data;
   Evas_Event_Key_Down *ev = event_info;
   char *value;

   value = ev->keyname;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD)
     {
	fprintf(stderr, "DBG: key down event '%s' is on hold, ignored.\n",
		value);
	return;
     }

   if (strcmp(value, "F11") == 0)
     ecore_evas_fullscreen_set(app->ee, !ecore_evas_fullscreen_get(app->ee));
}

static void
_eve_navigator_on_title_changed(void *data, Evas_Object *obj, void *event_info)
{
   struct app *app = data;
   EWebKit_Event_Title_Changed *ev = event_info;
   char *win_title;
   int len;

   len = strlen(ev->title);
   win_title = alloca(len + sizeof(" - eve"));
   if (!win_title)
     return;
   memcpy(win_title, ev->title, len);
   memcpy(win_title + len, " - eve", sizeof(" - eve"));
   ecore_evas_title_set(app->ee, win_title);
}

static int
main_signal_exit(void *data, int ev_type, void *ev)
{
   ecore_main_loop_quit();
   return 1;
}

int
main(int argc, char *argv[])
{
   Eina_Rectangle geometry = {0, 0, 0, 0};
   Evas_Object *o;
   char *engine = NULL;
   char *theme = NULL;
   char *url = "http://www.google.com";
   int verbose = 0;
   unsigned char quit_option = 0;
   unsigned char is_fullscreen = 0;
   int args;
   Ecore_Getopt_Value values[] = {
     ECORE_GETOPT_VALUE_STR(engine),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(is_fullscreen),
     ECORE_GETOPT_VALUE_PTR_CAST(geometry),
     ECORE_GETOPT_VALUE_STR(theme),
     ECORE_GETOPT_VALUE_INT(verbose),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_BOOL(quit_option),
     ECORE_GETOPT_VALUE_NONE
   };
   struct app app;

   if (!ecore_evas_init())
     return EXIT_FAILURE;

   if (!edje_init())
     goto shutdown_ecore_evas;

   ecore_app_args_set(argc, (const char **)argv);

   args = ecore_getopt_parse(&options, values, argc, argv);
   if (args < 0)
     {
        fputs("ERROR: could not parse options.\n", stderr);
        goto shutdown_edje;
     }

   if (quit_option)
     goto shutdown_edje;

   if (args < argc)
     url = argv[args];

   if ((geometry.w <= 0) && (geometry.h <= 0))
     {
        geometry.w = 640;
        geometry.h = 480;
     }

   if (!theme)
     theme = getenv("EVE_THEME");

   app.ee = ecore_evas_new(engine, 0, 0, geometry.w, geometry.h, NULL);
   if (!app.ee)
     goto shutdown_edje;

   ecore_evas_title_set(app.ee, "eve");
   app.evas = ecore_evas_get(app.ee);

   if (!app.evas)
     goto shutdown_edje;

   ewk_init(app.evas);

   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, main_signal_exit, NULL);

   o = eve_navigator_add(app.evas);
   if (theme)
     eve_navigator_theme_file_set(o, theme);
   evas_object_resize(o, geometry.w, geometry.h);
   ecore_evas_object_associate(app.ee, o, 0);
   evas_object_event_callback_add
     (o, EVAS_CALLBACK_DEL, _eve_navitagator_on_del, &app);
   evas_object_event_callback_add
     (o, EVAS_CALLBACK_KEY_DOWN, _eve_navigator_on_key_down, &app);
   evas_object_focus_set(eve_navigator_webview_get(o), 1);
   ewk_callback_title_changed_add
      (eve_navigator_webview_get(o), _eve_navigator_on_title_changed, &app);
   evas_object_show(o);
   app.navigator = o;

   if (url && (url[0] != '\0') && (strcmp(url, "about:blank") != 0))
     {
        // will load url as soon as the mainloop is running
        eve_navigator_load_url(app.navigator, url);
     }

   if (is_fullscreen)
     ecore_evas_fullscreen_set(app.ee, 1);

   ecore_main_loop_begin();

   ewk_shutdown();
   edje_shutdown();
   ecore_evas_shutdown();

   return EXIT_SUCCESS;

 shutdown_edje:
   edje_shutdown();
 shutdown_ecore_evas:
   ecore_evas_shutdown();

   return EXIT_FAILURE;
}
