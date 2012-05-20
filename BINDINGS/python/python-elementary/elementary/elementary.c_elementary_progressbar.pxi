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

cdef class Progressbar(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_progressbar_add(parent.obj))

    def pulse_set(self, pulse):
        elm_progressbar_pulse_set(self.obj, pulse)

    def pulse_get(self):
        return elm_progressbar_pulse_get(self.obj)

    def pulse(self, state):
        elm_progressbar_pulse(self.obj, state)

    def value_set(self, value):
        elm_progressbar_value_set(self.obj, value)

    def value_get(self):
        return elm_progressbar_value_get(self.obj)

    property value:
        def __get__(self):
            return self.value_get()

        def __set__(self, value):
            self.value_set(value)

    def span_size_set(self, size):
        elm_progressbar_span_size_set(self.obj, size)

    def span_size_get(self):
        return elm_progressbar_span_size_get(self.obj)

    property span_size:
        def __get__(self):
            return self.span_size_get()

        def __set__(self, size):
            self.span_size_set(size)

    def unit_format_set(self, format):
        if format is None:
            elm_progressbar_unit_format_set(self.obj, NULL)
        else:
            elm_progressbar_unit_format_set(self.obj, format)

    def unit_format_get(self):
        return elm_progressbar_unit_format_get(self.obj)

    property unit_format:
        def __get__(self):
            self.unit_format_get()

        def __set__(self, format):
            self.unit_format_set(format)

    def horizontal_set(self, horizontal):
        elm_progressbar_horizontal_set(self.obj, horizontal)

    def horizontal_get(self):
        return bool(elm_progressbar_horizontal_get(self.obj))

    property horizontal:
        def __get__(self):
            return self.horizontal_get()

        def __set__(self, horizontal):
            self.horizontal_set(horizontal)

    def inverted_set(self, inverted):
        elm_progressbar_inverted_set(self.obj, inverted)

    def inverted_get(self):
        return bool(elm_progressbar_inverted_get(self.obj))

    property inverted:
        def __get__(self):
            return self.inverted_get()

        def __set__(self, inverted):
            self.inverted_set(inverted)

_elm_widget_type_register("progressbar", Progressbar)
