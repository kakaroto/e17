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


cdef class Fileselector(Object):
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_fileselector_add(parent.obj))
        
    def selected_get(self):
        cdef char* path
        path = elm_fileselector_selected_get(self.obj)
        return path
    
    def path_set(self, path):
        elm_fileselector_path_set(self.obj, path)
    
    def expandable_set(self, expand):
        if expand:
            elm_fileselector_expandable_set(self.obj, 1)
        else
            elm_fileselector_expandable_set(self.obj, 0)
    
    
