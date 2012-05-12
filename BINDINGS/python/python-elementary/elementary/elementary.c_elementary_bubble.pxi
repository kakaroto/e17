# Copyright (c) 2008-2009 Simon Busch
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


cdef class Bubble(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_bubble_add(parent.obj))

    def pos_set(self, pos):
        elm_bubble_pos_set(self.obj, pos)

    def pos_get(self):
        return elm_bubble_pos_get(self.obj)

    property pos:
        def __get__(self):
            return self.pos_get()

        def __set__(self, value):
            self.pos_set(value)


_elm_widget_type_register("bubble", Bubble)
