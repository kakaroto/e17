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
        cdef const_char_ptr s
        s = elm_spinner_label_format_get(self.obj)
        if s == NULL:
            return ""
        return s

    property label_format:
        def __get__(self):
            return self.label_format_get()
    
        def __set__(self, value):
            self.label_format_set(value)

    def min_max_set(self, min, max):
        elm_spinner_min_max_set(self.obj, min, max)

    def min_max_get(self):
        cdef double min, max
        elm_spinner_min_max_get(self.obj, &min, &max)
        return (min, max)

    property min_max:
        def __get__(self):
            return self.min_max_get()
    
        def __set__(self, value):
            self.min_max_set(*value)
    
    def step_set(self, step):
        elm_spinner_step_set(self.obj, step)

    def step_get(self):
        return elm_spinner_step_get(self.obj)

    property step:
        def __get__(self):
            return self.step_get()

        def __set__(self, step):
            self.step_set(step)

    def value_set(self, value):
        elm_spinner_value_set(self.obj, value)

    def value_get(self):
        cdef double value
        value = elm_spinner_value_get(self.obj)
        return value

    property value:
        def __get__(self):
            return self.value_get()
        def __set__(self, value):
            self.value_set(value)

    def wrap_set(self, wrap):
        elm_spinner_wrap_set(self.obj, wrap)

    def wrap_get(self):
        return elm_spinner_wrap_get(self.obj)

    property wrap:
        def __get__(self):
            return self.wrap_get()
        def __set__(self, wrap):
            self.wrap_set(wrap)

    def editable_set(self, editable):
        elm_spinner_editable_set(self.obj, editable)

    def editable_get(self):
        return elm_spinner_editable_get(self.obj)

    property editable:
        def __get__(self):
            return self.editable_get()
        def __set__(self, editable):
            self.editable_set(editable)

    def special_value_add(self, value, label):
        elm_spinner_special_value_add(self.obj, value, label)

    def interval_set(self, interval):
        elm_spinner_interval_set(self.obj, interval)

    def interval_get(self):
        return elm_spinner_interval_get(self.obj)

    property interval:
        def __get__(self):
            return self.interval_get()

        def __set__(self, interval):
            self.interval_set(interval)

    def base_set(self, base):
        elm_spinner_base_set(self.obj, base)

    def base_get(self):
        return elm_spinner_base_get(self.obj)

    property base:
        def __get__(self):
            return self.base_get()

        def __set__(self, base):
            self.base_set(base)

    def round_set(self, rnd):
        elm_spinner_round_set(self.obj, rnd)

    def round_get(self):
        return elm_spinner_round_get(self.obj)

    property round:
        def __get__(self):
            return self.round_get()

        def __set__(self, rnd):
            self.round_set(rnd)

    def callback_changed_add(self, func, *args, **kwargs):
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

    def callback_delay_changed_add(self, func, *args, **kwargs):
        self._callback_add("delay,changed", func, *args, **kwargs)

    def callback_delay_changed_del(self, func):
        self._callback_del("delay,changed", func)


_elm_widget_type_register("spinner", Spinner)
