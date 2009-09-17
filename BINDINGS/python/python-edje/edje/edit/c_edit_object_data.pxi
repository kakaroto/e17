# Copyright (C) 2007-2008 Tiago Falcao
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Edje.  If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by edje.edit.pyx


cdef class Data:
    cdef EdjeEdit edje
    cdef object name

    def __init__(self, EdjeEdit edje, char *name):
        self.edje = edje
        self.name = name

    property value:
        def __get__(self):
            cdef char *val
            val = edje_edit_data_value_get(self.edje.obj, self.itemname)
            if val == NULL: return None
            r = val
            edje_edit_string_free(val)
            return r
        def __set__(self, value):
            edje_edit_data_value_set(self.edje.obj, self.itemname, value)

    def rename(self, char * newname):
        cdef unsigned char r
        r = edje_edit_data_name_set(self.edje.obj, self.itemname, newname)
        if r == 0:
            return False
        return True

