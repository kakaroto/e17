cdef extern from "Evas.h":
    ctypedef struct Evas


cdef extern from "Ecore_Evas.h":
    ####################################################################
    # Enumerations
    #
    ctypedef enum Ecore_Evas_Engine_Type:
        ECORE_EVAS_ENGINE_SOFTWARE_X11
        ECORE_EVAS_ENGINE_SOFTWARE_FB
        ECORE_EVAS_ENGINE_GL_X11
        ECORE_EVAS_ENGINE_SOFTWARE_BUFFER
        ECORE_EVAS_ENGINE_XRENDER_X11
        ECORE_EVAS_ENGINE_DIRECTFB


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

    Ecore_Evas *ecore_evas_fb_new(char *disp_name, int rotation, int w, int h)

    Ecore_Evas *ecore_evas_directfb_new(char *disp_name, int windowed, int x, int y, int w, int h)
    Ecore_DirectFB_Window *ecore_evas_directfb_window_get(Ecore_Evas *ee)

    Ecore_Evas *ecore_evas_buffer_new(int w, int h)
    int *ecore_evas_buffer_pixels_get(Ecore_Evas *ee)



    Ecore_Evas *ecore_evas_ecore_evas_get(Evas *e)
    void ecore_evas_free(Ecore_Evas *ee)
    Evas *ecore_evas_get(Ecore_Evas *ee)

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
    void ecore_evas_cursor_get(Ecore_Evas *ee, char **file, int *layer, int *hot_x, int *hot_y)
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
    void ecore_evas_avoid_damage_set(Ecore_Evas *ee, int on)
    int  ecore_evas_avoid_damage_get(Ecore_Evas *ee)
    void ecore_evas_withdrawn_set(Ecore_Evas *ee, int withdrawn)
    int  ecore_evas_withdrawn_get(Ecore_Evas *ee)
    void ecore_evas_sticky_set(Ecore_Evas *ee, int sticky)
    int  ecore_evas_sticky_get(Ecore_Evas *ee)
    void ecore_evas_ignore_events_set(Ecore_Evas *ee, int ignore)
    int  ecore_evas_ignore_events_get(Ecore_Evas *ee)


cdef class EcoreEvas:
    cdef Ecore_Evas *obj
    cdef object _evas
    cdef object _data
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


cdef class SoftwareX11(EcoreEvas):
    pass


cdef class GLX11(EcoreEvas):
    pass


cdef class XRenderX11(EcoreEvas):
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


cdef class DirectFB(EcoreEvas):
    pass


cdef class Buffer(EcoreEvas):
    cdef int _get_buf_size(self)
