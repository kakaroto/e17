# This file is included verbatim by c_evas.pyx

cdef class Rectangle(Object):
    def _new_obj(self):
        if self.obj == NULL:
            self._set_obj(evas_object_rectangle_add(self._evas.obj))
