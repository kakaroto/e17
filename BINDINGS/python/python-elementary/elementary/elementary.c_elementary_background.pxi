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
   
cdef class Background(Object):
    """
    Background widget object
    
    This widget represent a background of a window. It holds a 
    image as background.
    """
    
    def __init__(self, c_evas.Object parent):
        """Initialize the background
        
        @parm: B{parent} Parent window
        """
        self._set_obj(elm_bg_add(parent.obj))
        
    def file_set(self, filename, group = ""):
        """
        Set the image for the background
        
        @parm: B{filename} filename of the background image
        @parm: B{group} 
        """
        elm_bg_file_set(self.obj, filename, group)


