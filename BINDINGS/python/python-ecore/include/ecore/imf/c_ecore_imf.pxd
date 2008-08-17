# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
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

cdef extern from "Ecore_IMF.h":
    ####################################################################
    # Enumerations
    #
    ctypedef enum Ecore_IMF_Event_Type:
        ECORE_IMF_EVENT_MOUSE_DOWN
        ECORE_IMF_EVENT_MOUSE_UP
        ECORE_IMF_EVENT_MOUSE_IN
        ECORE_IMF_EVENT_MOUSE_OUT
        ECORE_IMF_EVENT_MOUSE_MOVE
        ECORE_IMF_EVENT_MOUSE_WHEEL
        ECORE_IMF_EVENT_KEY_DOWN
        ECORE_IMF_EVENT_KEY_UP


    ctypedef enum Ecore_IMF_Keyboard_Modifiers:
        ECORE_IMF_KEYBOARD_MODIFIER_NONE  = 0
        ECORE_IMF_KEYBOARD_MODIFIER_CTRL  = 1 << 0
        ECORE_IMF_KEYBOARD_MODIFIER_ALT   = 1 << 1
        ECORE_IMF_KEYBOARD_MODIFIER_SHIFT = 1 << 2
        ECORE_IMF_KEYBOARD_MODIFIER_WIN   = 1 << 3


    ctypedef enum Ecore_IMF_Keyboard_Locks:
        ECORE_IMF_KEYBOARD_LOCK_NONE      = 0
        ECORE_IMF_KEYBOARD_LOCK_NUM       = 1 << 0
        ECORE_IMF_KEYBOARD_LOCK_CAPS      = 1 << 1
        ECORE_IMF_KEYBOARD_LOCK_SCROLL    = 1 << 2


    ctypedef enum Ecore_IMF_Mouse_Flags:
        ECORE_IMF_MOUSE_NONE              = 0
        ECORE_IMF_MOUSE_DOUBLE_CLICK      = 1 << 0
        ECORE_IMF_MOUSE_TRIPLE_CLICK      = 1 << 1


    ctypedef enum Ecore_IMF_Input_Mode:
        ECORE_IMF_INPUT_MODE_ALPHA        = 1 << 0,
        ECORE_IMF_INPUT_MODE_NUMERIC      = 1 << 1,
        ECORE_IMF_INPUT_MODE_SPECIAL      = 1 << 2,
        ECORE_IMF_INPUT_MODE_HEXA         = 1 << 3,
        ECORE_IMF_INPUT_MODE_TELE         = 1 << 4,
        ECORE_IMF_INPUT_MODE_FULL         = (ECORE_IMF_INPUT_MODE_ALPHA | ECORE_IMF_INPUT_MODE_NUMERIC | ECORE_IMF_INPUT_MODE_SPECIAL)
        ECORE_IMF_INPUT_MODE_INVISIBLE    = 1 << 29,
        ECORE_IMF_INPUT_MODE_AUTOCAP      = 1 << 30


    ####################################################################
    # Structures
    #
    ctypedef struct Ecore_IMF_Context
    ctypedef struct Ecore_List


    ctypedef struct Ecore_IMF_Event_Preedit_Start:
        Ecore_IMF_Context *ctx


    ctypedef struct Ecore_IMF_Event_Preedit_End:
        Ecore_IMF_Context *ctx


    ctypedef struct Ecore_IMF_Event_Preedit_Changed:
        Ecore_IMF_Context *ctx


    ctypedef struct Ecore_IMF_Event_Commit:
        Ecore_IMF_Context *ctx


    ctypedef struct Ecore_IMF_Event_Delete_Surrounding:
        Ecore_IMF_Context *ctx
        int offset
        int n_chars


    ctypedef struct _Ecore_IMF_Point:
        int x
        int y


    ctypedef struct Ecore_IMF_Event_Mouse_Down:
        int button
        _Ecore_IMF_Point output
        _Ecore_IMF_Point canvas
        Ecore_IMF_Keyboard_Modifiers modifiers
        Ecore_IMF_Keyboard_Locks locks
        Ecore_IMF_Mouse_Flags flags
        unsigned int timestamp


    ctypedef struct Ecore_IMF_Event_Mouse_Up:
        int button
        _Ecore_IMF_Point output
        _Ecore_IMF_Point canvas
        Ecore_IMF_Keyboard_Modifiers modifiers
        Ecore_IMF_Keyboard_Locks locks
        Ecore_IMF_Mouse_Flags flags
        unsigned int timestamp


    ctypedef struct Ecore_IMF_Event_Mouse_In:
        int buttons
        _Ecore_IMF_Point output
        _Ecore_IMF_Point canvas
        Ecore_IMF_Keyboard_Modifiers modifiers
        Ecore_IMF_Keyboard_Locks locks
        unsigned int timestamp


    ctypedef struct Ecore_IMF_Event_Mouse_Out:
        int buttons
        _Ecore_IMF_Point output
        _Ecore_IMF_Point canvas
        Ecore_IMF_Keyboard_Modifiers modifiers
        Ecore_IMF_Keyboard_Locks locks
        unsigned int timestamp


    ctypedef struct _Ecore_IMF_Coordinate:
        _Ecore_IMF_Point output
        _Ecore_IMF_Point canvas


    ctypedef struct Ecore_IMF_Event_Mouse_Move:
        int buttons
        _Ecore_IMF_Coordinate cur
        _Ecore_IMF_Coordinate prev
        Ecore_IMF_Keyboard_Modifiers modifiers
        Ecore_IMF_Keyboard_Locks locks
        unsigned int timestamp


    ctypedef struct Ecore_IMF_Event_Mouse_Wheel:
        int direction
        int z
        _Ecore_IMF_Point output
        _Ecore_IMF_Point canvas
        Ecore_IMF_Keyboard_Modifiers modifiers
        Ecore_IMF_Keyboard_Locks locks
        unsigned int timestamp


    ctypedef struct Ecore_IMF_Event_Key_Down:
        char *keyname
        Ecore_IMF_Keyboard_Modifiers modifiers
        Ecore_IMF_Keyboard_Locks locks
        char *key
        char *string
        char *compose
        unsigned int timestamp


    ctypedef struct Ecore_IMF_Event_Key_Up:
        char *keyname
        Ecore_IMF_Keyboard_Modifiers modifiers
        Ecore_IMF_Keyboard_Locks locks
        char *key
        char *string
        char *compose
        unsigned int timestamp


    ctypedef union Ecore_IMF_Event:
        Ecore_IMF_Event_Mouse_Down mouse_down
        Ecore_IMF_Event_Mouse_Up mouse_up
        Ecore_IMF_Event_Mouse_In mouse_in
        Ecore_IMF_Event_Mouse_Out mouse_out
        Ecore_IMF_Event_Mouse_Move mouse_move
        Ecore_IMF_Event_Mouse_Wheel mouse_wheel
        Ecore_IMF_Event_Key_Down key_down
        Ecore_IMF_Event_Key_Up key_up


    ctypedef struct Ecore_IMF_Context_Class:
        void (*add)                 (Ecore_IMF_Context *ctx)
        void (*delete)              (Ecore_IMF_Context *ctx)
        void (*client_window_set)   (Ecore_IMF_Context *ctx, void *window)
        void (*client_canvas_set)   (Ecore_IMF_Context *ctx, void *canvas)
        void (*show)                (Ecore_IMF_Context *ctx)
        void (*hide)                (Ecore_IMF_Context *ctx)
        void (*preedit_string_get)  (Ecore_IMF_Context *ctx, char **str, int *cursor_pos)
        void (*focus_in)            (Ecore_IMF_Context *ctx)
        void (*focus_out)           (Ecore_IMF_Context *ctx)
        void (*reset)               (Ecore_IMF_Context *ctx)
        void (*cursor_position_set) (Ecore_IMF_Context *ctx, int cursor_pos)
        void (*use_preedit_set)     (Ecore_IMF_Context *ctx, int use_preedit)
        void (*input_mode_set)      (Ecore_IMF_Context *ctx, Ecore_IMF_Input_Mode input_mode)
        int  (*filter_event)        (Ecore_IMF_Context *ctx, Ecore_IMF_Event_Type type, Ecore_IMF_Event *event)


    ctypedef struct Ecore_IMF_Context_Info:
        char *id
        char *description
        char *default_locales
        char *canvas_type
        int  canvas_required


    ####################################################################
    # Functions
    #
    int  ecore_imf_init()
    int  ecore_imf_shutdown()

    Ecore_List *ecore_imf_context_available_ids_get()
    Ecore_List *ecore_imf_context_available_ids_by_canvas_type_get(char *canvas_type)
    char *ecore_imf_context_default_id_get()
    char *ecore_imf_context_default_id_by_canvas_type_get(char *canvas_type)
    Ecore_IMF_Context_Info *ecore_imf_context_info_by_id_get(char *id)
