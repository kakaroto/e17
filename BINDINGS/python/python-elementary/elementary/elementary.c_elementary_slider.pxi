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

    def label_set(self, label):
        _METHOD_DEPRECATED(self, "text_set")
        self.text_set(label)

    def label_get(self):
        _METHOD_DEPRECATED(self, "text_get")
        return self.text_get()

    def icon_set(self, c_evas.Object icon):
        elm_slider_icon_set(self.obj, icon.obj)

    def span_size_set(self, size):
        elm_slider_span_size_set(self.obj, size)

    def unit_format_set(self, format):
        elm_slider_unit_format_set(self.obj, format)

    def indicator_format_set(self, format):
        elm_slider_indicator_format_set(self.obj, format)

    def horizontal_set(self, horizontal):
        elm_slider_horizontal_set(self.obj, horizontal)

    def min_max_set(self, min, max):
        elm_slider_min_max_set(self.obj, min, max)


    def callback_changed_add(self, func, *args, **kwargs):
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

    def callback_delay_changed_add(self, func, *args, **kwargs):
        self._callback_add("delay,changed", func, *args, **kwargs)

    def callback_delay_changed_del(self, func):
        self._callback_del("delay,changed", func)


    property value:
        def __get__(self):
            cdef double value
            value = elm_slider_value_get(self.obj)
            return value

        def __set__(self, value):
            elm_slider_value_set(self.obj, value)

    def inverted_set(self, inverted):
        elm_slider_inverted_set(self.obj, inverted)


_elm_widget_type_register("slider", Slider)
