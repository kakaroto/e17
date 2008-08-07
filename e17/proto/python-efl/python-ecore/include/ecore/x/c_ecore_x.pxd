# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Carsten Haitzler
#
# This file is part of Python-Ecore.
#
# Python-Ecore is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Ecore is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Ecore.  If not, see <http://www.gnu.org/licenses/>.

cdef extern from "sys/types.h":
    ctypedef long pid_t

cdef extern from "Ecore_X.h":
    ####################################################################
    # Data Types
    #
    ctypedef unsigned int Ecore_X_ID
    ctypedef Ecore_X_ID Ecore_X_Window
    ctypedef void *Ecore_X_Visual
    ctypedef Ecore_X_ID Ecore_X_Pixmap
    ctypedef Ecore_X_ID Ecore_X_Drawable
    ctypedef void *Ecore_X_GC
    ctypedef Ecore_X_ID Ecore_X_Atom
    ctypedef Ecore_X_ID Ecore_X_Colormap
    ctypedef Ecore_X_ID Ecore_X_Time
    ctypedef Ecore_X_ID Ecore_X_Cursor
    ctypedef void Ecore_X_Display
    ctypedef void Ecore_X_Connection
    ctypedef void Ecore_X_Screen
    ctypedef Ecore_X_ID Ecore_X_Sync_Counter
    ctypedef Ecore_X_ID Ecore_X_Sync_Alarm

    ctypedef struct Ecore_X_Rectangle:
        int x
        int y
        unsigned int width
        unsigned int height

    ctypedef struct Ecore_X_Icon:
        unsigned int  width
        unsigned int height
        unsigned int *data

    ctypedef enum Ecore_X_Window_State:
        ECORE_X_WINDOW_STATE_ICONIFIED
        ECORE_X_WINDOW_STATE_MODAL
        ECORE_X_WINDOW_STATE_STICKY
        ECORE_X_WINDOW_STATE_MAXIMIZED_VERT
        ECORE_X_WINDOW_STATE_MAXIMIZED_HORZ
        ECORE_X_WINDOW_STATE_SHADED
        ECORE_X_WINDOW_STATE_SKIP_TASKBAR
        ECORE_X_WINDOW_STATE_SKIP_PAGER
        ECORE_X_WINDOW_STATE_HIDDEN
        ECORE_X_WINDOW_STATE_FULLSCREEN
        ECORE_X_WINDOW_STATE_ABOVE
        ECORE_X_WINDOW_STATE_BELOW
        ECORE_X_WINDOW_STATE_DEMANDS_ATTENTION
        ECORE_X_WINDOW_STATE_UNKNOWN

    ctypedef enum Ecore_X_Window_State_Action:
        ECORE_X_WINDOW_STATE_ACTION_REMOVE
        ECORE_X_WINDOW_STATE_ACTION_ADD
        ECORE_X_WINDOW_STATE_ACTION_TOGGLE

    ctypedef enum Ecore_X_Window_Stack_Mode:
        ECORE_X_WINDOW_STACK_ABOVE
        ECORE_X_WINDOW_STACK_BELOW
        ECORE_X_WINDOW_STACK_TOP_IF
        ECORE_X_WINDOW_STACK_BOTTOM_IF
        ECORE_X_WINDOW_STACK_OPPOSITE

    ctypedef enum Ecore_X_Randr_Rotation:
        ECORE_X_RANDR_ROT_0
        ECORE_X_RANDR_ROT_90
        ECORE_X_RANDR_ROT_180
        ECORE_X_RANDR_ROT_270
        ECORE_X_RANDR_FLIP_X
        ECORE_X_RANDR_FLIP_Y

    char *ECORE_X_SELECTION_TARGET_TARGETS
    char *ECORE_X_SELECTION_TARGET_TEXT
    char *ECORE_X_SELECTION_TARGET_COMPOUND_TEXT
    char *ECORE_X_SELECTION_TARGET_STRING
    char *ECORE_X_SELECTION_TARGET_UTF8_STRING
    char *ECORE_X_SELECTION_TARGET_FILENAME

    int ECORE_X_DND_VERSION

    Ecore_X_Atom ECORE_X_DND_ACTION_COPY
    Ecore_X_Atom ECORE_X_DND_ACTION_MOVE
    Ecore_X_Atom ECORE_X_DND_ACTION_LINK
    Ecore_X_Atom ECORE_X_DND_ACTION_ASK
    Ecore_X_Atom ECORE_X_DND_ACTION_PRIVATE

    ctypedef enum Ecore_X_Selection:
        ECORE_X_SELECTION_PRIMARY
        ECORE_X_SELECTION_SECONDARY
        ECORE_X_SELECTION_XDND
        ECORE_X_SELECTION_CLIPBOARD

    ctypedef enum Ecore_X_Event_Mode:
        ECORE_X_EVENT_MODE_NORMAL
        ECORE_X_EVENT_MODE_WHILE_GRABBED
        ECORE_X_EVENT_MODE_GRAB
        ECORE_X_EVENT_MODE_UNGRAB

    ctypedef enum Ecore_X_Event_Detail:
        ECORE_X_EVENT_DETAIL_ANCESTOR
        ECORE_X_EVENT_DETAIL_VIRTUAL
        ECORE_X_EVENT_DETAIL_INFERIOR
        ECORE_X_EVENT_DETAIL_NON_LINEAR
        ECORE_X_EVENT_DETAIL_NON_LINEAR_VIRTUAL
        ECORE_X_EVENT_DETAIL_POINTER
        ECORE_X_EVENT_DETAIL_POINTER_ROOT
        ECORE_X_EVENT_DETAIL_DETAIL_NONE

    ctypedef enum Ecore_X_Event_Mask:
        ECORE_X_EVENT_MASK_NONE
        ECORE_X_EVENT_MASK_KEY_DOWN
        ECORE_X_EVENT_MASK_KEY_UP
        ECORE_X_EVENT_MASK_MOUSE_DOWN
        ECORE_X_EVENT_MASK_MOUSE_UP
        ECORE_X_EVENT_MASK_MOUSE_IN
        ECORE_X_EVENT_MASK_MOUSE_OUT
        ECORE_X_EVENT_MASK_MOUSE_MOVE
        ECORE_X_EVENT_MASK_WINDOW_DAMAGE
        ECORE_X_EVENT_MASK_WINDOW_VISIBILITY
        ECORE_X_EVENT_MASK_WINDOW_CONFIGURE
        ECORE_X_EVENT_MASK_WINDOW_RESIZE_MANAGE
        ECORE_X_EVENT_MASK_WINDOW_MANAGE
        ECORE_X_EVENT_MASK_WINDOW_CHILD_CONFIGURE
        ECORE_X_EVENT_MASK_WINDOW_FOCUS_CHANGE
        ECORE_X_EVENT_MASK_WINDOW_PROPERTY
        ECORE_X_EVENT_MASK_WINDOW_COLORMAP
        ECORE_X_EVENT_MASK_WINDOW_GRAB
        ECORE_X_EVENT_MASK_MOUSE_WHEEL
        ECORE_X_EVENT_MASK_WINDOW_FOCUS_IN
        ECORE_X_EVENT_MASK_WINDOW_FOCUS_OUT

    ctypedef enum Ecore_X_Gravity:
        ECORE_X_GRAVITY_FORGET
        ECORE_X_GRAVITY_UNMAP
        ECORE_X_GRAVITY_NW
        ECORE_X_GRAVITY_N
        ECORE_X_GRAVITY_NE
        ECORE_X_GRAVITY_W
        ECORE_X_GRAVITY_CENTER
        ECORE_X_GRAVITY_E
        ECORE_X_GRAVITY_SW
        ECORE_X_GRAVITY_S
        ECORE_X_GRAVITY_SE
        ECORE_X_GRAVITY_STATIC

    ctypedef enum Ecore_X_Shape_Type:
        ECORE_X_SHAPE_BOUNDING
        ECORE_X_SHAPE_CLIP

    ctypedef struct _Ecore_Point:
        int x
        int y

    ctypedef struct Ecore_X_Event_Key_Down:
        char *keyname
        char *keysymbol
        char *key_compose
        int modifiers
        Ecore_X_Window win
        Ecore_X_Window event_win
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Key_Up:
        char *keyname
        char *keysymbol
        char *key_compose
        int modifiers
        Ecore_X_Window win
        Ecore_X_Window event_win
        Ecore_X_Time time

    ctypedef struct Ecore_X_Event_Mouse_Button_Down:
        int button
        int modifiers
        int x
        int y
        _Ecore_Point root
        Ecore_X_Window win
        Ecore_X_Window event_win
        Ecore_X_Time time
        unsigned int double_click
        unsigned int triple_click


    ctypedef struct Ecore_X_Event_Mouse_Button_Up:
        int button
        int modifiers
        int x
        int y
        _Ecore_Point root
        Ecore_X_Window win
        Ecore_X_Window event_win
        Ecore_X_Time time
        unsigned int double_click
        unsigned int triple_click


    ctypedef struct Ecore_X_Event_Mouse_Move:
        int modifiers
        int x
        int y
        _Ecore_Point root
        Ecore_X_Window win
        Ecore_X_Window event_win
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Mouse_In:
        int modifiers
        int x
        int y
        _Ecore_Point root
        Ecore_X_Window win
        Ecore_X_Window event_win
        Ecore_X_Event_Mode mode
        Ecore_X_Event_Detail detail
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Mouse_Out:
        int modifiers
        int x
        int y
        _Ecore_Point root
        Ecore_X_Window win
        Ecore_X_Window event_win
        Ecore_X_Event_Mode mode
        Ecore_X_Event_Detail detail
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Mouse_Wheel:
        int direction
        int z
        int modifiers
        int x
        int y
        _Ecore_Point root
        Ecore_X_Window win
        Ecore_X_Window event_win
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Focus_In:
        Ecore_X_Window win
        Ecore_X_Event_Mode mode
        Ecore_X_Event_Detail detail
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Focus_Out:
        Ecore_X_Window win
        Ecore_X_Event_Mode mode
        Ecore_X_Event_Detail detail
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Keymap:
        Ecore_X_Window win


    ctypedef struct Ecore_X_Event_Window_Damage:
        Ecore_X_Window win
        int x
        int y
        int w
        int h
        int count
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Visibility_Change:
        Ecore_X_Window win
        int fully_obscured
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Create:
        Ecore_X_Window win
        int override
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Destroy:
        Ecore_X_Window win
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Hide:
        Ecore_X_Window win
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Show:
        Ecore_X_Window win
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Show_Request:
        Ecore_X_Window win
        Ecore_X_Window parent
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Reparent:
        Ecore_X_Window win
        Ecore_X_Window parent
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Configure:
        Ecore_X_Window win
        Ecore_X_Window abovewin
        int x
        int y
        int w
        int h
        int border
        unsigned int override
        unsigned int from_wm
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Configure_Request:
        Ecore_X_Window win
        Ecore_X_Window abovewin
        int x
        int y
        int w
        int h
        int border
        Ecore_X_Window_Stack_Mode detail
        unsigned long value_mask
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Gravity:
        Ecore_X_Window win
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Resize_Request:
        Ecore_X_Window win
        int w
        int h
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Stack:
        Ecore_X_Window win
        Ecore_X_Window event_win
        Ecore_X_Window_Stack_Mode detail
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Stack_Request:
        Ecore_X_Window win
        Ecore_X_Window parent
        Ecore_X_Window_Stack_Mode detail
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Property:
        Ecore_X_Window win
        Ecore_X_Atom atom
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Colormap:
        Ecore_X_Window win
        Ecore_X_Colormap cmap
        int installed
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Selection_Clear:
        Ecore_X_Window win
        Ecore_X_Selection selection
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Selection_Request:
        Ecore_X_Window owner
        Ecore_X_Window requestor
        Ecore_X_Time time
        Ecore_X_Atom selection
        Ecore_X_Atom target
        Ecore_X_Atom property


    ctypedef struct Ecore_X_Event_Selection_Notify:
        Ecore_X_Window win
        Ecore_X_Time time
        Ecore_X_Selection selection
        char *target
        void *data


    ctypedef enum _Ecore_X_Selection_Data_Content:
        ECORE_X_SELECTION_CONTENT_NONE
        ECORE_X_SELECTION_CONTENT_TEXT
        ECORE_X_SELECTION_CONTENT_FILES
        ECORE_X_SELECTION_CONTENT_TARGETS
        ECORE_X_SELECTION_CONTENT_CUSTOM


    ctypedef struct Ecore_X_Selection_Data:
        _Ecore_X_Selection_Data_Content content
        unsigned char *data
        int length
        int format
        int (*free)(void *data)


    ctypedef struct Ecore_X_Selection_Data_Files:
        Ecore_X_Selection_Data data
        char **files
        int num_files


    ctypedef struct Ecore_X_Selection_Data_Text:
        Ecore_X_Selection_Data data
        char *text


    ctypedef struct Ecore_X_Selection_Data_Targets:
        Ecore_X_Selection_Data data
        char **targets
        int num_targets


    ctypedef struct Ecore_X_Event_Xdnd_Enter:
        Ecore_X_Window win
        Ecore_X_Window source
        char **types
        int num_types


    ctypedef struct Ecore_X_Event_Xdnd_Position:
        Ecore_X_Window win
        Ecore_X_Window source
        _Ecore_Point position
        Ecore_X_Atom action


    ctypedef struct Ecore_X_Event_Xdnd_Status:
        Ecore_X_Window win
        Ecore_X_Window target
        int will_accept
        Ecore_X_Rectangle rectangle
        Ecore_X_Atom action


    ctypedef struct Ecore_X_Event_Xdnd_Leave:
        Ecore_X_Window win
        Ecore_X_Window source


    ctypedef struct Ecore_X_Event_Xdnd_Drop:
        Ecore_X_Window win
        Ecore_X_Window source
        Ecore_X_Atom action
        _Ecore_Point position


    ctypedef struct Ecore_X_Event_Xdnd_Finished:
        Ecore_X_Window win
        Ecore_X_Window target
        int completed
        Ecore_X_Atom action


    ctypedef union _Ecore_X_Event_Client_Message_Data:
        char b[20]
        short s[10]
        long l[5]

    ctypedef struct Ecore_X_Event_Client_Message:
        Ecore_X_Window win
        Ecore_X_Atom message_type
        int format
        _Ecore_X_Event_Client_Message_Data data
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Shape:
        Ecore_X_Window win
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Screensaver_Notify:
        Ecore_X_Window win
        int on
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Sync_Counter:
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Sync_Alarm:
        Ecore_X_Time time
        Ecore_X_Sync_Alarm alarm


    ctypedef struct Ecore_X_Event_Screen_Change:
        Ecore_X_Window win
        Ecore_X_Window root
        int width
        int height


    ctypedef struct Ecore_X_Event_Window_Delete_Request:
        Ecore_X_Window win
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Prop_Title_Change:
        Ecore_X_Window win
        char *title
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Prop_Visible_Title_Change:
        Ecore_X_Window win
        char *title
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Prop_Icon_Name_Change:
        Ecore_X_Window win
        char *name
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Prop_Visible_Icon_Name_Change:
        Ecore_X_Window win
        char *name
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Prop_Client_Machine_Change:
        Ecore_X_Window win
        char *name
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Prop_Name_Class_Change:
        Ecore_X_Window win
        char *name
        char *clas
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Prop_Pid_Change:
        Ecore_X_Window win
        pid_t pid
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Window_Prop_Desktop_Change:
        Ecore_X_Window win
        long desktop
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Startup_Sequence:
        Ecore_X_Window win


    ctypedef struct Ecore_X_Event_Window_Move_Resize_Request:
        Ecore_X_Window win
        int x
        int y
        int direction
        int button
        int source


    ctypedef struct Ecore_X_Event_Window_State_Request:
        Ecore_X_Window win
        Ecore_X_Window_State_Action action
        Ecore_X_Window_State state[2]
        int source


    ctypedef struct Ecore_X_Event_Frame_Extents_Request:
        Ecore_X_Window win


    ctypedef struct Ecore_X_Event_Ping:
        Ecore_X_Window win
        Ecore_X_Window event_win
        Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Desktop_Change:
        Ecore_X_Window win
        unsigned int desk
        int source


    ctypedef struct Ecore_X_Event_Startup_Sequence:
        Ecore_X_Window win


    ctypedef struct Ecore_X_Event_Window_Move_Resize_Request:
        Ecore_X_Window win
        int x
        int y
        int direction
        int button
        int source


    ctypedef struct Ecore_X_Event_Window_State_Request:
       Ecore_X_Window win
       Ecore_X_Window_State_Action action
       Ecore_X_Window_State state[2]
       int source


    ctypedef struct Ecore_X_Event_Frame_Extents_Request:
       Ecore_X_Window win


    ctypedef struct Ecore_X_Event_Ping:
       Ecore_X_Window win
       Ecore_X_Window event_win
       Ecore_X_Time time


    ctypedef struct Ecore_X_Event_Desktop_Change:
       Ecore_X_Window win
       unsigned int desk
       int source

    int ECORE_X_EVENT_KEY_DOWN
    int ECORE_X_EVENT_KEY_UP
    int ECORE_X_EVENT_MOUSE_BUTTON_DOWN
    int ECORE_X_EVENT_MOUSE_BUTTON_UP
    int ECORE_X_EVENT_MOUSE_MOVE
    int ECORE_X_EVENT_MOUSE_IN
    int ECORE_X_EVENT_MOUSE_OUT
    int ECORE_X_EVENT_MOUSE_WHEEL
    int ECORE_X_EVENT_WINDOW_FOCUS_IN
    int ECORE_X_EVENT_WINDOW_FOCUS_OUT
    int ECORE_X_EVENT_WINDOW_KEYMAP
    int ECORE_X_EVENT_WINDOW_DAMAGE
    int ECORE_X_EVENT_WINDOW_VISIBILITY_CHANGE
    int ECORE_X_EVENT_WINDOW_CREATE
    int ECORE_X_EVENT_WINDOW_DESTROY
    int ECORE_X_EVENT_WINDOW_HIDE
    int ECORE_X_EVENT_WINDOW_SHOW
    int ECORE_X_EVENT_WINDOW_SHOW_REQUEST
    int ECORE_X_EVENT_WINDOW_REPARENT
    int ECORE_X_EVENT_WINDOW_CONFIGURE
    int ECORE_X_EVENT_WINDOW_CONFIGURE_REQUEST
    int ECORE_X_EVENT_WINDOW_GRAVITY
    int ECORE_X_EVENT_WINDOW_RESIZE_REQUEST
    int ECORE_X_EVENT_WINDOW_STACK
    int ECORE_X_EVENT_WINDOW_STACK_REQUEST
    int ECORE_X_EVENT_WINDOW_PROPERTY
    int ECORE_X_EVENT_WINDOW_COLORMAP
    int ECORE_X_EVENT_WINDOW_MAPPING
    int ECORE_X_EVENT_SELECTION_CLEAR
    int ECORE_X_EVENT_SELECTION_REQUEST
    int ECORE_X_EVENT_SELECTION_NOTIFY
    int ECORE_X_EVENT_CLIENT_MESSAGE
    int ECORE_X_EVENT_WINDOW_SHAPE
    int ECORE_X_EVENT_SCREENSAVER_NOTIFY
    int ECORE_X_EVENT_SYNC_COUNTER
    int ECORE_X_EVENT_SYNC_ALARM
    int ECORE_X_EVENT_SCREEN_CHANGE
    int ECORE_X_EVENT_DAMAGE_NOTIFY

    int ECORE_X_EVENT_WINDOW_DELETE_REQUEST

    int ECORE_X_EVENT_WINDOW_MOVE_RESIZE_REQUEST
    int ECORE_X_EVENT_WINDOW_STATE_REQUEST
    int ECORE_X_EVENT_FRAME_EXTENTS_REQUEST
    int ECORE_X_EVENT_PING
    int ECORE_X_EVENT_DESKTOP_CHANGE

    int ECORE_X_EVENT_STARTUP_SEQUENCE_NEW
    int ECORE_X_EVENT_STARTUP_SEQUENCE_CHANGE
    int ECORE_X_EVENT_STARTUP_SEQUENCE_REMOVE

    int ECORE_X_EVENT_XDND_ENTER
    int ECORE_X_EVENT_XDND_POSITION
    int ECORE_X_EVENT_XDND_STATUS
    int ECORE_X_EVENT_XDND_LEAVE
    int ECORE_X_EVENT_XDND_DROP
    int ECORE_X_EVENT_XDND_FINISHED

    int ECORE_X_MODIFIER_SHIFT
    int ECORE_X_MODIFIER_CTRL
    int ECORE_X_MODIFIER_ALT
    int ECORE_X_MODIFIER_WIN

    int ECORE_X_LOCK_SCROLL
    int ECORE_X_LOCK_NUM
    int ECORE_X_LOCK_CAPS

    ctypedef enum Ecore_X_WM_Protocol:
        ECORE_X_WM_PROTOCOL_DELETE_REQUEST
        ECORE_X_WM_PROTOCOL_TAKE_FOCUS
        ECORE_X_NET_WM_PROTOCOL_PING
        ECORE_X_NET_WM_PROTOCOL_SYNC_REQUEST
        ECORE_X_WM_PROTOCOL_NUM

    ctypedef enum Ecore_X_Window_Input_Mode:
        ECORE_X_WINDOW_INPUT_MODE_NONE
        ECORE_X_WINDOW_INPUT_MODE_PASSIVE
        ECORE_X_WINDOW_INPUT_MODE_ACTIVE_LOCAL
        ECORE_X_WINDOW_INPUT_MODE_ACTIVE_GLOBAL

    ctypedef enum Ecore_X_Window_State_Hint:
        ECORE_X_WINDOW_STATE_HINT_NONE
        ECORE_X_WINDOW_STATE_HINT_WITHDRAWN
        ECORE_X_WINDOW_STATE_HINT_NORMAL
        ECORE_X_WINDOW_STATE_HINT_ICONIC

    ctypedef enum Ecore_X_Window_Type:
        ECORE_X_WINDOW_TYPE_DESKTOP
        ECORE_X_WINDOW_TYPE_DOCK
        ECORE_X_WINDOW_TYPE_TOOLBAR
        ECORE_X_WINDOW_TYPE_MENU
        ECORE_X_WINDOW_TYPE_UTILITY
        ECORE_X_WINDOW_TYPE_SPLASH
        ECORE_X_WINDOW_TYPE_DIALOG
        ECORE_X_WINDOW_TYPE_NORMAL
        ECORE_X_WINDOW_TYPE_UNKNOWN

    ctypedef enum Ecore_X_Action:
        ECORE_X_ACTION_MOVE
        ECORE_X_ACTION_RESIZE
        ECORE_X_ACTION_MINIMIZE
        ECORE_X_ACTION_SHADE
        ECORE_X_ACTION_STICK
        ECORE_X_ACTION_MAXIMIZE_HORZ
        ECORE_X_ACTION_MAXIMIZE_VERT
        ECORE_X_ACTION_FULLSCREEN
        ECORE_X_ACTION_CHANGE_DESKTOP
        ECORE_X_ACTION_CLOSE

    ctypedef enum Ecore_X_Window_Configure_Mask:
        ECORE_X_WINDOW_CONFIGURE_MASK_X
        ECORE_X_WINDOW_CONFIGURE_MASK_Y
        ECORE_X_WINDOW_CONFIGURE_MASK_W
        ECORE_X_WINDOW_CONFIGURE_MASK_H
        ECORE_X_WINDOW_CONFIGURE_MASK_BORDER_WIDTH
        ECORE_X_WINDOW_CONFIGURE_MASK_SIBLING
        ECORE_X_WINDOW_CONFIGURE_MASK_STACK_MODE

    int ECORE_X_WINDOW_LAYER_BELOW
    int ECORE_X_WINDOW_LAYER_NORMAL
    int ECORE_X_WINDOW_LAYER_ABOVE
    int ECORE_X_PROP_LIST_REMOVE
    int ECORE_X_PROP_LIST_ADD
    int ECORE_X_PROP_LIST_TOGGLE

    ctypedef enum Ecore_X_MWM_Hint_Func:
        ECORE_X_MWM_HINT_FUNC_ALL
        ECORE_X_MWM_HINT_FUNC_RESIZE
        ECORE_X_MWM_HINT_FUNC_MOVE
        ECORE_X_MWM_HINT_FUNC_MINIMIZE
        ECORE_X_MWM_HINT_FUNC_MAXIMIZE
        ECORE_X_MWM_HINT_FUNC_CLOSE

    ctypedef enum Ecore_X_MWM_Hint_Decor:
        ECORE_X_MWM_HINT_DECOR_ALL
        ECORE_X_MWM_HINT_DECOR_BORDER
        ECORE_X_MWM_HINT_DECOR_RESIZEH
        ECORE_X_MWM_HINT_DECOR_TITLE
        ECORE_X_MWM_HINT_DECOR_MENU
        ECORE_X_MWM_HINT_DECOR_MINIMIZE
        ECORE_X_MWM_HINT_DECOR_MAXIMIZE

    ctypedef enum Ecore_X_MWM_Hint_Input:
        ECORE_X_MWM_HINT_INPUT_MODELESS
        ECORE_X_MWM_HINT_INPUT_PRIMARY_APPLICATION_MODAL
        ECORE_X_MWM_HINT_INPUT_SYSTEM_MODAL
        ECORE_X_MWM_HINT_INPUT_FULL_APPLICATION_MODAL

    ctypedef struct _Ecore_X_Window_Attributes_Event_Mask:
        Ecore_X_Event_Mask mine
        Ecore_X_Event_Mask all
        Ecore_X_Event_Mask no_propagate

    ctypedef struct Ecore_X_Window_Attributes:
        Ecore_X_Window root
        int x
        int y
        int w
        int h
        int border
        int depth
        unsigned char visible
        unsigned char viewable
        unsigned char override
        unsigned char input_only
        unsigned char save_under
        _Ecore_X_Window_Attributes_Event_Mask event_mask
        Ecore_X_Gravity window_gravity
        Ecore_X_Gravity pixel_gravity
        Ecore_X_Colormap colormap
        Ecore_X_Visual visual

    ctypedef struct Ecore_X_Screen_Size:
        int width
        int height

    ctypedef struct Ecore_X_Screen_Refresh_Rate:
        int rate

    ctypedef Ecore_X_ID Ecore_X_Picture
    ctypedef Ecore_X_ID Ecore_X_Region
    ctypedef enum Ecore_X_Region_Type:
        ECORE_X_REGION_BOUNDING
        ECORE_X_REGION_CLIP

    ctypedef Ecore_X_ID Ecore_X_Picture
    ctypedef Ecore_X_ID Ecore_X_Region
    ctypedef enum Ecore_X_Region_Type:
        ECORE_X_REGION_BOUNDING
        ECORE_X_REGION_CLIP

    ctypedef Ecore_X_ID Ecore_X_Damage

    ctypedef enum Ecore_X_Damage_Report_Level:
        ECORE_X_DAMAGE_REPORT_RAW_RECTANGLES
        ECORE_X_DAMAGE_REPORT_DELTA_RECTANGLES
        ECORE_X_DAMAGE_REPORT_BOUNDING_BOX
        ECORE_X_DAMAGE_REPORT_NON_EMPTY

    ctypedef struct Ecore_X_Event_Damage:
        Ecore_X_Damage_Report_Level level
        Ecore_X_Drawable drawable
        Ecore_X_Damage damage
        int more
        Ecore_X_Time time
        Ecore_X_Rectangle area
        Ecore_X_Rectangle geometry


    ####################################################################
    # Functions
    #
    int ecore_x_init(char *name)
    int ecore_x_shutdown()
    int ecore_x_disconnect()
    Ecore_X_Display *ecore_x_display_get()
    Ecore_X_Connection *ecore_x_connection_get()
    int ecore_x_fd_get()
    Ecore_X_Screen *ecore_x_default_screen_get()
    void ecore_x_double_click_time_set(double t)
    double ecore_x_double_click_time_get()
    void ecore_x_flush()
    void ecore_x_sync()
    void ecore_x_killall(Ecore_X_Window root)
    void ecore_x_kill(Ecore_X_Window win)

    Ecore_X_Time ecore_x_current_time_get()

    void ecore_x_error_handler_set(void (*func) (void *data), void *data)
    void ecore_x_io_error_handler_set(void (*func) (void *data), void *data)
    int ecore_x_error_request_get()
    int ecore_x_error_code_get()

    void ecore_x_event_mask_set(Ecore_X_Window w, Ecore_X_Event_Mask mask)
    void ecore_x_event_mask_unset(Ecore_X_Window w, Ecore_X_Event_Mask mask)

    int ecore_x_selection_notify_send(Ecore_X_Window requestor, Ecore_X_Atom selection, Ecore_X_Atom target, Ecore_X_Atom property, Ecore_X_Time time)
    void ecore_x_selection_primary_prefetch()
    void ecore_x_selection_primary_fetch()
    int ecore_x_selection_primary_set(Ecore_X_Window w, void *data, int size)
    int ecore_x_selection_primary_clear()
    void ecore_x_selection_secondary_prefetch()
    void ecore_x_selection_secondary_fetch()
    int ecore_x_selection_secondary_set(Ecore_X_Window w, void *data, int size)
    int ecore_x_selection_secondary_clear()
    void ecore_x_selection_xdnd_prefetch()
    void ecore_x_selection_xdnd_fetch()
    int ecore_x_selection_xdnd_set(Ecore_X_Window w, void *data, int size)
    int ecore_x_selection_xdnd_clear()
    void ecore_x_selection_clipboard_prefetch()
    void ecore_x_selection_clipboard_fetch()
    int ecore_x_selection_clipboard_set(Ecore_X_Window w, void *data, int size)
    int ecore_x_selection_clipboard_clear()
    void ecore_x_selection_primary_request(Ecore_X_Window w, char *target)
    void ecore_x_selection_secondary_request(Ecore_X_Window w, char *target)
    void ecore_x_selection_xdnd_request(Ecore_X_Window w, char *target)
    void ecore_x_selection_clipboard_request(Ecore_X_Window w, char *target)
    int ecore_x_selection_convert(Ecore_X_Atom selection, Ecore_X_Atom target, void **data_ret)
    void ecore_x_selection_converter_add(char *target, int (*func)(char *target, void *data, int size, void **data_ret, int *size_ret))
    void ecore_x_selection_converter_atom_add(Ecore_X_Atom target, int (*func)(char *target, void *data, int size, void **data_ret, int *size_ret))
    void ecore_x_selection_converter_del(char *target)
    void ecore_x_selection_converter_atom_del(Ecore_X_Atom target)
    void ecore_x_selection_parser_add(char *target, void *(*func)(char *target, void *data, int size, int format))
    void ecore_x_selection_parser_del(char *target)

    void ecore_x_dnd_aware_set(Ecore_X_Window win, int on)
    void ecore_x_dnd_version_get_prefetch(Ecore_X_Window window)
    void ecore_x_dnd_version_get_fetch()
    int ecore_x_dnd_version_get(Ecore_X_Window win)
    void ecore_x_dnd_type_get_prefetch(Ecore_X_Window window)
    void ecore_x_dnd_type_get_fetch()
    int ecore_x_dnd_type_isset(Ecore_X_Window win, char *type)
    void ecore_x_dnd_type_set(Ecore_X_Window win, char *type, int on)
    void ecore_x_dnd_types_set(Ecore_X_Window win, char **types, unsigned int num_types)
    void ecore_x_dnd_actions_set(Ecore_X_Window win, Ecore_X_Atom *actions, unsigned int num_actions)
    void ecore_x_dnd_begin_prefetch(Ecore_X_Window source)
    void ecore_x_dnd_begin_fetch()
    int ecore_x_dnd_begin(Ecore_X_Window source, unsigned char *data, int size)
    int ecore_x_dnd_drop()
    void ecore_x_dnd_send_status(int will_accept, int suppress, Ecore_X_Rectangle rectangle, Ecore_X_Atom action)
    void ecore_x_dnd_send_finished()

    Ecore_X_Window ecore_x_window_new(Ecore_X_Window parent, int x, int y, int w, int h)
    Ecore_X_Window ecore_x_window_override_new(Ecore_X_Window parent, int x, int y, int w, int h)
    int ecore_x_window_argb_get(Ecore_X_Window win)
    Ecore_X_Window ecore_x_window_manager_argb_new(Ecore_X_Window parent, int x, int y, int w, int h)
    Ecore_X_Window ecore_x_window_argb_new(Ecore_X_Window parent, int x, int y, int w, int h)
    Ecore_X_Window ecore_x_window_override_argb_new(Ecore_X_Window parent, int x, int y, int w, int h)
    Ecore_X_Window ecore_x_window_input_new(Ecore_X_Window parent, int x, int y, int w, int h)
    void ecore_x_window_configure(Ecore_X_Window win, Ecore_X_Window_Configure_Mask mask, int x, int y, int w, int h, int border_width, Ecore_X_Window sibling, int stack_mode)
    void ecore_x_window_cursor_set(Ecore_X_Window win, Ecore_X_Cursor c)
    void ecore_x_window_del(Ecore_X_Window win)
    void ecore_x_window_ignore_set(Ecore_X_Window win, int ignore)
    Ecore_X_Window *ecore_x_window_ignore_list(int *num)

    void ecore_x_window_delete_request_send(Ecore_X_Window win)
    void ecore_x_window_show(Ecore_X_Window win)
    void ecore_x_window_hide(Ecore_X_Window win)
    void ecore_x_window_move(Ecore_X_Window win, int x, int y)
    void ecore_x_window_resize(Ecore_X_Window win, int w, int h)
    void ecore_x_window_move_resize(Ecore_X_Window win, int x, int y, int w, int h)
    void ecore_x_window_focus(Ecore_X_Window win)
    void ecore_x_window_focus_at_time(Ecore_X_Window win, Ecore_X_Time t)
    void ecore_x_get_input_focus_prefetch()
    void ecore_x_get_input_focus_fetch()
    Ecore_X_Window ecore_x_window_focus_get()
    void ecore_x_window_raise(Ecore_X_Window win)
    void ecore_x_window_lower(Ecore_X_Window win)
    void ecore_x_window_reparent(Ecore_X_Window win, Ecore_X_Window new_parent, int x, int y)
    void ecore_x_window_size_get(Ecore_X_Window win, int *w, int *h)
    void ecore_x_window_geometry_get(Ecore_X_Window win, int *x, int *y, int *w, int *h)
    int ecore_x_window_border_width_get(Ecore_X_Window win)
    void ecore_x_window_border_width_set(Ecore_X_Window win, int width)
    int ecore_x_window_depth_get(Ecore_X_Window win)
    void ecore_x_window_cursor_show(Ecore_X_Window win, int show)
    void ecore_x_window_defaults_set(Ecore_X_Window win)
    int ecore_x_window_visible_get(Ecore_X_Window win)
    Ecore_X_Window ecore_x_window_shadow_tree_at_xy_with_skip_get(Ecore_X_Window base, int x, int y, Ecore_X_Window *skip, int skip_num)
    void ecore_x_window_shadow_tree_flush()
    Ecore_X_Window ecore_x_window_root_get(Ecore_X_Window win)
    Ecore_X_Window ecore_x_window_at_xy_get(int x, int y)
    Ecore_X_Window ecore_x_window_at_xy_with_skip_get(int x, int y, Ecore_X_Window *skip, int skip_num)
    Ecore_X_Window ecore_x_window_at_xy_begin_get(Ecore_X_Window begin, int x, int y)
    void ecore_x_query_tree_prefetch(Ecore_X_Window window)
    void ecore_x_query_tree_fetch()
    Ecore_X_Window ecore_x_window_parent_get(Ecore_X_Window win)

    void ecore_x_window_background_color_set(Ecore_X_Window win, unsigned short r, unsigned short g, unsigned short b)
    void ecore_x_window_gravity_set(Ecore_X_Window win, Ecore_X_Gravity grav)
    void ecore_x_window_pixel_gravity_set(Ecore_X_Window win, Ecore_X_Gravity grav)
    void ecore_x_window_pixmap_set(Ecore_X_Window win, Ecore_X_Pixmap pmap)
    void ecore_x_window_area_clear(Ecore_X_Window win, int x, int y, int w, int h)
    void ecore_x_window_area_expose(Ecore_X_Window win, int x, int y, int w, int h)
    void ecore_x_window_override_set(Ecore_X_Window win, int override)

    void ecore_x_window_prop_card32_set(Ecore_X_Window win, Ecore_X_Atom atom, unsigned int *val, unsigned int num)
    void ecore_x_window_prop_card32_get_prefetch(Ecore_X_Window window, Ecore_X_Atom atom)
    void ecore_x_window_prop_card32_get_fetch()
    int ecore_x_window_prop_card32_get(Ecore_X_Window win, Ecore_X_Atom atom, unsigned int *val, unsigned int len)
    int ecore_x_window_prop_card32_list_get(Ecore_X_Window win, Ecore_X_Atom atom, unsigned int **plst)

    void ecore_x_window_prop_xid_set(Ecore_X_Window win, Ecore_X_Atom atom, Ecore_X_Atom type, Ecore_X_ID * lst, unsigned int num)
    void ecore_x_window_prop_xid_get_prefetch(Ecore_X_Window window, Ecore_X_Atom atom, Ecore_X_Atom type)
    void ecore_x_window_prop_xid_get_fetch()
    int ecore_x_window_prop_xid_get(Ecore_X_Window win, Ecore_X_Atom atom, Ecore_X_Atom type, Ecore_X_ID * lst, unsigned int len)
    int ecore_x_window_prop_xid_list_get(Ecore_X_Window win, Ecore_X_Atom atom, Ecore_X_Atom type, Ecore_X_ID ** plst)
    void ecore_x_window_prop_xid_list_change(Ecore_X_Window win, Ecore_X_Atom atom, Ecore_X_Atom type, Ecore_X_ID item, int op)
    void ecore_x_window_prop_atom_set(Ecore_X_Window win, Ecore_X_Atom atom, Ecore_X_Atom * val, unsigned int num)
    void ecore_x_window_prop_atom_get_prefetch(Ecore_X_Window window, Ecore_X_Atom atom)
    void ecore_x_window_prop_atom_get_fetch()
    int ecore_x_window_prop_atom_get(Ecore_X_Window win, Ecore_X_Atom atom, Ecore_X_Atom * val, unsigned int len)
    int ecore_x_window_prop_atom_list_get(Ecore_X_Window win, Ecore_X_Atom atom, Ecore_X_Atom ** plst)
    void ecore_x_window_prop_atom_list_change(Ecore_X_Window win, Ecore_X_Atom atom, Ecore_X_Atom item, int op)
    void ecore_x_window_prop_window_set(Ecore_X_Window win, Ecore_X_Atom atom, Ecore_X_Window * val, unsigned int num)
    void ecore_x_window_prop_window_get_prefetch(Ecore_X_Window window, Ecore_X_Atom atom)
    void ecore_x_window_prop_window_get_fetch()
    int ecore_x_window_prop_window_get(Ecore_X_Window win, Ecore_X_Atom atom, Ecore_X_Window * val, unsigned int len)
    int ecore_x_window_prop_window_list_get(Ecore_X_Window win, Ecore_X_Atom atom, Ecore_X_Window ** plst)

    Ecore_X_Atom ecore_x_window_prop_any_type()
    void ecore_x_window_prop_property_set(Ecore_X_Window win, Ecore_X_Atom type, Ecore_X_Atom format, int size, void *data, int number)
    void ecore_x_window_prop_property_get_prefetch(Ecore_X_Window window, Ecore_X_Atom property, Ecore_X_Atom type)
    void ecore_x_window_prop_property_get_fetch()
    int ecore_x_window_prop_property_get(Ecore_X_Window win, Ecore_X_Atom property, Ecore_X_Atom type, int size, unsigned char **data, int *num)
    void ecore_x_window_prop_property_del(Ecore_X_Window win, Ecore_X_Atom property)
    void ecore_x_window_prop_list_prefetch(Ecore_X_Window window)
    void ecore_x_window_prop_list_fetch()
    Ecore_X_Atom *ecore_x_window_prop_list(Ecore_X_Window win, int *num_ret)
    void ecore_x_window_prop_string_set(Ecore_X_Window win, Ecore_X_Atom type, char *str)
    void ecore_x_window_prop_string_get_prefetch(Ecore_X_Window window, Ecore_X_Atom type)
    void ecore_x_window_prop_string_get_fetch()
    char *ecore_x_window_prop_string_get(Ecore_X_Window win, Ecore_X_Atom type)
    int ecore_x_window_prop_protocol_isset(Ecore_X_Window win, Ecore_X_WM_Protocol protocol)
    Ecore_X_WM_Protocol *ecore_x_window_prop_protocol_list_get(Ecore_X_Window win, int *num_ret)

    void ecore_x_window_shape_mask_set(Ecore_X_Window win, Ecore_X_Pixmap mask)
    void ecore_x_window_shape_window_set(Ecore_X_Window win, Ecore_X_Window shape_win)
    void ecore_x_window_shape_window_set_xy(Ecore_X_Window win, Ecore_X_Window shape_win, int x, int y)
    void ecore_x_window_shape_rectangle_set(Ecore_X_Window win, int x, int y, int w, int h)
    void ecore_x_window_shape_rectangles_set(Ecore_X_Window win, Ecore_X_Rectangle *rects, int num)
    void ecore_x_window_shape_window_add(Ecore_X_Window win, Ecore_X_Window shape_win)
    void ecore_x_window_shape_window_add_xy(Ecore_X_Window win, Ecore_X_Window shape_win, int x, int y)
    void ecore_x_window_shape_rectangle_add(Ecore_X_Window win, int x, int y, int w, int h)
    void ecore_x_window_shape_rectangle_clip(Ecore_X_Window win, int x, int y, int w, int h)
    void ecore_x_window_shape_rectangles_add(Ecore_X_Window win, Ecore_X_Rectangle *rects, int num)
    void ecore_x_window_shape_rectangles_get_prefetch(Ecore_X_Window window)
    void ecore_x_window_shape_rectangles_get_fetch()
    Ecore_X_Rectangle *ecore_x_window_shape_rectangles_get(Ecore_X_Window win, int *num_ret)
    void ecore_x_window_shape_events_select(Ecore_X_Window win, int on)

    Ecore_X_Pixmap ecore_x_pixmap_new(Ecore_X_Window win, int w, int h, int dep)
    void ecore_x_pixmap_del(Ecore_X_Pixmap pmap)
    void ecore_x_pixmap_paste(Ecore_X_Pixmap pmap, Ecore_X_Drawable dest, Ecore_X_GC gc, int sx, int sy, int w, int h, int dx, int dy)
    void ecore_x_pixmap_geometry_get(Ecore_X_Pixmap pmap, int *x, int *y, int *w, int *h)
    int ecore_x_pixmap_depth_get(Ecore_X_Pixmap pmap)

    Ecore_X_GC ecore_x_gc_new(Ecore_X_Drawable draw)
    void ecore_x_gc_del(Ecore_X_GC gc)

    int ecore_x_client_message32_send(Ecore_X_Window win, Ecore_X_Atom type, Ecore_X_Event_Mask mask, long d0, long d1, long d2, long d3, long d4)
    int ecore_x_client_message8_send(Ecore_X_Window win, Ecore_X_Atom type, void *data, int len)
    int ecore_x_mouse_move_send(Ecore_X_Window win, int x, int y)
    int ecore_x_mouse_down_send(Ecore_X_Window win, int x, int y, int b)
    int ecore_x_mouse_up_send(Ecore_X_Window win, int x, int y, int b)

    void ecore_x_drawable_geometry_get_prefetch(Ecore_X_Drawable drawable)
    void ecore_x_drawable_geometry_get_fetch()
    void ecore_x_drawable_geometry_get(Ecore_X_Drawable d, int *x, int *y, int *w, int *h)
    int ecore_x_drawable_border_width_get(Ecore_X_Drawable d)
    int ecore_x_drawable_depth_get(Ecore_X_Drawable d)

    int ecore_x_cursor_color_supported_get()
    Ecore_X_Cursor ecore_x_cursor_new(Ecore_X_Window win, int *pixels, int w, int h, int hot_x, int hot_y)
    void ecore_x_cursor_free(Ecore_X_Cursor c)
    Ecore_X_Cursor ecore_x_cursor_shape_get(int shape)
    void ecore_x_cursor_size_set(int size)
    int ecore_x_cursor_size_get()

    Ecore_X_Window *ecore_x_window_root_list(int *num_ret)
    Ecore_X_Window ecore_x_window_root_first_get()
    int ecore_x_window_manage(Ecore_X_Window win)
    void ecore_x_window_container_manage(Ecore_X_Window win)
    void ecore_x_window_client_manage(Ecore_X_Window win)
    void ecore_x_window_sniff(Ecore_X_Window win)
    void ecore_x_window_client_sniff(Ecore_X_Window win)
    void ecore_x_atom_get_prefetch(char *name)
    void ecore_x_atom_get_fetch()
    Ecore_X_Atom ecore_x_atom_get(char *name)

    void ecore_x_icccm_init()
    void ecore_x_icccm_state_set(Ecore_X_Window win, Ecore_X_Window_State_Hint state)
    Ecore_X_Window_State_Hint ecore_x_icccm_state_get(Ecore_X_Window win)
    void ecore_x_icccm_delete_window_send(Ecore_X_Window win, Ecore_X_Time t)
    void ecore_x_icccm_take_focus_send(Ecore_X_Window win, Ecore_X_Time t)
    void ecore_x_icccm_save_yourself_send(Ecore_X_Window win, Ecore_X_Time t)
    void ecore_x_icccm_move_resize_send(Ecore_X_Window win, int x, int y, int w, int h)
    void ecore_x_icccm_hints_set(Ecore_X_Window win, int accepts_focus, Ecore_X_Window_State_Hint initial_state, Ecore_X_Pixmap icon_pixmap, Ecore_X_Pixmap icon_mask, Ecore_X_Window icon_window, Ecore_X_Window window_group, int is_urgent)
    int ecore_x_icccm_hints_get(Ecore_X_Window win, int *accepts_focus, Ecore_X_Window_State_Hint *initial_state, Ecore_X_Pixmap *icon_pixmap, Ecore_X_Pixmap *icon_mask, Ecore_X_Window *icon_window, Ecore_X_Window *window_group, int *is_urgent)
    void ecore_x_icccm_size_pos_hints_set(Ecore_X_Window win, int request_pos, Ecore_X_Gravity gravity, int min_w, int min_h, int max_w, int max_h, int base_w, int base_h, int step_x, int step_y, double min_aspect, double max_aspect)
    int ecore_x_icccm_size_pos_hints_get(Ecore_X_Window win, int *request_pos, Ecore_X_Gravity *gravity, int *min_w, int *min_h, int *max_w, int *max_h, int *base_w, int *base_h, int *step_x, int *step_y, double *min_aspect, double *max_aspect)
    void ecore_x_icccm_title_set(Ecore_X_Window win, char *t)
    char *ecore_x_icccm_title_get(Ecore_X_Window win)
    void ecore_x_icccm_protocol_set(Ecore_X_Window win, Ecore_X_WM_Protocol protocol, int on)
    int ecore_x_icccm_protocol_isset(Ecore_X_Window win, Ecore_X_WM_Protocol protocol)
    void ecore_x_icccm_name_class_set(Ecore_X_Window win, char *n, char *c)
    void ecore_x_icccm_name_class_get(Ecore_X_Window win, char **n, char **c)
    char *ecore_x_icccm_client_machine_get(Ecore_X_Window win)
    void ecore_x_icccm_command_set(Ecore_X_Window win, int argc, char **argv)
    void ecore_x_icccm_command_get(Ecore_X_Window win, int *argc, char ***argv)
    char *ecore_x_icccm_icon_name_get(Ecore_X_Window win)
    void ecore_x_icccm_icon_name_set(Ecore_X_Window win, char *t)
    void ecore_x_icccm_colormap_window_set(Ecore_X_Window win, Ecore_X_Window subwin)
    void ecore_x_icccm_colormap_window_unset(Ecore_X_Window win, Ecore_X_Window subwin)
    void ecore_x_icccm_transient_for_set(Ecore_X_Window win, Ecore_X_Window forwin)
    void ecore_x_icccm_transient_for_unset(Ecore_X_Window win)
    Ecore_X_Window ecore_x_icccm_transient_for_get(Ecore_X_Window win)
    void ecore_x_icccm_window_role_set(Ecore_X_Window win, char *role)
    char *ecore_x_icccm_window_role_get(Ecore_X_Window win)
    void ecore_x_icccm_client_leader_set(Ecore_X_Window win, Ecore_X_Window l)
    Ecore_X_Window ecore_x_icccm_client_leader_get(Ecore_X_Window win)
    void ecore_x_icccm_iconic_request_send(Ecore_X_Window win, Ecore_X_Window root)

    void ecore_x_mwm_hints_get_prefetch(Ecore_X_Window window)
    void ecore_x_mwm_hints_get_fetch()
    int  ecore_x_mwm_hints_get(Ecore_X_Window win, Ecore_X_MWM_Hint_Func *fhint, Ecore_X_MWM_Hint_Decor *dhint, Ecore_X_MWM_Hint_Input *ihint)
    void ecore_x_mwm_borderless_set(Ecore_X_Window win, int borderless)

    void ecore_x_netwm_init()
    void ecore_x_netwm_shutdown()
    void ecore_x_netwm_wm_identify(Ecore_X_Window root, Ecore_X_Window check, char *wm_name)
    void ecore_x_netwm_supported_set(Ecore_X_Window root, Ecore_X_Atom *supported, int num)
    void ecore_x_netwm_supported_get_prefetch(Ecore_X_Window root)
    void ecore_x_netwm_supported_get_fetch()
    int ecore_x_netwm_supported_get(Ecore_X_Window root, Ecore_X_Atom **supported, int *num)
    void ecore_x_netwm_desk_count_set(Ecore_X_Window root, unsigned int n_desks)
    void ecore_x_netwm_desk_roots_set(Ecore_X_Window root, Ecore_X_Window *vroots, unsigned int n_desks)
    void ecore_x_netwm_desk_names_set(Ecore_X_Window root, char **names, unsigned int n_desks)
    void ecore_x_netwm_desk_size_set(Ecore_X_Window root, unsigned int width, unsigned int height)
    void ecore_x_netwm_desk_workareas_set(Ecore_X_Window root, unsigned int *areas, unsigned int n_desks)
    void ecore_x_netwm_desk_current_set(Ecore_X_Window root, unsigned int desk)
    void ecore_x_netwm_desk_viewports_set(Ecore_X_Window root, unsigned int *origins, unsigned int n_desks)
    void ecore_x_netwm_desk_layout_set(Ecore_X_Window root, int orientation, int columns, int rows, int starting_corner)
    void ecore_x_netwm_showing_desktop_set(Ecore_X_Window root, int on)
    void ecore_x_netwm_client_list_set(Ecore_X_Window root, Ecore_X_Window *p_clients, unsigned int n_clients)
    void ecore_x_netwm_client_list_stacking_set(Ecore_X_Window root, Ecore_X_Window *p_clients, unsigned int n_clients)
    void ecore_x_netwm_client_active_set(Ecore_X_Window root, Ecore_X_Window win)
    void ecore_x_netwm_name_set(Ecore_X_Window win, char *name)
    void ecore_x_netwm_name_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_name_get_fetch()
    int ecore_x_netwm_name_get(Ecore_X_Window win, char **name)
    void ecore_x_netwm_startup_id_set(Ecore_X_Window win, char *id)
    void ecore_x_netwm_startup_id_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_startup_id_get_fetch()
    int ecore_x_netwm_startup_id_get(Ecore_X_Window win, char **id)
    void ecore_x_netwm_visible_name_set(Ecore_X_Window win, char *name)
    void ecore_x_netwm_visible_name_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_visible_name_get_fetch()
    int ecore_x_netwm_visible_name_get(Ecore_X_Window win, char **name)
    void ecore_x_netwm_icon_name_set(Ecore_X_Window win, char *name)
    void ecore_x_netwm_icon_name_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_icon_name_get_fetch()
    int ecore_x_netwm_icon_name_get(Ecore_X_Window win, char **name)
    void ecore_x_netwm_visible_icon_name_set(Ecore_X_Window win, char *name)
    void ecore_x_netwm_visible_icon_name_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_visible_icon_name_get_fetch()
    int ecore_x_netwm_visible_icon_name_get(Ecore_X_Window win, char **name)
    void ecore_x_netwm_desktop_set(Ecore_X_Window win, unsigned int desk)
    void ecore_x_netwm_desktop_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_desktop_get_fetch()
    int ecore_x_netwm_desktop_get(Ecore_X_Window win, unsigned int *desk)
    void ecore_x_netwm_strut_set(Ecore_X_Window win, int left, int right, int top, int bottom)
    void ecore_x_netwm_strut_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_strut_get_fetch()
    int ecore_x_netwm_strut_get(Ecore_X_Window win, int *left, int *right, int *top, int *bottom)
    void ecore_x_netwm_strut_partial_set(Ecore_X_Window win, int left, int right, int top, int bottom, int left_start_y, int left_end_y, int right_start_y, int right_end_y, int top_start_x, int top_end_x, int bottom_start_x, int bottom_end_x)
    void ecore_x_netwm_strut_partial_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_strut_partial_get_fetch()
    int ecore_x_netwm_strut_partial_get(Ecore_X_Window win, int *left, int *right, int *top, int *bottom, int *left_start_y, int *left_end_y, int *right_start_y, int *right_end_y, int *top_start_x, int *top_end_x, int *bottom_start_x, int *bottom_end_x)
    void ecore_x_netwm_icons_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_icons_get_fetch()
    int ecore_x_netwm_icons_get(Ecore_X_Window win, Ecore_X_Icon **icon, int *num)
    void ecore_x_netwm_icon_geometry_set(Ecore_X_Window win, int x, int y, int width, int height)
    void ecore_x_netwm_icon_geometry_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_icon_geometry_get_fetch()
    int ecore_x_netwm_icon_geometry_get(Ecore_X_Window win, int *x, int *y, int *width, int *height)
    void ecore_x_netwm_pid_set(Ecore_X_Window win, int pid)
    void ecore_x_netwm_pid_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_pid_get_fetch()
    int ecore_x_netwm_pid_get(Ecore_X_Window win, int *pid)
    void ecore_x_netwm_handled_icons_set(Ecore_X_Window win)
    void ecore_x_netwm_handled_icons_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_handled_icons_get_fetch()
    int ecore_x_netwm_handled_icons_get(Ecore_X_Window win)
    void ecore_x_netwm_user_time_set(Ecore_X_Window win, unsigned int time)
    void ecore_x_netwm_user_time_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_user_time_get_fetch()
    int ecore_x_netwm_user_time_get(Ecore_X_Window win, unsigned int *time)
    void ecore_x_netwm_window_state_set(Ecore_X_Window win, Ecore_X_Window_State *state, unsigned int num)
    void ecore_x_netwm_window_state_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_window_state_get_fetch()
    int ecore_x_netwm_window_state_get(Ecore_X_Window win, Ecore_X_Window_State **state, unsigned int *num)
    void ecore_x_netwm_window_type_set(Ecore_X_Window win, Ecore_X_Window_Type type)
    void ecore_x_netwm_window_type_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_window_type_get_fetch()
    int ecore_x_netwm_window_type_get(Ecore_X_Window win, Ecore_X_Window_Type *type)
    int ecore_x_netwm_allowed_action_isset(Ecore_X_Window win, Ecore_X_Action action)
    void ecore_x_netwm_allowed_action_set(Ecore_X_Window win, Ecore_X_Action *action, unsigned int num)
    void ecore_x_netwm_allowed_action_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_allowed_action_get_fetch()
    int ecore_x_netwm_allowed_action_get(Ecore_X_Window win, Ecore_X_Action **action, unsigned int *num)
    void ecore_x_netwm_opacity_set(Ecore_X_Window win, unsigned int opacity)
    void ecore_x_netwm_opacity_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_opacity_get_fetch()
    int ecore_x_netwm_opacity_get(Ecore_X_Window win, unsigned int *opacity)
    void ecore_x_netwm_frame_size_set(Ecore_X_Window win, int fl, int fr, int ft, int fb)
    void ecore_x_netwm_frame_size_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_frame_size_get_fetch()
    int ecore_x_netwm_frame_size_get(Ecore_X_Window win, int *fl, int *fr, int *ft, int *fb)
    void ecore_x_netwm_sync_counter_get_prefetch(Ecore_X_Window window)
    void ecore_x_netwm_sync_counter_get_fetch()
    int ecore_x_netwm_sync_counter_get(Ecore_X_Window win, Ecore_X_Sync_Counter *counter)
    void ecore_x_netwm_ping_send(Ecore_X_Window win)
    void ecore_x_netwm_sync_request_send(Ecore_X_Window win, unsigned int serial)
    void ecore_x_netwm_state_request_send(Ecore_X_Window win, Ecore_X_Window root, Ecore_X_Window_State s1, Ecore_X_Window_State s2, int set)
    void ecore_x_netwm_desktop_request_send(Ecore_X_Window win, Ecore_X_Window root, unsigned int desktop)

    void ecore_x_e_init()
    void ecore_x_e_frame_size_set(Ecore_X_Window win, int fl, int fr, int ft, int fb)

    void ecore_x_xinerama_query_screens_prefetch()
    void ecore_x_xinerama_query_screens_fetch()
    int ecore_x_xinerama_screen_count_get()
    int ecore_x_xinerama_screen_geometry_get(int screen, int *x, int *y, int *w, int *h)

    void ecore_x_get_window_attributes_prefetch(Ecore_X_Window window)
    void ecore_x_get_window_attributes_fetch()
    int  ecore_x_window_attributes_get(Ecore_X_Window win, Ecore_X_Window_Attributes *att_ret)
    void ecore_x_window_save_set_add(Ecore_X_Window win)
    void ecore_x_window_save_set_del(Ecore_X_Window win)
    Ecore_X_Window *ecore_x_window_children_get(Ecore_X_Window win, int *num)

    int  ecore_x_pointer_control_set(int accel_num, int accel_denom, int threshold)
    void ecore_x_pointer_control_get_prefetch()
    void ecore_x_pointer_control_get_fetch()
    int  ecore_x_pointer_control_get(int *accel_num, int *accel_denom, int *threshold)
    int  ecore_x_pointer_grab(Ecore_X_Window win)
    int  ecore_x_pointer_confine_grab(Ecore_X_Window win)
    void ecore_x_pointer_ungrab()
    int  ecore_x_pointer_warp(Ecore_X_Window win, int x, int y)
    int  ecore_x_keyboard_grab(Ecore_X_Window win)
    void ecore_x_keyboard_ungrab()
    void ecore_x_grab()
    void ecore_x_ungrab()
    void ecore_x_passive_grab_replay_func_set(int (*func) (void *data, int event_type, void *event), void *data)
    void ecore_x_window_button_grab(Ecore_X_Window win, int button, Ecore_X_Event_Mask event_mask, int mod, int any_mod)
    void ecore_x_window_button_ungrab(Ecore_X_Window win, int button, int mod, int any_mod)
    void ecore_x_window_key_grab(Ecore_X_Window win, char *key, int mod, int any_mod)
    void ecore_x_window_key_ungrab(Ecore_X_Window win, char *key, int mod, int any_mod)

    void ecore_x_focus_reset()
    void ecore_x_events_allow_all()
    void ecore_x_pointer_last_xy_get(int *x, int *y)
    void ecore_x_pointer_xy_get_prefetch(Ecore_X_Window window)
    void ecore_x_pointer_xy_get_fetch()
    void ecore_x_pointer_xy_get(Ecore_X_Window win, int *x, int *y)

    Ecore_X_Sync_Alarm ecore_x_sync_alarm_new(Ecore_X_Sync_Counter counter)
    int ecore_x_sync_alarm_free(Ecore_X_Sync_Alarm alarm)

    int ecore_x_randr_events_select(Ecore_X_Window win, int on)
    void ecore_x_randr_get_screen_info_prefetch(Ecore_X_Window window)
    void ecore_x_randr_get_screen_info_fetch()
    Ecore_X_Randr_Rotation ecore_x_randr_screen_rotations_get(Ecore_X_Window root)
    Ecore_X_Randr_Rotation ecore_x_randr_screen_rotation_get(Ecore_X_Window root)
    void ecore_x_randr_screen_rotation_set(Ecore_X_Window root, Ecore_X_Randr_Rotation rot)
    Ecore_X_Screen_Size *ecore_x_randr_screen_sizes_get(Ecore_X_Window root, int *num)
    Ecore_X_Screen_Size ecore_x_randr_current_screen_size_get(Ecore_X_Window root)
    int ecore_x_randr_screen_size_set(Ecore_X_Window root, Ecore_X_Screen_Size size)

    Ecore_X_Screen_Refresh_Rate *ecore_x_randr_screen_refresh_rates_get(Ecore_X_Window root, int size_id, int *num)
    Ecore_X_Screen_Refresh_Rate ecore_x_randr_current_screen_refresh_rate_get(Ecore_X_Window root)

    int ecore_x_randr_screen_refresh_rate_set(Ecore_X_Window root, Ecore_X_Screen_Size size, Ecore_X_Screen_Refresh_Rate rate)

    Ecore_X_Region ecore_x_region_new(Ecore_X_Rectangle *rects, int num)
    Ecore_X_Region ecore_x_region_new_from_bitmap(Ecore_X_Pixmap bitmap)
    Ecore_X_Region ecore_x_region_new_from_window(Ecore_X_Window win, Ecore_X_Region_Type type)
    Ecore_X_Region ecore_x_region_new_from_gc(Ecore_X_GC gc)
    Ecore_X_Region ecore_x_region_new_from_picture(Ecore_X_Picture picture)
    void ecore_x_region_del(Ecore_X_Region region)
    void ecore_x_region_set(Ecore_X_Region region, Ecore_X_Rectangle *rects, int num)
    void ecore_x_region_copy(Ecore_X_Region dest, Ecore_X_Region source)
    void ecore_x_region_combine(Ecore_X_Region dest, Ecore_X_Region source1, Ecore_X_Region source2)
    void ecore_x_region_intersect(Ecore_X_Region dest, Ecore_X_Region source1, Ecore_X_Region source2)
    void ecore_x_region_subtract(Ecore_X_Region dest, Ecore_X_Region source1, Ecore_X_Region source2)
    void ecore_x_region_invert(Ecore_X_Region dest, Ecore_X_Rectangle *bounds, Ecore_X_Region source)
    void ecore_x_region_translate(Ecore_X_Region region, int dx, int dy)
    void ecore_x_region_extents(Ecore_X_Region dest, Ecore_X_Region source)
    void ecore_x_region_fetch_prefetch(Ecore_X_Region region)
    void ecore_x_region_fetch_fetch()
    Ecore_X_Rectangle * ecore_x_region_fetch(Ecore_X_Region region, int *num, Ecore_X_Rectangle *bounds)
    void ecore_x_region_expand(Ecore_X_Region dest, Ecore_X_Region source, unsigned int left, unsigned int right, unsigned int top, unsigned int bottom)
    void ecore_x_region_gc_clip_set(Ecore_X_Region region, Ecore_X_GC gc, int x_origin, int y_origin)
    void ecore_x_region_window_shape_set(Ecore_X_Region region, Ecore_X_Window win, Ecore_X_Shape_Type type, int x_offset, int y_offset)
    void ecore_x_region_picture_clip_set(Ecore_X_Region region, Ecore_X_Picture picture, int x_origin, int y_origin)

    int ecore_x_composite_query()

    int ecore_x_damage_query()
    Ecore_X_Damage ecore_x_damage_new(Ecore_X_Drawable d, Ecore_X_Damage_Report_Level level)
    void ecore_x_damage_del(Ecore_X_Damage damage)
    void ecore_x_damage_subtract(Ecore_X_Damage damage, Ecore_X_Region repair, Ecore_X_Region parts)

    int ecore_x_screen_is_composited(int screen)

    int ecore_x_dpms_query()
    void ecore_x_dpms_capable_get_prefetch()
    void ecore_x_dpms_capable_get_fetch()
    int ecore_x_dpms_capable_get()
    void ecore_x_dpms_enable_get_prefetch()
    void ecore_x_dpms_enable_get_fetch()
    int ecore_x_dpms_enabled_get()
    void ecore_x_dpms_enabled_set(int enabled)
    void ecore_x_dpms_timeouts_get_prefetch()
    void ecore_x_dpms_timeouts_get_fetch()
    void ecore_x_dpms_timeouts_get(unsigned int *standby, unsigned int *suspend, unsigned int *off)
    int ecore_x_dpms_timeouts_set(unsigned int standby, unsigned int suspend, unsigned int off)
    unsigned int ecore_x_dpms_timeout_standby_get()
    unsigned int ecore_x_dpms_timeout_suspend_get()
    unsigned int ecore_x_dpms_timeout_off_get()
    void ecore_x_dpms_timeout_standby_set(unsigned int new_timeout)
    void ecore_x_dpms_timeout_suspend_set(unsigned int new_timeout)
    void ecore_x_dpms_timeout_off_set(unsigned int new_timeout)

    ctypedef enum Ecore_X_Virtual_Keyboard_State:
        ECORE_X_VIRTUAL_KEYBOARD_STATE_UNKNOWN
        ECORE_X_VIRTUAL_KEYBOARD_STATE_OFF
        ECORE_X_VIRTUAL_KEYBOARD_STATE_ON
        ECORE_X_VIRTUAL_KEYBOARD_STATE_ALPHA
        ECORE_X_VIRTUAL_KEYBOARD_STATE_NUMERIC
        ECORE_X_VIRTUAL_KEYBOARD_STATE_PIN
        ECORE_X_VIRTUAL_KEYBOARD_STATE_PHONE_NUMBER
        ECORE_X_VIRTUAL_KEYBOARD_STATE_HEX
        ECORE_X_VIRTUAL_KEYBOARD_STATE_TERMINAL
        ECORE_X_VIRTUAL_KEYBOARD_STATE_PASSWORD
        
    void ecore_x_e_virtual_keyboard_state_set(Ecore_X_Window win, Ecore_X_Virtual_Keyboard_State state)

    
