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

cdef class Spinner(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_spinner_add(parent.obj))

    def label_format_set(self, format):
        elm_spinner_label_format_set(self.obj, format)

    def label_format_get(self):
        cdef char* str
        str = elm_spinner_label_format_get(self.obj)
        if str == NULL:
            return ""
        return str

    def min_max_set(self, min, max):
        elm_spinner_min_max_set(self.obj, min, max)

    def step_set(self, step):
        elm_spinner_step_set(self.obj, step)

    def value_set(self, value):
        elm_spinner_value_set(self.obj, value)

    def value_get(self):
        cdef double value
        value = elm_spinner_value_get(self.obj)
        return value

    def wrap_set(self, wrap):
        elm_spinner_wrap_set(self.obj, wrap)


    def callback_changed_add(self, func, *args, **kwargs):
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

    def callback_delay_changed_add(self, func, *args, **kwargs):
        self._callback_add("delay,changed", func, *args, **kwargs)

    def callback_delay_changed_del(self, func):
        self._callback_del("delay,changed", func)


_elm_widget_type_register("spinner", Spinner)
