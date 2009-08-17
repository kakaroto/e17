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
    
cdef class Progressbar(Object):    
    def __init__(self, c_evas.Object parent):
        self._set_obj(elm_progressbar_add(parent.obj))
    
    def label_set(self, label):
        elm_progressbar_label_set(self.obj, label)
        
    def icon_set(self, c_evas.Object icon):
        elm_progressbar_icon_set(self.obj, icon.obj)
        
    def span_size_set(self, size):
        elm_progressbar_span_size_get(self, size)
    
    def horizontal_set(self, horizontal):
        if horizontal:
            elm_progressbar_horizontal_set(self.obj, 1)
        else:
            elm_progressbar_horizontal_set(self.obj, 0)
            
    def inverted_set(self, inverted):
        if inverted:
            elm_progressbar_inverted_set(self.obj, 1)
        else:
            elm_progressbar_inverted_set(self.obj, 0)
            
    def pulse_set(self, pulse):
        if pulse:
            elm_progressbar_pulse_set(self.obj, 1)
        else:
            elm_progressbar_pulse_set(self.obj, 0)
            
    def pulse(self, pulse):
        if inverted:
            elm_progressbar_pulse_set(self.obj, 1)
        else:
            elm_progressbar_pulse_set(self.obj, 0)
            
    def unit_format_set(self, format):
        elm_progressbar_unit_format_set(self.obj, format)
        
    def value_set(self, value):
        elm_progressbar_value_set(self.obj, value)
    
    def value_get(self):
        cdef double value
        value = elm_progressbar_value_get(self.obj)
        return value
        
        
    
    
        

