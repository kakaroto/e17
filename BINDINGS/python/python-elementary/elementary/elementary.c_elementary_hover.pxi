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

cdef class Hover(Object):
    def __init__(self, c_evas.Object parent, obj = None):
        if obj is None:
            Object.__init__(self, parent.evas)
            self._set_obj(elm_hover_add(parent.obj))
        else:
            self._set_obj(<c_evas.Evas_Object*>obj)

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def target_set(self, c_evas.Object target):
        elm_hover_target_set(self.obj, target.obj)

    def parent_set(self, c_evas.Object parent):
        elm_hover_parent_set(self.obj, parent.obj)

    def content_set(self, swallow, c_evas.Object content):
        elm_hover_content_set(self.obj, swallow, content.obj)

    def best_content_location_get(self, axis):
        cdef char* string
        string = elm_hover_best_content_location_get(self.obj, axis)
        if string == NULL:
            return None
        return string


_elm_widget_type_register("hover", Hover)
