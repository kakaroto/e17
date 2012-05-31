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

cdef public class Table(Object) [object PyElementaryTable, type PyElementaryTable_Type]:
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_table_add(parent.obj))

    def homogeneous_set(self, homogeneous):
        elm_table_homogeneous_set(self.obj, homogeneous)

    def homogeneous_get(self):
        return elm_table_homogeneous_get(self.obj)

    property homogeneous:
        def __get__(self):
            return self.homogeneous_get()

        def __set__(self, value):
            self.homogeneous_set(value)

    def padding_set(self, horizontal, vertical):
        elm_table_padding_set(self.obj, horizontal, vertical)

    def padding_get(self):
        cdef c_evas.Evas_Coord horizontal, vertical
        elm_table_padding_get(self.obj, &horizontal, &vertical)
        return (horizontal, vertical)

    property padding:
        def __get__(self):
            return self.padding_get()

        def __set__(self, value):
            self.padding_set(*value)

    def pack(self, c_evas.Object subobj, x, y, w, h):
        elm_table_pack(self.obj, subobj.obj, x, y, w, h)

    def unpack(self, c_evas.Object subobj):
        elm_table_unpack(self.obj, subobj.obj)

    def clear(self, clear):
        elm_table_clear(self.obj, clear)

    def pack_set(c_evas.Object subobj, x, y, w, h):
        elm_table_pack_set(subobj.obj, x, y, w, h)

    def pack_get(c_evas.Object subobj):
        cdef int x, y, w, h
        elm_table_pack_get(subobj.obj, &x, &y, &w, &h)
        return (x, y, w, h)

_elm_widget_type_register("table", Table)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryTable_Type # hack to install metaclass
_install_metaclass(&PyElementaryTable_Type, ElementaryObjectMeta)
