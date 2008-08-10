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

# This file is included verbatim by edje.c_edje.pyx
cimport evas.c_evas as c_evas
import traceback


cdef void text_change_cb(void *data,
                         evas.c_evas.Evas_Object *obj, char *part) with gil:
    cdef Edje self
    self = <Edje>data
    if self._text_change_cb is None:
        return
    func, args, kargs = self._text_change_cb
    try:
        func(self, part, *args, **kargs)
    except Exception, e:
        traceback.print_exc()


cdef void message_handler_cb(void *data,
                             evas.c_evas.Evas_Object *obj,
                             Edje_Message_Type type,
                             int id, void *msg) with gil:
    cdef Edje self
    self = <Edje>data
    if self._message_handler_cb is None:
        return
    func, args, kargs = self._message_handler_cb
    try:
        func(self, Message_from_type(type, id, msg), *args, **kargs)
    except Exception, e:
        traceback.print_exc()


cdef void signal_cb(void *data,
                    evas.c_evas.Evas_Object *obj,
                    char *emission, char *source) with gil:
    cdef Edje self
    self = <Edje>evas.c_evas._Object_from_instance(<long>obj)
    lst = <object>data
    for func, args, kargs in lst:
        try:
            func(self, emission, source, *args, **kargs)
        except Exception, e:
            traceback.print_exc()


cdef _register_decorated_callbacks(obj):
    if not hasattr(obj, "__edje_signal_callbacks__"):
        return

    for attr_name, sig_data in obj.__edje_signal_callbacks__:
        attr_value = getattr(obj, attr_name)
        obj.signal_callback_add(sig_data[0], sig_data[1], attr_value)

    for attr_name in obj.__edje_message_callbacks__:
        attr_value = getattr(obj, attr_name)
        obj.message_handler_set(attr_value)

    for attr_name in obj.__edje_text_callbacks__:
        attr_value = getattr(obj, attr_name)
        obj.text_change_cb_set(attr_value)


class EdjeLoadError(Exception):
    "Exception to represent Edje load errors."
    def __init__(self, int code, char *file, char *group):
        if code == EDJE_LOAD_ERROR_NONE:
            msg = "No error"
        elif code == EDJE_LOAD_ERROR_GENERIC:
            msg = "Generic error"
        elif code == EDJE_LOAD_ERROR_DOES_NOT_EXIST:
            msg = "Does not exist"
        elif code == EDJE_LOAD_ERROR_PERMISSION_DENIED:
            msg = "Permission denied"
        elif code == EDJE_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED:
            msg = "Resource allocation failed"
        elif code == EDJE_LOAD_ERROR_CORRUPT_FILE:
            msg = "Corrupt file"
        elif code == EDJE_LOAD_ERROR_UNKNOWN_FORMAT:
            msg = "Unknown format"
        elif code == EDJE_LOAD_ERROR_INCOMPATIBLE_FILE:
            msg = "Incompatible file"
        elif code == EDJE_LOAD_ERROR_UNKNOWN_COLLECTION:
            msg = "Unknown collection"

        self.code = code
        self.file = file
        self.group = group
        Exception.__init__(self, "%s (file=%r, group=%r)" % (msg, file, group))


