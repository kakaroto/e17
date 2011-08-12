#include "elsa_client.h"
#include "Ecore_X.h"

#define ELM_INTERNAL_API_ARGESFSDFEFC
#include <elm_widget.h>


#define ELSA_GUI_GET(edj, name) edje_object_part_external_object_get(elm_layout_edje_get(edj), name)

typedef struct Elsa_Gui_ Elsa_Gui;
typedef struct Elsa_Gui_Item_ Elsa_Gui_Item;

struct Elsa_Gui_
{
   Evas_Object *win;
   Evas_Object *bg;
   Evas_Object *edj;
   Eina_List *xsessions;
   Eina_List *users;
   Eina_List *actions;
   Elsa_Xsession *selected_session;
   const char *theme;
};

typedef char *(*ElsaItemLabelGetFunc) (void *data, Evas_Object *obj, const char *part);
typedef Evas_Object *(*ElsaItemIconGetFunc) (void *data, Evas_Object *obj, const char *part);
typedef Eina_Bool (*ElsaItemStateGetFunc) (void *data, Evas_Object *obj, const char *part);
typedef void (*ElsaItemDelFunc) (void *data, Evas_Object *obj);

struct Elsa_Gui_Item_
{
   const char *item_style; //maybee need to be provided by theme ?
   struct
     {
        ElsaItemLabelGetFunc label_get;
        ElsaItemIconGetFunc  icon_get;
        ElsaItemStateGetFunc state_get;
        ElsaItemDelFunc      del;
     } func;
};

static Evas_Object *_elsa_gui_theme_get(Evas_Object *win, const char *group, const char *theme);
static void _elsa_gui_hostname_activated_cb(void *data, Evas_Object *obj, void *event_info);
static void _elsa_gui_password_activated_cb(void *data, Evas_Object *obj, void *event_info);
static void _elsa_gui_shutdown(void *data, Evas_Object *obj, void *event_info);
static void _elsa_gui_session_update(Elsa_Xsession *xsession);
static void _elsa_gui_users_list_set(Evas_Object *obj, Eina_List *users);
static void _elsa_gui_users_genlist_set(Evas_Object *obj, Eina_List *users);
static void _elsa_gui_users_gengrid_set(Evas_Object *obj, Eina_List *users);
static void _elsa_gui_user_sel_cb(void *data, Evas_Object *obj, void *event_info);
static void _elsa_gui_user_sel(Elsa_User *ou);

static void _elsa_gui_action_clicked_cb(void *data, Evas_Object *obj, void *event_info);
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
             fprintf(stderr, PACKAGE": can't load %s theme fallback to default\n", theme);
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
   Eina_List *l, *ll;
   Elsa_Xsession *xsess;
   Elsa_User *eu = NULL;

   txt = elsa_gui_user_get();
   if (!strcmp(txt, ""))
     {
        free(txt);
        return;
     }
   EINA_LIST_FOREACH(_gui->users, ll, eu)
      if (!strcmp(txt, eu->login)) break;
   free(txt);

   if (eu && eu->lsess)
     {
        EINA_LIST_FOREACH(_gui->xsessions, l, xsess)
          {
             if (!strcmp(xsess->name, eu->lsess))
               {
                  _elsa_gui_session_update(xsess);
                  break;
               }
          }
     }
   else if (_gui->xsessions)
     _elsa_gui_session_update(_gui->xsessions->data);
   elm_object_focus_set(data, EINA_TRUE);
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                           "elsa.auth.enable", "");
}

static void
_elsa_gui_login_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *sig __UNUSED__, const char *src __UNUSED__)
{
   elm_exit();
}

static void
_elsa_gui_shutdown(void *data __UNUSED__, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   elm_exit();
}

static void
_elsa_gui_login_cancel_cb(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *sig __UNUSED__, const char *src __UNUSED__)
{
   Evas_Object *o;

   o = ELSA_GUI_GET(_gui->edj, "hostname");
   elm_entry_entry_set(o, "");
   elm_object_focus_set(o, EINA_TRUE);
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
   elm_object_focus_set(ELSA_GUI_GET(_gui->edj, "password"), EINA_TRUE);
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
_elsa_gui_action_clicked_cb(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   Elsa_Action *ea;
   ea = data;
   if (ea) elsa_connect_action_send(ea->id);
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
   elm_object_focus_set(host, EINA_TRUE);
}

static void
_elsa_gui_session_update(Elsa_Xsession *xsession)
{
   Evas_Object *o, *icon;

   if (!xsession) return;
   o = ELSA_GUI_GET(_gui->edj, "xsessions");
   _gui->selected_session = xsession;
   elm_object_text_set(o, _gui->selected_session->name);
   icon = elm_icon_add(_gui->win);
   elm_icon_file_set(icon, _gui->selected_session->icon, NULL);
   elm_hoversel_icon_set(o, icon);
}

static void
_elsa_gui_sessions_populate()
{
   Evas_Object *o;

   Elsa_Xsession *xsession;
   Eina_List *l;

   o = ELSA_GUI_GET(_gui->edj, "xsessions");

   EINA_LIST_FOREACH(_gui->xsessions, l, xsession)
     {
        elm_hoversel_item_add(o, xsession->name, xsession->icon,
                              ELM_ICON_FILE,
                              _elsa_gui_xsessions_clicked_cb, xsession);
     }
   if (_gui->xsessions)
     _elsa_gui_session_update(_gui->xsessions->data);
}

void
elsa_gui_xsession_set(Eina_List *xsessions)
{
   if (!xsessions) return;
   _gui->xsessions = xsessions;
   _elsa_gui_sessions_populate();
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                           "elsa.xsession.enabled", "");
}

