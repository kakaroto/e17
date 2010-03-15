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


cdef class Group:
    cdef EdjeEdit edje

    def __init__(self, EdjeEdit edje):
        self.edje = edje

    def delete(self):
        """@rtype: bool"""
        return False

    property w_min:
        def __get__(self):
            return edje_edit_group_min_w_get(self.edje.obj)
        def __set__(self, value):
            edje_edit_group_min_w_set(self.edje.obj, value)

    property w_max:
        def __get__(self):
            return edje_edit_group_max_w_get(self.edje.obj)
        def __set__(self, value):
            edje_edit_group_max_w_set(self.edje.obj, value)

    property h_min:
        def __get__(self):
            return edje_edit_group_min_h_get(self.edje.obj)
        def __set__(self, value):
            edje_edit_group_min_h_set(self.edje.obj, value)

    property h_max:
        def __get__(self):
            return edje_edit_group_max_h_get(self.edje.obj)
        def __set__(self, value):
            edje_edit_group_max_h_set(self.edje.obj, value)

    def rename(self, char *name):
        return bool(edje_edit_group_name_set(self.edje.obj, name))
