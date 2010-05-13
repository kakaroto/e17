# Copyright (c) 2009 Boris Faure
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#


cdef class Separator(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_separator_add(parent.obj))

    def horizontal_set(self, b):
        elm_separator_horizontal_set(self.obj, b)

    def horizontal_get(self):
        return elm_separator_horizontal_get(self.obj)

    property horizontal:
        def __get__(self):
            return self.horizontal_get()

        def __set__(self, value):
            self.horizontal_set(value)


_elm_widget_type_register("separator", Separator)