static void
_elsa_gui_actions_populate()
{
   Evas_Object *o;

   Elsa_Action *action;
   Eina_List *l;

   o = ELSA_GUI_GET(_gui->edj, "actions");

   EINA_LIST_FOREACH(_gui->actions, l, action)
     {
        elm_hoversel_item_add(o, action->label, NULL,
                              ELM_ICON_FILE,
                              _elsa_gui_action_clicked_cb, action);
     }
}

void
elsa_gui_actions_set(Eina_List *actions)
{
   if (!actions) return;
   fprintf(stderr, PACKAGE": Action set\n");
   _gui->actions = actions;
   _elsa_gui_actions_populate();
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                           "elsa.action.enabled", "");
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

   _gui->theme = eina_stringshare_add(theme);
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
   _gui->win = NULL;
   eina_stringshare_del(_gui->theme);
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
///////////////// USER ////////////////////////////
void
elsa_gui_users_set(Eina_List *users)
{
   Evas_Object *ol;
   const char *type;

   ol = ELSA_GUI_GET(_gui->edj, "elsa_users");
   if ((ol) && ((type = elm_widget_type_get(ol))))
     {
        if (!strcmp(type, "list"))
          _elsa_gui_users_list_set(ol, users);
        else if (!strcmp(type, "genlist"))
          _elsa_gui_users_genlist_set(ol, users);
        else if (!strcmp(type, "gengrid"))
          _elsa_gui_users_gengrid_set(ol, users);

        edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                                "elsa.users.enabled", "");
        _gui->users = users;
     }
}



static void
_elsa_gui_user_sel(Elsa_User *eu)
{
   Evas_Object *o;
   Elsa_Xsession *xsess;
   Eina_List *l;

   o = ELSA_GUI_GET(_gui->edj, "hostname");
   elm_entry_entry_set(o, eu->login);
   elm_object_focus_set(ELSA_GUI_GET(_gui->edj, "password"), EINA_TRUE);
   edje_object_signal_emit(elm_layout_edje_get(_gui->edj),
                           "elsa.auth.enable", "");
   if (eu->lsess)
     {
        EINA_LIST_FOREACH(_gui->xsessions, l, xsess)
          {
             if (!strcmp(xsess->name, eu->lsess))
               {
                  _elsa_gui_session_update(xsess);
                  break;
               }
          }
     }
   else if (_gui->xsessions)
     _elsa_gui_session_update(_gui->xsessions->data);
}

static void
_elsa_gui_user_sel_cb(void *data, Evas_Object *obj __UNUSED__, void *event_info __UNUSED__)
{
   _elsa_gui_user_sel(data);
}

static char *
_elsa_gui_user_label_get(void *data, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   Elsa_User *eu;
   eu = data;
   return strdup(eu->login);
}

