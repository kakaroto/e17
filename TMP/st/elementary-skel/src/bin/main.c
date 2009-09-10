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
   Evas_Object *frame;
   Evas_Object *box;
   Evas_Object *label;
   struct {
      Evas_Object *quit;
      Evas_Object *inc;
   } button;
   unsigned int count;
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

static void
on_button_inc_clicked(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   App *app = data;
   char buf[128];

   app->count++;
   snprintf(buf, sizeof(buf), _("Counted %u times."), app->count);
   elm_label_label_set(app->label, buf);
}

static void
on_button_quit_clicked(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
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

   app->frame = elm_frame_add(app->win);
   if (!app->frame)
     {
	CRITICAL("Could not create frame.");
	return EINA_FALSE;
     }
   elm_frame_label_set
     (app->frame, _("Click 'Increment Counter' or 'Quit' buttons"));
   evas_object_size_hint_weight_set(app->frame, 1.0, 1.0);
   elm_win_resize_object_add(app->win, app->frame);
   evas_object_show(app->frame);

   app->box = elm_box_add(app->win);
   if (!app->box)
     {
	CRITICAL("Could not create box.");
	return EINA_FALSE;
     }
   elm_box_horizontal_set(app->box, EINA_FALSE);
   elm_box_homogenous_set(app->box, EINA_FALSE);
   evas_object_size_hint_weight_set(app->box, 1.0, 1.0);
   elm_frame_content_set(app->frame, app->box);
   evas_object_show(app->box);

   app->label = elm_label_add(app->win);
   if (!app->label)
     {
	CRITICAL("Could not create label.");
	return EINA_FALSE;
     }
   elm_label_label_set
     (app->label, _("Nothing counted, click 'Increment Counter'."));
   evas_object_size_hint_weight_set(app->label, 1.0, 1.0);
   elm_box_pack_start(app->box, app->label);
   evas_object_show(app->label);

   app->button.inc = elm_button_add(app->win);
   if (!app->button.inc)
     {
	CRITICAL("Could not create 'inc' button.");
	return EINA_FALSE;
     }
   elm_button_label_set(app->button.inc, _("Increment Counter"));
   evas_object_smart_callback_add
     (app->button.inc, "clicked", on_button_inc_clicked, app);
   elm_box_pack_end(app->box, app->button.inc);
   evas_object_show(app->button.inc);

   app->button.quit = elm_button_add(app->win);
   if (!app->button.quit)
     {
	CRITICAL("Could not create 'quit' button.");
	return EINA_FALSE;
     }
   elm_button_label_set(app->button.quit, _("Quit"));
   evas_object_smart_callback_add
     (app->button.quit, "clicked", on_button_quit_clicked, app);
   elm_box_pack_end(app->box, app->button.quit);
   evas_object_show(app->button.quit);

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
