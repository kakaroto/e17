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

cdef class InnerWindow(Window):
    """
    An inner window 
    """
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_win_inwin_add(parent.obj))
        
    def activate(self):
        """
        Activate the window
        """
        elm_win_inwin_activate(self.obj)
        
    def style_set(self, style):
        """
        Set the style for the inner window. Available styles are
        - default
        - minimal
        - minimal_vertical
        
        @parm: B{style} style for the inner window
        """
        elm_win_inwin_style_set(self.obj, style)
    

