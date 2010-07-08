#include "elm_indicator.h"
#include "elm_ind_win.h"
#include "elm_home.h"

/* local function prototypes */
static void _cb_win_del(void *data, Evas_Object *obj, void *event);
static void _cb_btn_home_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_btn_mode_clicked(void *data, Evas_Object *obj, void *event);
static void _cb_btn_kbd_clicked(void *data, Evas_Object *obj, void *event);
static Eina_Bool _cb_client_message(void *data, int type, void *event);
static Eina_Bool _cb_win_property_change(void *data, int type, void *event);
static void _cb_rect_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _cb_rect_mouse_move(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _cb_rect_mouse_up(void *data, Evas *evas, Evas_Object *obj, void *event);
static void _set_mode_icon(Elm_Ind_Win *iwin);
static void _set_kbd_icon(Ecore_X_Window xwin, Evas_Object *btn);

EAPI Elm_Ind_Win *
elm_ind_win_new(Ecore_X_Window zone) 
{
   Elm_Ind_Win *iwin;
   Ecore_X_Window xwin;
   Ecore_X_Window_State states[2];
   Ecore_X_Illume_Mode mode;
   Evas_Object *bg, *box, *icon, *btn, *rect;
   char buff[PATH_MAX];
   int zx, zy, zw;

   iwin = calloc(1, sizeof(Elm_Ind_Win));
   if (!iwin) return NULL;

   iwin->win = elm_win_add(NULL, "Illume-Indicator", ELM_WIN_DOCK);
   evas_object_data_set(iwin->win, "zone", (const void *)zone);
   elm_win_title_set(iwin->win, "Illume Indicator");
   evas_object_smart_callback_add(iwin->win, "delete-request", 
                                  _cb_win_del, iwin);

   xwin = elm_win_xwindow_get(iwin->win);
   ecore_x_icccm_hints_set(xwin, 0, 0, 0, 0, 0, 0, 0);
   states[0] = ECORE_X_WINDOW_STATE_SKIP_TASKBAR;
   states[1] = ECORE_X_WINDOW_STATE_SKIP_PAGER;
   ecore_x_netwm_window_state_set(xwin, states, 2);

   /* add background to window */
   bg = elm_bg_add(iwin->win);
   evas_object_size_hint_weight_set(bg, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(iwin->win, bg);
   evas_object_show(bg);

   /* create rect to handle dragging */
   rect = evas_object_rectangle_add(evas_object_evas_get(iwin->win));
   evas_object_color_set(rect, 0, 0, 0, 0);
   elm_win_resize_object_add(iwin->win, rect);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_DOWN, 
                                  _cb_rect_mouse_down, iwin);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_MOVE, 
                                  _cb_rect_mouse_move, iwin);
   evas_object_event_callback_add(rect, EVAS_CALLBACK_MOUSE_UP, 
                                  _cb_rect_mouse_up, iwin);
   evas_object_show(rect);

   /* add packing box to window */
   box = elm_box_add(iwin->win);
   elm_box_horizontal_set(box, EINA_TRUE);
   evas_object_size_hint_weight_set(box, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
   elm_win_resize_object_add(iwin->win, box);
   evas_object_show(box);

   /* create home button */
   icon = elm_icon_add(iwin->win);
   snprintf(buff, sizeof(buff), "%s/images/home.png", PACKAGE_DATA_DIR);
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   btn = elm_button_add(iwin->win);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_home_clicked, iwin);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(box, btn);
   evas_object_show(btn);
   evas_object_show(icon);

   /* create mode button */
   icon = elm_icon_add(iwin->win);
   iwin->mode = elm_button_add(iwin->win);
   elm_button_icon_set(iwin->mode, icon);
   evas_object_smart_callback_add(iwin->mode, "clicked", 
                                  _cb_btn_mode_clicked, iwin);
   evas_object_size_hint_align_set(iwin->mode, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(box, iwin->mode);
   evas_object_show(iwin->mode);
   _set_mode_icon(iwin);

   iwin->handlers = 
     eina_list_append(iwin->handlers, 
                      ecore_event_handler_add(ECORE_X_EVENT_CLIENT_MESSAGE, 
                                              _cb_client_message, iwin));

   /* create kbd toggle button */
   icon = elm_icon_add(iwin->win);
   snprintf(buff, sizeof(buff), "%s/images/kbd-off.png", PACKAGE_DATA_DIR);
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   btn = elm_button_add(iwin->win);
   elm_button_icon_set(btn, icon);
   evas_object_smart_callback_add(btn, "clicked", _cb_btn_kbd_clicked, NULL);
   evas_object_size_hint_align_set(btn, EVAS_HINT_FILL, 0.0);
   elm_box_pack_end(box, btn);
   evas_object_show(btn);
   evas_object_show(icon);

   iwin->handlers = 
     eina_list_append(iwin->handlers, 
                      ecore_event_handler_add(ECORE_X_EVENT_WINDOW_PROPERTY, 
                                              _cb_win_property_change, btn));

   ecore_x_window_geometry_get(zone, &zx, &zy, &zw, NULL);
   evas_object_move(iwin->win, zx, zy);
   evas_object_resize(iwin->win, zw, 32);
   evas_object_show(iwin->win);

   ecore_x_e_illume_indicator_geometry_set(zone, zx, zy, zw, 32);

   /* create first home window */
   elm_home_win_new(zone);

   /* create second home window if needed */
   mode = ecore_x_e_illume_mode_get(zone);
   if (mode > ECORE_X_ILLUME_MODE_SINGLE)
     elm_home_win_new(zone);

   return iwin;
}

/* local functions */
static void 
_cb_win_del(void *data, Evas_Object *obj, void *event) 
{
   elm_exit();
}

static void 
_cb_btn_home_clicked(void *data, Evas_Object *obj, void *event) 
{
   Elm_Ind_Win *iwin;
   Ecore_X_Window zone;

   if (!(iwin = data)) return;
   zone = (Ecore_X_Window)evas_object_data_get(iwin->win, "zone");
   elm_home_win_new(zone);
}

static void 
_cb_btn_mode_clicked(void *data, Evas_Object *obj, void *event) 
{
   Elm_Ind_Win *iwin;
   Ecore_X_Window zone;
   Ecore_X_Illume_Mode mode;

   if (!(iwin = data)) return;
   zone = (Ecore_X_Window)evas_object_data_get(iwin->win, "zone");
   mode = ecore_x_e_illume_mode_get(zone);
   mode += 1;
   if (mode > ECORE_X_ILLUME_MODE_DUAL_LEFT)
     mode = ECORE_X_ILLUME_MODE_SINGLE;
   ecore_x_e_illume_mode_set(zone, mode);
   ecore_x_e_illume_mode_send(zone, mode);
   _set_mode_icon(iwin);
}

static void 
_cb_btn_kbd_clicked(void *data, Evas_Object *obj, void *event) 
{
   Ecore_X_Window xwin;
   Ecore_X_Virtual_Keyboard_State state;

   if (!(xwin = ecore_x_window_focus_get())) return;
   state = ecore_x_e_virtual_keyboard_state_get(xwin);
   if (state <= ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF)
     ecore_x_e_virtual_keyboard_state_set(xwin, 
                                          ECORE_X_VIRTUAL_KEYBOARD_STATE_ON);
   else
     ecore_x_e_virtual_keyboard_state_set(xwin, 
                                          ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF);
   _set_kbd_icon(xwin, obj);
}

static Eina_Bool 
_cb_client_message(void *data, int type, void *event) 
{
   Elm_Ind_Win *iwin;
   Ecore_X_Event_Client_Message *ev;

   ev = event;
   if (!(iwin = data)) return EINA_TRUE;
   if (ev->message_type == ECORE_X_ATOM_E_ILLUME_HOME_NEW) 
     {
        Ecore_X_Window zone;

        zone = (Ecore_X_Window)evas_object_data_get(iwin->win, "zone");
        elm_home_win_new(zone);
     }
   else if (ev->message_type == ECORE_X_ATOM_E_ILLUME_MODE) 
     _set_mode_icon(iwin);

   return EINA_TRUE;
}

static Eina_Bool 
_cb_win_property_change(void *data, int type, void *event) 
{
   Ecore_X_Event_Window_Property *ev;

   ev = event;
   //if (ev->win != ecore_x_window_root_first_get()) return 1;
   if (ev->atom == ECORE_X_ATOM_NET_ACTIVE_WINDOW) 
     {
        Evas_Object *btn;
        Ecore_X_Window xwin = 0;

        btn = data;
        ecore_x_window_prop_window_get(ev->win, ev->atom, &xwin, 1);
        if (!xwin) return EINA_TRUE;
        _set_kbd_icon(xwin, btn);
     }
   return EINA_TRUE;
}

static void 
_cb_rect_mouse_down(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Evas_Event_Mouse_Down *ev;
   Elm_Ind_Win *iwin;

   ev = event;
   if (!(iwin = data)) return;
   if (ev->button == 1) 
     {
        Ecore_X_Window xwin, zone;

        iwin->mouse_down = 1;
        xwin = elm_win_xwindow_get(iwin->win);
        if (ecore_x_e_illume_drag_locked_get(xwin)) return;
        iwin->drag.start = 1;
        iwin->drag.dnd = 0;
        ecore_x_pointer_last_xy_get(NULL, &iwin->drag.y);
     }
}

static void 
_cb_rect_mouse_move(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Elm_Ind_Win *iwin;
   Evas_Event_Mouse_Move *ev;
   Ecore_X_Window xwin, zone;
   int x, y, h, py, ny, dy, zh;

   ev = event;
   if (!(iwin = data)) return;
   xwin = elm_win_xwindow_get(iwin->win);
   if (iwin->drag.start) 
     {
        ecore_x_e_illume_drag_start_send(xwin);
        iwin->drag.dnd = 1;
        iwin->drag.start = 0;
     }

   if (!iwin->drag.dnd) return;

   zone = (Ecore_X_Window)evas_object_data_get(iwin->win, "zone");

   /* grab the size of the screen */
   ecore_x_window_geometry_get(zone, &x, NULL, NULL, &zh);

   /* grab position & size of our window */
   evas_object_geometry_get(iwin->win, NULL, NULL, NULL, &h);
   elm_win_screen_position_get(iwin->win, NULL, &y);

   /* make sure we will not go off screen */
   if ((y + h + ev->cur.output.y) >= zh) return;

   ecore_x_pointer_last_xy_get(NULL, &py);
   dy = ((zh - h) / 8);

   if (ev->cur.output.y > ev->prev.output.y) 
     {
        if ((py - iwin->drag.y) < dy) return;
     }
   else if (ev->cur.output.y < ev->prev.output.y)
     {
        if ((iwin->drag.y - py) < dy) return;
     }
   else return;

   if (py > iwin->drag.y)
     ny = y + dy;
   else if (py < iwin->drag.y)
     ny = y - dy;
   else return;

   if (y != ny) 
     {
        ecore_x_window_move(xwin, x, ny);
        iwin->drag.y = py;
     }
}

static void 
_cb_rect_mouse_up(void *data, Evas *evas, Evas_Object *obj, void *event) 
{
   Elm_Ind_Win *iwin;
   Ecore_X_Window xwin;
   Evas_Event_Mouse_Up *ev;
   Ecore_X_Window zone;

   ev = event;
   if (ev->button != 1) return;
   if (!(iwin = data)) return;
   zone = (Ecore_X_Window)evas_object_data_get(iwin->win, "zone");
   if ((!iwin->drag.dnd) && (iwin->mouse_down == 1)) 
     {
        ecore_x_e_illume_quickpanel_state_send
          (zone, ECORE_X_ILLUME_QUICKPANEL_STATE_ON);
     }
   else if (iwin->drag.dnd) 
     {
        int x, y, w, h;

        xwin = elm_win_xwindow_get(iwin->win);
        ecore_x_window_geometry_get(xwin, &x, &y, &w, &h);
        ecore_x_e_illume_indicator_geometry_set(zone, x, y, w, h);
        ecore_x_e_illume_drag_end_send(xwin);
        ecore_x_e_illume_quickpanel_position_update_send(xwin);
     }
   iwin->drag.start = 0;
   iwin->drag.dnd = 0;
   iwin->drag.y = 0;
   iwin->mouse_down = 0;
}

static void 
_set_mode_icon(Elm_Ind_Win *iwin) 
{
   Ecore_X_Window zone;
   Ecore_X_Illume_Mode mode;
   Evas_Object *icon;
   char buff[PATH_MAX];

   if (!(icon = elm_button_icon_get(iwin->mode))) return;
   zone = (Ecore_X_Window)evas_object_data_get(iwin->win, "zone");
   mode = ecore_x_e_illume_mode_get(zone);
   if (mode <= ECORE_X_ILLUME_MODE_SINGLE)
     snprintf(buff, sizeof(buff), "%s/images/single.png", PACKAGE_DATA_DIR);
   else if (mode == ECORE_X_ILLUME_MODE_DUAL_TOP)
     snprintf(buff, sizeof(buff), "%s/images/dual-top.png", PACKAGE_DATA_DIR);
   else if (mode == ECORE_X_ILLUME_MODE_DUAL_LEFT)
     snprintf(buff, sizeof(buff), "%s/images/dual-left.png", PACKAGE_DATA_DIR);
   elm_icon_file_set(icon, buff, NULL);
   evas_object_size_hint_aspect_set(icon, EVAS_ASPECT_CONTROL_VERTICAL, 1, 1);
   elm_button_icon_set(iwin->mode, icon);
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
        if (state <= ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF) 
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
