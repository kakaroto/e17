/*
 * eve
 *
 * Copyright (C) 2009, Gustavo Sverzut Barbieri <barbieri@profusion.mobi>
 *
 * License LGPL-3, see COPYING file at project folder.
 */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
#include <Elementary.h>
#ifndef ELM_LIB_QUICKLAUNCH

#include "private.h"

#include <Ecore_Getopt.h>
#include <Ecore_File.h>
#include <E_DBus.h>
#include <Ecore_X.h>
#include <stdlib.h>
#include "gettext.h"

int _log_domain = -1;
Hist *hist = NULL;
Fav *fav = NULL;
Config *config = NULL;
App app;

struct Cursor {
  Evas_Object *object;
  int layer;
  int hot_x, hot_y;
};

struct Eve_DBus_Request_Name_Response {
  E_DBus_Connection *conn;
  const char *url;
};

static void
del_win(App *app, Evas_Object *win)
{
   Browser_Window *win_data;
   Eina_List *l;

   EINA_LIST_FOREACH(app->windows, l, win_data) if (win_data->win == win)
      break;

   evas_object_del(win);
   app->windows = eina_list_remove(app->windows, win_data);
   free(win_data);

   if (!app->windows)
      elm_exit();
}

static void
on_win_del_req(void *data, Evas_Object *win, void *event_info __UNUSED__)
{
   del_win(data, win);
}

void
window_mouse_enabled_set(Evas_Object *win, Eina_Bool setting)
{
   Evas *e = evas_object_evas_get(win);
   Ecore_Evas *ee = evas_data_attach_get(e);
   static struct Cursor *default_cursor = NULL;

   if (!default_cursor)
      {
         if (!(default_cursor = calloc(1, sizeof(*default_cursor)))) return;
         ecore_evas_cursor_get(ee, &default_cursor->object, &default_cursor->layer,
                               &default_cursor->hot_x, &default_cursor->hot_y);
      }

   if (!setting)
      {
          Evas_Object *cursor = evas_object_rectangle_add(e);

          evas_object_color_set(cursor, 0, 0, 0, 0);
          evas_object_resize(cursor, 1, 1);
          ecore_evas_object_cursor_set(ee, cursor, EVAS_LAYER_MIN, 0, 0);
      }
   else
      ecore_evas_object_cursor_set(ee, default_cursor->object, default_cursor->layer,
                                   default_cursor->hot_x, default_cursor->hot_y);
}

