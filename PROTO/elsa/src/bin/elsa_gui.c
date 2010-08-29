#include "elsa.h"
#include "Ecore_X.h"

#define ELSA_GUI_GET(edj, name) edje_object_part_external_object_get(elm_layout_edje_get(edj), name)

typedef struct Elsa_Gui_ Elsa_Gui;
typedef struct Elsa_Xsession_ Elsa_Xsession;

struct Elsa_Gui_
{
   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *edj;
   Eina_List *xsessions;
   Elsa_Xsession *selected_session;
};

struct Elsa_Xsession_
{
   const char *name;
   const char *command;
   const char *icon;
};

static Evas_Object *_elsa_gui_theme_get(Evas_Object *win, const char *group);
static void _elsa_gui_hostname_activated_cb(void *data, Evas_Object *obj, void *event_info);
static void _elsa_gui_password_activated_cb(void *data, Evas_Object *obj, void *event_info);
static void _elsa_gui_shutdown(void *data, Evas_Object *obj, void *event_info);

static Elsa_Gui *_gui;

static Evas_Object *
_elsa_gui_theme_get (Evas_Object *win, const char *group)
{
   Evas_Object *edje = NULL;

   edje = elm_layout_add(win);
   elm_layout_file_set(edje, PACKAGE_DATA_DIR"/themes/default.edj", group);

   return edje;
}

static void
_elsa_gui_hostname_activated_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   char *txt;

   txt = elsa_gui_user_get();
   if (!strcmp(txt, ""))
     {
        free(txt);
        return;
     }
   free(txt);
   elm_object_focus(data);
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                           "elsa.auth.enable", "");
}

static void
_elsa_gui_login_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *sig __UNUSED__, const char *src __UNUSED__)
{
   elsa_session_run(data, elsa_gui_login_command_get());
   elsa_gui_shutdown();
}

static void
_elsa_gui_login_cancel_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *sig __UNUSED__, const char *src __UNUSED__)
{
   Evas_Object *o;

   o = ELSA_GUI_GET(_gui->edj, "hostname");
   elm_scrolled_entry_entry_set(o, "");
   o = ELSA_GUI_GET(_gui->edj, "password");
   elm_scrolled_entry_entry_set(o, "");
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                           "elsa.auth.disable", "");
}

static void
_elsa_gui_login_request_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *sig __UNUSED__, const char *src __UNUSED__)
{
   char *h, *s;
   h = elsa_gui_user_get();
   s = elsa_gui_password_get();
   if (h && s)
     {
        if (strcmp(h, "") && strcmp(s, ""))
          {
             free(h);
             free(s);
             ecore_timer_add(0.1, elsa_session_login, NULL);
             return;
          }
     }
   if (h) free(h);
   if (s) free(s);
   elm_object_focus(ELSA_GUI_GET(_gui->edj, "password"));
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj), "elsa.auth.enable", "");
}

static void
_elsa_gui_shutdown(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elsa_gui_shutdown(NULL);
}

static void
_elsa_gui_password_activated_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   char *txt;

   txt = elsa_gui_password_get();
   if (!strcmp(txt, ""))
     {
        free(txt);
        return;
     }
   free(txt);
   ecore_timer_add(0.1, elsa_session_login, NULL);
}

static void
_elsa_gui_xsessions_clicked_cb(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   Evas_Object *icon;

   _gui->selected_session = data;
   elm_hoversel_label_set(obj, _gui->selected_session->name);
   icon = elm_icon_add(_gui->win);
   elm_icon_file_set(icon, _gui->selected_session->icon, NULL);
   elm_hoversel_icon_set(obj, icon);
   evas_object_show(icon);
}


