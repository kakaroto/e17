# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Ulisses Furquim
#
# This file is part of Python-Edje.
#
# Python-Edje is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Edje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Edje.  If not, see <http://www.gnu.org/licenses/>.

cimport evas.c_evas
import evas.c_evas

cdef extern from "Edje.h":
    ####################################################################
    # Enumerations
    #
    ctypedef enum Edje_Message_Type:
        EDJE_MESSAGE_NONE = 0
        EDJE_MESSAGE_SIGNAL = 1
        EDJE_MESSAGE_STRING = 2
        EDJE_MESSAGE_INT = 3
        EDJE_MESSAGE_FLOAT = 4
        EDJE_MESSAGE_STRING_SET = 5
        EDJE_MESSAGE_INT_SET = 6
        EDJE_MESSAGE_FLOAT_SET = 7
        EDJE_MESSAGE_STRING_INT = 8
        EDJE_MESSAGE_STRING_FLOAT = 9
        EDJE_MESSAGE_STRING_INT_SET = 10
        EDJE_MESSAGE_STRING_FLOAT_SET = 11

    ctypedef enum Edje_Aspect_Control:
        EDJE_ASPECT_CONTROL_NONE = 0
        EDJE_ASPECT_CONTROL_NEITHER = 1
        EDJE_ASPECT_CONTROL_HORIZONTAL = 2
        EDJE_ASPECT_CONTROL_VERTICAL = 3
        EDJE_ASPECT_CONTROL_BOTH = 4

    ctypedef enum Edje_Drag_Dir:
        EDJE_DRAG_DIR_NONE = 0
        EDJE_DRAG_DIR_X = 1
        EDJE_DRAG_DIR_Y = 2
        EDJE_DRAG_DIR_XY = 3

    ctypedef enum Edje_Load_Error:
        EDJE_LOAD_ERROR_NONE = 0
        EDJE_LOAD_ERROR_GENERIC = 1
        EDJE_LOAD_ERROR_DOES_NOT_EXIST = 2
        EDJE_LOAD_ERROR_PERMISSION_DENIED = 3
        EDJE_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED = 4
        EDJE_LOAD_ERROR_CORRUPT_FILE = 5
        EDJE_LOAD_ERROR_UNKNOWN_FORMAT = 6
        EDJE_LOAD_ERROR_INCOMPATIBLE_FILE = 7
        EDJE_LOAD_ERROR_UNKNOWN_COLLECTION = 8

    ctypedef enum Edje_Part_Type:
        EDJE_PART_TYPE_NONE      = 0
        EDJE_PART_TYPE_RECTANGLE = 1
        EDJE_PART_TYPE_TEXT      = 2
        EDJE_PART_TYPE_IMAGE     = 3
        EDJE_PART_TYPE_SWALLOW   = 4
        EDJE_PART_TYPE_TEXTBLOCK = 5
        EDJE_PART_TYPE_GRADIENT  = 6
        EDJE_PART_TYPE_GROUP     = 7
        EDJE_PART_TYPE_BOX       = 8
        EDJE_PART_TYPE_TABLE     = 9
        EDJE_PART_TYPE_EXTERNAL  = 10
        EDJE_PART_TYPE_LAST      = 11

    ctypedef enum Edje_Text_Effect:
       EDJE_TEXT_EFFECT_NONE                = 0
       EDJE_TEXT_EFFECT_PLAIN               = 1
       EDJE_TEXT_EFFECT_OUTLINE             = 2
       EDJE_TEXT_EFFECT_SOFT_OUTLINE        = 3
       EDJE_TEXT_EFFECT_SHADOW              = 4
       EDJE_TEXT_EFFECT_SOFT_SHADOW         = 5
       EDJE_TEXT_EFFECT_OUTLINE_SHADOW      = 6
       EDJE_TEXT_EFFECT_OUTLINE_SOFT_SHADOW = 7
       EDJE_TEXT_EFFECT_FAR_SHADOW          = 8
       EDJE_TEXT_EFFECT_FAR_SOFT_SHADOW     = 9
       EDJE_TEXT_EFFECT_GLOW                = 10
       EDJE_TEXT_EFFECT_LAST                = 11

    ctypedef enum Edje_Action_Type:
       EDJE_ACTION_TYPE_NONE          = 0
       EDJE_ACTION_TYPE_STATE_SET     = 1
       EDJE_ACTION_TYPE_ACTION_STOP   = 2
       EDJE_ACTION_TYPE_SIGNAL_EMIT   = 3
       EDJE_ACTION_TYPE_DRAG_VAL_SET  = 4
       EDJE_ACTION_TYPE_DRAG_VAL_STEP = 5
       EDJE_ACTION_TYPE_DRAG_VAL_PAGE = 6
       EDJE_ACTION_TYPE_SCRIPT        = 7
       EDJE_ACTION_TYPE_FOCUS_SET     = 8
       EDJE_ACTION_TYPE_LUA_SCRIPT    = 9
       EDJE_ACTION_TYPE_LAST          = 10

    ctypedef enum Edje_Tween_Mode:
       EDJE_TWEEN_MODE_NONE       = 0
       EDJE_TWEEN_MODE_LINEAR     = 1
       EDJE_TWEEN_MODE_SINUSOIDAL = 2
       EDJE_TWEEN_MODE_ACCELERATE = 3
       EDJE_TWEEN_MODE_DECELERATE = 4
       EDJE_TWEEN_MODE_LAST       = 5

    ctypedef enum Edje_External_Param_Type:
       EDJE_EXTERNAL_PARAM_TYPE_INT    = 0
       EDJE_EXTERNAL_PARAM_TYPE_DOUBLE = 1
       EDJE_EXTERNAL_PARAM_TYPE_STRING = 2
       EDJE_EXTERNAL_PARAM_TYPE_BOOL   = 3
       EDJE_EXTERNAL_PARAM_TYPE_CHOICE = 4
       EDJE_EXTERNAL_PARAM_TYPE_MAX    = 5

    cdef int EDJE_EXTERNAL_INT_UNSET
    cdef double EDJE_EXTERNAL_DOUBLE_UNSET
    cdef unsigned int EDJE_EXTERNAL_TYPE_ABI_VERSION

    ####################################################################
    # Structures
    #
    ctypedef struct Edje_Message_String:
        char *str

    ctypedef struct Edje_Message_Int:
        int val

    ctypedef struct Edje_Message_Float:
        double val

    ctypedef struct Edje_Message_String_Set:
        int count
        char *str[1]

    ctypedef struct Edje_Message_Int_Set:
        int count
        int val[1]

    ctypedef struct Edje_Message_Float_Set:
        int count
        double val[1]

    ctypedef struct Edje_Message_String_Int:
        char *str
        int val

    ctypedef struct Edje_Message_String_Float:
        char *str
        double val

    ctypedef struct Edje_Message_String_Int_Set:
        char *str
        int count
        int val[1]

    ctypedef struct Edje_Message_String_Float_Set:
        char *str
        int count
        double val[1]

    ctypedef struct Edje_External_Param:
        char *name
        Edje_External_Param_Type type
        int i
        double d
        char *s

    ctypedef struct aux_external_param_info_int:
        int default "def", min, max, step

    ctypedef struct aux_external_param_info_double:
        double default "def", min, max, step

    ctypedef struct aux_external_param_info_string:
        char *default "def", *accept_fmt, *deny_fmt

    ctypedef struct aux_external_param_info_bool:
        char *default "def", *false_str, *true_str

    ctypedef struct aux_external_param_info_choice:
        char *default "def", **choices

    ctypedef union aux_external_param_info:
        aux_external_param_info_int i
        aux_external_param_info_double d
        aux_external_param_info_string s
        aux_external_param_info_bool b
        aux_external_param_info_choice c

    ctypedef struct Edje_External_Param_Info:
        char *name
        Edje_External_Param_Type type
        aux_external_param_info info

    ctypedef struct Edje_External_Type:
        unsigned int abi_version
        char *module
        char *module_name
        evas.c_evas.Evas_Object *(*add)(void *data, evas.c_evas.Evas *evas, evas.c_evas.Evas_Object *parent, evas.c_evas.Eina_List *params,  char *part_name)
        void (*state_set)(void *data, evas.c_evas.Evas_Object *obj, void *from_params, void *to_params, float pos)
        void (*signal_emit)(void *data, evas.c_evas.Evas_Object *obj, char *emission, char *source)
        evas.c_evas.Eina_Bool (*param_set)(void *data, evas.c_evas.Evas_Object *obj, Edje_External_Param *param)
        evas.c_evas.Eina_Bool (*param_get)(void *data, evas.c_evas.Evas_Object *obj, Edje_External_Param *param)
        void *(*params_parse)(void *data, evas.c_evas.Evas_Object *obj, evas.c_evas.Eina_List *params)
        void (*params_free)(void *params)
        evas.c_evas.Evas_Object *(*icon_add)(void *data, evas.c_evas.Evas *e)
        evas.c_evas.Evas_Object *(*preview_add)(void *data, evas.c_evas.Evas *e)
        char *(*label_get)(void *data)
        char *(*description_get)(void *data)
        char *(*translate)(void *data, char *orig)
        Edje_External_Param_Info *parameters_info
        void *data

    ####################################################################
    # Engine
    #
    int edje_init()
    int edje_shutdown()

    void edje_frametime_set(double t)
    double edje_frametime_get()

    void edje_freeze()
    void edje_thaw()
    void edje_fontset_append_set(char *fonts)
    char *edje_fontset_append_get()

    evas.c_evas.Eina_List *edje_file_collection_list(char *file)
    void edje_file_collection_list_free(evas.c_evas.Eina_List *lst)
    int edje_file_group_exists(char *file, char *glob)
    char *edje_file_data_get(char *file, char *key)
    void edje_file_cache_set(int count)
    int edje_file_cache_get()
    void edje_file_cache_flush()
    void edje_collection_cache_set(int count)
    int edje_collection_cache_get()
    void edje_collection_cache_flush()

    void edje_color_class_set(char *color_class, int r, int g, int b, int a, int r2, int g2, int b2, int a2, int r3, int g3, int b3, int a3)
    void edje_color_class_del(char *color_class)
    evas.c_evas.Eina_List * edje_color_class_list()
    void edje_text_class_set(char *text_class, char *font, evas.c_evas.Evas_Font_Size size)
    void edje_text_class_del(char *text_class)
    evas.c_evas.Eina_List * edje_text_class_list()
    void edje_extern_object_min_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord minw, evas.c_evas.Evas_Coord minh)
    void edje_extern_object_max_size_set(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord maxw, evas.c_evas.Evas_Coord maxh)
    void edje_extern_object_aspect_set(evas.c_evas.Evas_Object *obj, Edje_Aspect_Control aspect, evas.c_evas.Evas_Coord aw, evas.c_evas.Evas_Coord ah)

    evas.c_evas.Evas_Object *edje_object_add(evas.c_evas.Evas *)

    char *edje_object_data_get(evas.c_evas.Evas_Object *obj, char *key)

    int edje_object_file_set(evas.c_evas.Evas_Object *obj, char *file, char *part)
    void edje_object_file_get(evas.c_evas.Evas_Object *obj, char **file, char **part)
    int edje_object_load_error_get(evas.c_evas.Evas_Object *obj)

    void edje_object_signal_callback_add(evas.c_evas.Evas_Object *obj, char *emission, char *source, void(*func)(void *data, evas.c_evas.Evas_Object *obj, char *emission, char *source), void *data)
    void *edje_object_signal_callback_del(evas.c_evas.Evas_Object *obj, char *emission, char *source, void(*func)(void *data, evas.c_evas.Evas_Object *obj, char *emission, char *source))
    void edje_object_signal_emit(evas.c_evas.Evas_Object *obj, char *emission, char *source)

    void edje_object_play_set(evas.c_evas.Evas_Object *obj, int play)
    int edje_object_play_get(evas.c_evas.Evas_Object *obj)
    void edje_object_animation_set(evas.c_evas.Evas_Object *obj, int on)
    int edje_object_animation_get(evas.c_evas.Evas_Object *obj)

    int edje_object_freeze(evas.c_evas.Evas_Object *obj)
    int edje_object_thaw(evas.c_evas.Evas_Object *obj)

    void edje_object_color_class_set(evas.c_evas.Evas_Object *obj, char *color_class, int r, int g, int b, int a, int r2, int g2, int b2, int a2, int r3, int g3, int b3, int a3)
    void edje_object_color_class_del(evas.c_evas.Evas_Object *obj, char *color_class)
    void edje_object_text_class_set(evas.c_evas.Evas_Object *obj, char *text_class, char *font, evas.c_evas.Evas_Font_Size size)

    void edje_object_size_min_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *minw, evas.c_evas.Evas_Coord *minh)
    void edje_object_size_max_get(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *maxw, evas.c_evas.Evas_Coord *maxh)
    void edje_object_calc_force(evas.c_evas.Evas_Object *obj)
    void edje_object_size_min_calc(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *minw, evas.c_evas.Evas_Coord *minh)
    evas.c_evas.Eina_Bool edje_object_parts_extends_calc(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Coord *x, evas.c_evas.Evas_Coord *y, evas.c_evas.Evas_Coord *w, evas.c_evas.Evas_Coord *h)

    int edje_object_part_exists(evas.c_evas.Evas_Object *obj, char *part)
    evas.c_evas.Evas_Object *edje_object_part_object_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_object_part_geometry_get(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Evas_Coord *x, evas.c_evas.Evas_Coord *y, evas.c_evas.Evas_Coord *w, evas.c_evas.Evas_Coord *h)

    void edje_object_text_change_cb_set(evas.c_evas.Evas_Object *obj, void(*func)(void *data, evas.c_evas.Evas_Object *obj, char *part), void *data)
    void edje_object_part_text_set(evas.c_evas.Evas_Object *obj, char *part, char *text)
    char *edje_object_part_text_get(evas.c_evas.Evas_Object *obj, char *part)

    void edje_object_part_text_select_all(evas.c_evas.Evas_Object *obj, char *part)
    void edje_object_part_text_select_none(evas.c_evas.Evas_Object *obj, char *part)

    void edje_object_part_text_unescaped_set(evas.c_evas.Evas_Object *obj, char *part, char *text_to_escape)
    char *edje_object_part_text_unescaped_get(evas.c_evas.Evas_Object *obj, char *part)


    void edje_object_part_swallow(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Evas_Object *obj_swallow)
    void edje_object_part_unswallow(evas.c_evas.Evas_Object *obj, evas.c_evas.Evas_Object *obj_swallow)
    evas.c_evas.Evas_Object *edje_object_part_swallow_get(evas.c_evas.Evas_Object *obj, char *part)

    evas.c_evas.Eina_Bool edje_object_part_box_append(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Evas_Object *child)
    evas.c_evas.Eina_Bool edje_object_part_box_prepend(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Evas_Object *child)
    evas.c_evas.Eina_Bool edje_object_part_box_insert_at(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Evas_Object *child, unsigned int pos)
    evas.c_evas.Eina_Bool edje_object_part_box_insert_before(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Evas_Object *child, evas.c_evas.Evas_Object *reference)
    evas.c_evas.Evas_Object *edje_object_part_box_remove(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Evas_Object *child)
    evas.c_evas.Evas_Object *edje_object_part_box_remove_at(evas.c_evas.Evas_Object *obj, char *part, unsigned int pos)
    evas.c_evas.Eina_Bool edje_object_part_box_remove_all(evas.c_evas.Evas_Object *obj, char *part, int clear)

    evas.c_evas.Eina_Bool edje_object_part_table_pack(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Evas_Object *child, unsigned short col, unsigned short row, unsigned short colspan, unsigned short rowspan)
    evas.c_evas.Eina_Bool edje_object_part_table_unpack(evas.c_evas.Evas_Object *obj, char *part, evas.c_evas.Evas_Object *child)
    evas.c_evas.Eina_Bool edje_object_part_table_col_row_size_get(evas.c_evas.Evas_Object *obj, char *part, int *cols, int *rows)
    evas.c_evas.Eina_Bool edje_object_part_table_clear(evas.c_evas.Evas_Object *obj, char *part, int clear)

    char *edje_object_part_state_get(evas.c_evas.Evas_Object *obj, char *part, double *val_ret)

    int edje_object_part_drag_dir_get(evas.c_evas.Evas_Object *obj, char *part)
    void edje_object_part_drag_value_set(evas.c_evas.Evas_Object *obj, char *part, double dx, double dy)
    void edje_object_part_drag_value_get(evas.c_evas.Evas_Object *obj, char *part, double *dx, double *dy)
    void edje_object_part_drag_size_set(evas.c_evas.Evas_Object *obj, char *part, double dw, double dh)
    void edje_object_part_drag_size_get(evas.c_evas.Evas_Object *obj, char *part, double *dw, double *dh)
    void edje_object_part_drag_step_set(evas.c_evas.Evas_Object *obj, char *part, double dx, double dy)
    void edje_object_part_drag_step_get(evas.c_evas.Evas_Object *obj, char *part, double *dx, double *dy)
    void edje_object_part_drag_page_set(evas.c_evas.Evas_Object *obj, char *part, double dx, double dy)
    void edje_object_part_drag_page_get(evas.c_evas.Evas_Object *obj, char *part, double *dx, double *dy)
    void edje_object_part_drag_step(evas.c_evas.Evas_Object *obj, char *part, double dx, double dy)
    void edje_object_part_drag_page(evas.c_evas.Evas_Object *obj, char *part, double dx, double dy)

    evas.c_evas.Evas_Object *edje_object_part_external_object_get(evas.c_evas.Evas_Object *obj, char *part)
    evas.c_evas.Eina_Bool edje_object_part_external_param_set(evas.c_evas.Evas_Object *obj, char *part, Edje_External_Param *param)
    evas.c_evas.Eina_Bool edje_object_part_external_param_get(evas.c_evas.Evas_Object *obj, char *part, Edje_External_Param *param)
    Edje_External_Param_Type edje_object_part_external_param_type_get(evas.c_evas.Evas_Object *obj, char *part, char *param)

    char *edje_external_param_type_str(Edje_External_Param_Type type)

    void edje_object_message_send(evas.c_evas.Evas_Object *obj, Edje_Message_Type type, int id, void *msg)
    void edje_object_message_handler_set(evas.c_evas.Evas_Object *obj, void(*func)(void *data, evas.c_evas.Evas_Object *obj, Edje_Message_Type type, int id, void *msg), void *data)
    void edje_object_message_signal_process(evas.c_evas.Evas_Object *obj)

    void edje_message_signal_process()

    unsigned int edje_external_type_abi_version_get()

    evas.c_evas.Eina_Iterator *edje_external_iterator_get()
    Edje_External_Param_Info *edje_external_param_info_get(char *type_name)
    Edje_External_Type *edje_external_type_get(char *type_name)

    evas.c_evas.Eina_Bool edje_module_load(char *name)
    evas.c_evas.Eina_List *edje_available_modules_get()


