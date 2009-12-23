#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_X_Atoms.h>
#include "elm_indicator.h"

#ifndef ELM_LIB_QUICKLAUNCH

typedef struct _Ind_Home_Win Ind_Home_Win;
typedef struct _Ind_Home_Exec Ind_Home_Exec;
struct _Ind_Home_Win 
{
   Evas_Object *win, *gl;
};
struct _Ind_Home_Exec 
{
   Efreet_Desktop *desktop;
   Ecore_Exe *exe;
   Ecore_Timer *timeout;
   pid_t pid;
};

/* local function prototypes */
static void _cb_win_del(void *data, Evas_Object *obj, void *event);
static void _cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _cb_mouse_move(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _cb_mouse_up(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _cb_btn_home_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_btn_dual_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_btn_kbd_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_home_win_del(void *data, Evas_Object *obj, void *event);
static int _cb_window_property_change(void *data, int type, void *event);
static char *_desk_gl_label_get(const void *data, Evas_Object *obj, const char *part);
static Evas_Object *_desk_gl_icon_get(const void *data, Evas_Object *obj, const char *part);
static void _desk_gl_del(const void *data, Evas_Object *obj);
static void _desk_gl_select(void *data, Evas_Object *obj, void *event);

static int _desktop_list_change(void *data, int type, void *event);
static int _desktop_update_deferred(void *data);
static void _desktop_run(Efreet_Desktop *desktop);
static int _desktop_run_timeout(void *data);
static int _desktop_run_exit(void *data, int type, void *event);

static void _apps_populate(void);
static void _apps_unpopulate(void);
static void _desktops_populate(void);

static void _toggle_kbd_icon(Ecore_X_Window xwin, Evas_Object *obj);

static Evas_Object *win = NULL;
static Eina_List *hwins = NULL;
static Eina_List *handlers = NULL;
static Eina_List *desktops = NULL;
static Eina_List *exes = NULL;
static Ecore_Timer *defer = NULL;
static Elm_Genlist_Item_Class it_desk;

EAPI int 
elm_main(int argc, char **argv) 
{
   Evas_Object *bg, *box, *btn, *icon;
   Evas_Object *clock, *rect;
   Ecore_X_Window xwin;
   char buff[PATH_MAX];

# ifdef ELM_EFREET
   elm_need_efreet();

   _apps_unpopulate();
   _apps_populate();

   handlers = 
     eina_list_append(handlers, 
                      ecore_event_handler_add(EFREET_EVENT_DESKTOP_LIST_CHANGE, 
                                              _desktop_list_change, NULL));
   handlers = 
     eina_list_append(handlers, 
                      ecore_event_handler_add(EFREET_EVENT_DESKTOP_CHANGE, 
                                              _desktop_list_change, NULL));

   handlers = 
     eina_list_append(handlers, 
                      ecore_event_handler_add(ECORE_EXE_EVENT_DEL, 
                                              _desktop_run_exit, NULL));
# endif

   win = elm_win_add(NULL, "elm_indicator", ELM_WIN_DOCK);
   elm_win_title_set(win, "Illume Indicator Window");
   evas_object_smart_callback_add(win, "delete-request", _cb_win_del, NULL);
   xwin = elm_win_xwindow_get(win);
   ecore_x_icccm_hints_set(xwin, 0, 0, 0, 0, 0, 0, 0);

   bg = elm_bg_add(win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, bg);
   evas_object_show(bg);

   box = elm_box_add(win);
   elm_box_horizontal_set(box, EINA_TRUE);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(win, box);
   evas_object_show(box);

   icon = elm_icon_add(win);
   snprintf(buff, sizeof(buff), "%s/images/home.png", PACKAGE_DATA_DIR);
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   btn = elm_button_add(win);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_home_clicked, NULL);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(box, btn);
   evas_object_show(btn);
   evas_object_show(icon);

   icon = elm_icon_add(win);
   snprintf(buff, sizeof(buff), "%s/images/dual-mode.png", PACKAGE_DATA_DIR);
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   btn = elm_button_add(win);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_dual_clicked, NULL);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(box, btn);
   evas_object_show(btn);
   evas_object_show(icon);

   icon = elm_icon_add(win);
   snprintf(buff, sizeof(buff), "%s/images/kbd-off.png", PACKAGE_DATA_DIR);
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);

   btn = elm_button_add(win);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_kbd_clicked, NULL);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.5);
   elm_box_pack_end(box, btn);
   evas_object_show(btn);
   evas_object_show(icon);

   clock = elm_clock_add(win);
   elm_clock_show_seconds_set(clock, 0);
   elm_clock_show_am_pm_set(clock, 1);
   elm_object_scale_set(clock, 0.5);
   elm_box_pack_end(box, clock);
   evas_object_show(clock);

   rect = evas_object_rectangle_add(evas_object_evas_get(win));
   evas_object_color_set(rect, 0, 0, 0, 0);
   elm_win_resize_object_add(win, rect);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_DOWN, 
                                  _cb_mouse_down, win);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_MOVE, 
                                  _cb_mouse_move, win);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_UP, 
                                  _cb_mouse_up, win);
   evas_object_repeat_events_set(rect, 1);
   evas_object_raise(rect);
   evas_object_show(rect);

   evas_object_show(win);

   handlers = 
     eina_list_append(handlers, 
                      ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY, 
                                              _cb_window_property_change, btn));

   elm_run();
   elm_shutdown();
   return 0;
}

