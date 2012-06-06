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

cdef extern from *:
    ctypedef char* const_char_ptr "const char *"
    ctypedef void const_void "const void"

cdef extern from "string.h":
    void *memcpy(void *dst, void *src, int n)
    char *strdup(char *str)

cdef extern from "Ecore.h":
    ctypedef void (*Ecore_Cb)(void *data)

cdef extern from "Ecore_X.h":
    ctypedef unsigned int Ecore_X_ID
    ctypedef Ecore_X_ID Ecore_X_Window

cdef extern from "Evas.h":
    ctypedef struct Evas_Object
    ctypedef Evas_Object const_Evas_Object "const Evas_Object"
    ctypedef int Evas_Coord
    ctypedef int Evas_Callback_Type
    ctypedef void (*Evas_Smart_Cb)(void *data, Evas_Object *obj, void *event_info)
    ctypedef int Evas_Font_Size

    ctypedef void *Evas_Object_Box_Data
    ctypedef void *Elm_Box_Transition
    ctypedef void (*Evas_Object_Box_Layout)(Evas_Object *o, Evas_Object_Box_Data *priv, void *user_data)
    void evas_object_box_layout_horizontal(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)
    void evas_object_box_layout_vertical(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)
    void evas_object_box_layout_homogeneous_vertical(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)
    void evas_object_box_layout_homogeneous_horizontal(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)
    void evas_object_box_layout_homogeneous_max_size_horizontal(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)
    void evas_object_box_layout_homogeneous_max_size_vertical(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)
    void evas_object_box_layout_flow_horizontal(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)
    void evas_object_box_layout_flow_vertical(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)
    void evas_object_box_layout_stack(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)
    void elm_box_layout_transition(Evas_Object *o, Evas_Object_Box_Data *priv, void *data)

cdef extern from "Eina.h":
    ctypedef struct Eina_List:
        void      *data
        Eina_List *next
        Eina_List *prev
        void      *accounting
    ctypedef Eina_List const_Eina_List "const Eina_List"
    ctypedef unsigned char Eina_Bool
    ctypedef struct Eina_Rectangle:
        int x
        int y
        int w
        int h
    ctypedef int (*Eina_Compare_Cb)(void *data1, void *data2)

cdef extern from "Edje.h":
    ctypedef void (*Edje_Signal_Cb)(void *data, Evas_Object *obj, const_char_ptr emission, const_char_ptr source)

# For Debugging
"""
cdef extern from "Ecore_Evas.h":
    ctypedef struct Ecore_Evas
    void ecore_evas_geometry_get(Ecore_Evas *ee, int *x, int *y, int *w, int *h)

cdef extern from "Ecore.h":
    ctypedef struct Ecore_Job

ctypedef struct Elm_Win:
    Ecore_Evas *ee
    evas.c_evas.Evas *evas
    Evas_Object *parent
    Evas_Object *win_obj
    Eina_List *subobjs
    Ecore_X_ID xwin
    Ecore_Job *deferred_resize_job
    Ecore_Job *deferred_child_eval_job
    Elm_Win_Type type
    Elm_Win_Keyboard_Mode kbdmode
    Eina_Bool autodel = 1
"""

