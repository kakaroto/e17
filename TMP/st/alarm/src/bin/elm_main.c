#include <Elementary.h>
#include "elm_priv.h"

Elm_Engine _elm_engine = ELM_SOFTWARE_X11;
char *_elm_appname = NULL;

static int
_elm_signal_exit(void *data, int ev_type, void *ev)
{
   elm_exit();
   return 1;
}

EAPI void
elm_init(int argc, char **argv)
{
   int i;
   
   eet_init();
   ecore_init();
   ecore_app_args_set(argc, (const char **)argv);
   ecore_file_init();
   evas_init();
   edje_init();
   ecore_evas_init(); // FIXME: check errors
   
   ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, _elm_signal_exit, NULL);
   
   _elm_appname = strdup(ecore_file_file_get(argv[0]));
}

EAPI void
elm_shutdown(void)
{
   free(_elm_appname);
   ecore_evas_shutdown();
   edje_shutdown();
   evas_shutdown();
   ecore_file_shutdown();
   ecore_shutdown();
   eet_shutdown();
}

EAPI void
elm_run(void)
{
   ecore_main_loop_begin();
}

EAPI void
elm_exit(void)
{
   ecore_main_loop_quit();
}
