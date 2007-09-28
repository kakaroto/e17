# This file is included verbatim by c_evas.pyx

cdef class Polygon(Object):
    def __init__(self, Canvas canvas not None, **kargs):
        Object.__init__(self, canvas)
        if self.obj == NULL:
            self._set_obj(evas_object_polygon_add(self.evas.obj))
        self._set_common_params(**kargs)

    def _set_common_params(self, points=None, **kargs):
        Object._set_common_params(self, **kargs)
        if points:
            for x, y in points:
                self.point_add(x, y)

    def point_add(self, int x, int y):
        """
        @parm: B{x}
        @parm: B{y}
        """
        evas_object_polygon_point_add(self.obj, x, y)

    def points_clear(self):
        evas_object_polygon_points_clear(self.obj)
