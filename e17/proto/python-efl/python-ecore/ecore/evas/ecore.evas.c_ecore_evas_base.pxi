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

# This file is included verbatim by c_ecore_evas.pyx

cimport evas.c_evas as c_evas
import traceback

cdef EcoreEvas EcoreEvas_from_instance(Ecore_Evas *obj):
    cdef void *data
    data = ecore_evas_data_get(obj, "python-ecore-evas")
    assert data != NULL, "Ecore_Evas is not wrapped by Python object."
    return <EcoreEvas>data

cdef void resize_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._resize_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void move_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._move_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void show_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._show_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void hide_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._hide_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void delete_request_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._delete_request_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void destroy_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._destroy_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void focus_in_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._focus_in_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void focus_out_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._focus_out_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void sticky_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._sticky_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void unsticky_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._unsticky_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void mouse_in_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._mouse_in_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void mouse_out_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._mouse_out_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void pre_render_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._pre_render_cb(self)
    except Exception, e:
        traceback.print_exc()

cdef void post_render_cb(Ecore_Evas *ee) with gil:
    cdef EcoreEvas self
    self = EcoreEvas_from_instance(ee)
    try:
        self._post_render_cb(self)
    except Exception, e:
        traceback.print_exc()



cdef class EcoreEvas:
    """Integration of Ecore and Evas providing a Window.

    This class does the integration of ecore and evas, setups the correct
    evas engine and handle events, redraws and other requeriments for a
    properly working system.

    @ivar evas: associated Evas canvas.
    @ivar alpha: if using alpha channel or not.
    @ivar shaped: if window is shaped or not.
    @ivar avoid_damage: if avoiding damage or not.
    @ivar focus: if window have focus or not.
    @ivar pos: window position.
    @ivar size: window size.
    @ivar size_base: window base size.
    @ivar size_max: maximum size.
    @ivar size_min: minimum size.
    @ivar size_step: size must be multiple of these values.
    @ivar geometry: position and size.
    @ivar ignore_events: if window ignores events or not.
    @ivar layer: which layer this window is.
    @ivar borderless: if window have border or not.
    @ivar fullscreen: if window is fullscreen.
    @ivar iconified: if window is minimized (iconified).
    @ivar maximized: if window is maximized or not.
    @ivar sticky: if window is sticky or not.
    @ivar override: if the WM should ignore or not this window.
    @ivar rotation: if window contents should be rotated or not.
    @ivar name_class: (name, class) that identifies this window (WM_CLASS)
    @ivar title: window title.
    @ivar visibility: if window is visible or not.
    @ivar withdrawn: if window is withdrawn or not.
    @ivar callback_delete_request: callback used to handle delete events.
    @ivar callback_destroy: callback used to handle destroy events.
    @ivar callback_focus_in: callback used to handle focus in events.
    @ivar callback_focus_out: callback used to handle focus out events.
    @ivar callback_mouse_in: callback used to handle mouse in events.
    @ivar callback_mouse_out: callback used to handle mouse out events.
    @ivar callback_post_render: callback used after render.
    @ivar callback_pre_render: callback used before render.
    @ivar callback_move: callback used to handle move events.
    @ivar callback_resize: callback used to handle resize events.
    @ivar callback_hide: callback used to handle hide events.
    @ivar callback_show: callback used to handle show events.
    @ivar callback_sticky: callback used to handle sticky events.
    @ivar callback_unsticky: callback used to handle unsticky events.

    @group Callbacks: callback_delete_request_set, callback_delete_request_get,
     callback_destroy_get, callback_destroy_set, callback_focus_in_get,
     callback_focus_in_set, callback_focus_out_get, callback_focus_out_set,
     callback_hide_get, callback_hide_set, callback_mouse_in_get,
     callback_mouse_in_set, callback_mouse_out_get, callback_mouse_out_set,
     callback_move_get, callback_move_set, callback_post_render_get,
     callback_post_render_set, callback_pre_render_get,
     callback_pre_render_set, callback_resize_get, callback_resize_set,
     callback_show_get, callback_show_set, callback_sticky_get,
     callback_sticky_set, callback_unsticky_get, callback_unsticky_set,
     callback_delete_request, callback_destroy, callback_focus_in,
     callback_focus_out, callback_hide, callback_mouse_in,
     callback_mouse_out, callback_move, callback_post_render,
     callback_pre_render, callback_resize, callback_show, callback_sticky,
     callback_unsticky
    """
    def __new__(self, *args, **kargs):
        self.obj = NULL
        self.data = dict()

    def __dealloc__(self):
        self.data = None
        if self.obj:
            if self.evas:
                self.evas._unset_obj()
                self.evas = None
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
            self.evas = evas.c_evas._Canvas_from_instance(evas_ptr)

        return 1

    def __str__(self):
        return "%s(Evas=%s)" % (self.__class__.__name__, self.evas)

    def __repr__(self):
        return "%s(0x%x, refcount=%d, Ecore_Evas=0x%x, Evas=%r)" % \
               (self.__class__.__name__, <unsigned long>self,
                PY_REFCOUNT(self), <unsigned long>self.obj,
                self.evas)

    def evas_get(self):
        "@rtype: L{evas.c_evas.Canvas}"
        return self.evas

    def cursor_get(self):
        "@rtype: (object, int, int, int)"
        cdef evas.c_evas.Evas_Object *eobj
        cdef int layer, hotx, hoty
        eobj = NULL
        ecore_evas_cursor_get(self.obj, &eobj, &layer, &hotx, &hoty)
        if eobj == NULL:
            eo = None
        else:
            eo = evas.c_evas._Object_from_instance(<long>eobj)
        return (eo, layer, hotx, hoty)

    def cursor_set(self, cursor, int layer=0, int hotx=0, int hoty=0):
        """Set window cursor to some file (image) or any Evas Object.

        @parm: B{cursor} either filename or L{evas.c_evas.Object}
        @parm: B{layer}
        @parm: B{hotx} horizontal hotspot
        @parm: B{hoty} vertical hotspot
        """
        if cursor is None:
            ecore_evas_cursor_set(self.obj, NULL, layer, hotx, hoty)
        elif isinstance(cursor, str):
            ecore_evas_cursor_set(self.obj, cursor, layer, hotx, hoty)
        elif isinstance(cursor, evas.c_evas.Object):
            self.object_cursor_set(cursor, layer, hotx, hoty)
        else:
            raise TypeError("cursor must be 'str', 'None' or "
                            "'evas.c_evas.Object'")

    def object_cursor_set(self, c_evas.Object cursor, int layer=0,
                          int hotx=0, int hoty=0):
        """Set window cursor to any Evas Object.

        @parm: B{cursor} an L{evas.c_evas.Object} to use as cursor.
        @parm: B{layer}
        @parm: B{hotx}
        @parm: B{hoty}
        """
        ecore_evas_object_cursor_set(self.obj, cursor.obj, layer, hotx, hoty)

    def move(self, int x, int y):
        "Move window to given position."
        ecore_evas_move(self.obj, x, y)

    def pos_set(self, int x, int y):
        "Same as L{move()}"
        ecore_evas_move(self.obj, x, y)

    def pos_get(self):
        "@rtype: tuple of int"
        cdef int x, y
        ecore_evas_geometry_get(self.obj, &x, &y, NULL, NULL)
        return (x, y)

    property pos:
        def __set__(self, spec):
            self.pos_set(*spec)

        def __get__(self):
            return self.pos_get()

    def managed_move(self, int x, int y):
        "This sets the managed geometry position of the window."
        ecore_evas_managed_move(self.obj, x, y)

    def resize(self, int w, int h):
        "Resize window to given size."
        ecore_evas_resize(self.obj, w, h)

    def size_set(self, int w, int h):
        "Same as L{resize()}"
        ecore_evas_resize(self.obj, w, h)

    def size_get(self):
        "@rtype: tuple of int"
        cdef int w, h
        ecore_evas_geometry_get(self.obj, NULL, NULL, &w, &h)
        return (w, h)

    property size:
        def __set__(self, spec):
            self.size_set(*spec)

        def __get__(self):
            return self.size_get()

    def move_resize(self, int x, int y, int w, int h):
        "Move and resize the window."
        ecore_evas_move_resize(self.obj, x, y, w, h)

    def geometry_set(self, int x, int y, int w, int h):
        "Same as L{move_resize()}."
        ecore_evas_move_resize(self.obj, x, y, w, h)

    def geometry_get(self):
        "@rtype: tuple of int"
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
        """Get rotation angle, in degrees.

        @rtype: int
        """
        return ecore_evas_rotation_get(self.obj)

    property rotation:
        def __get__(self):
            return self.rotation_get()

        def __set__(self, int value):
            self.rotation_set(value)


    def shaped_set(self, int shaped):
        """Set whether the object is shaped or not.

        If true, EcoreEvas will be transparent in areas without any
        L{evas.c_evas.Object}.
        """
        ecore_evas_shaped_set(self.obj, shaped)

    def shaped_get(self):
        """Get whether the object is shaped or not.

        A shaped EcoreEvas will be transparent in areas without any
        L{evas.c_evas.Object}.

        @rtype: bool
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
        """Get whether the object has an alpha channel or not.

        @rtype: bool
        """
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
        "@rtype: bool"
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

    def activate(self):
        ecore_evas_activate(self.obj)

    def title_set(self, char *t):
        ecore_evas_title_set(self.obj, t)

    def title_get(self):
        "@rtype: str"
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
        "Set name and class for this window (WM_CLASS)"
        ecore_evas_name_class_set(self.obj, n, c)

    def name_class_get(self):
        "@rtype: tuple of str"
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
        "@rtype: tuple of int"
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
        "@rtype: tuple of int"
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
        "@rtype: tuple of int"
        cdef int w, h
        ecore_evas_size_base_get(self.obj, &w, &h)
        return (w, h)

    property size_base:
        def __get__(self):
            return self.size_base_get()

        def __set__(self, spec):
            self.size_base_set(*spec)

    def size_step_set(self, int w, int h):
        """This limits the size of window to always being an integer
        multiple of the step size.
        """
        ecore_evas_size_step_set(self.obj, w, h)

    def size_step_get(self):
        "@rtype: tuple of int"
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
        "@rtype: int"
        return ecore_evas_layer_get(self.obj)

    property layer:
        def __get__(self):
            return self.layer_get()

        def __set__(self, int value):
            self.layer_set(value)

    def focus_set(self, int on):
        ecore_evas_focus_set(self.obj, on)

    def focus_get(self):
        "@rtype: bool"
        return bool(ecore_evas_focus_get(self.obj))

    property focus:
        def __get__(self):
            return self.focus_get()

        def __set__(self, int value):
            self.focus_set(value)

    def iconified_set(self, int on):
        ecore_evas_iconified_set(self.obj, on)

    def iconified_get(self):
        "@rtype: bool"
        return bool(ecore_evas_iconified_get(self.obj))

    property iconified:
        def __get__(self):
            return self.iconified_get()

        def __set__(self, int value):
            self.iconified_set(value)

    def borderless_set(self, int on):
        ecore_evas_borderless_set(self.obj, on)

    def borderless_get(self):
        "@rtype: bool"
        return bool(ecore_evas_borderless_get(self.obj))

    property borderless:
        def __get__(self):
            return self.borderless_get()

        def __set__(self, int value):
            self.borderless_set(value)

    def override_set(self, int on):
        ecore_evas_override_set(self.obj, on)

    def override_get(self):
        "@rtype: bool"
        return bool(ecore_evas_override_get(self.obj))

    property override:
        def __get__(self):
            return self.override_get()

        def __set__(self, int value):
            self.override_set(value)


    def maximized_set(self, int on):
        ecore_evas_maximized_set(self.obj, on)

    def maximized_get(self):
        "@rtype: bool"
        return bool(ecore_evas_maximized_get(self.obj))

    property maximized:
        def __get__(self):
            return self.maximized_get()

        def __set__(self, int value):
            self.maximized_set(value)

    def fullscreen_get(self):
        return bool(ecore_evas_fullscreen_get(self.obj))

    def fullscreen_set(self, int spec):
        "@rtype: bool"
        ecore_evas_fullscreen_set(self.obj, spec)

    property fullscreen:
        def __get__(self):
            return self.fullscreen_get()

        def __set__(self, int spec):
            self.fullscreen_set(spec)

    def avoid_damage_set(self, int damage_type):
        ecore_evas_avoid_damage_set(self.obj,
                                    <Ecore_Evas_Avoid_Damage_Type>damage_type)

    def avoid_damage_get(self):
        "@rtype: int"
        return <int>ecore_evas_avoid_damage_get(self.obj)

    property avoid_damage:
        def __get__(self):
            return self.avoid_damage_get()

        def __set__(self, int value):
            self.avoid_damage_set(value)

    def withdrawn_set(self, int on):
        ecore_evas_withdrawn_set(self.obj, on)

    def withdrawn_get(self):
        "@rtype: bool"
        return bool(ecore_evas_withdrawn_get(self.obj))

    property withdrawn:
        def __get__(self):
            return self.withdrawn_get()

        def __set__(self, int value):
            self.withdrawn_set(value)

    def sticky_set(self, int on):
        ecore_evas_sticky_set(self.obj, on)

    def sticky_get(self):
        "@rtype: bool"
        return bool(ecore_evas_sticky_get(self.obj))

    property sticky:
        def __get__(self):
            return self.sticky_get()

        def __set__(self, int value):
            self.sticky_set(value)

    def ignore_events_set(self, int on):
        ecore_evas_ignore_events_set(self.obj, on)

    def ignore_events_get(self):
        "@rtype: bool"
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
        """Set function to be called when window is resized.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._resize_cb = func
        if func is None:
            ecore_evas_callback_resize_set(self.obj, NULL)
        else:
            ecore_evas_callback_resize_set(self.obj, resize_cb)

    def callback_resize_get(self):
        "@rtype: callable"
        return self._resize_cb

    property callback_resize:
        def __get__(self):
            return self.callback_resize_get()

        def __set__(self, func):
            self.callback_resize_set(func)

        def __del__(self):
            self.callback_resize_set(None)

    def callback_move_set(self, func):
        """Set function to be called when window is moved.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._move_cb = func
        if func is None:
            ecore_evas_callback_move_set(self.obj, NULL)
        else:
            ecore_evas_callback_move_set(self.obj, move_cb)

    def callback_move_get(self):
        "@rtype: callable"
        return self._move_cb

    property callback_move:
        def __get__(self):
            return self.callback_move_get()

        def __set__(self, func):
            self.callback_move_set(func)

        def __del__(self):
            self.callback_move_set(None)

    def callback_show_set(self, func):
        """Set function to be called when window is shown.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._show_cb = func
        if func is None:
            ecore_evas_callback_show_set(self.obj, NULL)
        else:
            ecore_evas_callback_show_set(self.obj, show_cb)

    def callback_show_get(self):
        "@rtype: callable"
        return self._show_cb

    property callback_show:
        def __get__(self):
            return self.callback_show_get()

        def __set__(self, func):
            self.callback_show_set(func)

        def __del__(self):
            self.callback_show_set(None)

    def callback_hide_set(self, func):
        """Set function to be called when window is hidden.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._hide_cb = func
        if func is None:
            ecore_evas_callback_hide_set(self.obj, NULL)
        else:
            ecore_evas_callback_hide_set(self.obj, hide_cb)

    def callback_hide_get(self):
        "@rtype: callable"
        return self._hide_cb

    property callback_hide:
        def __get__(self):
            return self.callback_hide_get()

        def __set__(self, func):
            self.callback_hide_set(func)

        def __del__(self):
            self.callback_hide_set(None)

    def callback_delete_request_set(self, func):
        """Set function to be called when window gets a delete request.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._delete_request_cb = func
        if func is None:
            ecore_evas_callback_delete_request_set(self.obj, NULL)
        else:
            ecore_evas_callback_delete_request_set(self.obj, delete_request_cb)

    def callback_delete_request_get(self):
        "@rtype: callable"
        return self._delete_request_cb

    property callback_delete_request:
        def __get__(self):
            return self.callback_delete_request_get()

        def __set__(self, func):
            self.callback_delete_request_set(func)

        def __del__(self):
            self.callback_delete_request_set(None)

    def callback_destroy_set(self, func):
        """Set function to be called when window is destroyed.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._destroy_cb = func
        if func is None:
            ecore_evas_callback_destroy_set(self.obj, NULL)
        else:
            ecore_evas_callback_destroy_set(self.obj, destroy_cb)

    def callback_destroy_get(self):
        "@rtype: callable"
        return self._destroy_cb

    property callback_destroy:
        def __get__(self):
            return self.callback_destroy_get()

        def __set__(self, func):
            self.callback_destroy_set(func)

        def __del__(self):
            self.callback_destroy_set(None)

    def callback_focus_in_set(self, func):
        """Set function to be called when window gets focus.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._focus_in_cb = func
        if func is None:
            ecore_evas_callback_focus_in_set(self.obj, NULL)
        else:
            ecore_evas_callback_focus_in_set(self.obj, focus_in_cb)

    def callback_focus_in_get(self):
        "@rtype: callable"
        return self._focus_in_cb

    property callback_focus_in:
        def __get__(self):
            return self.callback_focus_in_get()

        def __set__(self, func):
            self.callback_focus_in_set(func)

        def __del__(self):
            self.callback_focus_in_set(None)

    def callback_focus_out_set(self, func):
        """Set function to be called when window is loses focus.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._focus_out_cb = func
        if func is None:
            ecore_evas_callback_focus_out_set(self.obj, NULL)
        else:
            ecore_evas_callback_focus_out_set(self.obj, focus_out_cb)

    def callback_focus_out_get(self):
        "@rtype: callable"
        return self._focus_out_cb

    property callback_focus_out:
        def __get__(self):
            return self.callback_focus_out_get()

        def __set__(self, func):
            self.callback_focus_out_set(func)

        def __del__(self):
            self.callback_focus_out_set(None)

    def callback_sticky_set(self, func):
        """Set function to be called when window becomes sticky.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._sticky_cb = func
        if func is None:
            ecore_evas_callback_sticky_set(self.obj, NULL)
        else:
            ecore_evas_callback_sticky_set(self.obj, sticky_cb)

    def callback_sticky_get(self):
        "@rtype: callable"
        return self._sticky_cb

    property callback_sticky:
        def __get__(self):
            return self.callback_sticky_get()

        def __set__(self, func):
            self.callback_sticky_set(func)

        def __del__(self):
            self.callback_sticky_set(None)

    def callback_unsticky_set(self, func):
        """Set function to be called when window becomes un-sticky.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._unsticky_cb = func
        if func is None:
            ecore_evas_callback_unsticky_set(self.obj, NULL)
        else:
            ecore_evas_callback_unsticky_set(self.obj, unsticky_cb)

    def callback_unsticky_get(self):
        "@rtype: callable"
        return self._unsticky_cb

    property callback_unsticky:
        def __get__(self):
            return self.callback_unsticky_get()

        def __set__(self, func):
            self.callback_unsticky_set(func)

        def __del__(self):
            self.callback_unsticky_set(None)

    def callback_mouse_in_set(self, func):
        """Set function to be called when mouse enters window.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._mouse_in_cb = func
        if func is None:
            ecore_evas_callback_mouse_in_set(self.obj, NULL)
        else:
            ecore_evas_callback_mouse_in_set(self.obj, mouse_in_cb)

    def callback_mouse_in_get(self):
        "@rtype: callable"
        return self._mouse_in_cb

    property callback_mouse_in:
        def __get__(self):
            return self.callback_mouse_in_get()

        def __set__(self, func):
            self.callback_mouse_in_set(func)

        def __del__(self):
            self.callback_mouse_in_set(None)

    def callback_mouse_out_set(self, func):
        """Set function to be called when mouse leaves window.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._mouse_out_cb = func
        if func is None:
            ecore_evas_callback_mouse_out_set(self.obj, NULL)
        else:
            ecore_evas_callback_mouse_out_set(self.obj, mouse_out_cb)

    def callback_mouse_out_get(self):
        "@rtype: callable"
        return self._mouse_out_cb

    property callback_mouse_out:
        def __get__(self):
            return self.callback_mouse_out_get()

        def __set__(self, func):
            self.callback_mouse_out_set(func)

        def __del__(self):
            self.callback_mouse_out_set(None)

    def callback_pre_render_set(self, func):
        """Set function to be called before evas is rendered.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._pre_render_cb = func
        if func is None:
            ecore_evas_callback_pre_render_set(self.obj, NULL)
        else:
            ecore_evas_callback_pre_render_set(self.obj, pre_render_cb)

    def callback_pre_render_get(self):
        "@rtype: callable"
        return self._pre_render_cb

    property callback_pre_render:
        def __get__(self):
            return self.callback_pre_render_get()

        def __set__(self, func):
            self.callback_pre_render_set(func)

        def __del__(self):
            self.callback_pre_render_set(None)

    def callback_post_render_set(self, func):
        """Set function to be called after evas is rendered.

        Signature: C{function(object)}
        """
        if func is not None and not callable(func):
            raise TypeError("func must be callable or None.")
        self._post_render_cb = func
        if func is None:
            ecore_evas_callback_post_render_set(self.obj, NULL)
        else:
            ecore_evas_callback_post_render_set(self.obj, post_render_cb)

    def callback_post_render_get(self):
        "@rtype: callable"
        return self._post_render_cb

    property callback_post_render:
        def __get__(self):
            return self.callback_post_render_get()

        def __set__(self, func):
            self.callback_post_render_set(func)

        def __del__(self):
            self.callback_post_render_set(None)


