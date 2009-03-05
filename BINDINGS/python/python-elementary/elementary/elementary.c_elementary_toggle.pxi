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

cdef class Toggle(Object):
    """
    A toggle widget
    """
    cdef int state_ptr
    
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_toggle_add(parent.obj))
        
        # set the state pointer for the toggle widget to always
        # know the current state
        self.state_ptr = 0
        elm_toggle_state_pointer_set(self.obj, &self.state_ptr)
        
        
    property changed:
        def __set__(self, value):
            self._callback_add("changed",value)
    
    def label_set(self, label):
        elm_toggle_label_set(self.obj, label)
        
    def icon_set(self, c_evas.Object icon):
        elm_toggle_icon_set(self.obj, icon.obj)
        
    def states_labels_set(self,onlabel, offlabel):
        elm_toggle_states_labels_set(self.obj, onlabel, offlabel)
        
    def state_set(self, state):
        if state == True:
            elm_toggle_state_set(self.obj, 1)
        elif state == False:
            elm_toggle_state_set(self.obj, 0)

    def state_get(self):
        if self.state_ptr == 1:
            return True
        return False


