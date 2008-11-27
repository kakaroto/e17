# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Caio Marcelo de Oliveira Filho, Ulisses Furquim
#
# This file is part of Python-Evas.
#
# Python-Evas is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Evas is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Evas.  If not, see <http://www.gnu.org/licenses/>.

cdef extern from "evas/python_evas_utils.h":
    int PY_REFCOUNT(object)


cdef extern from "Evas.h":
    ####################################################################
    # Enumerations
    #
    ctypedef enum Evas_Callback_Type:
        EVAS_CALLBACK_MOUSE_IN
        EVAS_CALLBACK_MOUSE_OUT
        EVAS_CALLBACK_MOUSE_DOWN
        EVAS_CALLBACK_MOUSE_UP
        EVAS_CALLBACK_MOUSE_MOVE
        EVAS_CALLBACK_MOUSE_WHEEL
        EVAS_CALLBACK_FREE
        EVAS_CALLBACK_KEY_DOWN
        EVAS_CALLBACK_KEY_UP
        EVAS_CALLBACK_FOCUS_IN
        EVAS_CALLBACK_FOCUS_OUT
        EVAS_CALLBACK_SHOW
        EVAS_CALLBACK_HIDE
        EVAS_CALLBACK_MOVE
        EVAS_CALLBACK_RESIZE
        EVAS_CALLBACK_RESTACK
        EVAS_CALLBACK_DEL
        EVAS_CALLBACK_HOLD
        EVAS_CALLBACK_CHANGED_SIZE_HINTS
        EVAS_CALLBACK_IMAGE_PRELOADED


    cdef enum Evas_Load_Error:
        EVAS_LOAD_ERROR_NONE = 0
        EVAS_LOAD_ERROR_GENERIC = 1
        EVAS_LOAD_ERROR_DOES_NOT_EXIST = 2
        EVAS_LOAD_ERROR_PERMISSION_DENIED = 3
        EVAS_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED = 4
        EVAS_LOAD_ERROR_CORRUPT_FILE = 5
        EVAS_LOAD_ERROR_UNKNOWN_FORMAT = 6


    ctypedef enum Evas_Button_Flags:
        EVAS_BUTTON_NONE
        EVAS_BUTTON_DOUBLE_CLICK
        EVAS_BUTTON_TRIPLE_CLICK


    ctypedef enum Evas_Event_Flags:
        EVAS_EVENT_FLAG_NONE
        EVAS_EVENT_FLAG_ON_HOLD


    ctypedef enum Evas_Font_Hinting_Flags:
        EVAS_FONT_HINTING_NONE
        EVAS_FONT_HINTING_AUTO
        EVAS_FONT_HINTING_BYTECODE


    ctypedef enum Evas_Colorspace:
        EVAS_COLORSPACE_ARGB8888
        EVAS_COLORSPACE_YCBCR422P601_PL
        EVAS_COLORSPACE_YCBCR422P709_PL
        EVAS_COLORSPACE_RGB565_A5P


    ctypedef enum Evas_Render_Op:
        EVAS_RENDER_BLEND
        EVAS_RENDER_BLEND_REL
        EVAS_RENDER_COPY
        EVAS_RENDER_COPY_REL
        EVAS_RENDER_ADD
        EVAS_RENDER_ADD_REL
        EVAS_RENDER_SUB
        EVAS_RENDER_SUB_REL
        EVAS_RENDER_TINT
        EVAS_RENDER_TINT_REL
        EVAS_RENDER_MASK
        EVAS_RENDER_MUL


    ctypedef enum Evas_Text_Style_Type:
        EVAS_TEXT_STYLE_PLAIN
        EVAS_TEXT_STYLE_SHADOW
        EVAS_TEXT_STYLE_OUTLINE
        EVAS_TEXT_STYLE_SOFT_OUTLINE
        EVAS_TEXT_STYLE_GLOW
        EVAS_TEXT_STYLE_OUTLINE_SHADOW
        EVAS_TEXT_STYLE_FAR_SHADOW
        EVAS_TEXT_STYLE_OUTLINE_SOFT_SHADOW
        EVAS_TEXT_STYLE_SOFT_SHADOW
        EVAS_TEXT_STYLE_FAR_SOFT_SHADOW


    ctypedef enum Evas_Textblock_Text_Type:
        EVAS_TEXTBLOCK_TEXT_RAW
        EVAS_TEXTBLOCK_TEXT_PLAIN

    ctypedef enum Evas_Smart_Class_Version:
        EVAS_SMART_CLASS_VERSION


    ctypedef enum Evas_Object_Pointer_Mode:
        EVAS_OBJECT_POINTER_MODE_AUTOGRAB
        EVAS_OBJECT_POINTER_MODE_NOGRAB


    ####################################################################
    # Structures
    #
    ctypedef struct Eina_List:
        void      *data
        Eina_List *next
        Eina_List *prev
        void      *accounting


    ctypedef struct Evas_Rectangle:
        int x
        int y
        int w
        int h

    ctypedef struct Evas_Hash
    ctypedef struct Evas
    ctypedef struct Evas_Object
    ctypedef struct Evas_Modifier
    ctypedef struct Evas_Lock
    ctypedef struct Evas_Smart
    ctypedef struct Evas_Native_Surface

    ctypedef struct Evas_Smart_Class:
        char *name
        int version
        void (*add)(Evas_Object *o)
        void (*delete "del")(Evas_Object *o)
        void (*move)(Evas_Object *o, int x, int y)
        void (*resize)(Evas_Object *o, int w, int h)
        void (*show)(Evas_Object *o)
        void (*hide)(Evas_Object *o)
        void (*color_set)(Evas_Object *o, int r, int g, int b, int a)
        void (*clip_set)(Evas_Object *o, Evas_Object *clip)
        void (*clip_unset)(Evas_Object *o)
        void (*calculate)(Evas_Object *o)
        void (*member_add)(Evas_Object *o, Evas_Object *child)
        void (*member_del)(Evas_Object *o, Evas_Object *child)
        void *data


    ctypedef struct Evas_Point:
        int x
        int y

    ctypedef struct Evas_Position:
        Evas_Point output
        Evas_Point canvas

    ctypedef struct Evas_Event_Mouse_In:
        int buttons
        Evas_Point output
        Evas_Point canvas
        void *data
        Evas_Modifier *modifiers
        Evas_Lock *locks
        unsigned int timestamp
        Evas_Event_Flags event_flags

    ctypedef struct Evas_Event_Mouse_Out:
        int buttons
        Evas_Point output
        Evas_Point canvas
        void *data
        Evas_Modifier *modifiers
        Evas_Lock *locks
        unsigned int timestamp
        Evas_Event_Flags event_flags

    ctypedef struct Evas_Event_Mouse_Down:
        int button
        Evas_Point output
        Evas_Point canvas
        void *data
        Evas_Modifier *modifiers
        Evas_Lock *locks
        Evas_Button_Flags flags
        unsigned int timestamp
        Evas_Event_Flags event_flags

    ctypedef struct Evas_Event_Mouse_Up:
        int button
        Evas_Point output
        Evas_Point canvas
        void *data
        Evas_Modifier *modifiers
        Evas_Lock *locks
        Evas_Button_Flags flags
        unsigned int timestamp
        Evas_Event_Flags event_flags

    ctypedef struct Evas_Event_Mouse_Move:
        int buttons
        Evas_Position cur
        Evas_Position prev
        void *data
        Evas_Modifier *modifiers
        Evas_Lock *locks
        unsigned int timestamp
        Evas_Event_Flags event_flags

    ctypedef struct Evas_Event_Mouse_Wheel:
        int direction # 0 = default up/down wheel
        int z         # ...,-2,-1 = down, 1,2,... = up */
        Evas_Point output
        Evas_Point canvas
        void *data
        Evas_Modifier *modifiers
        Evas_Lock *locks
        unsigned int timestamp
        Evas_Event_Flags event_flags

    ctypedef struct Evas_Event_Key_Down:
        char *keyname
        void *data
        Evas_Modifier *modifiers
        Evas_Lock *locks
        char *key
        char *string
        char *compose
        unsigned int timestamp
        Evas_Event_Flags event_flags

    ctypedef struct Evas_Event_Key_Up:
        char *keyname
        void *data
        Evas_Modifier *modifiers
        Evas_Lock *locks
        char *key
        char *string
        char *compose
        unsigned int timestamp
        Evas_Event_Flags event_flags

    ctypedef struct Evas_Event_Hold:
        int hold
        void *data
        unsigned int timestamp
        Evas_Event_Flags event_flags


    ####################################################################
    # Other typedefs
    #
    ctypedef int Evas_Coord
    ctypedef int Evas_Bool
    ctypedef int Evas_Angle
    ctypedef int Evas_Font_Size
    ctypedef void (*evas_event_callback_t)(void *data, Evas *e, Evas_Object *obj, void *event_info)

    ####################################################################
    # Engine
    #
    int evas_init()
    int evas_shutdown()

    Eina_List *eina_list_free(Eina_List *list)
    Eina_List *eina_list_append(Eina_List *list, void *data)


    ####################################################################
    # Canvas
    #
    Evas *evas_new()
    void evas_free(Evas *e)

    int evas_render_method_lookup(char *name)
    Eina_List *evas_render_method_list()
    void evas_render_method_list_free(Eina_List *list)

    void evas_output_method_set(Evas *e, int render_method)
    int evas_output_method_get(Evas *e)

    void evas_output_size_set(Evas *e, int w, int h)
    void evas_output_size_get(Evas *e, int *w, int *h)
    void evas_output_viewport_set(Evas *e, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
    void evas_output_viewport_get(Evas *e, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
    Evas_Coord evas_coord_screen_x_to_world(Evas *e, int x)
    Evas_Coord evas_coord_screen_y_to_world(Evas *e, int y)
    int evas_coord_world_x_to_screen(Evas *e, Evas_Coord x)
    int evas_coord_world_y_to_screen(Evas *e, Evas_Coord y)

    void evas_pointer_output_xy_get(Evas *e, int *x, int *y)
    void evas_pointer_canvas_xy_get(Evas *e, Evas_Coord *x, Evas_Coord *y)
    int evas_pointer_button_down_mask_get(Evas *e)
    Evas_Bool evas_pointer_inside_get(Evas *e)

    Evas_Object *evas_object_top_at_xy_get(Evas *e, Evas_Coord x, Evas_Coord y, Evas_Bool include_pass_events_objects, Evas_Bool include_hidden_objects)
    Evas_Object *evas_object_top_at_pointer_get(Evas *e)
    Evas_Object *evas_object_top_in_rectangle_get(Evas *e, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h, Evas_Bool include_pass_events_objects, Evas_Bool include_hidden_objects)

    Eina_List *evas_objects_at_xy_get(Evas *e, Evas_Coord x, Evas_Coord y, Evas_Bool include_pass_events_objects, Evas_Bool include_hidden_objects)
    Eina_List *evas_objects_in_rectangle_get(Evas *e, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h, Evas_Bool include_pass_events_objects, Evas_Bool include_hidden_objects)

    void evas_damage_rectangle_add(Evas *e, int x, int y, int w, int h)
    void evas_obscured_rectangle_add(Evas *e, int x, int y, int w, int h)
    void evas_obscured_clear(Evas *e)
    Eina_List *evas_render_updates(Evas *e)
    void evas_render_updates_free(Eina_List *updates)
    void evas_render(Evas *e)
    void evas_norender(Evas *e)
    void *evas_data_attach_get(Evas *e)
    void evas_data_attach_set(Evas *e, void *data)

    Evas_Object *evas_focus_get(Evas *e)

    void evas_event_freeze(Evas *e)
    void evas_event_thaw(Evas *e)
    int evas_event_freeze_get(Evas *e)

    void evas_event_feed_hold(Evas *e, int hold, unsigned int timestamp, void *data)

    void evas_font_path_clear(Evas *e)
    void evas_font_path_append(Evas *e, char *path)
    void evas_font_path_prepend(Evas *e, char *path)
    Eina_List  *evas_font_path_list(Evas *e)

    void evas_font_hinting_set(Evas *e, Evas_Font_Hinting_Flags hinting)
    Evas_Font_Hinting_Flags evas_font_hinting_get(Evas *e)
    Evas_Bool evas_font_hinting_can_hint(Evas *e, Evas_Font_Hinting_Flags hinting)

    void evas_font_cache_flush(Evas *e)
    void evas_font_cache_set(Evas *e, int size)
    int evas_font_cache_get(Evas *e)

    Eina_List *evas_font_available_list(Evas *e)
    void evas_font_available_list_free(Evas *e, Eina_List *available)

    void evas_image_cache_flush(Evas *e)
    void evas_image_cache_reload(Evas *e)
    void evas_image_cache_set(Evas *e, int size)
    int evas_image_cache_get(Evas *e)


    ####################################################################
    # Base Object
    #
    void evas_object_del(Evas_Object *obj)
    Evas *evas_object_evas_get(Evas_Object *obj)

    void evas_object_data_set(Evas_Object *obj, char *key, void *data)
    void *evas_object_data_get(Evas_Object *obj, char *key)
    void *evas_object_data_del(Evas_Object *obj, char *key)

    char *evas_object_type_get(Evas_Object *obj)

    void evas_object_layer_set(Evas_Object *obj, int l)
    int evas_object_layer_get(Evas_Object *obj)

    void evas_object_raise(Evas_Object *obj)
    void evas_object_lower(Evas_Object *obj)
    void evas_object_stack_above(Evas_Object *obj, Evas_Object *above)
    void evas_object_stack_below(Evas_Object *obj, Evas_Object *below)
    Evas_Object *evas_object_above_get(Evas_Object *obj)
    Evas_Object *evas_object_below_get(Evas_Object *obj)
    Evas_Object *evas_object_bottom_get(Evas *e)
    Evas_Object *evas_object_top_get(Evas *e)

    void evas_object_move(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
    void evas_object_resize(Evas_Object *obj, Evas_Coord w, Evas_Coord h)
    void evas_object_geometry_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)

    void evas_object_show(Evas_Object *obj)
    void evas_object_hide(Evas_Object *obj)
    Evas_Bool evas_object_visible_get(Evas_Object *obj)

    void evas_object_render_op_set(Evas_Object *obj, Evas_Render_Op op)
    Evas_Render_Op evas_object_render_op_get(Evas_Object *obj)

    void evas_object_anti_alias_set(Evas_Object *obj, Evas_Bool antialias)
    Evas_Bool evas_object_anti_alias_get(Evas_Object *obj)

    void evas_object_color_set(Evas_Object *obj, int r, int g, int b, int a)
    void evas_object_color_get(Evas_Object *obj, int *r, int *g, int *b, int *a)

    void evas_object_color_interpolation_set(Evas_Object *obj, int color_space)
    int evas_object_color_interpolation_get(Evas_Object *obj)

    void evas_color_argb_premul(int a, int *r, int *g, int *b)
    void evas_color_argb_unpremul(int a, int *r, int *g, int *b)

    void evas_color_hsv_to_rgb(float h, float s, float v, int *r, int *g, int *b)
    void evas_color_rgb_to_hsv(int r, int g, int b, float *h, float *s, float *v)

    void evas_object_clip_set(Evas_Object *obj, Evas_Object *clip)
    Evas_Object *evas_object_clip_get(Evas_Object *obj)
    void evas_object_clip_unset(Evas_Object *obj)
    Eina_List *evas_object_clipees_get(Evas_Object *obj)

    void evas_object_name_set(Evas_Object *obj, char *name)
    char *evas_object_name_get(Evas_Object *obj)
    Evas_Object *evas_object_name_find(Evas *e, char *name)

    void evas_object_event_callback_add(Evas_Object *obj, Evas_Callback_Type type, evas_event_callback_t func, void *data)
    void *evas_object_event_callback_del(Evas_Object *obj, Evas_Callback_Type type, evas_event_callback_t func)

    void evas_object_pass_events_set(Evas_Object *obj, Evas_Bool p)
    Evas_Bool evas_object_pass_events_get(Evas_Object *obj)
    void evas_object_repeat_events_set(Evas_Object *obj, Evas_Bool repeat)
    Evas_Bool evas_object_repeat_events_get(Evas_Object *obj)
    void evas_object_propagate_events_set(Evas_Object *obj, Evas_Bool prop)
    Evas_Bool evas_object_propagate_events_get(Evas_Object *obj)
    void evas_object_pointer_mode_set(Evas_Object *obj, Evas_Object_Pointer_Mode setting)
    Evas_Object_Pointer_Mode evas_object_pointer_mode_get(Evas_Object *obj)

    void evas_object_focus_set(Evas_Object *obj, Evas_Bool focus)
    Evas_Bool evas_object_focus_get(Evas_Object *obj)


    ####################################################################
    # Smart Object
    #
    void evas_smart_free(Evas_Smart *s)
    Evas_Smart *evas_smart_class_new(Evas_Smart_Class *sc)
    Evas_Smart_Class *evas_smart_class_get(Evas_Smart *s)

    void *evas_smart_data_get(Evas_Smart *s)

    Evas_Object *evas_object_smart_add(Evas *e, Evas_Smart *s)
    void evas_object_smart_member_add(Evas_Object *obj, Evas_Object *smart_obj)
    void evas_object_smart_member_del(Evas_Object *obj)
    Evas_Object *evas_object_smart_parent_get(Evas_Object *obj)
    Eina_List *evas_object_smart_members_get(Evas_Object *obj)
    Evas_Smart *evas_object_smart_smart_get(Evas_Object *obj)
    void *evas_object_smart_data_get(Evas_Object *obj)
    void evas_object_smart_data_set(Evas_Object *obj, void *data)
    void evas_object_smart_callback_add(Evas_Object *obj, char *event, void (*func) (void *data, Evas_Object *obj, void *event_info), void *data)
    void *evas_object_smart_callback_del(Evas_Object *obj, char *event, void (*func) (void *data, Evas_Object *obj, void *event_info))
    void evas_object_smart_callback_call(Evas_Object *obj, char *event, void *event_info)
    void evas_object_smart_changed(Evas_Object *obj)
    void evas_object_smart_need_recalculate_set(Evas_Object *obj, int value)
    int evas_object_smart_need_recalculate_get(Evas_Object *obj)
    void evas_object_smart_calculate(Evas_Object *obj)



    ####################################################################
    # Rectangle Object
    #
    Evas_Object *evas_object_rectangle_add(Evas *e)



    ####################################################################
    # Line Object
    #
    Evas_Object *evas_object_line_add(Evas *e)
    void evas_object_line_xy_set(Evas_Object *obj, Evas_Coord x1, Evas_Coord y1, Evas_Coord x2, Evas_Coord y2)
    void evas_object_line_xy_get(Evas_Object *obj, Evas_Coord *x1, Evas_Coord *y1, Evas_Coord *x2, Evas_Coord *y2)


    ####################################################################
    # Image Object
    #
    Evas_Object *evas_object_image_add(Evas *e)
    void evas_object_image_file_set(Evas_Object *obj, char *file, char *key)
    void evas_object_image_file_get(Evas_Object *obj, char **file, char **key)
    void evas_object_image_border_set(Evas_Object *obj, int l, int r, int t, int b)
    void evas_object_image_border_get(Evas_Object *obj, int *l, int *r, int *t, int *b)
    void evas_object_image_border_center_fill_set(Evas_Object *obj, Evas_Bool fill)
    Evas_Bool evas_object_image_border_center_fill_get(Evas_Object *obj)
    void evas_object_image_fill_set(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
    void evas_object_image_fill_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
    void evas_object_image_size_set(Evas_Object *obj, int w, int h)
    void evas_object_image_size_get(Evas_Object *obj, int *w, int *h)
    int evas_object_image_stride_get(Evas_Object *obj)
    int evas_object_image_load_error_get(Evas_Object *obj)
    void evas_object_image_data_set(Evas_Object *obj, void *data)
    void *evas_object_image_data_get(Evas_Object *obj, Evas_Bool for_writing)
    void evas_object_image_data_copy_set(Evas_Object *obj, void *data)
    void evas_object_image_data_update_add(Evas_Object *obj, int x, int y, int w, int h)
    void evas_object_image_alpha_set(Evas_Object *obj, Evas_Bool has_alpha)
    Evas_Bool evas_object_image_alpha_get(Evas_Object *obj)
    void evas_object_image_smooth_scale_set(Evas_Object *obj, Evas_Bool smooth_scale)
    Evas_Bool evas_object_image_smooth_scale_get(Evas_Object *obj)
    void evas_object_image_reload(Evas_Object *obj)
    Evas_Bool evas_object_image_save(Evas_Object *obj, char *file, char *key, char *flags)
    #Evas_Bool evas_object_image_pixels_import(Evas_Object *obj, Evas_Pixel_Import_Source *pixels)
    void evas_object_image_pixels_get_callback_set(Evas_Object *obj, void (*func) (void *data, Evas_Object *o), void *data)
    void evas_object_image_pixels_dirty_set(Evas_Object *obj, Evas_Bool dirty)
    Evas_Bool evas_object_image_pixels_dirty_get(Evas_Object *obj)
    void evas_object_image_load_dpi_set(Evas_Object *obj, double dpi)
    double evas_object_image_load_dpi_get(Evas_Object *obj)
    void evas_object_image_load_size_set(Evas_Object *obj, int w, int h)
    void evas_object_image_load_size_get(Evas_Object *obj, int *w, int *h)
    void evas_object_image_load_scale_down_set(Evas_Object *obj, int scale_down)
    int evas_object_image_load_scale_down_get(Evas_Object *obj)
    void evas_object_image_colorspace_set(Evas_Object *obj, Evas_Colorspace cspace)
    Evas_Colorspace evas_object_image_colorspace_get(Evas_Object *obj)
    void evas_object_image_native_surface_set(Evas_Object *obj, Evas_Native_Surface *surf)
    Evas_Native_Surface *evas_object_image_native_surface_get(Evas_Object *obj)


    ####################################################################
    # Gradient Object
    #
    Evas_Object *evas_object_gradient_add(Evas *e)
    void evas_object_gradient_color_stop_add(Evas_Object *obj, int r, int g, int b, int a, int delta)
    void evas_object_gradient_alpha_stop_add(Evas_Object *obj, int a, int delta)
    void evas_object_gradient_color_data_set(Evas_Object *obj, void *color_data, int len, Evas_Bool has_alpha)
    void evas_object_gradient_alpha_data_set(Evas_Object *obj, void *alpha_data, int len)
    void evas_object_gradient_clear(Evas_Object *obj)
    void evas_object_gradient_type_set(Evas_Object *obj, char *type, char *instance_params)
    void evas_object_gradient_type_get(Evas_Object *obj, char **type, char **instance_params)
    void evas_object_gradient_fill_set(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h)
    void evas_object_gradient_fill_get(Evas_Object *obj, Evas_Coord *x, Evas_Coord *y, Evas_Coord *w, Evas_Coord *h)
    void evas_object_gradient_fill_angle_set(Evas_Object *obj, Evas_Angle angle)
    Evas_Angle evas_object_gradient_fill_angle_get(Evas_Object *obj)
    void evas_object_gradient_fill_spread_set(Evas_Object *obj, int tile_mode)
    int evas_object_gradient_fill_spread_get(Evas_Object *obj)
    void evas_object_gradient_angle_set(Evas_Object *obj, Evas_Angle angle)
    Evas_Angle evas_object_gradient_angle_get(Evas_Object *obj)
    void evas_object_gradient_direction_set(Evas_Object *obj, int direction)
    int evas_object_gradient_direction_get(Evas_Object *obj)
    void evas_object_gradient_offset_set(Evas_Object *obj, float offset)
    float evas_object_gradient_offset_get(Evas_Object *obj)


    ####################################################################
    # Polygon Object
    #
    Evas_Object *evas_object_polygon_add(Evas *e)
    void evas_object_polygon_point_add(Evas_Object *obj, Evas_Coord x, Evas_Coord y)
    void evas_object_polygon_points_clear(Evas_Object *obj)


    ####################################################################
    # Text Object
    #
    Evas_Object *evas_object_text_add(Evas *e)
    void evas_object_text_font_source_set(Evas_Object *obj, char *font)
    char *evas_object_text_font_source_get(Evas_Object *obj)
    void evas_object_text_font_set(Evas_Object *obj, char *font, Evas_Font_Size size)
    void evas_object_text_font_get(Evas_Object *obj, char **font, Evas_Font_Size *size)
    void evas_object_text_text_set(Evas_Object *obj, char *text)
    char *evas_object_text_text_get(Evas_Object *obj)
    Evas_Coord evas_object_text_ascent_get(Evas_Object *obj)
    Evas_Coord evas_object_text_descent_get(Evas_Object *obj)
    Evas_Coord evas_object_text_max_ascent_get(Evas_Object *obj)
    Evas_Coord evas_object_text_max_descent_get(Evas_Object *obj)
    Evas_Coord evas_object_text_horiz_advance_get(Evas_Object *obj)
    Evas_Coord evas_object_text_vert_advance_get(Evas_Object *obj)
    Evas_Coord evas_object_text_inset_get(Evas_Object *obj)
    int evas_object_text_char_pos_get(Evas_Object *obj, int pos, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch)
    int evas_object_text_char_coords_get(Evas_Object *obj, Evas_Coord x, Evas_Coord y, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch)
    Evas_Text_Style_Type evas_object_text_style_get(Evas_Object *obj)
    void evas_object_text_style_set(Evas_Object *obj, Evas_Text_Style_Type type)
    void evas_object_text_shadow_color_set(Evas_Object *obj, int r, int g, int b, int a)
    void evas_object_text_shadow_color_get(Evas_Object *obj, int *r, int *g, int *b, int *a)
    void evas_object_text_glow_color_set(Evas_Object *obj, int r, int g, int b, int a)
    void evas_object_text_glow_color_get(Evas_Object *obj, int *r, int *g, int *b, int *a)
    void evas_object_text_glow2_color_set(Evas_Object *obj, int r, int g, int b, int a)
    void evas_object_text_glow2_color_get(Evas_Object *obj, int *r, int *g, int *b, int *a)
    void evas_object_text_outline_color_set(Evas_Object *obj, int r, int g, int b, int a)
    void evas_object_text_outline_color_get(Evas_Object *obj, int *r, int *g, int *b, int *a)
    void evas_object_text_style_pad_get(Evas_Object *obj, int *l, int *r, int *t, int *b)


cdef extern from "evas/evas_object_image_python_extras.h":
    ctypedef enum Evas_Object_Image_Rotation:
            ROTATE_NONE
            ROTATE_90
            ROTATE_180
            ROTATE_270

    int evas_object_image_mask_fill(Evas_Object *src, Evas_Object *mask, Evas_Object *surface, int x_mask, int y_mask, int x_surface, int y_surface)
    void evas_object_image_rotate(Evas_Object *image, Evas_Object_Image_Rotation rotation)



cdef public class Rect [object PyEvasRect, type PyEvasRect_Type]:
    cdef int x0, y0, x1, y1, cx, cy, _w, _h


cdef class EventPoint:
    cdef Evas_Point *obj

    cdef void _set_obj(self, Evas_Point *obj)
    cdef void _unset_obj(self)
    cdef void _check_validity(self) except *


cdef class EventPosition:
    cdef readonly EventPoint output
    cdef readonly EventPoint canvas

    cdef void _set_objs(self, void *output, void *canvas)
    cdef void _unset_objs(self)


cdef class EventMouseIn:
    cdef Evas_Event_Mouse_In *obj
    cdef readonly EventPosition position

    cdef void _set_obj(self, void *ptr)
    cdef void _unset_obj(self)
    cdef void _check_validity(self) except *


cdef class EventMouseOut:
    cdef Evas_Event_Mouse_Out *obj
    cdef readonly EventPosition position

    cdef void _set_obj(self, void *ptr)
    cdef void _unset_obj(self)
    cdef void _check_validity(self) except *


cdef class EventMouseDown:
    cdef Evas_Event_Mouse_Down *obj
    cdef readonly EventPosition position

    cdef void _set_obj(self, void *ptr)
    cdef void _unset_obj(self)
    cdef void _check_validity(self) except *


cdef class EventMouseUp:
    cdef Evas_Event_Mouse_Up *obj
    cdef readonly EventPosition position

    cdef void _set_obj(self, void *ptr)
    cdef void _unset_obj(self)
    cdef void _check_validity(self) except *


cdef class EventMouseMove:
    cdef Evas_Event_Mouse_Move *obj
    cdef readonly EventPosition position
    cdef readonly EventPosition prev_position

    cdef void _set_obj(self, void *ptr)
    cdef void _unset_obj(self)
    cdef void _check_validity(self) except *


cdef class EventMouseWheel:
    cdef Evas_Event_Mouse_Wheel *obj
    cdef readonly EventPosition position

    cdef void _set_obj(self, void *ptr)
    cdef void _unset_obj(self)
    cdef void _check_validity(self) except *


cdef class EventKeyDown:
    cdef Evas_Event_Key_Down *obj

    cdef void _set_obj(self, void *ptr)
    cdef void _unset_obj(self)
    cdef void _check_validity(self) except *


cdef class EventKeyUp:
    cdef Evas_Event_Key_Up *obj

    cdef void _set_obj(self, void *ptr)
    cdef void _unset_obj(self)
    cdef void _check_validity(self) except *


cdef class EventHold:
    cdef Evas_Event_Hold *obj

    cdef void _set_obj(self, void *ptr)
    cdef void _unset_obj(self)
    cdef void _check_validity(self) except *


cdef public class Canvas [object PyEvasCanvas, type PyEvasCanvas_Type]:
    cdef Evas *obj

    cdef int _set_obj(self, Evas *obj) except 0
    cdef int _unset_obj(self) except 0


cdef public class Object [object PyEvasObject, type PyEvasObject_Type]:
    cdef Evas_Object *obj
    cdef readonly Canvas evas
    cdef readonly data
    cdef object _callbacks

    cdef int _unset_obj(self) except 0
    cdef int _set_obj(self, Evas_Object *obj) except 0


cdef public class SmartObject(Object) [object PyEvasSmartObject,
                                       type PyEvasSmartObject_Type]:
    cdef object _smart_callbacks
    cdef object _m_delete
    cdef object _m_move
    cdef object _m_resize
    cdef object _m_show
    cdef object _m_hide
    cdef object _m_color_set
    cdef object _m_clip_set
    cdef object _m_clip_unset
    cdef object _m_calculate


cdef public class Rectangle(Object) [object PyEvasRectangle,
                                     type PyEvasRectangle_Type]:
    pass


cdef public class Line(Object) [object PyEvasLine, type PyEvasLine_Type]:
    pass


cdef public class Image(Object) [object PyEvasImage, type PyEvasImage_Type]:
    pass


cdef public class FilledImage(Image) [object PyEvasFilledImage,
                                      type PyEvasFilledImage_Type]:
    pass


cdef public class Gradient(Object) [object PyEvasGradient,
                                    type PyEvasGradient_Type]:
    pass


cdef public class Polygon(Object) [object PyEvasPolygon,
                                   type PyEvasPolygon_Type]:
    pass


cdef public class Text(Object) [object PyEvasText, type PyEvasText_Type]:
    pass


cdef public class ClippedSmartObject(SmartObject) \
         [object PyEvasClippedSmartObject, type PyEvasClippedSmartObject_Type]:
    cdef readonly Rectangle clipper