Eina_Bool
tab_add(Browser_Window *win, const char *url)
{
   Evas_Object *chrome = chrome_add(win, url);

   if (!chrome)
     {
        CRITICAL("Could not create chrome.");
        goto error_chrome_create;
     }

   evas_object_size_hint_weight_set(chrome, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   evas_object_show(chrome);

   win->chromes = eina_list_append(win->chromes, chrome);
   win->current_chrome = chrome;
   win->current_view = evas_object_data_get(chrome, "view");
   win->current_tab++;

   return EINA_TRUE;

error_chrome_create:
   evas_object_del(evas_object_data_get(chrome, "view"));

   return EINA_FALSE;
}

Eina_Bool
tab_focus_chrome(Browser_Window *win, Evas_Object *chrome)
{
   Eina_List *itr;
   int n;

   if (!chrome)
      return EINA_FALSE;

   for (n = 0, itr = win->chromes; itr->data != chrome; n++, itr = itr->next) ;

   evas_object_hide(win->current_chrome);

   win->current_chrome = chrome;
   win->current_view = evas_object_data_get(chrome, "view");
   win->current_tab = n;

   evas_object_show(win->current_chrome);
   evas_object_focus_set(win->current_view, EINA_TRUE);
   elm_pager_content_promote(win->pager, win->current_chrome);

   chrome_focused_notify(win->current_chrome);

   return EINA_TRUE;
}

Eina_Bool
tab_focus_nth(Browser_Window *win, unsigned int n)
{
   return tab_focus_chrome(win, eina_list_nth(win->chromes, n));
}

Eina_Bool
tab_focus_next(Browser_Window *win)
{
   unsigned int n_tabs = eina_list_count(win->chromes);

   if (win->current_tab > n_tabs)
      return EINA_FALSE;

   return tab_focus_nth(win, win->current_tab + 1);
}

Eina_Bool
tab_focus_prev(Browser_Window *win)
{
   if (win->current_tab == 0)
      return EINA_FALSE;

   return tab_focus_nth(win, win->current_tab - 1);
}

Eina_Bool
tab_close_chrome(Browser_Window *win, Evas_Object *chrome)
{
   Evas_Object *edje;

   EINA_SAFETY_ON_TRUE_RETURN_VAL(!win, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!chrome, EINA_FALSE);

   evas_object_del(chrome);
   win->chromes = eina_list_remove(win->chromes, chrome);
   if (!win->chromes)
     {
        del_win(win->app, win->win);
        return EINA_TRUE;
     }

   if (win->current_chrome == chrome)
      tab_focus_nth(win, 0);

   edje = elm_layout_edje_get(win->current_chrome);
   edje_object_signal_emit(edje, "hide,tab", "");

   return EINA_TRUE;
}

Eina_Bool
tab_close_nth(Browser_Window *win, int n)
{
   return tab_close_chrome(win, eina_list_nth(win->chromes, n));
}

Eina_Bool
tab_close_view(Browser_Window *win, Evas_Object *view)
{
   return tab_close_chrome(win, evas_object_data_get(view, "chrome"));
}

static Browser_Window *
add_win(App *app, const char *url)
{
   Browser_Window *win = malloc(sizeof(*win));

   if (!win)
     {
        CRITICAL("Could not create window data.");
        goto error_win_data;
     }

   win->app = app;
   win->chromes = NULL;
   win->current_chrome = NULL;
   win->current_view = NULL;
   win->current_tab = 0;
   win->list_history = NULL;
   win->list_history_titles = NULL;

   win->win = elm_win_add(NULL, "eve", ELM_WIN_BASIC);
   if (!win->win)
     {
        CRITICAL("Could not create window.");
        goto error_win_create;
     }

   elm_win_title_set(win->win, PACKAGE_STRING);
   elm_win_rotation_set(win->win, app->rotate);
   elm_win_fullscreen_set(win->win, app->is_fullscreen);
   window_mouse_enabled_set(win->win, config_enable_mouse_cursor_get(config));

   win->bg = edje_object_add(evas_object_evas_get(win->win));
   if (!win->bg)
     {
        CRITICAL("Could not create background.");
        goto error_bg_create;
     }

   if (!edje_object_file_set(win->bg, PACKAGE_DATA_DIR "/default.edj", "bg"))
     {
        int err = edje_object_load_error_get(win->bg);

        const char *msg = edje_load_error_str(err);

        CRITICAL("Could not load background theme: %s", msg);
        goto error_bg_theme_set;
     }

   evas_object_size_hint_weight_set(win->bg, EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win->win, win->bg);
   evas_object_layer_set(win->bg, EVAS_LAYER_MIN);
   evas_object_show(win->bg);

   win->pager = elm_pager_add(win->win);
   if (!win->pager)
     {
        CRITICAL("Could not create pager");
        goto error_pager_create;
     }

   elm_object_style_set(win->pager, "ewebkit");
   evas_object_size_hint_weight_set(win->pager, EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win->win, win->pager);
   evas_object_show(win->pager);

   if (!tab_add(win, url))
      goto error_tab_add;

   app->windows = eina_list_append(app->windows, win);
   evas_object_smart_callback_add
      (win->win, "delete-request", on_win_del_req, app);

   evas_object_resize(win->win, 480, 800);
   evas_object_show(win->win);

   return win;

error_bg_theme_set:
   evas_object_del(win->bg);
error_bg_create:
   evas_object_del(win->win);
error_win_create:
   free(win);
error_win_data:
error_tab_add:
error_pager_create:
   return NULL;
}

/**
 * Creates a new window, without any url to load, calling add_win().
 *
 * @return If a window was successfully created, it returns the correspondent view
 * object.
 */
Evas_Object *
window_create(void)
{
   Browser_Window *win = add_win(&app, NULL);

   if (!win)
      return NULL;

   return win->current_view;
}

void
window_title_set(Browser_Window *win, const char *title)
{
   char buf[4096];

   EINA_SAFETY_ON_NULL_RETURN(win);

   if (!title)
     {
        elm_win_title_set(win->win, PACKAGE_STRING);
        return;
     }

   snprintf(buf, sizeof(buf), "%s - %s", title, PACKAGE_STRING);
   elm_win_title_set(win->win, buf);
}

#define stringify(X) #X

static const Ecore_Getopt options = {
   PACKAGE_NAME,
   "%prog [options] [url]",
   PACKAGE_VERSION "Revision:" stringify(VREV),
   "(C) 2010 ProFUSION embedded systems",
   "LGPL-3",
   "WebKit-EFL demo browser for mobile systems with touchscreen.",
   EINA_TRUE,
   {
      ECORE_GETOPT_STORE_DEF_BOOL('F', "fullscreen", "start in fullscreen.", 1),
      ECORE_GETOPT_STORE_DEF_BOOL('P', "disable-plugins",
                                  "disable plugins (flash, etc).", 1),
      ECORE_GETOPT_STORE_DEF_BOOL('M', "disable-mouse",
                                  "disable mouse (hide it).", 1),
      ECORE_GETOPT_STORE_DEF_BOOL('T', "disable-touch-interface",
                                  "disable touch interface handling of mouse events", 1),
      ECORE_GETOPT_STORE_STR('U', "user-agent",
                             "user agent string to use. Special cases=iphone,safari,chrome,firefox,ie,ie9,ie8,ie7."),
      ECORE_GETOPT_STORE_DEF_UINT('R', "rotate", "Screen Rotation in degrees", 0),
      ECORE_GETOPT_VERSION('V', "version"),
      ECORE_GETOPT_COPYRIGHT('C', "copyright"),
      ECORE_GETOPT_LICENSE('L', "license"),
      ECORE_GETOPT_HELP('h', "help"),
      ECORE_GETOPT_SENTINEL
   }
};

static DBusMessage *
_cb_dbus_open_url(E_DBus_Object *obj __UNUSED__, DBusMessage *msg)
{
   DBusMessage *reply;
   Browser_Window *win = eina_list_data_get(app.windows);
   char *new_url;

   dbus_message_get_args(msg, NULL, DBUS_TYPE_STRING, &new_url, DBUS_TYPE_INVALID);
   tab_add(win, new_url);
   ecore_x_window_focus(elm_win_xwindow_get(win->win));

   return dbus_message_new_method_return(msg);
}

static void
_cb_dbus_request_name(void *data, DBusMessage *msg __UNUSED__, DBusError *err)
{
   struct Eve_DBus_Request_Name_Response *response = data;
   DBusError new_err;
   dbus_uint32_t ret;

   if (dbus_error_is_set(err))
      {
         dbus_error_free(err);
         goto cleanup;
      }

   dbus_error_init(&new_err);
   dbus_message_get_args(msg, &new_err, DBUS_TYPE_UINT32, &ret, DBUS_TYPE_INVALID);

   switch (ret) {
   case DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER:
   case DBUS_REQUEST_NAME_REPLY_ALREADY_OWNER:
      {
         E_DBus_Interface *iface = e_dbus_interface_new("mobi.profusion.eve");
         E_DBus_Object *eve_dbus = e_dbus_object_add(response->conn, "/mobi/profusion/eve", NULL);
         e_dbus_interface_method_add(iface, "open_url", "s", "", _cb_dbus_open_url);
         e_dbus_object_interface_attach(eve_dbus, iface);
      }
      break;
   case DBUS_REQUEST_NAME_REPLY_IN_QUEUE:
   case DBUS_REQUEST_NAME_REPLY_EXISTS:
      {
         DBusMessage *open_url = dbus_message_new_method_call("mobi.profusion.eve",
                                                              "/mobi/profusion/eve",
                                                              "mobi.profusion.eve",
                                                              "open_url");
         DBusMessageIter iter;
         dbus_message_iter_init_append(open_url, &iter);
         dbus_message_iter_append_basic(&iter, DBUS_TYPE_STRING, &response->url);
         e_dbus_message_send(response->conn, open_url, NULL, -1, NULL);
         dbus_message_unref(open_url);
      }
      exit(0);
   }

cleanup:
   free(response);
}

static void
state_save(void)
{
   Browser_Window *win;
   Eina_List *win_iter;

   if (!config_restore_state_get(config)) return;
   prefs_state_list_clear(prefs);

   EINA_LIST_FOREACH(app.windows, win_iter, win)
   {
      Evas_Object *chrome;
      Eina_List *chrome_iter;
      EINA_LIST_FOREACH(win->chromes, chrome_iter, chrome)
      {
         Evas_Object *view = evas_object_data_get(chrome, "view");
         prefs_state_add(prefs, prefs_opened_tab_new(ewk_view_uri_get(view)));
      }
   }
}

EAPI int
elm_main(int argc, char **argv)
{
   int r = 0, args;
   const char *home;
   const char *url;
   char path[PATH_MAX], *basename;
   Eina_Bool quit_option = EINA_FALSE;
   Eina_Bool disable_plugins = 0xff;
   Eina_Bool disable_mouse_cursor = 0xff;
   Eina_Bool disable_touch_interface = 0xff;
   char *user_agent_option = NULL;
   const char *user_agent_str;
   E_DBus_Connection *conn;
   size_t dirlen;

   Ecore_Getopt_Value values[] = {
      ECORE_GETOPT_VALUE_BOOL(app.is_fullscreen),
      ECORE_GETOPT_VALUE_BOOL(disable_plugins),
      ECORE_GETOPT_VALUE_BOOL(disable_mouse_cursor),
      ECORE_GETOPT_VALUE_BOOL(disable_touch_interface),
      ECORE_GETOPT_VALUE_STR(user_agent_option),
      ECORE_GETOPT_VALUE_UINT(app.rotate),
      ECORE_GETOPT_VALUE_BOOL(quit_option),
      ECORE_GETOPT_VALUE_BOOL(quit_option),
      ECORE_GETOPT_VALUE_BOOL(quit_option),
      ECORE_GETOPT_VALUE_BOOL(quit_option),
      ECORE_GETOPT_VALUE_NONE
   };

#if ENABLE_NLS
   setlocale(LC_ALL, "");
   bindtextdomain(GETTEXT_PACKAGE, LOCALEDIR);
   bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
   textdomain(GETTEXT_PACKAGE);
#endif

   _log_domain = eina_log_domain_register("eve", NULL);
   if (_log_domain < 0)
     {
        EINA_LOG_CRIT("could not create log domain 'eve'.");
        return -1;
     }

   args = ecore_getopt_parse(&options, values, argc, argv);
   if (args < 0)
     {
        ERR("Could not parse command line options.");
        return -1;
     }

   if (quit_option)
     {
        DBG("Command lines option requires quit.");
        return 0;
     }

   if (!user_agent_option ||
       (user_agent_option && (strcasecmp(user_agent_option, "eve") == 0)))
      user_agent_str = "Mozilla/5.0 (iPhone; U; CPU like Mac OS X; en) AppleWebKit/420+ (KHTML, like Gecko) Version/3.0 Mobile/1A543a Safari/419.3 " PACKAGE_NAME "/" PACKAGE_VERSION;
   else
     {
        /* http://www.useragentstring.com/ */

        if (strcasecmp(user_agent_option, "iphone") == 0)
          user_agent_str = "Mozilla/5.0 (iPhone; U; CPU like Mac OS X; en) AppleWebKit/420+ (KHTML, like Gecko) Version/3.0 Mobile/1A543a Safari/419.3";
        else if (strcasecmp(user_agent_option, "safari") == 0)
          user_agent_str = "Mozilla/5.0 (Macintosh; U; Intel Mac OS X 10_6_3; en-US) AppleWebKit/533.17.8 (KHTML, like Gecko) Version/5.0.1 Safari/533.17.8";

        else if (strcasecmp(user_agent_option, "chrome") == 0)
          user_agent_str = "Mozilla/5.0 (X11; U; Linux x86_64; en-US) AppleWebKit/534.7 (KHTML, like Gecko) Chrome/7.0.514.0 Safari/534.7";
        else if (strcasecmp(user_agent_option, "firefox") == 0)
          user_agent_str = "Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.0.2) Gecko/20121223 Firefox/3.8";

        else if (strcasecmp(user_agent_option, "ie") == 0) /* last */
          user_agent_str = "Mozilla/5.0 (Windows; U; MSIE 9.0; Windows NT 9.0; en-US)";
        else if (strcasecmp(user_agent_option, "ie9") == 0)
          user_agent_str = "Mozilla/5.0 (Windows; U; MSIE 9.0; Windows NT 9.0; en-US)";
        else if (strcasecmp(user_agent_option, "ie8") == 0)
          user_agent_str = "Mozilla/5.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; SLCC1; .NET CLR 3.0.4506.2152; .NET CLR 3.5.30729; .NET CLR 1.1.4322)";
        else if (strcasecmp(user_agent_option, "ie7") == 0)
          user_agent_str = "Mozilla/4.0 (compatible; MSIE 7.0b; Windows NT 6.0)";
        else
          user_agent_str = user_agent_option;
     }

   elm_theme_extension_add(NULL, PACKAGE_DATA_DIR "/default.edj");
   ewk_init();
   eve_state_init();
   e_dbus_init();

   home = getenv("HOME");
   if (!home || !home[0])
     {
        CRITICAL("Could not get $HOME");
        r = -1;
        goto end;
     }

   dirlen = snprintf(path, sizeof(path), "%s/.config/eve", home);
   if (dirlen >= sizeof(path))
     {
        ERR("Path is too long: %s/.config/eve", home);
        r = -1;
        goto end;
     }

   if (!ecore_file_mkpath(path))
     {
        ERR("Could not create %s", path);
        r = -1;
        goto end;
     }

   if (!ewk_settings_icon_database_path_set(path))
     {
        ERR("Could not set icon database path to %s", path);
        r = -1;
        goto end;
     }

   basename = path + dirlen;
   basename[0] = '/';
   basename++;
   dirlen++;
   eina_strlcpy(basename, "favorites.db", sizeof(path) - dirlen);
   fav = fav_load(path);
   if (!fav)
     {
        fav = fav_new(0);
        fav_save(fav, path);
     }

   eina_strlcpy(basename, "history.db", sizeof(path) - dirlen);
   hist = hist_load(path);
   if (!hist)
     {
        hist = hist_new(0);
        hist_save(hist, path);
     }

   eina_strlcpy(basename, "prefs.db", sizeof(path) - dirlen);
   prefs = prefs_load(path);
   if (!prefs)
     {
        Eina_Bool enable_mouse_cursor, enable_touch_interface, enable_plugins;

#define BOOL_OPT(opt)                                                   \
        enable_##opt = ((disable_##opt == 0xff) ? EINA_TRUE : !disable_##opt)
        BOOL_OPT(mouse_cursor);
        BOOL_OPT(touch_interface);
        BOOL_OPT(plugins);
#undef BOOL_OPT

        config = config_new(EINA_TRUE /* allow_popup */,
        		    EINA_TRUE /* enable_auto_load_images */,
        		    EINA_TRUE /* enable_auto_shrink_images */,
        		    EINA_TRUE /* enable_javascript */,
        		    enable_mouse_cursor,
        		    enable_plugins,
        		    EINA_FALSE /* enable_private_mode */,
        		    enable_touch_interface,
        		    DEFAULT_URL /* home_page */,
        		    NULL /* proxy */,
        		    EINA_FALSE /* restore_state */,
        		    user_agent_str);
        if (!config_save(config, path))
          {
             r = -1;
             goto end_config;
          }
     }

   hist = hist_load(path);
   if (!hist)
     {
        hist = hist_new();
        if (!hist_save(hist, path))
          {
             r = -1;
             goto end_hist;
          }
     }
   
   fav = fav_load(path);
   if (!fav)
     {
        fav = fav_new();
        if (!fav_save(fav, path))
          {
             r = -1;
             goto end_fav;
          }
     }

#define BOOL_OPT(opt)                                           \
   if (disable_##opt != 0xff)                                   \
     {                                                          \
        Eina_Bool old = config_enable_##opt##_get(config);      \
        Eina_Bool cur = !disable_##opt;                         \
        if (old != cur)                                         \
          {                                                     \
             INF("Changed preferences to "#opt"=%hhu", cur);    \
             config_enable_##opt##_set(config, cur);            \
          }                                                     \
     }
   BOOL_OPT(mouse_cursor);
   BOOL_OPT(touch_interface);
   BOOL_OPT(plugins);
#undef BOOL_OPT

   if (user_agent_option)
     {
        const char *old = config_user_agent_get(config);
        const char *cur = user_agent_str;
        if (strcmp(old, cur) != 0)
          {
             INF("Changed preferences to user_agent=\"%s\"", cur);
             config_user_agent_set(config, cur);
          }
     }

   if (args < argc)
      url = argv[args];
   else
      url = config_home_page_get(config);

   conn = e_dbus_bus_get(DBUS_BUS_SESSION);
   if (conn)
     {
        struct Eve_DBus_Request_Name_Response *response = calloc(1, sizeof(*response));
        if (!response) goto end;

        response->conn = conn;
        response->url = url;

        e_dbus_request_name(conn, "mobi.profusion.eve", 0, _cb_dbus_request_name, response);
     }

#if 0
   if (prefs_restore_state_get(prefs) && prefs_state_count(config) > 0)
     {
        Eina_List *previous_state = prefs_state_list_get(prefs);
        Eina_List *state_iter;
        Prefs_Opened_Tab *tab = eina_list_data_get(previous_state);
        Browser_Window *win;

        if (!add_win(&app, prefs_opened_tab_address_get(tab)))
          {
             r = -1;
             goto end;
          }

        win = eina_list_data_get(app.windows);
        EINA_LIST_FOREACH(previous_state->next, state_iter, tab)
          tab_add(win, prefs_opened_tab_address_get(tab));
     }
   else
#else
   if (!add_win(&app, url))
     {
        r = -1;
        goto end;
     }
#endif

   elm_run();
end:
   config_save(config, NULL);
   config_free(config);
end_config:
   hist_save(hist, NULL);
   hist_free(hist);
end_hist:
   fav_save(fav, NULL);
   fav_free(fav);
end_fav:
   if (conn) e_dbus_connection_close(conn);

   eina_log_domain_unregister(_log_domain);
   _log_domain = -1;
   elm_shutdown();
   ewk_shutdown();
   eve_state_shutdown();
   e_dbus_shutdown();
   return r;
}

#endif
ELM_MAIN()
