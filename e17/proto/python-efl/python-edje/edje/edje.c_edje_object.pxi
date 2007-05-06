# This file is included verbatim by edje.c_edje.pyx

cdef void text_change_cb(void *data, evas.c_evas.Evas_Object *obj, char *part):
    cdef Edje self
    self = <Edje>data
    if self._text_change_cb is None:
        return
    func, args, kargs = self._text_change_cb
    func(self, part, *args, **kargs)


cdef void message_handler_cb(void *data, evas.c_evas.Evas_Object *obj,
                             Edje_Message_Type type, int id, void *msg):
    cdef Edje self
    self = <Edje>data
    if self._message_handler_cb is None:
        return
    func, args, kargs = self._message_handler_cb
    func(self, Message_from_type(type, id, msg), *args, **kargs)


cdef void signal_cb(void *data, evas.c_evas.Evas_Object *obj,
                    char *emission, char *source):
    cdef Edje self
    self = <Edje>data
    lst = self._signal_callbacks[(emission, source)]
    for func, args, kargs in lst:
        func(self, emission, source, *args, **kargs)


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
    def __new__(self, evas.c_evas.Canvas evas):
        self._signal_callbacks = {}

    def _new_obj(self):
        if self.obj == NULL:
            self._set_obj(edje_object_add(self._evas.obj))

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

    def data_get(self, char *key):
        return edje_object_data_get(self.obj, key)

    def file_set(self, char *file, char *part):
        if edje_object_file_set(self.obj, file, part) == 0:
            raise EdjeLoadError(edje_object_load_error_get(self.obj))

    def file_get(self):
        cdef char *file, *part
        edje_object_file_get(self.obj, &file, &part)
        return (file, part)

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
        return edje_object_part_text_get(self.obj, part)

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
        return evas.c_evas.Object_from_instance(<long>o)

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

#    def message_send(self, int type, int id, **kargs):
#        edje_object_message_send(self.obj, type, id, msg)

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
                                            signal_cb, <void*>self)
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

