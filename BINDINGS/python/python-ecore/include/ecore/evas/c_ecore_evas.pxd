# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Carsten Haitzler, Ulisses Furquim
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

cimport evas.c_evas

cdef extern from "evas/python_evas_utils.h":
    int PY_REFCOUNT(object)

cdef extern from "Eina.h":
    ctypedef struct Eina_List:
        void *data
        Eina_List *next
        Eina_List *prev


cdef extern from "Ecore_Evas.h":
    ####################################################################
    # Enumerations
    #
    ctypedef enum Ecore_Evas_Engine_Type:
        ECORE_EVAS_ENGINE_SOFTWARE_BUFFER
        ECORE_EVAS_ENGINE_SOFTWARE_X11
        ECORE_EVAS_ENGINE_XRENDER_X11
        ECORE_EVAS_ENGINE_OPENGL_X11
        ECORE_EVAS_ENGINE_SOFTWARE_XCB
        ECORE_EVAS_ENGINE_XRENDER_XCB
        ECORE_EVAS_ENGINE_SOFTWARE_DDRAW
        ECORE_EVAS_ENGINE_DIRECT3D
        ECORE_EVAS_ENGINE_OPENGL_GLEW
        ECORE_EVAS_ENGINE_SOFTWARE_SDL
        ECORE_EVAS_ENGINE_DIRECTFB
        ECORE_EVAS_ENGINE_SOFTWARE_FB
        ECORE_EVAS_ENGINE_SOFTWARE_16_X11
        ECORE_EVAS_ENGINE_SOFTWARE_16_DDRAW
        ECORE_EVAS_ENGINE_SOFTWARE_16_WINCE

    ctypedef enum Ecore_Evas_Avoid_Damage_Type:
        ECORE_EVAS_AVOID_DAMAGE_NONE
        ECORE_EVAS_AVOID_DAMAGE_EXPOSE
        ECORE_EVAS_AVOID_DAMAGE_BUILT_IN

    ctypedef enum Ecore_Evas_Object_Associate_Flags:
        ECORE_EVAS_OBJECT_ASSOCIATE_BASE
        ECORE_EVAS_OBJECT_ASSOCIATE_STACK
        ECORE_EVAS_OBJECT_ASSOCIATE_LAYER


    ####################################################################
    # Structures
    #
    ctypedef struct Ecore_Evas

#     ctypedef struct Ecore_DirectFB_Cursor:
#         void *surface
#         int hot_x
#         int hot_y

    ctypedef struct Ecore_DirectFB_Window:
        unsigned id
        void *window
        void *surface
        void *cursor