cdef class Message:
    cdef int _type
    cdef int _id


cdef class MessageSignal(Message):
    pass


cdef class MessageString(Message):
    cdef Edje_Message_String *obj


cdef class MessageInt(Message):
    cdef Edje_Message_Int *obj


cdef class MessageFloat(Message):
    cdef Edje_Message_Float *obj


cdef class MessageStringSet(Message):
    cdef Edje_Message_String_Set *obj


cdef class MessageIntSet(Message):
    cdef Edje_Message_Int_Set *obj


cdef class MessageFloatSet(Message):
    cdef Edje_Message_Float_Set *obj


cdef class MessageStringInt(Message):
    cdef Edje_Message_String_Int *obj


cdef class MessageStringFloat(Message):
    cdef Edje_Message_String_Float *obj


cdef class MessageStringIntSet(Message):
    cdef Edje_Message_String_Int_Set *obj


cdef class MessageStringFloatSet(Message):
    cdef Edje_Message_String_Float_Set *obj


cdef class ExternalParam:
    cdef Edje_External_Param *obj


cdef class ExternalParamInfo:
    cdef Edje_External_Param_Info *obj
    cdef readonly object external_type
    cdef Edje_External_Type *_external_type_obj
    cdef _set_external_type(self, t)


cdef class ExternalParamInfoInt(ExternalParamInfo):
    pass


cdef class ExternalParamInfoDouble(ExternalParamInfo):
    pass


cdef class ExternalParamInfoString(ExternalParamInfo):
    pass


cdef class ExternalParamInfoBool(ExternalParamInfo):
    pass


cdef class ExternalParamInfoChoice(ExternalParamInfo):
    pass


cdef class ExternalType:
    cdef object _name
    cdef object _parameters_info
    cdef Edje_External_Type *_obj


cdef public class Edje(evas.c_evas.Object) [object PyEdje, type PyEdje_Type]:
    cdef object _text_change_cb
    cdef object _message_handler_cb
    cdef object _signal_callbacks

    cdef void message_send_int(self, int id, int data)
    cdef void message_send_float(self, int id, float data)
    cdef void message_send_str(self, int id, char *data)
    cdef void message_send_str_set(self, int id, data)
    cdef void message_send_str_int(self, int id, char *s, int i)
    cdef void message_send_str_float(self, int id, char *s, float f)
    cdef void message_send_str_int_set(self, int id, char *s, data)
    cdef void message_send_str_float_set(self, int id, char *s, data)
    cdef void message_send_int_set(self, int id, data)
    cdef void message_send_float_set(self, int id, data)
    cdef message_send_set(self, int id, data)
