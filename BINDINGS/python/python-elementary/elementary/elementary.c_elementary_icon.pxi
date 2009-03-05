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

cdef class Icon(Object):
    """
    A simple icon widget

    If you need a widget which presents a icon and is clickable, this widget
    is the best option for you.
    """
    def __init__(self, c_evas.Object parent):
        """
        @parm: B{parent} Parent Object
        """
        self._set_obj(elm_icon_add(parent.obj))


    def file_set(self, filename, group = None):
        """
        Set the path of the image for the icon

        @parm: B{filename} Filename of the image
        """
        if group == None:
            elm_icon_file_set(self.obj, filename, NULL)
        else:
            elm_icon_file_set(self.obj, filename, group)
        
    property clicked:
        """
        If the event clicked occured, the specified callback function will be 
        called.

        @parm: B{value} Callback function
        """
        def __set__(self, value):
            self._callback_add("clicked",value)

    def standard_set(self,standard):
        elm_icon_standard_set(self.obj,standard)

    def smooth_set(self,smooth):
        elm_icon_smooth_set(self.obj,smooth)

    def no_scale_set(self,no_scale):
        elm_icon_no_scale_set(self.obj,no_scale)

    def scale_set(self,scale_up,scale_down):
        elm_icon_scale_set(self.obj,scale_up,scale_down)

    def fill_outside_set(self,fill_outside):
        elm_icon_fill_outside_set(self.obj,fill_outside)


