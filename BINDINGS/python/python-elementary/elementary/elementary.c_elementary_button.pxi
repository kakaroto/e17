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

cdef class Button(Object):
    def __init__(self,c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_button_add(parent.obj))

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def label_set(self, label):
        _METHOD_DEPRECATED(self, "text_set")
        self.text_set(label)

    def label_get(self):
        _METHOD_DEPRECATED(self, "text_get")
        return self.text_get()

    property label:
        def __get__(self):
            return self.label_get()

        def __set__(self, value):
            self.label_set(value)

    def icon_set(self, c_evas.Object icon):
        elm_button_icon_set(self.obj, icon.obj)

    def icon_get(self):
        cdef c_evas.Evas_Object *icon
        icon = elm_button_icon_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> icon)

_elm_widget_type_register("button", Button)
