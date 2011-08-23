# Copyright (c) 2008 Simon Busch
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

cimport evas.c_evas
import evas.c_evas

cdef extern from *:
    ctypedef char* const_char_ptr "const char *"
    ctypedef void const_void "const void"

cdef extern from "string.h":
    void *memcpy(void *dst, void *src, int n)
    char *strdup(char *str)

cdef extern from "Ecore_X.h":
    ctypedef unsigned int Ecore_X_ID
    ctypedef Ecore_X_ID Ecore_X_Window

# For Debugging
"""
cdef extern from "Ecore_Evas.h":
    ctypedef struct Ecore_Evas
    void ecore_evas_geometry_get(Ecore_Evas *ee, int *x, int *y, int *w, int *h)

cdef extern from "Eina.h":
    ctypedef struct Eina_List

cdef extern from "Ecore.h":
    ctypedef struct Ecore_Job

ctypedef struct Elm_Win:
    Ecore_Evas *ee
    evas.c_evas.Evas *evas
    evas.c_evas.Evas_Object *parent
    evas.c_evas.Evas_Object *win_obj
    Eina_List *subobjs
    Ecore_X_ID xwin
    Ecore_Job *deferred_resize_job
    Ecore_Job *deferred_child_eval_job
    Elm_Win_Type type
    Elm_Win_Keyboard_Mode kbdmode
    evas.c_evas.Eina_Bool autodel = 1
"""

