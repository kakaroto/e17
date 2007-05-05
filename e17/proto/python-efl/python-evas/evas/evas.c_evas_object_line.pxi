# This file is included verbatim by c_evas.pyx

cdef class Line(Object):
    def _new_obj(self):
        if self.obj == NULL:
            self._set_obj(evas_object_line_add(self._evas.obj))

    def _set_common_params(self, start=None, end=None, size=None, pos=None,
                           geometry=None, color=None, name=None):
        if start and end:
            x1 = start[0]
            y1 = start[1]
            x2 = end[0]
            y2 = end[1]

            w = x2 - x1
            h = y2 - y1
            if w < 0:
                w = -w
                x = x2
            else:
                x = x1

            if h < 0:
                h = -h
                y = y2
            else:
                y = y1

            self.xy_set(x1, y1, x2, y2)

            if not geometry:
                if not size:
                    self.size_set(w, h)
                if not pos:
                    self.pos_set(x, y)

        elif start:
            self.start_set(*start)
        elif end:
            self.end_set(*end)
        Object._set_common_params(self, size=size, pos=pos, geometry=geometry,
                                  color=color, name=name)

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

