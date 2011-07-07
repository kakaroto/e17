#include "elsa_client.h"
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

static Evas_Object *_elsa_gui_theme_get(Evas_Object *win, const char *group, const char *theme);
static void _elsa_gui_hostname_activated_cb(void *data, Evas_Object *obj, void *event_info);
static void _elsa_gui_password_activated_cb(void *data, Evas_Object *obj, void *event_info);
static void _elsa_gui_shutdown(void *data, Evas_Object *obj, void *event_info);

static Elsa_Gui *_gui;

static Evas_Object *
_elsa_gui_theme_get (Evas_Object *win, const char *group, const char *theme)
{
   Evas_Object *edje = NULL;
   char buf[PATH_MAX];

   edje = elm_layout_add(win);
   snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/themes/%s.edj", theme);
   if (theme)
     {
        snprintf(buf, sizeof(buf), PACKAGE_DATA_DIR"/themes/%s.edj", theme);
        if (!elm_layout_file_set(edje, buf, group))
          {
             printf("can't load %s theme fallback to default\n", theme);
             elm_layout_file_set(edje, PACKAGE_DATA_DIR"/themes/default.edj", group);
          }
     }
   else
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
   elsa_gui_shutdown();
}

static void
_elsa_gui_shutdown(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elsa_gui_shutdown();
}

static void
_elsa_gui_login_cancel_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *sig __UNUSED__, const char *src __UNUSED__)
{
   Evas_Object *o;

   o = ELSA_GUI_GET(_gui->edj, "hostname");
   elm_entry_entry_set(o, "");
   elm_object_focus(o);
   o = ELSA_GUI_GET(_gui->edj, "password");
   elm_entry_entry_set(o, "");
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                           "elsa.auth.disable", "");
}

static void
_elsa_gui_login()
{
   char *h, *s;
   h = elsa_gui_user_get();
   s = elsa_gui_password_get();
   if (h && s)
     {
        if (strcmp(h, "") && strcmp(s, ""))
          {
             if (_gui->selected_session)
                  elsa_connect_auth_send(h, s, _gui->selected_session->name);
             else
                  elsa_connect_auth_send(h, s, NULL);
          }
     }
   if (h) free(h);
   if (s) free(s);
   elm_object_focus(ELSA_GUI_GET(_gui->edj, "password"));
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj), "elsa.auth.enable", "");
}

static void
_elsa_gui_login_request_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *sig __UNUSED__, const char *src __UNUSED__)
{
   _elsa_gui_login();
}

static void
_elsa_gui_password_activated_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   _elsa_gui_login();
}

static void
_elsa_gui_xsessions_clicked_cb(void *data, Evas_Object *obj, void *event_info __UNUSED__)
{
   Evas_Object *icon;

   _gui->selected_session = data;
   elm_object_text_set(obj, _gui->selected_session->name);
   icon = elm_icon_add(_gui->win);
   elm_icon_file_set(icon, _gui->selected_session->icon, NULL);
   elm_hoversel_icon_set(obj, icon);
   evas_object_show(icon);
}


static void
_elsa_gui_callback_add()
{
   Evas_Object *host, *pwd;
   Evas_Object *edj;

   edj = elm_layout_edje_get(_gui->edj);

   host = ELSA_GUI_GET(_gui->edj, "hostname");
   pwd = ELSA_GUI_GET(_gui->edj, "password");
   evas_object_smart_callback_add(host, "activated",
                                  _elsa_gui_hostname_activated_cb, pwd);
   evas_object_smart_callback_add(pwd, "activated",
                                  _elsa_gui_password_activated_cb, host);
   edje_object_signal_callback_add(edj, "elsa.auth.cancel", "",
                                   _elsa_gui_login_cancel_cb, NULL);
   edje_object_signal_callback_add(edj, "elsa.auth.request", "",
                                   _elsa_gui_login_request_cb, NULL);
   edje_object_signal_callback_add(edj, "elsa.auth.end", "",
                                   _elsa_gui_login_cb, NULL);
   elm_entry_single_line_set(host, EINA_TRUE);
   elm_entry_single_line_set(pwd, EINA_TRUE);
   elm_object_focus(host);
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
/*
  if (elsa_config->last_session)
          {
            if (!strcmp(xsession->name ,elsa_config->last_session))
              _gui->selected_session = xsession;
          }
          */

     }
   if (!_gui->selected_session) _gui->selected_session = _gui->xsessions->data;
   elm_object_text_set(o, _gui->selected_session->name);
   icon = elm_icon_add(_gui->win);
   elm_icon_file_set(icon, _gui->selected_session->icon, NULL);
   elm_hoversel_icon_set(o, icon);
}

