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

cdef c_evas.Evas_Object *_tooltip_content_create(void *data, c_evas.Evas_Object *o, c_evas.Evas_Object *t) with gil:
    cdef Object ret, obj, tooltip

    obj = <Object>c_evas.evas_object_data_get(o, "python-evas")
    tooltip = evas.c_evas._Object_from_instance(<long> t)
    (func, args, kargs) = <object>data
    ret = func(obj, tooltip *args, **kargs)
    if not ret:
        return NULL
    return ret.obj

cdef void _tooltip_data_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
    Py_DECREF(<object>data)

cdef evas.c_evas.Eina_Bool _event_dispatcher(o, src, c_evas.Evas_Callback_Type t, event_info):
    cdef Object obj = o
    cdef object ret
    for func, args, kargs in obj._elm_event_cbs:
        try:
            ret = func(obj, src, t, event_info, *args, **kargs)
        except Exception, e:
            traceback.print_exc()
        else:
            if ret:
                return True
    return False

cdef evas.c_evas.Eina_Bool _event_callback(void *data, c_evas.Evas_Object *o, c_evas.Evas_Object *src, c_evas.Evas_Callback_Type t, void *event_info) with gil:
    cdef Object obj = <Object>evas.c_evas._Object_from_instance(<long>o)
    cdef Object src_obj = <Object>evas.c_evas._Object_from_instance(<long>src)
    cdef evas.c_evas.Eina_Bool ret = False
    cdef evas.c_evas.EventKeyDown down_event
    cdef evas.c_evas.EventKeyUp up_event
    if t == evas.c_evas.EVAS_CALLBACK_KEY_DOWN:
        down_event = evas.c_evas.EventKeyDown()
        down_event._set_obj(event_info)
        ret = _event_dispatcher(obj, src_obj, t, down_event)
        down_event._unset_obj()
    elif t == evas.c_evas.EVAS_CALLBACK_KEY_UP:
        up_event = evas.c_evas.EventKeyUp()
        up_event._set_obj(event_info)
        ret = _event_dispatcher(obj, src_obj, t, up_event)
        up_event._unset_obj()


cdef void _event_data_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
    Py_DECREF(<object>data)

cdef class Canvas(evas.c_evas.Canvas):
    def __init__(self):
        pass