cimport ecore.c_ecore


cdef class Window:
    cdef readonly Ecore_X_Window xid

    cdef int _set_xid(self, Ecore_X_Window xid)


cdef class EventKeyDown(ecore.c_ecore.Event):
    cdef readonly object keyname
    cdef readonly object keysymbol
    cdef readonly object key_compose
    cdef readonly int modifiers
    cdef readonly Window win
    cdef readonly Window event_win
    cdef readonly unsigned int time


cdef class EventKeyUp(ecore.c_ecore.Event):
    cdef readonly object keyname
    cdef readonly object keysymbol
    cdef readonly object key_compose
    cdef readonly int modifiers
    cdef readonly Window win
    cdef readonly Window event_win
    cdef readonly unsigned int time


cdef class EventPoint:
    cdef readonly int x
    cdef readonly int y


cdef class EventMouseButtonDown(ecore.c_ecore.Event):
    cdef readonly int button
    cdef readonly int modifiers
    cdef readonly int x
    cdef readonly int y
    cdef readonly EventPoint root
    cdef readonly Window win
    cdef readonly Window event_win
    cdef readonly unsigned int time
    cdef readonly int double_click
    cdef readonly int triple_click


cdef class EventMouseButtonUp(ecore.c_ecore.Event):
    cdef readonly int button
    cdef readonly int modifiers
    cdef readonly int x
    cdef readonly int y
    cdef readonly EventPoint root
    cdef readonly Window win
    cdef readonly Window event_win
    cdef readonly unsigned int time
    cdef readonly int double_click
    cdef readonly int triple_click


