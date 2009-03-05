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

cdef class Object(evas.c_evas.Object):
    """
    elementary.Object

    An abstract class to manage object and callback handling. All 
    widgets are based on this class
    """

    
    def _callback_add(self, event, func):
        """Add a callback for this object

        Add a function as new callback-function for a specified event. The
        function will be called, if the event occured.

        @parm: B{event} Name of the event
        @parm: B{func} Function should be called, when the event occured
        """
        if not callable(func):
            raise TypeError("func is not callable")
        
        # implement per object event <> func list in global var _callback_mappings
        # _object_callback looks for the object, saved in Evas_Object in the callback list
        # and calls every func that matched the event
        
        mapping = _callback_mappings.get(<long>self.obj, None)
        if mapping is None:
            mapping = dict()
            mapping["__class__"] =  self
            mapping[event] = func
            _callback_mappings[<long>self.obj] = mapping
        else:
            mapping[event] = func
            _callback_mappings[<long>self.obj] = mapping
        
        # register callback
        e = event
        c_evas.evas_object_smart_callback_add(self.obj, event, _object_callback, <char *>e)
            
    def _callback_remove(self, event):
        """Removes all callback functions for the event
        
        Will remove all callback functions for the specified event. 

        @parm: B{event} Name of the event whose events should be removed
        """

        mapping = _callback_mappings.get(<long>self.obj, None)
        if mapping is not None:
            mapping.pop(event)
            _callback_mappings[<long>self.obj] = mapping

