# This file is included verbatim by c_evas.pyx

cdef public class Rectangle(Object) [object PyEvasRectangle,
                                     type PyEvasRectangle_Type]:
    def __init__(self, Canvas canvas not None, **kargs):
        Object.__init__(self, canvas)
        if self.obj == NULL:
            self._set_obj(evas_object_rectangle_add(self.evas.obj))
        self._set_common_params(**kargs)


cdef extern from "Python.h":
    cdef python.PyTypeObject PyEvasRectangle_Type # hack to install metaclass

_install_metaclass(&PyEvasRectangle_Type, EvasObjectMeta)
