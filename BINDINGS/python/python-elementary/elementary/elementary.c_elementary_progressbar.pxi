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

    def label_set(self, label):
        _METHOD_DEPRECATED(self, "text_set")
        self.text_set(label)

    def label_get(self):
        _METHOD_DEPRECATED(self, "text_get")
        return self.text_get()

    def icon_set(self, c_evas.Object icon):
        elm_progressbar_icon_set(self.obj, icon.obj)

    def span_size_set(self, size):
        elm_progressbar_span_size_set(self.obj, size)

    def horizontal_set(self, horizontal):
        elm_progressbar_horizontal_set(self.obj, horizontal)

    def inverted_set(self, inverted):
        elm_progressbar_inverted_set(self.obj, inverted)

    def pulse_set(self, pulse):
        elm_progressbar_pulse_set(self.obj, pulse)

    def pulse(self, state):
        elm_progressbar_pulse(self.obj, state)

    def unit_format_set(self, format):
        if format is None:
            elm_progressbar_unit_format_set(self.obj, NULL)
        else:
            elm_progressbar_unit_format_set(self.obj, format)

    def value_set(self, value):
        elm_progressbar_value_set(self.obj, value)

    def value_get(self):
        return elm_progressbar_value_get(self.obj)


_elm_widget_type_register("progressbar", Progressbar)
