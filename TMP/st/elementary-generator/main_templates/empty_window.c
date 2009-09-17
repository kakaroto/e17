/*
 * @PROJECT@
 *
 * Copyright (C) @YEAR@, @AUTHOR_NAME@ <@AUTHOR_EMAIL@>
 *
 * License @LICENSE@, see COPYING file at project folder.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <Elementary.h>
#ifndef ELM_LIB_QUICKLAUNCH

#include "gettext.h"

typedef struct _App App;

struct _App
{
   Evas_Object *win;
   Evas_Object *bg;
};

static int _log_domain = -1;
#define CRITICAL(...) EINA_LOG_DOM_CRIT(_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_log_domain, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_log_domain, __VA_ARGS__)

static void
on_win_del_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_exit();
}

static Eina_Bool
create_main_win(App *app)
{
   app->win = elm_win_add(NULL, "@PROJECT@", ELM_WIN_BASIC);
   if (!app->win)
     {
	CRITICAL("Could not create window.");
	return EINA_FALSE;
     }
   elm_win_title_set(app->win, "@PROJECT@");
   evas_object_smart_callback_add
     (app->win, "delete-request", on_win_del_req, NULL);

   app->bg = elm_bg_add(app->win);
   if (!app->bg)
     {
	CRITICAL("Could not create background.");
	return EINA_FALSE;
     }
   evas_object_size_hint_weight_set(app->bg, 1.0, 1.0);
   elm_win_resize_object_add(app->win, app->bg);
   evas_object_show(app->bg);



   evas_object_resize(app->win, 320, 240);
   evas_object_show(app->win);

   return EINA_TRUE;
}

EAPI int
elm_main(int argc __UNUSED__, char **argv __UNUSED__)
{
   App app;
   int r = 0;

#if ENABLE_NLS
   setlocale(LC_ALL, "");
   bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
   textdomain(GETTEXT_PACKAGE);
#endif

   _log_domain = eina_log_domain_register("@PROJECT@", NULL);
   if (_log_domain < 0)
     {
	EINA_LOG_CRIT("could not create log domain '@PROJECT@'.");
	return -1;
     }

   memset(&app, 0, sizeof(app));
   if (!create_main_win(&app))
     {
	r = -1;
	goto end;
     }

   elm_run();
 end:
   eina_log_domain_unregister(_log_domain);
   _log_domain = -1;
   elm_shutdown();
   return r;
}
#endif
ELM_MAIN()
