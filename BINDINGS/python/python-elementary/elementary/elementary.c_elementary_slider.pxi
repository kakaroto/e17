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

cdef class Slider(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_slider_add(parent.obj))

    def span_size_set(self, size):
        elm_slider_span_size_set(self.obj, size)

    def span_size_get(self):
        return elm_slider_span_size_get(self.obj)

    property span_size:
        def __get__(self):
            return self.span_size_get()

        def __set__(self, size):
            self.span_size_set(size)

    def unit_format_set(self, format):
        elm_slider_unit_format_set(self.obj, format)

    def unit_format_get(self):
        return elm_slider_unit_format_get(self.obj)

    property unit_format:
        def __get__(self):
            return self.unit_format_get()

        def __set__(self, format):
            self.unit_format_set(format)

    def indicator_format_set(self, format):
        elm_slider_indicator_format_set(self.obj, format)

    def indicator_format_get(self):
        return elm_slider_indicator_format_get(self.obj)

    property indicator_format:
        def __get__(self):
            return self.indicator_format_get()

        def __set__(self, format):
            self.indicator_format_set(format)

    #TODO: def indicator_format_function_set()

    #TODO: def units_format_function_set()

    def horizontal_set(self, horizontal):
        elm_slider_horizontal_set(self.obj, horizontal)

    def horizontal_get(self):
        return bool(elm_slider_horizontal_get(self.obj))

    property horizontal:
        def __get__(self):
            return self.horizontal_get()
        def __set__(self, horizontal):
            self.horizontal_set(horizontal)

    def min_max_set(self, min, max):
        elm_slider_min_max_set(self.obj, min, max)

    def min_max_get(self):
        cdef double min, max
        elm_slider_min_max_get(self.obj, &min, &max)
        return (min, max)

    property min_max:
        def __get__(self):
            return self.value_get()

        def __set__(self, value):
            self.value_set(*value)

    def value_set(self, value):
        elm_slider_value_set(self.obj, value)

    def value_get(self):
        return elm_slider_value_get(self.obj)

    property value:
        def __get__(self):
            return self.value_get()
        def __set__(self, value):
            self.value_set(value)

    def inverted_set(self, inverted):
        elm_slider_inverted_set(self.obj, inverted)

    def inverted_get(self):
        return bool(elm_slider_inverted_get(self.obj))

    property inverted:
        def __get__(self):
            return self.inverted_get()

        def __set__(self, inverted):
            self.inverted_set(inverted)

    def indicator_show_set(self, show):
        elm_slider_indicator_show_set(self.obj, show)

    def indicator_show_get(self):
        return bool(elm_slider_indicator_show_get(self.obj))

    property indicator_show:
        def __get__(self):
            return self.indicator_show_get()

        def __set__(self, show):
            self.indicator_show_set(show)

    def callback_changed_add(self, func, *args, **kwargs):
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

    def callback_slider_drag_start_add(self, func, *args, **kwargs):
        self._callback_add("slider,drag,start", func, *args, **kwargs)

    def callback_slider_drag_start_del(self, func):
        self._callback_del("slider,drag,start", func)

    def callback_slider_drag_stop_add(self, func, *args, **kwargs):
        self._callback_add("slider,drag,stop", func, *args, **kwargs)

    def callback_slider_drag_stop_del(self, func):
        self._callback_del("slider,drag,stop", func)

    def callback_delay_changed_add(self, func, *args, **kwargs):
        self._callback_add("delay,changed", func, *args, **kwargs)

    def callback_delay_changed_del(self, func):
        self._callback_del("delay,changed", func)

_elm_widget_type_register("slider", Slider)
