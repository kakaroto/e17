# This file is included verbatim by c_evas.pyx

cdef void obj_free_cb(void *data, Evas *e, Evas_Object *obj, void *event_info):
    cdef Object self
    self = <Object>data
    self.obj = NULL
    self._evas = <Canvas>None
    python.Py_DECREF(self)


cdef public class Object [object PyEvasObject, type PyEvasObject_Type]:
    def __new__(self, Canvas evas):
        self.obj = NULL
        self._evas = evas
        self._data = dict()
        self._callbacks = [None] * evas_event_callbacks_len

    def __str__(self):
        return "%s(0x%x, type=%r, refcount=%d, Evas_Object=0x%x)" % \
               (self.__class__.__name__, <unsigned long>self,
                self.type_get(), python.REFCOUNT(self),
                <unsigned long>self.obj)

    cdef int _unset_obj(self) except 0:
        assert self.obj != NULL, "Object must wrap something"
        assert evas_object_data_del(self.obj, "python-evas") == \
               <void *>self, "Object wrapped should refer to self"
        evas_object_event_callback_del(self.obj, EVAS_CALLBACK_FREE,
                                       obj_free_cb)
        self.obj = NULL
        python.Py_DECREF(self)
        return 1

    cdef int _set_obj(self, Evas_Object *obj) except 0:
        assert self.obj == NULL, "Object must be clean"
        self.obj = obj
        python.Py_INCREF(self)
        evas_object_data_set(obj, "python-evas", <void *>self)
        evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, obj_free_cb,
                                       <void *>self)
        return 1

    def __dealloc__(self):
        cdef void *data
        cdef Evas_Object *obj
        cdef evas_event_callback_t cb

        #print "==>DEALLOC: type=%s, instance=%s" % (self.type_get(),  self)

        if self.obj != NULL:
            for i, lst in enumerate(self._callbacks):
                if lst is not None:
                    cb = evas_event_callbacks[i]
                    evas_object_event_callback_del(self.obj, i, cb)

        self._data = None
        self._callbacks = None
        obj = self.obj
        if self.obj == NULL:
            return
        self.obj = NULL

        self._evas = <Canvas>None

        data = evas_object_data_get(obj, "python-evas")
        assert data == NULL, "Object must not be wrapped!"
        evas_object_event_callback_del(obj, EVAS_CALLBACK_FREE, obj_free_cb)
        evas_object_del(obj)

    def delete(self):
        if self.obj == NULL:
            raise ValueError("Object already deleted")
        evas_object_del(self.obj)

    def evas_get(self):
        return self._evas

    property evas:
        def __get__(self):
            return self._evas

    property data:
        def __get__(self):
            return self._data

    def type_get(self):
        "Get the Evas object's type"
        if self.obj:
            return evas_object_type_get(self.obj)

    property type:
        def __get__(self):
            return self.type_get()

    def layer_set(self, int l):
        evas_object_layer_set(self.obj, l)

    def layer_get(self):
        return evas_object_layer_get(self.obj)

    property layer:
        def __set__(self, int l):
            evas_object_layer_set(self.obj, l)

        def __get__(self):
            return evas_object_layer_get(self.obj)

    def raise_(self):
        "Raise to the top of its layer."
        evas_object_raise(self.obj)

    def lower(self):
        "Lower @p obj to the bottom of its layer."
        evas_object_lower(self.obj)

    def stack_above(self, Object above):
        evas_object_stack_above(self.obj, above.obj)

    def stack_below(self, Object below):
        evas_object_stack_below(self.obj, below.obj)

    def above_get(self):
        cdef Evas_Object *other
        other = evas_object_above_get(self.obj)
        return Object_from_instance(other)

    property above:
        def __get__(self):
            return self.above_get()

    def below_get(self):
        cdef Evas_Object *other
        other = evas_object_below_get(self.obj)
        return Object_from_instance(other)

    property below:
        def __get__(self):
            return self.below_get()

    def top_get(self):
        return self._evas.top_get()

    def bottom_get(self):
        return self._evas.bottom_get()

    def geometry_get(self):
        cdef int x, y, w, h
        evas_object_geometry_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    def geometry_set(self, int x, int y, int w, int h):
        evas_object_move(self.obj, x, y)
        evas_object_resize(self.obj, w, h)

    property geometry:
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y, w, h
            evas_object_geometry_get(self.obj, &x, &y, &w, &h)
            return (x, y, w, h)

        def __set__(self, spec):
            cdef int x, y, w, h
            x, y, w, h = spec
            evas_object_move(self.obj, x, y)
            evas_object_resize(self.obj, w, h)

    def size_get(self):
        cdef int w, h
        evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
        return (w, h)

    def size_set(self, int w, int h):
        evas_object_resize(self.obj, w, h)

    property size:
        def __get__(self): # replicated to avoid performance hit
            cdef int w, h
            evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
            return (w, h)

        def __set__(self, spec):
            cdef int w, h
            w, h = spec
            evas_object_resize(self.obj, w, h)

    def resize(self, int w, int h):
        evas_object_resize(self.obj, w, h)

    def pos_get(self):
        cdef int x, y
        evas_object_geometry_get(self.obj, &x, &y, NULL, NULL)
        return (x, y)

    def pos_set(self, int x, int y):
        evas_object_move(self.obj, x, y)

    property pos:
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y
            evas_object_geometry_get(self.obj, &x, &y, NULL, NULL)
            return (x, y)

        def __set__(self, spec):
            cdef int x, y
            x, y = spec
            evas_object_move(self.obj, x, y)

    def top_left_get(self):
        cdef int x, y
        evas_object_geometry_get(self.obj, &x, &y, NULL, NULL)
        return (x, y)

    def top_left_set(self, int x, int y):
        evas_object_move(self.obj, x, y)

    property top_left:
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y
            evas_object_geometry_get(self.obj, &x, &y, NULL, NULL)
            return (x, y)

        def __set__(self, spec):
            cdef int x, y
            x, y = spec
            evas_object_move(self.obj, x, y)

    def top_right_get(self):
        cdef int x, y, w
        evas_object_geometry_get(self.obj, &x, &y, &w, NULL)
        return (x + w, y)

    def top_right_set(self, int x, int y):
        evas_object_move(self.obj, x, y)

    property top_right:
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y, w
            evas_object_geometry_get(self.obj, &x, &y, &w, NULL)
            return (x + w, y)

        def __set__(self, spec):
            cdef int x, y, w
            x, y = spec
            evas_object_geometry_get(self.obj, NULL, NULL, &w, NULL)
            evas_object_move(self.obj, x - w, y)

    def bottom_left_get(self):
        cdef int x, y, h
        evas_object_geometry_get(self.obj, &x, &y, NULL, &h)
        return (x, y + h)

    def bottom_left_set(self, int x, int y):
        cdef int h
        evas_object_geometry_get(self.obj, NULL, NULL, NULL, &h)
        evas_object_move(self.obj, x, y - h)

    property bottom_left:
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y, h
            evas_object_geometry_get(self.obj, &x, &y, NULL, &h)
            return (x, y + h)

        def __set__(self, spec):
            cdef int x, y, h
            x, y = spec
            evas_object_geometry_get(self.obj, NULL, NULL, NULL, &h)
            evas_object_move(self.obj, x, y - h)

    def bottom_right_get(self):
        cdef int x, y, w, h
        evas_object_geometry_get(self.obj, &x, &y, &w, &h)
        return (x + w, y + h)

    def bottom_right_set(self, int x, int y):
        cdef int w, h
        evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
        evas_object_move(self.obj, x - w, y - h)

    property bottom_right:
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y, w, h
            evas_object_geometry_get(self.obj, &x, &y, &w, &h)
            return (x + w, y + h)

        def __set__(self, spec):
            cdef int x, y, w, h
            x, y = spec
            evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
            evas_object_move(self.obj, x - w, y - h)

    def center_get(self):
        cdef int x, y, w, h
        evas_object_geometry_get(self.obj, &x, &y, &w, &h)
        return (x + w/2, y + h/2)

    def center_set(self, int x, int y):
        cdef int w, h
        evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
        evas_object_move(self.obj, x - w/2, y - h/2)

    property center:
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y, w, h
            evas_object_geometry_get(self.obj, &x, &y, &w, &h)
            return (x + w/2, y + h/2)

        def __set__(self, spec):
            cdef int x, y, w, h
            x, y = spec
            evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
            evas_object_move(self.obj, x - w/2, y - h/2)

    property rect:
        """Returns a rectangle representing the object's geometry.

        Rectangles have useful operations like clip, clamp, union and
        also provides various attributes like top_left, center_x, ...

        Note that changing rectangle is a snapshot of current state, it
        is not synchronized to the object, so modifying attributes DO NOT
        change the object itself! You must set the Object.rect property
        to update object information.
        """
        def __get__(self):
            cdef int x, y, w, h
            evas_object_geometry_get(self.obj, &x, &y, &w, &h)
            return Rect(x, y, w, h)

        def __set__(self, spec):
            cdef Rect r
            if isinstance(spec, Rect):
                r = spec
            else:
                r = Rect(spec)
            evas_object_move(self.obj, r.x0, r.y0)
            evas_object_resize(self.obj, r._w, r._h)

    def move(self, int x, int y):
        evas_object_move(self.obj, x, y)

    def move_relative(self, int dx, int dy):
        cdef int x, y
        evas_object_geometry_get(self.obj, &x, &y, NULL, NULL)
        evas_object_move(self.obj, x + dx, y + dy)

    def show(self):
        evas_object_show(self.obj)

    def hide(self):
        evas_object_hide(self.obj)

    def visible_get(self):
        return bool(evas_object_visible_get(self.obj))

    def visible_set(self, spec):
        if spec:
            self.show()
        else:
            self.hide()

    property visible:
        def __get__(self):
            return self.visible_get()

        def __set__(self, spec):
            self.visible_set(spec)

    def render_op_get(self):
        return evas_object_render_op_get(self.obj)

    def render_op_set(self, int value):
        evas_object_render_op_set(self.obj, <Evas_Render_Op>value)

    property render_op:
        def __get__(self):
            return self.render_op_get()

        def __set__(self, int value):
            self.render_op_set(value)

    def anti_alias_get(self):
        return bool(evas_object_anti_alias_get(self.obj))

    def anti_alias_set(self, int value):
        evas_object_anti_alias_set(self.obj, value)

    property anti_alias:
        def __get__(self):
            return self.anti_alias_get()

        def __set__(self, int value):
            self.anti_alias_set(value)

    def color_set(self, int r, int g, int b, int a):
        """Set object color using r, g, b, a (uses pre-multiply colorspace!)

        You can convert non-premul to premul color space using
        color_parse(spec, is_premul=False) or using specific functions.
        """
        evas_object_color_set(self.obj, r, g, b, a)

    def color_get(self):
        "Get object color (r, g, b, a) (uses pre-multiply colorspace!)"
        cdef int r, g, b, a
        evas_object_color_get(self.obj, &r, &g, &b, &a)
        return (r, g, b, a)

    property color:
        "Object's (r, g, b, a) color, in pre-multiply colorspace"

        def __get__(self):
            return self.color_get()

        def __set__(self, color):
            self.color_set(*color)

    def color_interpolation_get(self):
        return evas_object_color_interpolation_get(self.obj)

    def color_interpolation_set(self, int value):
        evas_object_color_interpolation_set(self.obj, value)

    property color_interpolation:
        def __get__(self):
            return self.color_interpolation_get()

        def __set__(self, int value):
            self.color_interpolation_set(value)

    def clip_get(self):
        cdef Evas_Object *clip
        clip = evas_object_clip_get(self.obj)
        return Object_from_instance(clip)

    def clip_set(self, value):
        cdef Evas_Object *clip
        cdef Object o
        if value is None:
            evas_object_clip_unset(self.obj)
        elif isinstance(value, Object):
            o = <Object>value
            clip = o.obj
            evas_object_clip_set(self.obj, clip)
        else:
            raise ValueError("clip must be evas.Object or None")

    def clip_unset(self):
        evas_object_clip_unset(self.obj)

    property clip:
        def __get__(self):
            return self.clip_get()

        def __set__(self, value):
            self.clip_set(value)

    def name_get(self):
        return evas_object_name_get(self.obj)

    def name_set(self, char *value):
        evas_object_name_set(self.obj, value)

    property name:
        def __get__(self):
            return self.name_get()

        def __set__(self, char *value):
            self.name_set(value)

    def focus_get(self):
        return bool(evas_object_focus_get(self.obj))

    def focus_set(self, int value):
        evas_object_focus_set(self.obj, value)

    property focus:
        def __get__(self):
            return self.focus_get()

        def __set__(self, int value):
            self.focus_set(value)

    def event_callback_add(self, int type, func, *args, **kargs):
        cdef evas_event_callback_t cb
        if type < 0 or type >= evas_event_callbacks_len:
            raise ValueError("Invalid callback type")

        r = (func, args, kargs)
        lst = self._callbacks[type]
        if lst is not None:
            lst.append(r)
        else:
            self._callbacks[type] = [r]
            cb = evas_event_callbacks[type]
            evas_object_event_callback_add(self.obj,
                                           <Evas_Callback_Type>type,
                                           cb, <void*>self)

    def event_callback_del(self, int type, func):
        cdef evas_event_callback_t cb
        if type < 0 or type >= evas_event_callbacks_len:
            raise ValueError("Invalid callback type")

        lst = self._callbacks[type]
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

        del lst[i]
        if len(lst) == 0:
            self._callbacks[type] = None
            cb = evas_event_callbacks[type]
            evas_object_event_callback_del(self.obj,
                                           <Evas_Callback_Type>type, cb)

    def on_mouse_in_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_MOUSE_IN, func, *a, **k)

    def on_mouse_in_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_MOUSE_IN, func)

    def on_mouse_out_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_MOUSE_OUT, func, *a, **k)

    def on_mouse_out_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_MOUSE_OUT, func)

    def on_mouse_down_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_MOUSE_DOWN, func, *a, **k)

    def on_mouse_down_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_MOUSE_DOWN, func)

    def on_mouse_up_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_MOUSE_UP, func, *a, **k)

    def on_mouse_up_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_MOUSE_UP, func)

    def on_mouse_move_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_MOUSE_MOVE, func, *a, **k)

    def on_mouse_move_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_MOUSE_MOVE, func)

    def on_mouse_wheel_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_MOUSE_WHEEL, func, *a, **k)

    def on_mouse_wheel_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_MOUSE_WHEEL, func)

    def on_free_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_FREE, func, *a, **k)

    def on_free_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_FREE, func)

    def on_key_down_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_KEY_DOWN, func, *a, **k)

    def on_key_down_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_KEY_DOWN, func)

    def on_key_up_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_KEY_UP, func, *a, **k)

    def on_key_up_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_KEY_UP, func)

    def on_focus_in_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_FOCUS_IN, func, *a, **k)

    def on_focus_in_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_FOCUS_IN, func)

    def on_focus_out_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_FOCUS_OUT, func, *a, **k)

    def on_focus_out_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_FOCUS_OUT, func)

    def on_show_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_SHOW, func, *a, **k)

    def on_show_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_SHOW, func)

    def on_hide_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_HIDE, func, *a, **k)

    def on_hide_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_HIDE, func)

    def on_move_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_MOVE, func, *a, **k)

    def on_move_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_MOVE, func)

    def on_resize_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_RESIZE, func, *a, **k)

    def on_resize_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_RESIZE, func)

    def on_restack_add(self, func, *a, **k):
        self.event_callback_add(EVAS_CALLBACK_RESTACK, func, *a, **k)

    def on_restack_del(self, func):
        self.event_callback_del(EVAS_CALLBACK_RESTACK, func)

    def pass_events_get(self):
        return bool(evas_object_pass_events_get(self.obj))

    def pass_events_set(self, int value):
        evas_object_pass_events_set(self.obj, value)

    property pass_events:
        def __get__(self):
            return self.pass_events_get()

        def __set__(self, int value):
            self.pass_events_set(value)

    def repeat_events_get(self):
        return bool(evas_object_repeat_events_get(self.obj))

    def repeat_events_set(self, int value):
        evas_object_repeat_events_set(self.obj, value)

    property repeat_events:
        def __get__(self):
            return self.repeat_events_get()

        def __set__(self, int value):
            self.repeat_events_set(value)

    def propagate_events_get(self):
        return bool(evas_object_propagate_events_get(self.obj))

    def propagate_events_set(self, int value):
        evas_object_propagate_events_set(self.obj, value)

    property propagate_events:
        def __get__(self):
            return self.propagate_events_get()

        def __set__(self, int value):
            self.propagate_events_set(value)

    def parent_get(self):
        "Get object's parent, in the case it was added to some SmartObject."
        cdef Evas_Object *obj
        obj = evas_object_smart_parent_get(self.obj)
        return Object_from_instance(obj)

    property parent:
        def __get__(self):
            return self.parent_get()
