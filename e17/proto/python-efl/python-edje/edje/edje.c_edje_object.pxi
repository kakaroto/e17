# This file is included verbatim by edje.c_edje.pyx

cdef void text_change_cb(void *data, evas.c_evas.Evas_Object *obj, char *part):
    cdef Edje self
    self = <Edje>data
    if self._text_change_cb is None:
        return
    func, args, kargs = self._text_change_cb
    try:
        func(self, part, *args, **kargs)
    except Exception, e:
        import traceback
        traceback.print_exc()


cdef void message_handler_cb(void *data, evas.c_evas.Evas_Object *obj,
                             Edje_Message_Type type, int id, void *msg):
    cdef Edje self
    self = <Edje>data
    if self._message_handler_cb is None:
        return
    func, args, kargs = self._message_handler_cb
    try:
        func(self, Message_from_type(type, id, msg), *args, **kargs)
    except Exception, e:
        import traceback
        traceback.print_exc()


cdef void signal_cb(void *data, evas.c_evas.Evas_Object *obj,
                    char *emission, char *source):
    cdef Edje self
    self = <Edje>evas.c_evas._Object_from_instance(<long>obj)
    lst = <object>data
    for func, args, kargs in lst:
        try:
            func(self, emission, source, *args, **kargs)
        except Exception, e:
            import traceback
            traceback.print_exc()


cdef _register_decorated_callbacks(obj):
    for attr_name in dir(obj):
        attr_value = getattr(obj, attr_name)
        if callable(attr_value):
            if hasattr(attr_value, "edje_signal_callback"):
                emission, source = getattr(attr_value, "edje_signal_callback")
                obj.signal_callback_add(emission, source, attr_value)
            elif hasattr(attr_value, "edje_text_change_callback"):
                obj.text_change_cb_set(attr_value)
            elif hasattr(attr_value, "edje_message_handler"):
                obj.message_handler_set(attr_value)


class EdjeLoadError(Exception):
    def __init__(self, int code):
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
        Exception.__init__(self, msg)


