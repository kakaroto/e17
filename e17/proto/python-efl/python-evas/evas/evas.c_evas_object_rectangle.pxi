# This file is included verbatim by c_evas.pyx

cdef class Rectangle(Object):
    def __init__(self, Canvas canvas not None, **kargs):
        Object.__init__(self, canvas)
        if self.obj == NULL:
            self._set_obj(evas_object_rectangle_add(self._evas.obj))
        self._set_common_params(**kargs)