static void
_elsa_gui_callback_add()
{
   Evas_Object *host, *pwd;

   host = ELSA_GUI_GET(_gui->edj, "hostname");
   pwd = ELSA_GUI_GET(_gui->edj, "password");
   evas_object_smart_callback_add(host, "activated",
                                  _elsa_gui_hostname_activated_cb, pwd);
   evas_object_smart_callback_add(pwd, "activated",
                                  _elsa_gui_password_activated_cb, host);
   edje_object_signal_callback_add(elm_layout_edje_get(_gui->edj),
                                   "elsa.auth.cancel", "",
                                   _elsa_gui_login_cancel_cb, NULL);
   edje_object_signal_callback_add(elm_layout_edje_get(_gui->edj),
                                   "elsa.auth.request", "",
                                   _elsa_gui_login_request_cb, NULL);
   elm_entry_single_line_set(host, EINA_TRUE);
   elm_entry_single_line_set(pwd, EINA_TRUE);
   elm_object_focus(host);
}

static void
_elsa_gui_scan_desktops_file(const char *path)
{
   Efreet_Desktop *desktop;
   Eina_List *commands;
   Eina_List *l;
   Elsa_Xsession *xsession;
   char *command;

   desktop = efreet_desktop_get(path);
   if (!desktop) return;
   EINA_LIST_FOREACH(_gui->xsessions, l, xsession)
      {
         if (!strcmp(xsession->name, desktop->name))
           {
              efreet_desktop_free(desktop);
              return;
           }
      }

   commands = efreet_desktop_command_local_get(desktop, NULL);
   if (commands)
     {
        char *tmp;
        command = eina_list_data_get(commands);
        commands = eina_list_next(commands);
        EINA_LIST_FREE(commands, tmp)
           free(tmp);
     }
   if (command && desktop->name)
     {
        xsession = calloc(1, sizeof(Elsa_Xsession));
        xsession->command = eina_stringshare_add(command);
        xsession->name = eina_stringshare_add(desktop->name);
        if (desktop->icon) xsession->icon = eina_stringshare_add(desktop->icon);
        _gui->xsessions = eina_list_append(_gui->xsessions, xsession);
        fprintf(stderr, PACKAGE": find sessions %s\n", desktop->name);
        free(command);
     }
   efreet_desktop_free(desktop);
}

static void
_elsa_gui_scan_desktops(const char *dir)
{
   Eina_List *files;
   char *filename;
   char path[PATH_MAX];

   fprintf(stderr, PACKAGE": scanning directory %s\n", dir);
   files = ecore_file_ls(dir);
   EINA_LIST_FREE(files, filename)
     {
        snprintf(path, sizeof(path), "%s/%s", dir, filename);
        _elsa_gui_scan_desktops_file(path);
        free(filename);
     }
}

static void
_elsa_gui_init_desktops()
{
   char buf[PATH_MAX];
   Eina_List *dirs;
   const char *path;

   efreet_init();
   efreet_desktop_type_alias(EFREET_DESKTOP_TYPE_APPLICATION, "XSession");
   /* Maybee need to scan other directories ?
    * _elsa_gui_scan_desktops("/usr/share/xsessions");
    */
   snprintf(buf, sizeof(buf), "%s/xsessions", efreet_data_home_get());
   _elsa_gui_scan_desktops(buf);
   dirs = efreet_data_dirs_get();
   EINA_LIST_FREE(dirs, path)
     {
        snprintf(buf, sizeof(buf), "%s/xsessions", path);
        _elsa_gui_scan_desktops(buf);
        eina_stringshare_del(path);
     }
   efreet_shutdown();
}

static void
_elsa_gui_sessions_populate()
{
   Evas_Object *o, *icon;
   Elsa_Xsession *xsession;
   Eina_List *l;

   o = ELSA_GUI_GET(_gui->edj, "xsessions");

   EINA_LIST_FOREACH(_gui->xsessions, l, xsession)
     {
        elm_hoversel_item_add(o, xsession->name, xsession->icon,
                              ELM_ICON_FILE,
                              _elsa_gui_xsessions_clicked_cb, xsession);
        if (elsa_config->last_session)
          {
            if (!strcmp(xsession->name ,elsa_config->last_session))
              _gui->selected_session = xsession;
          }

     }
   if (!_gui->selected_session) _gui->selected_session = _gui->xsessions->data;
   elm_hoversel_label_set(o, _gui->selected_session->name);
   icon = elm_icon_add(_gui->win);
   elm_icon_file_set(icon, _gui->selected_session->icon, NULL);
   elm_hoversel_icon_set(o, icon);
}

