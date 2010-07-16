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

cdef class Flip(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_flip_add(parent.obj))

    def content_front_set(self, c_evas.Object content):
        elm_flip_content_front_set(self.obj, content.obj)

    def content_back_set(self, c_evas.Object content):
        elm_flip_content_back_set(self.obj, content.obj)

    def go(self, flip_mode):
        elm_flip_go(self.obj, flip_mode)

_elm_widget_type_register("flip", Flip)
