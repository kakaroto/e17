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
typedef struct _Genlist_Data Genlist_Data;

struct _App
{
   Evas_Object *win;
   Evas_Object *bg;
   struct
     {
	Evas_Object *vbox;
	Evas_Object *hbox;
     } box;
   Evas_Object *genlist;
   struct
     {
	Evas_Object *pager;
	Evas_Object *p_welcome;
	Evas_Object *p_wolf;
     } pager;
   struct
     {
	Evas_Object *quit;
     } button;
   unsigned int count;
};

struct _Genlist_Data
{
   App *app;
   Evas_Object *p_pager;
};

static Elm_Genlist_Item_Class itc;
static int _log_domain = -1;

#define CRITICAL(...) EINA_LOG_DOM_CRIT(_log_domain, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_log_domain, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_WARN(_log_domain, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_log_domain, __VA_ARGS__)
#define DBG(...) EINA_LOG_DOM_DBG(_log_domain, __VA_ARGS__)

   static char *
_gl_label_get(const void *data, Evas_Object *obj, const char *part)
{
   Genlist_Data *g_data = (Genlist_Data*) data;

   if(g_data->p_pager == g_data->app->pager.p_welcome)
     return strdup("Welcome");
   else if(g_data->p_pager == g_data->app->pager.p_wolf)
     return strdup("Wolf");
   return strdup("Error");
}

   static void
_gl_sel(void *data, Evas_Object *obj, void *event_info)
{
   Genlist_Data *g_data = (Genlist_Data*) data;

   if(g_data->p_pager == g_data->app->pager.p_welcome)
     elm_pager_content_promote(g_data->app->pager.pager, g_data->app->pager.p_welcome);
   else if(g_data->p_pager == g_data->app->pager.p_wolf)
      elm_pager_content_promote(g_data->app->pager.pager, g_data->app->pager.p_wolf);
}

   static void
on_win_del_req(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_exit();
}

   static void
on_button_quit_clicked(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_exit();
}

   static Eina_Bool
create_main_win(App *app)
{
   Evas_Object *lb;

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

   app->box.vbox = elm_box_add(app->win);
   evas_object_size_hint_weight_set(app->box.vbox, 1.0, 1.0);
   elm_win_resize_object_add(app->win, app->box.vbox);
   evas_object_show(app->box.vbox);

   app->box.hbox = elm_box_add(app->win);
   elm_box_horizontal_set(app->box.hbox, EINA_TRUE);
   evas_object_size_hint_weight_set(app->box.hbox, 1.0, 1.0);
   evas_object_size_hint_align_set(app->box.hbox, -1.0, -1.0);
   evas_object_show(app->box.hbox);
   elm_box_pack_end(app->box.vbox, app->box.hbox);

   app->pager.pager = elm_pager_add(app->win);
   evas_object_size_hint_weight_set(app->pager.pager, 1.0, 1.0);
   evas_object_size_hint_align_set(app->pager.pager, 1.0, -1.0);
   elm_box_pack_end(app->box.hbox, app->pager.pager);
   evas_object_show(app->pager.pager);

   //page 1
   app->pager.p_welcome = elm_box_add(app->win);
   evas_object_size_hint_weight_set(app->pager.p_welcome, 1.0, 1.0);
   evas_object_show(app->pager.p_welcome);
   lb = elm_label_add(app->win);
   elm_label_label_set(lb,
		       "This is page 1 in a pager stack.<br>"
		       "<br>"
		       "Welcome in the world of Elementary !"
		       );
   elm_box_pack_end(app->pager.p_welcome, lb);
   evas_object_show(lb);
   elm_pager_content_push(app->pager.pager, app->pager.p_welcome);
   //

   //page 2
   app->pager.p_wolf = elm_box_add(app->win);
   evas_object_size_hint_weight_set(app->pager.p_wolf, 1.0, 1.0);
   evas_object_show(app->pager.p_wolf);
   lb = elm_label_add(app->win);
   elm_label_label_set(lb,
		       "This is page 2 in a pager stack.<br>"
		       "<br>"
		       "Only in Ravnica do the wolves watch the flock."
		       );
   elm_box_pack_end(app->pager.p_wolf, lb);
   evas_object_show(lb);
   elm_pager_content_push(app->pager.pager, app->pager.p_wolf);
   //

   elm_pager_content_promote(app->pager.pager, app->pager.p_welcome);

   //the genlist
   app->genlist = elm_genlist_add(app->win);
   elm_genlist_horizontal_mode_set(app->genlist, ELM_LIST_LIMIT);
   evas_object_size_hint_weight_set(app->genlist, 1.0, 1.0);
   evas_object_size_hint_align_set(app->genlist, -1.0, -1.0);
   elm_box_pack_start(app->box.hbox, app->genlist);
   evas_object_show(app->genlist);

   itc.item_style     = "default";
   itc.func.label_get = _gl_label_get;

   Genlist_Data *data = calloc(1, sizeof(Genlist_Data));
   data->app = app;
   data->p_pager = app->pager.p_welcome;
   elm_genlist_item_append(app->genlist, &itc,
	 data/* item data */,
	 NULL/* parent */,
	 ELM_GENLIST_ITEM_NONE,
	 _gl_sel/* func */,
	 data/* func data */);

   data = calloc(1, sizeof(Genlist_Data));
   data->app = app;
   data->p_pager = app->pager.p_wolf;
   elm_genlist_item_append(app->genlist, &itc,
	 data/* item data */,
	 NULL/* parent */,
	 ELM_GENLIST_ITEM_NONE,
	 _gl_sel/* func */,
	 data/* func data */);
   //

   app->button.quit = elm_button_add(app->win);
   if (!app->button.quit)
     {
	CRITICAL("Could not create 'quit' button.");
	return EINA_FALSE;
     }
   elm_button_label_set(app->button.quit, _("Quit"));
   evas_object_smart_callback_add
      (app->button.quit, "clicked", on_button_quit_clicked, app);
   evas_object_size_hint_weight_set(app->button.quit, -1.0, -1.0);
   evas_object_size_hint_align_set(app->button.quit, -1.0, 0.0);
   elm_box_pack_end(app->box.vbox, app->button.quit);
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
