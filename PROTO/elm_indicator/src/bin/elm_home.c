#include "elm_indicator.h"
#include "elm_home.h"

/* local function prototypes */
static void _apps_populate(void);
static void _apps_unpopulate(void);
static void _desktops_populate(void);
static Eina_Bool _desktops_list_change(void *data, int type, void *event);
static Eina_Bool _desktops_run_exit(void *data, int type, void *event);
static void _desktops_run(Efreet_Desktop *desktop);
static Eina_Bool _desktops_run_timeout(void *data);
static Eina_Bool _desktops_update_deferred(void *data);
static char *_cb_gl_label_get(const void *data, Evas_Object *obj, const char *part);
static Evas_Object *_cb_gl_icon_get(const void *data, Evas_Object *obj, const char *part);
static void _cb_gl_item_del(const void *data, Evas_Object *obj);
static void _cb_gl_item_select(void *data, Evas_Object *obj, void *event);

/* local variables */
static Eina_List *hwins = NULL;
static Eina_List *handlers = NULL;
static Eina_List *desktops = NULL;
static Eina_List *exes = NULL;
static Ecore_Timer *defer = NULL;
static Elm_Genlist_Item_Class it_desk;

EAPI int 
elm_home_init(void) 
{
#ifdef ELM_EFREET
   elm_need_efreet();

   _apps_unpopulate();
   _apps_populate();

   handlers = 
     eina_list_append(handlers, 
                      ecore_event_handler_add(EFREET_EVENT_DESKTOP_CACHE_UPDATE, 
                                              _desktops_list_change, NULL));
   handlers = 
     eina_list_append(handlers, 
                      ecore_event_handler_add(ECORE_EXE_EVENT_DEL, 
                                              _desktops_run_exit, NULL));
#endif

   return 1;
}

EAPI int 
elm_home_shutdown(void) 
{
   Elm_Home_Win *hwin;
   Ecore_Event_Handler *handler;
   Elm_Home_Exec *exec;

   EINA_LIST_FREE(exes, exec) 
     {
        if (exec->exe) 
          {
             ecore_exe_terminate(exec->exe);
             ecore_exe_free(exec->exe);
             exec->exe = NULL;
          }
        if (exec->timer) ecore_timer_del(exec->timer);
        exec->timer = NULL;
        free(exec);
     }

   EINA_LIST_FREE(hwins, hwin)
     {
        if (hwin->win) evas_object_del(hwin->win);
        free(hwin);
     }

   _apps_unpopulate();

   EINA_LIST_FREE(handlers, handler)
     ecore_event_handler_del(handler);

   return 1;
}