cdef extern from "Elementary.h":
    ctypedef enum Elm_Win_Type:
        ELM_WIN_BASIC
        ELM_WIN_DIALOG_BASIC
        ELM_WIN_DESKTOP
        ELM_WIN_DOCK
        ELM_WIN_TOOLBAR
        ELM_WIN_MENU
        ELM_WIN_UTILITY
        ELM_WIN_SPLASH

    ctypedef enum Elm_Win_Keyboard_Mode:
        ELM_WIN_KEYBOARD_OFF
        ELM_WIN_KEYBOARD_ON
        ELM_WIN_KEYBOARD_ALPHA
        ELM_WIN_KEYBOARD_NUMERIC
        ELM_WIN_KEYBOARD_PIN
        ELM_WIN_KEYBOARD_PHONE_NUMBER
        ELM_WIN_KEYBOARD_HEX
        ELM_WIN_KEYBOARD_TERMINAL
        ELM_WIN_KEYBOARD_PASSWORD

    ctypedef enum Elm_Policy:
        ELM_POLICY_QUIT
        ELM_POLICY_LAST

    ctypedef enum Elm_Policy_Quit:
        ELM_POLICY_QUIT_NONE
        ELM_POLICY_QUIT_LAST_WINDOW_CLOSED

    ctypedef enum Elm_Hover_Axis:
        ELM_HOVER_AXIS_NONE
        ELM_HOVER_AXIS_HORIZONTAL
        ELM_HOVER_AXIS_VERTICAL
        ELM_HOVER_AXIS_BOTH

    ctypedef enum Elm_Text_Format:
        ELM_TEXT_FORMAT_PLAIN_UTF8
        ELM_TEXT_FORMAT_MARKUP_UTF8

    ctypedef enum Elm_List_Mode:
        ELM_LIST_COMPRESS
        ELM_LIST_SCROLL
        ELM_LIST_LIMIT
        ELM_LIST_EXPAND

    ctypedef enum Elm_Flip_Mode:
        ELM_FLIP_ROTATE_Y_CENTER_AXIS
        ELM_FLIP_ROTATE_X_CENTER_AXIS
        ELM_FLIP_ROTATE_XZ_CENTER_AXIS
        ELM_FLIP_ROTATE_YZ_CENTER_AXIS
        ELM_FLIP_CUBE_LEFT
        ELM_FLIP_CUBE_RIGHT

    ctypedef enum Elm_Genlist_Item_Flags:
        ELM_GENLIST_ITEM_NONE
        ELM_GENLIST_ITEM_SUBITEMS

    ctypedef enum Elm_Image_Orient:
        ELM_IMAGE_ORIENT_NONE
        ELM_IMAGE_ROTATE_90_CW
        ELM_IMAGE_ROTATE_180_CW
        ELM_IMAGE_ROTATE_90_CCW
        ELM_IMAGE_FLIP_HORIZONTAL
        ELM_IMAGE_FLIP_VERTICAL
        ELM_IMAGE_FLIP_TRANSPOSE
        ELM_IMAGE_FLIP_TRANSVERSE

    ctypedef enum Elm_Scroller_Policy:
        ELM_SCROLLER_POLICY_AUTO
        ELM_SCROLLER_POLICY_ON
        ELM_SCROLLER_POLICY_OFF

    ctypedef enum Elm_Panel_Orient:
        ELM_PANEL_ORIENT_TOP,
        ELM_PANEL_ORIENT_BOTTOM,
        ELM_PANEL_ORIENT_LEFT,
        ELM_PANEL_ORIENT_RIGHT,

    ctypedef enum Elm_Icon_Type:
        ELM_ICON_NONE
        ELM_ICON_FILE
        ELM_ICON_STANDARD

    ctypedef enum Elm_Wrap_Type:
        ELM_WRAP_NONE
        ELM_WRAP_CHAR
        ELM_WRAP_WORD
        ELM_WRAP_MIXED
        ELM_WRAP_LAST

    ctypedef struct Elm_Entry_Anchor_Info
    ctypedef struct Elm_Entry_Anchorview_Info:
        char *name
        int   button
        evas.c_evas.Evas_Object *hover
        evas.c_evas.Eina_Rectangle anchor, hover_parent
        evas.c_evas.Eina_Bool hover_left
        evas.c_evas.Eina_Bool hover_right
        evas.c_evas.Eina_Bool hover_top
        evas.c_evas.Eina_Bool hover_bottom

    ctypedef struct Elm_Entry_Anchorblock_Info:
        char *name
        int   button
        evas.c_evas.Evas_Object *hover
        evas.c_evas.Eina_Rectangle anchor, hover_parent
        evas.c_evas.Eina_Bool hover_left
        evas.c_evas.Eina_Bool hover_right
        evas.c_evas.Eina_Bool hover_top
        evas.c_evas.Eina_Bool hover_bottom

    ctypedef char *(*GenlistItemLabelGetFunc)(void *data, evas.c_evas.Evas_Object *obj, const_char_ptr part)
    ctypedef evas.c_evas.Evas_Object *(*GenlistItemIconGetFunc)(void *data, evas.c_evas.Evas_Object *obj, const_char_ptr part)
    ctypedef evas.c_evas.Eina_Bool (*GenlistItemStateGetFunc)(void *data, evas.c_evas.Evas_Object *obj, const_char_ptr part)
    ctypedef void (*GenlistItemDelFunc)(void *data, evas.c_evas.Evas_Object *obj)

    ctypedef struct Elm_Genlist_Item_Class_Func:
        GenlistItemLabelGetFunc label_get
        GenlistItemIconGetFunc icon_get
        GenlistItemStateGetFunc state_get
        GenlistItemDelFunc del_ "del"

    ctypedef struct Elm_Genlist_Item_Class:
        char *item_style
        Elm_Genlist_Item_Class_Func func

    ctypedef char *(*GengridItemLabelGetFunc)(void *data, evas.c_evas.Evas_Object *obj, const_char_ptr part)
    ctypedef evas.c_evas.Evas_Object *(*GengridItemIconGetFunc)(void *data, evas.c_evas.Evas_Object *obj, const_char_ptr part)
    ctypedef evas.c_evas.Eina_Bool (*GengridItemStateGetFunc)(void *data, evas.c_evas.Evas_Object *obj, const_char_ptr part)
    ctypedef void (*GengridItemDelFunc)(void *data, evas.c_evas.Evas_Object *obj)

    ctypedef struct Elm_Gengrid_Item_Class_Func:
        GengridItemLabelGetFunc label_get
        GengridItemIconGetFunc icon_get
        GengridItemStateGetFunc state_get
        GengridItemDelFunc del_ "del"

    ctypedef struct Elm_Gengrid_Item_Class:
        char *item_style
        Elm_Gengrid_Item_Class_Func func

    ctypedef evas.c_evas.Evas_Object *(*Elm_Tooltip_Content_Cb) (void *data, evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *tooltip)
    ctypedef evas.c_evas.Evas_Object *(*Elm_Tooltip_Item_Content_Cb) (void *data, evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *tooltip, void *item)
    ctypedef evas.c_evas.Eina_Bool (*Elm_Event_Cb) (void *data, evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *src, evas.c_evas.Evas_Callback_Type t, void *event_info)

    ctypedef struct Elm_Hoversel_Item
    ctypedef struct Elm_Menu_Item
    ctypedef struct Elm_Toolbar_Item
    ctypedef struct Elm_List_Item
    ctypedef struct Elm_Carousel_Item
    ctypedef struct Elm_Genlist_Item
    ctypedef struct Elm_Gengrid_Item
    ctypedef struct Elm_Theme

    # Basic elementary functions
    void elm_init(int argc,char** argv)
    void elm_shutdown()
    void elm_run() nogil
    void elm_exit()

    # Quicklaunch
    void         elm_quicklaunch_init(int argc, char **argv)
    void         elm_quicklaunch_sub_init(int argc, char **argv)
    void         elm_quicklaunch_sub_shutdown()
    void         elm_quicklaunch_shutdown()
    void         elm_quicklaunch_seed()
    evas.c_evas.Eina_Bool    elm_quicklaunch_prepare(int argc, char **argv)
    evas.c_evas.Eina_Bool    elm_quicklaunch_fork(int argc, char **argv, char *cwd, void (*postfork_func) (void *data), void *postfork_data)
    void         elm_quicklaunch_cleanup()
    int          elm_quicklaunch_fallback(int argc, char **argv)
    char        *elm_quicklaunch_exe_path_get(char *exe)

    # Policy
    evas.c_evas.Eina_Bool     elm_policy_set(unsigned int policy, int value)
    int          elm_policy_get(unsigned int policy)

    # Object handling
    char        *elm_object_widget_type_get(evas.c_evas.Evas_Object *obj)

    void         elm_object_scale_set(evas.c_evas.Evas_Object *obj, double scale)
    double       elm_object_scale_get(evas.c_evas.Evas_Object *obj)
    void         elm_object_style_set(evas.c_evas.Evas_Object *obj, char *style)
    char*        elm_object_style_get(evas.c_evas.Evas_Object *obj)
    void         elm_object_disabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool disabled)
    evas.c_evas.Eina_Bool elm_object_disabled_get(evas.c_evas.Evas_Object *obj)

    void         elm_object_scroll_hold_push(evas.c_evas.Evas_Object *obj)
    void         elm_object_scroll_hold_pop(evas.c_evas.Evas_Object *obj)
    void         elm_object_scroll_freeze_push(evas.c_evas.Evas_Object *obj)
    void         elm_object_scroll_freeze_pop(evas.c_evas.Evas_Object *obj)

    double       elm_tooltip_delay_get()
    evas.c_evas.Eina_Bool elm_tooltip_delay_set(double delay)
    evas.c_evas.Eina_Bool elm_cursor_engine_only_set(int engine_only)
    int          elm_cursor_engine_only_get()
    void         elm_object_tooltip_show(evas.c_evas.Evas_Object *obj)
    void         elm_object_tooltip_hide(evas.c_evas.Evas_Object *obj)
    void         elm_object_tooltip_text_set(evas.c_evas.Evas_Object *obj, char *text)
    void         elm_object_tooltip_content_cb_set(evas.c_evas.Evas_Object *obj, Elm_Tooltip_Content_Cb func, void *data, evas.c_evas.Evas_Smart_Cb del_cb)
    void         elm_object_tooltip_unset(evas.c_evas.Evas_Object *obj)
    void         elm_object_tooltip_style_set(evas.c_evas.Evas_Object *obj, char *style)
    char*        elm_object_tooltip_style_get(evas.c_evas.Evas_Object *obj)

    void         elm_object_cursor_set(evas.c_evas.Evas_Object *obj, char *cursor)
    void         elm_object_cursor_unset(evas.c_evas.Evas_Object *obj)
    void         elm_object_cursor_style_set(evas.c_evas.Evas_Object *obj, char *style)
    char*        elm_object_cursor_style_get(evas.c_evas.Evas_Object *obj)
    void         elm_object_cursor_engine_only_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool engine_only)
    evas.c_evas.Eina_Bool elm_object_cursor_engine_only_get(evas.c_evas.Evas_Object *obj)


    void         elm_object_text_part_set(evas.c_evas.Evas_Object *obj, const_char_ptr part, const_char_ptr label)
    char*        elm_object_text_part_get(evas.c_evas.Evas_Object *obj, const_char_ptr part)

    void         elm_object_text_set(evas.c_evas.Evas_Object *obj, const_char_ptr label)
    char*        elm_object_text_get(evas.c_evas.Evas_Object *obj)


    double       elm_scale_get()
    void         elm_scale_set(double scale)
    evas.c_evas.Evas_Coord elm_finger_size_get()
    void         elm_finger_size_set(evas.c_evas.Evas_Coord size)

    # Focus handling
    void         elm_object_focus_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool x)
    evas.c_evas.Eina_Bool elm_object_focus_get(evas.c_evas.Evas_Object *obj)
    void         elm_object_focus_allow_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool enable)
    evas.c_evas.Eina_Bool elm_object_focus_allow_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool elm_focus_highlight_enabled_get()
    void         elm_focus_highlight_enabled_set(evas.c_evas.Eina_Bool enable)
    evas.c_evas.Eina_Bool elm_focus_highlight_animate_get()
    void         elm_focus_highlight_animate_set(evas.c_evas.Eina_Bool animate)

    void         elm_object_event_callback_add(evas.c_evas.Evas_Object *obj, Elm_Event_Cb func, const_void *data)
    void        *elm_object_event_callback_del(evas.c_evas.Evas_Object *obj, Elm_Event_Cb func, const_void *data)

    void         elm_coords_finger_size_adjust(int times_w, evas.c_evas.Evas_Coord *w, int times_h, evas.c_evas.Evas_Coord *h)

    void         elm_theme_overlay_add(Elm_Theme *th, char *item)
    void         elm_theme_extension_add(Elm_Theme *th, char *item)

    void         elm_object_focus_custom_chain_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_List *objs)
    void         elm_object_focus_custom_chain_unset(evas.c_evas.Evas_Object *obj)
    evas.c_evas.const_Eina_List *elm_object_focus_custom_chain_get(evas.c_evas.const_Evas_Object *obj)
    void         elm_object_focus_custom_chain_append(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *child, evas.c_evas.Evas_Object *relative_child)
    void         elm_object_focus_custom_chain_prepend(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *child, evas.c_evas.Evas_Object *relative_child)

    # Window handling
    evas.c_evas.Evas_Object *elm_win_add(evas.c_evas.Evas_Object* parent,char* name,Elm_Win_Type type)
    void elm_win_resize_object_add(evas.c_evas.Evas_Object* obj,evas.c_evas.Evas_Object* subobj)
    void elm_win_resize_object_del(evas.c_evas.Evas_Object* obj,evas.c_evas.Evas_Object* subobj)
    void elm_win_title_set(evas.c_evas.Evas_Object* obj,char *title)
    void elm_win_autodel_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool autodel)
    void elm_win_activate(evas.c_evas.Evas_Object *obj)
    void elm_win_lower(evas.c_evas.Evas_Object *obj)
    void elm_win_raise(evas.c_evas.Evas_Object *obj)
    void elm_win_borderless_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool borderless)
    evas.c_evas.Eina_Bool elm_win_borderless_get(evas.c_evas.Evas_Object *obj)
    void elm_win_shaped_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool shaped)
    evas.c_evas.Eina_Bool elm_win_shaped_get(evas.c_evas.Evas_Object *obj)
    void elm_win_alpha_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool alpha)
    evas.c_evas.Eina_Bool elm_win_alpha_get(evas.c_evas.Evas_Object *obj)
    void elm_win_override_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool override)
    evas.c_evas.Eina_Bool elm_win_override_get(evas.c_evas.Evas_Object *obj)
    void elm_win_fullscreen_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool fullscreen)
    evas.c_evas.Eina_Bool elm_win_fullscreen_get(evas.c_evas.Evas_Object *obj)
    void elm_win_maximized_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool maximized)
    evas.c_evas.Eina_Bool elm_win_maximized_get(evas.c_evas.Evas_Object *obj)
    void elm_win_iconified_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool iconified)
    evas.c_evas.Eina_Bool elm_win_iconified_get(evas.c_evas.Evas_Object *obj)
    void elm_win_layer_set(evas.c_evas.Evas_Object *obj, int layer)
    int elm_win_layer_get(evas.c_evas.Evas_Object *obj)
    void elm_win_rotation_set(evas.c_evas.Evas_Object *obj, int rotation)
    int elm_win_rotation_get(evas.c_evas.Evas_Object *obj)
    void elm_win_sticky_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool sticky)
    evas.c_evas.Eina_Bool elm_win_sticky_get(evas.c_evas.Evas_Object *obj)


    void elm_win_keyboard_mode_set(evas.c_evas.Evas_Object *obj, Elm_Win_Keyboard_Mode mode)
    void elm_win_keyboard_win_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool is_keyboard)

    void elm_win_focus_highlight_enabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool enabled)
    evas.c_evas.Eina_Bool elm_win_focus_highlight_enabled_get(evas.c_evas.Evas_Object *obj)

    # X specific call - wont't work on non-x engines (return 0)
    Ecore_X_Window elm_win_xwindow_get(evas.c_evas.Evas_Object *obj)

    # Inwin object
    evas.c_evas.Evas_Object *elm_win_inwin_add(evas.c_evas.Evas_Object *obj)
    void elm_win_inwin_activate(evas.c_evas.Evas_Object *obj)
    void elm_win_inwin_content_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)

    # Background object
    evas.c_evas.Evas_Object *elm_bg_add(evas.c_evas.Evas_Object *parent)
    void elm_bg_file_set(evas.c_evas.Evas_Object *obj, char *file, char *group)

    # Icon object
    evas.c_evas.Evas_Object *elm_icon_add(evas.c_evas.Evas_Object *parent)
    void elm_icon_file_set(evas.c_evas.Evas_Object *obj, char *file, char *group)
    void elm_icon_standard_set(evas.c_evas.Evas_Object *obj, char* name)
    void elm_icon_smooth_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool smooth)
    void elm_icon_no_scale_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool no_scale)
    void elm_icon_scale_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool scale_up, evas.c_evas.Eina_Bool scale_down)
    void elm_icon_fill_outside_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool fill_outside)
    void elm_icon_prescale_set(evas.c_evas.Evas_Object *obj, int size)

    # Image object
    evas.c_evas.Evas_Object* elm_image_add(evas.c_evas.Evas_Object *parent)
    evas.c_evas.Eina_Bool    elm_image_file_set(evas.c_evas.Evas_Object *obj, char *file, char *group)
    void                     elm_image_smooth_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool smooth)
    void                     elm_image_no_scale_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool no_scale)
    void                     elm_image_scale_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool scale_up,evas.c_evas.Eina_Bool scale_down)
    void                     elm_image_fill_outside_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool fill_outside)
    void                     elm_image_prescale_set(evas.c_evas.Evas_Object *obj, int size)
    void                     elm_image_orient_set(evas.c_evas.Evas_Object *obj, Elm_Image_Orient orient)

    # Box object
    evas.c_evas.Evas_Object *elm_box_add(evas.c_evas.Evas_Object *parent)
    void elm_box_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool horizontal)
    void elm_box_homogeneous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool homogeneous)
    void elm_box_homogenous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool homogenous)
    void elm_box_pack_start(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj)
    void elm_box_pack_end(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj)
    void elm_box_pack_before(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj, evas.c_evas.Evas_Object *before)
    void elm_box_pack_after(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj, evas.c_evas.Evas_Object *after)
    void elm_box_clear(evas.c_evas.Evas_Object *obj)
    void elm_box_unpack(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj)
    void elm_box_unpack_all(evas.c_evas.Evas_Object *obj)

    # Button object
    evas.c_evas.Evas_Object *elm_button_add(evas.c_evas.Evas_Object *parent)
    void elm_button_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    evas.c_evas.Evas_Object *elm_button_icon_get(evas.c_evas.Evas_Object *obj)

    # Scroller object
    evas.c_evas.Evas_Object *elm_scroller_add(evas.c_evas.Evas_Object *parent)
    void elm_scroller_content_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *child)
    void elm_scroller_content_min_limit(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool w, evas.c_evas.Eina_Bool h)
    void elm_scroller_region_show(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord x, evas.c_evas.Evas_Coord y, evas.c_evas.Evas_Coord w, evas. c_evas.Evas_Coord h)
    void elm_scroller_policy_set(evas.c_evas.Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v)
    void elm_scroller_region_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *x, evas.c_evas.Evas_Coord *y, evas.c_evas.Evas_Coord *w, evas.c_evas.Evas_Coord *h)
    void elm_scroller_child_size_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *w, evas.c_evas.Evas_Coord *h)
    void elm_scroller_bounce_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool h_bounce, evas.c_evas.Eina_Bool v_bounce)
    void elm_scroller_page_relative_set(evas.c_evas.Evas_Object *obj, double h_pagerel, double v_pagerel)
    void elm_scroller_page_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord h_pagesize, evas.c_evas.Evas_Coord v_pagesize)
    void elm_scroller_region_bring_in(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord x, evas.c_evas.Evas_Coord y, evas.c_evas.Evas_Coord w, evas.c_evas.Evas_Coord h)

    # Label object
    evas.c_evas.Evas_Object *elm_label_add(evas.c_evas.Evas_Object *parent)
    void elm_label_fontsize_set(evas.c_evas.Evas_Object *obj, int fontsize)
    void elm_label_text_align_set(evas.c_evas.Evas_Object *obj, char *alignmode)
    void elm_label_text_color_set(evas.c_evas.Evas_Object *obj, unsigned int r, unsigned int g, unsigned int b, unsigned int a)

    # Toggle object
    evas.c_evas.Evas_Object *elm_toggle_add(evas.c_evas.Evas_Object *parent)
    void elm_toggle_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    void elm_toggle_states_labels_set(evas.c_evas.Evas_Object *obj, char *onlabel, char *offlabel)
    void elm_toggle_state_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool state)
    evas.c_evas.Eina_Bool    elm_toggle_state_get(evas.c_evas.Evas_Object *obj)
    void elm_toggle_state_pointer_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool *statep)

    # Frame object
    evas.c_evas.Evas_Object* elm_frame_add(evas.c_evas.Evas_Object *parent)
    void elm_frame_content_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)

    # Flip object
    evas.c_evas.Evas_Object* elm_flip_add(evas.c_evas.Evas_Object *parent)
    void elm_flip_content_front_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)
    void elm_flip_content_back_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)
    void elm_flip_go(evas.c_evas.Evas_Object *obj, Elm_Flip_Mode mode)
    void elm_flip_go(evas.c_evas.Evas_Object *obj, Elm_Flip_Mode mode)
    evas.c_evas.Eina_Bool elm_flip_front_get(evas.c_evas.Evas_Object *obj)
    void elm_flip_perspective_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord foc, evas.c_evas.Evas_Coord x, evas.c_evas.Evas_Coord y)

    # Table object
    evas.c_evas.Evas_Object *elm_table_add(evas.c_evas.Evas_Object *parent)
    void elm_table_homogeneous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool homogeneous)
    void elm_table_homogenous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool homogenous)
    void elm_table_padding_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord horizontal, evas.c_evas.Evas_Coord vertical)
    void elm_table_pack(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj, int x, int y, int w, int h)
    void elm_table_unpack(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj)
    void elm_table_clear(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool clear)

    # Clock object
    evas.c_evas.Evas_Object *elm_clock_add(evas.c_evas.Evas_Object *parent)
    void elm_clock_time_set(evas.c_evas.Evas_Object *obj, int hrs, int min, int sec)
    void elm_clock_time_get(evas.c_evas.Evas_Object *obj, int *hrs, int *min, int *sec)
    void elm_clock_edit_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool edit)
    void elm_clock_show_am_pm_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool am_pm)
    void elm_clock_show_seconds_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool seconds)

    # Layout object
    evas.c_evas.Evas_Object *elm_layout_add(evas.c_evas.Evas_Object *parent)
    evas.c_evas.Eina_Bool elm_layout_file_set(evas.c_evas.Evas_Object *obj, char *file, char *group)
    evas.c_evas.Eina_Bool elm_layout_theme_set(evas.c_evas.Evas_Object *obj, char *clas, char *group, char *style)
    void elm_layout_content_set(evas.c_evas.Evas_Object *obj, char *swallow, evas.c_evas.Evas_Object *content)
    evas.c_evas.Evas_Object *elm_layout_content_get(evas.c_evas.Evas_Object *obj, char *swallow)
    evas.c_evas.Evas_Object *elm_layout_content_unset(evas.c_evas.Evas_Object *obj, char *swallow)
    evas.c_evas.Evas_Object *elm_layout_edje_get(evas.c_evas.Evas_Object *obj)
    void elm_layout_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    evas.c_evas.Evas_Object *elm_layout_icon_get(evas.c_evas.Evas_Object *obj)
    void elm_layout_end_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *end)
    evas.c_evas.Evas_Object *elm_layout_end_get(evas.c_evas.Evas_Object *obj)
    void elm_layout_sizing_eval(evas.c_evas.Evas_Object *obj)

    # Notify object
    evas.c_evas.Evas_Object *elm_notify_add(evas.c_evas.Evas_Object *parent)
    void elm_notify_content_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)
    evas.c_evas.Evas_Object *elm_notify_content_unset(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Evas_Object *elm_notify_content_get(evas.c_evas.Evas_Object *obj)
    void elm_notify_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    void elm_notify_orient_set(evas.c_evas.Evas_Object *obj, int orient)
    int elm_notify_orient_get(evas.c_evas.Evas_Object *obj)
    void elm_notify_timeout_set(evas.c_evas.Evas_Object *obj, double timeout)
    double elm_notify_timeout_get(evas.c_evas.Evas_Object *obj)
    void elm_notify_repeat_events_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool repeat)
    evas.c_evas.Eina_Bool elm_notify_repeat_events_get(evas.c_evas.Evas_Object *obj)

    # Hover object
    evas.c_evas.Evas_Object *elm_hover_add(evas.c_evas.Evas_Object *parent)
    void elm_hover_target_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *target)
    void elm_hover_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    void elm_hover_content_set(evas.c_evas.Evas_Object *obj, char *swallow, evas.c_evas.Evas_Object *content)
    void elm_hover_style_set(evas.c_evas.Evas_Object *obj, char *style)
    char  *elm_hover_best_content_location_get(evas.c_evas.Evas_Object *obj, Elm_Hover_Axis pref_axis)

    # Entry Object
    evas.c_evas.Evas_Object *elm_entry_add(evas.c_evas.Evas_Object *parent)
    void elm_entry_single_line_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool single_line)
    void elm_entry_password_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool password)
    void elm_entry_entry_set(evas.c_evas.Evas_Object *obj, char *entry)
    char *elm_entry_entry_get(evas.c_evas.Evas_Object *obj)
    char *elm_entry_selection_get(evas.c_evas.Evas_Object *obj)
    void elm_entry_entry_insert(evas.c_evas.Evas_Object *obj, char *entry)
    void elm_entry_line_wrap_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool wrap)
    void elm_entry_editable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool editable)
    void elm_entry_select_none(evas.c_evas.Evas_Object *obj)
    void elm_entry_select_all(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool elm_entry_cursor_next(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool elm_entry_cursor_prev(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool elm_entry_cursor_up(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool elm_entry_cursor_down(evas.c_evas.Evas_Object *obj)
    void elm_entry_cursor_begin_set(evas.c_evas.Evas_Object *obj)
    void elm_entry_cursor_end_set(evas.c_evas.Evas_Object *obj)
    void elm_entry_cursor_line_begin_set(evas.c_evas.Evas_Object *obj)
    void elm_entry_cursor_line_end_set(evas.c_evas.Evas_Object *obj)
    void elm_entry_cursor_selection_begin(evas.c_evas.Evas_Object *obj)
    void elm_entry_cursor_selection_end(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool elm_entry_cursor_is_format_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool elm_entry_cursor_is_visible_format_get(evas.c_evas.Evas_Object *obj)
    char *elm_entry_cursor_content_get(evas.c_evas.Evas_Object *obj)
    void elm_entry_selection_cut(evas.c_evas.Evas_Object *obj)
    void elm_entry_selection_copy(evas.c_evas.Evas_Object *obj)
    void elm_entry_selection_paste(evas.c_evas.Evas_Object *obj)
    char        *elm_entry_markup_to_utf8(char *s)
    char        *elm_entry_utf8_to_markup(char *s)
    void elm_entry_context_menu_disabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool disabled)

    # Scrolled Entry Object
    evas.c_evas.Evas_Object *elm_scrolled_entry_add(evas.c_evas.Evas_Object *parent)
    void         elm_scrolled_entry_single_line_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool single_line)
    evas.c_evas.Eina_Bool    elm_scrolled_entry_single_line_get(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_password_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool password)
    evas.c_evas.Eina_Bool    elm_scrolled_entry_password_get(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_entry_set(evas.c_evas.Evas_Object *obj, char *entry)
    char  *elm_scrolled_entry_entry_get(evas.c_evas.Evas_Object *obj)
    char  *elm_scrolled_entry_selection_get(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_entry_insert(evas.c_evas.Evas_Object *obj, char *entry)
    void         elm_scrolled_entry_line_wrap_set(evas.c_evas.Evas_Object *obj, Elm_Wrap_Type wrap_type)
    void         elm_scrolled_entry_editable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool editable)
    evas.c_evas.Eina_Bool    elm_scrolled_entry_editable_get(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_select_none(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_select_all(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_scrolled_entry_cursor_next(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_scrolled_entry_cursor_prev(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_scrolled_entry_cursor_up(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_scrolled_entry_cursor_down(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_cursor_begin_set(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_cursor_end_set(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_cursor_line_begin_set(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_cursor_line_end_set(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_cursor_selection_begin(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_cursor_selection_end(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_scrolled_entry_cursor_is_format_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_scrolled_entry_cursor_is_visible_format_get(evas.c_evas.Evas_Object *obj)
    char  *elm_scrolled_entry_cursor_content_get(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_selection_cut(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_selection_copy(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_selection_paste(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_context_menu_clear(evas.c_evas.Evas_Object *obj)
    #TODO: void         elm_scrolled_entry_context_menu_item_add(evas.c_evas.Evas_Object *obj, char *label, char *icon_file, Elm_Icon_Type icon_type, Evas_Smart_Cb func, void *data)
    void         elm_scrolled_entry_context_menu_disabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool disabled)
    evas.c_evas.Eina_Bool    elm_scrolled_entry_context_menu_disabled_get(evas.c_evas.Evas_Object *obj)
    void         elm_scrolled_entry_scrollbar_policy_set(evas.c_evas.Evas_Object *obj, Elm_Scroller_Policy h, Elm_Scroller_Policy v)
    void         elm_scrolled_entry_bounce_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool h_bounce, evas.c_evas.Eina_Bool v_bounce)

    # Composite objects

    # Anchorview object
    evas.c_evas.Evas_Object *elm_anchorview_add(evas.c_evas.Evas_Object *parent)
    void elm_anchorview_hover_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    void elm_anchorview_hover_style_set(evas.c_evas.Evas_Object *obj, char *style)
    void elm_anchorview_hover_end(evas.c_evas.Evas_Object *obj)
    void elm_anchorview_bounce_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool h_bounce, evas.c_evas.Eina_Bool v_bounce)

    # Anchorblock object
    evas.c_evas.Evas_Object *elm_anchorblock_add(evas.c_evas.Evas_Object *parent)
    void                     elm_anchorblock_hover_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    evas.c_evas.Evas_Object *elm_anchorblock_hover_parent_get(evas.c_evas.Evas_Object *obj)
    void                     elm_anchorblock_hover_style_set(evas.c_evas.Evas_Object *obj, char *style)
    void                     elm_anchorblock_hover_end(evas.c_evas.Evas_Object *obj)

    # Bubble object
    evas.c_evas.Evas_Object *elm_bubble_add(evas.c_evas.Evas_Object *parent)
    void elm_bubble_content_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)
    void elm_bubble_icon_set(evas.c_evas.Evas_Object *pbj, evas.c_evas.Evas_Object *icon)
    evas.c_evas.Evas_Object *elm_bubble_icon_get(evas.c_evas.Evas_Object *obj)
    void elm_bubble_corner_set(evas.c_evas.Evas_Object *obj, char *corner)

    # Photo object
    evas.c_evas.Evas_Object *elm_photo_add(evas.c_evas.Evas_Object *parent)
    void elm_photo_file_set(evas.c_evas.Evas_Object *obj, char *file)
    void elm_photo_size_set(evas.c_evas.Evas_Object *obj, int size)

    # Hoversel object
    evas.c_evas.Evas_Object *elm_hoversel_add(evas.c_evas.Evas_Object *parent)
    void elm_hoversel_hover_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object
            *parent)
    void elm_hoversel_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    evas.c_evas.Evas_Object *elm_hoversel_icon_get(evas.c_evas.Evas_Object *obj)
    void elm_hoversel_hover_begin(evas.c_evas.Evas_Object *obj)
    void elm_hoversel_hover_end(evas.c_evas.Evas_Object *obj)
    void elm_hoversel_clear(evas.c_evas.Evas_Object *obj)
    Elm_Hoversel_Item *elm_hoversel_item_add(evas.c_evas.Evas_Object *obj, char *label, char
            *icon_file, Elm_Icon_Type icon_type, void (*func)(void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    void elm_hoversel_item_del(Elm_Hoversel_Item *item)
    void* elm_hoversel_item_data_get(Elm_Hoversel_Item *it)
    char *elm_hoversel_item_label_get(Elm_Hoversel_Item *it)
    void elm_hoversel_item_del_cb_set(Elm_Hoversel_Item *item, void (*func)(void *data, evas.c_evas.Evas_Object *obj, void *event_info))
    void elm_hoversel_item_icon_set(Elm_Hoversel_Item *it, char *icon_file,
                                    char *icon_group, Elm_Icon_Type icon_type)
    void elm_hoversel_item_icon_get(Elm_Hoversel_Item *it, char **icon_file, char **icon_group, Elm_Icon_Type *icon_type)

    # Toolbar object
    evas.c_evas.Evas_Object *elm_toolbar_add(evas.c_evas.Evas_Object *parent)
    void elm_toolbar_icon_size_set(evas.c_evas.Evas_Object *obj, int icon_size)
    int elm_toolbar_icon_size_get(evas.c_evas.Evas_Object *obj)
    Elm_Toolbar_Item *elm_toolbar_item_append(evas.c_evas.Evas_Object *obj, char *icon, char *label, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Toolbar_Item *elm_toolbar_item_prepend(evas.c_evas.Evas_Object *obj, char *icon, char *label, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Toolbar_Item *elm_toolbar_item_insert_before(evas.c_evas.Evas_Object *obj, Elm_Toolbar_Item *before, char *icon, char *label, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Toolbar_Item *elm_toolbar_item_insert_after(evas.c_evas.Evas_Object *obj, Elm_Toolbar_Item *after, char *icon, char *label, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Toolbar_Item *elm_toolbar_first_item_get(evas.c_evas.Evas_Object *obj)
    Elm_Toolbar_Item *elm_toolbar_last_item_get(evas.c_evas.Evas_Object *obj)
    Elm_Toolbar_Item *elm_toolbar_item_next_get(Elm_Toolbar_Item *item)
    Elm_Toolbar_Item *elm_toolbar_item_prev_get(Elm_Toolbar_Item *item)
    char *elm_toolbar_item_icon_get(Elm_Toolbar_Item *item)
    char *elm_toolbar_item_label_get(Elm_Toolbar_Item *item)
    void  elm_toolbar_item_label_set(Elm_Toolbar_Item *item, char *label)
    void *elm_toolbar_item_data_get(Elm_Toolbar_Item *item)
    void  elm_toolbar_item_data_set(Elm_Toolbar_Item *item, void *data)
    void  elm_toolbar_item_del(Elm_Toolbar_Item *item)
    void  elm_toolbar_item_del_cb_set(Elm_Toolbar_Item *it, void (*func)(void *data, evas.c_evas.Evas_Object *obj, void *event_info))
    void  elm_toolbar_item_selected_set(Elm_Toolbar_Item *item, evas.c_evas.Eina_Bool selected)
    evas.c_evas.Eina_Bool elm_toolbar_item_selected_get(Elm_Toolbar_Item *item)
    evas.c_evas.Eina_Bool elm_toolbar_item_disabled_get(Elm_Toolbar_Item *item)
    Elm_Toolbar_Item *elm_toolbar_selected_item_get(evas.c_evas.Evas_Object *obj)
    void  elm_toolbar_item_disabled_set(Elm_Toolbar_Item *item, evas.c_evas.Eina_Bool disabled)
    void  elm_toolbar_item_separator_set(Elm_Toolbar_Item *item, evas.c_evas.Eina_Bool separator)
    evas.c_evas.Eina_Bool elm_toolbar_item_separator_get(Elm_Toolbar_Item *item)
    evas.c_evas.Eina_Bool  elm_toolbar_no_select_mode_get(evas.c_evas.Evas_Object *obj)
    void  elm_toolbar_no_select_mode_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool setting)
    void  elm_toolbar_homogeneous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool homogeneous)
    void  elm_toolbar_homogenous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool homogenous)
    void  elm_toolbar_menu_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    void  elm_toolbar_align_set(evas.c_evas.Evas_Object *obj, double align)
    void  elm_toolbar_item_menu_set(Elm_Toolbar_Item *item, evas.c_evas.Eina_Bool menu)
    evas.c_evas.Evas_Object *elm_toolbar_item_menu_get(Elm_Toolbar_Item *item)
    void         elm_toolbar_item_tooltip_text_set(Elm_Toolbar_Item *item, char *text)
    void         elm_toolbar_item_tooltip_content_cb_set(Elm_Toolbar_Item *item, Elm_Tooltip_Item_Content_Cb func, void *data, evas.c_evas.Evas_Smart_Cb del_cb)
    void         elm_toolbar_item_tooltip_unset(Elm_Toolbar_Item *item)
    void         elm_toolbar_item_tooltip_style_set(Elm_Toolbar_Item *item, char *style)
    char*        elm_toolbar_item_tooltip_style_get(Elm_Toolbar_Item *item)
    void         elm_toolbar_item_cursor_set(Elm_Toolbar_Item *item, char *cursor)
    void         elm_toolbar_item_cursor_unset(Elm_Toolbar_Item *item)
    void         elm_toolbar_item_cursor_style_set(Elm_Toolbar_Item *item, char *style)
    char*        elm_toolbar_item_cursor_style_get(Elm_Toolbar_Item *item)
    void         elm_toolbar_item_cursor_engine_only_set(Elm_Toolbar_Item *item, evas.c_evas.Eina_Bool engine_only)
    evas.c_evas.Eina_Bool elm_toolbar_item_cursor_engine_only_get(Elm_Toolbar_Item *item)

    # List object
    evas.c_evas.Evas_Object *elm_list_add(evas.c_evas.Evas_Object *parent)
    Elm_List_Item *elm_list_item_append(evas.c_evas.Evas_Object *obj, char *label, evas.c_evas.Evas_Object *icon, evas.c_evas.Evas_Object *end, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_List_Item *elm_list_item_prepend(evas.c_evas.Evas_Object *obj, char *label, evas.c_evas.Evas_Object *icon, evas.c_evas.Evas_Object *end, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_List_Item *elm_list_item_insert_before(evas.c_evas.Evas_Object *obj, Elm_List_Item *before, char *label, evas.c_evas.Evas_Object *icon, evas.c_evas.Evas_Object *end, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_List_Item *elm_list_item_insert_after(evas.c_evas.Evas_Object *obj, Elm_List_Item *after, char *label, evas.c_evas.Evas_Object *icon, evas.c_evas.Evas_Object *end, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    void        elm_list_clear(evas.c_evas.Evas_Object *obj)
    void         elm_list_go(evas.c_evas.Evas_Object *obj)
    void      elm_list_multi_select_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool multi)
    evas.c_evas.Eina_Bool      elm_list_multi_select_get(evas.c_evas.Evas_Object *obj)
    void         elm_list_mode_set(evas.c_evas.Evas_Object *obj, Elm_List_Mode mode)
    Elm_List_Mode       elm_list_mode_get(evas.c_evas.Evas_Object *obj)
    void         elm_list_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool horizontal)
    evas.c_evas.Eina_Bool       elm_list_horizontal_get(evas.c_evas.Evas_Object *obj)
    void      elm_list_always_select_mode_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool multi)
    evas.c_evas.Eina_List     *elm_list_items_get(evas.c_evas.Evas_Object *obj)
    Elm_List_Item *elm_list_selected_item_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_List     *elm_list_selected_items_get(evas.c_evas.Evas_Object *obj)
    void         elm_list_item_selected_set(Elm_List_Item *item, evas.c_evas.Eina_Bool selected)
    evas.c_evas.Eina_Bool      elm_list_item_selected_get(Elm_List_Item *item)
    void         elm_list_item_show(Elm_List_Item *item)
    void         elm_list_item_del(Elm_List_Item *item)
    void         elm_list_item_del_cb_set(Elm_List_Item *item, void (*func)(void *data, evas.c_evas.Evas_Object *obj, void *event_info))
    void  *elm_list_item_data_get(Elm_List_Item *item)
    evas.c_evas.Evas_Object *elm_list_item_icon_get(Elm_List_Item *item)
    evas.c_evas.Evas_Object *elm_list_item_end_get(Elm_List_Item *item)
    evas.c_evas.Evas_Object *elm_list_item_object_get(Elm_List_Item *item)
    char* elm_list_item_label_get(Elm_List_Item *item)
    void elm_list_item_label_set(Elm_List_Item *item, char* text)
    Elm_List_Item *elm_list_item_prev(Elm_List_Item *it)
    Elm_List_Item *elm_list_item_next(Elm_List_Item *it)
    void         elm_list_item_tooltip_text_set(Elm_List_Item *item, char *text)
    void         elm_list_item_tooltip_content_cb_set(Elm_List_Item *item, Elm_Tooltip_Item_Content_Cb func, void *data, evas.c_evas.Evas_Smart_Cb del_cb)
    void         elm_list_item_tooltip_unset(Elm_List_Item *item)
    void         elm_list_item_tooltip_style_set(Elm_List_Item *item, char *style)
    char*        elm_list_item_tooltip_style_get(Elm_List_Item *item)
    void         elm_list_item_cursor_set(Elm_List_Item *item, char *cursor)
    void         elm_list_item_cursor_unset(Elm_List_Item *item)
    void         elm_list_item_cursor_style_set(Elm_List_Item *item, char *style)
    char*        elm_list_item_cursor_style_get(Elm_List_Item *item)
    void         elm_list_item_cursor_engine_only_set(Elm_List_Item *item, evas.c_evas.Eina_Bool engine_only)
    evas.c_evas.Eina_Bool elm_list_item_cursor_engine_only_get(Elm_List_Item *item)
    void elm_list_bounce_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool h_bounce, evas.c_evas.Eina_Bool v_bounce)
    void 	elm_list_scroller_policy_set(evas.c_evas.Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v)


    # Carousel object
    evas.c_evas.Evas_Object *elm_carousel_add(evas.c_evas.Evas_Object *parent)
    Elm_Carousel_Item *elm_carousel_item_add(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon, char *label, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    void         elm_carousel_item_del(Elm_Carousel_Item *item)
    void         elm_carousel_item_select(Elm_Carousel_Item *item)

    # Slider object
    evas.c_evas.Evas_Object *elm_slider_add(evas.c_evas.Evas_Object *parent)
    void elm_slider_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    void elm_slider_span_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord size)
    void elm_slider_unit_format_set(evas.c_evas.Evas_Object *obj, char *format)
    void elm_slider_indicator_format_set(evas.c_evas.Evas_Object *obj, char *indicator)
    void elm_slider_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool horizontal)
    void elm_slider_min_max_set(evas.c_evas.Evas_Object *obj, double min, double max)
    void elm_slider_value_set(evas.c_evas.Evas_Object *obj, double val)
    double elm_slider_value_get(evas.c_evas.Evas_Object *obj)
    void elm_slider_inverted_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool inverted)

    # Generic List
    evas.c_evas.Evas_Object *elm_genlist_add(evas.c_evas.Evas_Object *parent)
    void elm_genlist_clear(evas.c_evas.Evas_Object *obj)
    void elm_genlist_multi_select_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool multi)
    void elm_genlist_horizontal_set(evas.c_evas.Evas_Object *obj, Elm_List_Mode mode)
    Elm_List_Mode elm_genlist_horizontal_get(evas.c_evas.Evas_Object *obj)
    void elm_genlist_always_select_mode_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool always_select)
    void elm_genlist_no_select_mode_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool no_select)
    void elm_genlist_compress_mode_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool compress)
    void elm_genlist_bounce_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool h_bounce, evas.c_evas.Eina_Bool v_bounce)
    void elm_genlist_homogeneous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool homogeneous)
    void elm_genlist_block_count_set(evas.c_evas.Evas_Object *obj, int n)
    Elm_Genlist_Item *elm_genlist_item_append(evas.c_evas.Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item_Flags flags, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Genlist_Item *elm_genlist_item_prepend(evas.c_evas.Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item_Flags flags, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Genlist_Item *elm_genlist_item_insert_before(evas.c_evas.Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item *before, Elm_Genlist_Item_Flags flags, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Genlist_Item *elm_genlist_item_insert_after(evas.c_evas.Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Genlist_Item *parent, Elm_Genlist_Item *after, Elm_Genlist_Item_Flags flags, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Genlist_Item *elm_genlist_selected_item_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_List *elm_genlist_selected_items_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_List *elm_genlist_realized_items_get(evas.c_evas.Evas_Object *obj)
    Elm_Genlist_Item *elm_genlist_at_xy_item_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord x, evas.c_evas.Evas_Coord y, int *posret)
    Elm_Genlist_Item *elm_genlist_first_item_get(evas.c_evas.Evas_Object *obj)
    Elm_Genlist_Item *elm_genlist_last_item_get(evas.c_evas.Evas_Object *obj)

    Elm_Genlist_Item *elm_genlist_item_next_get(Elm_Genlist_Item *item)
    Elm_Genlist_Item *elm_genlist_item_prev_get(Elm_Genlist_Item *item)
    evas.c_evas.Evas_Object *elm_genlist_item_genlist_get(Elm_Genlist_Item *item)
    Elm_Genlist_Item *elm_genlist_item_parent_get(Elm_Genlist_Item *it)
    void elm_genlist_item_subitems_clear(Elm_Genlist_Item *item)
    void elm_genlist_item_selected_set(Elm_Genlist_Item *item, evas.c_evas.Eina_Bool selected)
    evas.c_evas.Eina_Bool elm_genlist_item_selected_get(Elm_Genlist_Item *item)
    void elm_genlist_item_expanded_set(Elm_Genlist_Item *item, evas.c_evas.Eina_Bool expanded)
    evas.c_evas.Eina_Bool elm_genlist_item_expanded_get(Elm_Genlist_Item *item)
    void elm_genlist_item_disabled_set(Elm_Genlist_Item *item, evas.c_evas.Eina_Bool disabled)
    evas.c_evas.Eina_Bool elm_genlist_item_disabled_get(Elm_Genlist_Item *item)
    void elm_genlist_item_display_only_set(Elm_Genlist_Item *it, evas.c_evas.Eina_Bool display_only)
    evas.c_evas.Eina_Bool elm_genlist_item_display_only_get(Elm_Genlist_Item *it)
    void elm_genlist_item_show(Elm_Genlist_Item *item)
    void elm_genlist_item_bring_in(Elm_Genlist_Item *item)
    void elm_genlist_item_top_show(Elm_Genlist_Item *item)
    void elm_genlist_item_top_bring_in(Elm_Genlist_Item *item)
    void elm_genlist_item_middle_show(Elm_Genlist_Item *it)
    void elm_genlist_item_middle_bring_in(Elm_Genlist_Item *it)
    void elm_genlist_item_del(Elm_Genlist_Item *item)
    void *elm_genlist_item_data_get(Elm_Genlist_Item *item)
    void elm_genlist_item_data_set(Elm_Genlist_Item *it, void *data)
    evas.c_evas.Evas_Object *elm_genlist_item_object_get(Elm_Genlist_Item *it)
    void elm_genlist_item_update(Elm_Genlist_Item *item)
    void         elm_genlist_item_tooltip_text_set(Elm_Genlist_Item *item, char *text)
    void         elm_genlist_item_tooltip_content_cb_set(Elm_Genlist_Item *item, Elm_Tooltip_Item_Content_Cb func, void *data, evas.c_evas.Evas_Smart_Cb del_cb)
    void         elm_genlist_item_tooltip_unset(Elm_Genlist_Item *item)
    void         elm_genlist_item_tooltip_style_set(Elm_Genlist_Item *item, char *style)
    char*        elm_genlist_item_tooltip_style_get(Elm_Genlist_Item *item)
    void         elm_genlist_item_cursor_set(Elm_Genlist_Item *item, char *cursor)
    void         elm_genlist_item_cursor_unset(Elm_Genlist_Item *item)
    void         elm_genlist_item_cursor_style_set(Elm_Genlist_Item *item, char *style)
    char*        elm_genlist_item_cursor_style_get(Elm_Genlist_Item *item)
    void         elm_genlist_item_cursor_engine_only_set(Elm_Genlist_Item *item, evas.c_evas.Eina_Bool engine_only)
    evas.c_evas.Eina_Bool elm_genlist_item_cursor_engine_only_get(Elm_Genlist_Item *item)

    # Generic Grid
    evas.c_evas.Evas_Object *elm_gengrid_add(evas.c_evas.Evas_Object *parent)
    void elm_gengrid_clear(evas.c_evas.Evas_Object *obj)
    void elm_gengrid_item_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord w, evas.c_evas.Evas_Coord h)
    void elm_gengrid_item_size_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *w, evas.c_evas.Evas_Coord *h)
    void elm_gengrid_align_set(evas.c_evas.Evas_Object *obj,  double align_x, double align_y)
    void elm_gengrid_align_get(evas.c_evas.Evas_Object *obj,  double *align_x, double *align_y)

    void elm_gengrid_multi_select_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool multi)
    evas.c_evas.Eina_Bool elm_gengrid_multi_select_get(evas.c_evas.Evas_Object *obj)
    void elm_gengrid_always_select_mode_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool always_select)
    evas.c_evas.Eina_Bool elm_gengrid_always_select_mode_get(evas.c_evas.Evas_Object *obj)
    void elm_gengrid_no_select_mode_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool no_select)
    evas.c_evas.Eina_Bool elm_gengrid_no_select_mode_get(evas.c_evas.Evas_Object *obj)
    void elm_gengrid_bounce_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool h_bounce, evas.c_evas.Eina_Bool v_bounce)
    void elm_gengrid_bounce_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool *h_bounce, evas.c_evas.Eina_Bool *v_bounce)
    void elm_gengrid_page_relative_set(evas.c_evas.Evas_Object *obj, double h_pagerel, double v_pagerel)
    void elm_gengrid_page_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord h_pagesize, evas.c_evas.Evas_Coord v_pagesize)
    void elm_gengrid_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool setting) 

    Elm_Gengrid_Item *elm_gengrid_item_append(evas.c_evas.Evas_Object *obj, Elm_Gengrid_Item_Class *itc, void *data, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Gengrid_Item *elm_gengrid_item_prepend(evas.c_evas.Evas_Object *obj, Elm_Gengrid_Item_Class *itc, void *data, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Gengrid_Item *elm_gengrid_item_insert_before(evas.c_evas.Evas_Object *obj, Elm_Gengrid_Item_Class *itc, void *data, Elm_Gengrid_Item *before, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Gengrid_Item *elm_gengrid_item_insert_after(evas.c_evas.Evas_Object *obj, Elm_Gengrid_Item_Class *itc, void *data, Elm_Gengrid_Item *after, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Gengrid_Item *elm_gengrid_selected_item_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_List *elm_gengrid_selected_items_get(evas.c_evas.Evas_Object *obj)
    Elm_Gengrid_Item *elm_gengrid_first_item_get(evas.c_evas.Evas_Object *obj)
    Elm_Gengrid_Item *elm_gengrid_last_item_get(evas.c_evas.Evas_Object *obj)

    Elm_Gengrid_Item *elm_gengrid_item_next_get(Elm_Gengrid_Item *item)
    Elm_Gengrid_Item *elm_gengrid_item_prev_get(Elm_Gengrid_Item *item)
    
    evas.c_evas.Evas_Object *elm_gengrid_item_gengrid_get(Elm_Gengrid_Item *item)
    void elm_gengrid_item_selected_set(Elm_Gengrid_Item *item, evas.c_evas.Eina_Bool selected)
    evas.c_evas.Eina_Bool elm_gengrid_item_selected_get(Elm_Gengrid_Item *item)
    void elm_gengrid_item_show(Elm_Gengrid_Item *item)
    void elm_gengrid_item_bring_in(Elm_Gengrid_Item *item)
    void elm_gengrid_item_del(Elm_Gengrid_Item *item)
    void *elm_gengrid_item_data_get(Elm_Gengrid_Item *item)
    void elm_gengrid_item_data_set(Elm_Gengrid_Item *it, void *data)
    evas.c_evas.Evas_Object *elm_gengrid_item_object_get(Elm_Gengrid_Item *it)
    void elm_gengrid_item_update(Elm_Gengrid_Item *item)
    void elm_gengrid_item_pos_get(Elm_Gengrid_Item *item, unsigned int *x, unsigned int *y)

    void  elm_gengrid_item_tooltip_text_set(Elm_Gengrid_Item *item, char *text)
    void  elm_gengrid_item_tooltip_content_cb_set(Elm_Gengrid_Item *item, Elm_Tooltip_Item_Content_Cb func, void *data, evas.c_evas.Evas_Smart_Cb del_cb)
    void  elm_gengrid_item_tooltip_unset(Elm_Gengrid_Item *item)
    void  elm_gengrid_item_tooltip_style_set(Elm_Gengrid_Item *item, char *style)
    char* elm_gengrid_item_tooltip_style_get(Elm_Gengrid_Item *item)
    void  elm_gengrid_item_cursor_set(Elm_Gengrid_Item *item, char *cursor)
    void  elm_gengrid_item_cursor_unset(Elm_Gengrid_Item *item)
    void  elm_gengrid_item_cursor_style_set(Elm_Gengrid_Item *item, char *style)
    char* elm_gengrid_item_cursor_style_get(Elm_Gengrid_Item *item)
    void  elm_gengrid_item_cursor_engine_only_set(Elm_Gengrid_Item *item, evas.c_evas.Eina_Bool engine_only)
    evas.c_evas.Eina_Bool elm_gengrid_item_cursor_engine_only_get(Elm_Gengrid_Item *item)


    # Check widget
    evas.c_evas.Evas_Object *elm_check_add(evas.c_evas.Evas_Object *parent)
    void         elm_check_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    evas.c_evas.Evas_Object *elm_check_icon_get(evas.c_evas.Evas_Object *obj)
    void         elm_check_state_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool state)
    evas.c_evas.Eina_Bool    elm_check_state_get(evas.c_evas.Evas_Object *obj)
    void         elm_check_state_pointer_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool *statep)

    # Radio widget
    evas.c_evas.Evas_Object *elm_radio_add(evas.c_evas.Evas_Object *parent)
    void         elm_radio_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    void         elm_radio_group_add(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *group)
    void         elm_radio_state_value_set(evas.c_evas.Evas_Object *obj, int value)
    void         elm_radio_value_set(evas.c_evas.Evas_Object *obj, int value)
    int          elm_radio_value_get(evas.c_evas.Evas_Object *obj)
    void         elm_radio_value_pointer_set(evas.c_evas.Evas_Object *obj, int *valuep)

    # Pager widget
    evas.c_evas.Evas_Object *elm_pager_add(evas.c_evas.Evas_Object *parent)
    void         elm_pager_content_push(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)
    void         elm_pager_content_pop(evas.c_evas.Evas_Object *obj)
    void         elm_pager_content_promote(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)
    evas.c_evas.Evas_Object *elm_pager_content_bottom_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Evas_Object *elm_pager_content_top_get(evas.c_evas.Evas_Object *obj)

    # Fileselector widget
    evas.c_evas.Evas_Object *elm_fileselector_add(evas.c_evas.Evas_Object *parent)
    void         elm_fileselector_is_save_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool is_save)
    evas.c_evas.Eina_Bool elm_fileselector_is_save_get(evas.c_evas.Evas_Object *obj)
    char        *elm_fileselector_selected_get(evas.c_evas.Evas_Object *obj)
    void         elm_fileselector_path_set(evas.c_evas.Evas_Object *obj, char *path)
    char        *elm_fileselector_path_get(evas.c_evas.Evas_Object *obj)
    void         elm_fileselector_expandable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool expand)

    # Progressbar widget
    evas.c_evas.Evas_Object *elm_progressbar_add(evas.c_evas.Evas_Object *parent)
    void         elm_progressbar_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    void         elm_progressbar_span_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord size)
    void         elm_progressbar_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool horizontal)
    void         elm_progressbar_inverted_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool inverted)
    void         elm_progressbar_pulse_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool pulse)
    void         elm_progressbar_pulse(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool state)
    void         elm_progressbar_unit_format_set(evas.c_evas.Evas_Object *obj, char *format)
    void         elm_progressbar_value_set(evas.c_evas.Evas_Object *obj, double val)
    double       elm_progressbar_value_get(evas.c_evas.Evas_Object *obj)

    # Separator
    evas.c_evas.Evas_Object *elm_separator_add(evas.c_evas.Evas_Object *parent)
    void elm_separator_horizontal_set(evas.c_evas.Evas_Object *obj,
                                      evas.c_evas.Eina_Bool)
    evas.c_evas.Eina_Bool elm_separator_horizontal_get(evas.c_evas.Evas_Object *obj)

    # Spinner
    evas.c_evas.Evas_Object *elm_spinner_add(evas.c_evas.Evas_Object *parent)
    void         elm_spinner_label_format_set(evas.c_evas.Evas_Object *obj, char *format)
    char        *elm_spinner_label_format_get(evas.c_evas.Evas_Object *obj)
    void         elm_spinner_min_max_set(evas.c_evas.Evas_Object *obj, double min, double max)
    void         elm_spinner_step_set(evas.c_evas.Evas_Object *obj, double step)
    void         elm_spinner_value_set(evas.c_evas.Evas_Object *obj, double val)
    double       elm_spinner_value_get(evas.c_evas.Evas_Object *obj)
    void         elm_spinner_wrap_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool wrap)


    # Menu object
    evas.c_evas.Evas_Object *elm_menu_add(evas.c_evas.Evas_Object *parent)
    void  elm_menu_move(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord x, evas.c_evas.Evas_Coord y)
    void  elm_menu_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    Elm_Menu_Item *elm_menu_item_add(evas.c_evas.Evas_Object *obj, Elm_Menu_Item *parent, char *icon, char *label, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Menu_Item *elm_menu_item_separator_add(evas.c_evas.Evas_Object *obj, Elm_Menu_Item *parent)
    void  elm_menu_item_label_set(Elm_Menu_Item *item, char *label)
    char *elm_menu_item_label_get(Elm_Menu_Item *item)
    void  elm_menu_item_object_icon_name_set(Elm_Menu_Item *item, char *icon)
    void  elm_menu_item_disabled_set(Elm_Menu_Item *item, evas.c_evas.Eina_Bool disabled)
    void  elm_menu_item_del(Elm_Menu_Item *item)
    void  elm_menu_item_del_cb_set(Elm_Menu_Item *it, void (*func)(void *data, evas.c_evas.Evas_Object *obj, void *event_info))
    void *elm_menu_item_data_get(Elm_Menu_Item *it)
    void  elm_menu_item_data_set(Elm_Menu_Item *item, void *data)
    evas.c_evas.Evas_Object *elm_menu_object_get(Elm_Menu_Item *it)
    evas.c_evas.Eina_List *elm_menu_item_subitems_get(Elm_Menu_Item *item)

    # Panel
    evas.c_evas.Evas_Object *elm_panel_add(evas.c_evas.Evas_Object *parent)
    void elm_panel_orient_set(evas.c_evas.Evas_Object *obj, Elm_Panel_Orient orient)
    Elm_Panel_Orient elm_panel_orient_get(evas.c_evas.Evas_Object *obj)
    void elm_panel_content_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)
    void elm_panel_hidden_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool hidden)
    evas.c_evas.Eina_Bool elm_panel_hidden_get(evas.c_evas.Evas_Object *obj)
    void elm_panel_toggle(evas.c_evas.Evas_Object *obj)
