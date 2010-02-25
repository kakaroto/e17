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

cdef class Spectra:
    cdef EdjeEdit edje
    cdef object _name

    def __init__(self, EdjeEdit edje, char *name):
        self.edje = edje
        self._name = name

    property name:
        def __get__(self):
            return self._name

        def __set__(self, char *newname):
            self.rename(newname)

    def rename(self, char *newname):
        cdef unsigned char r
        r = edje_edit_spectra_name_set(self.edje.obj, self.name, newname)
        if r == 0:
            return False
        self._name = newname
        return True

    def stop_num_get(self):
        return edje_edit_spectra_stop_num_get(self.edje.obj, self.name)

    def stop_num_set(self, int num):
        edje_edit_spectra_stop_num_set(self.edje.obj, self.name, num)

    def stop_color_get(self, int stop_num):
        cdef int r, g, b, a, d
        edje_edit_spectra_stop_color_get(self.edje.obj, self.name, stop_num,
                                         &r, &g, &b, &a, &d)
        return (r, g, b, a, d)

    def stop_color_set(self, int stop_num, int r, int g, int b, int a, int d):
        edje_edit_spectra_stop_color_set(self.edje.obj, self.name, stop_num,
                                         r, g, b, a, d)
