#include <Ecore.h>
#include <Ecore_X.h>
#include <Ecore_X_Atoms.h>
#include "elm_indicator.h"
#include "elm_home.h"

#ifndef ELM_LIB_QUICKLAUNCH

/* local function prototypes */
static int _cb_client_message(void *data, int type, void *event);
static void _cb_win_del(void *data, Evas_Object *obj, void *event);
static void _cb_win_move(void *data, Evas_Object *obj, void *event);
static int _cb_win_property_change(void *data, int type, void *event);
static void _cb_btn_home_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_btn_mode_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_btn_kbd_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_rect_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _cb_rect_mouse_move(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _cb_rect_mouse_up(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _set_mode_icon(Evas_Object *btn);
static void _set_kbd_icon(Ecore_X_Window xwin, Evas_Object *btn);
static Eina_Bool _can_drag_win(void);

/* local variables */
static Evas_Object *ind = NULL;
static Eina_List *handlers = NULL;
static int my = 0;

EAPI int 
elm_main(int argc, char **argv) 
{
   Ecore_X_Window xwin;
   Ecore_X_Illume_Mode mode;
   Evas_Object *bg, *box, *icon, *btn, *rect;
   char buff[PATH_MAX];

   if (!elm_home_init()) return EXIT_FAILURE;

   /* create main window */
   ind = elm_win_add(NULL, "elm_indicator", ELM_WIN_BASIC);
   elm_win_title_set(ind, "Illume Indicator Window");
   evas_object_smart_callback_add(ind, "delete-request", _cb_win_del, NULL);
   evas_object_smart_callback_add(ind, "moved", _cb_win_move, NULL);
   xwin = elm_win_xwindow_get(ind);
   ecore_x_icccm_hints_set(xwin, 0, 0, 0, 0, 0, 0, 0);

   /* add background to window */
   bg = elm_bg_add(ind);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(ind, bg);
   evas_object_show(bg);

   /* add packing box to window */
   box = elm_box_add(ind);
   elm_box_horizontal_set(box, EINA_TRUE);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(ind, box);
   evas_object_show(box);

   /* create home button */
   icon = elm_icon_add(ind);
   snprintf(buff, sizeof(buff), "%s/images/home.png", PACKAGE_DATA_DIR);
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   btn = elm_button_add(ind);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_home_clicked, NULL);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(box, btn);
   evas_object_show(btn);
   evas_object_show(icon);

   /* create mode button */
   icon = elm_icon_add(ind);
   btn = elm_button_add(ind);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_mode_clicked, NULL);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(box, btn);
   evas_object_show(btn);
   _set_mode_icon(btn);

   /* add handler for client messages */
   handlers = 
     eina_list_append(handlers, 
                      ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, 
                                              _cb_client_message, btn));

   /* create kbd toggle button */
   icon = elm_icon_add(ind);
   snprintf(buff, sizeof(buff), "%s/images/kbd-off.png", PACKAGE_DATA_DIR);
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   btn = elm_button_add(ind);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_kbd_clicked, NULL);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(box, btn);
   evas_object_show(btn);
   evas_object_show(icon);

   /* add handler for window property changes */
   handlers = 
     eina_list_append(handlers, 
                      ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY, 
                                              _cb_win_property_change, btn));

   /* create rect to handle dragging */
   rect = evas_object_rectangle_add(evas_object_evas_get(ind));
   evas_object_color_set(rect, 0, 0, 0, 0);
   elm_win_resize_object_add(ind, rect);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_DOWN, 
                                  _cb_rect_mouse_down, ind);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_MOVE, 
                                  _cb_rect_mouse_move, ind);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_UP, 
                                  _cb_rect_mouse_up, ind);
   evas_object_repeat_events_set(rect, EINA_TRUE);
   evas_object_raise(rect);
   evas_object_show(rect);

   ecore_x_e_illume_drag_locked_set(xwin, _can_drag_win());

   /* show the indicator window */
   evas_object_show(ind);

   /* create first home window */
   elm_home_win_new();

   /* if we are in dual mode, create another home window */
   mode = ecore_x_e_illume_mode_get(ecore_x_window_root_first_get());
   if (mode > ECORE_X_ILLUME_MODE_SINGLE)
     elm_home_win_new();

   /* tell elm to start */
   elm_run();

   /* shutdown home subsystem. This cleans up any home windows */
   elm_home_shutdown();

   /* shutdown elm */
   elm_shutdown();
   return 0;
}

/* local functions */
static int 
_cb_client_message(void *data, int type, void *event) 
{
   Ecore_X_Event_Client_Message *ev;

   ev = event;
   if (ev->message_type == ECORE_X_ATOM_E_ILLUME_HOME) 
     elm_home_win_new();
   else if (ev->message_type == ECORE_X_ATOM_E_ILLUME_MODE) 
     _set_mode_icon(data);
   return 1;
}

static void 
_cb_win_del(void *data, Evas_Object *obj, void *event) 
{
   Ecore_Event_Handler *handler;

   EINA_LIST_FREE(handlers, handler)
     ecore_event_handler_del(handler);

   elm_exit();
}

static void 
_cb_win_move(void *data, Evas_Object *obj, void *event) 
{
   int x, y;

   elm_win_screen_position_get(obj, &x, &y);
   printf("Moved To: %d\n", y);
}

static int 
_cb_win_property_change(void *data, int type, void *event) 
{
   Ecore_X_Event_Window_Property *ev;

   ev = event;
   if (ev->win != ecore_x_window_root_first_get()) return 1;
   if (ev->atom == ECORE_X_ATOM_NET_ACTIVE_WINDOW) 
     {
        Evas_Object *btn;
        Ecore_X_Window xwin = 0;

        btn = data;
        ecore_x_window_prop_window_get(ev->win, ev->atom, &xwin, 1);
        if (!xwin) return 1;
        _set_kbd_icon(xwin, btn);
     }
   return 1;
}