cdef extern from "Elementary.h":

    # enums
    ctypedef enum Elm_Actionslider_Pos:
       ELM_ACTIONSLIDER_NONE
       ELM_ACTIONSLIDER_LEFT
       ELM_ACTIONSLIDER_CENTER
       ELM_ACTIONSLIDER_RIGHT
       ELM_ACTIONSLIDER_ALL

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

    ctypedef enum Elm_Calendar_Mark_Repeat_Type:
        ELM_CALENDAR_UNIQUE
        ELM_CALENDAR_DAILY
        ELM_CALENDAR_WEEKLY
        ELM_CALENDAR_MONTHLY
        ELM_CALENDAR_ANNUALLY

    ctypedef enum Elm_Calendar_Select_Mode:
        ELM_CALENDAR_SELECT_MODE_DEFAULT
        ELM_CALENDAR_SELECT_MODE_ALWAYS
        ELM_CALENDAR_SELECT_MODE_NONE
        ELM_CALENDAR_SELECT_MODE_ONDEMAND

    ctypedef enum Elm_Ctxpopup_Direction:
        ELM_CTXPOPUP_DIRECTION_DOWN
        ELM_CTXPOPUP_DIRECTION_RIGHT
        ELM_CTXPOPUP_DIRECTION_LEFT
        ELM_CTXPOPUP_DIRECTION_UP
        ELM_CTXPOPUP_DIRECTION_UNKNOWN

    ctypedef enum Elm_Calendar_Weekday:
        ELM_DAY_SUNDAY
        ELM_DAY_MONDAY
        ELM_DAY_TUESDAY
        ELM_DAY_WEDNESDAY
        ELM_DAY_THURSDAY
        ELM_DAY_FRIDAY
        ELM_DAY_SATURDAY
        ELM_DAY_LAST

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

    ctypedef enum Elm_Colorselector_Mode:
        ELM_COLORSELECTOR_PALETTE
        ELM_COLORSELECTOR_COMPONENTS
        ELM_COLORSELECTOR_BOTH

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

    ctypedef enum Elm_Focus_Direction:
        ELM_FOCUS_PREVIOUS
        ELM_FOCUS_NEXT

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
    #ctypedef struct Elm_Calendar_Mark:
        #Evas_Object *obj
        #Eina_List *node
        #struct tm mark_time
        #const_char_ptr mark_type
        #Elm_Calendar_Mark_Repeat_Type repeat

    ctypedef struct Elm_Color_RGBA:
        unsigned int r
        unsigned int g
        unsigned int b
        unsigned int a

    ctypedef struct _Elm_Custom_Palette:
        const_char_ptr palette_name
        Eina_List *color_list

    ctypedef struct Elm_Entry_Anchor_Info:
        char *name
        int   button
        Evas_Coord x
        Evas_Coord y
        Evas_Coord w
        Evas_Coord h

    ctypedef struct Elm_Entry_Anchor_Hover_Info:
        Elm_Entry_Anchor_Info *anchor_info
        Evas_Object *hover
        Eina_Rectangle hover_parent
        Eina_Bool hover_left
        Eina_Bool hover_right
        Eina_Bool hover_top
        Eina_Bool hover_bottom

    ctypedef struct Elm_Toolbar_Item_State:
        char *icon
        char *label
        Evas_Smart_Cb func
        void *data

    ctypedef char *(*GenlistItemLabelGetFunc)(void *data, Evas_Object *obj, const_char_ptr part)
    ctypedef Evas_Object *(*GenlistItemIconGetFunc)(void *data, Evas_Object *obj, const_char_ptr part)
    ctypedef Eina_Bool (*GenlistItemStateGetFunc)(void *data, Evas_Object *obj, const_char_ptr part)
    ctypedef void (*GenlistItemDelFunc)(void *data, Evas_Object *obj)

    ctypedef struct Elm_Genlist_Item_Class_Func:
        GenlistItemLabelGetFunc text_get
        GenlistItemIconGetFunc content_get
        GenlistItemStateGetFunc state_get
        GenlistItemDelFunc del_ "del"

    ctypedef struct Elm_Genlist_Item_Class:
        char *item_style
        Elm_Genlist_Item_Class_Func func

    ctypedef char *(*GengridItemLabelGetFunc)(void *data, Evas_Object *obj, const_char_ptr part)
    ctypedef Evas_Object *(*GengridItemIconGetFunc)(void *data, Evas_Object *obj, const_char_ptr part)
    ctypedef Eina_Bool (*GengridItemStateGetFunc)(void *data, Evas_Object *obj, const_char_ptr part)
    ctypedef void (*GengridItemDelFunc)(void *data, Evas_Object *obj)

    ctypedef struct Elm_Gengrid_Item_Class_Func:
        GengridItemLabelGetFunc text_get
        GengridItemIconGetFunc content_get
        GengridItemStateGetFunc state_get
        GengridItemDelFunc del_ "del"

    ctypedef struct Elm_Gengrid_Item_Class:
        char *item_style
        Elm_Gengrid_Item_Class_Func func

    ctypedef Evas_Object *(*Elm_Tooltip_Content_Cb) (void *data, Evas_Object *obj, Evas_Object *tooltip)
    ctypedef Evas_Object *(*Elm_Tooltip_Item_Content_Cb) (void *data, Evas_Object *obj, Evas_Object *tooltip, void *item)
    ctypedef Eina_Bool (*Elm_Event_Cb) (void *data, Evas_Object *obj, Evas_Object *src, Evas_Callback_Type t, void *event_info)
    #ctypedef char * (*Elm_Calendar_Format_Cb)(struct tm *stime)

    ctypedef struct Elm_Object_Item
    ctypedef Elm_Object_Item const_Elm_Object_Item "const Elm_Object_Item"
    ctypedef struct Elm_Theme

    # General
    void                     elm_init(int argc, char** argv)
    void                     elm_shutdown()
    void                     elm_run() nogil
    void                     elm_exit()

    # General - Quicklaunch (XXX: Only used by macros?)
    void                     elm_quicklaunch_init(int argc, char **argv)
    void                     elm_quicklaunch_sub_init(int argc, char **argv)
    void                     elm_quicklaunch_sub_shutdown()
    void                     elm_quicklaunch_shutdown()
    void                     elm_quicklaunch_seed()
    Eina_Bool                elm_quicklaunch_prepare(int argc, char **argv)
    Eina_Bool                elm_quicklaunch_fork(int argc, char **argv, char *cwd, void (*postfork_func) (void *data), void *postfork_data)
    void                     elm_quicklaunch_cleanup()
    int                      elm_quicklaunch_fallback(int argc, char **argv)
    char                    *elm_quicklaunch_exe_path_get(char *exe)

    # General - Policy
    Eina_Bool                elm_policy_set(unsigned int policy, int value)
    int                      elm_policy_get(unsigned int policy)

    # General - Language
    void                     elm_language_set(const_char_ptr lang)

    # Config
    Eina_Bool                elm_config_save()
    void                     elm_config_reload()
    void                     elm_config_all_flush()
    char                    *elm_config_profile_get()
    char                    *elm_config_profile_dir_get(char *profile, Eina_Bool is_user)
    void                     elm_config_profile_dir_free(char *p_dir)
    Eina_List               *elm_config_profile_list_get()
    void                     elm_config_profile_list_free(Eina_List *l)
    void                     elm_config_profile_set(char *profile)
    Eina_Bool                elm_config_scroll_bounce_enabled_get()
    void                     elm_config_scroll_bounce_enabled_set(Eina_Bool enabled)
    double                   elm_config_scroll_bounce_friction_get()
    void                     elm_config_scroll_bounce_friction_set(double friction)
    double                   elm_config_scroll_page_scroll_friction_get()
    void                     elm_config_scroll_page_scroll_friction_set(double friction)
    double                   elm_config_scroll_bring_in_scroll_friction_get()
    void                     elm_config_scroll_bring_in_scroll_friction_set(double friction)
    double                   elm_config_scroll_zoom_friction_get()
    void                     elm_config_scroll_zoom_friction_set(double friction)
    Eina_Bool                elm_config_scroll_thumbscroll_enabled_get()
    void                     elm_config_scroll_thumbscroll_enabled_set(Eina_Bool enabled)
    unsigned int             elm_config_scroll_thumbscroll_threshold_get()
    void                     elm_config_scroll_thumbscroll_threshold_set(unsigned int threshold)
    double                   elm_config_scroll_thumbscroll_momentum_threshold_get()
    void                     elm_config_scroll_thumbscroll_momentum_threshold_set(double threshold)
    double                   elm_config_scroll_thumbscroll_friction_get()
    void                     elm_config_scroll_thumbscroll_friction_set(double friction)
    double                   elm_config_scroll_thumbscroll_border_friction_get()
    void                     elm_config_scroll_thumbscroll_border_friction_set(double friction)
    double                   elm_config_scroll_thumbscroll_sensitivity_friction_get()
    void                     elm_config_scroll_thumbscroll_sensitivity_friction_set(double friction)
    double                   elm_config_longpress_timeout_get()
    void                     elm_config_longpress_timeout_set(double longpress_timeout)
    double                   elm_config_tooltip_delay_get()
    Eina_Bool                elm_config_tooltip_delay_set(double delay)
    int                      elm_config_cursor_engine_only_get()
    Eina_Bool                elm_config_cursor_engine_only_set(int engine_only)
    double                   elm_config_scale_get()
    void                     elm_config_scale_set(double scale)
    Eina_Bool                elm_config_password_show_last_get()
    void                     elm_config_password_show_last_set(Eina_Bool password_show_last)
    double                   elm_config_password_show_last_timeout_get()
    void                     elm_config_password_show_last_timeout_set(double password_show_last_timeout)
    char                    *elm_config_engine_get()
    void                     elm_config_engine_set(const_char_ptr engine)
    char                    *elm_config_preferred_engine_get()
    void                     elm_config_preferred_engine_set(const_char_ptr engine)
    Eina_List               *elm_config_text_classes_list_get()
    void                     elm_config_text_classes_list_free(Eina_List *list)
    Eina_List               *elm_config_font_overlay_list_get()
    void                     elm_config_font_overlay_set(char *text_class, char *font, Evas_Font_Size size)
    void                     elm_config_font_overlay_unset(char *text_class)
    void                     elm_config_font_overlay_apply()
    Evas_Coord               elm_config_finger_size_get()
    void                     elm_config_finger_size_set(Evas_Coord size)
    int                      elm_config_cache_flush_interval_get()
    void                     elm_config_cache_flush_interval_set(int size)
    Eina_Bool                elm_config_cache_flush_enabled_get()
    void                     elm_config_cache_flush_enabled_set(Eina_Bool enabled)
    int                      elm_config_cache_font_cache_size_get()
    void                     elm_config_cache_font_cache_size_set(int size)
    int                      elm_config_cache_image_cache_size_get()
    void                     elm_config_cache_image_cache_size_set(int size)
    int                      elm_config_cache_edje_file_cache_size_get()
    void                     elm_config_cache_edje_file_cache_size_set(int size)
    int                      elm_config_cache_edje_collection_cache_size_get()
    void                     elm_config_cache_edje_collection_cache_size_set(int size)
    Eina_Bool                elm_config_focus_highlight_enabled_get()
    void                     elm_config_focus_highlight_enabled_set(Eina_Bool enable)
    Eina_Bool                elm_config_focus_highlight_animate_get()
    void                     elm_config_focus_highlight_animate_set(Eina_Bool animate)
    Eina_Bool                elm_config_mirrored_get()
    void                     elm_config_mirrored_set(Eina_Bool mirrored)

    # Finger
    void                     elm_coords_finger_size_adjust(int times_w, Evas_Coord *w, int times_h, Evas_Coord *h)

    # Theme
    Elm_Theme               *elm_theme_new()
    void                     elm_theme_free(Elm_Theme *th)
    void                     elm_theme_copy(Elm_Theme *th, Elm_Theme *thdst)
    void                     elm_theme_ref_set(Elm_Theme *th, Elm_Theme *thref)
    Elm_Theme               *elm_theme_ref_get(Elm_Theme *th)
    Elm_Theme               *elm_theme_default_get()
    void                     elm_theme_overlay_add(Elm_Theme *th, char *item)
    void                     elm_theme_overlay_del(Elm_Theme *th, char *item)
    Eina_List               *elm_theme_overlay_list_get(Elm_Theme *th)
    void                     elm_theme_extension_add(Elm_Theme *th, char *item)
    void                     elm_theme_extension_del(Elm_Theme *th, char *item)
    Eina_List               *elm_theme_extension_list_get(Elm_Theme *th)
    void                     elm_theme_set(Elm_Theme *th, char *theme)
    char                    *elm_theme_get(Elm_Theme *th)
    Eina_List               *elm_theme_list_get(Elm_Theme *th)
    char                    *elm_theme_list_item_path_get(char *f, Eina_Bool *in_search_path)
    void                     elm_theme_flush(Elm_Theme *th)
    void                     elm_theme_full_flush()
    Eina_List               *elm_theme_name_available_list_new()
    void                     elm_theme_name_available_list_free(Eina_List *list)
    char                    *elm_theme_data_get(Elm_Theme *th, char *key)

    # Object handling
    void                     elm_object_part_text_set(Evas_Object *obj, const_char_ptr part, const_char_ptr label)
    void                     elm_object_text_set(Evas_Object *obj, const_char_ptr label)
    char*                    elm_object_part_text_get(Evas_Object *obj, const_char_ptr part)
    char*                    elm_object_text_get(Evas_Object *obj)
    void                     elm_object_part_content_set(Evas_Object *obj, char *part, Evas_Object *icon)
    void                     elm_object_content_set(Evas_Object *obj, Evas_Object *icon)
    Evas_Object             *elm_object_part_content_get(Evas_Object *obj, char *part)
    Evas_Object             *elm_object_content_get(Evas_Object *obj)
    Evas_Object             *elm_object_part_content_unset(Evas_Object *obj, char *part)
    Evas_Object             *elm_object_content_unset(Evas_Object *obj)
    void                     elm_object_access_info_set(Evas_Object *obj, char *txt)
    Evas_Object             *elm_object_name_find(Evas_Object *obj, char *name, int recurse)
    void                     elm_object_style_set(Evas_Object *obj, char *style)
    char                    *elm_object_style_get(Evas_Object *obj)
    void                     elm_object_disabled_set(Evas_Object *obj, Eina_Bool disabled)
    Eina_Bool                elm_object_disabled_get(Evas_Object *obj)
    Eina_Bool                elm_object_widget_check(Evas_Object *obj)
    Evas_Object             *elm_object_parent_widget_get(Evas_Object *obj)
    Evas_Object             *elm_object_top_widget_get(Evas_Object *obj)
    char                    *elm_object_widget_type_get(Evas_Object *obj)
    void                     elm_object_signal_emit(Evas_Object *obj, char *emission, char *source)
    void                     elm_object_signal_callback_add(Evas_Object *obj, char *emission, char *source, Edje_Signal_Cb func, void *data)
    void                    *elm_object_signal_callback_del(Evas_Object *obj, char *emission, char *source, Edje_Signal_Cb func)
    void                     elm_object_event_callback_add(Evas_Object *obj, Elm_Event_Cb func, const_void *data)
    void                    *elm_object_event_callback_del(Evas_Object *obj, Elm_Event_Cb func, const_void *data)

    # Object - Cursors (elm_cursor.h)
    void                     elm_object_cursor_set(Evas_Object *obj, char *cursor)
    char                    *elm_object_cursor_get(Evas_Object *obj)
    void                     elm_object_cursor_unset(Evas_Object *obj)
    void                     elm_object_cursor_style_set(Evas_Object *obj, char *style)
    char*                    elm_object_cursor_style_get(Evas_Object *obj)
    void                     elm_object_cursor_theme_search_enabled_set(Evas_Object *obj, Eina_Bool theme_search)
    Eina_Bool                elm_object_cursor_theme_search_enabled_get(Evas_Object *obj)

    # Object - Focus (elm_focus.h)
    Eina_Bool                elm_object_focus_get(Evas_Object *obj)
    void                     elm_object_focus_set(Evas_Object *obj, Eina_Bool focus)
    void                     elm_object_focus_allow_set(Evas_Object *obj, Eina_Bool enable)
    Eina_Bool                elm_object_focus_allow_get(Evas_Object *obj)
    void                     elm_object_focus_custom_chain_set(Evas_Object *obj, Eina_List *objs)
    void                     elm_object_focus_custom_chain_unset(Evas_Object *obj)
    const_Eina_List         *elm_object_focus_custom_chain_get(const_Evas_Object *obj)
    void                     elm_object_focus_custom_chain_append(Evas_Object *obj, Evas_Object *child, Evas_Object *relative_child)
    void                     elm_object_focus_custom_chain_prepend(Evas_Object *obj, Evas_Object *child, Evas_Object *relative_child)
    void                     elm_object_focus_next(Evas_Object *obj, Elm_Focus_Direction direction)
    void                     elm_object_tree_focus_allow_set(Evas_Object *obj, Eina_Bool focusable)
    Eina_Bool                elm_object_tree_focus_allow_get(Evas_Object *obj)

    # Object - Mirroring (elm_mirroring.h)
    Eina_Bool                elm_object_mirrored_get(Evas_Object *obj)
    void                     elm_object_mirrored_set(Evas_Object *obj, Eina_Bool mirrored)
    Eina_Bool                elm_object_mirrored_automatic_get(Evas_Object *obj)
    void                     elm_object_mirrored_automatic_set(Evas_Object *obj, Eina_Bool automatic)

    # Object - Scaling (elm_scale.h)
    void                     elm_object_scale_set(Evas_Object *obj, double scale)
    double                   elm_object_scale_get(Evas_Object *obj)

    # Object - Scrollhints (elm_scroll.h)
    void                     elm_object_scroll_hold_push(Evas_Object *obj)
    void                     elm_object_scroll_hold_pop(Evas_Object *obj)
    void                     elm_object_scroll_freeze_push(Evas_Object *obj)
    void                     elm_object_scroll_freeze_pop(Evas_Object *obj)
    void                     elm_object_scroll_lock_x_set(Evas_Object *obj, Eina_Bool lock)
    void                     elm_object_scroll_lock_y_set(Evas_Object *obj, Eina_Bool lock)
    Eina_Bool                elm_object_scroll_lock_x_get(Evas_Object *obj)
    Eina_Bool                elm_object_scroll_lock_y_get(Evas_Object *obj)

    # Object - Theme (elm_theme.h)
    void                     elm_object_theme_set(Evas_Object *obj, Elm_Theme *th)
    Elm_Theme               *elm_object_theme_get(Evas_Object *obj)

    # Object - Tooltips (elm_tooltip.h)
    void                     elm_object_tooltip_show(Evas_Object *obj)
    void                     elm_object_tooltip_hide(Evas_Object *obj)
    void                     elm_object_tooltip_text_set(Evas_Object *obj, char *text)
    void                     elm_object_tooltip_domain_translatable_text_set(Evas_Object *obj, char *domain, char *text)
    void                     elm_object_tooltip_translatable_text_set(Evas_Object *obj, char *text)
    void                     elm_object_tooltip_content_cb_set(Evas_Object *obj, Elm_Tooltip_Content_Cb func, void *data, Evas_Smart_Cb del_cb)
    void                     elm_object_tooltip_unset(Evas_Object *obj)
    void                     elm_object_tooltip_style_set(Evas_Object *obj, char *style)
    char                    *elm_object_tooltip_style_get(Evas_Object *obj)
    Eina_Bool                elm_object_tooltip_window_mode_set(Evas_Object *obj, Eina_Bool disable)
    Eina_Bool                elm_object_tooltip_window_mode_get(Evas_Object *obj)

    # Object - Translatable text (elm_general.h)
    void                     elm_object_domain_translatable_text_part_set(Evas_Object *obj, char *part, char *domain, char *text)
    void                     elm_object_domain_translatable_text_set(Evas_Object *obj, char *domain, char *text)
    void                     elm_object_translatable_text_set(Evas_Object *obj, char *text)
    char                    *elm_object_translatable_text_part_get(Evas_Object *obj, char *part)
    char                    *elm_object_translatable_text_get(Evas_Object *obj)

    # Object Item
    Evas_Object             *elm_object_item_widget_get(Elm_Object_Item *it)
    void                     elm_object_item_part_content_set(Elm_Object_Item *it, const_char_ptr part, Evas_Object* content)
    void                     elm_object_item_content_set(Elm_Object_Item *it, Evas_Object* content)
    Evas_Object             *elm_object_item_part_content_get(Elm_Object_Item *it, const_char_ptr part)
    Evas_Object             *elm_object_item_content_get(Elm_Object_Item *it)
    Evas_Object             *elm_object_item_part_content_unset(Elm_Object_Item *it, const_char_ptr part)
    Evas_Object             *elm_object_item_content_unset(Elm_Object_Item *it)
    void                     elm_object_item_part_text_set(Elm_Object_Item *item, const_char_ptr part, char *label)
    void                     elm_object_item_text_set(Elm_Object_Item *item, char *label)
    char                    *elm_object_item_part_text_get(Elm_Object_Item *item, const_char_ptr part)
    char                    *elm_object_item_text_get(Elm_Object_Item *item)
    void                     elm_object_item_access_info_set(Elm_Object_Item *it, const_char_ptr txt)
    void                    *elm_object_item_data_get(Elm_Object_Item *item)
    void                     elm_object_item_data_set(Elm_Object_Item *item, void *data)
    void                     elm_object_item_signal_emit(Elm_Object_Item *it, const_char_ptr emission, const_char_ptr source)
    void                     elm_object_item_disabled_set(Elm_Object_Item *it, Eina_Bool disabled)
    Eina_Bool                elm_object_item_disabled_get(Elm_Object_Item *it)
    void                     elm_object_item_del_cb_set(Elm_Object_Item *it, Evas_Smart_Cb del_cb)
    void                     elm_object_item_del(Elm_Object_Item *item)
    void                     elm_object_item_tooltip_text_set(Elm_Object_Item *it, const_char_ptr text)
    Eina_Bool                elm_object_item_tooltip_window_mode_set(Elm_Object_Item *it, Eina_Bool disable)
    Eina_Bool                elm_object_item_tooltip_window_mode_get(Elm_Object_Item *it)
    void                     elm_object_item_tooltip_content_cb_set(Elm_Object_Item *it, Elm_Tooltip_Item_Content_Cb func, void *data, Evas_Smart_Cb del_cb)
    void                     elm_object_item_tooltip_unset(Elm_Object_Item *it)
    void                     elm_object_item_tooltip_style_set(Elm_Object_Item *it, const_char_ptr style)
    const_char_ptr           elm_object_item_tooltip_style_get(Elm_Object_Item *it)
    void                     elm_object_item_cursor_set(Elm_Object_Item *it, const_char_ptr cursor)
    const_char_ptr           elm_object_item_cursor_get(Elm_Object_Item *it)
    void                     elm_object_item_cursor_unset(Elm_Object_Item *it)
    void                     elm_object_item_cursor_style_set(Elm_Object_Item *it, const_char_ptr style)
    const_char_ptr           elm_object_item_cursor_style_get(Elm_Object_Item *it)
    void                     elm_object_item_cursor_engine_only_set(Elm_Object_Item *it, Eina_Bool engine_only)
    Eina_Bool                elm_object_item_cursor_engine_only_get(Elm_Object_Item *it)

    # Widgets

    # Actionslider          (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_actionslider_add(Evas_Object *parent)
    char                    *elm_actionslider_selected_label_get(Evas_Object *obj)
    void                     elm_actionslider_indicator_pos_set(Evas_Object *obj, Elm_Actionslider_Pos pos)
    Elm_Actionslider_Pos     elm_actionslider_indicator_pos_get(Evas_Object *obj)
    void                     elm_actionslider_magnet_pos_set(Evas_Object *obj, Elm_Actionslider_Pos pos)
    Elm_Actionslider_Pos     elm_actionslider_magnet_pos_get(Evas_Object *obj)
    void                     elm_actionslider_enabled_pos_set(Evas_Object *obj, Elm_Actionslider_Pos pos)
    Elm_Actionslider_Pos     elm_actionslider_enabled_pos_get(Evas_Object *obj)

    # Background            (api:DONE  cb:N/A   test:DONE  doc:DONE)
    Evas_Object             *elm_bg_add(Evas_Object *parent)
    Eina_Bool                elm_bg_file_set(Evas_Object *obj, char *file, char *group)
    void                     elm_bg_file_get(Evas_Object *obj, char **file, char **group)
    void                     elm_bg_option_set(Evas_Object *obj, Elm_Bg_Option option)
    Elm_Bg_Option            elm_bg_option_get(Evas_Object *obj)
    void                     elm_bg_color_set(Evas_Object *obj, int r, int g, int b)
    void                     elm_bg_color_get(Evas_Object *obj, int *r, int *g, int *b)
    void                     elm_bg_load_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h)

    # Box                   (api:DONE  cb:N/A   test:DONE  doc:DONE)
    Evas_Object             *elm_box_add(Evas_Object *parent)
    void                     elm_box_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)
    Eina_Bool                elm_box_horizontal_get(Evas_Object *obj)
    void                     elm_box_homogeneous_set(Evas_Object *obj, Eina_Bool homogeneous)
    Eina_Bool                elm_box_homogeneous_get(Evas_Object *obj)
    void                     elm_box_pack_start(Evas_Object *obj, Evas_Object *subobj)
    void                     elm_box_pack_end(Evas_Object *obj, Evas_Object *subobj)
    void                     elm_box_pack_before(Evas_Object *obj, Evas_Object *subobj, Evas_Object *before)
    void                     elm_box_pack_after(Evas_Object *obj, Evas_Object *subobj, Evas_Object *after)
    void                     elm_box_clear(Evas_Object *obj)
    void                     elm_box_unpack(Evas_Object *obj, Evas_Object *subobj)
    void                     elm_box_unpack_all(Evas_Object *obj)
    void                     elm_box_recalculate(Evas_Object *obj)
    Eina_List               *elm_box_children_get(Evas_Object *obj)
    void                     elm_box_padding_set(Evas_Object *obj, Evas_Coord horizontal, Evas_Coord vertical)
    void                     elm_box_padding_get(Evas_Object *obj, Evas_Coord *horizontal, Evas_Coord *vertical)
    void                     elm_box_align_set(Evas_Object *obj, double horizontal, double vertical)
    void                     elm_box_align_get(Evas_Object *obj, double *horizontal, double *vertical)
    void                     elm_box_layout_set(Evas_Object *obj, Evas_Object_Box_Layout cb, const_void *data, Ecore_Cb free_data)
    Elm_Box_Transition      *elm_box_transition_new(double duration, Evas_Object_Box_Layout start_layout, void *start_layout_data, Ecore_Cb start_layout_free_data, Evas_Object_Box_Layout end_layout, void *end_layout_data, Ecore_Cb end_layout_free_data, Ecore_Cb transition_end_cb, void *transition_end_data)
    void                     elm_box_transition_free(void *data)

    # Bubble                (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_bubble_add(Evas_Object *parent)
    void                     elm_bubble_pos_set(Evas_Object *obj, Elm_Bubble_Pos pos)
    Elm_Bubble_Pos           elm_bubble_pos_get(Evas_Object *obj)

    # Button                (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_button_add(Evas_Object *parent)
    void                     elm_button_autorepeat_set(Evas_Object *obj, Eina_Bool on)
    Eina_Bool                elm_button_autorepeat_get(Evas_Object *obj)
    void                     elm_button_autorepeat_initial_timeout_set(Evas_Object *obj, double t)
    double                   elm_button_autorepeat_initial_timeout_get(Evas_Object *obj)
    void                     elm_button_autorepeat_gap_timeout_set(Evas_Object *obj, double t)
    double                   elm_button_autorepeat_gap_timeout_get(Evas_Object *obj)

    # Calendar              (api:TODO  cb:DONE  test:TODO  doc:DONE)
    Evas_Object             *elm_calendar_add(Evas_Object *parent)
    #const_char_ptr          *elm_calendar_weekdays_names_get(Evas_Object *obj)
    #void                     elm_calendar_weekdays_names_set(Evas_Object *obj, const_char_ptr weekdays[])
    void                     elm_calendar_min_max_year_set(Evas_Object *obj, int min, int max)
    void                     elm_calendar_min_max_year_get(Evas_Object *obj, int *min, int *max)
    void                     elm_calendar_select_mode_set(Evas_Object *obj, Elm_Calendar_Select_Mode mode)
    Elm_Calendar_Select_Mode elm_calendar_select_mode_get(Evas_Object *obj)
    #void                     elm_calendar_selected_time_set(Evas_Object *obj, struct tm *selected_time)
    #Eina_Bool                elm_calendar_selected_time_get(Evas_Object *obj, struct tm *selected_time)
    #void                     elm_calendar_format_function_set(Evas_Object *obj, Elm_Calendar_Format_Cb format_func)
    #Elm_Calendar_Mark       *elm_calendar_mark_add(Evas_Object *obj, const_char_ptr mark_type, struct tm *mark_time, Elm_Calendar_Mark_Repeat_Type repeat)
    #void                     elm_calendar_mark_del(Elm_Calendar_Mark *mark)
    void                     elm_calendar_marks_clear(Evas_Object *obj)
    const_Eina_List         *elm_calendar_marks_get(Evas_Object *obj)
    void                     elm_calendar_marks_draw(Evas_Object *obj)
    void                     elm_calendar_interval_set(Evas_Object *obj, double interval)
    double                   elm_calendar_interval_get(Evas_Object *obj)
    void                     elm_calendar_first_day_of_week_set(Evas_Object *obj, Elm_Calendar_Weekday day)
    Elm_Calendar_Weekday     elm_calendar_first_day_of_week_get(Evas_Object *obj)

    # Check                 (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_check_add(Evas_Object *parent)
    void                     elm_check_state_set(Evas_Object *obj, Eina_Bool state)
    Eina_Bool                elm_check_state_get(Evas_Object *obj)

    # Clock                 (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_clock_add(Evas_Object *parent)
    void                     elm_clock_time_set(Evas_Object *obj, int hrs, int min, int sec)
    void                     elm_clock_time_get(Evas_Object *obj, int *hrs, int *min, int *sec)
    void                     elm_clock_edit_set(Evas_Object *obj, Eina_Bool edit)
    Eina_Bool                elm_clock_edit_get(Evas_Object *obj)
    void                     elm_clock_edit_mode_set(Evas_Object *obj, Elm_Clock_Edit_Mode mode)
    Elm_Clock_Edit_Mode      elm_clock_edit_mode_get(Evas_Object *obj)
    void                     elm_clock_show_am_pm_set(Evas_Object *obj, Eina_Bool am_pm)
    Eina_Bool                elm_clock_show_am_pm_get(Evas_Object *obj)
    void                     elm_clock_show_seconds_set(Evas_Object *obj, Eina_Bool seconds)
    Eina_Bool                elm_clock_show_seconds_get(Evas_Object *obj)
    void                     elm_clock_first_interval_set(Evas_Object *obj, double interval)
    double                   elm_clock_first_interval_get(Evas_Object *obj)

    # Colorselector         (api:DONE  cb:DONE  test:TODO  doc:DONE)
    Evas_Object             *elm_colorselector_add(Evas_Object *parent)
    void                     elm_colorselector_color_set(Evas_Object *obj, int r, int g, int b, int a)
    void                     elm_colorselector_color_get(Evas_Object *obj, int *r, int *g, int *b, int *a)
    void                     elm_colorselector_mode_set(Evas_Object *obj, Elm_Colorselector_Mode mode)
    Elm_Colorselector_Mode   elm_colorselector_mode_get(Evas_Object *obj)
    void                     elm_colorselector_palette_item_color_get(Elm_Object_Item *it, int *r, int *g, int *b, int *a)
    void                     elm_colorselector_palette_item_color_set(Elm_Object_Item *it, int r, int g, int b, int a)
    Elm_Object_Item         *elm_colorselector_palette_color_add(Evas_Object *obj, int r, int g, int b, int a)
    void                     elm_colorselector_palette_clear(Evas_Object *obj)
    void                     elm_colorselector_palette_name_set(Evas_Object *obj, const_char_ptr palette_name)
    const_char_ptr           elm_colorselector_palette_name_get(Evas_Object *obj)

    # Conformant            (XXX)

    # Ctxpopup              (api:DONE  cb:DONE  test:TODO  doc:DONE)
    Evas_Object             *elm_ctxpopup_add(Evas_Object *parent)
    void                     elm_ctxpopup_hover_parent_set(Evas_Object *obj, Evas_Object *parent)
    Evas_Object             *elm_ctxpopup_hover_parent_get(Evas_Object *obj)
    void                     elm_ctxpopup_clear(Evas_Object *obj)
    void                     elm_ctxpopup_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)
    Eina_Bool                elm_ctxpopup_horizontal_get(Evas_Object *obj)
    Elm_Object_Item         *elm_ctxpopup_item_append(Evas_Object *obj, const_char_ptr label, Evas_Object *icon, Evas_Smart_Cb func, const_void *data)
    void                     elm_ctxpopup_direction_priority_set(Evas_Object *obj, Elm_Ctxpopup_Direction first, Elm_Ctxpopup_Direction second, Elm_Ctxpopup_Direction third, Elm_Ctxpopup_Direction fourth)
    void                     elm_ctxpopup_direction_priority_get(Evas_Object *obj, Elm_Ctxpopup_Direction *first, Elm_Ctxpopup_Direction *second, Elm_Ctxpopup_Direction *third, Elm_Ctxpopup_Direction *fourth)
    Elm_Ctxpopup_Direction   elm_ctxpopup_direction_get(Evas_Object *obj)
    void                     elm_ctxpopup_dismiss(Evas_Object *obj)

    # Datetime              (XXX)

    # Dayselector           (XXX)

    # Diskselector          (XXX)

    # Entry                 (api:TODO  cb:DONE  test:TODO  doc:TODO)
    Evas_Object             *elm_entry_add(Evas_Object *parent)
    void                     elm_entry_single_line_set(Evas_Object *obj, Eina_Bool single_line)
    Eina_Bool                elm_entry_single_line_get(Evas_Object *obj)
    void                     elm_entry_password_set(Evas_Object *obj, Eina_Bool password)
    Eina_Bool                elm_entry_password_get(Evas_Object *obj)
    void                     elm_entry_entry_set(Evas_Object *obj, char *entry)
    char                    *elm_entry_entry_get(Evas_Object *obj)
    void                     elm_entry_entry_append(Evas_Object *obj, char *text)
    Eina_Bool                elm_entry_is_empty(Evas_Object *obj)
    char                    *elm_entry_selection_get(Evas_Object *obj)
    Evas_Object             *elm_entry_textblock_get(Evas_Object *obj)
    void                     elm_entry_calc_force(Evas_Object *obj)
    void                     elm_entry_entry_insert(Evas_Object *obj, char *entry)
    void                     elm_entry_line_wrap_set(Evas_Object *obj, Elm_Wrap_Type wrap)
    Elm_Wrap_Type            elm_entry_line_wrap_get(Evas_Object *obj)
    void                     elm_entry_editable_set(Evas_Object *obj, Eina_Bool editable)
    Eina_Bool                elm_entry_editable_get(Evas_Object *obj)
    void                     elm_entry_select_none(Evas_Object *obj)
    void                     elm_entry_select_all(Evas_Object *obj)
    Eina_Bool                elm_entry_cursor_next(Evas_Object *obj)
    Eina_Bool                elm_entry_cursor_prev(Evas_Object *obj)
    Eina_Bool                elm_entry_cursor_up(Evas_Object *obj)
    Eina_Bool                elm_entry_cursor_down(Evas_Object *obj)
    void                     elm_entry_cursor_begin_set(Evas_Object *obj)
    void                     elm_entry_cursor_end_set(Evas_Object *obj)
    void                     elm_entry_cursor_line_begin_set(Evas_Object *obj)
    void                     elm_entry_cursor_line_end_set(Evas_Object *obj)
    void                     elm_entry_cursor_selection_begin(Evas_Object *obj)
    void                     elm_entry_cursor_selection_end(Evas_Object *obj)
    Eina_Bool                elm_entry_cursor_is_format_get(Evas_Object *obj)
    Eina_Bool                elm_entry_cursor_is_visible_format_get(Evas_Object *obj)
    char                    *elm_entry_cursor_content_get(Evas_Object *obj)
    Eina_Bool                elm_entry_cursor_geometry_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
    void                     elm_entry_cursor_pos_set(Evas_Object *obj, int pos)
    int                      elm_entry_cursor_pos_get(Evas_Object *obj)
    void                     elm_entry_selection_cut(Evas_Object *obj)
    void                     elm_entry_selection_copy(Evas_Object *obj)
    void                     elm_entry_selection_paste(Evas_Object *obj)
    char                    *elm_entry_markup_to_utf8(char *s)
    char                    *elm_entry_utf8_to_markup(char *s)
    Eina_Bool                elm_entry_file_set(Evas_Object *obj, char *file, Elm_Text_Format format)
    void                     elm_entry_file_get(Evas_Object *obj, const_char_ptr *file, Elm_Text_Format *format)
    void                     elm_entry_file_save(Evas_Object *obj)
    void                     elm_entry_autosave_set(Evas_Object *obj, Eina_Bool autosave)
    Eina_Bool                elm_entry_autosave_get(Evas_Object *obj)
    void                     elm_entry_scrollable_set(Evas_Object *obj, Eina_Bool scrollable)
    Eina_Bool                elm_entry_scrollable_get(Evas_Object *obj)
    void                     elm_entry_scrollbar_policy_set(Evas_Object *obj, Elm_Scroller_Policy h, Elm_Scroller_Policy v)
    void                     elm_entry_icon_visible_set(Evas_Object *obj, Eina_Bool setting)
    void                     elm_entry_context_menu_clear(Evas_Object *obj)
    void                     elm_entry_context_menu_disabled_set(Evas_Object *obj, Eina_Bool disabled)
    Eina_Bool                elm_entry_context_menu_disabled_get(Evas_Object *obj)
    void                     elm_entry_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce)
    void                     elm_entry_bounce_get(Evas_Object *obj, Eina_Bool *h_bounce, Eina_Bool *v_bounce)
    void                     elm_entry_input_panel_enabled_set(Evas_Object *obj, Eina_Bool enabled)
    Eina_Bool                elm_entry_input_panel_enabled_get(Evas_Object *obj)
    void                     elm_entry_input_panel_layout_set(Evas_Object *obj, Elm_Input_Panel_Layout layout)
    Elm_Input_Panel_Layout   elm_entry_input_panel_layout_get(Evas_Object *obj)
    void                     elm_entry_input_panel_show(Evas_Object *obj)
    void                     elm_entry_input_panel_hide(Evas_Object *obj)
    void                     elm_entry_input_panel_language_set(Evas_Object *obj, Elm_Input_Panel_Lang lang)
    Elm_Input_Panel_Lang     elm_entry_input_panel_language_get(Evas_Object *obj)
    void                     elm_entry_input_panel_return_key_type_set(Evas_Object *obj, Elm_Input_Panel_Return_Key_Type return_key_type)
    Elm_Input_Panel_Return_Key_Type elm_entry_input_panel_return_key_type_get(Evas_Object *obj)
    void                     elm_entry_input_panel_return_key_disabled_set(Evas_Object *obj, Eina_Bool disabled)
    Eina_Bool                elm_entry_input_panel_return_key_disabled_get(Evas_Object *obj)
    void                     elm_entry_input_panel_return_key_autoenabled_set(Evas_Object *obj, Eina_Bool disabled)
    void                     elm_entry_imf_context_reset(Evas_Object *obj)
    void                     elm_entry_prediction_allow_set(Evas_Object *obj, Eina_Bool allow)
    Eina_Bool                elm_entry_prediction_allow_get(Evas_Object *obj)
    void                     elm_entry_cnp_mode_set(Evas_Object *obj, Elm_Cnp_Mode cnp_mode)
    Elm_Cnp_Mode             elm_entry_cnp_mode_get(Evas_Object *obj)
    void                     elm_entry_anchor_hover_parent_set(Evas_Object *obj, Evas_Object *anchor_hover_parent)
    Evas_Object             *elm_entry_anchor_hover_parent_get(Evas_Object *obj)
    void                     elm_entry_anchor_hover_style_set(Evas_Object *obj, char *anchor_hover_style)
    char                    *elm_entry_anchor_hover_style_get(Evas_Object *obj)
    void                     elm_entry_anchor_hover_end(Evas_Object *obj)

    # Fileselector          (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_fileselector_add(Evas_Object *parent)
    void                     elm_fileselector_is_save_set(Evas_Object *obj, Eina_Bool is_save)
    Eina_Bool                elm_fileselector_is_save_get(Evas_Object *obj)
    void                     elm_fileselector_folder_only_set(Evas_Object *obj, Eina_Bool value)
    Eina_Bool                elm_fileselector_folder_only_get(Evas_Object *obj)
    void                     elm_fileselector_buttons_ok_cancel_set(Evas_Object *obj, Eina_Bool value)
    Eina_Bool                elm_fileselector_buttons_ok_cancel_get(Evas_Object *obj)
    void                     elm_fileselector_expandable_set(Evas_Object *obj, Eina_Bool value)
    Eina_Bool                elm_fileselector_expandable_get(Evas_Object *obj)
    void                     elm_fileselector_path_set(Evas_Object *obj, char *path)
    char                    *elm_fileselector_path_get(Evas_Object *obj)
    Eina_Bool                elm_fileselector_selected_set(Evas_Object *obj, char *path)
    char                    *elm_fileselector_selected_get(Evas_Object *obj)
    void                     elm_fileselector_mode_set(Evas_Object *obj, Elm_Fileselector_Mode mode)
    Elm_Fileselector_Mode    elm_fileselector_mode_get(Evas_Object *obj)

    # Fileselector Button   (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_fileselector_button_add(Evas_Object *parent)
    void                     elm_fileselector_button_window_title_set(Evas_Object *obj, char *title)
    char                    *elm_fileselector_button_window_title_get(Evas_Object *obj)
    void                     elm_fileselector_button_window_size_set(Evas_Object *obj, Evas_Coord width, Evas_Coord height)
    void                     elm_fileselector_button_window_size_get(Evas_Object *obj, Evas_Coord *width, Evas_Coord *height)
    void                     elm_fileselector_button_folder_only_set(Evas_Object *obj, Eina_Bool value)
    Eina_Bool                elm_fileselector_button_folder_only_get(Evas_Object *obj)
    void                     elm_fileselector_button_inwin_mode_set(Evas_Object *obj, Eina_Bool value)
    Eina_Bool                elm_fileselector_button_inwin_mode_get(Evas_Object *obj)
    void                     elm_fileselector_button_is_save_set(Evas_Object *obj, Eina_Bool value)
    Eina_Bool                elm_fileselector_button_is_save_get(Evas_Object *obj)
    void                     elm_fileselector_button_path_set(Evas_Object *obj, char *path)
    char                    *elm_fileselector_button_path_get(Evas_Object *obj)
    void                     elm_fileselector_button_expandable_set(Evas_Object *obj, Eina_Bool value)
    Eina_Bool                elm_fileselector_button_expandable_get(Evas_Object *obj)

    # Fileselector Entry    (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_fileselector_entry_add(Evas_Object *parent)
    void                     elm_fileselector_entry_window_title_set(Evas_Object *obj, char *title)
    char                    *elm_fileselector_entry_window_title_get(Evas_Object *obj)
    void                     elm_fileselector_entry_window_size_set(Evas_Object *obj, Evas_Coord width, Evas_Coord height)
    void                     elm_fileselector_entry_window_size_get(Evas_Object *obj, Evas_Coord *width, Evas_Coord *height)
    void                     elm_fileselector_entry_path_set(Evas_Object *obj, char *path)
    char                    *elm_fileselector_entry_path_get(Evas_Object *obj)
    void                     elm_fileselector_entry_expandable_set(Evas_Object *obj, Eina_Bool value)
    Eina_Bool                elm_fileselector_entry_expandable_get(Evas_Object *obj)
    void                     elm_fileselector_entry_folder_only_set(Evas_Object *obj, Eina_Bool value)
    Eina_Bool                elm_fileselector_entry_folder_only_get(Evas_Object *obj)
    void                     elm_fileselector_entry_is_save_set(Evas_Object *obj, Eina_Bool value)
    Eina_Bool                elm_fileselector_entry_is_save_get(Evas_Object *obj)
    void                     elm_fileselector_entry_inwin_mode_set(Evas_Object *obj, Eina_Bool value)
    Eina_Bool                elm_fileselector_entry_inwin_mode_get(Evas_Object *obj)
    void                     elm_fileselector_entry_selected_set(Evas_Object *obj, char *path)
    char                    *elm_fileselector_entry_selected_get(Evas_Object *obj)

    # Flip                  (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_flip_add(Evas_Object *parent)
    Eina_Bool                elm_flip_front_visible_get(Evas_Object *obj)
    void                     elm_flip_perspective_set(Evas_Object *obj, Evas_Coord foc, Evas_Coord x, Evas_Coord y)
    void                     elm_flip_go(Evas_Object *obj, Elm_Flip_Mode mode)
    void                     elm_flip_interaction_set(Evas_Object *obj, Elm_Flip_Interaction mode)
    Elm_Flip_Interaction     elm_flip_interaction_get(Evas_Object *obj)
    void                     elm_flip_interaction_direction_enabled_set(Evas_Object *obj, Elm_Flip_Direction dir, Eina_Bool enabled)
    Eina_Bool                elm_flip_interaction_direction_enabled_get(Evas_Object *obj, Elm_Flip_Direction dir)
    void                     elm_flip_interaction_direction_hitsize_set(Evas_Object *obj, Elm_Flip_Direction dir, double hitsize)
    double                   elm_flip_interaction_direction_hitsize_get(Evas_Object *obj, Elm_Flip_Direction dir)

    # Flipselector          (XXX)

    # Frame                 (api:DONE  cb:DONE  test:TODO  doc:DONE)
    Evas_Object             *elm_frame_add(Evas_Object *parent)
    void                     elm_frame_autocollapse_set(Evas_Object *obj, Eina_Bool autocollapse)
    Eina_Bool                elm_frame_autocollapse_get(Evas_Object *obj)
    void                     elm_frame_collapse_set(Evas_Object *obj, Eina_Bool collapse)
    Eina_Bool                elm_frame_collapse_get(Evas_Object *obj)
    void                     elm_frame_collapse_go(Evas_Object *obj, Eina_Bool collapse)

    # Generic Grid          (api:TODO  cb:TODO  test:DONE  doc:TODO)
    Evas_Object             *elm_gengrid_add(Evas_Object *parent)
    void                     elm_gengrid_clear(Evas_Object *obj)
    void                     elm_gengrid_multi_select_set(Evas_Object *obj, Eina_Bool multi)
    Eina_Bool                elm_gengrid_multi_select_get(Evas_Object *obj)
    void                     elm_gengrid_horizontal_set(Evas_Object *obj, Eina_Bool setting)
    Eina_Bool                elm_gengrid_horizontal_get(Evas_Object *obj)
    void                     elm_gengrid_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce)
    void                     elm_gengrid_bounce_get(Evas_Object *obj, Eina_Bool *h_bounce, Eina_Bool *v_bounce)
    Elm_Object_Item         *elm_gengrid_item_append(Evas_Object *obj, Elm_Gengrid_Item_Class *itc, void *data, Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_gengrid_item_prepend(Evas_Object *obj, Elm_Gengrid_Item_Class *itc, void *data, Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_gengrid_item_insert_before(Evas_Object *obj, Elm_Gengrid_Item_Class *itc, void *data, Elm_Object_Item *before, Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_gengrid_item_insert_after(Evas_Object *obj, Elm_Gengrid_Item_Class *itc, void *data, Elm_Object_Item *after, Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_gengrid_selected_item_get(Evas_Object *obj)
    Eina_List               *elm_gengrid_selected_items_get(Evas_Object *obj)
    Eina_List               *elm_gengrid_realized_items_get(Evas_Object *obj)
    void                     elm_gengrid_realized_items_update(Evas_Object *obj)
    Elm_Object_Item         *elm_gengrid_first_item_get(Evas_Object *obj)
    Elm_Object_Item         *elm_gengrid_last_item_get(Evas_Object *obj)
    void                     elm_gengrid_scroller_policy_set(Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v)
    void                     elm_gengrid_scroller_policy_get(Evas_Object *obj, Elm_Scroller_Policy *policy_h, Elm_Scroller_Policy *policy_v)
    unsigned int             elm_gengrid_items_count(Evas_Object *obj)
    void                     elm_gengrid_item_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
    void                     elm_gengrid_item_size_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)
    void                     elm_gengrid_group_item_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
    void                     elm_gengrid_group_item_size_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)
    void                     elm_gengrid_align_set(Evas_Object *obj,  double align_x, double align_y)
    void                     elm_gengrid_align_get(Evas_Object *obj,  double *align_x, double *align_y)
    void                     elm_gengrid_reorder_mode_set(Evas_Object *obj, Eina_Bool reorder_mode)
    Eina_Bool                elm_gengrid_reorder_mode_get(Evas_Object *obj)
    void                     elm_gengrid_page_relative_set(Evas_Object *obj, double h_pagerel, double v_pagerel)
    void                     elm_gengrid_page_relative_get(Evas_Object *obj, double *h_pagerel, double *v_pagerel)
    void                     elm_gengrid_page_size_set(Evas_Object *obj, Evas_Coord h_pagesize, Evas_Coord v_pagesize)
    void                     elm_gengrid_current_page_get(Evas_Object *obj, int *h_pagenum, int *v_pagenum)
    void                     elm_gengrid_last_page_get(Evas_Object *obj, int *h_pagenum, int *v_pagenum)
    void                     elm_gengrid_page_show(Evas_Object *obj, int h_pagenum, int v_pagenum)
    void                     elm_gengrid_page_bring_in(Evas_Object *obj, int h_pagenum, int v_pagenum)
    void                     elm_gengrid_filled_set(Evas_Object *obj, Eina_Bool fill)
    Eina_Bool                elm_gengrid_filled_get(Evas_Object *obj)
    void                     elm_gengrid_select_mode_set(Evas_Object *obj, Elm_Object_Select_Mode mode)
    Elm_Object_Select_Mode   elm_gengrid_select_mode_get(Evas_Object *obj)
    void                     elm_gengrid_highlight_mode_set(Evas_Object *obj, Eina_Bool highlight)
    Eina_Bool                elm_gengrid_highlight_mode_get(Evas_Object *obj)

    Elm_Object_Item         *elm_gengrid_first_item_get(Evas_Object *obj)
    Elm_Object_Item         *elm_gengrid_last_item_get(Evas_Object *obj)
    int                      elm_gengrid_item_index_get(Elm_Object_Item *it)
    void                     elm_gengrid_item_select_mode_set(Elm_Object_Item *it, Elm_Object_Select_Mode mode)
    Elm_Object_Select_Mode   elm_gengrid_item_select_mode_get(Elm_Object_Item *it)
    Elm_Object_Item         *elm_gengrid_item_next_get(Elm_Object_Item *item)
    Elm_Object_Item         *elm_gengrid_item_prev_get(Elm_Object_Item *item)
    void                     elm_gengrid_item_selected_set(Elm_Object_Item *item, Eina_Bool selected)
    Eina_Bool                elm_gengrid_item_selected_get(Elm_Object_Item *item)
    void                     elm_gengrid_item_show(Elm_Object_Item *item, Elm_Genlist_Item_Scrollto_Type scrollto_type)
    void                     elm_gengrid_item_bring_in(Elm_Object_Item *item, Elm_Genlist_Item_Scrollto_Type scrollto_type)
    Evas_Object             *elm_gengrid_item_object_get(Elm_Object_Item *it)
    void                     elm_gengrid_item_update(Elm_Object_Item *item)
    void                     elm_gengrid_item_pos_get(Elm_Object_Item *item, unsigned int *x, unsigned int *y)
    void                     elm_gengrid_item_tooltip_text_set(Elm_Object_Item *item, char *text)
    void                     elm_gengrid_item_tooltip_content_cb_set(Elm_Object_Item *item, Elm_Tooltip_Item_Content_Cb func, void *data, Evas_Smart_Cb del_cb)
    void                     elm_gengrid_item_tooltip_unset(Elm_Object_Item *item)
    void                     elm_gengrid_item_tooltip_style_set(Elm_Object_Item *item, char *style)
    char                    *elm_gengrid_item_tooltip_style_get(Elm_Object_Item *item)
    Eina_Bool                elm_gengrid_item_tooltip_window_mode_set(Elm_Object_Item *it, Eina_Bool disable)
    Eina_Bool                elm_gengrid_item_tooltip_window_mode_get(Elm_Object_Item *it)
    void                     elm_gengrid_item_cursor_set(Elm_Object_Item *item, char *cursor)
    char                    *elm_gengrid_item_cursor_get(Elm_Object_Item *item)
    void                     elm_gengrid_item_cursor_unset(Elm_Object_Item *item)
    void                     elm_gengrid_item_cursor_style_set(Elm_Object_Item *item, char *style)
    char                    *elm_gengrid_item_cursor_style_get(Elm_Object_Item *item)
    void                     elm_gengrid_item_cursor_engine_only_set(Elm_Object_Item *item, Eina_Bool engine_only)
    Eina_Bool                elm_gengrid_item_cursor_engine_only_get(Elm_Object_Item *item)

    # Generic List          (api:TODO  cb:DONE  test:DONE  doc:TODO)
    Evas_Object             *elm_genlist_add(Evas_Object *parent)
    void                     elm_genlist_clear(Evas_Object *obj)
    void                     elm_genlist_multi_select_set(Evas_Object *obj, Eina_Bool multi)
    Eina_Bool                elm_genlist_multi_select_get(Evas_Object *obj)
    void                     elm_genlist_mode_set(Evas_Object *obj, Elm_List_Mode mode)
    Elm_List_Mode            elm_genlist_mode_get(Evas_Object *obj)
    void                     elm_genlist_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce)
    void                     elm_genlist_bounce_get(Evas_Object *obj, Eina_Bool *h_bounce, Eina_Bool *v_bounce)
    Elm_Object_Item         *elm_genlist_item_append(Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Object_Item *parent, Elm_Genlist_Item_Type flags, Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_genlist_item_prepend(Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Object_Item *parent, Elm_Genlist_Item_Type flags, Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_genlist_item_insert_before(Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Object_Item *parent, Elm_Object_Item *before, Elm_Genlist_Item_Type flags, Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_genlist_item_insert_after(Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Object_Item *parent, Elm_Object_Item *after, Elm_Genlist_Item_Type flags, Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_genlist_item_sorted_insert(Evas_Object *obj, Elm_Genlist_Item_Class *itc, void *data, Elm_Object_Item *parent, Elm_Genlist_Item_Type flags, Eina_Compare_Cb comp, Evas_Smart_Cb func, void *func_data)
    Elm_Object_Item         *elm_genlist_selected_item_get(Evas_Object *obj)
    Eina_List               *elm_genlist_selected_items_get(Evas_Object *obj)
    Eina_List               *elm_genlist_realized_items_get(Evas_Object *obj)
    Elm_Object_Item         *elm_genlist_first_item_get(Evas_Object *obj)
    Elm_Object_Item         *elm_genlist_last_item_get(Evas_Object *obj)
    void                     elm_genlist_scroller_policy_set(Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v)
    void                     elm_genlist_scroller_policy_get(Evas_Object *obj, Elm_Scroller_Policy *policy_h, Elm_Scroller_Policy *policy_v)

    Elm_Object_Item         *elm_genlist_item_next_get(Elm_Object_Item *item)
    Elm_Object_Item         *elm_genlist_item_prev_get(Elm_Object_Item *item)
    void                     elm_genlist_item_selected_set(Elm_Object_Item *item, Eina_Bool selected)
    Eina_Bool                elm_genlist_item_selected_get(Elm_Object_Item *item)
    void                     elm_genlist_item_show(Elm_Object_Item *item, Elm_Genlist_Item_Scrollto_Type scrollto_type)
    void                     elm_genlist_item_bring_in(Elm_Object_Item *item, Elm_Genlist_Item_Scrollto_Type scrollto_type)
    void                     elm_genlist_item_update(Elm_Object_Item *item)
    void                     elm_genlist_item_item_class_update(Elm_Object_Item *it, Elm_Genlist_Item_Class *itc)
    Elm_Genlist_Item_Class  *elm_genlist_item_item_class_get(Elm_Object_Item *it)
    int                      elm_genlist_item_index_get(Elm_Object_Item *it)
    void                     elm_genlist_realized_items_update(Evas_Object *obj)
    unsigned int             elm_genlist_items_count(Evas_Object *obj)
    void                     elm_genlist_item_tooltip_text_set(Elm_Object_Item *item, char *text)
    void                     elm_genlist_item_tooltip_content_cb_set(Elm_Object_Item *item, Elm_Tooltip_Item_Content_Cb func, void *data, Evas_Smart_Cb del_cb)
    void                     elm_genlist_item_tooltip_unset(Elm_Object_Item *item)
    void                     elm_genlist_item_tooltip_style_set(Elm_Object_Item *item, char *style)
    char                    *elm_genlist_item_tooltip_style_get(Elm_Object_Item *item)
    Eina_Bool                elm_genlist_item_tooltip_window_mode_set(Elm_Object_Item *it, Eina_Bool disable)
    Eina_Bool                elm_genlist_item_tooltip_window_mode_get(Elm_Object_Item *it)
    void                     elm_genlist_item_cursor_set(Elm_Object_Item *item, char *cursor)
    char                    *elm_genlist_item_cursor_get(Elm_Object_Item *it)
    void                     elm_genlist_item_cursor_unset(Elm_Object_Item *item)
    void                     elm_genlist_item_cursor_style_set(Elm_Object_Item *item, char *style)
    char*                    elm_genlist_item_cursor_style_get(Elm_Object_Item *item)
    void                     elm_genlist_item_cursor_engine_only_set(Elm_Object_Item *item, Eina_Bool engine_only)
    Eina_Bool                elm_genlist_item_cursor_engine_only_get(Elm_Object_Item *item)
    void                     elm_genlist_homogeneous_set(Evas_Object *obj, Eina_Bool homogeneous)
    Eina_Bool                elm_genlist_homogeneous_get(Evas_Object *obj)
    void                     elm_genlist_block_count_set(Evas_Object *obj, int n)
    int                      elm_genlist_block_count_get(Evas_Object *obj)
    void                     elm_genlist_longpress_timeout_set(Evas_Object *obj, double timeout)
    double                   elm_genlist_longpress_timeout_get(Evas_Object *obj)
    Elm_Object_Item         *elm_genlist_at_xy_item_get(Evas_Object *obj, Evas_Coord x, Evas_Coord y, int *posret)
    Elm_Object_Item         *elm_genlist_item_parent_get(Elm_Object_Item *it)
    void                     elm_genlist_item_subitems_clear(Elm_Object_Item *item)
    void                     elm_genlist_item_expanded_set(Elm_Object_Item *item, Eina_Bool expanded)
    Eina_Bool                elm_genlist_item_expanded_get(Elm_Object_Item *item)
    int                      elm_genlist_item_expanded_depth_get(Elm_Object_Item *it)
    void                     elm_genlist_item_all_contents_unset(Elm_Object_Item *it, Eina_List **l)
    void                     elm_genlist_item_promote(Elm_Object_Item *it)
    void                     elm_genlist_item_demote(Elm_Object_Item *it)
    void                     elm_genlist_item_fields_update(Elm_Object_Item *item, char *parts, Elm_Genlist_Item_Field_Type itf)
    void                     elm_genlist_item_decorate_mode_set(Elm_Object_Item *it, char *decorate_it_type, Eina_Bool decorate_it_set)
    char                    *elm_genlist_item_decorate_mode_get(Elm_Object_Item *it)
    Elm_Object_Item         *elm_genlist_decorated_item_get(Evas_Object *obj)
    void                     elm_genlist_reorder_mode_set(Evas_Object *obj, Eina_Bool reorder_mode)
    Eina_Bool                elm_genlist_reorder_mode_get(Evas_Object *obj)
    Elm_Genlist_Item_Type    elm_genlist_item_type_get(Elm_Object_Item *it)
    void                     elm_genlist_decorate_mode_set(Evas_Object *obj, Eina_Bool decorated)
    Eina_Bool                elm_genlist_decorate_mode_get(Evas_Object *obj)
    void                     elm_genlist_item_flip_set(Elm_Object_Item *it, Eina_Bool flip)
    Eina_Bool                elm_genlist_item_flip_get(Elm_Object_Item *it)
    void                     elm_genlist_tree_effect_enabled_set(Evas_Object *obj, Eina_Bool enabled)
    Eina_Bool                elm_genlist_tree_effect_enabled_get(Evas_Object *obj)
    void                     elm_genlist_item_select_mode_set(Elm_Object_Item *it, Elm_Object_Select_Mode mode)
    Elm_Object_Select_Mode   elm_genlist_item_select_mode_get(Elm_Object_Item *it)
    void                     elm_genlist_highlight_mode_set(Evas_Object *obj, Eina_Bool highlight)
    Eina_Bool                elm_genlist_highlight_mode_get(Evas_Object *obj)
    void                     elm_genlist_select_mode_set(Evas_Object *obj, Elm_Object_Select_Mode mode)
    Elm_Object_Select_Mode   elm_genlist_select_mode_get(Evas_Object *obj)

    # GLView                (XXX)

    # Grid                  (api:DONE  cb:N/A   test:TODO  doc:DONE)
    Evas_Object             *elm_grid_add(Evas_Object *parent)
    void                     elm_grid_size_set(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
    void                     elm_grid_size_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)
    void                     elm_grid_pack(Evas_Object *obj, Evas_Object *subobj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
    void                     elm_grid_unpack(Evas_Object *obj, Evas_Object *subobj)
    void                     elm_grid_clear(Evas_Object *obj, Eina_Bool clear)
    void                     elm_grid_pack_set(Evas_Object *subobj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
    void                     elm_grid_pack_get(Evas_Object *subobj, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
    Eina_List               *elm_grid_children_get(Evas_Object *obj)

    # Hover                 (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_hover_add(Evas_Object *parent)
    void                     elm_hover_target_set(Evas_Object *obj, Evas_Object *target)
    Evas_Object             *elm_hover_target_get(Evas_Object *obj)
    void                     elm_hover_parent_set(Evas_Object *obj, Evas_Object *parent)
    Evas_Object             *elm_hover_parent_get(Evas_Object *obj)
    char                    *elm_hover_best_content_location_get(Evas_Object *obj, Elm_Hover_Axis pref_axis)
    void                     elm_hover_dismiss(Evas_Object *obj)

    # Hoversel              (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_hoversel_add(Evas_Object *parent)
    void                     elm_hoversel_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)
    Eina_Bool                elm_hoversel_horizontal_get(Evas_Object *obj)
    void                     elm_hoversel_hover_parent_set(Evas_Object *obj, Evas_Object *parent)
    Evas_Object             *elm_hoversel_hover_parent_get(Evas_Object *obj)
    void                     elm_hoversel_hover_begin(Evas_Object *obj)
    void                     elm_hoversel_hover_end(Evas_Object *obj)
    Eina_Bool                elm_hoversel_expanded_get(Evas_Object *obj)
    void                     elm_hoversel_clear(Evas_Object *obj)
    Eina_List               *elm_hoversel_items_get(Evas_Object *obj)
    Elm_Object_Item         *elm_hoversel_item_add(Evas_Object *obj, char *label, char *icon_file, Elm_Icon_Type icon_type, Evas_Smart_Cb func, void *data)
    void                     elm_hoversel_item_icon_set(Elm_Object_Item *it, char *icon_file, char *icon_group, Elm_Icon_Type icon_type)
    void                     elm_hoversel_item_icon_get(Elm_Object_Item *it, char **icon_file, char **icon_group, Elm_Icon_Type *icon_type)

    # Icon                  (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_icon_add(Evas_Object *parent)
    void                     elm_icon_thumb_set(Evas_Object *obj, char *file, char *group)
    Eina_Bool                elm_icon_standard_set(Evas_Object *obj, char* name)
    char                    *elm_icon_standard_get(Evas_Object *obj)
    void                     elm_icon_order_lookup_set(Evas_Object *obj, Elm_Icon_Lookup_Order order)
    Elm_Icon_Lookup_Order    elm_icon_order_lookup_get(Evas_Object *obj)

    # Image                 (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_image_add(Evas_Object *parent)
    Eina_Bool                elm_image_memfile_set(Evas_Object *obj, const_void *img, size_t size, const_char_ptr format, const_char_ptr key)
    Eina_Bool                elm_image_file_set(Evas_Object *obj, char *file, char *group)
    void                     elm_image_file_get(Evas_Object *obj, char **file, char **group)
    void                     elm_image_smooth_set(Evas_Object *obj, Eina_Bool smooth)
    Eina_Bool                elm_image_smooth_get(Evas_Object *obj)
    void                     elm_image_object_size_get(Evas_Object *obj, int *w, int *h)
    void                     elm_image_no_scale_set(Evas_Object *obj, Eina_Bool no_scale)
    Eina_Bool                elm_image_no_scale_get(Evas_Object *obj)
    void                     elm_image_resizable_set(Evas_Object *obj, Eina_Bool scale_up,Eina_Bool scale_down)
    void                     elm_image_resizable_get(Evas_Object *obj, Eina_Bool *scale_up,Eina_Bool *scale_down)
    void                     elm_image_fill_outside_set(Evas_Object *obj, Eina_Bool fill_outside)
    Eina_Bool                elm_image_fill_outside_get(Evas_Object *obj)
    void                     elm_image_preload_disabled_set(Evas_Object *obj, Eina_Bool disabled)
    void                     elm_image_prescale_set(Evas_Object *obj, int size)
    int                      elm_image_prescale_get(Evas_Object *obj)
    void                     elm_image_orient_set(Evas_Object *obj, Elm_Image_Orient orient)
    Elm_Image_Orient         elm_image_orient_get(Evas_Object *obj)
    void                     elm_image_editable_set(Evas_Object *obj, Eina_Bool editable)
    Eina_Bool                elm_image_editable_get(Evas_Object *obj)
    Evas_Object             *elm_image_object_get(Evas_Object *obj)
    void                     elm_image_aspect_fixed_set(Evas_Object *obj, Eina_Bool fixed)
    Eina_Bool                elm_image_aspect_fixed_get(Evas_Object *obj)
    Eina_Bool                elm_image_animated_available_get(Evas_Object *obj)
    void                     elm_image_animated_set(Evas_Object *obj, Eina_Bool animated)
    Eina_Bool                elm_image_animated_get(Evas_Object *obj)
    void                     elm_image_animated_play_set(Evas_Object *obj, Eina_Bool play)
    Eina_Bool                elm_image_animated_play_get(Evas_Object *obj)

    # Index                 (api:TODO  cb:DONE  test:TODO  doc:DONE)
    Evas_Object             *elm_index_add(Evas_Object *parent)
    void                     elm_index_autohide_disabled_set(Evas_Object *obj, Eina_Bool disabled)
    Eina_Bool                elm_index_autohide_disabled_get(Evas_Object *obj)
    void                     elm_index_item_level_set(Evas_Object *obj, int level)
    int                      elm_index_item_level_get(Evas_Object *obj)
    void                     elm_index_item_selected_set(Elm_Object_Item *it, Eina_Bool selected)
    Elm_Object_Item         *elm_index_selected_item_get(Evas_Object *obj, int level)
    Elm_Object_Item         *elm_index_item_append(Evas_Object *obj, const_char_ptr letter, Evas_Smart_Cb func, const_void *data)
    Elm_Object_Item         *elm_index_item_prepend(Evas_Object *obj, const_char_ptr letter, Evas_Smart_Cb func, const_void *data)
    Elm_Object_Item         *elm_index_item_insert_after(Evas_Object *obj, Elm_Object_Item *after, const_char_ptr letter, Evas_Smart_Cb func, const_void *data)
    Elm_Object_Item         *elm_index_item_insert_before(Evas_Object *obj, Elm_Object_Item *before, const_char_ptr letter, Evas_Smart_Cb func, const_void *data)
    Elm_Object_Item         *elm_index_item_sorted_insert(Evas_Object *obj, const_char_ptr letter, Evas_Smart_Cb func, const_void *data, Eina_Compare_Cb cmp_func, Eina_Compare_Cb cmp_data_func)
    Elm_Object_Item         *elm_index_item_find(Evas_Object *obj, void *data)
    void                     elm_index_item_clear(Evas_Object *obj)
    void                     elm_index_level_go(Evas_Object *obj, int level)
    char                    *elm_index_item_letter_get(Elm_Object_Item *item)
    void                     elm_index_indicator_disabled_set(Evas_Object *obj, Eina_Bool disabled)
    Eina_Bool                elm_index_indicator_disabled_get(Evas_Object *obj)
    void                     elm_index_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)
    Eina_Bool                elm_index_horizontal_get(Evas_Object *obj)

    # Inwin                 (api:DONE  cb:N/A   test:DONE  doc:DONE)
    Evas_Object             *elm_win_inwin_add(Evas_Object *obj)
    void                     elm_win_inwin_activate(Evas_Object *obj)
    void                     elm_win_inwin_content_set(Evas_Object *obj, Evas_Object *content)
    Evas_Object             *elm_win_inwin_content_get(Evas_Object *obj)
    Evas_Object             *elm_win_inwin_content_unset(Evas_Object *obj)

    # Label                 (api:DONE  cb:DONE  test:TODO  doc:DONE)
    Evas_Object             *elm_label_add(Evas_Object *parent)
    void                     elm_label_line_wrap_set(Evas_Object *obj, Elm_Wrap_Type wrap)
    Elm_Wrap_Type            elm_label_line_wrap_get(Evas_Object *obj)
    void                     elm_label_wrap_width_set(Evas_Object *obj, Evas_Coord w)
    Evas_Coord               elm_label_wrap_width_get(Evas_Object *obj)
    void                     elm_label_ellipsis_set(Evas_Object *obj, Eina_Bool ellipsis)
    Eina_Bool                elm_label_ellipsis_get(Evas_Object *obj)
    void                     elm_label_slide_set(Evas_Object *obj, Eina_Bool slide)
    Eina_Bool                elm_label_slide_get(Evas_Object *obj)
    void                     elm_label_slide_duration_set(Evas_Object *obj, double duration)
    double                   elm_label_slide_duration_get(Evas_Object *obj)

    # Layout                (api:TODO  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_layout_add(Evas_Object *parent)
    Eina_Bool                elm_layout_file_set(Evas_Object *obj, char *file, char *group)
    Eina_Bool                elm_layout_theme_set(Evas_Object *obj, char *clas, char *group, char *style)
    void                     elm_layout_signal_emit(Evas_Object *obj, char *emission, char *source)
    void                     elm_layout_signal_callback_add(Evas_Object *obj, char *emission, char *source, Edje_Signal_Cb func, void *data)
    void                    *elm_layout_signal_callback_del(Evas_Object *obj, char *emission, char *source, Edje_Signal_Cb func)
    Eina_Bool                elm_layout_box_append(Evas_Object *obj, char *part, Evas_Object *child)
    Eina_Bool                elm_layout_box_prepend(Evas_Object *obj, char *part, Evas_Object *child)
    Eina_Bool                elm_layout_box_insert_before(Evas_Object *obj, char *part, Evas_Object *child, Evas_Object *reference)
    Eina_Bool                elm_layout_box_insert_at(Evas_Object *obj, char *part, Evas_Object *child, unsigned int pos)
    Evas_Object             *elm_layout_box_remove(Evas_Object *obj, char *part, Evas_Object *child)
    Eina_Bool                elm_layout_box_remove_all(Evas_Object *obj, char *part, Eina_Bool clear)
    Eina_Bool                elm_layout_table_pack(Evas_Object *obj, char *part, Evas_Object *child_obj, unsigned short col, unsigned short row, unsigned short colspan, unsigned short rowspan)
    Evas_Object             *elm_layout_table_unpack(Evas_Object *obj, char *part, Evas_Object *child_obj)
    Eina_Bool                elm_layout_table_clear(Evas_Object *obj, char *part, Eina_Bool clear)
    Evas_Object             *elm_layout_edje_get(Evas_Object *obj)
    char                    *elm_layout_data_get(Evas_Object *obj, char *key)
    void                     elm_layout_sizing_eval(Evas_Object *obj)
    Eina_Bool                elm_layout_part_cursor_set(Evas_Object *obj, char *part_name, char *cursor)
    char                    *elm_layout_part_cursor_get(Evas_Object *obj, char *part_name)
    Eina_Bool                elm_layout_part_cursor_unset(Evas_Object *obj, char *part_name)
    Eina_Bool                elm_layout_part_cursor_style_set(Evas_Object *obj, char *part_name, char *style)
    char                    *elm_layout_part_cursor_style_get(Evas_Object *obj, char *part_name)
    Eina_Bool                elm_layout_part_cursor_engine_only_set(Evas_Object *obj, char *part_name, Eina_Bool engine_only)
    Eina_Bool                elm_layout_part_cursor_engine_only_get(Evas_Object *obj, char *part_name)
    Eina_Bool                elm_layout_content_set(Evas_Object *obj, char *swallow, Evas_Object *content)
    Evas_Object             *elm_layout_content_get(Evas_Object *obj, char *swallow)
    Evas_Object             *elm_layout_content_unset(Evas_Object *obj, char *swallow)
    Eina_Bool                elm_layout_text_set(Evas_Object *obj, char *part, char *text)
    char                    *elm_layout_text_get(Evas_Object *obj, char *part)
    void                     elm_layout_icon_set(Evas_Object *obj, Evas_Object *icon)
    Evas_Object             *elm_layout_icon_get(Evas_Object *obj)
    void                     elm_layout_end_set(Evas_Object *obj, Evas_Object *end)
    Evas_Object             *elm_layout_end_get(Evas_Object *obj)

    # List                  (api:TODO  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_list_add(Evas_Object *parent)
    void                     elm_list_go(Evas_Object *obj)
    void                     elm_list_multi_select_set(Evas_Object *obj, Eina_Bool multi)
    Eina_Bool                elm_list_multi_select_get(Evas_Object *obj)
    void                     elm_list_mode_set(Evas_Object *obj, Elm_List_Mode mode)
    Elm_List_Mode            elm_list_mode_get(Evas_Object *obj)
    void                     elm_list_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)
    Eina_Bool                elm_list_horizontal_get(Evas_Object *obj)
    void                     elm_list_select_mode_set(Evas_Object *obj, Elm_Object_Select_Mode mode)
    Elm_Object_Select_Mode   elm_list_select_mode_get(Evas_Object *obj)
    void                     elm_list_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce)
    void                     elm_list_bounce_get(Evas_Object *obj, Eina_Bool *h_bounce, Eina_Bool *v_bounce)
    void                     elm_list_scroller_policy_set(Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v)
    void                     elm_list_scroller_policy_get(Evas_Object *obj, Elm_Scroller_Policy *policy_h, Elm_Scroller_Policy *policy_v)
    Elm_Object_Item         *elm_list_item_append(Evas_Object *obj, char *label, Evas_Object *icon, Evas_Object *end, Evas_Smart_Cb func, void *data)
    Elm_Object_Item         *elm_list_item_prepend(Evas_Object *obj, char *label, Evas_Object *icon, Evas_Object *end, Evas_Smart_Cb func, void *data)
    Elm_Object_Item         *elm_list_item_insert_before(Evas_Object *obj, Elm_Object_Item *before, char *label, Evas_Object *icon, Evas_Object *end, Evas_Smart_Cb func, void *data)
    Elm_Object_Item         *elm_list_item_insert_after(Evas_Object *obj, Elm_Object_Item *after, char *label, Evas_Object *icon, Evas_Object *end, Evas_Smart_Cb func, void *data)
    Elm_Object_Item         *elm_list_item_sorted_insert(Evas_Object *obj, char *label, Evas_Object *icon, Evas_Object *end, Evas_Smart_Cb func, void *data, Eina_Compare_Cb cmp_func)
    void                     elm_list_clear(Evas_Object *obj)
    Eina_List               *elm_list_items_get(Evas_Object *obj)
    Elm_Object_Item         *elm_list_selected_item_get(Evas_Object *obj)
    Eina_List               *elm_list_selected_items_get(Evas_Object *obj)
    void                     elm_list_item_selected_set(Elm_Object_Item *item, Eina_Bool selected)
    Eina_Bool                elm_list_item_selected_get(Elm_Object_Item *item)
    void                     elm_list_item_separator_set(Elm_Object_Item *it, Eina_Bool setting)
    Eina_Bool                elm_list_item_separator_get(Elm_Object_Item *it)
    void                     elm_list_item_show(Elm_Object_Item *item)
    void                     elm_list_item_bring_in(Elm_Object_Item *it)
    Evas_Object             *elm_list_item_object_get(Elm_Object_Item *item)
    Elm_Object_Item         *elm_list_item_prev(Elm_Object_Item *it)
    Elm_Object_Item         *elm_list_item_next(Elm_Object_Item *it)
    Elm_Object_Item         *elm_list_first_item_get(Evas_Object *obj)
    Elm_Object_Item         *elm_list_last_item_get(Evas_Object *obj)

    # Map                   (XXX)

    # Mapbuf                (XXX)

    # Menu                  (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_menu_add(Evas_Object *parent)
    void                     elm_menu_parent_set(Evas_Object *obj, Evas_Object *parent)
    Evas_Object             *elm_menu_parent_get(Evas_Object *obj)
    void                     elm_menu_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
    void                     elm_menu_close(Evas_Object *obj)
    Eina_List               *elm_menu_items_get(Evas_Object *obj)
    Evas_Object             *elm_menu_item_object_get(Elm_Object_Item *it)
    Elm_Object_Item         *elm_menu_item_add(Evas_Object *obj, Elm_Object_Item *parent, char *icon, char *label, Evas_Smart_Cb func, void *data)
    void                     elm_menu_item_icon_name_set(Elm_Object_Item *it, const_char_ptr icon)
    const_char_ptr           elm_menu_item_icon_name_get(Elm_Object_Item *it)
    void                     elm_menu_item_selected_set(Elm_Object_Item *it, Eina_Bool selected)
    Eina_Bool                elm_menu_item_selected_get(Elm_Object_Item *it)
    Elm_Object_Item         *elm_menu_item_separator_add(Evas_Object *obj, Elm_Object_Item *parent)
    Eina_Bool                elm_menu_item_is_separator(Elm_Object_Item *it)
    Eina_List               *elm_menu_item_subitems_get(Elm_Object_Item *it)
    int                      elm_menu_item_index_get(Elm_Object_Item *it)
    Elm_Object_Item         *elm_menu_selected_item_get(Evas_Object *obj)
    Elm_Object_Item         *elm_menu_last_item_get(Evas_Object *obj)
    Elm_Object_Item         *elm_menu_first_item_get(Evas_Object *obj)
    Elm_Object_Item         *elm_menu_item_next_get(Elm_Object_Item *it)
    Elm_Object_Item         *elm_menu_item_prev_get(Elm_Object_Item *it)

    # Multibuttonentry      (XXX)

    # Naviframe             (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_naviframe_add(Evas_Object *parent)
    Elm_Object_Item         *elm_naviframe_item_push(Evas_Object *obj, char *title_label, Evas_Object *prev_btn, Evas_Object *next_btn, Evas_Object *content, char *item_style)
    Elm_Object_Item         *elm_naviframe_item_insert_before(Evas_Object *obj, Elm_Object_Item *before, const_char_ptr title_label, Evas_Object *prev_btn, Evas_Object *next_btn, Evas_Object *content, const_char_ptr item_style)
    Elm_Object_Item         *elm_naviframe_item_insert_after(Evas_Object *obj, Elm_Object_Item *after, const_char_ptr title_label, Evas_Object *prev_btn, Evas_Object *next_btn, Evas_Object *content, const_char_ptr item_style)
    Evas_Object             *elm_naviframe_item_pop(Evas_Object *obj)
    void                     elm_naviframe_item_pop_to(Elm_Object_Item *it)
    void                     elm_naviframe_item_promote(Elm_Object_Item *it)
    void                     elm_naviframe_content_preserve_on_pop_set(Evas_Object *obj, Eina_Bool preserve)
    Eina_Bool                elm_naviframe_content_preserve_on_pop_get(Evas_Object *obj)
    Elm_Object_Item         *elm_naviframe_top_item_get(Evas_Object *obj)
    Elm_Object_Item         *elm_naviframe_bottom_item_get(Evas_Object *obj)
    void                     elm_naviframe_item_style_set(Elm_Object_Item *it, const_char_ptr item_style)
    const_char_ptr           elm_naviframe_item_style_get(Elm_Object_Item *it)
    void                     elm_naviframe_item_title_visible_set(Elm_Object_Item *it, Eina_Bool visible)
    Eina_Bool                elm_naviframe_item_title_visible_get(Elm_Object_Item *it)
    void                     elm_naviframe_prev_btn_auto_pushed_set(Evas_Object *obj, Eina_Bool auto_pushed)
    Eina_Bool                elm_naviframe_prev_btn_auto_pushed_get(Evas_Object *obj)
    Eina_List               *elm_naviframe_items_get(Evas_Object *obj)
    void                     elm_naviframe_event_enabled_set(Evas_Object *obj, Eina_Bool enabled)
    Eina_Bool                elm_naviframe_event_enabled_get(Evas_Object *obj)
    Elm_Object_Item         *elm_naviframe_item_simple_push(Evas_Object *obj, Evas_Object *content)
    void                     elm_naviframe_item_simple_promote(Evas_Object *obj, Evas_Object *content)

    # Notify                (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_notify_add(Evas_Object *parent)
    void                     elm_notify_parent_set(Evas_Object *obj, Evas_Object *parent)
    Evas_Object             *elm_notify_parent_get(Evas_Object *obj)
    void                     elm_notify_orient_set(Evas_Object *obj, int orient)
    int                      elm_notify_orient_get(Evas_Object *obj)
    void                     elm_notify_timeout_set(Evas_Object *obj, double timeout)
    double                   elm_notify_timeout_get(Evas_Object *obj)
    void                     elm_notify_allow_events_set(Evas_Object *obj, Eina_Bool repeat)
    Eina_Bool                elm_notify_allow_events_get(Evas_Object *obj)

    # Panel                 (api:DONE  cb:N/A   test:DONE  doc:DONE)
    Evas_Object             *elm_panel_add(Evas_Object *parent)
    void                     elm_panel_orient_set(Evas_Object *obj, Elm_Panel_Orient orient)
    Elm_Panel_Orient         elm_panel_orient_get(Evas_Object *obj)
    void                     elm_panel_hidden_set(Evas_Object *obj, Eina_Bool hidden)
    Eina_Bool                elm_panel_hidden_get(Evas_Object *obj)
    void                     elm_panel_toggle(Evas_Object *obj)

    # Panes                 (XXX)

    # Photo                 (api:DONE  cb:DONE  test:TODO  doc:DONE)
    Evas_Object             *elm_photo_add(Evas_Object *parent)
    Eina_Bool                elm_photo_file_set(Evas_Object *obj, char *file)
    void                     elm_photo_thumb_set(Evas_Object *obj, char *file, char *group)
    void                     elm_photo_size_set(Evas_Object *obj, int size)
    void                     elm_photo_fill_inside_set(Evas_Object *obj, Eina_Bool fill)
    void                     elm_photo_editable_set(Evas_Object *obj, Eina_Bool editable)
    void                     elm_photo_aspect_fixed_set(Evas_Object *obj, Eina_Bool fixed)
    Eina_Bool                elm_photo_aspect_fixed_get(Evas_Object *obj)

    # Photocam              (XXX)

    # Plug                  (XXX)

    # Popup                 (XXX)

    # Progressbar           (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_progressbar_add(Evas_Object *parent)
    void                     elm_progressbar_pulse_set(Evas_Object *obj, Eina_Bool pulse)
    Eina_Bool                elm_progressbar_pulse_get(Evas_Object *obj)
    void                     elm_progressbar_pulse(Evas_Object *obj, Eina_Bool state)
    void                     elm_progressbar_value_set(Evas_Object *obj, double val)
    double                   elm_progressbar_value_get(Evas_Object *obj)
    void                     elm_progressbar_span_size_set(Evas_Object *obj, Evas_Coord size)
    Evas_Coord               elm_progressbar_span_size_get(Evas_Object *obj)
    void                     elm_progressbar_unit_format_set(Evas_Object *obj, char *format)
    char                    *elm_progressbar_unit_format_get(Evas_Object *obj)
    void                     elm_progressbar_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)
    Eina_Bool                elm_progressbar_horizontal_get(Evas_Object *obj)
    void                     elm_progressbar_inverted_set(Evas_Object *obj, Eina_Bool inverted)
    Eina_Bool                elm_progressbar_inverted_get(Evas_Object *obj)

    # Radio                 (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_radio_add(Evas_Object *parent)
    void                     elm_radio_group_add(Evas_Object *obj, Evas_Object *group)
    void                     elm_radio_state_value_set(Evas_Object *obj, int value)
    int                      elm_radio_state_value_get(Evas_Object *obj)
    void                     elm_radio_value_set(Evas_Object *obj, int value)
    int                      elm_radio_value_get(Evas_Object *obj)
    void                     elm_radio_value_pointer_set(Evas_Object *obj, int *valuep)
    Evas_Object             *elm_radio_selected_object_get(Evas_Object *obj)

    # Route                 (XXX)

    # Scroller              (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_scroller_add(Evas_Object *parent)
    void                     elm_scroller_custom_widget_base_theme_set(Evas_Object *obj, char *widget, char *base)
    void                     elm_scroller_content_min_limit(Evas_Object *obj, Eina_Bool w, Eina_Bool h)
    void                     elm_scroller_region_show(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
    void                     elm_scroller_policy_set(Evas_Object *obj, Elm_Scroller_Policy policy_h, Elm_Scroller_Policy policy_v)
    void                     elm_scroller_policy_get(Evas_Object *obj, Elm_Scroller_Policy *policy_h, Elm_Scroller_Policy *policy_v)
    void                     elm_scroller_region_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
    void                     elm_scroller_child_size_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)
    void                     elm_scroller_bounce_set(Evas_Object *obj, Eina_Bool h_bounce, Eina_Bool v_bounce)
    void                     elm_scroller_bounce_get(Evas_Object *obj, Eina_Bool *h_bounce, Eina_Bool *v_bounce)
    void                     elm_scroller_page_relative_set(Evas_Object *obj, double h_pagerel, double v_pagerel)
    void                     elm_scroller_page_size_set(Evas_Object *obj, Evas_Coord h_pagesize, Evas_Coord v_pagesize)
    void                     elm_scroller_current_page_get(Evas_Object *obj, int *h_pagenumber, int *v_pagenumber)
    void                     elm_scroller_last_page_get(Evas_Object *obj, int *h_pagenumber, int *v_pagenumber)
    void                     elm_scroller_page_show(Evas_Object *obj, int h_pagenumber, int v_pagenumber)
    void                     elm_scroller_page_bring_in(Evas_Object *obj, int h_pagenumber, int v_pagenumber)
    void                     elm_scroller_region_bring_in(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
    void                     elm_scroller_propagate_events_set(Evas_Object *obj, Eina_Bool propagation)
    Eina_Bool                elm_scroller_propagate_events_get(Evas_Object *obj)
    void                     elm_scroller_gravity_set(Evas_Object *obj, double x, double y)
    void                     elm_scroller_gravity_get(Evas_Object *obj, double *x, double *y)

    # SegmentControl        (XXX)

    # Separator             (api:DONE  cb:N/A   test:DONE  doc:DONE)
    Evas_Object             *elm_separator_add(Evas_Object *parent)
    void                     elm_separator_horizontal_set(Evas_Object *obj, Eina_Bool)
    Eina_Bool                elm_separator_horizontal_get(Evas_Object *obj)

    # Slider                (api:TODO  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_slider_add(Evas_Object *parent)
    void                     elm_slider_span_size_set(Evas_Object *obj, Evas_Coord size)
    Evas_Coord               elm_slider_span_size_get(Evas_Object *obj)
    void                     elm_slider_unit_format_set(Evas_Object *obj, char *format)
    char                    *elm_slider_unit_format_get(Evas_Object *obj)
    void                     elm_slider_indicator_format_set(Evas_Object *obj, char *indicator)
    char                    *elm_slider_indicator_format_get(Evas_Object *obj)
    #void                     elm_slider_indicator_format_function_set(Evas_Object *obj, char *(*func)(double val), void (*free_func)(char *str))
    #void                     elm_slider_units_format_function_set(Evas_Object *obj, char *(*func)(double val), void (*free_func)(char *str))
    void                     elm_slider_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)
    Eina_Bool                elm_slider_horizontal_get(Evas_Object *obj)
    void                     elm_slider_min_max_set(Evas_Object *obj, double min, double max)
    void                     elm_slider_min_max_get(Evas_Object *obj, double *min, double *max)
    void                     elm_slider_value_set(Evas_Object *obj, double val)
    double                   elm_slider_value_get(Evas_Object *obj)
    void                     elm_slider_inverted_set(Evas_Object *obj, Eina_Bool inverted)
    Eina_Bool                elm_slider_inverted_get(Evas_Object *obj)
    void                     elm_slider_indicator_show_set(Evas_Object *obj, Eina_Bool show)
    Eina_Bool                elm_slider_indicator_show_get(Evas_Object *obj)

    # Slideshow             (XXX)

    # Spinner               (api:DONE  cb:DONE  test:DONE  doc:DONE)
    Evas_Object             *elm_spinner_add(Evas_Object *parent)
    void                     elm_spinner_label_format_set(Evas_Object *obj, char *format)
    char                    *elm_spinner_label_format_get(Evas_Object *obj)
    void                     elm_spinner_min_max_set(Evas_Object *obj, double min, double max)
    void                     elm_spinner_min_max_get(Evas_Object *obj, double *min, double *max)
    void                     elm_spinner_step_set(Evas_Object *obj, double step)
    double                   elm_spinner_step_get(Evas_Object *obj)
    void                     elm_spinner_value_set(Evas_Object *obj, double val)
    double                   elm_spinner_value_get(Evas_Object *obj)
    void                     elm_spinner_wrap_set(Evas_Object *obj, Eina_Bool wrap)
    Eina_Bool                elm_spinner_wrap_get(Evas_Object *obj)
    void                     elm_spinner_editable_set(Evas_Object *obj, Eina_Bool editable)
    Eina_Bool                elm_spinner_editable_get(Evas_Object *obj)
    void                     elm_spinner_special_value_add(Evas_Object *obj, double value, char *label)
    void                     elm_spinner_interval_set(Evas_Object *obj, double interval)
    double                   elm_spinner_interval_get(Evas_Object *obj)
    void                     elm_spinner_base_set(Evas_Object *obj, double base)
    double                   elm_spinner_base_get(Evas_Object *obj)
    void                     elm_spinner_round_set(Evas_Object *obj, int rnd)
    int                      elm_spinner_round_get(Evas_Object *obj)

    # Table                 (api:DONE  cb:DONE  test:DONE  doc:TODO)
    Evas_Object             *elm_table_add(Evas_Object *parent)
    void                     elm_table_homogeneous_set(Evas_Object *obj, Eina_Bool homogeneous)
    Eina_Bool                elm_table_homogeneous_get(Evas_Object *obj)
    void                     elm_table_padding_set(Evas_Object *obj, Evas_Coord horizontal, Evas_Coord vertical)
    void                     elm_table_padding_get(Evas_Object *obj, Evas_Coord *horizontal, Evas_Coord *vertical)
    void                     elm_table_pack(Evas_Object *obj, Evas_Object *subobj, int x, int y, int w, int h)
    void                     elm_table_unpack(Evas_Object *obj, Evas_Object *subobj)
    void                     elm_table_clear(Evas_Object *obj, Eina_Bool clear)
    void                     elm_table_pack_set(Evas_Object *subobj, int x, int y, int w, int h)
    void                     elm_table_pack_get(Evas_Object *subobj, int *x, int *y, int *w, int *h)

    # Thumb                 (XXX)

    # Toolbar               (api:TODO  cb:DONE  test:DONE  doc:TODO)
    Evas_Object             *elm_toolbar_add(Evas_Object *parent)
    void                     elm_toolbar_icon_size_set(Evas_Object *obj, int icon_size)
    int                      elm_toolbar_icon_size_get(Evas_Object *obj)
    void                     elm_toolbar_icon_order_lookup_set(Evas_Object *obj, Elm_Icon_Lookup_Order order)
    Elm_Icon_Lookup_Order    elm_toolbar_icon_order_lookup_get(Evas_Object *obj)
    Elm_Object_Item         *elm_toolbar_item_append(Evas_Object *obj, char *icon, char *label, Evas_Smart_Cb func, void *data)
    Elm_Object_Item         *elm_toolbar_item_prepend(Evas_Object *obj, char *icon, char *label, Evas_Smart_Cb func, void *data)
    Elm_Object_Item         *elm_toolbar_item_insert_before(Evas_Object *obj, Elm_Object_Item *before, char *icon, char *label, Evas_Smart_Cb func, void *data)
    Elm_Object_Item         *elm_toolbar_item_insert_after(Evas_Object *obj, Elm_Object_Item *after, char *icon, char *label, Evas_Smart_Cb func, void *data)
    Elm_Object_Item         *elm_toolbar_first_item_get(Evas_Object *obj)
    Elm_Object_Item         *elm_toolbar_last_item_get(Evas_Object *obj)
    Elm_Object_Item         *elm_toolbar_item_next_get(Elm_Object_Item *item)
    Elm_Object_Item         *elm_toolbar_item_prev_get(Elm_Object_Item *item)
    void                     elm_toolbar_item_priority_set(Elm_Object_Item *item, int priority)
    int                      elm_toolbar_item_priority_get(Elm_Object_Item *item)
    Elm_Object_Item          elm_toolbar_item_find_by_label(Evas_Object *obj, char *label)
    Eina_Bool                elm_toolbar_item_selected_get(Elm_Object_Item *item)
    void                     elm_toolbar_item_selected_set(Elm_Object_Item *item, Eina_Bool selected)
    Elm_Object_Item         *elm_toolbar_selected_item_get(Evas_Object *obj)
    void                     elm_toolbar_item_icon_set(Elm_Object_Item *item, char *icon)
    char                    *elm_toolbar_item_icon_get(Elm_Object_Item *item)
    Evas_Object             *elm_toolbar_item_object_get(Elm_Object_Item *item)
    Evas_Object             *elm_toolbar_item_icon_object_get(Elm_Object_Item *item)
    #Eina_Bool                elm_toolbar_item_icon_memfile_set(Elm_Object_Item *item, char *img, char *size, char *format, char *key)
    Eina_Bool                elm_toolbar_item_icon_file_set(Elm_Object_Item *item, char *file, char *key)
    void                     elm_toolbar_item_separator_set(Elm_Object_Item *item, Eina_Bool separator)
    Eina_Bool                elm_toolbar_item_separator_get(Elm_Object_Item *item)
    void                     elm_toolbar_shrink_mode_set(Evas_Object *obj, Elm_Toolbar_Shrink_Mode shrink_mode)
    Elm_Toolbar_Shrink_Mode  elm_toolbar_shrink_mode_get(Evas_Object *obj)
    void                     elm_toolbar_homogeneous_set(Evas_Object *obj, Eina_Bool homogeneous)
    Eina_Bool                elm_toolbar_homogeneous_get(Evas_Object *obj)
    void                     elm_toolbar_menu_parent_set(Evas_Object *obj, Evas_Object *parent)
    Evas_Object             *elm_toolbar_menu_parent_get(Evas_Object *obj)
    void                     elm_toolbar_align_set(Evas_Object *obj, double align)
    double                   elm_toolbar_align_get(Evas_Object *obj)
    void                     elm_toolbar_item_menu_set(Elm_Object_Item *item, Eina_Bool menu)
    Evas_Object             *elm_toolbar_item_menu_get(Elm_Object_Item *item)
    Elm_Toolbar_Item_State  *elm_toolbar_item_state_add(Elm_Object_Item *item, char *icon, char *label, Evas_Smart_Cb func, void *data)
    Eina_Bool                elm_toolbar_item_state_del(Elm_Object_Item *item, Elm_Toolbar_Item_State *state)
    Eina_Bool                elm_toolbar_item_state_set(Elm_Object_Item *item, Elm_Toolbar_Item_State *state)
    void                     elm_toolbar_item_state_unset(Elm_Object_Item *item)
    Elm_Toolbar_Item_State  *elm_toolbar_item_state_get(Elm_Object_Item *item)
    Elm_Toolbar_Item_State  *elm_toolbar_item_state_next(Elm_Object_Item *item)
    Elm_Toolbar_Item_State  *elm_toolbar_item_state_prev(Elm_Object_Item *item)
    void                     elm_toolbar_horizontal_set(Evas_Object *obj, Eina_Bool horizontal)
    Eina_Bool                elm_toolbar_horizontal_get(Evas_Object *obj)
    unsigned int             elm_toolbar_items_count(Evas_Object *obj)
    void                     elm_toolbar_select_mode_set(Evas_Object *obj, Elm_Object_Select_Mode mode)
    Elm_Object_Select_Mode   elm_toolbar_select_mode_get(Evas_Object *obj)

    # Transit               (XXX)

    # Video                 (api:DONE  cb:DONE  test:TODO  doc:TODO)
    Evas_Object             *elm_player_add(Evas_Object *parent)
    Evas_Object             *elm_video_add(Evas_Object *parent)
    Eina_Bool                elm_video_file_set(Evas_Object *video, const_char_ptr filename)
    Evas_Object             *elm_video_emotion_get(Evas_Object *video)
    void                     elm_video_play(Evas_Object *video)
    void                     elm_video_pause(Evas_Object *video)
    void                     elm_video_stop(Evas_Object *video)
    Eina_Bool                elm_video_is_playing_get(Evas_Object *video)
    Eina_Bool                elm_video_is_seekable_get(Evas_Object *video)
    Eina_Bool                elm_video_audio_mute_get(Evas_Object *video)
    void                     elm_video_audio_mute_set(Evas_Object *video, Eina_Bool mute)
    double                   elm_video_audio_level_get(Evas_Object *video)
    void                     elm_video_audio_level_set(Evas_Object *video, double volume)
    double                   elm_video_play_position_get(Evas_Object *video)
    void                     elm_video_play_position_set(Evas_Object *video, double position)
    double                   elm_video_play_length_get(Evas_Object *video)
    void                     elm_video_remember_position_set(Evas_Object *video, Eina_Bool remember)
    Eina_Bool                elm_video_remember_position_get(Evas_Object *video)
    const_char_ptr           elm_video_title_get(Evas_Object *video)

    # Web                   (XXX)
    ctypedef struct Elm_Web_Frame_Load_Error:
        int code
        Eina_Bool is_cancellation
        const_char_ptr domain
        const_char_ptr description
        const_char_ptr failing_url
        Evas_Object *frame

    ctypedef void (*Elm_Web_Console_Message)(void *data, Evas_Object *obj, const_char_ptr message, unsigned int line_number, const_char_ptr source_id)

    Eina_Bool elm_need_web()
    Eina_Bool elm_web_history_enabled_get(Evas_Object *obj)
    Evas_Object *elm_web_webkit_view_get(Evas_Object *obj)
    Evas_Object *elm_web_add(Evas_Object *parent)
    Eina_Bool elm_web_uri_set(Evas_Object *obj,char *uri)
    const_char_ptr elm_web_uri_get(Evas_Object *obj)
    const_char_ptr elm_web_useragent_get(Evas_Object *obj)
    double elm_web_zoom_get(Evas_Object *obj)
    Elm_Web_Zoom_Mode elm_web_zoom_mode_get(Evas_Object *obj)

    Eina_Bool elm_web_back(Evas_Object *obj)

    void elm_web_console_message_hook_set(Evas_Object *obj, Elm_Web_Console_Message func, void *data)

    # Window                (api:DONE  cb:DONE  test:TODO  doc:TODO)
    Evas_Object             *elm_win_add(Evas_Object *parent, char *name, Elm_Win_Type type)
    void                     elm_win_resize_object_add(Evas_Object *obj, Evas_Object* subobj)
    void                     elm_win_resize_object_del(Evas_Object *obj, Evas_Object* subobj)
    void                     elm_win_title_set(Evas_Object *obj, char *title)
    char                    *elm_win_title_get(Evas_Object *obj)
    void                     elm_win_icon_name_set(Evas_Object *obj, char *icon_name)
    char                    *elm_win_icon_name_get(Evas_Object *obj)
    void                     elm_win_role_set(Evas_Object *obj, char *role)
    char                    *elm_win_role_get(Evas_Object *obj)
    void                     elm_win_icon_object_set(Evas_Object* obj, Evas_Object* icon)
    const_Evas_Object       *elm_win_icon_object_get(Evas_Object*)
    void                     elm_win_autodel_set(Evas_Object *obj, Eina_Bool autodel)
    Eina_Bool                elm_win_autodel_get(Evas_Object *obj)
    void                     elm_win_activate(Evas_Object *obj)
    void                     elm_win_lower(Evas_Object *obj)
    void                     elm_win_raise(Evas_Object *obj)
    void                     elm_win_center(Evas_Object *obj, Eina_Bool h, Eina_Bool v)
    void                     elm_win_borderless_set(Evas_Object *obj, Eina_Bool borderless)
    Eina_Bool                elm_win_borderless_get(Evas_Object *obj)
    void                     elm_win_shaped_set(Evas_Object *obj, Eina_Bool shaped)
    Eina_Bool                elm_win_shaped_get(Evas_Object *obj)
    void                     elm_win_alpha_set(Evas_Object *obj, Eina_Bool alpha)
    Eina_Bool                elm_win_alpha_get(Evas_Object *obj)
    void                     elm_win_override_set(Evas_Object *obj, Eina_Bool override)
    Eina_Bool                elm_win_override_get(Evas_Object *obj)
    void                     elm_win_fullscreen_set(Evas_Object *obj, Eina_Bool fullscreen)
    Eina_Bool                elm_win_fullscreen_get(Evas_Object *obj)
    void                     elm_win_maximized_set(Evas_Object *obj, Eina_Bool maximized)
    Eina_Bool                elm_win_maximized_get(Evas_Object *obj)
    void                     elm_win_iconified_set(Evas_Object *obj, Eina_Bool iconified)
    Eina_Bool                elm_win_iconified_get(Evas_Object *obj)
    void                     elm_win_withdrawn_set(Evas_Object *obj, Eina_Bool withdrawn)
    Eina_Bool                elm_win_withdrawn_get(Evas_Object *obj)
    void                     elm_win_urgent_set(Evas_Object *obj, Eina_Bool urgent)
    Eina_Bool                elm_win_urgent_get(Evas_Object *obj)
    void                     elm_win_demand_attention_set(Evas_Object *obj, Eina_Bool demand_attention)
    Eina_Bool                elm_win_demand_attention_get(Evas_Object *obj)
    void                     elm_win_modal_set(Evas_Object *obj, Eina_Bool modal)
    Eina_Bool                elm_win_modal_get(Evas_Object *obj)
    void                     elm_win_aspect_set(Evas_Object *obj, double aspect)
    double                   elm_win_aspect_get(Evas_Object *obj)
    void                     elm_win_layer_set(Evas_Object *obj, int layer)
    int                      elm_win_layer_get(Evas_Object *obj)
    void                     elm_win_rotation_set(Evas_Object *obj, int rotation)
    void                     elm_win_rotation_with_resize_set(Evas_Object *obj, int rotation)
    int                      elm_win_rotation_get(Evas_Object *obj)
    void                     elm_win_sticky_set(Evas_Object *obj, Eina_Bool sticky)
    Eina_Bool                elm_win_sticky_get(Evas_Object *obj)
    void                     elm_win_conformant_set(Evas_Object *obj, Eina_Bool conformant)
    Eina_Bool                elm_win_conformant_get(Evas_Object *obj)

    void                     elm_win_quickpanel_set(Evas_Object *obj, Eina_Bool quickpanel)
    Eina_Bool                elm_win_quickpanel_get(Evas_Object *obj)
    void                     elm_win_quickpanel_priority_major_set(Evas_Object *obj, int priority)
    int                      elm_win_quickpanel_priority_major_get(Evas_Object *obj)
    void                     elm_win_quickpanel_priority_minor_set(Evas_Object *obj, int priority)
    int                      elm_win_quickpanel_priority_minor_get(Evas_Object *obj)
    void                     elm_win_quickpanel_zone_set(Evas_Object *obj, int zone)
    int                      elm_win_quickpanel_zone_get(Evas_Object *obj)

    void                     elm_win_prop_focus_skip_set(Evas_Object *obj, Eina_Bool skip)
    void                     elm_win_illume_command_send(Evas_Object *obj, Elm_Illume_Command command, params)
    Evas_Object             *elm_win_inlined_image_object_get(Evas_Object *obj)
    Eina_Bool                elm_win_focus_get(Evas_Object *obj)
    void                     elm_win_screen_constrain_set(Evas_Object *obj, Eina_Bool constrain)
    Eina_Bool                elm_win_screen_constrain_get(Evas_Object *obj)
    void                     elm_win_screen_size_get(Evas_Object *obj, int *x, int *y, int *w, int *h)

    void                     elm_win_focus_highlight_enabled_set(Evas_Object *obj, Eina_Bool enabled)
    Eina_Bool                elm_win_focus_highlight_enabled_get(Evas_Object *obj)
    void                     elm_win_focus_highlight_style_set(Evas_Object *obj, char *style)
    char                    *elm_win_focus_highlight_style_get(Evas_Object *obj)

    void                     elm_win_keyboard_mode_set(Evas_Object *obj, Elm_Win_Keyboard_Mode mode)
    Elm_Win_Keyboard_Mode    elm_win_keyboard_mode_get(Evas_Object *obj)
    void                     elm_win_keyboard_win_set(Evas_Object *obj, Eina_Bool is_keyboard)
    Eina_Bool                elm_win_keyboard_win_get(Evas_Object *obj)

    void                     elm_win_indicator_mode_set(Evas_Object *obj, Elm_Win_Indicator_Mode mode)
    Elm_Win_Indicator_Mode   elm_win_indicator_mode_get(Evas_Object *obj)
    void                     elm_win_indicator_opacity_set(Evas_Object *obj, Elm_Win_Indicator_Opacity_Mode mode)
    Elm_Win_Indicator_Opacity_Mode elm_win_indicator_opacity_get(Evas_Object *obj)

    void                     elm_win_screen_position_get(Evas_Object *obj, int *x, int *y)
    Eina_Bool                elm_win_socket_listen(Evas_Object *obj, char *svcname, int svcnum, Eina_Bool svcsys)

    # X specific call - wont't work on non-x engines (return 0)
    Ecore_X_Window           elm_win_xwindow_get(Evas_Object *obj)
