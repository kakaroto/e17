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

    # enums
    ctypedef enum Elm_Bg_Option:
        ELM_BG_OPTION_CENTER
        ELM_BG_OPTION_SCALE
        ELM_BG_OPTION_STRETCH
        ELM_BG_OPTION_TILE
        ELM_BG_OPTION_LAST

    ctypedef enum Elm_Bubble_Pos:
        ELM_BUBBLE_POS_TOP_LEFT
        ELM_BUBBLE_POS_TOP_RIGHT
        ELM_BUBBLE_POS_BOTTOM_LEFT
        ELM_BUBBLE_POS_BOTTOM_RIGHT

    ctypedef enum Elm_Clock_Edit_Mode:
        ELM_CLOCK_EDIT_DEFAULT = 0
        ELM_CLOCK_EDIT_HOUR_DECIMAL = 1 << 0
        ELM_CLOCK_EDIT_HOUR_UNIT = 1 << 1
        ELM_CLOCK_EDIT_MIN_DECIMAL = 1 << 2
        ELM_CLOCK_EDIT_MIN_UNIT = 1 << 3
        ELM_CLOCK_EDIT_SEC_DECIMAL = 1 << 4
        ELM_CLOCK_EDIT_SEC_UNIT = 1 << 5
        ELM_CLOCK_EDIT_ALL = (1 << 6) - 1

    ctypedef enum Elm_Cnp_Mode:
        ELM_CNP_MODE_MARKUP     # copy & paste text with markup tag
        ELM_CNP_MODE_NO_IMAGE   # copy & paste text without item(image) tag
        ELM_CNP_MODE_PLAINTEXT  # copy & paste text without markup tag

    ctypedef enum Elm_Fileselector_Mode:
        ELM_FILESELECTOR_LIST
        ELM_FILESELECTOR_GRID

    ctypedef enum Elm_Flip_Direction:
        ELM_FLIP_DIRECTION_UP
        ELM_FLIP_DIRECTION_DOWN
        ELM_FLIP_DIRECTION_LEFT
        ELM_FLIP_DIRECTION_RIGHT

    ctypedef enum Elm_Flip_Interaction:
        ELM_FLIP_INTERACTION_NONE
        ELM_FLIP_INTERACTION_ROTATE
        ELM_FLIP_INTERACTION_CUBE
        ELM_FLIP_INTERACTION_PAGE

    ctypedef enum Elm_Flip_Mode:
        ELM_FLIP_ROTATE_Y_CENTER_AXIS
        ELM_FLIP_ROTATE_X_CENTER_AXIS
        ELM_FLIP_ROTATE_XZ_CENTER_AXIS
        ELM_FLIP_ROTATE_YZ_CENTER_AXIS
        ELM_FLIP_CUBE_LEFT
        ELM_FLIP_CUBE_RIGHT
        ELM_FLIP_CUBE_UP
        ELM_FLIP_CUBE_DOWN
        ELM_FLIP_PAGE_LEFT
        ELM_FLIP_PAGE_RIGHT
        ELM_FLIP_PAGE_UP
        ELM_FLIP_PAGE_DOWN

    ctypedef enum Elm_Genlist_Item_Type:
        ELM_GENLIST_ITEM_NONE
        ELM_GENLIST_ITEM_TREE
        ELM_GENLIST_ITEM_GROUP
        ELM_GENLIST_ITEM_MAX

    ctypedef enum Elm_Genlist_Item_Field_Type:
        ELM_GENLIST_ITEM_FIELD_ALL
        ELM_GENLIST_ITEM_FIELD_TEXT
        ELM_GENLIST_ITEM_FIELD_CONTENT
        ELM_GENLIST_ITEM_FIELD_STATE

    ctypedef enum Elm_Genlist_Item_Scrollto_Type:
        ELM_GENLIST_ITEM_SCROLLTO_NONE      # no scrollto
        ELM_GENLIST_ITEM_SCROLLTO_IN        # to the nearest viewport
        ELM_GENLIST_ITEM_SCROLLTO_TOP       # to the top of viewport
        ELM_GENLIST_ITEM_SCROLLTO_MIDDLE    # to the middle of viewport

    ctypedef enum Elm_Hover_Axis:
        ELM_HOVER_AXIS_NONE
        ELM_HOVER_AXIS_HORIZONTAL
        ELM_HOVER_AXIS_VERTICAL
        ELM_HOVER_AXIS_BOTH

    ctypedef enum Elm_Icon_Lookup_Order:
        ELM_ICON_LOOKUP_FDO_THEME
        ELM_ICON_LOOKUP_THEME_FDO
        ELM_ICON_LOOKUP_FDO
        ELM_ICON_LOOKUP_THEME

    ctypedef enum Elm_Icon_Type:
        ELM_ICON_NONE
        ELM_ICON_FILE
        ELM_ICON_STANDARD

    ctypedef enum Elm_Illume_Command:
        ELM_ILLUME_COMMAND_FOCUS_BACK
        ELM_ILLUME_COMMAND_FOCUS_FORWARD
        ELM_ILLUME_COMMAND_FOCUS_HOME
        ELM_ILLUME_COMMAND_CLOSE

    ctypedef enum Elm_Image_Orient:
        ELM_IMAGE_ORIENT_NONE
        ELM_IMAGE_ROTATE_90
        ELM_IMAGE_ROTATE_180
        ELM_IMAGE_ROTATE_270
        ELM_IMAGE_FLIP_HORIZONTAL
        ELM_IMAGE_FLIP_VERTICAL
        ELM_IMAGE_FLIP_TRANSPOSE
        ELM_IMAGE_FLIP_TRANSVERSE

    ctypedef enum Elm_Input_Panel_Lang:
        ELM_INPUT_PANEL_LANG_AUTOMATIC
        ELM_INPUT_PANEL_LANG_ALPHABET

    ctypedef enum Elm_Input_Panel_Layout:
        ELM_INPUT_PANEL_LAYOUT_NORMAL
        ELM_INPUT_PANEL_LAYOUT_NUMBER
        ELM_INPUT_PANEL_LAYOUT_EMAIL
        ELM_INPUT_PANEL_LAYOUT_URL
        ELM_INPUT_PANEL_LAYOUT_PHONENUMBER
        ELM_INPUT_PANEL_LAYOUT_IP
        ELM_INPUT_PANEL_LAYOUT_MONTH
        ELM_INPUT_PANEL_LAYOUT_NUMBERONLY
        ELM_INPUT_PANEL_LAYOUT_INVALID
        ELM_INPUT_PANEL_LAYOUT_HEX
        ELM_INPUT_PANEL_LAYOUT_TERMINAL
        ELM_INPUT_PANEL_LAYOUT_PASSWORD

    ctypedef enum Elm_Input_Panel_Return_Key_Type:
       ELM_INPUT_PANEL_RETURN_KEY_TYPE_DEFAULT
       ELM_INPUT_PANEL_RETURN_KEY_TYPE_DONE
       ELM_INPUT_PANEL_RETURN_KEY_TYPE_GO
       ELM_INPUT_PANEL_RETURN_KEY_TYPE_JOIN
       ELM_INPUT_PANEL_RETURN_KEY_TYPE_LOGIN
       ELM_INPUT_PANEL_RETURN_KEY_TYPE_NEXT
       ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEARCH
       ELM_INPUT_PANEL_RETURN_KEY_TYPE_SEND

    ctypedef enum Elm_List_Mode:
        ELM_LIST_COMPRESS
        ELM_LIST_SCROLL
        ELM_LIST_LIMIT
        ELM_LIST_EXPAND

    ctypedef enum Elm_Notify_Orient:
        ELM_NOTIFY_ORIENT_TOP
        ELM_NOTIFY_ORIENT_CENTER
        ELM_NOTIFY_ORIENT_BOTTOM
        ELM_NOTIFY_ORIENT_LEFT
        ELM_NOTIFY_ORIENT_RIGHT
        ELM_NOTIFY_ORIENT_TOP_LEFT
        ELM_NOTIFY_ORIENT_TOP_RIGHT
        ELM_NOTIFY_ORIENT_BOTTOM_LEFT
        ELM_NOTIFY_ORIENT_BOTTOM_RIGHT
        ELM_NOTIFY_ORIENT_LAST

    ctypedef enum Elm_Object_Select_Mode:
        ELM_OBJECT_SELECT_MODE_DEFAULT        # default select mode
        ELM_OBJECT_SELECT_MODE_ALWAYS         # always select mode
        ELM_OBJECT_SELECT_MODE_NONE           # no select mode
        ELM_OBJECT_SELECT_MODE_DISPLAY_ONLY   # no select mode with no finger size rule
        ELM_OBJECT_SELECT_MODE_MAX

    ctypedef enum Elm_Panel_Orient:
        ELM_PANEL_ORIENT_TOP
        ELM_PANEL_ORIENT_BOTTOM
        ELM_PANEL_ORIENT_LEFT
        ELM_PANEL_ORIENT_RIGHT

    ctypedef enum Elm_Policy:
        ELM_POLICY_QUIT
        ELM_POLICY_LAST

    ctypedef enum Elm_Policy_Quit:
        ELM_POLICY_QUIT_NONE
        ELM_POLICY_QUIT_LAST_WINDOW_CLOSED

    ctypedef enum Elm_Scroller_Policy:
        ELM_SCROLLER_POLICY_AUTO
        ELM_SCROLLER_POLICY_ON
        ELM_SCROLLER_POLICY_OFF

    ctypedef enum Elm_Text_Format:
        ELM_TEXT_FORMAT_PLAIN_UTF8
        ELM_TEXT_FORMAT_MARKUP_UTF8

    ctypedef enum Elm_Toolbar_Shrink_Mode:
        ELM_TOOLBAR_SHRINK_NONE
        ELM_TOOLBAR_SHRINK_HIDE
        ELM_TOOLBAR_SHRINK_SCROLL
        ELM_TOOLBAR_SHRINK_MENU
        ELM_TOOLBAR_SHRINK_EXPAND
        ELM_TOOLBAR_SHRINK_LAST

    ctypedef enum Elm_Web_Zoom_Mode:
        ELM_WEB_ZOOM_MODE_MANUAL 	#Zoom controlled normally by elm_web_zoom_set.
        ELM_WEB_ZOOM_MODE_AUTO_FIT 	#Zoom until content fits in web object
        ELM_WEB_ZOOM_MODE_AUTO_FILL	#Zoom until content fills web object.

    ctypedef enum Elm_Win_Type:
        ELM_WIN_BASIC
        ELM_WIN_DIALOG_BASIC
        ELM_WIN_DESKTOP
        ELM_WIN_DOCK
        ELM_WIN_TOOLBAR
        ELM_WIN_MENU
        ELM_WIN_UTILITY
        ELM_WIN_SPLASH
        ELM_WIN_DROPDOWN_MENU
        ELM_WIN_POPUP_MENU
        ELM_WIN_TOOLTIP
        ELM_WIN_NOTIFICATION
        ELM_WIN_COMBO
        ELM_WIN_DND
        ELM_WIN_INLINED_IMAGE
        ELM_WIN_SOCKET_IMAGE

    ctypedef enum Elm_Win_Indicator_Mode:
        ELM_WIN_INDICATOR_UNKNOWN
        ELM_WIN_INDICATOR_HIDE
        ELM_WIN_INDICATOR_SHOW

    ctypedef enum Elm_Win_Indicator_Opacity_Mode:
        ELM_WIN_INDICATOR_OPACITY_UNKNOWN
        ELM_WIN_INDICATOR_OPAQUE
        ELM_WIN_INDICATOR_TRANSLUCENT
        ELM_WIN_INDICATOR_TRANSPARENT

    ctypedef enum Elm_Win_Keyboard_Mode:
        ELM_WIN_KEYBOARD_UNKNOWN      # Unknown keyboard state
        ELM_WIN_KEYBOARD_OFF          # Request to deactivate the keyboard
        ELM_WIN_KEYBOARD_ON           # Enable keyboard with default layout
        ELM_WIN_KEYBOARD_ALPHA        # Alpha (a-z) keyboard layout
        ELM_WIN_KEYBOARD_NUMERIC      # Numeric keyboard layout
        ELM_WIN_KEYBOARD_PIN          # PIN keyboard layout
        ELM_WIN_KEYBOARD_PHONE_NUMBER # Phone keyboard layout
        ELM_WIN_KEYBOARD_HEX          # Hexadecimal numeric keyboard layout
        ELM_WIN_KEYBOARD_TERMINAL     # Full (QWERTY) keyboard layout
        ELM_WIN_KEYBOARD_PASSWORD     # Password keyboard layout
        ELM_WIN_KEYBOARD_IP           # IP keyboard layout
        ELM_WIN_KEYBOARD_HOST         # Host keyboard layout
        ELM_WIN_KEYBOARD_FILE         # File keyboard layout
        ELM_WIN_KEYBOARD_URL          # URL keyboard layout
        ELM_WIN_KEYBOARD_KEYPAD       # Keypad layout
        ELM_WIN_KEYBOARD_J2ME         # J2ME keyboard layout

    ctypedef enum Elm_Wrap_Type:
        ELM_WRAP_NONE
        ELM_WRAP_CHAR
        ELM_WRAP_WORD
        ELM_WRAP_MIXED


    # structs
    ctypedef struct Elm_Entry_Anchor_Info:
        char *name
        int   button
        evas.c_evas.Evas_Coord x
        evas.c_evas.Evas_Coord y
        evas.c_evas.Evas_Coord w
        evas.c_evas.Evas_Coord h

    ctypedef struct Elm_Entry_Anchor_Hover_Info:
        Elm_Entry_Anchor_Info *anchor_info
        evas.c_evas.Evas_Object *hover
        evas.c_evas.Eina_Rectangle hover_parent
        evas.c_evas.Eina_Bool hover_left
        evas.c_evas.Eina_Bool hover_right
        evas.c_evas.Eina_Bool hover_top
        evas.c_evas.Eina_Bool hover_bottom

    #ctypedef struct Elm_Toolbar_Item_State:
        #char *icon
        #char *label
        #Evas_Smart_Cb func
        #void *data


    ctypedef char *(*GenlistItemLabelGetFunc)(void *data, evas.c_evas.Evas_Object *obj, const_char_ptr part)
    ctypedef evas.c_evas.Evas_Object *(*GenlistItemIconGetFunc)(void *data, evas.c_evas.Evas_Object *obj, const_char_ptr part)
    ctypedef evas.c_evas.Eina_Bool (*GenlistItemStateGetFunc)(void *data, evas.c_evas.Evas_Object *obj, const_char_ptr part)
    ctypedef void (*GenlistItemDelFunc)(void *data, evas.c_evas.Evas_Object *obj)

    ctypedef struct Elm_Genlist_Item_Class_Func:
        GenlistItemLabelGetFunc text_get
        GenlistItemIconGetFunc content_get
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
        GengridItemLabelGetFunc text_get
        GengridItemIconGetFunc content_get
        GengridItemStateGetFunc state_get
        GengridItemDelFunc del_ "del"

    ctypedef struct Elm_Gengrid_Item_Class:
        char *item_style
        Elm_Gengrid_Item_Class_Func func

    ctypedef evas.c_evas.Evas_Object *(*Elm_Tooltip_Content_Cb) (void *data, evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *tooltip)
    ctypedef evas.c_evas.Evas_Object *(*Elm_Tooltip_Item_Content_Cb) (void *data, evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *tooltip, void *item)
    ctypedef evas.c_evas.Eina_Bool (*Elm_Event_Cb) (void *data, evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *src, evas.c_evas.Evas_Callback_Type t, void *event_info)

    ctypedef struct Elm_Object_Item
    ctypedef struct Elm_Theme

    # Basic elementary functions
    void elm_init(int argc,char** argv)
    void elm_shutdown()
    void elm_run() nogil
    void elm_exit()

    # Engine related
    char *elm_config_engine_get()
    void elm_config_engine_set(const_char_ptr engine)
    char *elm_config_preferred_engine_get()
    void elm_config_preferred_engine_set(const_char_ptr engine)

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

    void                     elm_object_content_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    evas.c_evas.Evas_Object *elm_object_content_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Evas_Object *elm_object_content_unset(evas.c_evas.Evas_Object *obj)

    void                     elm_object_part_content_set(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Evas_Object *icon)
    evas.c_evas.Evas_Object *elm_object_part_content_get(evas.c_evas.Evas_Object *obj, char *part)
    evas.c_evas.Evas_Object *elm_object_part_content_unset(evas.c_evas.Evas_Object *obj, char *part)

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

    double       elm_config_tooltip_delay_get()
    evas.c_evas.Eina_Bool elm_config_tooltip_delay_set(double delay)
    evas.c_evas.Eina_Bool elm_config_cursor_engine_only_set(int engine_only)
    int          elm_config_cursor_engine_only_get()
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
    void         elm_object_cursor_theme_search_enabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool theme_search)
    evas.c_evas.Eina_Bool elm_object_cursor_theme_search_enabled_get(evas.c_evas.Evas_Object *obj)


    void         elm_object_part_text_set(evas.c_evas.Evas_Object *obj, const_char_ptr part, const_char_ptr label)
    char*        elm_object_part_text_get(evas.c_evas.Evas_Object *obj, const_char_ptr part)

    void         elm_object_text_set(evas.c_evas.Evas_Object *obj, const_char_ptr label)
    char*        elm_object_text_get(evas.c_evas.Evas_Object *obj)

    void         elm_object_item_disabled_set(Elm_Object_Item *it, evas.c_evas.Eina_Bool disabled)
    evas.c_evas.Eina_Bool elm_object_item_disabled_get(Elm_Object_Item *it)
    void         elm_object_item_text_set(Elm_Object_Item *item, char *label)
    char*        elm_object_item_text_get(Elm_Object_Item *item)
    void*        elm_object_item_data_get(Elm_Object_Item *item)
    void         elm_object_item_data_set(Elm_Object_Item *item, void *data)
    void         elm_object_item_del(Elm_Object_Item *item)
    evas.c_evas.Evas_Object* elm_object_item_widget_get(Elm_Object_Item *it)
    void         elm_object_item_del_cb_set(Elm_Object_Item *it, evas.c_evas.Evas_Smart_Cb del_cb)
    evas.c_evas.Eina_Bool elm_object_item_cursor_engine_only_get(Elm_Object_Item *it)
    void         elm_object_item_cursor_engine_only_set(Elm_Object_Item *it, evas.c_evas.Eina_Bool engine_only)
    const_char_ptr elm_object_item_cursor_style_get(Elm_Object_Item *it)
    void         elm_object_item_cursor_style_set(Elm_Object_Item *it, const_char_ptr style)
    const_char_ptr elm_object_item_cursor_get(Elm_Object_Item *it)
    void         elm_object_item_cursor_set(Elm_Object_Item *it, const_char_ptr cursor)
    void         elm_object_item_cursor_unset(Elm_Object_Item *it)
    const_char_ptr elm_object_item_tooltip_style_get(Elm_Object_Item *it)
    void         elm_object_item_tooltip_style_set(Elm_Object_Item *it, const_char_ptr style)
    void         elm_object_item_tooltip_text_set(Elm_Object_Item *it, const_char_ptr text)
    void         elm_object_item_tooltip_content_cb_set(Elm_Object_Item *it, Elm_Tooltip_Item_Content_Cb func, void *data, evas.c_evas.Evas_Smart_Cb del_cb)
    void         elm_object_item_tooltip_unset(Elm_Object_Item *it)
    void         elm_object_item_part_content_set(Elm_Object_Item *it, const_char_ptr part, evas.c_evas.Evas_Object* content)
    evas.c_evas.Evas_Object* elm_object_item_part_content_get(Elm_Object_Item *it, const_char_ptr part)
    evas.c_evas.Evas_Object* elm_object_item_part_content_unset(Elm_Object_Item *it, const_char_ptr part)

    double       elm_config_scale_get()
    void         elm_config_scale_set(double scale)
    evas.c_evas.Evas_Coord elm_config_finger_size_get()
    void         elm_config_finger_size_set(evas.c_evas.Evas_Coord size)

    # Focus handling
    void         elm_object_focus_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool x)
    evas.c_evas.Eina_Bool elm_object_focus_get(evas.c_evas.Evas_Object *obj)
    void         elm_object_focus_allow_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool enable)
    evas.c_evas.Eina_Bool elm_object_focus_allow_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool elm_config_focus_highlight_enabled_get()
    void         elm_config_focus_highlight_enabled_set(evas.c_evas.Eina_Bool enable)
    evas.c_evas.Eina_Bool elm_config_focus_highlight_animate_get()
    void         elm_config_focus_highlight_animate_set(evas.c_evas.Eina_Bool animate)

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

    # X specific call - wont't work on non-x engines (return 0)
    Ecore_X_Window elm_win_xwindow_get(evas.c_evas.Evas_Object *obj)

    # Inwin object
    evas.c_evas.Evas_Object *elm_win_inwin_add(evas.c_evas.Evas_Object *obj)
    void                    elm_win_inwin_activate(evas.c_evas.Evas_Object *obj)
    void                    elm_win_inwin_content_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)
    evas.c_evas.Evas_Object *elm_win_inwin_content_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Evas_Object *elm_win_inwin_content_unset(evas.c_evas.Evas_Object *obj)

    # Background object (api:DONE  cb:TODO  test:DONE  doc:TODO))
    evas.c_evas.Evas_Object *elm_bg_add(evas.c_evas.Evas_Object *parent)
    void                     elm_bg_file_set(evas.c_evas.Evas_Object *obj, char *file, char *group)
    void                     elm_bg_file_get(evas.c_evas.Evas_Object *obj, char **file, char **group)
    void                     elm_bg_option_set(evas.c_evas.Evas_Object *obj, Elm_Bg_Option option)
    Elm_Bg_Option            elm_bg_option_get(evas.c_evas.Evas_Object *obj)
    void                     elm_bg_color_set(evas.c_evas.Evas_Object *obj, int r, int g, int b)
    void                     elm_bg_color_get(evas.c_evas.Evas_Object *obj, int *r, int *g, int *b)
    void                     elm_bg_load_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord w, evas.c_evas.Evas_Coord h)

    # Box object (DONE with TODO) XXX
    evas.c_evas.Evas_Object *elm_box_add(evas.c_evas.Evas_Object *parent)
    void                     elm_box_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool horizontal)
    evas.c_evas.Eina_Bool    elm_box_horizontal_get(evas.c_evas.Evas_Object *obj)
    void                     elm_box_homogeneous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool homogeneous)
    evas.c_evas.Eina_Bool    elm_box_homogeneous_get(evas.c_evas.Evas_Object *obj)
    void                     elm_box_pack_start(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj)
    void                     elm_box_pack_end(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj)
    void                     elm_box_pack_before(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj, evas.c_evas.Evas_Object *before)
    void                     elm_box_pack_after(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj, evas.c_evas.Evas_Object *after)
    void                     elm_box_clear(evas.c_evas.Evas_Object *obj)
    void                     elm_box_unpack(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj)
    void                     elm_box_unpack_all(evas.c_evas.Evas_Object *obj)
    void                     elm_box_recalculate(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_List   *elm_box_children_get(evas.c_evas.Evas_Object *obj)
    void                     elm_box_padding_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord horizontal, evas.c_evas.Evas_Coord vertical)
    void                     elm_box_padding_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *horizontal, evas.c_evas.Evas_Coord *vertical)
    void                     elm_box_align_set(evas.c_evas.Evas_Object *obj, double horizontal, double vertical)
    void                     elm_box_align_get(evas.c_evas.Evas_Object *obj, double *horizontal, double *vertical)

    # Bubble object (api:DONE  cb:TODO  test:DONE  doc:TODO)
    evas.c_evas.Evas_Object *elm_bubble_add(evas.c_evas.Evas_Object *parent)
    void                     elm_bubble_pos_set(evas.c_evas.Evas_Object *obj, Elm_Bubble_Pos pos)
    Elm_Bubble_Pos           elm_bubble_pos_get(evas.c_evas.Evas_Object *obj)

    # Check widget (api:DONE  cb:TODO  test:DONE  doc:TODO)
    evas.c_evas.Evas_Object *elm_check_add(evas.c_evas.Evas_Object *parent)
    void                     elm_check_state_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool state)
    evas.c_evas.Eina_Bool    elm_check_state_get(evas.c_evas.Evas_Object *obj)

    # Clock object (api:DONE  cb:TODO  test:DONE  doc:TODO)
    evas.c_evas.Evas_Object *elm_clock_add(evas.c_evas.Evas_Object *parent)
    void                     elm_clock_time_set(evas.c_evas.Evas_Object *obj, int hrs, int min, int sec)
    void                     elm_clock_time_get(evas.c_evas.Evas_Object *obj, int *hrs, int *min, int *sec)
    void                     elm_clock_edit_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool edit)
    evas.c_evas.Eina_Bool    elm_clock_edit_get(evas.c_evas.Evas_Object *obj)
    void                     elm_clock_edit_mode_set(evas.c_evas.Evas_Object *obj, Elm_Clock_Edit_Mode mode)
    Elm_Clock_Edit_Mode      elm_clock_edit_mode_get(evas.c_evas.Evas_Object *obj)
    void                     elm_clock_show_am_pm_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool am_pm)
    evas.c_evas.Eina_Bool    elm_clock_show_am_pm_get(evas.c_evas.Evas_Object *obj)
    void                     elm_clock_show_seconds_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool seconds)
    evas.c_evas.Eina_Bool    elm_clock_show_seconds_get(evas.c_evas.Evas_Object *obj)
    void                     elm_clock_first_interval_set(evas.c_evas.Evas_Object *obj, double interval)
    double                   elm_clock_first_interval_get(evas.c_evas.Evas_Object *obj)

    # Entry Object (api:DONEwithTODO  cb:TODO  test:TODO  doc:TODO) XXX
    evas.c_evas.Evas_Object *elm_entry_add(evas.c_evas.Evas_Object *parent)
    void                     elm_entry_single_line_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool single_line)
    evas.c_evas.Eina_Bool    elm_entry_single_line_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_password_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool password)
    evas.c_evas.Eina_Bool    elm_entry_password_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_entry_set(evas.c_evas.Evas_Object *obj, char *entry)
    char                    *elm_entry_entry_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_entry_append(evas.c_evas.Evas_Object *obj, char *text)
    evas.c_evas.Eina_Bool    elm_entry_is_empty(evas.c_evas.Evas_Object *obj)
    char                    *elm_entry_selection_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Evas_Object *elm_entry_textblock_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_calc_force(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_entry_insert(evas.c_evas.Evas_Object *obj, char *entry)
    void                     elm_entry_line_wrap_set(evas.c_evas.Evas_Object *obj, Elm_Wrap_Type wrap)
    Elm_Wrap_Type            elm_entry_line_wrap_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_editable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool editable)
    evas.c_evas.Eina_Bool    elm_entry_editable_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_select_none(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_select_all(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_entry_cursor_next(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_entry_cursor_prev(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_entry_cursor_up(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_entry_cursor_down(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_cursor_begin_set(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_cursor_end_set(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_cursor_line_begin_set(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_cursor_line_end_set(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_cursor_selection_begin(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_cursor_selection_end(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_entry_cursor_is_format_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_entry_cursor_is_visible_format_get(evas.c_evas.Evas_Object *obj)
    char                    *elm_entry_cursor_content_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_entry_cursor_geometry_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *x, evas.c_evas.Evas_Coord *y, evas.c_evas.Evas_Coord *w, evas.c_evas.Evas_Coord *h)
    void                     elm_entry_cursor_pos_set(evas.c_evas.Evas_Object *obj, int pos)
    int                      elm_entry_cursor_pos_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_selection_cut(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_selection_copy(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_selection_paste(evas.c_evas.Evas_Object *obj)
    char                    *elm_entry_markup_to_utf8(char *s)
    char                    *elm_entry_utf8_to_markup(char *s)
    evas.c_evas.Eina_Bool    elm_entry_file_set(evas.c_evas.Evas_Object *obj, char *file, Elm_Text_Format format)
    void                     elm_entry_file_get(evas.c_evas.Evas_Object *obj, char **file, Elm_Text_Format *format)
    void                     elm_entry_file_save(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_autosave_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool autosave)
    evas.c_evas.Eina_Bool    elm_entry_autosave_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_scrollable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool scrollable)
    evas.c_evas.Eina_Bool    elm_entry_scrollable_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_scrollbar_policy_set(evas.c_evas.Evas_Object *obj, Elm_Scroller_Policy h, Elm_Scroller_Policy v)
    void                     elm_entry_icon_visible_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool setting)
    void                     elm_entry_context_menu_clear(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_context_menu_disabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool disabled)
    evas.c_evas.Eina_Bool    elm_entry_context_menu_disabled_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_bounce_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool h_bounce, evas.c_evas.Eina_Bool v_bounce)
    void                     elm_entry_bounce_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool *h_bounce, evas.c_evas.Eina_Bool *v_bounce)
    void                     elm_entry_input_panel_enabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool enabled)
    evas.c_evas.Eina_Bool    elm_entry_input_panel_enabled_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_input_panel_layout_set(evas.c_evas.Evas_Object *obj, Elm_Input_Panel_Layout layout)
    Elm_Input_Panel_Layout   elm_entry_input_panel_layout_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_input_panel_show(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_input_panel_hide(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_input_panel_language_set(evas.c_evas.Evas_Object *obj, Elm_Input_Panel_Lang lang)
    Elm_Input_Panel_Lang     elm_entry_input_panel_language_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_input_panel_return_key_type_set(evas.c_evas.Evas_Object *obj, Elm_Input_Panel_Return_Key_Type return_key_type)
    Elm_Input_Panel_Return_Key_Type elm_entry_input_panel_return_key_type_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_input_panel_return_key_disabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool disabled)
    evas.c_evas.Eina_Bool    elm_entry_input_panel_return_key_disabled_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_input_panel_return_key_autoenabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool disabled)
    void                     elm_entry_imf_context_reset(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_prediction_allow_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool allow)
    evas.c_evas.Eina_Bool    elm_entry_prediction_allow_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_cnp_mode_set(evas.c_evas.Evas_Object *obj, Elm_Cnp_Mode cnp_mode)
    Elm_Cnp_Mode             elm_entry_cnp_mode_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_anchor_hover_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *anchor_hover_parent)
    evas.c_evas.Evas_Object *elm_entry_anchor_hover_parent_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_anchor_hover_style_set(evas.c_evas.Evas_Object *obj, char *anchor_hover_style)
    char                    *elm_entry_anchor_hover_style_get(evas.c_evas.Evas_Object *obj)
    void                     elm_entry_anchor_hover_end(evas.c_evas.Evas_Object *obj)

    # Fileselector widget (api:DONE  cb:DONE  test:DONE  doc:TODO)
    evas.c_evas.Evas_Object *elm_fileselector_add(evas.c_evas.Evas_Object *parent)
    void                     elm_fileselector_is_save_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool is_save)
    evas.c_evas.Eina_Bool    elm_fileselector_is_save_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_folder_only_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool value)
    evas.c_evas.Eina_Bool    elm_fileselector_folder_only_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_buttons_ok_cancel_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool value)
    evas.c_evas.Eina_Bool    elm_fileselector_buttons_ok_cancel_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_expandable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool value)
    evas.c_evas.Eina_Bool    elm_fileselector_expandable_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_path_set(evas.c_evas.Evas_Object *obj, char *path)
    char                    *elm_fileselector_path_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_fileselector_selected_set(evas.c_evas.Evas_Object *obj, char *path)
    char                    *elm_fileselector_selected_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_mode_set(evas.c_evas.Evas_Object *obj, Elm_Fileselector_Mode mode)
    Elm_Fileselector_Mode    elm_fileselector_mode_get(evas.c_evas.Evas_Object *obj)

    # Flip object (api:DONE  cb:DONE  test:DONE doc:TODO)
    evas.c_evas.Evas_Object* elm_flip_add(evas.c_evas.Evas_Object *parent)
    evas.c_evas.Eina_Bool    elm_flip_front_visible_get(evas.c_evas.Evas_Object *obj)
    void                     elm_flip_perspective_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord foc, evas.c_evas.Evas_Coord x, evas.c_evas.Evas_Coord y)
    void                     elm_flip_go(evas.c_evas.Evas_Object *obj, Elm_Flip_Mode mode)
    void                     elm_flip_interaction_set(evas.c_evas.Evas_Object *obj, Elm_Flip_Interaction mode)
    Elm_Flip_Interaction     elm_flip_interaction_get(evas.c_evas.Evas_Object *obj)
    void                     elm_flip_interaction_direction_enabled_set(evas.c_evas.Evas_Object *obj, Elm_Flip_Direction dir, evas.c_evas.Eina_Bool enabled)
    evas.c_evas.Eina_Bool    elm_flip_interaction_direction_enabled_get(evas.c_evas.Evas_Object *obj, Elm_Flip_Direction dir)
    void                     elm_flip_interaction_direction_hitsize_set(evas.c_evas.Evas_Object *obj, Elm_Flip_Direction dir, double hitsize)
    double                   elm_flip_interaction_direction_hitsize_get(evas.c_evas.Evas_Object *obj, Elm_Flip_Direction dir)

    # Fileselector Button widget (api:DONE  cb:DONE  test:DONE  doc:TODO)
    evas.c_evas.Evas_Object *elm_fileselector_button_add(evas.c_evas.Evas_Object *parent)
    void                     elm_fileselector_button_window_title_set(evas.c_evas.Evas_Object *obj, char *title)
    char                    *elm_fileselector_button_window_title_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_button_window_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord width, evas.c_evas.Evas_Coord height)
    void                     elm_fileselector_button_window_size_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *width, evas.c_evas.Evas_Coord *height)
    void                     elm_fileselector_button_folder_only_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool value)
    evas.c_evas.Eina_Bool    elm_fileselector_button_folder_only_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_button_inwin_mode_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool value)
    evas.c_evas.Eina_Bool    elm_fileselector_button_inwin_mode_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_button_is_save_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool value)
    evas.c_evas.Eina_Bool    elm_fileselector_button_is_save_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_button_path_set(evas.c_evas.Evas_Object *obj, char *path)
    char                    *elm_fileselector_button_path_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_button_expandable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool value)
    evas.c_evas.Eina_Bool    elm_fileselector_button_expandable_get(evas.c_evas.Evas_Object *obj)

    # Fileselector Entry widget (api:DONE  cb:DONE  test:DONE  doc:TODO)
    evas.c_evas.Evas_Object *elm_fileselector_entry_add(evas.c_evas.Evas_Object *parent)
    void                     elm_fileselector_entry_window_title_set(evas.c_evas.Evas_Object *obj, char *title)
    char                    *elm_fileselector_entry_window_title_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_entry_window_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord width, evas.c_evas.Evas_Coord height)
    void                     elm_fileselector_entry_window_size_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *width, evas.c_evas.Evas_Coord *height)
    void                     elm_fileselector_entry_folder_only_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool value)
    evas.c_evas.Eina_Bool    elm_fileselector_entry_folder_only_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_entry_inwin_mode_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool value)
    evas.c_evas.Eina_Bool    elm_fileselector_entry_inwin_mode_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_entry_is_save_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool value)
    evas.c_evas.Eina_Bool    elm_fileselector_entry_is_save_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_entry_selected_set(evas.c_evas.Evas_Object *obj, char *path)
    char                    *elm_fileselector_entry_selected_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_entry_path_set(evas.c_evas.Evas_Object *obj, char *path)
    char                    *elm_fileselector_entry_path_get(evas.c_evas.Evas_Object *obj)
    void                     elm_fileselector_entry_expandable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool value)
    evas.c_evas.Eina_Bool    elm_fileselector_entry_expandable_get(evas.c_evas.Evas_Object *obj)

    # Frame object (api:DONE  cb:DONE  test:TODO  doc:TODO)
    evas.c_evas.Evas_Object *elm_frame_add(evas.c_evas.Evas_Object *parent)
    void                     elm_frame_autocollapse_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool autocollapse)
    evas.c_evas.Eina_Bool    elm_frame_autocollapse_get(evas.c_evas.Evas_Object *obj)
    void                     elm_frame_collapse_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool collapse)
    evas.c_evas.Eina_Bool    elm_frame_collapse_get(evas.c_evas.Evas_Object *obj)
    void                     elm_frame_collapse_go(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool collapse)

    # Gengrid object (api:DONEwithTODO  cd:TODO  test:OK  doc:HALF)
    evas.c_evas.Evas_Object *elm_gengrid_add(evas.c_evas.Evas_Object *parent)
    void                     elm_gengrid_clear(evas.c_evas.Evas_Object *obj)
    void                     elm_gengrid_multi_select_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool multi)
    evas.c_evas.Eina_Bool    elm_gengrid_multi_select_get(evas.c_evas.Evas_Object *obj)
    void                     elm_gengrid_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool setting)
    evas.c_evas.Eina_Bool    elm_gengrid_horizontal_get(evas.c_evas.Evas_Object *obj)
    void                     elm_gengrid_bounce_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool h_bounce, evas.c_evas.Eina_Bool v_bounce)
    void                     elm_gengrid_bounce_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool *h_bounce, evas.c_evas.Eina_Bool *v_bounce)
    Elm_Object_Item         *elm_gengrid_item_append(evas.c_evas.Evas_Object *obj, Elm_Gengrid_Item_Class *itc, void *data, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_gengrid_item_prepend(evas.c_evas.Evas_Object *obj, Elm_Gengrid_Item_Class *itc, void *data, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_gengrid_item_insert_before(evas.c_evas.Evas_Object *obj, Elm_Gengrid_Item_Class *itc, void *data, Elm_Object_Item *before, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_gengrid_item_insert_after(evas.c_evas.Evas_Object *obj, Elm_Gengrid_Item_Class *itc, void *data, Elm_Object_Item *after, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_gengrid_selected_item_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_List   *elm_gengrid_selected_items_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_List   *elm_gengrid_realized_items_get(evas.c_evas.Evas_Object *obj)
    void                     elm_gengrid_realized_items_update(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item         *elm_gengrid_first_item_get(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item         *elm_gengrid_last_item_get(evas.c_evas.Evas_Object *obj)
    void                     elm_gengrid_scroller_policy_set(evas.c_evas.Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v)
    void                     elm_gengrid_scroller_policy_get(evas.c_evas.Evas_Object *obj, Elm_Scroller_Policy *policy_h, Elm_Scroller_Policy *policy_v)
    unsigned int             elm_gengrid_items_count(evas.c_evas.Evas_Object *obj)
    void                     elm_gengrid_item_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord w, evas.c_evas.Evas_Coord h)
    void                     elm_gengrid_item_size_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *w, evas.c_evas.Evas_Coord *h)
    void                     elm_gengrid_group_item_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord w, evas.c_evas.Evas_Coord h)
    void                     elm_gengrid_group_item_size_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *w, evas.c_evas.Evas_Coord *h)
    void                     elm_gengrid_align_set(evas.c_evas.Evas_Object *obj,  double align_x, double align_y)
    void                     elm_gengrid_align_get(evas.c_evas.Evas_Object *obj,  double *align_x, double *align_y)
    void                     elm_gengrid_reorder_mode_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool reorder_mode)
    evas.c_evas.Eina_Bool    elm_gengrid_reorder_mode_get(evas.c_evas.Evas_Object *obj)
    void                     elm_gengrid_page_relative_set(evas.c_evas.Evas_Object *obj, double h_pagerel, double v_pagerel)
    void                     elm_gengrid_page_relative_get(evas.c_evas.Evas_Object *obj, double *h_pagerel, double *v_pagerel)
    void                     elm_gengrid_page_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord h_pagesize, evas.c_evas.Evas_Coord v_pagesize)
    void                     elm_gengrid_current_page_get(evas.c_evas.Evas_Object *obj, int *h_pagenum, int *v_pagenum)
    void                     elm_gengrid_last_page_get(evas.c_evas.Evas_Object *obj, int *h_pagenum, int *v_pagenum)
    void                     elm_gengrid_page_show(evas.c_evas.Evas_Object *obj, int h_pagenum, int v_pagenum)
    void                     elm_gengrid_page_bring_in(evas.c_evas.Evas_Object *obj, int h_pagenum, int v_pagenum)
    void                     elm_gengrid_filled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool fill)
    evas.c_evas.Eina_Bool    elm_gengrid_filled_get(evas.c_evas.Evas_Object *obj)
    void                     elm_gengrid_select_mode_set(evas.c_evas.Evas_Object *obj, Elm_Object_Select_Mode mode)
    Elm_Object_Select_Mode   elm_gengrid_select_mode_get(evas.c_evas.Evas_Object *obj)
    void                     elm_gengrid_highlight_mode_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool highlight)
    evas.c_evas.Eina_Bool    elm_gengrid_highlight_mode_get(evas.c_evas.Evas_Object *obj)

    Elm_Object_Item         *elm_gengrid_first_item_get(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item         *elm_gengrid_last_item_get(evas.c_evas.Evas_Object *obj)
    int                      elm_gengrid_item_index_get(Elm_Object_Item *it)
    void                     elm_gengrid_item_select_mode_set(Elm_Object_Item *it, Elm_Object_Select_Mode mode)
    Elm_Object_Select_Mode   elm_gengrid_item_select_mode_get(Elm_Object_Item *it)
    Elm_Object_Item         *elm_gengrid_item_next_get(Elm_Object_Item *item)
    Elm_Object_Item         *elm_gengrid_item_prev_get(Elm_Object_Item *item)
    void                     elm_gengrid_item_selected_set(Elm_Object_Item *item, evas.c_evas.Eina_Bool selected)
    evas.c_evas.Eina_Bool    elm_gengrid_item_selected_get(Elm_Object_Item *item)
    void                     elm_gengrid_item_show(Elm_Object_Item *item, Elm_Genlist_Item_Scrollto_Type scrollto_type)
    void                     elm_gengrid_item_bring_in(Elm_Object_Item *item, Elm_Genlist_Item_Scrollto_Type scrollto_type)
    evas.c_evas.Evas_Object *elm_gengrid_item_object_get(Elm_Object_Item *it)
    void                     elm_gengrid_item_update(Elm_Object_Item *item)
    void                     elm_gengrid_item_pos_get(Elm_Object_Item *item, unsigned int *x, unsigned int *y)
    void                     elm_gengrid_item_tooltip_text_set(Elm_Object_Item *item, char *text)
    void                     elm_gengrid_item_tooltip_content_cb_set(Elm_Object_Item *item, Elm_Tooltip_Item_Content_Cb func, void *data, evas.c_evas.Evas_Smart_Cb del_cb)
    void                     elm_gengrid_item_tooltip_unset(Elm_Object_Item *item)
    void                     elm_gengrid_item_tooltip_style_set(Elm_Object_Item *item, char *style)
    char                    *elm_gengrid_item_tooltip_style_get(Elm_Object_Item *item)
    evas.c_evas.Eina_Bool    elm_gengrid_item_tooltip_window_mode_set(Elm_Object_Item *it, evas.c_evas.Eina_Bool disable)
    evas.c_evas.Eina_Bool    elm_gengrid_item_tooltip_window_mode_get(Elm_Object_Item *it)
    void                     elm_gengrid_item_cursor_set(Elm_Object_Item *item, char *cursor)
    char                    *elm_gengrid_item_cursor_get(Elm_Object_Item *item)
    void                     elm_gengrid_item_cursor_unset(Elm_Object_Item *item)
    void                     elm_gengrid_item_cursor_style_set(Elm_Object_Item *item, char *style)
    char                    *elm_gengrid_item_cursor_style_get(Elm_Object_Item *item)
    void                     elm_gengrid_item_cursor_engine_only_set(Elm_Object_Item *item, evas.c_evas.Eina_Bool engine_only)
    evas.c_evas.Eina_Bool    elm_gengrid_item_cursor_engine_only_get(Elm_Object_Item *item)

    # Icon object (api:TODO  cb:DONE  test:TODO  doc:TODO)
    evas.c_evas.Evas_Object *elm_icon_add(evas.c_evas.Evas_Object *parent)
    evas.c_evas.Eina_Bool    elm_icon_file_set(evas.c_evas.Evas_Object *obj, char *file, char *group)
    #evas.c_evas.Eina_Bool   elm_icon_memfile_set(evas.c_evas.Evas_Object *obj, void *img, size_t size, char *format, char *key);
    void                    *elm_icon_file_get(evas.c_evas.Evas_Object *obj, char **file, char **group)
    void                     elm_icon_thumb_set(evas.c_evas.Evas_Object *obj, char *file, char *group)
    evas.c_evas.Eina_Bool    elm_icon_standard_set(evas.c_evas.Evas_Object *obj, char* name)
    char                    *elm_icon_standard_get(evas.c_evas.Evas_Object *obj)
    void                     elm_icon_smooth_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool smooth)
    evas.c_evas.Eina_Bool    elm_icon_smooth_get(evas.c_evas.Evas_Object *obj)
    void                     elm_icon_no_scale_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool no_scale)
    evas.c_evas.Eina_Bool    elm_icon_no_scale_get(evas.c_evas.Evas_Object *obj)
    void                     elm_icon_resizable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool scale_up, evas.c_evas.Eina_Bool scale_down)
    void                     elm_icon_resizable_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool *scale_up, evas.c_evas.Eina_Bool *scale_down)
    void                     elm_icon_size_get(evas.c_evas.Evas_Object *obj, int *w, int *h)
    void                     elm_icon_fill_outside_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool fill_outside)
    evas.c_evas.Eina_Bool    elm_icon_fill_outside_get(evas.c_evas.Evas_Object *obj)
    void                     elm_icon_prescale_set(evas.c_evas.Evas_Object *obj, int size)
    int                      elm_icon_prescale_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Evas_Object *elm_icon_object_get(evas.c_evas.Evas_Object *obj)
    void                     elm_icon_order_lookup_set(evas.c_evas.Evas_Object *obj, Elm_Icon_Lookup_Order order)
    Elm_Icon_Lookup_Order    elm_icon_order_lookup_get(evas.c_evas.Evas_Object *obj)
    void                     elm_icon_preload_disabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool disabled)
    evas.c_evas.Eina_Bool    elm_icon_animated_available_get(evas.c_evas.Evas_Object *obj)
    void                     elm_icon_animated_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool animated)
    evas.c_evas.Eina_Bool    elm_icon_animated_get(evas.c_evas.Evas_Object *obj)
    void                     elm_icon_animated_play_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool play)
    evas.c_evas.Eina_Bool    elm_icon_animated_play_get(evas.c_evas.Evas_Object *obj)
    void                     elm_icon_aspect_fixed_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool fixed)
    evas.c_evas.Eina_Bool    elm_icon_aspect_fixed_get(evas.c_evas.Evas_Object *obj)

    # Image object (api:DONE  cb:DONE  test:TODO  doc:TODO)
    evas.c_evas.Evas_Object *elm_image_add(evas.c_evas.Evas_Object *parent)
    evas.c_evas.Eina_Bool    elm_image_file_set(evas.c_evas.Evas_Object *obj, char *file, char *group)
    void                     elm_image_file_get(evas.c_evas.Evas_Object *obj, char **file, char **group)
    void                     elm_image_smooth_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool smooth)
    evas.c_evas.Eina_Bool    elm_image_smooth_get(evas.c_evas.Evas_Object *obj)
    void                     elm_image_object_size_get(evas.c_evas.Evas_Object *obj, int *w, int *h)
    void                     elm_image_no_scale_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool no_scale)
    evas.c_evas.Eina_Bool    elm_image_no_scale_get(evas.c_evas.Evas_Object *obj)
    void                     elm_image_resizable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool scale_up,evas.c_evas.Eina_Bool scale_down)
    void                     elm_image_resizable_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool *scale_up,evas.c_evas.Eina_Bool *scale_down)
    void                     elm_image_fill_outside_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool fill_outside)
    evas.c_evas.Eina_Bool    elm_image_fill_outside_get(evas.c_evas.Evas_Object *obj)
    void                     elm_image_preload_disabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool disabled)
    void                     elm_image_prescale_set(evas.c_evas.Evas_Object *obj, int size)
    int                      elm_image_prescale_get(evas.c_evas.Evas_Object *obj)
    void                     elm_image_orient_set(evas.c_evas.Evas_Object *obj, Elm_Image_Orient orient)
    Elm_Image_Orient         elm_image_orient_get(evas.c_evas.Evas_Object *obj)
    void                     elm_image_editable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool editable)
    evas.c_evas.Eina_Bool    elm_image_editable_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Evas_Object *elm_image_object_get(evas.c_evas.Evas_Object *obj)
    void                     elm_image_aspect_fixed_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool fixed)
    evas.c_evas.Eina_Bool    elm_image_aspect_fixed_get(evas.c_evas.Evas_Object *obj)

    # Button object
    evas.c_evas.Evas_Object *elm_button_add(evas.c_evas.Evas_Object *parent)
    void                     elm_button_autorepeat_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool on)
    evas.c_evas.Eina_Bool    elm_button_autorepeat_get(evas.c_evas.Evas_Object *obj)
    void                     elm_button_autorepeat_initial_timeout_set(evas.c_evas.Evas_Object *obj, double t)
    double                   elm_button_autorepeat_initial_timeout_get(evas.c_evas.Evas_Object *obj)
    void                     elm_button_autorepeat_gap_timeout_set(evas.c_evas.Evas_Object *obj, double t)
    double                   elm_button_autorepeat_gap_timeout_get(evas.c_evas.Evas_Object *obj)

    # Web object
    ctypedef struct Elm_Web_Frame_Load_Error:
        int code
        evas.c_evas.Eina_Bool is_cancellation
        const_char_ptr domain
        const_char_ptr description
        const_char_ptr failing_url
        evas.c_evas.Evas_Object *frame

    ctypedef void (*Elm_Web_Console_Message)(void *data, evas.c_evas.Evas_Object *obj, const_char_ptr message, unsigned int line_number, const_char_ptr source_id)

    evas.c_evas.Eina_Bool elm_need_web()
    evas.c_evas.Eina_Bool elm_web_history_enabled_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Evas_Object *elm_web_webkit_view_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Evas_Object *elm_web_add(evas.c_evas.Evas_Object *parent)
    evas.c_evas.Eina_Bool elm_web_uri_set(evas.c_evas.Evas_Object *obj,char *uri)
    const_char_ptr elm_web_uri_get(evas.c_evas.Evas_Object *obj)
    const_char_ptr elm_web_useragent_get(evas.c_evas.Evas_Object *obj)
    double elm_web_zoom_get(evas.c_evas.Evas_Object *obj)
    Elm_Web_Zoom_Mode elm_web_zoom_mode_get(evas.c_evas.Evas_Object *obj)

    evas.c_evas.Eina_Bool elm_web_back(evas.c_evas.Evas_Object *obj)

    void elm_web_console_message_hook_set(evas.c_evas.Evas_Object *obj, Elm_Web_Console_Message func, void *data)

    # Label object
    evas.c_evas.Evas_Object *elm_label_add(evas.c_evas.Evas_Object *parent)
    void elm_label_line_wrap_set(evas.c_evas.Evas_Object *obj, Elm_Wrap_Type wrap)
    Elm_Wrap_Type elm_label_line_wrap_get(evas.c_evas.Evas_Object *obj)
    void elm_label_wrap_width_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord w)
    evas.c_evas.Evas_Coord elm_label_wrap_width_get(evas.c_evas.Evas_Object *obj)
    void elm_label_ellipsis_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool ellipsis)
    evas.c_evas.Eina_Bool elm_label_ellipsis_get(evas.c_evas.Evas_Object *obj)
    void elm_label_slide_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool slide)
    evas.c_evas.Eina_Bool elm_label_slide_get(evas.c_evas.Evas_Object *obj)
    void elm_label_slide_duration_set(evas.c_evas.Evas_Object *obj, double duration)
    double elm_label_slide_duration_get(evas.c_evas.Evas_Object *obj)

    # Layout object
    evas.c_evas.Evas_Object *elm_layout_add(evas.c_evas.Evas_Object *parent)
    evas.c_evas.Eina_Bool elm_layout_file_set(evas.c_evas.Evas_Object *obj, char *file, char *group)
    evas.c_evas.Eina_Bool elm_layout_theme_set(evas.c_evas.Evas_Object *obj, char *clas, char *group, char *style)
    evas.c_evas.Evas_Object *elm_layout_edje_get(evas.c_evas.Evas_Object *obj)
    void elm_layout_icon_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *icon)
    evas.c_evas.Evas_Object *elm_layout_icon_get(evas.c_evas.Evas_Object *obj)
    void elm_layout_end_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *end)
    evas.c_evas.Evas_Object *elm_layout_end_get(evas.c_evas.Evas_Object *obj)
    void elm_layout_sizing_eval(evas.c_evas.Evas_Object *obj)

    # Notify object (api:DONE  cb:DONE  test:TODO  doc:DONE)
    evas.c_evas.Evas_Object *elm_notify_add(evas.c_evas.Evas_Object *parent)
    void                     elm_notify_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    evas.c_evas.Evas_Object *elm_notify_parent_get(evas.c_evas.Evas_Object *obj)
    void                     elm_notify_orient_set(evas.c_evas.Evas_Object *obj, int orient)
    int                      elm_notify_orient_get(evas.c_evas.Evas_Object *obj)
    void                     elm_notify_timeout_set(evas.c_evas.Evas_Object *obj, double timeout)
    double                   elm_notify_timeout_get(evas.c_evas.Evas_Object *obj)
    void                     elm_notify_allow_events_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool repeat)
    evas.c_evas.Eina_Bool    elm_notify_allow_events_get(evas.c_evas.Evas_Object *obj)

    # Hover object
    evas.c_evas.Evas_Object *elm_hover_add(evas.c_evas.Evas_Object *parent)
    void elm_hover_target_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *target)
    void elm_hover_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    void elm_hover_style_set(evas.c_evas.Evas_Object *obj, char *style)
    char  *elm_hover_best_content_location_get(evas.c_evas.Evas_Object *obj, Elm_Hover_Axis pref_axis)

    # Hoversel object
    evas.c_evas.Evas_Object *elm_hoversel_add(evas.c_evas.Evas_Object *parent)
    void elm_hoversel_hover_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    void elm_hoversel_hover_begin(evas.c_evas.Evas_Object *obj)
    void elm_hoversel_hover_end(evas.c_evas.Evas_Object *obj)
    void elm_hoversel_clear(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item *elm_hoversel_item_add(evas.c_evas.Evas_Object *obj, char *label, char *icon_file, Elm_Icon_Type icon_type, void (*func)(void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    void elm_hoversel_item_icon_set(Elm_Object_Item *it, char *icon_file, char *icon_group, Elm_Icon_Type icon_type)
    void elm_hoversel_item_icon_get(Elm_Object_Item *it, char **icon_file, char **icon_group, Elm_Icon_Type *icon_type)

    # List object
    evas.c_evas.Evas_Object *elm_list_add(evas.c_evas.Evas_Object *parent)
    Elm_Object_Item *elm_list_item_append(evas.c_evas.Evas_Object *obj, char *label, evas.c_evas.Evas_Object *icon, evas.c_evas.Evas_Object *end, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Object_Item *elm_list_item_prepend(evas.c_evas.Evas_Object *obj, char *label, evas.c_evas.Evas_Object *icon, evas.c_evas.Evas_Object *end, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Object_Item *elm_list_item_insert_before(evas.c_evas.Evas_Object *obj, Elm_Object_Item *before, char *label, evas.c_evas.Evas_Object *icon, evas.c_evas.Evas_Object *end, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Object_Item *elm_list_item_insert_after(evas.c_evas.Evas_Object *obj, Elm_Object_Item *after, char *label, evas.c_evas.Evas_Object *icon, evas.c_evas.Evas_Object *end, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    void        elm_list_clear(evas.c_evas.Evas_Object *obj)
    void         elm_list_go(evas.c_evas.Evas_Object *obj)
    void      elm_list_multi_select_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool multi)
    evas.c_evas.Eina_Bool      elm_list_multi_select_get(evas.c_evas.Evas_Object *obj)
    void         elm_list_mode_set(evas.c_evas.Evas_Object *obj, Elm_List_Mode mode)
    Elm_List_Mode       elm_list_mode_get(evas.c_evas.Evas_Object *obj)
    void         elm_list_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool horizontal)
    evas.c_evas.Eina_Bool       elm_list_horizontal_get(evas.c_evas.Evas_Object *obj)
    void      elm_list_select_mode_set(evas.c_evas.Evas_Object *obj, Elm_Object_Select_Mode mode)
    evas.c_evas.Eina_List     *elm_list_items_get(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item *elm_list_selected_item_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_List     *elm_list_selected_items_get(evas.c_evas.Evas_Object *obj)
    void         elm_list_item_selected_set(Elm_Object_Item *item, evas.c_evas.Eina_Bool selected)
    evas.c_evas.Eina_Bool      elm_list_item_selected_get(Elm_Object_Item *item)
    void         elm_list_item_show(Elm_Object_Item *item)
    evas.c_evas.Evas_Object *elm_list_item_object_get(Elm_Object_Item *item)
    Elm_Object_Item *elm_list_item_prev(Elm_Object_Item *it)
    Elm_Object_Item *elm_list_item_next(Elm_Object_Item *it)
    void elm_list_bounce_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool h_bounce, evas.c_evas.Eina_Bool v_bounce)
    void 	elm_list_scroller_policy_set(evas.c_evas.Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v)

    # Generic List
    evas.c_evas.Evas_Object *elm_genlist_add(evas.c_evas.Evas_Object *parent)
    void elm_genlist_clear(evas.c_evas.Evas_Object *obj)
    void elm_genlist_multi_select_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool multi)
    void elm_genlist_select_mode_set(evas.c_evas.Evas_Object *obj, Elm_Object_Select_Mode mode)
    void elm_genlist_bounce_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool h_bounce, evas.c_evas.Eina_Bool v_bounce)
    void elm_genlist_homogeneous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool homogeneous)
    void elm_genlist_block_count_set(evas.c_evas.Evas_Object *obj, int n)
    Elm_Object_Item *elm_genlist_item_append(evas.c_evas.Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Object_Item *parent, Elm_Genlist_Item_Type flags, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item *elm_genlist_item_prepend(evas.c_evas.Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Object_Item *parent, Elm_Genlist_Item_Type flags, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item *elm_genlist_item_insert_before(evas.c_evas.Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Object_Item *parent, Elm_Object_Item *before, Elm_Genlist_Item_Type flags, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item *elm_genlist_item_insert_after(evas.c_evas.Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Object_Item *parent, Elm_Object_Item *after, Elm_Genlist_Item_Type flags, evas.c_evas.Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item *elm_genlist_selected_item_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_List *elm_genlist_selected_items_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_List *elm_genlist_realized_items_get(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item *elm_genlist_at_xy_item_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord x, evas.c_evas.Evas_Coord y, int *posret)
    Elm_Object_Item *elm_genlist_first_item_get(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item *elm_genlist_last_item_get(evas.c_evas.Evas_Object *obj)

    Elm_Object_Item *elm_genlist_item_next_get(Elm_Object_Item *item)
    Elm_Object_Item *elm_genlist_item_prev_get(Elm_Object_Item *item)
    Elm_Object_Item *elm_genlist_item_parent_get(Elm_Object_Item *it)
    void elm_genlist_item_subitems_clear(Elm_Object_Item *item)
    void elm_genlist_item_selected_set(Elm_Object_Item *item, evas.c_evas.Eina_Bool selected)
    evas.c_evas.Eina_Bool elm_genlist_item_selected_get(Elm_Object_Item *item)
    void elm_genlist_item_expanded_set(Elm_Object_Item *item, evas.c_evas.Eina_Bool expanded)
    evas.c_evas.Eina_Bool elm_genlist_item_expanded_get(Elm_Object_Item *item)
    void elm_genlist_item_select_mode_set(Elm_Object_Item *it, Elm_Object_Select_Mode mode)
    Elm_Object_Select_Mode elm_genlist_item_select_mode_get(Elm_Object_Item *it)
    void elm_genlist_item_show(Elm_Object_Item *item, Elm_Genlist_Item_Scrollto_Type scrollto_type)
    void elm_genlist_item_bring_in(Elm_Object_Item *item, Elm_Genlist_Item_Scrollto_Type scrollto_type)
    void elm_genlist_item_update(Elm_Object_Item *item)
    void elm_genlist_item_fields_update(Elm_Object_Item *item, char *parts, Elm_Genlist_Item_Field_Type itf)
    void         elm_genlist_item_tooltip_text_set(Elm_Object_Item *item, char *text)
    void         elm_genlist_item_tooltip_content_cb_set(Elm_Object_Item *item, Elm_Tooltip_Item_Content_Cb func, void *data, evas.c_evas.Evas_Smart_Cb del_cb)
    void         elm_genlist_item_tooltip_unset(Elm_Object_Item *item)
    void         elm_genlist_item_tooltip_style_set(Elm_Object_Item *item, char *style)
    char*        elm_genlist_item_tooltip_style_get(Elm_Object_Item *item)
    void         elm_genlist_item_cursor_set(Elm_Object_Item *item, char *cursor)
    void         elm_genlist_item_cursor_unset(Elm_Object_Item *item)
    void         elm_genlist_item_cursor_style_set(Elm_Object_Item *item, char *style)
    char*        elm_genlist_item_cursor_style_get(Elm_Object_Item *item)
    void         elm_genlist_item_cursor_engine_only_set(Elm_Object_Item *item, evas.c_evas.Eina_Bool engine_only)
    evas.c_evas.Eina_Bool elm_genlist_item_cursor_engine_only_get(Elm_Object_Item *item)
    Elm_List_Mode elm_genlist_mode_get(evas.c_evas.Evas_Object *obj)
    void         elm_genlist_mode_set(evas.c_evas.Evas_Object *obj, Elm_List_Mode mode)

    # Naviframe widget
    evas.c_evas.Evas_Object *elm_naviframe_add(evas.c_evas.Evas_Object *parent)
    Elm_Object_Item *elm_naviframe_item_push(evas.c_evas.Evas_Object *obj, const_char_ptr title_label, evas.c_evas.Evas_Object *prev_btn, evas.c_evas.Evas_Object *next_btn, evas.c_evas.Evas_Object *content, const_char_ptr item_style)
    Elm_Object_Item *elm_naviframe_item_insert_before(evas.c_evas.Evas_Object *obj, Elm_Object_Item *before, const_char_ptr title_label, evas.c_evas.Evas_Object *prev_btn, evas.c_evas.Evas_Object *next_btn, evas.c_evas.Evas_Object *content, const_char_ptr item_style)
    Elm_Object_Item *elm_naviframe_item_insert_after(evas.c_evas.Evas_Object *obj, Elm_Object_Item *after, const_char_ptr title_label, evas.c_evas.Evas_Object *prev_btn, evas.c_evas.Evas_Object *next_btn, evas.c_evas.Evas_Object *content, const_char_ptr item_style)
    evas.c_evas.Evas_Object *elm_naviframe_item_pop(evas.c_evas.Evas_Object *obj)
    void         elm_naviframe_item_pop_to(Elm_Object_Item *it)
    void         elm_naviframe_item_promote(Elm_Object_Item *it)
    void         elm_naviframe_content_preserve_on_pop_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool preserve)
    evas.c_evas.Eina_Bool elm_naviframe_content_preserve_on_pop_get(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item *elm_naviframe_top_item_get(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item *elm_naviframe_bottom_item_get(evas.c_evas.Evas_Object *obj)
    void         elm_naviframe_item_style_set(Elm_Object_Item *it, const_char_ptr item_style)
    const_char_ptr elm_naviframe_item_style_get(Elm_Object_Item *it)
    void         elm_naviframe_item_title_visible_set(Elm_Object_Item *it, evas.c_evas.Eina_Bool visible)
    evas.c_evas.Eina_Bool elm_naviframe_item_title_visible_get(Elm_Object_Item *it)
    void         elm_naviframe_prev_btn_auto_pushed_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool auto_pushed)
    evas.c_evas.Eina_List *elm_naviframe_items_get(evas.c_evas.Evas_Object *obj)
    void         elm_naviframe_event_enabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool enabled)
    evas.c_evas.Eina_Bool elm_naviframe_event_enabled_get(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item *elm_naviframe_item_simple_push(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)
    void         elm_naviframe_item_simple_promote(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *content)

    # Separator
    evas.c_evas.Evas_Object *elm_separator_add(evas.c_evas.Evas_Object *parent)
    void elm_separator_horizontal_set(evas.c_evas.Evas_Object *obj,
                                      evas.c_evas.Eina_Bool)
    evas.c_evas.Eina_Bool elm_separator_horizontal_get(evas.c_evas.Evas_Object *obj)

    # Menu object
    evas.c_evas.Evas_Object *elm_menu_add(evas.c_evas.Evas_Object *parent)
    void  elm_menu_move(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord x, evas.c_evas.Evas_Coord y)
    void  elm_menu_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    Elm_Object_Item *elm_menu_item_add(evas.c_evas.Evas_Object *obj, Elm_Object_Item *parent, char *icon, char *label, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Object_Item *elm_menu_item_separator_add(evas.c_evas.Evas_Object *obj, Elm_Object_Item *parent)
    evas.c_evas.Evas_Object *elm_menu_object_get(Elm_Object_Item *it)
    evas.c_evas.Eina_List *elm_menu_item_subitems_get(Elm_Object_Item *item)
    void  elm_menu_item_icon_name_set(Elm_Object_Item *it, const_char_ptr icon)

    # Panel (api:DONE  cb:N/A  test:TODO  doc:TODO)
    evas.c_evas.Evas_Object *elm_panel_add(evas.c_evas.Evas_Object *parent)
    void                     elm_panel_orient_set(evas.c_evas.Evas_Object *obj, Elm_Panel_Orient orient)
    Elm_Panel_Orient         elm_panel_orient_get(evas.c_evas.Evas_Object *obj)
    void                     elm_panel_hidden_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool hidden)
    evas.c_evas.Eina_Bool    elm_panel_hidden_get(evas.c_evas.Evas_Object *obj)
    void                     elm_panel_toggle(evas.c_evas.Evas_Object *obj)

    # Progressbar object (api:DONE  cb:DONE  test:DONE  doc:TODO)
    evas.c_evas.Evas_Object *elm_progressbar_add(evas.c_evas.Evas_Object *parent)
    void                     elm_progressbar_pulse_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool pulse)
    evas.c_evas.Eina_Bool    elm_progressbar_pulse_get(evas.c_evas.Evas_Object *obj)
    void                     elm_progressbar_pulse(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool state)
    void                     elm_progressbar_value_set(evas.c_evas.Evas_Object *obj, double val)
    double                   elm_progressbar_value_get(evas.c_evas.Evas_Object *obj)
    void                     elm_progressbar_span_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord size)
    evas.c_evas.Evas_Coord   elm_progressbar_span_size_get(evas.c_evas.Evas_Object *obj)
    void                     elm_progressbar_unit_format_set(evas.c_evas.Evas_Object *obj, char *format)
    char                    *elm_progressbar_unit_format_get(evas.c_evas.Evas_Object *obj)
    void                     elm_progressbar_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool horizontal)
    evas.c_evas.Eina_Bool    elm_progressbar_horizontal_get(evas.c_evas.Evas_Object *obj)
    void                     elm_progressbar_inverted_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool inverted)
    evas.c_evas.Eina_Bool    elm_progressbar_inverted_get(evas.c_evas.Evas_Object *obj)

    # Photo object (api:DONE  cb:DONE  test:TODO  doc:TODO)
    evas.c_evas.Evas_Object *elm_photo_add(evas.c_evas.Evas_Object *parent)
    evas.c_evas.Eina_Bool    elm_photo_file_set(evas.c_evas.Evas_Object *obj, char *file)
    void                     elm_photo_thumb_set(evas.c_evas.Evas_Object *obj, char *file, char *group)
    void                     elm_photo_size_set(evas.c_evas.Evas_Object *obj, int size)
    void                     elm_photo_fill_inside_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool fill)
    void                     elm_photo_editable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool editable)
    void                     elm_photo_aspect_fixed_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool fixed)
    evas.c_evas.Eina_Bool    elm_photo_aspect_fixed_get(evas.c_evas.Evas_Object *obj)

    # Radio object (api:DONE  cb:DONE  test:OK  doc:TODO)
    evas.c_evas.Evas_Object *elm_radio_add(evas.c_evas.Evas_Object *parent)
    void                     elm_radio_group_add(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *group)
    void                     elm_radio_state_value_set(evas.c_evas.Evas_Object *obj, int value)
    int                      elm_radio_state_value_get(evas.c_evas.Evas_Object *obj)
    void                     elm_radio_value_set(evas.c_evas.Evas_Object *obj, int value)
    int                      elm_radio_value_get(evas.c_evas.Evas_Object *obj)
    #void                    elm_radio_value_pointer_set(evas.c_evas.Evas_Object *obj, int *valuep)
    evas.c_evas.Evas_Object *elm_radio_selected_object_get(evas.c_evas.Evas_Object *obj)

    # Scroller object (api:DONE  cb:DONE  test:OK  doc:TODO)
    evas.c_evas.Evas_Object *elm_scroller_add(evas.c_evas.Evas_Object *parent)
    void                     elm_scroller_custom_widget_base_theme_set(evas.c_evas.Evas_Object *obj, char *widget, char *base)
    void                     elm_scroller_content_min_limit(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool w, evas.c_evas.Eina_Bool h)
    void                     elm_scroller_region_show(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord x, evas.c_evas.Evas_Coord y, evas.c_evas.Evas_Coord w, evas. c_evas.Evas_Coord h)
    void                     elm_scroller_policy_set(evas.c_evas.Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v)
    void                     elm_scroller_policy_get(evas.c_evas.Evas_Object *obj, Elm_Scroller_Policy *policy_h, Elm_Scroller_Policy *policy_v)
    void                     elm_scroller_region_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *x, evas.c_evas.Evas_Coord *y, evas.c_evas.Evas_Coord *w, evas.c_evas.Evas_Coord *h)
    void                     elm_scroller_child_size_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *w, evas.c_evas.Evas_Coord *h)
    void                     elm_scroller_bounce_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool h_bounce, evas.c_evas.Eina_Bool v_bounce)
    void                     elm_scroller_bounce_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool *h_bounce, evas.c_evas.Eina_Bool *v_bounce)
    void                     elm_scroller_page_relative_set(evas.c_evas.Evas_Object *obj, double h_pagerel, double v_pagerel)
    void                     elm_scroller_page_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord h_pagesize, evas.c_evas.Evas_Coord v_pagesize)
    void                     elm_scroller_current_page_get(evas.c_evas.Evas_Object *obj, int *h_pagenumber, int *v_pagenumber)
    void                     elm_scroller_last_page_get(evas.c_evas.Evas_Object *obj, int *h_pagenumber, int *v_pagenumber)
    void                     elm_scroller_page_show(evas.c_evas.Evas_Object *obj, int h_pagenumber, int v_pagenumber)
    void                     elm_scroller_page_bring_in(evas.c_evas.Evas_Object *obj, int h_pagenumber, int v_pagenumber)
    void                     elm_scroller_region_bring_in(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord x, evas.c_evas.Evas_Coord y, evas.c_evas.Evas_Coord w, evas.c_evas.Evas_Coord h)
    void                     elm_scroller_propagate_events_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool propagation)
    evas.c_evas.Eina_Bool    elm_scroller_propagate_events_get(evas.c_evas.Evas_Object *obj)
    void                     elm_scroller_gravity_set(evas.c_evas.Evas_Object *obj, double x, double y)
    void                     elm_scroller_gravity_get(evas.c_evas.Evas_Object *obj, double *x, double *y)

    # Slider object (api:DONEwithTODO  cb:DONE  test:TODO  doc:TODO)
    evas.c_evas.Evas_Object *elm_slider_add(evas.c_evas.Evas_Object *parent)
    void                     elm_slider_span_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord size)
    evas.c_evas.Evas_Coord   elm_slider_span_size_get(evas.c_evas.Evas_Object *obj)
    void                     elm_slider_unit_format_set(evas.c_evas.Evas_Object *obj, char *format)
    char                    *elm_slider_unit_format_get(evas.c_evas.Evas_Object *obj)
    void                     elm_slider_indicator_format_set(evas.c_evas.Evas_Object *obj, char *indicator)
    char                    *elm_slider_indicator_format_get(evas.c_evas.Evas_Object *obj)
    void                     elm_slider_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool horizontal)
    evas.c_evas.Eina_Bool    elm_slider_horizontal_get(evas.c_evas.Evas_Object *obj)
    void                     elm_slider_min_max_set(evas.c_evas.Evas_Object *obj, double min, double max)
    void                     elm_slider_min_max_get(evas.c_evas.Evas_Object *obj, double *min, double *max)
    void                     elm_slider_value_set(evas.c_evas.Evas_Object *obj, double val)
    double                   elm_slider_value_get(evas.c_evas.Evas_Object *obj)
    void                     elm_slider_inverted_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool inverted)
    evas.c_evas.Eina_Bool    elm_slider_inverted_get(evas.c_evas.Evas_Object *obj)
    void                     elm_slider_indicator_show_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool show)
    evas.c_evas.Eina_Bool    elm_slider_indicator_show_get(evas.c_evas.Evas_Object *obj)

    # Spinner object (api:DONE  cb:DONE  test:OK  doc:TODO)
    evas.c_evas.Evas_Object *elm_spinner_add(evas.c_evas.Evas_Object *parent)
    void                     elm_spinner_label_format_set(evas.c_evas.Evas_Object *obj, char *format)
    char                    *elm_spinner_label_format_get(evas.c_evas.Evas_Object *obj)
    void                     elm_spinner_min_max_set(evas.c_evas.Evas_Object *obj, double min, double max)
    void                     elm_spinner_min_max_get(evas.c_evas.Evas_Object *obj, double *min, double *max)
    void                     elm_spinner_step_set(evas.c_evas.Evas_Object *obj, double step)
    double                   elm_spinner_step_get(evas.c_evas.Evas_Object *obj)
    void                     elm_spinner_value_set(evas.c_evas.Evas_Object *obj, double val)
    double                   elm_spinner_value_get(evas.c_evas.Evas_Object *obj)
    void                     elm_spinner_wrap_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool wrap)
    evas.c_evas.Eina_Bool    elm_spinner_wrap_get(evas.c_evas.Evas_Object *obj)
    void                     elm_spinner_editable_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool editable)
    evas.c_evas.Eina_Bool    elm_spinner_editable_get(evas.c_evas.Evas_Object *obj)
    void                     elm_spinner_special_value_add(evas.c_evas.Evas_Object *obj, double value, char *label)
    void                     elm_spinner_interval_set(evas.c_evas.Evas_Object *obj, double interval)
    double                   elm_spinner_interval_get(evas.c_evas.Evas_Object *obj)
    void                     elm_spinner_base_set(evas.c_evas.Evas_Object *obj, double base)
    double                   elm_spinner_base_get(evas.c_evas.Evas_Object *obj)
    void                     elm_spinner_round_set(evas.c_evas.Evas_Object *obj, int rnd)
    int                      elm_spinner_round_get(evas.c_evas.Evas_Object *obj)

    # Table object (api:DONE  cb:DONE  test:DONE  doc:TODO)
    evas.c_evas.Evas_Object *elm_table_add(evas.c_evas.Evas_Object *parent)
    void                     elm_table_homogeneous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool homogeneous)
    evas.c_evas.Eina_Bool    elm_table_homogeneous_get(evas.c_evas.Evas_Object *obj)
    void                     elm_table_padding_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord horizontal, evas.c_evas.Evas_Coord vertical)
    void                     elm_table_padding_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *horizontal, evas.c_evas.Evas_Coord *vertical)
    void                     elm_table_pack(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj, int x, int y, int w, int h)
    void                     elm_table_unpack(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *subobj)
    void                     elm_table_clear(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool clear)
    void                     elm_table_pack_set(evas.c_evas.Evas_Object *subobj, int x, int y, int w, int h)
    void                     elm_table_pack_get(evas.c_evas.Evas_Object *subobj, int *x, int *y, int *w, int *h)

    # Toolbar object (api:DONEwithTODO  cb:DONE  test:OK  doc:TODO)
    evas.c_evas.Evas_Object *elm_toolbar_add(evas.c_evas.Evas_Object *parent)
    void                     elm_toolbar_icon_size_set(evas.c_evas.Evas_Object *obj, int icon_size)
    int                      elm_toolbar_icon_size_get(evas.c_evas.Evas_Object *obj)
    void                     elm_toolbar_icon_order_lookup_set(evas.c_evas.Evas_Object *obj, Elm_Icon_Lookup_Order order)
    Elm_Icon_Lookup_Order    elm_toolbar_icon_order_lookup_get(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item         *elm_toolbar_item_append(evas.c_evas.Evas_Object *obj, char *icon, char *label, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Object_Item         *elm_toolbar_item_prepend(evas.c_evas.Evas_Object *obj, char *icon, char *label, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Object_Item         *elm_toolbar_item_insert_before(evas.c_evas.Evas_Object *obj, Elm_Object_Item *before, char *icon, char *label, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Object_Item         *elm_toolbar_item_insert_after(evas.c_evas.Evas_Object *obj, Elm_Object_Item *after, char *icon, char *label, void (*func) (void *data, evas.c_evas.Evas_Object *obj, void *event_info), void *data)
    Elm_Object_Item         *elm_toolbar_first_item_get(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item         *elm_toolbar_last_item_get(evas.c_evas.Evas_Object *obj)
    Elm_Object_Item         *elm_toolbar_item_next_get(Elm_Object_Item *item)
    Elm_Object_Item         *elm_toolbar_item_prev_get(Elm_Object_Item *item)
    void                     elm_toolbar_item_priority_set(Elm_Object_Item *item, int priority)
    int                      elm_toolbar_item_priority_get(Elm_Object_Item *item)
    Elm_Object_Item          elm_toolbar_item_find_by_label(evas.c_evas.Evas_Object *obj, char *label)
    evas.c_evas.Eina_Bool    elm_toolbar_item_selected_get(Elm_Object_Item *item)
    void                     elm_toolbar_item_selected_set(Elm_Object_Item *item, evas.c_evas.Eina_Bool selected)
    Elm_Object_Item         *elm_toolbar_selected_item_get(evas.c_evas.Evas_Object *obj)
    void                     elm_toolbar_item_icon_set(Elm_Object_Item *item, char *icon)
    char                    *elm_toolbar_item_icon_get(Elm_Object_Item *item)
    evas.c_evas.Evas_Object *elm_toolbar_item_object_get(Elm_Object_Item *item)
    evas.c_evas.Evas_Object *elm_toolbar_item_icon_object_get(Elm_Object_Item *item)
    #evas.c_evas.Eina_Bool   elm_toolbar_item_icon_memfile_set(Elm_Object_Item *item, char *img, char *size, char *format, char *key)
    evas.c_evas.Eina_Bool    elm_toolbar_item_icon_file_set(Elm_Object_Item *item, char *file, char *key)
    void                     elm_toolbar_item_separator_set(Elm_Object_Item *item, evas.c_evas.Eina_Bool separator)
    evas.c_evas.Eina_Bool    elm_toolbar_item_separator_get(Elm_Object_Item *item)
    void                     elm_toolbar_shrink_mode_set(evas.c_evas.Evas_Object *obj, Elm_Toolbar_Shrink_Mode shrink_mode)
    Elm_Toolbar_Shrink_Mode  elm_toolbar_shrink_mode_get(evas.c_evas.Evas_Object *obj)
    void                     elm_toolbar_homogeneous_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool homogeneous)
    evas.c_evas.Eina_Bool    elm_toolbar_homogeneous_get(evas.c_evas.Evas_Object *obj)
    void                     elm_toolbar_menu_parent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *parent)
    evas.c_evas.Evas_Object *elm_toolbar_menu_parent_get(evas.c_evas.Evas_Object *obj)
    void                     elm_toolbar_align_set(evas.c_evas.Evas_Object *obj, double align)
    double                   elm_toolbar_align_get(evas.c_evas.Evas_Object *obj)
    void                     elm_toolbar_item_menu_set(Elm_Object_Item *item, evas.c_evas.Eina_Bool menu)
    evas.c_evas.Evas_Object *elm_toolbar_item_menu_get(Elm_Object_Item *item)
    #Elm_Toolbar_Item_State  *elm_toolbar_item_state_add(Elm_Object_Item *item, char *icon, char *label, Evas_Smart_Cb func, void *data)
    #evas.c_evas.Eina_Bool    elm_toolbar_item_state_del(Elm_Object_Item *item, Elm_Toolbar_Item_State *state)
    #evas.c_evas.Eina_Bool    elm_toolbar_item_state_set(Elm_Object_Item *item, Elm_Toolbar_Item_State *state)
    #void                     elm_toolbar_item_state_unset(Elm_Object_Item *item)
    #Elm_Toolbar_Item_State  *elm_toolbar_item_state_get(Elm_Object_Item *item)
    #Elm_Toolbar_Item_State  *elm_toolbar_item_state_next(Elm_Object_Item *item)
    #Elm_Toolbar_Item_State  *elm_toolbar_item_state_prev(Elm_Object_Item *item)
    void                     elm_toolbar_horizontal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool horizontal)
    evas.c_evas.Eina_Bool    elm_toolbar_horizontal_get(evas.c_evas.Evas_Object *obj)
    unsigned int             elm_toolbar_items_count(evas.c_evas.Evas_Object *obj)
    void                     elm_toolbar_select_mode_set(evas.c_evas.Evas_Object *obj, Elm_Object_Select_Mode mode)
    Elm_Object_Select_Mode   elm_toolbar_select_mode_get(evas.c_evas.Evas_Object *obj)

    # Window object (api:DONE  cb:DONE  test:TODO  doc:DONE)
    evas.c_evas.Evas_Object *elm_win_add(evas.c_evas.Evas_Object* parent, char *name, Elm_Win_Type type)
    void                     elm_win_resize_object_add(evas.c_evas.Evas_Object* obj, evas.c_evas.Evas_Object* subobj)
    void                     elm_win_resize_object_del(evas.c_evas.Evas_Object* obj, evas.c_evas.Evas_Object* subobj)
    void                     elm_win_title_set(evas.c_evas.Evas_Object* obj, char *title)
    char                    *elm_win_title_get(evas.c_evas.Evas_Object* obj)
    void                     elm_win_icon_name_set(evas.c_evas.Evas_Object *obj, char *icon_name)
    char                    *elm_win_icon_name_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_role_set(evas.c_evas.Evas_Object *obj, char *role)
    char                    *elm_win_role_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_icon_object_set(evas.c_evas.Evas_Object* obj, evas.c_evas.Evas_Object* icon)
    evas.c_evas.Evas_Object *elm_win_icon_object_get(evas.c_evas.Evas_Object*)
    void                     elm_win_autodel_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool autodel)
    evas.c_evas.Eina_Bool    elm_win_autodel_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_activate(evas.c_evas.Evas_Object *obj)
    void                     elm_win_lower(evas.c_evas.Evas_Object *obj)
    void                     elm_win_raise(evas.c_evas.Evas_Object *obj)
    void                     elm_win_center(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool h, evas.c_evas.Eina_Bool v)
    void                     elm_win_borderless_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool borderless)
    evas.c_evas.Eina_Bool    elm_win_borderless_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_shaped_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool shaped)
    evas.c_evas.Eina_Bool    elm_win_shaped_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_alpha_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool alpha)
    evas.c_evas.Eina_Bool    elm_win_alpha_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_override_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool override)
    evas.c_evas.Eina_Bool    elm_win_override_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_fullscreen_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool fullscreen)
    evas.c_evas.Eina_Bool    elm_win_fullscreen_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_maximized_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool maximized)
    evas.c_evas.Eina_Bool    elm_win_maximized_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_iconified_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool iconified)
    evas.c_evas.Eina_Bool    elm_win_iconified_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_withdrawn_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool withdrawn)
    evas.c_evas.Eina_Bool    elm_win_withdrawn_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_urgent_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool urgent)
    evas.c_evas.Eina_Bool    elm_win_urgent_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_demand_attention_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool demand_attention)
    evas.c_evas.Eina_Bool    elm_win_demand_attention_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_modal_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool modal)
    evas.c_evas.Eina_Bool    elm_win_modal_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_aspect_set(evas.c_evas.Evas_Object *obj, double aspect)
    double                   elm_win_aspect_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_layer_set(evas.c_evas.Evas_Object *obj, int layer)
    int                      elm_win_layer_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_rotation_set(evas.c_evas.Evas_Object *obj, int rotation)
    void                     elm_win_rotation_with_resize_set(evas.c_evas.Evas_Object *obj, int rotation)
    int                      elm_win_rotation_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_sticky_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool sticky)
    evas.c_evas.Eina_Bool    elm_win_sticky_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_conformant_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool conformant)
    evas.c_evas.Eina_Bool    elm_win_conformant_get(evas.c_evas.Evas_Object *obj)

    void                     elm_win_quickpanel_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool quickpanel)
    evas.c_evas.Eina_Bool    elm_win_quickpanel_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_quickpanel_priority_major_set(evas.c_evas.Evas_Object *obj, int priority)
    int                      elm_win_quickpanel_priority_major_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_quickpanel_priority_minor_set(evas.c_evas.Evas_Object *obj, int priority)
    int                      elm_win_quickpanel_priority_minor_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_quickpanel_zone_set(evas.c_evas.Evas_Object *obj, int zone)
    int                      elm_win_quickpanel_zone_get(evas.c_evas.Evas_Object *obj)

    void                     elm_win_prop_focus_skip_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool skip)
    void                     elm_win_illume_command_send(evas.c_evas.Evas_Object *obj, Elm_Illume_Command command, params)
    evas.c_evas.Evas_Object *elm_win_inlined_image_object_get(evas.c_evas.Evas_Object *obj)
    evas.c_evas.Eina_Bool    elm_win_focus_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_screen_constrain_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool constrain)
    evas.c_evas.Eina_Bool    elm_win_screen_constrain_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_screen_size_get(evas.c_evas.Evas_Object *obj, int *x, int *y, int *w, int *h)

    void                     elm_win_focus_highlight_enabled_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool enabled)
    evas.c_evas.Eina_Bool    elm_win_focus_highlight_enabled_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_focus_highlight_style_set(evas.c_evas.Evas_Object *obj, char *style)
    char                    *elm_win_focus_highlight_style_get(evas.c_evas.Evas_Object *obj)

    void                     elm_win_keyboard_mode_set(evas.c_evas.Evas_Object *obj, Elm_Win_Keyboard_Mode mode)
    Elm_Win_Keyboard_Mode    elm_win_keyboard_mode_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_keyboard_win_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_Bool is_keyboard)
    evas.c_evas.Eina_Bool    elm_win_keyboard_win_get(evas.c_evas.Evas_Object *obj)

    void                     elm_win_indicator_mode_set(evas.c_evas.Evas_Object *obj, Elm_Win_Indicator_Mode mode)
    Elm_Win_Indicator_Mode   elm_win_indicator_mode_get(evas.c_evas.Evas_Object *obj)
    void                     elm_win_indicator_opacity_set(evas.c_evas.Evas_Object *obj, Elm_Win_Indicator_Opacity_Mode mode)
    Elm_Win_Indicator_Opacity_Mode elm_win_indicator_opacity_get(evas.c_evas.Evas_Object *obj)

    void                     elm_win_screen_position_get(evas.c_evas.Evas_Object *obj, int *x, int *y)
    evas.c_evas.Eina_Bool    elm_win_socket_listen(evas.c_evas.Evas_Object *obj, char *svcname, int svcnum, evas.c_evas.Eina_Bool svcsys)
