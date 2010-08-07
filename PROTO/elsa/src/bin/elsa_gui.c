#include "elsa.h"
#include "Ecore_X.h"

typedef struct Elsa_Gui_
{
   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *edj;
} Elsa_Gui;

static Evas_Object *_elsa_gui_theme_get(Evas_Object *win, const char *group);
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
_elsa_gui_shutdown(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elsa_gui_shutdown(NULL);
}

static void
_elsa_gui_login_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *emission __UNUSED__, const char *source __UNUSED__)
{
   struct passwd *pwd;
   char *hostname = NULL;
   char *password = NULL;
   char *term;
   char buf[4096];

   hostname = elsa_gui_user_get();
   password = elsa_gui_password_get();
   if (!hostname || !password) return;
   if (!strcmp(hostname, "") || !strcmp(password, ""))
     {
        fprintf(stderr, "%s: no login\n", PACKAGE);
        free(hostname);
        free(password);
        return;
     }
   free(hostname);
   free(password);
#ifdef HAVE_PAM
   int status;
   status = elsa_pam_authenticate();
   if (status)
     {
        elsa_gui_auth_error();
        return;
     }
   if (!elsa_pam_open_session())
     {
        pwd = getpwnam(elsa_pam_item_get(ELSA_PAM_ITEM_USER));
        endpwent();
        term = getenv("TERM");
        if (term) elsa_pam_env_set("TERM", term);
        elsa_pam_env_set("HOME", pwd->pw_dir);
        elsa_pam_env_set("SHELL", pwd->pw_shell);
        elsa_pam_env_set("USER", pwd->pw_name);
        elsa_pam_env_set("LOGNAME", pwd->pw_name);
        elsa_pam_env_set("PATH", "./:/bin:/usr/bin:/usr/local/bin");
        elsa_pam_env_set("DISPLAY", ":0.0");
        elsa_pam_env_set("MAIL", "");
        snprintf(buf, sizeof(buf), "%s/.Xauthority", pwd->pw_dir);
        elsa_pam_env_set("XAUTHORITY", buf);
        elsa_session_run(pwd);
     }
#endif
}


static void
_elsa_gui_callback_add()
{
   edje_object_signal_callback_add(elm_layout_edje_get(_gui->edj), "clicked", "login",
                                   _elsa_gui_login_cb, NULL);
}


int
elsa_gui_init()
{
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

   _elsa_gui_callback_add();

   evas_object_show(_gui->edj);
   evas_object_resize(_gui->win, 1024, 768);
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
   o = edje_object_part_external_object_get(elm_layout_edje_get(_gui->edj), "hostname");
   if (o) return elm_entry_markup_to_utf8(elm_scrolled_entry_entry_get(o));
   return NULL;
}

char *
elsa_gui_password_get()
{
   Evas_Object *o;
   o = edje_object_part_external_object_get(elm_layout_edje_get(_gui->edj), "password");
   if (o) return elm_entry_markup_to_utf8(elm_scrolled_entry_entry_get(o));
   return NULL;
}

void
elsa_gui_auth_error()
{
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj), "elsa.auth.error", "");
}

