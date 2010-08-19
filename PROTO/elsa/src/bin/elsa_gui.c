#include "elsa.h"
#include "Ecore_X.h"

#define ELSA_GUI_GET(edj, name) edje_object_part_external_object_get(elm_layout_edje_get(edj), name)

typedef struct Elsa_Gui_
{
   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *edj;
} Elsa_Gui;

static Evas_Object *_elsa_gui_theme_get(Evas_Object *win, const char *group);
static void _elsa_gui_hostname_activated_cb(void *data, Evas_Object *obj, void *event_info);
static void _elsa_gui_password_activated_cb(void *data, Evas_Object *obj, void *event_info);
static void _elsa_gui_shutdown(void *data, Evas_Object *obj, void *event_info);

static Elsa_Gui *_gui;

static Evas_Object *
_elsa_gui_theme_get (Evas_Object *win, const char *group)
{
   char buffer[PATH_MAX];
   Evas_Object *edje = NULL;

   edje = elm_layout_add(win);
   snprintf(buffer, sizeof(buffer), "%s/themes/default.edj", PACKAGE_DATA_DIR);
   elm_layout_file_set(edje, buffer, group);

   return edje;
}

static void
_elsa_gui_hostname_activated_cb(void *data, Evas_Object *obj, void *event_info)
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
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj), "elsa.auth.enable", "");
}

static void
_elsa_gui_login_cb(void *data, Evas_Object *obj, const char *sig, const char *src)
{
   elsa_session_run(data);
}

static void
_elsa_gui_login_cancel_cb(void *data, Evas_Object *obj, const char *sig, const char *src)
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
_elsa_gui_login_request_cb(void *data, Evas_Object *obj, const char *sig, const char *src)
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
_elsa_gui_password_activated_cb(void *data, Evas_Object *obj, void *event_info)
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
   //elm_win_fullscreen_set(_gui->win, EINA_TRUE);
   root = ecore_x_window_root_first_get();
   ecore_x_window_size_get(root, &w, &h);

   _elsa_gui_callback_add();

   evas_object_show(_gui->edj);
   evas_object_resize(_gui->win, w, h);
   evas_object_show(_gui->win);


   return 0;
}


void
elsa_gui_shutdown()
{
   fprintf(stderr, PACKAGE": Gui shutdown\n");
/*
#ifdef HAVE_PAM
   elsa_pam_shutdown();
#endif
   elm_exit();
*/
   evas_object_del(_gui->win);
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