int
elsa_gui_init()
{

   Ecore_X_Window root;
   int w, h;
   fprintf(stderr, PACKAGE": Gui init\n");

   _gui = calloc(1, sizeof(Elsa_Gui));
   if (!_gui)
     {
        fprintf(stderr, "Not Enough memory\n");
        return 1;
     }

#ifdef XNEST_DEBUG
   char *tmp = getenv("DISPLAY");
   if (tmp && *tmp)
     {
        fprintf(stderr, PACKAGE": Using display name %s", tmp);
     }
#endif

   _gui->win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_title_set(_gui->win, PACKAGE);
   evas_object_smart_callback_add(_gui->win, "delete_request",
                                  _elsa_gui_shutdown, NULL);

   _gui->edj = _elsa_gui_theme_get(_gui->win, "elsa");
   if (!_gui->edj)
     {
        fprintf(stderr, "Tut Tut Tut no theme\n -> %s\n!!!!\n", PACKAGE);
        return 2;
     }


   evas_object_size_hint_weight_set(_gui->edj,
                                    EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   elm_win_resize_object_add(_gui->win, _gui->edj);

   /* have a fullscreen window */
   root = ecore_x_window_root_first_get();
   ecore_x_window_size_get(root, &w, &h);

   if (elsa_config->xsessions)
     {
        _elsa_gui_init_desktops();
     }
   if (_gui->xsessions)
     {
        edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                                "elsa.xsession.enabled", "");
        _elsa_gui_sessions_populate();
     }
   else
     edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                             "elsa.xsession.disabled", "");

   _elsa_gui_callback_add();

   evas_object_show(_gui->edj);
   evas_object_resize(_gui->win, w, h);
   evas_object_show(_gui->win);


   return 0;
}


void
elsa_gui_shutdown()
{
   Elsa_Xsession *xsession;
   fprintf(stderr, PACKAGE": Gui shutdown\n");
/*
#ifdef HAVE_PAM
   elsa_pam_shutdown();
#endif
   elm_exit();
*/
   evas_object_del(_gui->win);
   EINA_LIST_FREE(_gui->xsessions, xsession)
     {
        eina_stringshare_del(xsession->name);
        eina_stringshare_del(xsession->command);
        if (xsession->icon) eina_stringshare_del(xsession->icon);
     }
   if (_gui) free(_gui);
}

char *
elsa_gui_user_get()
{
   Evas_Object *o;
   o = ELSA_GUI_GET(_gui->edj, "hostname");
   if (o) return elm_entry_markup_to_utf8(elm_scrolled_entry_entry_get(o));
   return NULL;
}

char *
elsa_gui_password_get()
{
   Evas_Object *o;
   o = ELSA_GUI_GET(_gui->edj, "password");
   if (o) return elm_entry_markup_to_utf8(elm_scrolled_entry_entry_get(o));
   return NULL;
}

void
elsa_gui_auth_error()
{
   Evas_Object *o;

   o = ELSA_GUI_GET(_gui->edj, "password");
   elm_scrolled_entry_entry_set(o, "");
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                           "elsa.auth.error", "");
}

void
elsa_gui_auth_valid(void *data)
{
   edje_object_signal_callback_add(elm_layout_edje_get(_gui->edj),
                                   "elsa.auth.end", "",
                                   _elsa_gui_login_cb, data);
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                           "elsa.auth.valid", "");
}


const char *
elsa_gui_login_command_get()
{
   if (elsa_config->xsessions && _gui->selected_session)
     {
        elsa_config_last_session_set(_gui->selected_session->name);
        return strdup(_gui->selected_session->command);
     }
   return strdup(elsa_config->command.session_login);
}

