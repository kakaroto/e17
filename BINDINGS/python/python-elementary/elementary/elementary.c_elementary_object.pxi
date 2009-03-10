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

cdef object _callback_mappings
_callback_mappings = dict()

cdef void _object_callback(void *data, c_evas.Evas_Object *obj, void *event_info):
    try:
        mapping = _callback_mappings.get(<long>obj,None)
        if mapping is not None:
            func = mapping.get(<char*>data,None)
        
            if not callable(func):
                raise TypeError("func is not callable")
        
            func(mapping["__class__"],<char*>data, mapping.get("__data__", None))
    except Exception, e:
        traceback.print_exc()

cdef class Object(evas.c_evas.Object):
    """
    elementary.Object

    An abstract class to manage object and callback handling. All 
    widgets are based on this class
    """

    def scale_set(self, scale):
        elm_object_scale_set(self.obj, scale)
        
    def scale_get(self):
        cdef double scale
        scale = elm_object_scale_get(self.obj)
        return scale
    
    def focus(self):
        elm_object_focus(self.obj)
    
    def _callback_add(self, event, args, data = None):
        """Add a callback for this object

        Add a function as new callback-function for a specified event. The
        function will be called, if the event occured.

        @parm: B{event} Name of the event
        @parm: B{func} Function should be called, when the event occured
        """
        
        # implement per object event <> func list in global var _callback_mappings
        # _object_callback looks for the object, saved in Evas_Object in the callback list
        # and calls every func that matched the event
        
        # if func is an array with two elements the first element is the callback
        # function and the second the data
        # This allows to assign a callback function to a widget with one line ...
        if type(args) == tuple:
            if len(args) == 2:
                callback = args[0]
                data = args[1]
        else:
            callback = args
            
        if not callable(callback):
            raise TypeError("callback is not callable")
        
        mapping = _callback_mappings.get(<long>self.obj, None)
        if mapping is None:
            mapping = dict()
            mapping["__class__"] =  self
            mapping[event] = callback
            mapping["__data__"] = data
            _callback_mappings[<long>self.obj] = mapping
        else:
            mapping[event] = callback
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
            
    def _get_obj_addr(self):
        """
        Return the adress of the internal save Evas_Object
        
        @return: Address of saved Evas_Object
        """
        return <long>self.obj