cdef public class Edje(evas.c_evas.Object) [object PyEdje, type PyEdje_Type]:
    """Edje evas object.

    This is a high level L{evas.SmartObject} that is defined as a group of
    parts (L{evas.c_evas.Object}, usually written in text files (.edc) and
    compiled as a package using EET to store resources (.edj).

    Edje is an important EFL component because it makes easy to split logic
    and UI, usually used as theme engine but can be much more powerful than
    just changing some random images or text fonts.

    Edje also provides scripting through Embryo and communication can be
    done using messages and signals.

    @warning: although Edje provides L{part_object_get()}, you should B{NOT}
      mess with these objects states or you'll screw the given Edje. The
      objects you get with this function should be handled as "read-only".
    @attention: messages are one way only! If you emit a message from Python
      you will just get it from your Embryo script, if you emit from Embryo
      you just get it in Python. If you want to emit events and capture
      them on the same side, use signals.
    @note: You can debug messages and signals by capturing all of them,
      example:
        >>> def sig_dbg(obj, emission, source):
        ...     print "%s: %s %s" % (obj, emission, source)
        ...
        >>> my_edje.signal_callback_add("*", "*", sig_dbg)
        >>> def msg_dbg(obj, msg):
        ...     print "%s: %s" % (obj, msg)
        ...
        >>> my_edje.message_handler_set(msg_dbg)
    """
    def __new__(self, *a, **ka):
        self._signal_callbacks = {}

    def __init__(self, c_evas.Canvas canvas not None, **kargs):
        evas.c_evas.Object.__init__(self, canvas)
        if self.obj == NULL:
            self._set_obj(edje_object_add(self.evas.obj))
        self._set_common_params(**kargs)

    def __free_wrapper_resources(self, ed):
        self._signal_callbacks.clear()
        self._text_change_cb = None
        self._message_handler_cb = None

    cdef int _set_obj(self, c_evas.Evas_Object *obj) except 0:
        cdef int r
        r = evas.c_evas.Object._set_obj(self, obj)
        if r == 1:
            self.event_callback_add(evas.c_evas.EVAS_CALLBACK_FREE,
                                    self.__free_wrapper_resources)
            _register_decorated_callbacks(self)
        return r

    def _set_common_params(self, file=None, group=None, size=None, pos=None,
                           geometry=None, color=None, name=None):
        if file:
            self.file_set(file, group)
        evas.c_evas.Object._set_common_params(self, size=size, pos=pos,
                                              geometry=geometry, color=color,
                                              name=name)
        if not size and not geometry:
            w, h = self.size_min_get()
            self.size_set(w, h)

    def __str__(self):
        x, y, w, h = self.geometry_get()
        r, g, b, a = self.color_get()
        file, group = self.file_get()
        name = self.name_get()
        if name:
            name_str = "name=%r, "
        else:
            name_str = ""
        clip = bool(self.clip_get() is not None)
        return ("%s(%sfile=%r, group=%r, geometry=(%d, %d, %d, %d), "
                "color=(%d, %d, %d, %d), layer=%s, clip=%s, visible=%s)") % \
               (self.__class__.__name__, name_str, file, group, x, y, w, h,
                r, g, b, a, self.layer_get(), clip, self.visible_get())

    def __repr__(self):
        x, y, w, h = self.geometry_get()
        r, g, b, a = self.color_get()
        file, group = self.file_get()
        clip = bool(self.clip_get() is not None)
        return ("%s(%#x, type=%r, refcount=%d, Evas_Object=%#x, name=%r, "
                "file=%r, group=%r, geometry=(%d, %d, %d, %d), "
                "color=(%d, %d, %d, %d), layer=%s, clip=%r, visible=%s)") % \
               (self.__class__.__name__, <unsigned long><void *>self,
                self.type_get(), PY_REFCOUNT(self), <unsigned long>self.obj,
                self.name_get(), file, group, x, y, w, h, r, g, b, a,
                self.layer_get(), clip, self.visible_get())

    def data_get(self, char *key):
        """Get data from Edje data collection (defined in .edj).

        Data collection is defined inside an Edje file as::

           collections {
              group {
                 name: "a_group";
                 data {
                    item: "key1" "value1";
                    item: "key2" "value2";
                 }
              }
           }

        @attention: this differs from L{Edje.data}! L{Edje.data} is a
          Python specific utility provided as a dictionary. This function
          returns data stored on the B{Edje} (.edj), stored inside a
          C{data} section inside the C{group} that defines this object.
        """
        cdef char *s
        s = edje_object_data_get(self.obj, key)
        if s != NULL:
            return s

    def file_set(self, char *file, char *group):
        "@raise EdjeLoadError: if error occurred during load."
        if edje_object_file_set(self.obj, file, group) == 0:
            raise EdjeLoadError(edje_object_load_error_get(self.obj),
                                file, group)

    def file_get(self):
        "@rtype: tuple for str"
        cdef char *file, *group
        edje_object_file_get(self.obj, &file, &group)
        if file == NULL:
            f = None
        else:
            f = file
        if group == NULL:
            p = None
        else:
            p = group
        return (f, p)

    def load_error_get(self):
        "@rtype: int"
        return edje_object_load_error_get(self.obj)

    def play_get(self):
        "@rtype: bool"
        return bool(edje_object_play_get(self.obj))

    def play_set(self, int value):
        "Set the Edje to play or pause."
        edje_object_play_set(self.obj, value)

    property play:
        def __get__(self):
            return self.play_get()

        def __set__(self, int value):
            self.play_set(value)

    def animation_get(self):
        "@rtype: bool"
        return bool(edje_object_animation_get(self.obj))

    def animation_set(self, int value):
        "Set animation state."
        edje_object_animation_set(self.obj, value)

    property animation:
        def __get__(self):
            return self.animation_get()

        def __set__(self, int value):
            self.animation_set(value)

    def freeze(self):
        """This puts all changes on hold.

           Successive freezes will nest, requiring an equal number of thaws.

           @rtype: int
        """
        return edje_object_freeze(self.obj)

    def thaw(self):
        "Thaw object."
        return edje_object_thaw(self.obj)

    def color_class_set(self, char *color_class,
                        int r, int g, int b, int a,
                        int r2, int g2, int b2, int a2,
                        int r3, int g3, int b3, int a3):
        """Set color class.

        @parm: B{color_class} color class name.
        @parm: B{r}
        @parm: B{g}
        @parm: B{b}
        @parm: B{a}
        @parm: B{r2}
        @parm: B{g2}
        @parm: B{b2}
        @parm: B{a2}
        @parm: B{r3}
        @parm: B{g3}
        @parm: B{b3}
        @parm: B{a3}
        """
        edje_object_color_class_set(self.obj, color_class,
                                    r, g, b, a,
                                    r2, g2, b2, a2,
                                    r3, g3, b3, a3)

    def color_class_del(self, char *color_class):
        edje_object_color_class_del(self.obj, color_class)

    def text_class_set(self, char *text_class, char *font, int size):
        """Set text class.

        @parm: B{text_class} text class name.
        @parm: B{font}
        @parm: B{size}
        """
        edje_object_text_class_set(self.obj, text_class, font, size)

    def size_min_get(self):
        "@rtype: tuple of int"
        cdef int w, h
        edje_object_size_min_get(self.obj, &w, &h)
        return (w, h)

    property size_min:
        def __get__(self):
            return self.size_min_get()

    def size_max_get(self):
        "@rtype: tuple of int"
        cdef int w, h
        edje_object_size_max_get(self.obj, &w, &h)
        return (w, h)

    property size_max:
        def __get__(self):
            return self.size_max_get()

    def calc_force(self):
        "Force recalculation of parts state (geometry, position, ...)"
        edje_object_calc_force(self.obj)

    def size_min_calc(self):
        "Request object to calculate minimum size."
        cdef int w, h
        edje_object_size_min_calc(self.obj, &w, &h)
        return (w, h)

    def part_exists(self, char *part):
        "@rtype: bool"
        return bool(edje_object_part_exists(self.obj, part))

    def part_object_get(self, char *part):
        """Get L{evas.c_evas.Object} that represents this part.

        @warning: You should never modify the state of the returned object
          (with L{Edje.move()} or L{Edje.hide()} for example),
          but you can safely query info about its current state
          (with L{Edje.visible_get()} or L{Edje.color_get()} for example).
        """
        cdef evas.c_evas.Evas_Object *o
        o = edje_object_part_object_get(self.obj, part)
        return evas.c_evas._Object_from_instance(<long>o)

    def part_geometry_get(self, char *part):
        "@rtype: tuple of int"
        cdef int x, y, w, h
        edje_object_part_geometry_get(self.obj, part, &x, &y, &w, &h)
        return (x, y, w, h)

    def part_size_get(self, char *part):
        "@rtype: tuple of int"
        cdef int w, h
        edje_object_part_geometry_get(self.obj, part, NULL, NULL, &w, &h)
        return (w, h)

    def part_pos_get(self, char *part):
        "@rtype: tuple of int"
        cdef int x, y
        edje_object_part_geometry_get(self.obj, part, &x, &y, NULL, NULL)
        return (x, y)

    def text_change_cb_set(self, func, *args, **kargs):
        """Set function to callback on text changes.

        Signature: C{function(object, part, *args, **kargs)}
        """
        if func is None:
            self._text_change_cb = None
            edje_object_text_change_cb_set(self.obj, NULL, NULL)
        elif callable(func):
            self._text_change_cb = (func, args, kargs)
            edje_object_text_change_cb_set(self.obj, text_change_cb,
                                           <void*>self)
        else:
            raise TypeError("func must be callable or None")

    def part_text_set(self, char *part, char *text):
        edje_object_part_text_set(self.obj, part, text)

    def part_text_get(self, char *part):
        "@rtype: str"
        cdef char *s
        s = edje_object_part_text_get(self.obj, part)
        if s == NULL:
            return None
        else:
            return s

    def part_swallow(self, char *part, c_evas.Object obj):
        """Swallows an object into the edje

        Swallows the object into the edje part so that all geometry changes
        for the part affect the swallowed object. (e.g. resize, move, show,
        raise/lower, etc.).

        If an object has already been swallowed into this part, then it will
        first be unswallowed before the new object is swallowed.
        """
        edje_object_part_swallow(self.obj, part, obj.obj)

    def part_unswallow(self, c_evas.Object obj):
        edje_object_part_unswallow(self.obj, obj.obj)

    def part_swallow_get(self, char *part):
        "@rtype: L{evas.c_evas.Object}"
        cdef evas.c_evas.Evas_Object *o
        o = edje_object_part_swallow_get(self.obj, part)
        return evas.c_evas._Object_from_instance(<long>o)

    def part_state_get(self, char *part):
        "@rtype: (name, value)"
        cdef double sv
        cdef char *sn
        sn = edje_object_part_state_get(self.obj, part, &sv)
        return (sn, sv)

    def part_drag_dir_get(self, char *part):
        "@rtype: int"
        return edje_object_part_drag_dir_get(self.obj, part)

    def part_drag_value_set(self, char *part, double dx, double dy):
        edje_object_part_drag_value_set(self.obj, part, dx, dy)

    def part_drag_value_get(self, char *part):
        "@rtype: tuple of float"
        cdef double dx, dy
        edje_object_part_drag_value_get(self.obj, part, &dx, &dy)
        return (dx, dy)

    def part_drag_size_set(self, char *part, double dw, double dh):
        edje_object_part_drag_size_set(self.obj, part, dw, dh)

    def part_drag_size_get(self, char *part):
        "@rtype: tuple of float"
        cdef double dw, dh
        edje_object_part_drag_size_get(self.obj, part, &dw, &dh)
        return (dw, dh)

    def part_drag_step_set(self, char *part, double dx, double dy):
        edje_object_part_drag_step_set(self.obj, part, dx, dy)

    def part_drag_step_get(self, char *part):
        "@rtype: tuple of float"
        cdef double dx, dy
        edje_object_part_drag_step_get(self.obj, part, &dx, &dy)
        return (dx, dy)

    def part_drag_step(self, char *part, double dx, double dy):
        edje_object_part_drag_step(self.obj, part, dx, dy)

    def part_drag_page_set(self, char *part, double dx, double dy):
        edje_object_part_drag_page_set(self.obj, part, dx, dy)

    def part_drag_page_get(self, char *part):
        "@rtype: tuple of float"
        cdef double dx, dy
        edje_object_part_drag_page_get(self.obj, part, &dx, &dy)
        return (dx, dy)

    def part_drag_page(self, char *part, double dx, double dy):
        edje_object_part_drag_page(self.obj, part, dx, dy)

    cdef void message_send_int(self, int id, int data):
        cdef Edje_Message_Int m
        m.val = data
        edje_object_message_send(self.obj, EDJE_MESSAGE_INT, id, <void*>&m)

    cdef void message_send_float(self, int id, float data):
        cdef Edje_Message_Float m
        m.val = data
        edje_object_message_send(self.obj, EDJE_MESSAGE_FLOAT, id, <void*>&m)

    cdef void message_send_str(self, int id, char *data):
        cdef Edje_Message_String m
        m.str = data
        edje_object_message_send(self.obj, EDJE_MESSAGE_STRING, id, <void*>&m)

    cdef void message_send_str_set(self, int id, data):
        cdef int count, i
        cdef Edje_Message_String_Set *m

        count = len(data)
        m = <Edje_Message_String_Set*>python.PyMem_Malloc(
            sizeof(Edje_Message_String_Set) + (count - 1) * sizeof(char *))

        m.count = count
        i = 0
        for s in data:
            m.str[i] = s
            i = i + 1

        edje_object_message_send(self.obj, EDJE_MESSAGE_STRING_SET, id,
                                 <void*>m)
        python.PyMem_Free(m)

    cdef void message_send_str_int(self, int id, char *s, int i):
        cdef Edje_Message_String_Int m
        m.str = s
        m.val = i
        edje_object_message_send(self.obj, EDJE_MESSAGE_STRING_INT, id,
                                 <void*>&m)

    cdef void message_send_str_float(self, int id, char *s, float f):
        cdef Edje_Message_String_Float m
        m.str = s
        m.val = f
        edje_object_message_send(self.obj, EDJE_MESSAGE_STRING_FLOAT, id,
                                 <void*>&m)

    cdef void message_send_str_int_set(self, int id, char *s, data):
        cdef int count, i
        cdef Edje_Message_String_Int_Set *m

        count = len(data)
        m = <Edje_Message_String_Int_Set*>python.PyMem_Malloc(
            sizeof(Edje_Message_String_Int_Set) + (count - 1) * sizeof(int))

        m.str = s
        m.count = count
        i = 0
        for f in data:
            m.val[i] = f
            i = i + 1

        edje_object_message_send(self.obj, EDJE_MESSAGE_STRING_INT_SET, id,
                                 <void*>m)
        python.PyMem_Free(m)

    cdef void message_send_str_float_set(self, int id, char *s, data):
        cdef int count, i
        cdef Edje_Message_String_Float_Set *m

        count = len(data)
        m = <Edje_Message_String_Float_Set*>python.PyMem_Malloc(
            sizeof(Edje_Message_String_Float_Set) +
            (count - 1) * sizeof(double))

        m.str = s
        m.count = count
        i = 0
        for f in data:
            m.val[i] = f
            i = i + 1

        edje_object_message_send(self.obj, EDJE_MESSAGE_STRING_FLOAT_SET, id,
                                 <void*>m)
        python.PyMem_Free(m)

    cdef void message_send_int_set(self, int id, data):
        cdef int count, i
        cdef Edje_Message_Int_Set *m

        count = len(data)
        m = <Edje_Message_Int_Set*>python.PyMem_Malloc(
            sizeof(Edje_Message_Int_Set) + (count - 1) * sizeof(int))

        m.count = count
        i = 0
        for f in data:
            m.val[i] = f
            i = i + 1

        edje_object_message_send(self.obj, EDJE_MESSAGE_INT_SET, id,
                                 <void*>m)
        python.PyMem_Free(m)

    cdef void message_send_float_set(self, int id, data):
        cdef int count, i
        cdef Edje_Message_Float_Set *m

        count = len(data)
        m = <Edje_Message_Float_Set*>python.PyMem_Malloc(
            sizeof(Edje_Message_Float_Set) + (count - 1) * sizeof(double))

        m.count = count
        i = 0
        for f in data:
            m.val[i] = f
            i = i + 1

        edje_object_message_send(self.obj, EDJE_MESSAGE_FLOAT_SET, id,
                                 <void*>m)
        python.PyMem_Free(m)

    cdef message_send_set(self, int id, data):
        second_item = data[1]
        item_type = type(second_item)
        for e in data[2:]:
            if type(e) != item_type:
                raise TypeError("every element of data should be the "
                                "same type '%s'" % item_type.__name__)
        head = data[0]
        if isinstance(head, (int, long)):
            self.message_send_int_set(id, data)
        elif isinstance(head, float):
            self.message_send_float_set(id, data)
        elif isinstance(head, basestring):
            if issubclass(item_type, basestring):
                self.message_send_str_set(id, data)
            elif item_type == int or item_type == long:
                if len(data) == 2:
                    self.message_send_str_int(id, head, second_item)
                else:
                    self.message_send_str_int_set(id, head, data[2:])
            elif item_type == float:
                if len(data) == 2:
                    self.message_send_str_float(id, head, second_item)
                else:
                    self.message_send_str_float_set(id, head, data[2:])

    def message_send(self, int id, data):
        """Send message with given id and data.

        Data should be pure-python types that will be converted to
        the L{Message} subclass that better fits it. Supported are:
         - long, int, float, str
         - list of long, int, float, str
         - str and one of long, int, float
         - str and a list of one of long, int, float

        Messages sent will B{NOT} be available at Python-side (ie:
        L{message_handler_set()}), but just at Embryo-side.

        @raise TypeError: if data has no supported EdjeMessage counterpart.
        """
        if isinstance(data, (long, int)):
            self.message_send_int(id, data)
        elif isinstance(data, float):
            self.message_send_float(id, data)
        elif isinstance(data, basestring):
            self.message_send_str(id, data)
        elif isinstance(data, (tuple, list)):
            if len(data) < 1:
                return
            if len(data) < 2:
                self.message_send(id, data[0])
                return

            if not isinstance(data[0], (long, int, float, basestring)):
                raise TypeError("invalid message list type '%s'" %
                                type(data[0]).__name__)

            self.message_send_set(id, data)
        else:
            raise TypeError("invalid message type '%s'" % type(data).__name__)

    def message_handler_set(self, func, *args, **kargs):
        """Set the handler of messages comming from Embryo.

        Signature: C{function(object, message, *args, **kargs)}

        @attention: this just handle messages sent from Embryo.
        @raise TypeError: if func is not callable or None.
        """
        if func is None:
            self._message_handler_cb = None
            edje_object_message_handler_set(self.obj, NULL, NULL)
        elif callable(func):
            self._message_handler_cb = (func, args, kargs)
            edje_object_message_handler_set(self.obj, message_handler_cb,
                                           <void*>self)
        else:
            raise TypeError("func must be callable or None")

    def message_signal_process(self):
        "Manually iterate message signal system."
        edje_object_message_signal_process(self.obj)

    def signal_callback_add(self, char *emission, char *source, func,
                            *args, **kargs):
        """Add callback to given signal (emission, source).

        Signature: C{function(object, emission, source, *args, **kargs)}

        @parm: B{emission} the emission to listen, may be or contain '*' to
          match multiple.
        @parm: B{source} the emission's source to listen, may be or contain
          '*' to match multiple.
        @parm: B{func} the callable to use. Will get any further arguments
          you gave to signal_callback_add().

        @raise TypeError: if func is not callable.
        """
        if not callable(func):
            raise TypeError("func must be callable")

        lst = self._signal_callbacks.setdefault((emission, source), [])
        if not lst:
            edje_object_signal_callback_add(self.obj, emission, source,
                                            signal_cb, <void*>lst)
        lst.append((func, args, kargs))

    def signal_callback_del(self, char *emission, char *source, func):
        "Remove the callable associated with given emission and source."
        key = (emission, source)
        lst = self._signal_callbacks[key]
        i = -1
        for i, (f, a, k) in enumerate(lst):
            if func == f:
                break
        del lst[i]
        if not lst:
            del self._signal_callbacks[key]
            edje_object_signal_callback_del(self.obj, emission, source,
                                            signal_cb)

    def signal_emit(self, char *emission, char *source):
        "Emit signal with B{emission} and B{source}"
        edje_object_signal_emit(self.obj, emission, source)


