# Copyright 2012 Kai Huuhko <kai.huuhko@gmail.com>
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

cdef class PaletteItem(ObjectItem):
    def __init__(self, c_evas.Object colorselector, r, g, b, a):
        self.obj = elm_colorselector_palette_color_add(colorselector.obj, r, g, b, a)

    def color_get(self):
        cdef int r, g, b, a
        elm_colorselector_palette_item_color_get(self.obj, &r, &g, &b, &a)
        return (r, g, b, a)

    def color_set(self, r, g, b, a):
        elm_colorselector_palette_item_color_set(self.obj, r, g, b, a)

    property color:
        def __get__(self):
            return self.color_get()
        def __set__(self, value):
            self.color_set(value)

def _colorselector_item_conv(long addr):
    cdef Elm_Object_Item *it = <Elm_Object_Item *>addr
    cdef void *data = elm_object_item_data_get(it)
    if data == NULL:
        return None
    else:
        prm = <object>data
        return prm[2]

cdef class Colorselector(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_colorselector_add(parent.obj))

    def color_set(self, r, g, b, a):
        elm_colorselector_color_set(self.obj, r, g, b, a)

    def color_get(self):
        cdef int r, g, b, a
        elm_colorselector_color_get(self.obj, &r, &g, &b, &a)
        return (r, g, b, a)

    property color:
        def __get__(self):
            return self.color_get()
        def __set__(self, value):
            self.color_set(value)

    def mode_set(self, mode):
        elm_colorselector_mode_set(self.obj, mode)

    def mode_get(self):
        return elm_colorselector_mode_get(self.obj)

    property mode:
        def __get__(self):
            return self.mode_get()
        def __set__(self, mode):
            self.mode_set(mode)

    def palette_clear(self):
        elm_colorselector_palette_clear(self.obj)

    def palette_name_set(self, palette_name):
        elm_colorselector_palette_name_set(self.obj, palette_name)

    def palette_name_get(self):
        return elm_colorselector_palette_name_get(self.obj)

    property palette_name:
        def __get__(self):
            return self.palette_name_get()
        def __set__(self, palette_name):
            self.palette_name_set(palette_name)

    def callback_selected_add(self, func, conv, *args, **kwargs):
        self._callback_add("selected", func, conv, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del("selected", func)

    def callback_color_item_selected_add(self, func, *args, **kwargs):
        self._callback_add_full("color,item,selected", _colorselector_item_conv, func, *args, **kwargs)

    def callback_color_item_selected_del(self, func):
        self._callback_del_full("color,item,selected", _colorselector_item_conv, func)

    def callback_color_item_longpressed_add(self, func, *args, **kwargs):
        self._callback_add_full("color,item,longpressed", _colorselector_item_conv, func, *args, **kwargs)

    def callback_color_item_longpressed_del(self, func):
        self._callback_del_full("color,item,longpressed", _colorselector_item_conv, func)