static void 
_cb_win_del(void *data, Evas_Object *obj, void *event) 
{
   Ind_Home_Win *hwin;
   Ind_Home_Exec *exec;
   Ecore_Event_Handler *handle;

   EINA_LIST_FREE(exes, exec) 
     {
        if (exec->timeout) ecore_timer_del(exec->timeout);
        free(exec);
     }

   EINA_LIST_FREE(hwins, hwin) 
     {
        if (hwin->win) evas_object_del(hwin->win);
        free(hwin);
     }

   _apps_unpopulate();

   EINA_LIST_FREE(handlers, handle)
     ecore_event_handler_del(handle);

   elm_exit();
}

static void 
_cb_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Evas_Event_Mouse_Down *ev;
   Ecore_X_Window xwin;
   Evas_Object *win;

   if (!(win = data)) return;
   ev = event;
   xwin = elm_win_xwindow_get(win);
   if (ev->button == 1) 
     {
        if (ecore_x_e_illume_drag_locked_get(xwin)) return;
        ecore_x_e_illume_drag_start_send(xwin);
     }
}

static void 
_cb_mouse_move(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Ecore_X_Window xwin;
   Evas_Event_Mouse_Move *ev;
   Evas_Object *win;
   int x, y;

   if (!(win = data)) return;
   ev = event;
   xwin = elm_win_xwindow_get(win);
   if (ecore_x_e_illume_drag_locked_get(xwin)) return;
   if (!ecore_x_e_illume_drag_get(xwin)) return;
   evas_object_geometry_get(win, &x, &y, NULL, NULL);
   evas_object_move(win, x, (y + ev->cur.output.y));
}

static void 
_cb_mouse_up(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Ecore_X_Window xwin;
   Evas_Event_Mouse_Up *ev;
   Evas_Object *win;

   ev = event;
   if (ev->button != 1) return;
   if (!(win = data)) return;
   xwin = elm_win_xwindow_get(win);
   if (ecore_x_e_illume_drag_locked_get(xwin)) return;
   if (!ecore_x_e_illume_drag_get(xwin)) return;
   ecore_x_e_illume_drag_end_send(xwin);
}

