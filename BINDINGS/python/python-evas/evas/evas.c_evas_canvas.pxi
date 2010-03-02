# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Caio Marcelo de Oliveira Filho
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

# This file is included verbatim by c_evas.pyx

cdef int _canvas_free_wrapper_resources(Canvas canvas) except 0:
    cdef int i
    for i from 0 <= i < evas_canvas_event_callbacks_len:
        canvas._callbacks[i] = None
    return 1


cdef int _canvas_unregister_callbacks(Canvas canvas) except 0:
    cdef Evas *e
    cdef Evas_Event_Cb cb
    e = canvas.obj
    if e != NULL:
        for i, lst in enumerate(canvas._callbacks):
            if lst is not None:
                cb = evas_canvas_event_callbacks[i]
                evas_event_callback_del(e, i, cb)
    return 1


cdef _canvas_add_callback_to_list(Canvas canvas, int type, func, args, kargs):
    if type < 0 or type >= evas_canvas_event_callbacks_len:
        raise ValueError("Invalid callback type")

    r = (func, args, kargs)
    lst = canvas._callbacks[type]
    if lst is not None:
        lst.append(r)
        return False
    else:
        canvas._callbacks[type] = [r]
        return True


cdef _canvas_del_callback_from_list(Canvas canvas, int type, func):
    if type < 0 or type >= evas_canvas_event_callbacks_len:
        raise ValueError("Invalid callback type")

    lst = canvas._callbacks[type]
    if not lst:
        raise ValueError("Callback %s was not registered with type %d" %
                         (func, type))

    i = None
    for i, r in enumerate(lst):
        if func == r[0]:
            break
    else:
        raise ValueError("Callback %s was not registered with type %d" %
                         (func, type))

    lst.pop(i)
    if len(lst) == 0:
        canvas._callbacks[type] = None
        return True
    else:
        return False