cdef class Object(evas.c_evas.Object):
    cdef object _elmcallbacks
    cdef object _elm_event_cbs
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
        cdef const_char_ptr style
        style = elm_object_style_get(self.obj)
        return style

    def disabled_set(self, disabled):
        elm_object_disabled_set(self.obj, disabled)

    def disabled_get(self):
        return elm_object_disabled_get(self.obj)

    def focus(self):
        _METHOD_DEPRECATED(self, "focus_set")
        elm_object_focus_set(self.obj, 1)

    def unfocus(self):
        _METHOD_DEPRECATED(self, "focus_set")
        elm_object_focus_set(self.obj, 0)

    def focus_get(self):
        elm_object_focus_get(self.obj)

    def focus_allow_set(self, allow):
        elm_object_focus_allow_set(self.obj, allow)
    
    def focus_allow_get(self):
        return elm_object_focus_allow_get(self.obj)
    
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

    def tooltip_show(self):
        """ Force show tooltip of object

        Force show the tooltip and disable hide on mouse_out
        If another content is set as tooltip, the visible tooltip will hidden
        and showed again with new content.

        This can force show more than one tooltip at a time.
        """
        elm_object_tooltip_show(self.obj)

    def tooltip_hide(self):
        """ Force hide tooltip of the object

        Force hide tooltip of object and (re)enable future mouse interations.
        """
        elm_object_tooltip_hide(self.obj)

    def tooltip_text_set(self, char *text):
        """ Set the text to be shown in the tooltip object

        Setup the text as tooltip object. The object can have only one
        tooltip, so any previous tooltip data is removed.
        Internaly, this method call @tooltip_content_cb_set
        """
        elm_object_tooltip_text_set(self.obj, text)

    def tooltip_content_cb_set(self, func, *args, **kargs):
        """ Set the content to be shown in the tooltip object

        @param: B{func} Function to be create tooltip content, called when
                need show tooltip.

        Setup the tooltip to object. The object can have only one
        tooltip, so any previews tooltip data is removed. func(owner,
        tooltip, args, kargs) will be called every time that need
        show the tooltip and it should return a valid
        Evas_Object. This object is then managed fully by tooltip
        system and is deleted when the tooltip is gone.
        """
        if not callable(func):
            raise TypeError("func must be callable")

        cdef void *cbdata

        data = (func, args, kargs)
        Py_INCREF(data)
        cbdata = <void *>data
        elm_object_tooltip_content_cb_set(self.obj, _tooltip_content_create,
                                          cbdata, _tooltip_data_del_cb)

    def tooltip_unset(self):
        """ Unset tooltip from object

        Remove tooltip from object. If used the @tool_text_set the internal
        copy of label will be removed correctly. If used
        @tooltip_content_cb_set, the data will be unreferred but no freed.
        """
        elm_object_tooltip_unset(self.obj)

    def tooltip_style_set(self, style=None):
        """ Sets a different style for this object tooltip.

        @note before you set a style you should define a tooltip with
        elm_object_tooltip_content_cb_set() or
        elm_object_tooltip_text_set()
        """
        if style:
            elm_object_tooltip_style_set(self.obj, style)
        else:
            elm_object_tooltip_style_set(self.obj, NULL)

    def tooltip_style_get(self):
        """ Get the style for this object tooltip.
        """
        cdef const_char_ptr style
        style = elm_object_tooltip_style_get(self.obj)
        if style == NULL:
            return None
        return style

    def cursor_set(self, char *cursor):
        """ Set the cursor to be shown when mouse is over the object

        Set the cursor that will be displayed when mouse is over the
        object. The object can have only one cursor set to it, so if
        this function is called twice for an object, the previous set
        will be unset.
        """
        elm_object_cursor_set(self.obj, cursor)

    def cursor_unset(self):
        """ Unset cursor for object

        Unset cursor for object, and set the cursor to default if the mouse
        was over this object.
        """
        elm_object_cursor_unset(self.obj)

    def cursor_style_set(self, style=None):
        """ Sets a different style for this object cursor.

        @note before you set a style you should define a cursor with
        elm_object_cursor_set()
        """
        if style:
            elm_object_cursor_style_set(self.obj, style)
        else:
            elm_object_cursor_style_set(self.obj, NULL)

    def cursor_style_get(self):
        """ Get the style for this object cursor.
        """
        cdef const_char_ptr style
        style = elm_object_cursor_style_get(self.obj)
        if style == NULL:
            return None
        return style

    def cursor_engine_only_set(self, engine_only):
        """ Sets cursor engine only usage for this object.

        @note before you set engine only usage you should define a cursor with
        elm_object_cursor_set()
        """
        elm_object_cursor_engine_only_set(self.obj, bool(engine_only))

    def cursor_engine_only_get(self):
        """ Get the engine only usage for this object.
        """
        return elm_object_cursor_engine_only_get(self.obj)

    def text_set(self, text):
        """Sets the main text for this object.

        @parm: B{text} any text to set as the main textual part of this object.
        @see: L{text_get()} and L{text_part_set()}
        """
        elm_object_text_set(self.obj, text)

    def text_get(self):
        """Gets the main text for this object.

        @return: the main text or None if nothing was set.
        @see: L{text_set()} and L{text_part_get()}
        """
        cdef const_char_ptr l
        l = elm_object_text_get(self.obj)
        if l == NULL:
            return None
        return l

    property text:
        def __get__(self):
            return self.text_get()

        def __set__(self, value):
            self.text_set(value)

    def text_part_set(self, part, text):
        """Sets the text of a given part of this object.

        @parm: B{part} part name to set the text.
        @parm: B{text} text to set.
        @see: L{text_set()} and L{text_part_get()}
        """
        elm_object_text_part_set(self.obj, part, text)

    def text_part_get(self, part):
        """Gets the text of a given part of this object.

        @parm: B{part} part name to get the text.
        @return: the text of a part or None if nothing was set.
        @see: L{text_get()} and L{text_part_set()}
        """
        cdef const_char_ptr l
        l = elm_object_text_part_get(self.obj, part)
        if l == NULL:
            return None
        return l

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
        Return the address of the internal save Evas_Object

        @return: Address of saved Evas_Object
        """
        return <long>self.obj

    def focus_custom_chain_set(self, lst):
        elm_object_focus_custom_chain_unset(self.obj)
        cdef Object obj
        for obj in lst:
            elm_object_focus_custom_chain_append(self.obj, obj.obj, NULL)

    def focus_custom_chain_get(self):
        cdef c_evas.Evas_Object *o
        cdef Object obj
        cdef evas.c_evas.const_Eina_List *lst
        ret = []
        lst = elm_object_focus_custom_chain_get(self.obj)
        while lst:
            o = <c_evas.Evas_Object *> lst.data
            obj = <Object>c_evas.evas_object_data_get(o, "python-evas")
            ret.append(obj)
            lst = lst.next
        return ret

    def focus_custom_chain_unset(self, lst):
        elm_object_focus_custom_chain_unset(self.obj)

    def focus_custom_chain_append(self, Object obj, Object relative=None):
        cdef c_evas.Evas_Object *rel = NULL
        if relative:
            rel = relative.obj
        elm_object_focus_custom_chain_append(self.obj, obj.obj, rel)

    def focus_custom_chain_prepend(self, Object obj, Object relative=None):
        cdef c_evas.Evas_Object *rel = NULL
        if relative:
            rel = relative.obj
        elm_object_focus_custom_chain_prepend(self.obj, obj.obj, rel)

    def elm_event_callback_add(self, func, *args, **kargs):
        if not callable(func):
            raise TypeError("func must be callable")

        if self._elm_event_cbs is None:
            self._elm_event_cbs = []

        if not self._elm_event_cbs:
            elm_object_event_callback_add(self.obj, _event_callback, NULL)

        data = (func, args, kargs)
        self._elm_event_cbs.append(data)

    def elm_event_callback_del(self, func, *args, **kargs):
        data = (func, args, kargs)
        self._elm_event_cbs.remove(data)

        if not self._elm_event_cbs:
            elm_object_event_callback_del(self.obj, _event_callback, NULL)


def __elm_widget_cls_resolver(long ptr):
    cdef c_evas.Evas_Object *obj = <c_evas.Evas_Object *>ptr
    cdef const_char_ptr t

    t = elm_object_widget_type_get(obj)
    assert t != NULL
    return _elm_widget_type_mapping.get(t, None)

evas.c_evas._extended_object_mapping_register("elm_widget",
                                              __elm_widget_cls_resolver)