#
#    Ecore_IMF_Context      *ecore_imf_context_add(char *id)
#    Ecore_IMF_Context_Info *ecore_imf_context_info_get(Ecore_IMF_Context *ctx)
#
#    void ecore_imf_context_del(Ecore_IMF_Context *ctx)
#    void ecore_imf_context_client_window_set(Ecore_IMF_Context *ctx, void *window)
#    void ecore_imf_context_client_canvas_set(Ecore_IMF_Context *ctx, void *canvas)
#    void ecore_imf_context_show(Ecore_IMF_Context *ctx)
#    void ecore_imf_context_hide(Ecore_IMF_Context *ctx)
#    void ecore_imf_context_preedit_string_get(Ecore_IMF_Context *ctx, char **str, int *cursor_pos)
#    void ecore_imf_context_focus_in(Ecore_IMF_Context *ctx)
#    void ecore_imf_context_focus_out(Ecore_IMF_Context *ctx)
#    void ecore_imf_context_reset(Ecore_IMF_Context *ctx)
#    void ecore_imf_context_cursor_position_set(Ecore_IMF_Context *ctx, int cursor_pos)
#    void ecore_imf_context_use_preedit_set(Ecore_IMF_Context *ctx, int use_preedit)
#    void ecore_imf_context_retrieve_surrounding_callback_set(Ecore_IMF_Context *ctx, int (*func)(void *data, Ecore_IMF_Context *ctx, char **text, int *cursor_pos), void *data)
#    void ecore_imf_context_input_mode_set(Ecore_IMF_Context *ctx, Ecore_IMF_Input_Mode input_mode)
#    Ecore_IMF_Input_Mode ecore_imf_context_input_mode_get(Ecore_IMF_Context *ctx)
#    int  ecore_imf_context_filter_event(Ecore_IMF_Context *ctx, Ecore_IMF_Event_Type type, Ecore_IMF_Event *event)
#
#    Ecore_IMF_Context *ecore_imf_context_new(Ecore_IMF_Context_Class *ctxc)
#    void  ecore_imf_context_data_set(Ecore_IMF_Context *ctx, void *data)
#    void *ecore_imf_context_data_get(Ecore_IMF_Context *ctx)
#    int   ecore_imf_context_surrounding_get(Ecore_IMF_Context *ctx, char **text, int *cursor_pos)
#    void  ecore_imf_context_preedit_start_event_add(Ecore_IMF_Context *ctx)
#    void  ecore_imf_context_preedit_end_event_add(Ecore_IMF_Context *ctx)
#    void  ecore_imf_context_preedit_changed_event_add(Ecore_IMF_Context *ctx)
#    void  ecore_imf_context_commit_event_add(Ecore_IMF_Context *ctx, char *str)
#    void  ecore_imf_context_delete_surrounding_event_add(Ecore_IMF_Context *ctx, int offset, int n_chars)
#
#
#cdef class EcoreIMFContext:
#    cdef Ecore_IMF_Context_Info *info
#    cdef readonly char *id
#    cdef readonly char *description
#    cdef readonly char *default_locales
#    cdef readonly char *canvas_type
#