cdef public class Canvas [object PyEvasCanvas, type PyEvasCanvas_Type]:
    """Evas Canvas.

    Canvas is the base drawing area and scene manager, it should have
    a number of objects (or actors) that will be managed. Object state
    is monitored and redraw is optimized based on changes.

    @ivar rect: L{Rect} describing object geometry, for easy manipulation.
          Changing this L{Rect} will not affect current geometry, you have
          to set it again to have this behavior.

    @attention: Canvas must be associated with an Input/Output system in
                order to be functional. So far it's impossible to do this
                association directly from Python, so you should create
                Canvas indirectly using C{ecore.evas} classes, like
                L{ecore.evas.SoftwareX11}.
    @see: L{ecore.evas.SoftwareX11}, L{ecore.evas.Buffer}, ...

    @group Most used: size_get, size_set, size, rect
    @group Factories: Rectangle, Line, Image, FilledImage, Gradient,
       Polygon, Text, Textblock
    @group Children manipulation: top_at_xy_get, top_at_pointer_get,
           top_in_rectangle_get, objects_at_xy_get, objects_in_rectangle_get,
           top_get, top, bottom_get, bottom, focus_get, focus, object_name_find
    @group Coordinates: viewport_set, viewport_get, viewport,
           coord_screen_x_to_world, coord_screen_y_to_world,
           coord_world_x_to_screen, coord_world_y_to_screen
    @group Pointer: pointer_output_xy_get, pointer_output_xy,
           pointer_canvas_xy_get, pointer_canvas_xy,
           pointer_button_down_mask_get, pointer_button_down_mask,
           pointer_inside_get, pointer_inside
    @group Image settings: image_cache_flush, image_cache_reload,
           image_cache_set, image_cache_get, image_cache
    @group Font settings: font_cache_flush, font_cache_get, font_cache_set,
           font_cache, font_path_clear, font_path_append, font_path_prepend,
           font_path_list, font_available_list, font_hinting_can_hint,
           font_hinting_set, font_hinting_get, font_hinting
    @group Event feeding: feed_hold
    @group Often unused: damage_rectangle_add, obscured_rectangle_add,
           obscured_clear, render_updates, render, norender, freeze_get,
           output_method_set, output_method_get, output_method
    """
    def __new__(self, *a, **ka):
        self.obj = NULL
        self._callbacks = [None] * evas_canvas_event_callbacks_len

    def __init__(self, method=None, size=None, viewport=None):
        self._new_evas()
        if method:
            self.output_method_set(method)
        if size:
            self.size_set(*size)
        if viewport:
            self.viewport_set(*viewport)

    def __dealloc__(self):
        if self.obj:
            _canvas_unregister_callbacks(self)
            Canvas_forget(<long>self.obj)
            evas_free(self.obj)
            self.obj = NULL
        self._callbacks = None

    cdef int _set_obj(self, Evas *obj) except 0:
        assert self.obj == NULL, "Object must be clean"
        self.obj = obj
        Canvas_remember(<long>self.obj, self)
        _canvas_unregister_callbacks(self)
        _canvas_free_wrapper_resources(self)
        return 1

    cdef int _unset_obj(self) except 0:
        Canvas_forget(<long>self.obj)
        self.obj = NULL
        return 1

    def _new_evas(self):
        """Creates an internal (wrapped) 'Evas*' for this object.

        If this Python wrapper is shallow then it allocate a new
        'Evas*' using C{evas_new()}

        @warning: internal.
        """
        if self.obj == NULL:
            self._set_obj(evas_new())

    def __str__(self):
        w, h = self.size_get()
        return "%s(size=(%d, %d), method=%r)" % \
               (self.__class__.__name__, w, h, self.output_method_get())

    def __repr__(self):
        w, h = self.size_get()
        return ("%s(%#x, refcount=%d, Evas=%#x, size=(%d, %d), "
                "method=%r)") % \
               (self.__class__.__name__, <unsigned long><void *>self,
                PY_REFCOUNT(self), <unsigned long>self.obj,
                w, h, self.output_method_get())

    def output_method_set(self, method):
        """Set canvas render method, can be either a name or id.

        @parm: method
        """
        cdef int engine_id

        if isinstance(method, (int, long)):
            engine_id = method
        elif isinstance(method, basestring):
            engine_id = evas_render_method_lookup(method)
        else:
            raise TypeError("method must be integer or string")

        evas_output_method_set(self.obj, engine_id)

    def output_method_get(self):
        """Returns the id of the current output method, or 0 if error happened.

        @rtype: int
        """
        return evas_output_method_get(self.obj)

    property output_method:
        def __set__(self, method):
            self.output_method_set(method)

        def __get__(self):
            return self.output_method_get()

    def engine_info_set(self, unsigned long ptr):
        """Set the engine information pointer.

        Note that given value is a pointer, usually acquired with L{info_get()}
        and is totally engine and platform dependent.

        This call is very low level and is meant for extension to use,
        they usually do the machinery in C and just handle pointers as
        integers.

        If in doubt, don't mess with it. Use ecore.evas instead, it's
        the way to go for regular uses.
        """
        evas_engine_info_set(self.obj, <void *>ptr)

    def engine_info_get(self):
        """Get the engine information pointer.

        Note that given value is a pointer and is totally engine and
        platform dependent.

        This call is very low level and is meant for extension to use,
        they usually do the machinery in C and just handle pointers as
        integers.

        If in doubt, don't mess with it. Use ecore.evas instead, it's
        the way to go for regular uses.

        @return: pointer as integer (unsigned long).
        """
        return <unsigned long><void *>evas_engine_info_get(self.obj)

    property engine_info:
        def __set__(self, ptr):
            self.engine_info_set(ptr)

        def __get__(self):
            return self.engine_info_get()

    def size_set(self, int w, int h):
        """Set canvas size.

        @parm: w
        @parm: h
        """
        evas_output_size_set(self.obj, w, h)

    def size_get(self):
        """@rtype: tuple of int"""
        cdef int w, h
        evas_output_size_get(self.obj, &w, &h)
        return (w, h)

    property size:
        def __set__(self, spec):
            self.size_set(*spec)

        def __get__(self):
            return self.size_get()

    property rect:
        def __set__(self, spec):
            cdef Rect r
            r = Rect(spec)
            self.size_set(r.w, r.h)

        def __get__(self):
            cdef int w, h
            w, h = self.size_get()
            return Rect(0, 0, w, h)

    def viewport_set(self, int x, int y, int w, int h):
        """Sets the output viewport of the given evas in evas units.

        The output viewport is the area of the evas that will be visible to
        the viewer. The viewport will be stretched to fit the output target
        of the evas when rendering is performed.

        @note: The coordinate values do not have to map 1-to-1 with the output
               target.  However, it is generally advised that it is done for
               ease of use.
        """

        evas_output_viewport_set(self.obj, x, y, w, h)

    def viewport_get(self):
        "@rtype: list of int"
        cdef int x, y, w, h
        evas_output_viewport_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    property viewport:
        def __set__(self, spec):
            self.viewport_set(*spec)

        def __get__(self):
            return self.viewport_get()

    def coord_screen_x_to_world(self, int x):
        "@rtype: int"
        return evas_coord_screen_x_to_world(self.obj, x)

    def coord_screen_y_to_world(self, int y):
        "@rtype: int"
        return evas_coord_screen_y_to_world(self.obj, y)

    def coord_world_x_to_screen(self, int x):
        "@rtype: int"
        return evas_coord_world_x_to_screen(self.obj, x)

    def coord_world_y_to_screen(self, int y):
        "@rtype: int"
        return evas_coord_world_y_to_screen(self.obj, y)

    def pointer_output_xy_get(self):
        """Returns the pointer's (x, y) relative to output."""
        cdef int x, y
        evas_pointer_output_xy_get(self.obj, &x, &y)
        return (x, y)

    property pointer_output_xy:
        def __get__(self):
            return self.pointer_output_xy_get()

    def pointer_canvas_xy_get(self):
        """Returns the pointer's (x, y) relative to canvas."""
        cdef int x, y
        evas_pointer_canvas_xy_get(self.obj, &x, &y)
        return (x, y)

    property pointer_canvas_xy:
        def __get__(self):
            return self.pointer_canvas_xy_get()

    def pointer_button_down_mask_get(self):
        """Returns a bitmask with the mouse buttons currently pressed set to 1.

        The least significant bit corresponds to the first mouse button
        (button 1) and the most significant bit corresponds to the last mouse
        button (button 32).

        @rtype: int
        """
        return evas_pointer_button_down_mask_get(self.obj)

    property pointer_button_down_mask:
        def __get__(self):
            return self.pointer_button_down_mask_get()

    def pointer_inside_get(self):
        """Returns whether the mouse pointer is logically inside the canvas.

        @rtype: bool
        """
        return bool(evas_pointer_inside_get(self.obj))

    property pointer_inside:
        def __get__(self):
            return self.pointer_inside_get()

    def top_at_xy_get(self, int x, int y,
                      include_pass_events_objects=False,
                      include_hidden_objects=False):
        """Get the topmost object at (x, y).

        @parm: B{x}
        @parm: B{y}
        @parm: B{include_pass_events_objects} if to include objects passing events.
        @parm: B{include_hidden_objects} if to include hidden objects.

        @return: child object.
        @rtype: L{Object}
        """
        cdef int ip, ih
        cdef Evas_Object *o
        ip = include_pass_events_objects
        ih = include_hidden_objects
        o = evas_object_top_at_xy_get(self.obj, x, y, ip, ih)
        return Object_from_instance(o)

    def top_at_pointer_get(self):
        """Get the topmost object at pointer position.

        @return: child object.
        @rtype: L{Object}
        """
        cdef Evas_Object *o
        o = evas_object_top_at_pointer_get(self.obj)
        return Object_from_instance(o)

    def top_in_rectangle_get(self, int x, int y, int w, int h,
                             include_pass_events_objects=False,
                             include_hidden_objects=False):
        """Get the topmost object at given geometry.

        @parm: B{x}
        @parm: B{y}
        @parm: B{w}
        @parm: B{h}
        @parm: B{include_pass_events_objects} if to include objects passing events.
        @parm: B{include_hidden_objects} if to include hidden objects.

        @return: child object.
        @rtype: L{Object}
        """
        cdef int ip, ih
        cdef Evas_Object *o
        ip = include_pass_events_objects
        ih = include_hidden_objects
        o = evas_object_top_in_rectangle_get(self.obj, x, y, w, h, ip, ih)
        return Object_from_instance(o)

    def objects_at_xy_get(self, int x, int y,
                          include_pass_events_objects=False,
                          include_hidden_objects=False):
        """Get all children at (x, y).

        @parm: B{x}
        @parm: B{y}
        @parm: B{include_pass_events_objects} if to include objects passing events.
        @parm: B{include_hidden_objects} if to include hidden objects.

        @return: children objects.
        @rtype: list of L{Object}
        """
        cdef Eina_List *objs, *itr
        cdef int ip, ih
        cdef Evas_Object *o
        ip = include_pass_events_objects
        ih = include_hidden_objects
        objs = evas_objects_at_xy_get(self.obj, x, y, ip, ih)
        lst = []
        itr = objs
        while itr != NULL:
            o = <Evas_Object*>itr.data
            lst.append(Object_from_instance(o))
            itr = itr.next
        eina_list_free(objs)
        return lst

    def objects_in_rectangle_get(self, int x, int y, int w, int h,
                                 include_pass_events_objects=False,
                                 include_hidden_objects=False):
        """Get all children at given geometry.

        @parm: B{x}
        @parm: B{y}
        @parm: B{w}
        @parm: B{h}
        @parm: B{include_pass_events_objects} if to include objects passing events.
        @parm: B{include_hidden_objects} if to include hidden objects.

        @return: children objects.
        @rtype: list of L{Object}
        """
        cdef Eina_List *objs, *itr
        cdef int ip, ih
        cdef Evas_Object *o
        ip = include_pass_events_objects
        ih = include_hidden_objects
        objs = evas_objects_in_rectangle_get(self.obj, x, y, w, h, ip, ih)
        lst = []
        itr = objs
        while itr != NULL:
            o = <Evas_Object*>itr.data
            lst.append(Object_from_instance(o))
            itr = itr.next
        eina_list_free(objs)
        return lst

    def damage_rectangle_add(self, int x, int y, int w, int h):
        evas_damage_rectangle_add(self.obj, x, y, w, h)

    def obscured_rectangle_add(self, int x, int y, int w, int h):
        evas_obscured_rectangle_add(self.obj, x, y, w, h)

    def obscured_clear(self):
        evas_obscured_clear(self.obj)

    def render_updates(self):
        cdef Eina_List *lst

        lst = evas_render_updates(self.obj)
        evas_render_updates_free(lst)

    def render(self):
        "Force canvas to redraw pending updates."
        evas_render(self.obj)

    def norender(self):
        evas_norender(self.obj)

    def top_get(self):
        "@rtype: L{Object}"
        cdef Evas_Object *other
        other = evas_object_top_get(self.obj)
        return Object_from_instance(other)

    property top:
        def __get__(self):
            return self.top_get()

    def bottom_get(self):
        "@rtype: L{Object}"
        cdef Evas_Object *other
        other = evas_object_bottom_get(self.obj)
        return Object_from_instance(other)

    property bottom:
        def __get__(self):
            return self.bottom_get()

    def focus_get(self):
        "@rtype: L{Object}"
        cdef Evas_Object *o
        o = evas_focus_get(self.obj)
        return Object_from_instance(o)

    property focus:
        def __get__(self):
            return self.focus_get()

    def object_name_find(self, char *name):
        """Find object by name.
        @parm: name
        @rtype: L{Object}
        """
        cdef Evas_Object *other
        other = evas_object_name_find(self.obj, name)
        return Object_from_instance(other)

    def image_cache_flush(self):
        evas_image_cache_flush(self.obj)

    def image_cache_reload(self):
        evas_image_cache_reload(self.obj)

    def image_cache_set(self, int size):
        evas_image_cache_set(self.obj, size)

    def image_cache_get(self):
        "@rtype: int"
        return evas_image_cache_get(self.obj)

    property image_cache:
        def __get__(self):
            return self.image_cache_get()

        def __set__(self, int value):
            self.image_cache_set(value)

    def font_cache_flush(self):
        evas_font_cache_flush(self.obj)

    def font_cache_get(self):
        "@rtype: int"
        return evas_font_cache_get(self.obj)

    def font_cache_set(self, int value):
        evas_font_cache_set(self.obj, value)

    property font_cache:
        def __get__(self):
            return self.font_cache_get()

        def __set__(self, int value):
            self.font_cache_set(value)

    def font_path_clear(self):
        evas_font_path_clear(self.obj)

    def font_path_append(self, char *path):
        evas_font_path_append(self.obj, path)

    def font_path_prepend(self, char *path):
        evas_font_path_prepend(self.obj, path)

    def font_path_list(self):
        "@rtype: list of str"
        cdef Eina_List *itr
        lst = []
        itr = evas_font_path_list(self.obj)
        while itr != NULL:
            lst.append(<char*>itr.data)
            itr = itr.next
        return lst

    def font_available_list(self):
        "@rtype: list of str"
        cdef void *p
        cdef Eina_List *itr, *head
        lst = []
        p = <void*>evas_font_available_list(self.obj) # avoid warning
        head = <Eina_List*>p
        itr = head
        while itr != NULL:
            lst.append(<char*>itr.data)
            itr = itr.next
        evas_font_available_list_free(self.obj, head)
        return lst

    def font_hinting_can_hint(self, int flags):
        "@rtype: bool"
        return bool(evas_font_hinting_can_hint(self.obj,
                                               <Evas_Font_Hinting_Flags>flags))

    def font_hinting_set(self, int flags):
        """@parm: B{flags} one of EVAS_FONT_HINTING_NONE,
                EVAS_FONT_HINTING_AUTO or EVAS_FONT_HINTING_BYTECODE
        """
        evas_font_hinting_set(self.obj, <Evas_Font_Hinting_Flags>flags)

    def font_hinting_get(self):
        "@rtype: int"
        return <int>evas_font_hinting_get(self.obj)

    property font_hinting:
        def __get__(self):
            return self.font_hinting_get()

        def __set__(self, int value):
            self.font_hinting_set(value)

    def freeze(self):
        "Freeze event processing"
        evas_event_freeze(self.obj)

    def thaw(self):
        "Thaw (unfreeze) event processing"
        evas_event_thaw(self.obj)

    def freeze_get(self):
        "@rtype: int"
        return evas_event_freeze_get(self.obj)

    def key_modifier_is_set(self, modifier):
        "@rtype: bool"
        return bool(evas_key_modifier_is_set(evas_key_modifier_get(self.obj),
                                             modifier))

    def event_callback_add(self, int type, func, *args, **kargs):
        """Add a new callback for the given event.

        @parm: B{type} an integer with event type code, like
               C{EVAS_CALLBACK_CANVAS_FOCUS_IN},
               C{EVAS_CALLBACK_RENDER_FLUSH_PRE} and other
               C{EVAS_CALLBACK_*} constants.
        @parm: B{func} function to call back, this function will have one of
               the following signatures:
                - C{function(object, event, *args, **kargs)}
                - C{function(object, *args, **kargs)}
               The former is used by events that provide more data
               (none so far), while the second is used by events
               without. Parameters given at the end of
               C{event_callback_add()} will be given to the callback.
               Note that the object passed to the callback in B{event}
               parameter will only be valid during the callback, using
               it after callback returns will raise an ValueError.

        @raise ValueError: if B{type} is unknown.
        @raise TypeError: if B{func} is not callable.
        """
        cdef Evas_Event_Cb cb

        if not callable(func):
            raise TypeError("func must be callable")

        if _canvas_add_callback_to_list(self, type, func, args, kargs):
            cb = evas_canvas_event_callbacks[type]
            evas_event_callback_add(self.obj, type, cb, <void*>self)

    def event_callback_del(self, int type, func):
        """Remove callback for the given event.

        @parm: B{type} an integer with event type code.
        @parm: B{func} function used with L{event_callback_add()}.
        @precond: B{type} and B{func} must be used as parameter for
           L{event_callback_add()}.

        @raise ValueError: if B{type} is unknown or if there was no
           B{func} connected with this type.
        """
        cdef Evas_Event_Cb cb
        if _canvas_del_callback_from_list(self, type, func):
            cb = evas_canvas_event_callbacks[type]
            evas_event_callback_del(self.obj, type, cb)

    def on_canvas_focus_in_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_CANVAS_FOCUS_IN, ...)

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_CANVAS_FOCUS_IN, func, *a, **k)

    def on_canvas_focus_in_del(self, func):
        "Same as event_callback_del(EVAS_CALLBACK_CANVAS_FOCUS_IN, ...)"
        self.event_callback_del(EVAS_CALLBACK_CANVAS_FOCUS_IN, func)

    def on_canvas_focus_out_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_CANVAS_FOCUS_OUT, ...)

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_CANVAS_FOCUS_OUT, func, *a, **k)

    def on_canvas_focus_out_del(self, func):
        "Same as event_callback_del(EVAS_CALLBACK_CANVAS_FOCUS_OUT, ...)"
        self.event_callback_del(EVAS_CALLBACK_CANVAS_FOCUS_OUT, func)

    def on_render_flush_pre_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_RENDER_FLUSH_PRE, ...)

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_RENDER_FLUSH_PRE, func, *a, **k)

    def on_render_flush_pre_del(self, func):
        "Same as event_callback_del(EVAS_CALLBACK_RENDER_FLUSH_PRE, ...)"
        self.event_callback_del(EVAS_CALLBACK_RENDER_FLUSH_PRE, func)

    def on_render_flush_post_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_RENDER_FLUSH_POST, ...)

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_RENDER_FLUSH_POST, func, *a, **k)

    def on_render_flush_post_del(self, func):
        "Same as event_callback_del(EVAS_CALLBACK_RENDER_FLUSH_POST, ...)"
        self.event_callback_del(EVAS_CALLBACK_RENDER_FLUSH_POST, func)

    # Event feeding
    def feed_hold(self, int hold, unsigned int timestamp):
        evas_event_feed_hold(self.obj, hold, timestamp, NULL)

    # Factory
    def Rectangle(self, **kargs):
        """Factory of L{evas.Rectangle} associated with this canvas.
        @rtype: L{Rectangle<evas.Rectangle>}
        """
        return Rectangle(self, **kargs)

    def Line(self, **kargs):
        """Factory of L{evas.Line} associated with this canvas.
        @rtype: L{Line<evas.Line>}
        """
        return Line(self, **kargs)

    def Image(self, **kargs):
        """Factory of L{evas.Image} associated with this canvas.
        @rtype: L{Image<evas.Image>}
        """
        return Image(self, **kargs)

    def FilledImage(self, **kargs):
        """Factory of L{evas.FilledImage} associated with this canvas.
        @rtype: L{FilledImage<evas.FilledImage>}
        """
        return FilledImage(self, **kargs)

    def Gradient(self, **kargs):
        """Factory of L{evas.Gradient} associated with this canvas.
        @rtype: L{Gradient<evas.Gradient>}
        """
        return Gradient(self, **kargs)

    def Polygon(self, **kargs):
        """Factory of L{evas.Polygon} associated with this canvas.
        @rtype: L{Polygon<evas.Polygon>}
        """
        return Polygon(self, **kargs)

    def Text(self, **kargs):
        """Factory of L{evas.Text} associated with this canvas.
        @rtype: L{Text<evas.Text>}
        """
        return Text(self, **kargs)

    def Textblock(self, **kargs):
        """Factory of L{evas.Textblock} associated with this canvas.
        @rtype: L{Textblock<evas.Textblock>}
        """
        return Textblock(self, **kargs)
