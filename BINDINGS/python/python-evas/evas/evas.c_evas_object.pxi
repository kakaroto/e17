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

# This file is included verbatim by c_evas.pyx

import traceback

cdef int _object_free_wrapper_resources(Object obj) except 0:
    cdef int i
    for i from 0 <= i < evas_object_event_callbacks_len:
        obj._callbacks[i] = None
    obj.data.clear()
    return 1


cdef int _object_unregister_callbacks(Object obj) except 0:
    cdef Evas_Object *o
    cdef Evas_Object_Event_Cb cb
    o = obj.obj
    if o != NULL:
        for i, lst in enumerate(obj._callbacks):
            if lst is not None:
                cb = evas_object_event_callbacks[i]
                evas_object_event_callback_del(o, i, cb)

    evas_object_event_callback_del(o, EVAS_CALLBACK_FREE, obj_free_cb)
    return 1


cdef void obj_free_cb(void *data, Evas *e,
                      Evas_Object *obj, void *event_info) with gil:
    cdef Object self
    self = <Object>data

    self.obj = NULL
    self.evas = <Canvas>None

    lst = self._callbacks[EVAS_CALLBACK_FREE]
    if lst is not None:
        for func, args, kargs in lst:
            try:
                func(self, *args, **kargs)
            except:
                traceback.print_exc()

    _object_free_wrapper_resources(self)
    Py_DECREF(self)


cdef _object_register_decorated_callbacks(obj):
    if not hasattr(obj, "__evas_event_callbacks__"):
        return

    for attr_name, evt in obj.__evas_event_callbacks__:
        attr_value = getattr(obj, attr_name)
        obj.event_callback_add(evt, attr_value)


cdef _object_add_callback_to_list(Object obj, int type, func, args, kargs):
    if type < 0 or type >= evas_object_event_callbacks_len:
        raise ValueError("Invalid callback type")

    r = (func, args, kargs)
    lst = obj._callbacks[type]
    if lst is not None:
        lst.append(r)
        return False
    else:
        obj._callbacks[type] = [r]
        return True


cdef _object_del_callback_from_list(Object obj, int type, func):
    if type < 0 or type >= evas_object_event_callbacks_len:
        raise ValueError("Invalid callback type")

    lst = obj._callbacks[type]
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
        obj._callbacks[type] = None
        return True
    else:
        return False