static void 
_cb_btn_home_clicked(void *data, Evas_Object *obj, void *event) 
{
   Ind_Home_Win *hwin;
   Evas_Object *bg, *gl;

   hwin = calloc(1, sizeof(Ind_Home_Win));
   if (!hwin) return;

   hwin->win = elm_win_add(NULL, "elm_indicator_home", ELM_WIN_BASIC);
   elm_win_title_set(hwin->win, "Illume Indicator Home Window");
   evas_object_smart_callback_add(hwin->win, "delete-request", 
                                  _cb_home_win_del, hwin);

   bg = elm_bg_add(hwin->win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(hwin->win, bg);
   evas_object_show(bg);

   it_desk.item_style = "default";
   it_desk.func.label_get = _desk_gl_label_get;
   it_desk.func.icon_get = _desk_gl_icon_get;
   it_desk.func.state_get = NULL;
   it_desk.func.del = _desk_gl_del;

   hwin->gl = elm_genlist_add(hwin->win);
   evas_object_size_hint_weight_set(hwin->gl, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(hwin->win, hwin->gl);
   evas_object_show(hwin->gl);

# ifdef ELM_EFREET
   if (desktops) 
     {
        Efreet_Desktop *d;
        Eina_List *l = NULL;

        EINA_LIST_FOREACH(desktops, l, d) 
          elm_genlist_item_append(hwin->gl, &it_desk, d, NULL, 
                                  ELM_GENLIST_ITEM_NONE, 
                                  _desk_gl_select, d);
     }
# endif

   evas_object_resize(hwin->win, 200, 200);
   evas_object_show(hwin->win);

   hwins = eina_list_append(hwins, hwin);
}

static void 
_cb_btn_dual_clicked(void *data, Evas_Object *obj, void *event) 
{
   Ecore_X_Window xwin;
   Ecore_X_Illume_Mode mode;

   xwin = ecore_x_window_root_first_get();
   mode = ecore_x_e_illume_mode_get(xwin);

   /* we do a mode set on the root xwindow so that illume module can read 
    * the current setting and adjust accordingly */
   if (mode <= ECORE_X_ILLUME_MODE_SINGLE) 
     mode = ECORE_X_ILLUME_MODE_DUAL;
   else 
     mode = ECORE_X_ILLUME_MODE_SINGLE;
   ecore_x_e_illume_mode_set(xwin, mode);
   ecore_x_e_illume_mode_send(xwin, mode);
}

static void 
_cb_btn_kbd_clicked(void *data, Evas_Object *obj, void *event) 
{
   Ecore_X_Window xwin;
   Ecore_X_Virtual_Keyboard_State state;

   xwin = ecore_x_window_focus_get();
   if (!xwin) return;

   state = ecore_x_e_virtual_keyboard_state_get(xwin);
   if ((state == ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF) || 
       (state == ECORE_X_VIRTUAL_KEYBOARD_STATE_UNKNOWN)) 
     ecore_x_e_virtual_keyboard_state_set(xwin, 
                                          ECORE_X_VIRTUAL_KEYBOARD_STATE_ON);
   else 
     ecore_x_e_virtual_keyboard_state_set(xwin, 
                                          ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF);

   _toggle_kbd_icon(xwin, obj);
}

static void 
_cb_home_win_del(void *data, Evas_Object *obj, void *event) 
{
   Ind_Home_Win *hwin, *win;
   Eina_List *l;

   if (!(hwin = data)) return;
   EINA_LIST_FOREACH(hwins, l, win) 
     {
        if (hwin->win == win->win) 
          {
             hwins = eina_list_remove_list(hwins, l);
             if (win->gl) evas_object_del(win->gl);
             if (win->win) evas_object_del(win->win);
             free(win);
             break;
          }
     }
}

static int 
_cb_window_property_change(void *data, int type, void *event) 
{
   Evas_Object *btn;
   Ecore_X_Event_Window_Property *ev;
   Ecore_X_Window xwin = 0;

   ev = event;
   if ((ev->win != ecore_x_window_root_first_get())) return 1;
   btn = data;
   if (ev->atom == ECORE_X_ATOM_NET_ACTIVE_WINDOW) 
     {
        ecore_x_window_prop_window_get(ev->win, ev->atom, &xwin, 1);
        if (!xwin) return 1;
        _toggle_kbd_icon(xwin, btn);
     }
   return 1;
}

static char *
_desk_gl_label_get(const void *data, Evas_Object *obj, const char *part) 
{
# ifdef ELM_EFREET
   if (!data) return NULL;
   Efreet_Desktop *d = (Efreet_Desktop *)data;
   if ((!d) || (!d->name)) return NULL;
   return strdup(d->name);
# else
   return NULL;
# endif
}

static Evas_Object *
_desk_gl_icon_get(const void *data, Evas_Object *obj, const char *part) 
{
#ifdef ELM_EFREET
   if (!data) return NULL;

   Efreet_Desktop *d = (Efreet_Desktop *)data;
   char *path;
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
        free(path);
        return ic;
     }
   return ic;
#else
   return NULL;
#endif
}

static void 
_desk_gl_del(const void *data, Evas_Object *obj) 
{

}

static void 
_desk_gl_select(void *data, Evas_Object *obj, void *event) 
{
# ifdef ELM_EFREET
   if (!data) return;
   Efreet_Desktop *d = (Efreet_Desktop *)data;
   if ((!d) || (!d->name)) return;
   if (d->exec) _desktop_run(d);
# endif
}

static int 
_desktop_list_change(void *data, int type, void *event) 
{
   if (defer) ecore_timer_del(defer);
   defer = ecore_timer_add(1.0, _desktop_update_deferred, NULL);
   return 1;
}

static int 
_desktop_update_deferred(void *data) 
{
   _apps_unpopulate();
   _apps_populate();
   defer = NULL;
   return 0;
}

static void 
_desktop_run(Efreet_Desktop *desktop) 
{
   Ind_Home_Exec *exec;
   Eina_List *l;

   if ((!desktop) || (!desktop->exec)) return;
   EINA_LIST_FOREACH(exes, l, exec) 
     {
        if (exec->desktop == desktop) 
          {
             if (exec->exe) return;
          }
     }

   exec = calloc(1, sizeof(Ind_Home_Exec));
   if (!exec) return;

   exec->exe = ecore_exe_run(desktop->exec, NULL);
   exec->desktop = desktop;
   if (exec->exe) 
     exec->pid = ecore_exe_pid_get(exec->exe);
   exec->timeout = ecore_timer_add(20.0, _desktop_run_timeout, exec);
   exes = eina_list_append(exes, exec);
}

static int 
_desktop_run_timeout(void *data) 
{
   Ind_Home_Exec *exec;

   if (!(exec = data)) return 1;
   if (!exec->exe) 
     {
        exes = eina_list_remove(exes, exec);
        free(exec);
        return 0;
     }
   exec->timeout = NULL;
   return 0;
}

static int 
_desktop_run_exit(void *data, int type, void *event) 
{
   Ind_Home_Exec *exec;
   Eina_List *l;
   Ecore_Exe_Event_Del *ev;

   ev = event;
   EINA_LIST_FOREACH(exes, l, exec) 
     {
        if (exec->pid == ev->pid) 
          {
             exes = eina_list_remove_list(exes, l);
             if (exec->timeout) ecore_timer_del(exec->timeout);
             free(exec);
             return 1;
          }
     }
   return 1;
}

static void 
_apps_populate(void) 
{
# ifdef ELM_EFREET
   _desktops_populate();
# endif
}

static void 
_apps_unpopulate(void) 
{
# ifdef ELM_EFREET
   eina_list_free(desktops);
   desktops = NULL;
# endif
}

static void 
_desktops_populate(void) 
{
# ifdef ELM_EFREET
   desktops = efreet_util_desktop_name_glob_list("*");
   if (desktops) 
     {
        Ind_Home_Win *hwin;
        Efreet_Desktop *d;
        Eina_List *l = NULL, *ll = NULL;

        EINA_LIST_FOREACH(hwins, l, hwin) 
          {
             if (!hwin->gl) continue;
             elm_genlist_clear(hwin->gl);
             EINA_LIST_FOREACH(desktops, ll, d) 
               elm_genlist_item_append(hwin->gl, &it_desk, d, NULL, 
                                       ELM_GENLIST_ITEM_NONE, _desk_gl_select, 
                                       d);
          }
     }
# endif
}

static void 
_toggle_kbd_icon(Ecore_X_Window xwin, Evas_Object *obj) 
{
   Evas_Object *icon;
   Ecore_X_Virtual_Keyboard_State state;
   char buff[PATH_MAX];

   if (!xwin) 
     snprintf(buff, sizeof(buff), "%s/images/kbd-off.png", PACKAGE_DATA_DIR);
   else 
     {
        state = ecore_x_e_virtual_keyboard_state_get(xwin);
        if ((state == ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF) || 
            (state == ECORE_X_VIRTUAL_KEYBOARD_STATE_UNKNOWN)) 
          {
//             ecore_x_e_virtual_keyboard_state_set(xwin, 
//                                                  ECORE_X_VIRTUAL_KEYBOARD_STATE_ON);
             snprintf(buff, sizeof(buff), "%s/images/kbd-off.png", 
                      PACKAGE_DATA_DIR);
          }
        else 
          {
//             ecore_x_e_virtual_keyboard_state_set(xwin, 
//                                                  ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF);
             snprintf(buff, sizeof(buff), "%s/images/kbd-on.png", 
                      PACKAGE_DATA_DIR);
          }
     }

   icon = elm_button_icon_get(obj);
   if (!icon) return;
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_button_icon_set(obj, icon);
}

#endif
ELM_MAIN();