static void 
_cb_btn_home_clicked(void *data, Evas_Object *obj, void *event) 
{
   elm_home_win_new();
}

static void 
_cb_btn_mode_clicked(void *data, Evas_Object *obj, void *event) 
{
   Ecore_X_Window xwin;
   Ecore_X_Illume_Mode mode;

   xwin = ecore_x_window_root_first_get();
   mode = ecore_x_e_illume_mode_get(xwin);
   mode += 1;
   if (mode > ECORE_X_ILLUME_MODE_DUAL_LEFT)
     mode = ECORE_X_ILLUME_MODE_SINGLE;
   ecore_x_e_illume_mode_set(xwin, mode);
   ecore_x_e_illume_mode_send(xwin, mode);
   _set_mode_icon(obj);
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
     ecore_x_e_virtual_keyboard_state_set(xwin, ECORE_X_VIRTUAL_KEYBOARD_STATE_ON);
   else
     ecore_x_e_virtual_keyboard_state_set(xwin, ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF);
   _set_kbd_icon(xwin, obj);
}

static void 
_cb_rect_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Evas_Event_Mouse_Down *ev;

   ev = event;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return;
   if (ev->button == 1) 
     {
        Evas_Object *win;
        Ecore_X_Window xwin;

        if (!(win = data)) return;
        xwin = elm_win_xwindow_get(win);
        if (ecore_x_e_illume_drag_locked_get(xwin)) return;
        ecore_x_e_illume_drag_set(xwin, 1);
        ecore_x_e_illume_drag_start_send(xwin);
        ecore_x_pointer_last_xy_get(NULL, &my);
     }
}

static void 
_cb_rect_mouse_move(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Evas_Event_Mouse_Move *ev;
   Ecore_X_Window xwin;
   Evas_Object *win;
   int x, y, h, py, ny, dy, zh;

   ev = event;
   if (!(win = data)) return;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return;

   xwin = elm_win_xwindow_get(win);
   if (ecore_x_e_illume_drag_locked_get(xwin)) return;
   if (!ecore_x_e_illume_drag_get(xwin)) return;

   /* grab the size of the screen */
   ecore_x_window_geometry_get(ecore_x_window_root_first_get(), 
                               NULL, NULL, NULL, &zh);

   /* grab position & size of our window */
   evas_object_geometry_get(win, NULL, NULL, NULL, &h);
   elm_win_screen_position_get(win, &x, &y);

   /* make sure we will not go off screen */
   if ((y + h + ev->cur.output.y) >= zh) return;

   ecore_x_pointer_last_xy_get(NULL, &py);
   dy = ((zh - h) / 8);

   if (ev->cur.output.y > ev->prev.output.y) 
     {
        if ((py - my) < dy) return;
     }
   else 
     {
        if ((my - py) < dy) return;
     }

   if (py > my)
     ny = y + dy;
   else if (py <= my)
     ny = y - dy;
   else return;

   if (y != ny) 
     {
        ecore_x_window_move(xwin, x, ny);
        evas_object_move(win, x, ny);
        my = py;
     }
}

static void 
_cb_rect_mouse_up(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Ecore_X_Window xwin;
   Evas_Event_Mouse_Up *ev;
   Evas_Object *win;

   ev = event;
   if (ev->event_flags & EVAS_EVENT_FLAG_ON_HOLD) return;
   if (ev->button != 1) return;
   if (!(win = data)) return;
   xwin = elm_win_xwindow_get(win);
   if (ecore_x_e_illume_drag_locked_get(xwin)) return;
   if (!ecore_x_e_illume_drag_get(xwin)) return;
   ecore_x_e_illume_drag_end_send(xwin);
   my = 0;
}

static void 
_set_mode_icon(Evas_Object *btn) 
{
   Ecore_X_Window xwin;
   Ecore_X_Illume_Mode mode;
   Evas_Object *icon;
   char buff[PATH_MAX];

   if (!(icon = elm_button_icon_get(btn))) return;
   xwin = ecore_x_window_root_first_get();
   mode = ecore_x_e_illume_mode_get(xwin);
   if (mode == ECORE_X_ILLUME_MODE_SINGLE)
     snprintf(buff, sizeof(buff), "%s/images/single.png", PACKAGE_DATA_DIR);
   else if (mode == ECORE_X_ILLUME_MODE_DUAL_TOP)
     snprintf(buff, sizeof(buff), "%s/images/dual-top.png", PACKAGE_DATA_DIR);
   else if (mode == ECORE_X_ILLUME_MODE_DUAL_LEFT)
     snprintf(buff, sizeof(buff), "%s/images/dual-left.png", PACKAGE_DATA_DIR);

   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_button_icon_set(btn, icon);
   evas_object_show(icon);
}

static void 
_set_kbd_icon(Ecore_X_Window xwin, Evas_Object *btn) 
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
             snprintf(buff, sizeof(buff), "%s/images/kbd-off.png", 
                      PACKAGE_DATA_DIR);
          }
        else 
          {
             snprintf(buff, sizeof(buff), "%s/images/kbd-on.png", 
                      PACKAGE_DATA_DIR);
          }
     }

   icon = elm_button_icon_get(btn);
   if (!icon) return;
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_button_icon_set(btn, icon);
}

static Eina_Bool 
_can_drag_win(void) 
{
   Ecore_X_Illume_Mode mode;

   mode = ecore_x_e_illume_mode_get(ecore_x_window_root_first_get());
   if (mode == ECORE_X_ILLUME_MODE_DUAL_TOP)
     return EINA_FALSE;
   return EINA_TRUE;
}

#endif
ELM_MAIN();