static Evas_Object *
_elsa_gui_user_icon_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part)
{
   Evas_Object *ic = NULL;
   Elsa_User *eu;
   eu = data;

   if (eu && !strcmp(part, "elm.swallow.icon"))
     {
        if (eu->image)
          {
             ic = elm_icon_add(_gui->win);
             elm_icon_file_set(ic, eu->image, "elsa/user/icon");
          }
        else
          {
             ic = _elsa_gui_theme_get(_gui->win, "elsa/user/default",
                                      _gui->theme);
          }
        evas_object_size_hint_weight_set(ic, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
        evas_object_show(ic);
     }
   return ic;
}

static Eina_Bool
_elsa_gui_user_state_get(void *data __UNUSED__, Evas_Object *obj __UNUSED__, const char *part __UNUSED__)
{
   return EINA_FALSE;
}

static void
_elsa_gui_user_del(void *data __UNUSED__, Evas_Object *obj __UNUSED__)
{
}

///////////////// LIST ///////////////////////////////
static void
_elsa_gui_users_list_set(Evas_Object *obj, Eina_List *users)
{
   Elsa_User *eu;
   Eina_List *l;

   EINA_LIST_FOREACH(users, l, eu)
      elm_list_item_append(obj, eu->login, NULL, NULL,
                           _elsa_gui_user_sel_cb, eu);
   elm_list_go(obj);
}

///////////////// USER GENLIST /////////////////////////////
static Elm_Genlist_Item_Class _elsa_glc;
static void
_elsa_gui_users_genlist_set(Evas_Object *obj, Eina_List *users)
{
   Elsa_User *eu;
   Eina_List *l;

   _elsa_glc.item_style = "default";
   _elsa_glc.func.label_get = _elsa_gui_user_label_get;
   _elsa_glc.func.icon_get = _elsa_gui_user_icon_get;
   _elsa_glc.func.state_get = _elsa_gui_user_state_get;
   _elsa_glc.func.del = _elsa_gui_user_del;


   EINA_LIST_FOREACH(users, l, eu)
      elm_genlist_item_append(obj, &_elsa_glc,
                              eu, NULL, ELM_GENLIST_ITEM_NONE,
                              _elsa_gui_user_sel_cb, eu);
}

///////////////// USER GENGRID /////////////////////////////
static Elm_Gengrid_Item_Class _elsa_ggc;
static void
_elsa_gui_users_gengrid_set(Evas_Object *obj, Eina_List *users)
{
   Elsa_User *eu;
   Eina_List *l;

   _elsa_ggc.item_style = "default";
   _elsa_ggc.func.label_get = _elsa_gui_user_label_get;
   _elsa_ggc.func.icon_get = _elsa_gui_user_icon_get;
   _elsa_ggc.func.state_get = _elsa_gui_user_state_get;
   _elsa_ggc.func.del = _elsa_gui_user_del;

   evas_object_size_hint_weight_set(obj, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   EINA_LIST_FOREACH(users, l, eu)
      elm_gengrid_item_append(obj, &_elsa_ggc,
                              eu, _elsa_gui_user_sel_cb, eu);
   evas_object_show(obj);

}



////////////////////////////////////////////////////////////////////////////////
///////////////// LIST ///////////////////////////////
static void
_elsa_gui_list_fill(Evas_Object *obj, Elsa_Gui_Item egi, Eina_List *users, Evas_Smart_Cb func, void *data)
{
   Elsa_User *eu;
   Eina_List *l;

   EINA_LIST_FOREACH(users, l, eu)
      elm_list_item_append(obj, egi.func.label_get(eu, NULL, NULL), NULL, NULL,
                           func, data);
   elm_list_go(obj);
}

///////////////// GENLIST /////////////////////////////
static void
_elsa_gui_genlist_fill(Evas_Object *obj, Elsa_Gui_Item egi, Eina_List *users, Evas_Smart_Cb func, void *data)
{
   Elsa_User *eu;
   Eina_List *l;
   Elm_Genlist_Item_Class glc;

   glc.item_style = egi.item_style;
   glc.func.label_get = egi.func.label_get;
   glc.func.icon_get = egi.func.icon_get;
   glc.func.state_get = egi.func.state_get;
   glc.func.del = egi.func.del;


   EINA_LIST_FOREACH(users, l, eu)
      elm_genlist_item_append(obj, &glc,
                              eu, NULL, ELM_GENLIST_ITEM_NONE,
                              func, data);
}

///////////////// GENGRID /////////////////////////////
static void
_elsa_gui_gengrid_fill(Evas_Object *obj, Elsa_Gui_Item egi, Eina_List *users, Evas_Smart_Cb func, void *data)
{
   Elsa_User *eu;
   Eina_List *l;
   Elm_Gengrid_Item_Class ggc;

   ggc.item_style = egi.item_style;
   ggc.func.label_get = egi.func.label_get;
   ggc.func.icon_get = egi.func.icon_get;
   ggc.func.state_get = egi.func.state_get;
   ggc.func.del = egi.func.del;

   EINA_LIST_FOREACH(users, l, eu)
      elm_gengrid_item_append(obj, &ggc,
                              eu, func, data);
}

static void
_elsa_gui_cont_fill(Evas_Object *obj, Elsa_Gui_Item egi, Eina_List *users, Evas_Smart_Cb func, void *data)
{
   const char *type;
   if ((type = elm_widget_type_get(obj)))
     {
        if (!strcmp(type, "list"))
          _elsa_gui_list_fill(obj, egi, users, func, data);
        else if (!strcmp(type, "genlist"))
          _elsa_gui_genlist_fill(obj, egi, users, func, data);
        else if (!strcmp(type, "gengrid"))
          _elsa_gui_gengrid_fill(obj, egi, users, func, data);
     }
}

