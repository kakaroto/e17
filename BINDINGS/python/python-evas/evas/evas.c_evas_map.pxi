# Copyright (C) 2012 Davide Andreoli  <dave@gurumeditation.it>
#
# This file is part of Python-Evas.
#
# Python-Evas is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Evas is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Evas.  If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by c_evas.pyx



cdef public class Map(object) [object PyEvasMap, type PyEvasMap_Type]:
    cdef Evas_Map *map

    def __cinit__(self, *a, **ka):
        self.map = NULL

    def __init__(self, int count):
        self.map = evas_map_new(count)

    def delete(self):
        evas_map_free(self.map)
        self.map = NULL

    def util_points_populate_from_object_full(self, Object obj, Evas_Coord z):
        evas_map_util_points_populate_from_object_full(self.map, obj.obj, z)

    def util_points_populate_from_object(self, Object obj):
        evas_map_util_points_populate_from_object(self.map, obj.obj)

    def util_points_populate_from_geometry(self, Evas_Coord x, Evas_Coord y, Evas_Coord w, Evas_Coord h, Evas_Coord z):
        evas_map_util_points_populate_from_geometry(self.map, x, y, w, h, z)

    def util_points_color_set(self, int r, int g, int b, int a):
        evas_map_util_points_color_set(self.map, r, g, b, a)

    def util_rotate(self, double degrees, Evas_Coord cx, Evas_Coord cy):
        evas_map_util_rotate(self.map, degrees, cx, cy)

    def util_zoom(self, double zoomx, double zoomy, Evas_Coord cx, Evas_Coord cy):
        evas_map_util_zoom(self.map, zoomx, zoomy, cx, cy)
    
    def util_3d_rotate(self, double dx, double dy, double dz, Evas_Coord cx, Evas_Coord cy, Evas_Coord cz):
        evas_map_util_3d_rotate(self.map, dx, dy, dz, cx, cy, cz)

    def util_3d_lighting(self, Evas_Coord lx, Evas_Coord ly, Evas_Coord lz, int lr, int lg, int lb, int ar, int ag, int ab):
        evas_map_util_3d_lighting(self.map, lx, ly, lz, lr, lg, lb, ar, ag, ab)

    def util_3d_perspective(self, Evas_Coord px, Evas_Coord py, Evas_Coord z0, Evas_Coord foc):
        evas_map_util_3d_perspective(self.map, px, py, z0, foc)

    def util_clockwise_get(self):
        return bool(evas_map_util_clockwise_get(self.map))

    property util_clockwise:
        def __get__(self):
            return self.util_clockwise_get()

    def smooth_set(self, bool smooth):
        evas_map_smooth_set(self.map, smooth)

    def smooth_get(self):
        return bool(evas_map_smooth_get(self.map))

    property smooth:
        def __get__(self):
            return self.smooth_get()
        def __set__(self, value):
            self.smooth_set(value)

    def alpha_set(self, bool alpha):
        evas_map_alpha_set(self.map, alpha)

    def alpha_get(self):
        return bool(evas_map_alpha_get(self.map))

    property alpha:
        def __get__(self):
            return self.alpha_get()
        def __set__(self, value):
            self.alpha_set(value)

    def count_get(self):
        return evas_map_count_get(self.map)

    property count:
        def __get__(self):
            return self.count_get()
    
    def point_coord_set(self, int idx, Evas_Coord x, Evas_Coord y, Evas_Coord z):
        evas_map_point_coord_set(self.map, idx, x, y, z)

    def point_coord_get(self, int idx):
        cdef int x, y, z
        evas_map_point_coord_get(self.map, idx, &x, &y, &z)
        return (x, y, z)

    property point_coord:
        def __get__(self):
            return self.point_coord_get()
        def __set__(self, value):
            self.point_coord_set(*value)

    def point_image_uv_set(self, int idx, double u, double v):
        evas_map_point_image_uv_set(self.map, idx, u, v)

    def point_image_uv_get(self,  int idx):
        cdef double u, v
        evas_map_point_image_uv_get(self.map, idx, &u, &v)
        return (u, v)

    property point_image_uv:
        def __get__(self):
            return self.point_image_uv_get()
        def __set__(self, value):
            self.point_image_uv_set(*value)

    def point_color_set(self, int idx, int r, int g, int b, int a):
        evas_map_point_color_set(self.map, idx, r, g, b, a)

    def point_color_get(self, int idx):
        cdef int r, g, b, a
        evas_map_point_color_get(self.map, idx, &r, &g, &b, &a)
        return (r, g, b, a)

    property point_color:
        def __get__(self):
            return self.point_color_get()
        def __set__(self, value):
            self.point_color_set(*value)


cdef extern from "Evas.h": # hack to force type to be known
    cdef PyTypeObject PyEvasMap_Type # hack to install metaclass
_install_metaclass(&PyEvasMap_Type, EvasObjectMeta)
