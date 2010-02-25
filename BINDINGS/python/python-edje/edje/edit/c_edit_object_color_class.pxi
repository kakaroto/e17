# Copyright (C) 2007-2008 ProFUSION embedded systems
#
# This file is part of Python-Edje.
#
# Python-Edje is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Edje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Edje. If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by edje.edit.pyx

cdef class Color_Class:
    cdef EdjeEdit edje
    cdef object _name

    def __init__(self, EdjeEdit edje, char *name):
        self.edje = edje
        self._name = name

    property name:
        def __get__(self):
            return self._name

        def __set__(self, newname):
            self.rename(newname)

    def rename(self, char *newname):
        cdef unsigned char r
        r = edje_edit_color_class_name_set(self.edje.obj, self.name, newname)
        if r == 0:
            return False
        self._name = newname
        return True

    def colors_get(self):
        cdef int r, g, b, a
        cdef int r2, g2, b2, a2
        cdef int r3, g3, b3, a3
        edje_edit_color_class_colors_get(self.edje.obj, self.name,
                                         &r, &g, &b, &a,
                                         &r2, &g2, &b2, &a2,
                                         &r3, &g3, &b3, &a3)
        return ((r, g, b, a),
                (r2, g2, b2, a2),
                (r3, g3, b3, a3))

    def colors_set(self, int r, int g, int b, int a,
                   int r2, int g2, int b2, int a2,
                   int r3, int g3, int b3, int a3):
        edje_edit_color_class_colors_set(self.edje.obj, self.name,
                                         r, g, b, a,
                                         r2, g2, b2, a2,
                                         r3, g3, b3, a3)

