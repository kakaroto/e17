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

    def label_set(self, label):
        _METHOD_DEPRECATED(self, "text_set")
        self.text_set(label)

    def label_get(self):
        _METHOD_DEPRECATED(self, "text_get")
        return self.text_get()

    property label:
        def __get__(self):
            return self.label_get()

        def __set__(self, label):
            self.label_set(label)

    def fontsize_set(self, int fontsize):
        """Set the label fontsize"""
        elm_label_fontsize_set(self.obj, fontsize)

    def align_set(self, char *alignmode):
        """Set the label text alignment"""
        elm_label_text_align_set(self.obj, alignmode)

    def text_color_set(self, int r, int g, int b, int a):
        """Set the label text color"""
        elm_label_text_color_set(self.obj, r, g, b, a)

_elm_widget_type_register("label", Label)
