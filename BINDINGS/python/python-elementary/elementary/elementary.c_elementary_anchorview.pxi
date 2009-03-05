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

cdef class AnchorView(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_anchorview_add(parent.obj))
        
    property anchor_clicked:
        def __set__(self, value):
            self._callback_add("anchor,clicked",value)

    def text_set(self, text):
        elm_anchorview_text_set(self.obj, text)

    def hover_parent_set(self, c_evas.Object parent):
        elm_anchorview_hover_parent_set(self.obj, parent.obj)

    def hover_style_set(self, style):
        elm_anchorview_hover_style_set(self.obj, style)

    def hover_end(self):
        elm_anchorview_hover_end(self.obj)
 