void
elsa_gui_xsession_set(Eina_List *xsessions)
{
   if (!xsessions) return;
   _gui->xsessions = xsessions;
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                           "elsa.xsession.enabled", "");
   _elsa_gui_sessions_populate();
}

int
elsa_gui_init(const char *theme)
{

   Ecore_X_Window root;
   int w, h;
   fprintf(stderr, PACKAGE": client Gui init\n");

   _gui = calloc(1, sizeof(Elsa_Gui));
   if (!_gui)
     {
        fprintf(stderr, PACKAGE": client Not Enough memory\n");
        return 1;
     }

#ifdef XNEST_DEBUG
   char *tmp = getenv("DISPLAY");
   if (tmp && *tmp)
     {
        fprintf(stderr, PACKAGE": client Using display name %s", tmp);
     }
#endif

   _gui->win = elm_win_add(NULL, "main", ELM_WIN_BASIC);
   elm_win_fullscreen_set(_gui->win, EINA_TRUE);
   elm_win_title_set(_gui->win, PACKAGE);
   evas_object_smart_callback_add(_gui->win, "delete,request",
                                  _elsa_gui_shutdown, NULL);

   _gui->edj = _elsa_gui_theme_get(_gui->win, "elsa", theme);
   if (!_gui->edj)
     {
        fprintf(stderr, PACKAGE": client Tut Tut Tut no theme\n");
        return 2;
     }


   evas_object_size_hint_weight_set(_gui->edj,
                                    EVAS_HINT_EXPAND,
                                    EVAS_HINT_EXPAND);
   elm_win_resize_object_add(_gui->win, _gui->edj);

   _elsa_gui_callback_add();

   /* Get root window and init pointer. Maybee add theme for this pointer */
   root = ecore_x_window_root_first_get();
   ecore_x_window_cursor_show(root, 0);
   ecore_x_window_cursor_set(root, ECORE_X_CURSOR_ARROW);
   ecore_x_window_cursor_show(root, 1);

   /* have a fullscreen window */
   ecore_x_window_size_get(root, &w, &h);
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
   evas_object_del(_gui->win);
   EINA_LIST_FREE(_gui->xsessions, xsession)
     {
        eina_stringshare_del(xsession->name);
        eina_stringshare_del(xsession->command);
        if (xsession->icon) eina_stringshare_del(xsession->icon);
     }
   if (_gui) free(_gui);
   elm_exit();
}

char *
elsa_gui_user_get()
{
   Evas_Object *o;
   o = ELSA_GUI_GET(_gui->edj, "hostname");
   if (o) return elm_entry_markup_to_utf8(elm_entry_entry_get(o));
   return NULL;
}

char *
elsa_gui_password_get()
{
   Evas_Object *o;
   o = ELSA_GUI_GET(_gui->edj, "password");
   if (o) return elm_entry_markup_to_utf8(elm_entry_entry_get(o));
   return NULL;
}

void
elsa_gui_auth_error()
{
   Evas_Object *o;

   o = ELSA_GUI_GET(_gui->edj, "password");
   elm_entry_entry_set(o, "");
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                           "elsa.auth.error", "");
}

void
elsa_gui_auth_valid()
{
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                           "elsa.auth.valid", "");
}

