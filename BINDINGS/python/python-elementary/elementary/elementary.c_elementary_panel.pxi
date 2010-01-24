# Copyright (c) 2010 Boris Faure
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

cdef class Panel(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_panel_add(parent.obj))

    def orient_set(self, orient):
        elm_panel_orient_set(self.obj, orient)

    property orient:
        def __set__(self, orient):
            elm_panel_orient_set(self.obj, orient)


    def content_set(self, c_evas.Object content):
        cdef c_evas.Evas_Object *o
        if content is not None:
            o = content.obj
        else:
            o = NULL
        elm_panel_content_set(self.obj, o)

    property content:
        def __set__(self, c_evas.Object content):
            self.content_set(content)