#        Ecore_DirectFB_Cursor *cursor


    ####################################################################
    # Other typedefs
    #
    ctypedef unsigned int Ecore_X_Window

    int ecore_evas_init()
    int ecore_evas_shutdown()

    int ecore_evas_engine_type_supported_get(Ecore_Evas_Engine_Type engine)

    Eina_List  *ecore_evas_engines_get()
    void ecore_evas_engines_free(Eina_List *engines)
    Ecore_Evas *ecore_evas_new(char *engine_name, int x, int y, int w, int h, char *extra_options)
    char *ecore_evas_engine_name_get(Ecore_Evas *ee)

    int ecore_evas_object_associate(Ecore_Evas *ee, evas.c_evas.Evas_Object *obj, Ecore_Evas_Object_Associate_Flags flags)
    int ecore_evas_object_dissociate(Ecore_Evas *ee, evas.c_evas.Evas_Object *obj)
    evas.c_evas.Evas_Object *ecore_evas_object_associate_get(Ecore_Evas *ee)

    Ecore_Evas *ecore_evas_software_x11_new(char *disp_name, Ecore_X_Window parent, int x, int y, int w, int h)
    Ecore_X_Window ecore_evas_software_x11_window_get(Ecore_Evas *ee)
    Ecore_X_Window ecore_evas_software_x11_subwindow_get(Ecore_Evas *ee)
    void ecore_evas_software_x11_direct_resize_set(Ecore_Evas *ee, int on)
    int ecore_evas_software_x11_direct_resize_get(Ecore_Evas *ee)
    void ecore_evas_software_x11_extra_event_window_add(Ecore_Evas *ee, Ecore_X_Window win)

    Ecore_Evas *ecore_evas_gl_x11_new(char *disp_name, Ecore_X_Window parent, int x, int y, int w, int h)
    Ecore_X_Window ecore_evas_gl_x11_window_get(Ecore_Evas *ee)
    Ecore_X_Window ecore_evas_gl_x11_subwindow_get(Ecore_Evas *ee)
    void ecore_evas_gl_x11_direct_resize_set(Ecore_Evas *ee, int on)
    int ecore_evas_gl_x11_direct_resize_get(Ecore_Evas *ee)
    void ecore_evas_gl_x11_extra_event_window_add(Ecore_Evas *ee, Ecore_X_Window win)

    Ecore_Evas *ecore_evas_xrender_x11_new(char *disp_name, Ecore_X_Window parent, int x, int y, int w, int h)
    Ecore_X_Window ecore_evas_xrender_x11_window_get(Ecore_Evas *ee)
    Ecore_X_Window ecore_evas_xrender_x11_subwindow_get(Ecore_Evas *ee)
    void ecore_evas_xrender_x11_direct_resize_set(Ecore_Evas *ee, int on)
    int ecore_evas_xrender_x11_direct_resize_get(Ecore_Evas *ee)
    void ecore_evas_xrender_x11_extra_event_window_add(Ecore_Evas *ee, Ecore_X_Window win)

    Ecore_Evas *ecore_evas_software_x11_16_new(char *disp_name, Ecore_X_Window parent, int x, int y, int w, int h)
    Ecore_X_Window ecore_evas_software_x11_16_window_get(Ecore_Evas *ee)
    Ecore_X_Window ecore_evas_software_x11_16_subwindow_get(Ecore_Evas *ee)
    void ecore_evas_software_x11_16_direct_resize_set(Ecore_Evas *ee, int on)
    int ecore_evas_software_x11_16_direct_resize_get(Ecore_Evas *ee)
    void ecore_evas_software_x11_16_extra_event_window_add(Ecore_Evas *ee, Ecore_X_Window win)

    Ecore_Evas *ecore_evas_fb_new(char *disp_name, int rotation, int w, int h)

    Ecore_Evas *ecore_evas_directfb_new(char *disp_name, int windowed, int x, int y, int w, int h)
    Ecore_DirectFB_Window *ecore_evas_directfb_window_get(Ecore_Evas *ee)

    Ecore_Evas *ecore_evas_buffer_new(int w, int h)
    int *ecore_evas_buffer_pixels_get(Ecore_Evas *ee)



    Ecore_Evas *ecore_evas_ecore_evas_get(evas.c_evas.Evas *e)
    void ecore_evas_free(Ecore_Evas *ee)
    evas.c_evas.Evas *ecore_evas_get(Ecore_Evas *ee)

    void *ecore_evas_data_get(Ecore_Evas *ee, char *key)
    void ecore_evas_data_set(Ecore_Evas *ee, char *key, void *data)

    void ecore_evas_callback_resize_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_move_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_show_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_hide_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_delete_request_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_destroy_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_focus_in_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_focus_out_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_sticky_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_unsticky_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_mouse_in_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_mouse_out_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_pre_render_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))
    void ecore_evas_callback_post_render_set(Ecore_Evas *ee, void (*func) (Ecore_Evas *ee))

    void ecore_evas_move(Ecore_Evas *ee, int x, int y)
    void ecore_evas_managed_move(Ecore_Evas *ee, int x, int y)
    void ecore_evas_resize(Ecore_Evas *ee, int w, int h)
    void ecore_evas_move_resize(Ecore_Evas *ee, int x, int y, int w, int h)
    void ecore_evas_geometry_get(Ecore_Evas *ee, int *x, int *y, int *w, int *h)
    void ecore_evas_rotation_set(Ecore_Evas *ee, int rot)
    int  ecore_evas_rotation_get(Ecore_Evas *ee)
    void ecore_evas_shaped_set(Ecore_Evas *ee, int shaped)
    int  ecore_evas_shaped_get(Ecore_Evas *ee)
    void ecore_evas_alpha_set(Ecore_Evas *ee, int alpha)
    int  ecore_evas_alpha_get(Ecore_Evas *ee)
    void ecore_evas_show(Ecore_Evas *ee)
    void ecore_evas_hide(Ecore_Evas *ee)
    int  ecore_evas_visibility_get(Ecore_Evas *ee)
    void ecore_evas_raise(Ecore_Evas *ee)
    void ecore_evas_lower(Ecore_Evas *ee)
    void ecore_evas_activate(Ecore_Evas *ee)
    void ecore_evas_title_set(Ecore_Evas *ee, char *t)
    char *ecore_evas_title_get(Ecore_Evas *ee)
    void ecore_evas_name_class_set(Ecore_Evas *ee, char *n, char *c)
    void ecore_evas_name_class_get(Ecore_Evas *ee, char **n, char **c)
    void ecore_evas_size_min_set(Ecore_Evas *ee, int w, int h)
    void ecore_evas_size_min_get(Ecore_Evas *ee, int *w, int *h)
    void ecore_evas_size_max_set(Ecore_Evas *ee, int w, int h)
    void ecore_evas_size_max_get(Ecore_Evas *ee, int *w, int *h)
    void ecore_evas_size_base_set(Ecore_Evas *ee, int w, int h)
    void ecore_evas_size_base_get(Ecore_Evas *ee, int *w, int *h)
    void ecore_evas_size_step_set(Ecore_Evas *ee, int w, int h)
    void ecore_evas_size_step_get(Ecore_Evas *ee, int *w, int *h)
    void ecore_evas_cursor_set(Ecore_Evas *ee, char *file, int layer, int hot_x, int hot_y)
    void ecore_evas_cursor_get(Ecore_Evas *ee, evas.c_evas.Evas_Object **obj, int *layer, int *hot_x, int *hot_y)
    void ecore_evas_object_cursor_set(Ecore_Evas *ee, evas.c_evas.Evas_Object *obj, int layer, int hot_x, int hot_y)
    void ecore_evas_layer_set(Ecore_Evas *ee, int layer)
    int  ecore_evas_layer_get(Ecore_Evas *ee)
    void ecore_evas_focus_set(Ecore_Evas *ee, int on)
    int  ecore_evas_focus_get(Ecore_Evas *ee)
    void ecore_evas_iconified_set(Ecore_Evas *ee, int on)
    int  ecore_evas_iconified_get(Ecore_Evas *ee)
    void ecore_evas_borderless_set(Ecore_Evas *ee, int on)
    int  ecore_evas_borderless_get(Ecore_Evas *ee)
    void ecore_evas_override_set(Ecore_Evas *ee, int on)
    int  ecore_evas_override_get(Ecore_Evas *ee)
    void ecore_evas_maximized_set(Ecore_Evas *ee, int on)
    int  ecore_evas_maximized_get(Ecore_Evas *ee)
    void ecore_evas_fullscreen_set(Ecore_Evas *ee, int on)
    int  ecore_evas_fullscreen_get(Ecore_Evas *ee)
    void ecore_evas_avoid_damage_set(Ecore_Evas *ee, Ecore_Evas_Avoid_Damage_Type damage_type)
    Ecore_Evas_Avoid_Damage_Type  ecore_evas_avoid_damage_get(Ecore_Evas *ee)
    void ecore_evas_withdrawn_set(Ecore_Evas *ee, int withdrawn)
    int  ecore_evas_withdrawn_get(Ecore_Evas *ee)
    void ecore_evas_sticky_set(Ecore_Evas *ee, int sticky)
    int  ecore_evas_sticky_get(Ecore_Evas *ee)
    void ecore_evas_ignore_events_set(Ecore_Evas *ee, int ignore)
    int  ecore_evas_ignore_events_get(Ecore_Evas *ee)


