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


cdef void _object_callback(void *data,
                           c_evas.Evas_Object *o, void *event_info) with gil:
    cdef Object obj
    cdef object event, ei
    obj = <Object>c_evas.evas_object_data_get(o, "python-evas")
    event = <object>data
    lst = tuple(obj._elmcallbacks[event])
    for event_conv, func, args, kargs in lst:
        try:
            if event_conv is None:
                func(obj, *args, **kargs)
            else:
                ei = event_conv(<long>event_info)
                func(obj, ei, *args, **kargs)
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

    def _callback_add_full(self, char *event, event_conv, func, *args, **kargs):
        """Add a callback for the smart event specified by event.

        @parm: B{event} event name
        @parm: B{event_conv} Conversion function to get the
               pointer (as a long) to the object to be given to the
               function as the second parameter. If None, then no
               parameter will be given to the callback.
        @parm: B{func} what to callback. Should have the signature:
           C{function(object, event_info, *args, **kargs)}
           C{function(object, *args, **kargs)} (if no event_conv is provided)
        @raise TypeError: if B{func} is not callable.
        @raise TypeError: if B{event_conv} is not callable or None.
        """
        if not callable(func):
            raise TypeError("func must be callable")
        if event_conv is not None and not callable(event_conv):
            raise TypeError("event_conv must be None or callable")

        if self._elmcallbacks is None:
            self._elmcallbacks = {}

        e = intern(event)
        lst = self._elmcallbacks.setdefault(e, [])
        if not lst:
            c_evas.evas_object_smart_callback_add(self.obj, event,
                                                  _object_callback, <void *>e)
        lst.append((event_conv, func, args, kargs))

    def _callback_del_full(self, char *event, event_conv, func):
        """Remove a smart callback.

        Removes a callback that was added by L{callback_add()}.

        @parm: B{event} event name
        @parm: B{event_conv} same as registered with _callback_add_full()
        @parm: B{func} what to callback, should have be previously registered.
        @precond: B{event}, B{event_conv} and B{func} must be used as
           parameter for L{_callback_add_full()}.

        @raise ValueError: if there was no B{func} connected with this event.
        """
        try:
            lst = self._elmcallbacks[event]
        except KeyError, e:
            raise ValueError("Unknown event %r" % e)

        i = -1
        for i, (ec, f, a, k) in enumerate(lst):
            if event_conv == ec and func == f:
                break
        else:
            raise ValueError("Callback %s was not registered with event %r" %
                             (func, e))

        lst.pop(i)
        if lst:
            return
        self._elmcallbacks.pop(event)
        c_evas.evas_object_smart_callback_del(self.obj, event, _object_callback)

    def _callback_add(self, char *event, func, *args, **kargs):
        """Add a callback for the smart event specified by event.

        @parm: B{event} event name
        @parm: B{func} what to callback. Should have the signature:
           C{function(object, *args, **kargs)}
        @raise TypeError: if B{func} is not callable.
        """
        return self._callback_add_full(event, None, func, *args, **kargs)

    def _callback_del(self, char *event, func):
        """Remove a smart callback.

        Removes a callback that was added by L{callback_add()}.

        @parm: B{event} event name
        @parm: B{func} what to callback, should have be previously registered.
        @precond: B{event} and B{func} must be used as parameter for
           L{_callback_add_full()}.

        @raise ValueError: if there was no B{func} connected with this event.
        """
        return self._callback_del_full(event, None, func)

    def _callback_remove(self, event, func=None, *args, **kwargs):
        import warnings
        warnings.warn("use _callback_del_full() instead.", DeprecationWarning)
        if func is not None:
            return self._callback_del(event, func)
        else:
            self._elmcallbacks.pop(event)
            c_evas.evas_object_smart_callback_del(self.obj, event,
                                                  _object_callback)

    def _get_obj_addr(self):
        """
        Return the adress of the internal save Evas_Object

        @return: Address of saved Evas_Object
        """
        return <long>self.obj


def __elm_widget_cls_resolver(long ptr):
    cdef c_evas.Evas_Object *obj = <c_evas.Evas_Object *>ptr
    cdef char *t

    t = elm_object_widget_type_get(obj)
    assert t != NULL
    return _elm_widget_type_mapping.get(t, None)

evas.c_evas._extended_object_mapping_register("elm_widget",
                                              __elm_widget_cls_resolver)