EAPI void 
elm_home_win_new(Ecore_X_Window zone) 
{
   Elm_Home_Win *hwin;
   Evas_Object *bg;
   int x, y, w, h;

   hwin = calloc(1, sizeof(Elm_Home_Win));
   if (!hwin) return;

   hwin->win = elm_win_add(NULL, "Illume-Home", ELM_WIN_BASIC);
   elm_win_title_set(hwin->win, "Illume Home");

   bg = elm_bg_add(hwin->win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(hwin->win, bg);
   evas_object_show(bg);

   it_desk.item_style = "default";
   it_desk.func.label_get = _cb_gl_label_get;
   it_desk.func.content_get = _cb_gl_icon_get;
   it_desk.func.state_get = NULL;
   it_desk.func.del = _cb_gl_item_del;

   hwin->gl = elm_genlist_add(hwin->win);
   evas_object_size_hint_weight_set(hwin->gl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(hwin->win, hwin->gl);
   evas_object_show(hwin->gl);

#ifdef ELM_EFREET
   if (desktops) 
     {
        Efreet_Desktop *d;
        Eina_List *l = NULL;

        EINA_LIST_FOREACH(desktops, l, d)
          elm_genlist_item_append(hwin->gl, &it_desk, d, NULL, 
                                  ELM_GENLIST_ITEM_NONE, 
                                  _cb_gl_item_select, d);
     }
#endif

   ecore_x_window_geometry_get(zone, &x, &y, &w, &h);
   evas_object_move(hwin->win, x, y);
   evas_object_resize(hwin->win, w, h);
   evas_object_show(hwin->win);

   hwins = eina_list_append(hwins, hwin);
}

/* local functions */
static void 
_apps_populate(void) 
{
#ifdef ELM_EFREET
   _desktops_populate();
#endif
}

static void 
_apps_unpopulate(void) 
{
#ifdef ELM_EFREET
   Efreet_Desktop *desktop;
   EINA_LIST_FREE(desktops, desktop)
     efreet_desktop_free(desktop);
   desktops = NULL;
#endif
}

static void 
_desktops_populate(void) 
{
#ifdef ELM_EFREET
   desktops = efreet_util_desktop_name_glob_list("*");
   if (desktops) 
     {
        Elm_Home_Win *hwin;
        Efreet_Desktop *d;
        Eina_List *l = NULL, *ll = NULL;

        EINA_LIST_FOREACH(hwins, l, hwin) 
          {
             if (!hwin->gl) continue;
             elm_genlist_clear(hwin->gl);
             EINA_LIST_FOREACH(desktops, ll, d)
               elm_genlist_item_append(hwin->gl, &it_desk, d, NULL, 
                                       ELM_GENLIST_ITEM_NONE, 
                                       _cb_gl_item_select, d);
          }
     }
#endif
}

static Eina_Bool 
_desktops_list_change(void *data, int type, void *event) 
{
   if (defer) ecore_timer_del(defer);
   defer = ecore_timer_add(1.0, _desktops_update_deferred, NULL);
   return EINA_TRUE;
}

static Eina_Bool 
_desktops_run_exit(void *data, int type, void *event) 
{
   Elm_Home_Exec *exec;
   Eina_List *l;
   Ecore_Exe_Event_Del *ev;

   ev = event;
   EINA_LIST_FOREACH(exes, l, exec) 
     {
        if (exec->pid == ev->pid) 
          {
             exes = eina_list_remove_list(exes, l);
             if (exec->timer) ecore_timer_del(exec->timer);
             exec->timer = NULL;
             exec->exe = NULL;
             free(exec);
             return EINA_TRUE;
          }
     }
   return EINA_TRUE;
}

static void 
_desktops_run(Efreet_Desktop *desktop) 
{
   Elm_Home_Exec *exec;
   Eina_List *l;

   if ((!desktop) || (!desktop->exec)) return;
   EINA_LIST_FOREACH(exes, l, exec) 
     {
        if (exec->desktop == desktop) 
          {
             if (exec->exe) return;
          }
     }

   exec = calloc(1, sizeof(Elm_Home_Exec));
   if (!exec) return;

   exec->exe = ecore_exe_run(desktop->exec, NULL);
   exec->desktop = desktop;
   if (exec->exe)
     exec->pid = ecore_exe_pid_get(exec->exe);
   exec->timer = ecore_timer_add(2.0, _desktops_run_timeout, exec);
   exes = eina_list_append(exes, exec);
}

static Eina_Bool 
_desktops_run_timeout(void *data) 
{
   Elm_Home_Exec *exec;

   if (!(exec = data)) return EINA_TRUE;
   if (!exec->exe) 
     {
        exes = eina_list_remove(exes, exec);
        exec->timer = NULL;
        free(exec);
        return EINA_FALSE;
     }
   exec->timer = NULL;
   return EINA_FALSE;
}

static Eina_Bool 
_desktops_update_deferred(void *data) 
{
   _apps_unpopulate();
   _apps_populate();
   defer = NULL;
   return EINA_FALSE;
}

static char *
_cb_gl_label_get(const void *data, Evas_Object *obj, const char *part) 
{
#ifdef ELM_EFREET
   if (!data) return NULL;
   Efreet_Desktop *d = (Efreet_Desktop *)data;
   if ((!d) || (!d->name)) return NULL;
   return strdup(d->name);
#else
   return NULL;
#endif
}

static Evas_Object *
_cb_gl_icon_get(const void *data, Evas_Object *obj, const char *part) 
{
#ifdef ELM_EFREET
   if (!data) return NULL;

   Efreet_Desktop *d = (Efreet_Desktop *)data;
   const char *path;
   Evas_Object *ic;

   if (!d) return NULL;
   ic = elm_icon_add(obj);
   evas_object_size_hint_aspect_set(ic, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   if (!(!strcmp(part, "elm.swallow.icon"))) return ic;
   if (!d->icon) return ic;
   path = efreet_icon_path_find("default", d->icon, 48);
   if (!path) 
     {
        path = efreet_icon_path_find("hicolor", d->icon, 48);
        if (!path) 
          {
             path = efreet_icon_path_find("gnome", d->icon, 48);
             if (!path)
               path = efreet_icon_path_find("Human", d->icon, 48);
          }
     }
   if (path) 
     {
        elm_icon_file_set(ic, path, NULL);
        return ic;
     }
   return ic;
#else
   return NULL;
#endif
}

static void 
_cb_gl_item_del(const void *data, Evas_Object *obj) 
{

}

static void 
_cb_gl_item_select(void *data, Evas_Object *obj, void *event) 
{
#ifdef ELM_EFREET
   if (!data) return;
   Efreet_Desktop *d = (Efreet_Desktop *)data;
   if ((!d) || (!d->name)) return;
   if (d->exec) _desktops_run(d);
#endif
}