cdef extern from "Ecore_X.h":
    ctypedef unsigned int Ecore_X_Cursor
    ctypedef unsigned int Ecore_X_Virtual_Keyboard_State

    cdef void ecore_x_window_cursor_set(Ecore_X_Window win, Ecore_X_Cursor c)
    Ecore_X_Cursor ecore_x_cursor_shape_get(int shape)
    cdef void ecore_x_e_virtual_keyboard_state_set(Ecore_X_Window win, Ecore_X_Virtual_Keyboard_State state)


cdef class EcoreEvas:
    cdef Ecore_Evas *obj
    cdef readonly evas.c_evas.Canvas evas
    cdef readonly object data
    cdef object _resize_cb
    cdef object _move_cb
    cdef object _show_cb
    cdef object _hide_cb
    cdef object _delete_request_cb
    cdef object _destroy_cb
    cdef object _focus_in_cb
    cdef object _focus_out_cb
    cdef object _sticky_cb
    cdef object _unsticky_cb
    cdef object _mouse_in_cb
    cdef object _mouse_out_cb
    cdef object _pre_render_cb
    cdef object _post_render_cb

    cdef int _set_obj(EcoreEvas self, Ecore_Evas *obj) except 0


cdef class BaseX11(EcoreEvas):
    pass

cdef class SoftwareX11(BaseX11):
    pass


cdef class GLX11(BaseX11):
    pass


cdef class XRenderX11(BaseX11):
    pass


cdef class SoftwareX11_16(BaseX11):
    pass


cdef class FB(EcoreEvas):
    pass


# cdef class DirectFBCursor:
#     cdef Ecore_DirectFB_Cursor *obj

#     cdef _set_obj(self, Ecore_DirectFB_Cursor *obj)


# cdef class DirectFBWindow:
#     cdef Ecore_DirectFB_Window *obj
#     cdef DirectFBCursor _cursor

#     cdef _set_obj(self, Ecore_DirectFB_Window *obj)


# cdef class DirectFB(EcoreEvas):
#     pass


cdef class Buffer(EcoreEvas):
    cdef int _get_buf_size(self)