cdef class EventMouseMove(ecore.c_ecore.Event):
    cdef readonly int modifiers
    cdef readonly int x
    cdef readonly int y
    cdef readonly EventPoint root
    cdef readonly Window win
    cdef readonly Window event_win
    cdef readonly unsigned int time


cdef class EventMouseIn(ecore.c_ecore.Event):
    cdef readonly int modifiers
    cdef readonly int x
    cdef readonly int y
    cdef readonly EventPoint root
    cdef readonly Window win
    cdef readonly Window event_win
    cdef readonly int mode
    cdef readonly int detail
    cdef readonly unsigned int time


cdef class EventMouseOut(ecore.c_ecore.Event):
    cdef readonly int modifiers
    cdef readonly int x
    cdef readonly int y
    cdef readonly EventPoint root
    cdef readonly Window win
    cdef readonly Window event_win
    cdef readonly int mode
    cdef readonly int detail
    cdef readonly unsigned int time


cdef class EventMouseWheel(ecore.c_ecore.Event):
    cdef readonly int direction
    cdef readonly int z
    cdef readonly int modifiers
    cdef readonly int x
    cdef readonly int y
    cdef readonly EventPoint root
    cdef readonly Window win
    cdef readonly Window event_win
    cdef readonly unsigned int time


cdef class EventWindowFocusIn(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly int mode
    cdef readonly int detail
    cdef readonly unsigned int time


cdef class EventWindowFocusOut(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly int mode
    cdef readonly int detail
    cdef readonly unsigned int time


cdef class EventWindowKeymap(ecore.c_ecore.Event):
    cdef readonly Window win


cdef class EventWindowDamage(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly int x
    cdef readonly int y
    cdef readonly int w
    cdef readonly int h
    cdef readonly int count
    cdef readonly unsigned int time


cdef class EventWindowVisibilityChange(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly int fully_obscured
    cdef readonly unsigned int time


cdef class EventWindowCreate(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly int override
    cdef readonly unsigned int time


cdef class EventWindowDestroy(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly unsigned int time


cdef class EventWindowHide(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly unsigned int time


cdef class EventWindowShow(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly unsigned int time


cdef class EventWindowShowRequest(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly Window parent
    cdef readonly unsigned int time


cdef class EventWindowReparent(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly Window parent
    cdef readonly unsigned int time


cdef class EventWindowConfigure(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly Window above_win
    cdef readonly int x
    cdef readonly int y
    cdef readonly int w
    cdef readonly int h
    cdef readonly int border
    cdef readonly unsigned int override
    cdef readonly unsigned int from_wm
    cdef readonly unsigned int time


cdef class EventWindowConfigureRequest(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly Window above_win
    cdef readonly int x
    cdef readonly int y
    cdef readonly int w
    cdef readonly int h
    cdef readonly int border
    cdef readonly int stack_mode
    cdef readonly unsigned long value_mask
    cdef readonly unsigned int time


cdef class EventWindowGravity(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly unsigned int time


cdef class EventWindowResizeRequest(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly int w
    cdef readonly int h
    cdef readonly unsigned int time


cdef class EventWindowStack(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly Window event_win
    cdef readonly int stack_mode
    cdef readonly unsigned int time


cdef class EventWindowStackRequest(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly Window parent
    cdef readonly int stack_mode
    cdef readonly unsigned int time


cdef class EventDesktopChange(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly long desktop
    cdef readonly unsigned int time


cdef class EventPing(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly Window event_win
    cdef readonly unsigned int time


cdef class EventWindowStateRequest(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly int action
    cdef readonly int state1
    cdef readonly int state2
    cdef readonly int source


cdef class EventFrameExtentsRequest(ecore.c_ecore.Event):
    cdef readonly Window win


cdef class EventWindowShape(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly unsigned int time


cdef class EventScreensaverNotify(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly int on
    cdef readonly unsigned int time


cdef class EventSyncCounter(ecore.c_ecore.Event):
    cdef readonly unsigned int time


cdef class EventSyncAlarm(ecore.c_ecore.Event):
    cdef readonly unsigned int time
    cdef readonly unsigned int alarm


cdef class EventScreenChange(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly Window root
    cdef readonly int width
    cdef readonly int height


cdef class EventWindowDeleteRequest(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly unsigned int time


cdef class EventWindowMoveResizeRequest(ecore.c_ecore.Event):
    cdef readonly Window win
    cdef readonly int x
    cdef readonly int y
    cdef readonly int button
    cdef readonly int source




## cdef class EventWindowProperty(ecore.c_ecore.Event):
##     cdef readonly Window win
##     Atom atom
##     cdef readonly unsigned int time


## cdef class EventWindowColormap(ecore.c_ecore.Event):
##     cdef readonly Window win
##     Colormap cmap
##     cdef readonly int installed
##     cdef readonly unsigned int time


## cdef class EventSelectionClear(ecore.c_ecore.Event):
##     cdef readonly Window win
##     Selection selection
##     cdef readonly unsigned int time


## cdef class EventSelectionRequest(ecore.c_ecore.Event):
##     cdef readonly Window owner
##     cdef readonly Window requestor
##     cdef readonly unsigned int time
##     Atom selection
##     Atom target
##     Atom property


## cdef class EventSelectionNotify(ecore.c_ecore.Event):
##     cdef readonly Window win
##     cdef readonly unsigned int time
##     Selection selection
##     cdef readonly object target
##     void *data


## cdef class EventDamage(ecore.c_ecore.Event):
##     cdef readonly int level
##     Ecore_X_Drawable drawable
##     Ecore_X_Damage damage
##     cdef readonly int more
##     cdef readonly unsigned int time
##     cdef readonly EventRectangle area
##     cdef readonly EventRectangle geometry