cdef public class Object(object) [object PyEvasObject, type PyEvasObject_Type]:

    """Basic Graphical Object (or actor).

    Objects are managed by L{Canvas} in a non-immediate way, that is,
    all operations, like moving, resizing, changing the color, etc will
    not trigger immediate repainting, instead it will save the new state
    and mark both this object and its Canvas as "dirty" so can be redrawn
    on L{Canvas.render()} (usually called by the underlying system, like
    B{ecore.evas} when you're entering idle. This means that doesn't matter
    how many times you're moving an object between frame updates: just the
    last state will be used, that's why you really should do animations
    using L{ecore.animator_add()} instead of L{ecore.timer_add()}, since
    it will call registered functions in one batch and then trigger redraw,
    instead of calling one function, then redraw, then the next function,
    and redraw...

    The most important concept for evas object is B{clipping}
    (L{clip_set()} and L{clip_unset()}), usually done by use of
    L{Rectangle} as clipper. Clip objects will affect the drawing behavior:
        - Limiting visibility
        - Limiting geometry
        - Modulating color
    Clips respects the hierarchy: the minimum area and the composed color
    will be used used at the end, if one object is not visible, othe lower
    objects (clipped by it) will not be visible as well. Clipping is the
    recommended way of doing fade out/in effect, instead of changing object's
    color, clip it to a rectangle and change its color: this will work as
    expected with every object, unlike directly changing color that just
    work for L{Image}s.

    As with every evas component, colors should be specified in
    B{pre-multiplied} format, see L{evas.color_parse()} and
    L{evas.color_argb_premul()}.

    Objects can be grouped by means of L{SmartObject}, a virtual class
    that can have it's methods implemented in order to apply methods to
    its children.

    @attention: since we have two systems controlling object's life (Evas
        and Python) objects need to be explicitly deleted using L{delete()}
        call. If this call is not issued, the Python object will not be
        released, but if the object is deleted by Evas (ie: due parent
        deletion), the object will become "shallow" and all operations
        will either have no effect or raise exceptions. You can be notified
        of object deletion by the C{EVAS_CALLBACK_FREE} (see L{on_free_add()}
        or L{event_callback_add()}.

    @ivar data: utility dict used to hold any user data.
    @ivar evas: L{Canvas} that owns this object.

    @group State manipulation: clip_set, clip_get, clip_unset, clip,
        color_set, color_get, color, show, hide, visible_set, visible_get,
        visible, delete, is_deleted
    @group Positioning: pos_set, pos_get, pos, move, move_relative,
        size_set, size_get, size, resize, geometry*, center*, top_left*,
        top_right*, bottom_left*, bottom_right*
    @group Layer & Stack manipulation: above_get, above, below_get, below,
        layer_set, layer_get, layer, lower, raise_, stack_above, stack_below,
        bottom_get, bottom, top_get, top
    @group Event processing control: focus_set, focus_get, focus,
        pass_events*, repeat_events*, propagate_events*
    @group Event callbacks: event_callback_*, on_*
    @group Often unused: render_op*, anti_alias*, pointer_mode*

    @param evas: Evas canvas for this object
    @type evas: L{Canvas}

    """

    def __cinit__(self, *a, **ka):
        self.obj = NULL
        self.evas = None
        self.data = dict()
        self._callbacks = [None] * evas_object_event_callbacks_len

    def __init__(self, Canvas evas not None):
        self.evas = evas

    def __str__(self):
        x, y, w, h = self.geometry_get()
        r, g, b, a = self.color_get()
        name = self.name_get()
        if name:
            name_str = "name=%r, "
        else:
            name_str = ""
        clip = bool(self.clip_get() is not None)
        return ("%s(%sgeometry=(%d, %d, %d, %d), color=(%d, %d, %d, %d), "
                "layer=%s, clip=%s, visible=%s)") % \
               (self.__class__.__name__, name_str, x, y, w, h,
                r, g, b, a, self.layer_get(), clip, self.visible_get())

    def __repr__(self):
        x, y, w, h = self.geometry_get()
        r, g, b, a = self.color_get()
        clip = bool(self.clip_get() is not None)
        return ("%s(%#x, type=%r, refcount=%d, Evas_Object=%#x, name=%r, "
                "geometry=(%d, %d, %d, %d), color=(%d, %d, %d, %d), "
                "layer=%s, clip=%r, visible=%s)") % \
               (self.__class__.__name__, <unsigned long><void *>self,
                self.type_get(), PY_REFCOUNT(self), <unsigned long>self.obj,
                self.name_get(), x, y, w, h, r, g, b, a, self.layer_get(),
                clip, self.visible_get())

    cdef int _unset_obj(self) except 0:
        assert self.obj != NULL, "Object must wrap something"
        _object_unregister_callbacks(self)
        _object_free_wrapper_resources(self)
        assert evas_object_data_del(self.obj, _cfruni("python-evas")) == <void*>self, \
               "Evas_Object has incorrect python-evas data"
        self.obj = NULL
        self.evas = <Canvas>None
        Py_DECREF(self)
        return 1

    cdef int _set_obj(self, Evas_Object *obj) except 0:
        assert self.obj == NULL, "Object must be clean"
        assert evas_object_data_get(obj, _cfruni("python-evas")) == NULL, \
               "Evas_Object must not wrapped by something else!"
        self.obj = obj
        Py_INCREF(self)
        evas_object_data_set(obj, _cfruni("python-evas"), <void *>self)
        evas_object_event_callback_add(obj, EVAS_CALLBACK_FREE, obj_free_cb,
                                       <void *>self)
        _object_register_decorated_callbacks(self)
        self.name_set(self.__class__.__name__)
        return 1

    def _set_evas(self, Canvas evas not None):
        self.evas = evas

    def __dealloc__(self):
        cdef void *data
        cdef Evas_Object *obj

        _object_unregister_callbacks(self)
        self.data = None
        self._callbacks = None
        obj = self.obj
        if obj == NULL:
            return
        self.obj = NULL
        self.evas = <Canvas>None

        data = evas_object_data_get(obj, _cfruni("python-evas"))
        assert data == NULL, "Object must not be wrapped!"
        evas_object_del(obj)

    def delete(self):
        """delete()

        Delete object and free it's internal (wrapped) resources.

        @note: after this operation the object will be still alive in
            Python, but it will be shallow and every operation
            will have no effect (and may raise exceptions).
        @raise ValueError: if object already deleted.

        """
        if self.obj == NULL:
            raise ValueError("Object already deleted")
        evas_object_del(self.obj)

    def is_deleted(self):
        """is_deleted()

        @rtype: bool

        """
        return bool(self.obj == NULL)

    def _set_common_params(self, size=None, pos=None, geometry=None,
                           color=None, name=None):
        """Set common parameters in one go."""
        if size:
            self.size_set(*size)
        if pos:
            self.pos_set(*pos)
        if geometry:
            self.geometry_set(*geometry)
        if color:
            self.color_set(*color_parse(color))
        if name:
            self.name_set(name)

    def evas_get(self):
        """evas_get()

        @rtype: L{Canvas}

        """
        return self.evas

    def type_get(self):
        """type_get()

        Get the Evas object's type

        @rtype: string
        """
        if self.obj:
            return _ctouni(evas_object_type_get(self.obj))

    property type:
        """Type name, ie: "rectangle".

        @type: string

        """
        def __get__(self):
            return self.type_get()

    def layer_set(self, int layer):
        """layer_set(layer)

        @param layer: Layer number.
        @type layer: int

        """
        evas_object_layer_set(self.obj, layer)

    def layer_get(self):
        """layer_get()

        @return: Layer number.
        @rtype: int

        """
        return evas_object_layer_get(self.obj)

    property layer:
        """Object's layer number.

        @type: int

        """
        def __set__(self, int l):
            evas_object_layer_set(self.obj, l)

        def __get__(self):
            return evas_object_layer_get(self.obj)

    def raise_(self):
        """raise_()

        Raise to the top of its layer.

        """
        evas_object_raise(self.obj)

    def lower(self):
        """lower()

        Lower to the bottom of its layer.

        """
        evas_object_lower(self.obj)

    def stack_above(self, Object above):
        """stack_above(above)

        Reorder to be above the given one.

        @param above:
        @type above: L{Object}

        """
        evas_object_stack_above(self.obj, above.obj)

    def stack_below(self, Object below):
        """stack_below(below)

        Reorder to be below the given one.

        @param below:
        @type below: L{Object}

        """
        evas_object_stack_below(self.obj, below.obj)

    def above_get(self):
        """above_get()

        @return: The object above this.
        @rtype: L{Object}

        """
        cdef Evas_Object *other
        other = evas_object_above_get(self.obj)
        return Object_from_instance(other)

    property above:
        """The object above this.

        @type: L{Object}

        """
        def __get__(self):
            return self.above_get()

    def below_get(self):
        """below_get()

        @return: The object below this.
        @rtype: L{Object}

        """
        cdef Evas_Object *other
        other = evas_object_below_get(self.obj)
        return Object_from_instance(other)

    property below:
        """The object below this.

        @type: L{Object}

        """
        def __get__(self):
            return self.below_get()

    def top_get(self):
        """top_get()

        @return: The topmost object. (Same as self.evas.top_get()).
        @rtype: L{Object}

        """
        return self.evas.top_get()

    property top:
        def __get__(self):
            return self.top_get()

    def bottom_get(self):
        """bottom_get()

        @return: The bottommost object. (Same as self.evas.bottom_get()).
        @rtype: L{Object}

        """
        return self.evas.bottom_get()

    property bottom:
        def __get__(self):
            return self.bottom_get()

    def geometry_get(self):
        """geometry_get()

        @return: Object geometry (position and size).
        @rtype: tuple of int

        """
        cdef int x, y, w, h
        evas_object_geometry_get(self.obj, &x, &y, &w, &h)
        return (x, y, w, h)

    def geometry_set(self, int x, int y, int w, int h):
        """geometry_set(x, y, w, h)

        Set object geometry (position and size).

        @param x: Position on the X axis.
        @type x: int
        @param y: Position on the Y axis.
        @type y: int
        @param w: Width.
        @type w: int
        @param h: Height.
        @type h: int

        """
        evas_object_move(self.obj, x, y)
        evas_object_resize(self.obj, w, h)

    property geometry:
        """Object's position and size (X, Y, W, H).

        @type: tuple of ints

        """
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
        """size_get()

        @return: Object size.
        @rtype: tuple of ints

        """
        cdef int w, h
        evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
        return (w, h)

    def size_set(self, int w, int h):
        """size_set(w, h)

        Set object size.

        @param w: Width
        @type w: int
        @param h: Height
        @type h: int

        """
        evas_object_resize(self.obj, w, h)

    property size:
        """Object's size (width and height).

        @type: tuple of ints

        """
        def __get__(self): # replicated to avoid performance hit
            cdef int w, h
            evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
            return (w, h)

        def __set__(self, spec):
            cdef int w, h
            w, h = spec
            evas_object_resize(self.obj, w, h)

    def resize(self, int w, int h):
        """resize(w, h)

        Same as L{size_set()}.

        @param w: Width.
        @type w: int
        @param h: Height.
        @type h: int

        """
        evas_object_resize(self.obj, w, h)

    def pos_get(self):
        """pos_get()

        @return: Object position on the X and Y axes.
        @rtype: tuple of ints

        """
        cdef int x, y
        evas_object_geometry_get(self.obj, &x, &y, NULL, NULL)
        return (x, y)

    def pos_set(self, int x, int y):
        """pos_set(x, y)

        Set object position.

        @param x: X coordinate
        @type x: int
        @param y: Y coordinate
        @type y: int

        """
        evas_object_move(self.obj, x, y)

    property pos:
        """Object's position on the X and Y coordinates.

        @type: tuple of ints

        """
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y
            evas_object_geometry_get(self.obj, &x, &y, NULL, NULL)
            return (x, y)

        def __set__(self, spec):
            cdef int x, y
            x, y = spec
            evas_object_move(self.obj, x, y)

    def top_left_get(self):
        """top_left_get()

        @return: Coordinates of top-left corner
        @rtype: tuple of ints

        """
        cdef int x, y
        evas_object_geometry_get(self.obj, &x, &y, NULL, NULL)
        return (x, y)

    def top_left_set(self, int x, int y):
        """top_left_set(x, y)

        Set X and Y coordinates of top-left corner.

        @param x: X coordinate
        @type x: int
        @param y: Y coordinate
        @type y: int

        """
        evas_object_move(self.obj, x, y)

    property top_left:
        """Object's top-left corner coordinates.

        @type: tuple of ints

        """
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y
            evas_object_geometry_get(self.obj, &x, &y, NULL, NULL)
            return (x, y)

        def __set__(self, spec):
            cdef int x, y
            x, y = spec
            evas_object_move(self.obj, x, y)

    def top_center_get(self):
        """top_center_get()

        @return: Coordinates of the top-center position
        @rtype: tuple of int

        """
        cdef int x, y, w
        evas_object_geometry_get(self.obj, &x, &y, &w, NULL)
        return (x + w/2, y)

    def top_center_set(self, int x, int y):
        """top_center_set(x, y)

        Set the position of top-center.

        @param x: X coordinate.
        @type x: int
        @param y: Y coordinate.
        @type y: int

        """
        cdef int w
        evas_object_geometry_get(self.obj, NULL, NULL, &w, NULL)
        evas_object_move(self.obj, x - w/2, y)

    property top_center:
        """The coordinates of the top-center position.

        @type: tuple of ints

        """
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y, w
            evas_object_geometry_get(self.obj, &x, &y, &w, NULL)
            return (x + w/2, y)

        def __set__(self, spec):
            cdef int x, y, w
            x, y = spec
            evas_object_geometry_get(self.obj, NULL, NULL, &w, NULL)
            evas_object_move(self.obj, x - w/2, y)

    def top_right_get(self):
        """top_right_get()

        @return: (x, y) of top-right corner.
        @rtype: tuple of int

        """
        cdef int x, y, w
        evas_object_geometry_get(self.obj, &x, &y, &w, NULL)
        return (x + w, y)

    def top_right_set(self, int x, int y):
        """top_right_set(x, y)

        Set (x, y) of top-right corner.

        @param x: X coordinate
        @type x: int
        @param y: Y coordinate
        @type y: int

        """
        cdef int w
        evas_object_geometry_get(self.obj, NULL, NULL, &w, NULL)
        evas_object_move(self.obj, x - w, y)

    property top_right:
        """Object's top-right corner coordinates.

        @type: tuple of ints

        """
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y, w
            evas_object_geometry_get(self.obj, &x, &y, &w, NULL)
            return (x + w, y)

        def __set__(self, spec):
            cdef int x, y, w
            x, y = spec
            evas_object_geometry_get(self.obj, NULL, NULL, &w, NULL)
            evas_object_move(self.obj, x - w, y)

    def left_center_get(self):
        """left_center_get()

        @return: (x, y) of left-center.
        @rtype: tuple of ints

        """
        cdef int x, y, h
        evas_object_geometry_get(self.obj, &x, &y, NULL, &h)
        return (x, y + h/2)

    def left_center_set(self, int x, int y):
        """left_center_set(x, y)

        Set (x, y) of left-center.

        @param x: X coordinate
        @type x: int
        @param y: Y coordinate
        @type y: int

        """
        cdef int h
        evas_object_geometry_get(self.obj, NULL, NULL, NULL, &h)
        evas_object_move(self.obj, x, y - h/2)

    property left_center:
        """The coordinates of the left-center position.

        @type: tuple of ints

        """
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y, h
            evas_object_geometry_get(self.obj, &x, &y, NULL, &h)
            return (x, y + h/2)

        def __set__(self, spec):
            cdef int x, y, h
            x, y = spec
            evas_object_geometry_get(self.obj, NULL, NULL, NULL, &h)
            evas_object_move(self.obj, x, y - h/2)

    def right_center_get(self):
        """right_center_get()

        @return: (x, y) of right-center.
        @rtype: tuple of ints

        """
        cdef int x, y, w, h
        evas_object_geometry_get(self.obj, &x, &y, &w, &h)
        return (x + w, y + h/2)

    def right_center_set(self, int x, int y):
        """right_center_set(x, y)

        Set (x, y) of right-center.

        @param x: X coordinate
        @type x: int
        @param y: Y coordinate
        @type y: int

        """
        cdef int w, h
        evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
        evas_object_move(self.obj, x - w, y - h/2)

    property right_center:
        """The coordinates of the right-center position.

        @type: tuple of ints

        """
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y, w, h
            evas_object_geometry_get(self.obj, &x, &y, &w, &h)
            return (x + w, y + h/2)

        def __set__(self, spec):
            cdef int x, y, w, h
            x, y = spec
            evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
            evas_object_move(self.obj, x - w, y - h/2)

    def bottom_left_get(self):
        """bottom_left_get()

        @return: (x, y) of bottom-left corner.
        @rtype: tuple of ints

        """
        cdef int x, y, h
        evas_object_geometry_get(self.obj, &x, &y, NULL, &h)
        return (x, y + h)

    def bottom_left_set(self, int x, int y):
        """bottom_left_set(x, y)

        Set (x, y) of bottom-left corner.

        @param x: X coordinate
        @type x: int
        @param y: Y coordinate
        @type y: int

        """
        cdef int h
        evas_object_geometry_get(self.obj, NULL, NULL, NULL, &h)
        evas_object_move(self.obj, x, y - h)

    property bottom_left:
        """Object's bottom-left corner coordinates.

        @type: tuple of ints

        """
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y, h
            evas_object_geometry_get(self.obj, &x, &y, NULL, &h)
            return (x, y + h)

        def __set__(self, spec):
            cdef int x, y, h
            x, y = spec
            evas_object_geometry_get(self.obj, NULL, NULL, NULL, &h)
            evas_object_move(self.obj, x, y - h)

    def bottom_center_get(self):
        """bottom_center_get()

        @return: (x, y) of bottom-center.
        @rtype: tuple of ints

        """
        cdef int x, y, w, h
        evas_object_geometry_get(self.obj, &x, &y, &w, &h)
        return (x + w/2, y + h)

    def bottom_center_set(self, int x, int y):
        """bottom_center_set(x, y)

        Set (x, y) of bottom-center.

        @param x: X coordinate
        @type x: int
        @param y: Y coordinate
        @type y: int

        """
        cdef int w, h
        evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
        evas_object_move(self.obj, x - w/2, y - h)

    property bottom_center:
        """Object's bottom-center coordinates.

        @type: tuple of ints

        """
        def __get__(self): # replicated to avoid performance hit
            cdef int x, y, w, h
            evas_object_geometry_get(self.obj, &x, &y, &w, &h)
            return (x + w/2, y + h)

        def __set__(self, spec):
            cdef int x, y, w, h
            x, y = spec
            evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
            evas_object_move(self.obj, x - w/2, y - h)

    def bottom_right_get(self):
        """bottom_right_get()

        @return: (x, y) of bottom-right corner.
        @rtype: tuple of int

        """
        cdef int x, y, w, h
        evas_object_geometry_get(self.obj, &x, &y, &w, &h)
        return (x + w, y + h)

    def bottom_right_set(self, int x, int y):
        """bottom_right_set(x, y)

        Set (x, y) of bottom-right corner.

        @param x: X coordinate
        @type x: int
        @param y: Y coordinate
        @type y: int

        """
        cdef int w, h
        evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
        evas_object_move(self.obj, x - w, y - h)

    property bottom_right:
        """Object's bottom-right corner coordinates.

        @type: tuple of ints

        """
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
        """center_get()

        @return: (x, y) of center.
        @rtype: tuple of ints

        """
        cdef int x, y, w, h
        evas_object_geometry_get(self.obj, &x, &y, &w, &h)
        return (x + w/2, y + h/2)

    def center_set(self, int x, int y):
        """center_set(x, y)

        Set (x, y) of center.

        @param x: X coordinate
        @type x: int
        @param y: Y coordinate
        @type y: int

        """
        cdef int w, h
        evas_object_geometry_get(self.obj, NULL, NULL, &w, &h)
        evas_object_move(self.obj, x - w/2, y - h/2)

    property center:
        """Object's center coordinates.

        @type: tuple of ints

        """
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
        """A rectangle representing the object's geometry.

        Rectangles have useful operations like clip, clamp, union and
        also provides various attributes like top_left, center_x, ...

        Note that changing rectangle is a snapshot of current state, it
        is not synchronized to the object, so modifying attributes DO NOT
        change the object itself! You must set the Object.rect property
        to update object information.

        @type: tuple of ints

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

    def size_hint_min_get(self):
        """size_hint_min_get()

        Retrieve minimum size hint.

        @return: Minimum size hint.
        @rtype: tuple of ints (w, h)

        """
        cdef int w, h
        evas_object_size_hint_min_get(self.obj, &w, &h)
        return (w, h)

    def size_hint_min_set(self, int w, int h):
        """size_hint_min_set(w, h)

        Set hint for minimum size.

        This is a hint and not an enforcement. value 0 is disabled.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @param w: horizontal value.
        @type w: int
        @param h: vertical value.
        @type h: int

        """
        evas_object_size_hint_min_set(self.obj, w, h)

    property size_hint_min:
        """Hint about minimum size.

        This is not an enforcement, just a hint that can be used by
        other objects like Edje, boxes, tables and others.

        Value 0 is disabled.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @type: tuple of ints

        """
        def __get__(self):
            return self.size_hint_min_get()

        def __set__(self, spec):
            self.size_hint_min_set(*spec)

    def size_hint_max_get(self):
        """size_hint_max_get()

        Retrieve maximum size hint.

        @return: Maximum size hint.
        @rtype: tuple of integers (w, h)

        """
        cdef int w, h
        evas_object_size_hint_max_get(self.obj, &w, &h)
        return (w, h)

    def size_hint_max_set(self, int w, int h):
        """size_hint_max_set(w, h)

        Set hint for maximum size.

        This is a hint and not an enforcement. value -1 is disabled.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @param w: horizontal value.
        @type w: int
        @param h: vertical value.
        @type h: int

        """
        evas_object_size_hint_max_set(self.obj, w, h)

    property size_hint_max:
        """Hint about maximum size.

        This is not an enforcement, just a hint that can be used by
        other objects like Edje, boxes, tables and others.

        Value -1 is disabled.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @type: tuple of ints

        """
        def __get__(self):
            return self.size_hint_max_get()

        def __set__(self, spec):
            self.size_hint_max_set(*spec)

    def size_hint_request_get(self):
        """size_hint_request_get()

        Retrieve request size hint.

        @return: Request size hint.
        @rtype: tuple of integers (w, h)

        """
        cdef int w, h
        evas_object_size_hint_request_get(self.obj, &w, &h)
        return (w, h)

    def size_hint_request_set(self, int w, int h):
        """size_hint_request_set(w, h)

        Set hint for request size.

        This is a hint and not an enforcement. value 0 is disabled.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @param w: horizontal value.
        @type w: int
        @param h: vertical value.
        @type h: int

        """
        evas_object_size_hint_request_set(self.obj, w, h)

    property size_hint_request:
        """Hint about request size.

        This is not an enforcement, just a hint that can be used by
        other objects like Edje, boxes, tables and others.

        Value 0 is disabled.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @type: tuple of ints

        """
        def __get__(self):
            return self.size_hint_request_get()

        def __set__(self, spec):
            self.size_hint_request_set(*spec)

    def size_hint_aspect_get(self):
        """size_hint_aspect_get()

        Retrieve aspect hint.

        @return: Aspect hint.
        @rtype: tuple of ints (aspect, w, h)

        """
        cdef int w, h
        cdef Evas_Aspect_Control aspect
        evas_object_size_hint_aspect_get(self.obj, &aspect, &w, &h)
        return (<int>aspect, w, h)

    def size_hint_aspect_set(self, int aspect, int w, int h):
        """size_hint_aspect_set(aspect, w, h)

        Set hint for aspect.

        This is a hint and not an enforcement. If aspect is
        EVAS_ASPECT_CONTROL_NONE (0), then it's disabled.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @param aspect: aspect value, one of EVAS_ASPECT_CONTROL_*.
        @param w: horizontal value.
        @type w: int
        @param h: vertical value.
        @type h: int

        """
        evas_object_size_hint_aspect_set(self.obj, <Evas_Aspect_Control>aspect,
                                         w, h)

    property size_hint_aspect:
        """Hint about aspect.

        This is not an enforcement, just a hint that can be used by
        other objects like Edje, boxes, tables and others.

        Aspect EVAS_ASPECT_CONTROL_NONE is disabled.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @type: tuple of ints

        """
        def __get__(self):
            return self.size_hint_aspect_get()

        def __set__(self, spec):
            self.size_hint_aspect_set(*spec)

    def size_hint_align_get(self):
        """size_hint_align_get()

        Retrieve alignment hint.

        @return: Alignment hint.
        @rtype: tuple of floats (x, y)

        """
        cdef double x, y
        evas_object_size_hint_align_get(self.obj, &x, &y)
        return (x, y)

    def size_hint_align_set(self, float x, float y):
        """size_hint_align_set(x, y)

        Set hint for alignment.

        This is a hint and not an enforcement. Accepted values are in
        the 0.0 to 1.0 range, with the special value -1.0 used to
        specify"justify" or "fill" by some users. See documentation of
        possible users.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @param x: horizontal value.
        @type x: float
        @param y: vertical value.
        @type y: float

        """
        evas_object_size_hint_align_set(self.obj, x, y)

    property size_hint_align:
        """Hint about alignment.

        This is not an enforcement, just a hint that can be used by
        other objects like Edje, boxes, tables and others.

        Accepted values are in the 0.0 to 1.0 range, with the special
        value -1.0 used to specify"justify" or "fill" by some users.
        See documentation of possible users.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @type: tuple of floats

        """
        def __get__(self):
            return self.size_hint_align_get()

        def __set__(self, spec):
            self.size_hint_align_set(*spec)

    def size_hint_weight_get(self):
        """size_hint_weight_get()

        Retrieve weight hint.

        @return: Weight hint.
        @rtype: tuple of floats (x, y)

        """
        cdef double x, y
        evas_object_size_hint_weight_get(self.obj, &x, &y)
        return (x, y)

    def size_hint_weight_set(self, float x, float y):
        """size_hint_weight_set(x, y)

        Set hint for weight.

        This is a hint and not an enforcement. Value 0.0 is disabled.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @param x: horizontal value.
        @type x: float
        @param y: vertical value.
        @type y: float

        """
        evas_object_size_hint_weight_set(self.obj, x, y)

    property size_hint_weight:
        """Hint about weight.

        This is not an enforcement, just a hint that can be used by
        other objects like Edje, boxes, tables and others.

        Value 0.0 is disabled.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @type: tuple of floats

        """
        def __get__(self):
            return self.size_hint_weight_get()

        def __set__(self, spec):
            self.size_hint_weight_set(*spec)

    def size_hint_padding_get(self):
        """size_hint_padding_get()

        Retrieve padding hint.

        @return: Padding hint.
        @rtype: tuple of integers (l, r, t, b)

        """
        cdef int l, r, t, b
        evas_object_size_hint_padding_get(self.obj, &l, &r, &t, &b)
        return (l, r, t, b)

    def size_hint_padding_set(self, int l, int r, int t, int b):
        """size_hint_padding_set(l, r, t, b)

        Set hint for padding.

        This is a hint and not an enforcement.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        @param l: left value.
        @type l: int
        @param r: right value.
        @type r: int
        @param t: top value.
        @type t: int
        @param b: bottom value.
        @type b: int

        """
        evas_object_size_hint_padding_set(self.obj, l, r, t, b)

    property size_hint_padding:
        """Hint about padding.

        This is not an enforcement, just a hint that can be used by
        other objects like Edje, boxes, tables and others.

        When this property changes, EVAS_CALLBACK_CHANGED_SIZE_HINTS
        will be emitted.

        """
        def __get__(self):
            return self.size_hint_padding_get()

        def __set__(self, spec):
            self.size_hint_padding_set(*spec)

    def move(self, int x, int y):
        """move(x, y)

        Same as L{pos_set()}.

        """
        evas_object_move(self.obj, x, y)

    def move_relative(self, int dx, int dy):
        """move_relative(dx, dy)

        Move relatively to current position.

        """
        cdef int x, y
        evas_object_geometry_get(self.obj, &x, &y, NULL, NULL)
        evas_object_move(self.obj, x + dx, y + dy)

    def show(self):
        """show()

        Show the object.

        """
        evas_object_show(self.obj)

    def hide(self):
        """hide()

        Hide the object.

        """
        evas_object_hide(self.obj)

    def visible_get(self):
        """visible_get()

        @return: Object visibility.
        @rtype: bool

        """
        return bool(evas_object_visible_get(self.obj))

    def visible_set(self, spec):
        """visible_set(spec)

        @param spec: Visibility
        @type spec: bool

        """
        if spec:
            self.show()
        else:
            self.hide()

    property visible:
        """Whenever it's visible or not.

        @type: bool

        """
        def __get__(self):
            return self.visible_get()

        def __set__(self, spec):
            self.visible_set(spec)

    def static_clip_get(self):
        """static_clip_get()

        A hint flag on the object, whether this is used as a static clipper
        or not.

        @return: The hint flag
        @rtype: bool

        """
        return bool(evas_object_static_clip_get(self.obj))

    def static_clip_set(self, int value):
        """static_clip_set(value)

        Set a hint flag on the object that this is used as a static clipper.

        @param value: The hint flag
        @type value: bool

        """
        evas_object_static_clip_set(self.obj, value)

    property static_clip:
        """A hint flag on the object, whether this is used as a static clipper
        or not.

        @type: bool

        """
        def __get__(self):
            return self.static_clip_get()

        def __set__(self, value):
            self.static_clip_set(value)

    def render_op_get(self):
        """render_op_get()

        @return: The operation used for rendering.
        @rtype: Evas_Render_Op

        """
        return evas_object_render_op_get(self.obj)

    def render_op_set(self, int value):
        """render_op_set(value)

        Set the operation used for rendering.

        @param value: EVAS_RENDER_BLEND, EVAS_RENDER_BLEND_REL,
            EVAS_RENDER_COPY, EVAS_RENDER_COPY_REL EVAS_RENDER_ADD,
            EVAS_RENDER_ADD_REL, EVAS_RENDER_SUB, EVAS_RENDER_SUB_REL,
            EVAS_RENDER_TINT, EVAS_RENDER_TINT_REL, EVAS_RENDER_MASK or
            EVAS_RENDER_MUL
        """
        evas_object_render_op_set(self.obj, <Evas_Render_Op>value)

    property render_op:
        """Render operation used at drawing.

        @type: Evas_Render_Op

        """
        def __get__(self):
            return self.render_op_get()

        def __set__(self, int value):
            self.render_op_set(value)

    def anti_alias_get(self):
        """anti_alias_get()

        @return: Whether the object is to be drawn anti-aliased.
        @rtype: bool

        """
        return bool(evas_object_anti_alias_get(self.obj))

    def anti_alias_set(self, int value):
        """anti_alias_set(value)

        Enable or disable the object to be drawn anti-aliased.

        @param value: Anti-aliasing
        @type value: bool

        """
        evas_object_anti_alias_set(self.obj, value)

    property anti_alias:
        """If anti-aliased primitives should be used.

        @type: bool

        """
        def __get__(self):
            return self.anti_alias_get()

        def __set__(self, int value):
            self.anti_alias_set(value)

    def color_set(self, int r, int g, int b, int a):
        """color_set(r, g, b, a)

        Set object color using r, g, b, a (uses pre-multiply colorspace!)

        @param r: Red
        @type r: int
        @param g: Green
        @type g: int
        @param b: Blue
        @type b: int
        @param a: Alpha
        @type a: int

        @note: You can convert non-premul to premul color space using
            L{evas.color_parse()} or using specific function
            L{evas.color_argb_premul()}.

        @see: L{evas.color_parse()}, L{evas.color_argb_premul()}.

        """
        evas_object_color_set(self.obj, r, g, b, a)

    def color_get(self):
        """color_get()

        Get object color (r, g, b, a) (uses pre-multiply colorspace!)

        @return: (r, g, b, a)
        @rtype: tuple of int
        @see: L{evas.color_argb_unpremul()}

        """
        cdef int r, g, b, a
        evas_object_color_get(self.obj, &r, &g, &b, &a)
        return (r, g, b, a)

    property color:
        """Object's (r, g, b, a) color, in pre-multiply colorspace.

        @type: tuple of ints

        """

        def __get__(self):
            return self.color_get()

        def __set__(self, color):
            self.color_set(*color)

    def clip_get(self):
        """clip_get()

        @return: The object currently clipping this object, or None.
        @rtype: L{Object} or None

        """
        cdef Evas_Object *clip
        clip = evas_object_clip_get(self.obj)
        return Object_from_instance(clip)

    def clip_set(self, value):
        """clip_set(value)

        Set the object to clip this object, or None to unset.

        @param value: L{Object} or None to remove clip.
        @type value: L{Object}

        """
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
        """clip_unset()

        Remove any clip of this object.

        """
        evas_object_clip_unset(self.obj)

    property clip:
        """Object's clipper.

        @type: L{Object}

        """
        def __get__(self):
            return self.clip_get()

        def __set__(self, value):
            self.clip_set(value)

        def __del__(self):
            self.clip_unset()

    def clipees_get(self):
        """clipees_get()

        @return: Objects currently clipped by this.
        @rtype: tuple of L{Object}

        """
        cdef const_Eina_List *itr
        cdef Object o
        ret = []
        itr = evas_object_clipees_get(self.obj)
        while itr:
            o = Object_from_instance(<Evas_Object*>itr.data)
            ret.append(o)
            itr = itr.next
        return tuple(ret)

    property clipees:
        """Objects that this object clips.

        @type: tuple of L{Object}s

        """
        def __get__(self):
            return self.clipees_get()

    def name_get(self):
        """name_get()

        @return: The name of the object.
        @rtype: string

        """
        return _ctouni(evas_object_name_get(self.obj))

    def name_set(self, value):
        """name_set(value)

        Set the name of the object.

        Names have no great utility, you can use them to help debug or even
        to retrive them later by L{Canvas.object_name_find()}.

        @param value: The name of the object
        @type value: string

        """
        evas_object_name_set(self.obj, _cfruni(value))

    property name:
        """Object name or C{None}.

        @type: string

        """
        def __get__(self):
            return _ctouni(evas_object_name_get(self.obj))

        def __set__(self, value):
            evas_object_name_set(self.obj, _cfruni(value))

    def focus_get(self):
        """focus_get()

        Returns if this object currently have the focus.

        @rtype: bool

        """
        return bool(evas_object_focus_get(self.obj))

    def focus_set(self, value):
        """focus_set(value)

        @param value: If this object currently have the focus.
        @type value: bool

        """
        evas_object_focus_set(self.obj, value)

    property focus:
        """Whenever object currently have the focus.

        @type: bool

        """
        def __get__(self):
            return self.focus_get()

        def __set__(self, int value):
            self.focus_set(value)

    def event_callback_add(self, int type, func, *args, **kargs):
        """event_callback_add(type, func, *args, **kwargs)

        Add a new callback for the given event.

        @param type: an integer with event type code, like
            C{EVAS_CALLBACK_MOUSE_IN}, C{EVAS_CALLBACK_KEY_DOWN} and
            other C{EVAS_CALLBACK_*} constants.
        @type type: int
        @param func: function to call back, this function will have one of
            the following signatures:
                - C{function(object, event, *args, **kargs)}
                - C{function(object, *args, **kargs)}
            The former is used by events that provide more data, like
            C{EVAS_CALLBACK_MOUSE_*}, C{EVAS_CALLBACK_KEY_*}, while the
            second is used by events without. Parameters given at the
            end of C{event_callback_add()} will be given to the callback.
            Note that the object passed to the callback in B{event}
            parameter will only be valid during the callback, using it
            after callback returns will raise an ValueError.
        @type func: function

        @raise ValueError: if B{type} is unknown.
        @raise TypeError: if B{func} is not callable.
        """
        cdef Evas_Object_Event_Cb cb

        if not callable(func):
            raise TypeError("func must be callable")

        if _object_add_callback_to_list(self, type, func, args, kargs):
            if type != EVAS_CALLBACK_FREE:
                cb = evas_object_event_callbacks[type]
                evas_object_event_callback_add(self.obj, type, cb, <void*>self)

    def event_callback_del(self, int type, func):
        """event_callback_del(type, func)

        Remove callback for the given event.

        @param type: an integer with event type code.
        @type type: int
        @param func: function used with L{event_callback_add()}.
        @type func: function
        @precond: B{type} and B{func} must be used as parameter for
            L{event_callback_add()}.

        @raise ValueError: if B{type} is unknown or if there was no
            B{func} connected with this type.

        """
        cdef Evas_Object_Event_Cb cb
        if _object_del_callback_from_list(self, type, func):
            if type != EVAS_CALLBACK_FREE:
                cb = evas_object_event_callbacks[type]
                evas_object_event_callback_del(self.obj, type, cb)

    def on_mouse_in_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_MOUSE_IN, ...)

        Expected signature: C{function(object, event_info, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_MOUSE_IN, func, *a, **k)

    def on_mouse_in_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_MOUSE_IN, ...)"""
        self.event_callback_del(EVAS_CALLBACK_MOUSE_IN, func)

    def on_mouse_out_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_MOUSE_OUT, ...)

        Expected signature: C{function(object, event_info, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_MOUSE_OUT, func, *a, **k)

    def on_mouse_out_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_MOUSE_OUT, ...)"""
        self.event_callback_del(EVAS_CALLBACK_MOUSE_OUT, func)

    def on_mouse_down_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_MOUSE_DOWN, ...)

        Expected signature: C{function(object, event_info, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_MOUSE_DOWN, func, *a, **k)

    def on_mouse_down_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_MOUSE_DOWN, ...)"""
        self.event_callback_del(EVAS_CALLBACK_MOUSE_DOWN, func)

    def on_mouse_up_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_MOUSE_UP, ...)

        Expected signature: C{function(object, event_info, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_MOUSE_UP, func, *a, **k)

    def on_mouse_up_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_MOUSE_UP, ...)"""
        self.event_callback_del(EVAS_CALLBACK_MOUSE_UP, func)

    def on_mouse_move_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_MOUSE_MOVE, ...)

        Expected signature: C{function(object, event_info, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_MOUSE_MOVE, func, *a, **k)

    def on_mouse_move_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_MOUSE_MOVE, ...)"""
        self.event_callback_del(EVAS_CALLBACK_MOUSE_MOVE, func)

    def on_mouse_wheel_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_MOUSE_WHEEL, ...)

        Expected signature: C{function(object, event_info, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_MOUSE_WHEEL, func, *a, **k)

    def on_mouse_wheel_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_MOUSE_WHEEL, ...)"""
        self.event_callback_del(EVAS_CALLBACK_MOUSE_WHEEL, func)

    def on_free_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_FREE, ...)

        This is called after freeing object resources (see
        EVAS_CALLBACK_DEL).

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_FREE, func, *a, **k)

    def on_free_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_FREE, ...)"""
        self.event_callback_del(EVAS_CALLBACK_FREE, func)

    def on_key_down_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_KEY_DOWN, ...)

        Expected signature: C{function(object, event_info, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_KEY_DOWN, func, *a, **k)

    def on_key_down_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_KEY_DOWN, ...)"""
        self.event_callback_del(EVAS_CALLBACK_KEY_DOWN, func)

    def on_key_up_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_KEY_UP, ...)

        Expected signature: C{function(object, event_info, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_KEY_UP, func, *a, **k)

    def on_key_up_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_KEY_UP, ...)"""
        self.event_callback_del(EVAS_CALLBACK_KEY_UP, func)

    def on_focus_in_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_FOCUS_IN, ...)

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_FOCUS_IN, func, *a, **k)

    def on_focus_in_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_FOCUS_IN, ...)"""
        self.event_callback_del(EVAS_CALLBACK_FOCUS_IN, func)

    def on_focus_out_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_FOCUS_OUT, ...)

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_FOCUS_OUT, func, *a, **k)

    def on_focus_out_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_FOCUS_OUT, ...)"""
        self.event_callback_del(EVAS_CALLBACK_FOCUS_OUT, func)

    def on_show_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_SHOW, ...)

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_SHOW, func, *a, **k)

    def on_show_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_SHOW, ...)"""
        self.event_callback_del(EVAS_CALLBACK_SHOW, func)

    def on_hide_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_HIDE, ...)

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_HIDE, func, *a, **k)

    def on_hide_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_HIDE, ...)"""
        self.event_callback_del(EVAS_CALLBACK_HIDE, func)

    def on_move_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_MOVE, ...)

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_MOVE, func, *a, **k)

    def on_move_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_MOVE, ...)"""
        self.event_callback_del(EVAS_CALLBACK_MOVE, func)

    def on_resize_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_RESIZE, ...)

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_RESIZE, func, *a, **k)

    def on_resize_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_RESIZE, ...)"""
        self.event_callback_del(EVAS_CALLBACK_RESIZE, func)

    def on_restack_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_RESTACK, ...)

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_RESTACK, func, *a, **k)

    def on_restack_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_RESTACK, ...)"""
        self.event_callback_del(EVAS_CALLBACK_RESTACK, func)

    def on_del_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_DEL, ...)

        This is called before freeing object resources (see
        EVAS_CALLBACK_FREE).

        Expected signature: C{function(object, *args, **kargs)}
        """
        self.event_callback_add(EVAS_CALLBACK_DEL, func, *a, **k)

    def on_del_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_DEL, ...)"""
        self.event_callback_del(EVAS_CALLBACK_DEL, func)

    def on_hold_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_HOLD, ...)"""
        self.event_callback_add(EVAS_CALLBACK_HOLD, func, *a, **k)

    def on_hold_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_HOLD, ...)"""
        self.event_callback_del(EVAS_CALLBACK_HOLD, func)

    def on_changed_size_hints_add(self, func, *a, **k):
        """Same as event_callback_add(EVAS_CALLBACK_CHANGED_SIZE_HINTS, ...)"""
        self.event_callback_add(EVAS_CALLBACK_CHANGED_SIZE_HINTS, func, *a, **k)

    def on_changed_size_hints_del(self, func):
        """Same as event_callback_del(EVAS_CALLBACK_CHANGED_SIZE_HINTS, ...)"""
        self.event_callback_del(EVAS_CALLBACK_CHANGED_SIZE_HINTS, func)

    def pass_events_get(self):
        """pass_events_get()

        @return: Whether event processing is enabled for this object.
        @rtype: bool

        """
        return bool(evas_object_pass_events_get(self.obj))

    def pass_events_set(self, value):
        """pass_events_set(value)

        Enable or disable event processing by this object.

        If True, this will cause events on it to be ignored. They will be
        triggered on the next lower object (that is not set to pass events)
        instead.

        Objects that pass events will also not be accounted in some operations
        unless explicitly required, like L{Canvas.top_at_xy_get()},
        L{Canvas.top_in_rectangle_get()}, L{Canvas.objects_at_xy_get()},
        L{Canvas.objects_in_rectangle_get()}.

        @param value: Whether event processing is enabled for this object.
        @type value: bool

        """
        evas_object_pass_events_set(self.obj, value)

    property pass_events:
        """Whenever object should ignore and pass events.

        @type: bool

        """
        def __get__(self):
            return self.pass_events_get()

        def __set__(self, int value):
            self.pass_events_set(value)

    def repeat_events_get(self):
        """repeat_events_get()

        @return: Whether events continue propagation after being processed.
        @rtype: bool

        """
        return bool(evas_object_repeat_events_get(self.obj))

    def repeat_events_set(self, value):
        """repeat_events_set(value)

        Enable or disable events to continue propagation after being processed.

        If True, this will cause events on it to be processed but then
        they will be triggered on the next lower object (that is not set to
        pass events).

        @param value: Whether events continue propagation after being
            processed.
        @type value: bool

        """
        evas_object_repeat_events_set(self.obj, value)

    property repeat_events:
        """Whenever object should process and then repeat events.

        @type: bool

        """
        def __get__(self):
            return self.repeat_events_get()

        def __set__(self, int value):
            self.repeat_events_set(value)

    def propagate_events_get(self):
        """propagate_events_get()

        @return: Propagation of events to the objects parent.
        @rtype: bool

        """
        return bool(evas_object_propagate_events_get(self.obj))

    def propagate_events_set(self, value):
        """propagate_events_set(value)

        Enable or disable propagation of events to the objects parent.

        If True, this will cause events on this object to propagate to its
        L{SmartObject} parent, if it's a member of one.

        @param value: Propagation of events to the objects parent.
        @type value: bool

        """
        evas_object_propagate_events_set(self.obj, value)

    property propagate_events:
        """Whenever object should propagate events to its parent.

        @type: bool

        """
        def __get__(self):
            return self.propagate_events_get()

        def __set__(self, int value):
            self.propagate_events_set(value)

    def pointer_mode_get(self):
        """pointer_mode_get()

        @return: How to deal with pointer events.
        @rtype: int

        """
        return <int>evas_object_pointer_mode_get(self.obj)

    def pointer_mode_set(self, int value):
        """pointer_mode_set(value)

        How to deal with pointer events.

        This function has direct effect on event callbacks related to mouse.

        If C{EVAS_OBJECT_POINTER_MODE_AUTOGRAB}, then when mouse is
        down at this object, events will be restricted to it as source, mouse
        moves, for example, will be emitted even if outside this object area.

        If C{EVAS_OBJECT_POINTER_MODE_NOGRAB}, then events will be emitted
        just when inside this object area.

        The default value is C{EVAS_OBJECT_POINTER_MODE_AUTOGRAB}.

        @param value: EVAS_OBJECT_POINTER_MODE_AUTOGRAB or
           EVAS_OBJECT_POINTER_MODE_NOGRAB
        @type value: Evas_Object_Pointer_Mode

        """
        evas_object_pointer_mode_set(self.obj, <Evas_Object_Pointer_Mode>value)

    property pointer_mode:
        """If pointer should be grabbed while processing events.

        @type: Evas_Object_Pointer_Mode

        """
        def __get__(self):
            return self.pointer_mode_get()

        def __set__(self, int value):
            self.pointer_mode_set(value)

    def parent_get(self):
        """parent_get()

        @return: Object's parent, in the case it was added to some SmartObject.
        @rtype: L{Object}

        """
        cdef Evas_Object *obj
        obj = evas_object_smart_parent_get(self.obj)
        return Object_from_instance(obj)

    property parent:
        """L{Object} that this object is member of, or C{None}.

        @type: L{Object}

        """
        def __get__(self):
            return self.parent_get()

    def map_enabled_set(self, enabled):
        evas_object_map_enable_set(self.obj, bool(enabled))
    
    def map_enabled_get(self):
        return bool(evas_object_map_enable_get(self.obj))

    property map_enabled:
        def __get__(self):
            return bool(evas_object_map_enable_get(self.obj))
        def __set__(self, value):
            evas_object_map_enable_set(self.obj, bool(value))
    
    def map_set(self, Map map):
        evas_object_map_set(self.obj, map.map)

    def map_get(self):
        # TODO dunno how to do this in a sane way
        #return evas_object_map_get(self.obj)
        return None

    property map:
        def __get__(self):
            return None # TODO
        def __set__(self, Map map):
            evas_object_map_set(self.obj, map.map)
    
    

cdef extern from "Evas.h": # hack to force type to be known
    cdef PyTypeObject PyEvasObject_Type # hack to install metaclass
_install_metaclass(&PyEvasObject_Type, EvasObjectMeta)
