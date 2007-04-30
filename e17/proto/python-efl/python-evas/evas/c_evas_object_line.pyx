# This file is included verbatim by c_evas.pyx

cdef class Line(Object):
    def _new_obj(self):
        if self.obj == NULL:
            self._set_obj(evas_object_line_add(self._evas.obj))


    def xy_set(self, int x1, int y1, int x2, int y2):
        evas_object_line_xy_set(self.obj, x1, y1, x2, y2)

    def xy_get(self):
        cdef int x1, y1, x2, y2
        evas_object_line_xy_get(self.obj, &x1, &y1, &x2, &y2)
        return (x1, y1, x2, y2)

    def start_set(self, x1, y1):
        cdef int x2, y2
        evas_object_line_xy_get(self.obj, NULL, NULL, &x2, &y2)
        evas_object_line_xy_set(self.obj, x1, y1, x2, y2)

    def start_get(self):
        cdef int x1, y1
        evas_object_line_xy_get(self.obj, &x1, &y1, NULL, NULL)
        return (x1, y1)

    property start:
        def __set__(self, spec):
            cdef int x1, y1, x2, y2
            x1, y1 = spec
            evas_object_line_xy_get(self.obj, NULL, NULL, &x2, &y2)
            evas_object_line_xy_set(self.obj, x1, y1, x2, y2)

        def __get__(self):
            cdef int x1, y1
            evas_object_line_xy_get(self.obj, &x1, &y1, NULL, NULL)
            return (x1, y1)

    def end_set(self, x2, y2):
        cdef int x1, y1
        evas_object_line_xy_get(self.obj, &x1, &y1, NULL, NULL)
        evas_object_line_xy_set(self.obj, x1, y1, x2, y2)

    def end_get(self):
        cdef int x2, y2
        evas_object_line_xy_get(self.obj, NULL, NULL, &x2, &y2)
        return (x2, y2)

    property end:
        def __set__(self, spec):
            cdef int x1, y1, x2, y2
            x2, y2 = spec
            evas_object_line_xy_get(self.obj, &x1, &y1, NULL, NULL)
            evas_object_line_xy_set(self.obj, x1, y1, x2, y2)

        def __get__(self):
            cdef int x2, y2
            evas_object_line_xy_get(self.obj, NULL, NULL, &x2, &y2)
            return (x2, y2)

