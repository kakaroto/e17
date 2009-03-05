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

cdef class Bubble(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_bubble_add(parent.obj))
        
    def label_set(self, label):
        elm_bubble_label_set(self.obj, label)
        
    def info_set(self, info):
        elm_bubble_info_set(self.obj, info)
        
    def content_set(self, c_evas.Object content):
        elm_bubble_content_set(self.obj, content.obj)
    
    def icon_set(self, c_evas.Object icon):
        elm_bubble_icon_set(self.obj, icon.obj)
        
    def corner_set(self, corner):
        elm_bubble_corner_set(self.obj, corner)