class EdjeObjectMeta(evas.c_evas.EvasObjectMeta):
    def __init__(cls, name, bases, dict_):
        evas.c_evas.EvasObjectMeta.__init__(cls, name, bases, dict_)
        cls._fetch_callbacks()

    def _fetch_callbacks(cls):
        if "__edje_signal_callbacks__" in cls.__dict__:
            return

        cls.__edje_signal_callbacks__ = []
        cls.__edje_message_callbacks__ = []
        cls.__edje_text_callbacks__ = []

        sig_append = cls.__edje_signal_callbacks__.append
        msg_append = cls.__edje_message_callbacks__.append
        txt_append = cls.__edje_text_callbacks__.append

        for name in dir(cls):
            val = getattr(cls, name)
            if not callable(val):
                continue

            if hasattr(val, "edje_signal_callback"):
                sig_data = getattr(val, "edje_signal_callback")
                sig_append((name, sig_data))
            elif hasattr(val, "edje_message_handler"):
                msg_append(name)
            elif hasattr(val, "edje_text_change_callback"):
                txt_append(name)


cdef extern from "Python.h":
    cdef python.PyTypeObject PyEdje_Type # hack to install metaclass

_install_metaclass(&PyEdje_Type, EdjeObjectMeta)


evas.c_evas._object_mapping_register("edje", Edje)
