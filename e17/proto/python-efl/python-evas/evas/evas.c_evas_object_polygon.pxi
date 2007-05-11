# This file is included verbatim by c_evas.pyx

cdef class Polygon(Object):
    def _new_obj(self):
        if self.obj == NULL:
            self._set_obj(evas_object_polygon_add(self._evas.obj))

    def _set_common_params(self, points=None, size=None, pos=None,
                           geometry=None, color=None, name=None):
        Object._set_common_params(self, size=size, pos=pos, geometry=geometry,
                                  color=color, name=name)
        if points:
            for x, y in points:
                self.point_add(x, y)

    def point_add(self, int x, int y):
        evas_object_polygon_point_add(self.obj, x, y)

    def points_clear(self):
        evas_object_polygon_points_clear(self.obj)
