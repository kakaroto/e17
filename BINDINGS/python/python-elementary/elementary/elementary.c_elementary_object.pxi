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


cdef void _object_callback(void *cbtl, c_evas.Evas_Object *o, void *event_info) with gil:
    l = <object>cbtl
    for cbt in l:
        try:
            (obj, callback, args, kwargs) = <object>cbt
            if event_info != NULL:
               callback(obj, <long>event_info, *args, **kwargs)
            else:
               callback(obj, *args, **kwargs)
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

    def _callback_add(self, event, callback, *args, **kwargs):
        """Add a callback for this object

        Add a function as new callback-function for a specified event. The
        function will be called, if the event occured.

        @parm: B{event} Name of the event
        @parm: B{callback} Function should be called, when the event occured
        @parm: B{args} Argument tuple passed to the function when called
        @parm: B{kwargs} Argument dictionnary passed to the function when
               called
        """
        cdef object cbt
        cdef void *data

        if not callable(callback):
            raise TypeError("callback is not callable")

        cbt = (self, callback, args, kwargs)

        if self._elmcallbacks is None:
            self._elmcallbacks = {}
        if self._elmcallbacks.has_key(event):
            self._elmcallbacks[event].append(cbt)
        else:
            self._elmcallbacks[event] = [cbt]
            # register callback
            data = <void*>self._elmcallbacks[event]
            c_evas.evas_object_smart_callback_add(self.obj, event,
                                                  _object_callback, data)

    def _callback_remove(self, event, func=None, *args, **kwargs):
        """Removes all callback functions for the event

        Will remove all callback functions for the specified event.

        @parm: B{event} Name of the event whose events should be removed
        @parm: B{func} If set, will remove only this callback
        """
        if self._elmcallbacks and self._elmcallbacks.has_key(event):
            if func is None:
                c_evas.evas_object_smart_callback_del(self.obj, event,
                                                      _object_callback)
                self._elmcallbacks[event] = None
            else:
                for i, cbt in enumerate(self._elmcallbacks[event]):
                    if (cbt is not None
                        and (self, func, args, kwargs) == <object>cbt):
                        self._elmcallbacks[event][i] = None
                if not self._elmcallbacks[event]:
                    c_evas.evas_object_smart_callback_del(self.obj, event,
                                                          _object_callback)

    def _get_obj_addr(self):
        """
        Return the adress of the internal save Evas_Object

        @return: Address of saved Evas_Object
        """
        return <long>self.obj

