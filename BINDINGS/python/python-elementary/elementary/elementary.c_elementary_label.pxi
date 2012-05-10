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

cdef class Label(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_label_add(parent.obj))

    def line_wrap_set(self, Elm_Wrap_Type wrap):
        elm_label_line_wrap_set(self.obj, wrap)

    def line_wrap_get(self):
        return elm_label_line_wrap_get(self.obj)

    def wrap_width_set(self, int w):
        elm_label_wrap_width_set(self.obj, w)

    def wrap_width_get(self):
        return elm_label_wrap_width_get(self.obj)

    def ellipsis_set(self, bool ellipsis):
        elm_label_ellipsis_set(self.obj, ellipsis)

    def ellipsis_get(self):
        return elm_label_ellipsis_get(self.obj)

    def slide_set(self, bool slide):
        elm_label_slide_set(self.obj, slide)

    def slide_get(self):
        elm_label_slide_get(self.obj)

    def slide_duration_set(self, double duration):
        elm_label_slide_duration_set(self.obj, duration)

    def slide_duration_get(self):
        elm_label_slide_duration_get(self.obj)

_elm_widget_type_register("label", Label)