cdef class Edje(evas.c_evas.Object):
    def __new__(self, *a, **ka):
        self._signal_callbacks = {}

    def __init__(self, evas.c_evas.Canvas canvas not None, **kargs):
        evas.c_evas.Object.__init__(self, canvas)
        if self.obj == NULL:
            self._set_obj(edje_object_add(self._evas.obj))
            _register_decorated_callbacks(self)
        self._set_common_params(**kargs)

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
        return ("%s(%sfile=%r, group=%r, geometry=(%d, %d, %d, %d), "
                "color=(%d, %d, %d, %d), layer=%s, clip=%s, visible=%s)") % \
               (self.__class__.__name__, name_str, file, group, x, y, w, h,
                r, g, b, a, self.layer_get(), self.clip_get(),
                self.visible_get())

    def __repr__(self):
        x, y, w, h = self.geometry_get()
        r, g, b, a = self.color_get()
        file, group = self.file_get()
        return ("%s(0x%x, type=%r, refcount=%d, Evas_Object=0x%x, name=%r, "
                "file=%r, group=%r, geometry=(%d, %d, %d, %d), "
                "color=(%d, %d, %d, %d), layer=%s, clip=%r, visible=%s)") % \
               (self.__class__.__name__, <unsigned long>self,
                self.type_get(), PY_REFCOUNT(self), <unsigned long>self.obj,
                self.name_get(), file, group, x, y, w, h, r, g, b, a,
                self.layer_get(), self.clip_get(), self.visible_get())

    def data_get(self, char *key):
        cdef char *s
        s = edje_object_data_get(self.obj, key)
        if s != NULL:
            return s

    def file_set(self, char *file, char *part):
        if edje_object_file_set(self.obj, file, part) == 0:
            raise EdjeLoadError(edje_object_load_error_get(self.obj))

    def file_get(self):
        cdef char *file, *part
        edje_object_file_get(self.obj, &file, &part)
        if file == NULL:
            f = None
        else:
            f = file
        if part == NULL:
            p = None
        else:
            p = part
        return (f, p)

    def load_error_get(self):
        return edje_object_load_error_get(self.obj)

    def play_get(self):
        return bool(edje_object_play_get(self.obj))

    def play_set(self, int value):
        edje_object_play_set(self.obj, value)

    property play:
        def __get__(self):
            return self.play_get()

        def __set__(self, int value):
            self.play_set(value)

    def animation_get(self):
        return bool(edje_object_animation_get(self.obj))

    def animation_set(self, int value):
        edje_object_animation_set(self.obj, value)

    property animation:
        def __get__(self):
            return self.animation_get()

        def __set__(self, int value):
            self.animation_set(value)

    def freeze(self):
        return edje_object_freeze(self.obj)

    def thaw(self):
        return edje_object_thaw(self.obj)

    def color_class_set(self, char *color_class,
                        int r, int g, int b, int a,
                        int r2, int g2, int b2, int a2,
                        int r3, int g3, int b3, int a3):
        edje_object_color_class_set(self.obj, color_class,
                                    r, g, b, a,
                                    r2, g2, b2, a2,
                                    r3, g3, b3, a3)

    def color_class_del(self, char *color_class):
        edje_object_color_class_del(self.obj, color_class)

    def text_class_set(self, char *text_class, char *font, int size):
        edje_object_text_class_set(self.obj, text_class, font, size)

    def size_min_get(self):
        cdef int w, h
        edje_object_size_min_get(self.obj, &w, &h)
        return (w, h)

    property size_min:
        def __get__(self):
            return self.size_min_get()

    def size_max_get(self):
        cdef int w, h
        edje_object_size_max_get(self.obj, &w, &h)
        return (w, h)

    property size_max:
        def __get__(self):
            return self.size_max_get()

    def calc_force(self):
        edje_object_calc_force(self.obj)

    def size_min_calc(self):
        cdef int w, h
        edje_object_size_min_calc(self.obj, &w, &h)
        return (w, h)

    def part_exists(self, char *part):
        return bool(edje_object_part_exists(self.obj, part))

    def part_object_get(self, char *part):
        cdef evas.c_evas.Evas_Object *o
        o = edje_object_part_object_get(self.obj, part)
        return evas.c_evas._Object_from_instance(<long>o)

    def part_geometry_get(self, char *part):
        cdef int x, y, w, h
        edje_object_part_geometry_get(self.obj, part, &x, &y, &w, &h)
        return (x, y, w, h)

    def part_size_get(self, char *part):
        cdef int w, h
        edje_object_part_geometry_get(self.obj, part, NULL, NULL, &w, &h)
        return (w, h)

    def part_pos_get(self, char *part):
        cdef int x, y
        edje_object_part_geometry_get(self.obj, part, &x, &y, NULL, NULL)
        return (x, y)

    def text_change_cb_set(self, func, *args, **kargs):
        if func is None:
            self._text_change_cb = None
            edje_object_text_change_cb_set(self.obj, NULL, NULL)
        else:
            self._text_change_cb = (func, args, kargs)
            edje_object_text_change_cb_set(self.obj, text_change_cb,
                                           <void*>self)

    def part_text_set(self, char *part, char *text):
        edje_object_part_text_set(self.obj, part, text)

    def part_text_get(self, char *part):
        cdef char *s
        s = edje_object_part_text_get(self.obj, part)
        if s == NULL:
            return None
        else:
            return s

    def part_swallow(self, char *part, obj):
        cdef evas.c_evas.Object o
        if not isinstance(obj, evas.c_evas.Object):
            raise ValueError("Parameter \"obj\" should be of type evas.Object")
        o = obj
        edje_object_part_swallow(self.obj, part, o.obj)

    def part_unswallow(self, obj):
        cdef evas.c_evas.Object o
        if not isinstance(obj, evas.c_evas.Object):
            raise ValueError("Parameter \"obj\" should be of type evas.Object")
        o = obj
        edje_object_part_unswallow(self.obj, o.obj)

    def part_swallow_get(self, char *part):
        cdef evas.c_evas.Evas_Object *o
        o = edje_object_part_swallow_get(self.obj, part)
        return evas.c_evas._Object_from_instance(<long>o)

    def part_state_get(self, char *part):
        cdef double sv
        cdef char *sn
        sn = edje_object_part_state_get(self.obj, part, &sv)
        return (sn, sv)

    def part_drag_dir_get(self, char *part):
        return edje_object_part_drag_dir_get(self.obj, part)

    def part_drag_value_set(self, char *part, double dx, double dy):
        edje_object_part_drag_value_set(self.obj, part, dx, dy)

    def part_drag_value_get(self, char *part):
        cdef double dx, dy
        edje_object_part_drag_value_get(self.obj, part, &dx, &dy)
        return (dx, dy)

    def part_drag_size_set(self, char *part, double dw, double dh):
        edje_object_part_drag_size_set(self.obj, part, dw, dh)

    def part_drag_size_get(self, char *part):
        cdef double dw, dh
        edje_object_part_drag_size_get(self.obj, part, &dw, &dh)
        return (dw, dh)

    def part_drag_step_set(self, char *part, double dx, double dy):
        edje_object_part_drag_step_set(self.obj, part, dx, dy)

    def part_drag_step_get(self, char *part):
        cdef double dx, dy
        edje_object_part_drag_step_get(self.obj, part, &dx, &dy)
        return (dx, dy)

    def part_drag_step(self, char *part, double dx, double dy):
        edje_object_part_drag_step(self.obj, part, dx, dy)

    def part_drag_page_set(self, char *part, double dx, double dy):
        edje_object_part_drag_page_set(self.obj, part, dx, dy)

    def part_drag_page_get(self, char *part):
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

            item_type = type(data[0])
            if item_type not in (long, int, float, str, unicode):
                raise TypeError("invalid message list type '%s'" %
                                item_type.__name__)

            self.message_send_set(id, data)
        else:
            raise TypeError("invalid message type '%s'" % type(data).__name__)

    def message_handler_set(self, func, *args, **kargs):
        if func is None:
            self._message_handler_cb = None
            edje_object_message_handler_set(self.obj, NULL, NULL)
        else:
            self._message_handler_cb = (func, args, kargs)
            edje_object_message_handler_set(self.obj, message_handler_cb,
                                           <void*>self)

    def message_signal_process(self):
        edje_object_message_signal_process(self.obj)

    def signal_callback_add(self, char *emission, char *source, func,
                            *args, **kargs):
        lst = self._signal_callbacks.setdefault((emission, source), [])
        if not lst:
            edje_object_signal_callback_add(self.obj, emission, source,
                                            signal_cb, <void*>lst)
        lst.append((func, args, kargs))

    def signal_callback_del(self, char *emission, char *source, func):
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
        edje_object_signal_emit(self.obj, emission, source)


evas.c_evas._object_mapping_register("edje", Edje)
