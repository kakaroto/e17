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

cdef public class Frame(Object) [object PyElementaryFrame, type PyElementaryFrame_Type]:
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_frame_add(parent.obj))

    def autocollapse_set(self, autocollapse):
        elm_frame_autocollapse_set(self.obj, autocollapse)

    def autocollapse_get(self):
        return elm_frame_autocollapse_get(self.obj)

    property autocollapse:
        def __get__(self):
            return self.autocollapse_get()

        def __set__(self, value):
            self.autocollapse_set(value)

    def collapse_set(self, autocollapse):
        elm_frame_collapse_set(self.obj, autocollapse)

    def collapse_get(self):
        return elm_frame_collapse_get(self.obj)

    property collapse:
        def __get__(self):
            return self.collapse_get()

        def __set__(self, value):
            self.collapse_set(value)

    def collapse_go(self, collapse):
        elm_frame_collapse_go(self.obj, collapse)


    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

_elm_widget_type_register("frame", Frame)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryFrame_Type # hack to install metaclass
_install_metaclass(&PyElementaryFrame_Type, ElementaryObjectMeta)
