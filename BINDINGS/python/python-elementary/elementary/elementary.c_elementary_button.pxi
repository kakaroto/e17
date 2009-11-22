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

    def callback_clicked_remove(self, func = None, *args, **kwargs):
        self._callback_remove("clicked", func, *args, **kwargs)

    def label_set(self, label):
        elm_button_label_set(self.obj, label)

    def icon_set(self, c_evas.Object icon):
        elm_button_icon_set(self.obj, icon.obj)

    def icon_get(self):
        cdef c_evas.Evas_Object *icon
        icon = elm_button_icon_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> icon)

    def style_set(self, style):
        elm_button_style_set(self.obj, style)

