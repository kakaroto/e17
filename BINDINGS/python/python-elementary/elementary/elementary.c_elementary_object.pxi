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


cdef void _object_callback(void *cbt, c_evas.Evas_Object *o, void *event_info) with gil:
    try:
        (event, obj, callback, data) = <object>cbt
        if not callable(callback):
            raise TypeError("callback is not callable")
        #Use old parameters order not to break existing programs
        callback(obj, <long>event_info, data)
        # should be: callback(data, obj, <long>event_info)
    except Exception, e:
        traceback.print_exc()

cdef class Canvas(evas.c_evas.Canvas):
    def __init__(self):
        pass

cdef class Object(evas.c_evas.Object):
    cdef object _elmcallbacks
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

    def style_set(self, style):
        elm_object_style_set(self.obj, style)

    def style_get(self):
        cdef char *style
        style = elm_object_style_get(self.obj)
        return style

    def disabled_set(self, disabled):
        elm_object_disabled_set(self.obj, disabled)

    def disabled_get(self):
        return elm_object_disabled_get(self.obj)

    def focus(self):
        elm_object_focus(self.obj)

    def scroll_hold_pop(self):
        """Pop the scroll hold by 1

        This decrements the scroll hold count by one. If it is more
        than 0 it will take effect on the parents of the indicated
        object.
        """
        elm_object_scroll_hold_pop(self.obj)

    def scroll_hold_push(self):
        """Push the scroll hold by 1

        This increments the scroll hold count by one. If it is more
        than 0 it will take effect on the parents of the indicated
        object.
        """
        elm_object_scroll_hold_push(self.obj)

    def scroll_freeze_pop(self):
        """Pop the scroll freeze by 1

        This decrements the scroll freeze count by one. If it is more
        than 0 it will take effect on the parents of the indicated
        object.
        """
        elm_object_scroll_freeze_pop(self.obj)

    def scroll_freeze_push(self):
        """Push the scroll freeze by 1

        This increments the scroll freeze count by one. If it is more
        than 0 it will take effect on the parents of the indicated
        object.
        """
        elm_object_scroll_freeze_push(self.obj)

    def _callback_add(self, event, args):
        """Add a callback for this object

        Add a function as new callback-function for a specified event. The
        function will be called, if the event occured.

        @parm: B{event} Name of the event
        @parm: B{func} Function should be called, when the event occured
        """
        cdef object cbt

        # if func is an array with two elements the first element is the callback
        # function and the second the data
        # This allows to assign a callback function to a widget with one line ...
        if type(args) == tuple:
            if len(args) == 2:
                callback = args[0]
                data = args[1]
        else:
            callback = args
            data = None

        if not callable(callback):
            raise TypeError("callback is not callable")

        cbt = (event, self, callback, data)
        if self._elmcallbacks is None:
            self._elmcallbacks = []
        self._elmcallbacks.append(cbt)
        # register callback
        c_evas.evas_object_smart_callback_add(self.obj, event, _object_callback,
                                              <void *>cbt)

    def _callback_remove(self, event):
        """Removes all callback functions for the event

        Will remove all callback functions for the specified event. 

        @parm: B{event} Name of the event whose events should be removed
        """
        if self._elmcallbacks:
            for i, cbt in enumerate(self._elmcallbacks):
                if cbt is not None:
                    (ev, obj, callback, data) = <object>cbt
                    if event == ev:
                        c_evas.evas_object_smart_callback_del(self.obj, event,
                                                              _object_callback)
                        self._elmcallbacks[i] = None

    def _get_obj_addr(self):
        """
        Return the adress of the internal save Evas_Object

        @return: Address of saved Evas_Object
        """
        return <long>self.obj

