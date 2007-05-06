# This file is included verbatim by c_ecore_evas.pyx

cdef EcoreEvas EcoreEvas_from_instance(Ecore_Evas *obj):
    cdef void *data
    data = ecore_evas_data_get(obj, "python-ecore-evas")
    assert data != NULL, "Ecore_Evas is not wrapped by Python object."
    return <EcoreEvas>data

cdef void resize_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._resize_cb(self)

cdef void move_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._move_cb(self)

cdef void show_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._show_cb(self)

cdef void hide_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._hide_cb(self)

cdef void delete_request_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._delete_request_cb(self)

cdef void destroy_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._destroy_cb(self)

cdef void focus_in_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._focus_in_cb(self)

cdef void focus_out_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._focus_out_cb(self)

cdef void sticky_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._sticky_cb(self)

cdef void unsticky_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._unsticky_cb(self)

cdef void mouse_in_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._mouse_in_cb(self)

cdef void mouse_out_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._mouse_out_cb(self)

cdef void pre_render_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._pre_render_cb(self)

cdef void post_render_cb(Ecore_Evas *ee):
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    self._post_render_cb(self)



cdef class EcoreEvas:
    def __new__(self, *args, **kargs):
        self.obj = NULL
        self._data = dict()

    def __dealloc__(self):
        self._data = None
        if self.obj:
            if self._evas:
                self._evas._unset_obj()
                self._evas = None
            ecore_evas_data_set(self.obj, "python-ecore-evas", NULL)
            ecore_evas_free(self.obj)
            self.obj = NULL

    cdef int _set_obj(EcoreEvas self, Ecore_Evas *obj) except 0:
        cdef long evas_ptr

        assert self.obj == NULL, "Object must be clean"
        self.obj = obj
        ecore_evas_data_set(obj, "python-ecore-evas", <void *>self)

        evas_ptr = <long>ecore_evas_get(obj)
        if evas_ptr:
            self._evas = evas.c_evas._Canvas_from_instance(evas_ptr)

        return 1

    def __str__(self):
        return "%s(0x%x, refcount=%d, Ecore_Evas=0x%x, Evas=%s)" % \
               (self.__class__.__name__, <unsigned long>self,
                python.REFCOUNT(self), <unsigned long>self.obj,
                self._evas)

    def evas_get(self):
        return self._evas

    property evas:
        def __get__(self):
            return self._evas

    property data:
        def __get__(self):
            return self._data

    def cursor_get(self):
        cdef char *filename
        cdef int layer, hotx, hoty
        ecore_evas_cursor_get(self.obj, &filename, &layer, &hotx, &hoty)
        if filename == NULL:
            fn = None
        else:
            fn = filename
        return (fn, layer, hotx, hoty)

    def cursor_set(self, filename, int layer=0, int hotx=0, int hoty=0):
        cdef char *f
        if filename is None:
            f = NULL
        else:
            f = filename
        ecore_evas_cursor_set(self.obj, f, layer, hotx, hoty)

    def move(self, int x, int y):
        ecore_evas_move(self.obj, x, y)

    def pos_set(self, int x, int y):
        ecore_evas_move(self.obj, x, y)

    def pos_get(self):
        cdef int x, y
        ecore_evas_geometry_get(self.obj, &x, &y, NULL, NULL)
        return (x, y)

    property pos:
        def __set__(self, spec):
            self.pos_set(*spec)

        def __get__(self):
            return self.pos_get()

    def managed_move(self, int x, int y):
        ecore_evas_managed_move(self.obj, x, y)

    def resize(self, int w, int h):
        ecore_evas_resize(self.obj, w, h)

    def size_set(self, int w, int h):
        ecore_evas_resize(self.obj, w, h)

    def size_get(self):
        cdef int w, h
        ecore_evas_geometry_get(self.obj, NULL, NULL, &w, &h)
        return (w, h)

    property size:
        def __set__(self, spec):
            self.size_set(*spec)

        def __get__(self):
            return self.size_get()

    def move_resize(self, int x, int y, int w, int h):
        ecore_evas_move_resize(self.obj, x, y, w, h)

    def geometry_set(self, int x, int y, int w, int h):
        ecore_evas_move_resize(self.obj, x, y, w, h)

    def geometry_get(self):
        cdef int x, y, w, h
        ecore_evas_geometry_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    property geometry:
        def __set__(self, spec):
            self.geometry_set(*spec)

        def __get__(self):
            return self.geometry_get()

    def rotation_set(self, int rot):
        """Set rotation angle, in degrees.

        Acceptable values depends on underlying engine, but usually this
        is multiple of 90 degrees.
        """
        ecore_evas_rotation_set(self.obj, rot)

    def rotation_get(self):
        "Get rotation angle, in degrees."
        return ecore_evas_rotation_get(self.obj)

    property rotation:
        def __get__(self):
            return self.rotation_get()

        def __set__(self, int value):
            self.rotation_set(value)


    def shaped_set(self, int shaped):
        """Set whether the object is shaped or not.

        If true, EcoreEvas will be transparent in areas without any
        evas.Object.
        """
        ecore_evas_shaped_set(self.obj, shaped)

    def shaped_get(self):
        """Get whether the object is shaped or not.

        A shaped EcoreEvas will be transparent in areas without any
        evas.Object
        """
        return bool(ecore_evas_shaped_get(self.obj))

    property shaped:
        def __get__(self):
            return self.shaped_get()

        def __set__(self, int value):
            self.shaped_set(value)


    def alpha_set(self, int alpha):
        """Set whether the object has an alpha channel or not.

        This method allows you to make an EcoreEvas translucent using an
        alpha channel. See shaped_set() for details. The difference
        between a shaped window and a window with an alpha channel is
        that an alpha channel supports multiple levels of transpararency,
        as opposed to the 1 bit transparency of a shaped window (a pixel
        is either opaque, or it's transparent).
        """
        ecore_evas_alpha_set(self.obj, alpha)

    def alpha_get(self):
        "Get whether the object has an alpha channel or not."
        return bool(ecore_evas_alpha_get(self.obj))

    property alpha:
        def __get__(self):
            return self.alpha_get()

        def __set__(self, int value):
            self.alpha_set(value)

    def show(self):
        ecore_evas_show(self.obj)

    def hide(self):
        ecore_evas_hide(self.obj)

    def visibility_set(self, on):
        if on:
            self.show()
        else:
            self.hide()

    def visibility_get(self):
        return bool(ecore_evas_visibility_get(self.obj))

    property visibility:
        def __get__(self):
            return self.visibility_get()

        def __set__(self, int value):
            self.visibility_set(value)

    def raise_(self):
        ecore_evas_raise(self.obj)

    def lower(self):
        ecore_evas_lower(self.obj)

    def title_set(self, char *t):
        ecore_evas_title_set(self.obj, t)

    def title_get(self):
        cdef char *s
        s = ecore_evas_title_get(self.obj)
        if s != NULL:
            return s

    property title:
        def __get__(self):
            return self.title_get()

        def __set__(self, char *value):
            self.title_set(value)

    def name_class_set(self, char *n, char *c):
        ecore_evas_name_class_set(self.obj, n, c)

    def name_class_get(self):
        cdef char *n, *c
        ecore_evas_name_class_get(self.obj, &n, &c)
        if n == NULL:
            name = None
        else:
            name = n
        if c == NULL:
            cls = None
        else:
            cls = c
        return (name, cls)

    property name_class:
        def __get__(self):
            return self.name_class_get()

        def __set__(self, spec):
            self.name_class_set(*spec)

    def size_min_set(self, int w, int h):
        ecore_evas_size_min_set(self.obj, w, h)

    def size_min_get(self):
        cdef int w, h
        ecore_evas_size_min_get(self.obj, &w, &h)
        return (w, h)

    property size_min:
        def __get__(self):
            return self.size_min_get()

        def __set__(self, spec):
            self.size_min_set(*spec)

    def size_max_set(self, int w, int h):
        ecore_evas_size_max_set(self.obj, w, h)

    def size_max_get(self):
        cdef int w, h
        ecore_evas_size_max_get(self.obj, &w, &h)
        return (w, h)

    property size_max:
        def __get__(self):
            return self.size_max_get()

        def __set__(self, spec):
            self.size_max_set(*spec)

    def size_base_set(self, int w, int h):
        ecore_evas_size_base_set(self.obj, w, h)

    def size_base_get(self):
        cdef int w, h
        ecore_evas_size_base_get(self.obj, &w, &h)
        return (w, h)

    property size_base:
        def __get__(self):
            return self.size_base_get()

        def __set__(self, spec):
            self.size_base_set(*spec)

    def size_step_set(self, int w, int h):
        ecore_evas_size_step_set(self.obj, w, h)

    def size_step_get(self):
        cdef int w, h
        ecore_evas_size_step_get(self.obj, &w, &h)
        return (w, h)

    property size_step:
        def __get__(self):
            return self.size_step_get()

        def __set__(self, spec):
            self.size_step_set(*spec)

    def layer_set(self, int layer):
        ecore_evas_layer_set(self.obj, layer)

    def layer_get(self):
        return bool(ecore_evas_layer_get(self.obj))

    property layer:
        def __get__(self):
            return self.layer_get()

        def __set__(self, int value):
            self.layer_set(value)

    def focus_set(self, int on):
        ecore_evas_focus_set(self.obj, on)

    def focus_get(self):
        return bool(ecore_evas_focus_get(self.obj))

    property focus:
        def __get__(self):
            return self.focus_get()

        def __set__(self, int value):
            self.focus_set(value)

    def iconified_set(self, int on):
        ecore_evas_iconified_set(self.obj, on)

    def iconified_get(self):
        return bool(ecore_evas_iconified_get(self.obj))

    property iconified:
        def __get__(self):
            return self.iconified_get()

        def __set__(self, int value):
            self.iconified_set(value)

    def borderless_set(self, int on):
        ecore_evas_borderless_set(self.obj, on)

    def borderless_get(self):
        return bool(ecore_evas_borderless_get(self.obj))

    property borderless:
        def __get__(self):
            return self.borderless_get()

        def __set__(self, int value):
            self.borderless_set(value)

    def override_set(self, int on):
        ecore_evas_override_set(self.obj, on)

    def override_get(self):
        return bool(ecore_evas_override_get(self.obj))

    property override:
        def __get__(self):
            return self.override_get()

        def __set__(self, int value):
            self.override_set(value)


    def maximized_set(self, int on):
        ecore_evas_maximized_set(self.obj, on)

    def maximized_get(self):
        return bool(ecore_evas_maximized_get(self.obj))

    property maximized:
        def __get__(self):
            return self.maximized_get()

        def __set__(self, int value):
            self.maximized_set(value)

    def fullscreen_get(self):
        return bool(ecore_evas_fullscreen_get(self.obj))

    def fullscreen_set(self, int spec):
        ecore_evas_fullscreen_set(self.obj, spec)

    property fullscreen:
        def __get__(self):
            return self.fullscreen_get()

        def __set__(self, int spec):
            self.fullscreen_set(spec)

    def avoid_damage_set(self, int on):
        ecore_evas_avoid_damage_set(self.obj, on)

    def avoid_damage_get(self):
        return bool(ecore_evas_avoid_damage_get(self.obj))

    property avoid_damage:
        def __get__(self):
            return self.avoid_damage_get()

        def __set__(self, int value):
            self.avoid_damage_set(value)

    def withdrawn_set(self, int withdrawn):
        ecore_evas_withdrawn_set(self.obj, on)

    def withdrawn_get(self):
        return bool(ecore_evas_withdrawn_get(self.obj))

    property withdrawn:
        def __get__(self):
            return self.withdrawn_get()

        def __set__(self, int value):
            self.withdrawn_set(value)

    def sticky_set(self, int sticky):
        ecore_evas_sticky_set(self.obj, on)

    def sticky_get(self):
        return bool(ecore_evas_sticky_get(self.obj))

    property sticky:
        def __get__(self):
            return self.sticky_get()

        def __set__(self, int value):
            self.sticky_set(value)

    def ignore_events_set(self, int ignore):
        ecore_evas_ignore_events_set(self.obj, on)

    def ignore_events_get(self):
        return bool(ecore_evas_ignore_events_get(self.obj))

    property ignore_events:
        def __get__(self):
            return self.ignore_events_get()

        def __set__(self, int value):
            self.ignore_events_set(value)


    ####################################################################
    # Callbacks
    #
    def callback_resize_set(self, func):
        self._resize_cb = func
        if func is None:
            ecore_evas_callback_resize_set(self.obj, NULL)
        else:
            ecore_evas_callback_resize_set(self.obj, resize_cb)

    def callback_resize_get(self):
        return self._resize_cb

    property callback_resize:
        def __get__(self):
            return self.callback_resize_get()

        def __set__(self, func):
            self.callback_resize_set(func)

        def __del__(self):
            self.callback_resize_set(None)

    def callback_move_set(self, func):
        self._move_cb = func
        if func is None:
            ecore_evas_callback_move_set(self.obj, NULL)
        else:
            ecore_evas_callback_move_set(self.obj, move_cb)

    def callback_move_get(self):
        return self._move_cb

    property callback_move:
        def __get__(self):
            return self.callback_move_get()

        def __set__(self, func):
            self.callback_move_set(func)

        def __del__(self):
            self.callback_move_set(None)

    def callback_show_set(self, func):
        self._show_cb = func
        if func is None:
            ecore_evas_callback_show_set(self.obj, NULL)
        else:
            ecore_evas_callback_show_set(self.obj, show_cb)

    def callback_show_get(self):
        return self._show_cb

    property callback_show:
        def __get__(self):
            return self.callback_show_get()

        def __set__(self, func):
            self.callback_show_set(func)

        def __del__(self):
            self.callback_show_set(None)

    def callback_hide_set(self, func):
        self._hide_cb = func
        if func is None:
            ecore_evas_callback_hide_set(self.obj, NULL)
        else:
            ecore_evas_callback_hide_set(self.obj, hide_cb)

    def callback_hide_get(self):
        return self._hide_cb

    property callback_hide:
        def __get__(self):
            return self.callback_hide_get()

        def __set__(self, func):
            self.callback_hide_set(func)

        def __del__(self):
            self.callback_hide_set(None)

    def callback_delete_request_set(self, func):
        self._delete_request_cb = func
        if func is None:
            ecore_evas_callback_delete_request_set(self.obj, NULL)
        else:
            ecore_evas_callback_delete_request_set(self.obj, delete_request_cb)

    def callback_delete_request_get(self):
        return self._delete_request_cb

    property callback_delete_request:
        def __get__(self):
            return self.callback_delete_request_get()

        def __set__(self, func):
            self.callback_delete_request_set(func)

        def __del__(self):
            self.callback_delete_request_set(None)

    def callback_destroy_set(self, func):
        self._destroy_cb = func
        if func is None:
            ecore_evas_callback_destroy_set(self.obj, NULL)
        else:
            ecore_evas_callback_destroy_set(self.obj, destroy_cb)

    def callback_destroy_get(self):
        return self._destroy_cb

    property callback_destroy:
        def __get__(self):
            return self.callback_destroy_get()

        def __set__(self, func):
            self.callback_destroy_set(func)

        def __del__(self):
            self.callback_destroy_set(None)

    def callback_focus_in_set(self, func):
        self._focus_in_cb = func
        if func is None:
            ecore_evas_callback_focus_in_set(self.obj, NULL)
        else:
            ecore_evas_callback_focus_in_set(self.obj, focus_in_cb)

    def callback_focus_in_get(self):
        return self._focus_in_cb

    property callback_focus_in:
        def __get__(self):
            return self.callback_focus_in_get()

        def __set__(self, func):
            self.callback_focus_in_set(func)

        def __del__(self):
            self.callback_focus_in_set(None)

    def callback_focus_out_set(self, func):
        self._focus_out_cb = func
        if func is None:
            ecore_evas_callback_focus_out_set(self.obj, NULL)
        else:
            ecore_evas_callback_focus_out_set(self.obj, focus_out_cb)

    def callback_focus_out_get(self):
        return self._focus_out_cb

    property callback_focus_out:
        def __get__(self):
            return self.callback_focus_out_get()

        def __set__(self, func):
            self.callback_focus_out_set(func)

        def __del__(self):
            self.callback_focus_out_set(None)

    def callback_sticky_set(self, func):
        self._sticky_cb = func
        if func is None:
            ecore_evas_callback_sticky_set(self.obj, NULL)
        else:
            ecore_evas_callback_sticky_set(self.obj, sticky_cb)

    def callback_sticky_get(self):
        return self._sticky_cb

    property callback_sticky:
        def __get__(self):
            return self.callback_sticky_get()

        def __set__(self, func):
            self.callback_sticky_set(func)

        def __del__(self):
            self.callback_sticky_set(None)

    def callback_unsticky_set(self, func):
        self._unsticky_cb = func
        if func is None:
            ecore_evas_callback_unsticky_set(self.obj, NULL)
        else:
            ecore_evas_callback_unsticky_set(self.obj, unsticky_cb)

    def callback_unsticky_get(self):
        return self._unsticky_cb

    property callback_unsticky:
        def __get__(self):
            return self.callback_unsticky_get()

        def __set__(self, func):
            self.callback_unsticky_set(func)

        def __del__(self):
            self.callback_unsticky_set(None)

    def callback_mouse_in_set(self, func):
        self._mouse_in_cb = func
        if func is None:
            ecore_evas_callback_mouse_in_set(self.obj, NULL)
        else:
            ecore_evas_callback_mouse_in_set(self.obj, mouse_in_cb)

    def callback_mouse_in_get(self):
        return self._mouse_in_cb

    property callback_mouse_in:
        def __get__(self):
            return self.callback_mouse_in_get()

        def __set__(self, func):
            self.callback_mouse_in_set(func)

        def __del__(self):
            self.callback_mouse_in_set(None)

    def callback_mouse_out_set(self, func):
        self._mouse_out_cb = func
        if func is None:
            ecore_evas_callback_mouse_out_set(self.obj, NULL)
        else:
            ecore_evas_callback_mouse_out_set(self.obj, mouse_out_cb)

    def callback_mouse_out_get(self):
        return self._mouse_out_cb

    property callback_mouse_out:
        def __get__(self):
            return self.callback_mouse_out_get()

        def __set__(self, func):
            self.callback_mouse_out_set(func)

        def __del__(self):
            self.callback_mouse_out_set(None)

    def callback_pre_render_set(self, func):
        self._pre_render_cb = func
        if func is None:
            ecore_evas_callback_pre_render_set(self.obj, NULL)
        else:
            ecore_evas_callback_pre_render_set(self.obj, pre_render_cb)

    def callback_pre_render_get(self):
        return self._pre_render_cb

    property callback_pre_render:
        def __get__(self):
            return self.callback_pre_render_get()

        def __set__(self, func):
            self.callback_pre_render_set(func)

        def __del__(self):
            self.callback_pre_render_set(None)

    def callback_post_render_set(self, func):
        self._post_render_cb = func
        if func is None:
            ecore_evas_callback_post_render_set(self.obj, NULL)
        else:
            ecore_evas_callback_post_render_set(self.obj, post_render_cb)

    def callback_post_render_get(self):
        return self._post_render_cb

    property callback_post_render:
        def __get__(self):
            return self.callback_post_render_get()

        def __set__(self, func):
            self.callback_post_render_set(func)

        def __del__(self):
            self.callback_post_render_set(None)


