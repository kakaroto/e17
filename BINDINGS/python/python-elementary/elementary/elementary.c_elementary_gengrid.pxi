# Copyright (c) 2010 ProFUSION embedded systems
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

import traceback

cdef _py_elm_gengrid_item_call(func, Evas_Object *obj, part, data) with gil:
    try:
        o = Object_from_instance(obj)
        return func(o, _ctouni(part), data)
    except Exception as e:
        traceback.print_exc()
        return None

cdef char *_py_elm_gengrid_item_text_get(void *data, Evas_Object *obj, const_char_ptr part) with gil:
    cdef GengridItem item = <object>data
    cdef object params = item.params
    cdef GengridItemClass itc = params[0]

    func = itc._text_get_func
    if func is None:
        return NULL

    ret = _py_elm_gengrid_item_call(func, obj, part, params[1])
    if ret is not None:
        return strdup(_fruni(ret))
    else:
        return NULL

cdef Evas_Object *_py_elm_gengrid_item_content_get(void *data, Evas_Object *obj, const_char_ptr part) with gil:
    cdef GengridItem item = <object>data
    cdef object params = item.params
    cdef evasObject icon
    cdef GengridItemClass itc = params[0]

    func = itc._content_get_func
    if func is None:
        return NULL

    ret = _py_elm_gengrid_item_call(func, obj, part, params[1])
    if ret is not None:
        try:
            icon = ret
            return icon.obj
        except Exception as e:
            traceback.print_exc()
            return NULL
    else:
        return NULL

cdef Eina_Bool _py_elm_gengrid_item_state_get(void *data, Evas_Object *obj, const_char_ptr part) with gil:
    cdef GengridItem item = <object>data
    cdef object params = item.params
    cdef GengridItemClass itc = params[0]

    func = itc._state_get_func
    if func is None:
        return False

    ret = _py_elm_gengrid_item_call(func, obj, part, params[1])
    if ret is not None:
        return bool(ret)
    else:
        return False

cdef void _py_elm_gengrid_object_item_del(void *data, Evas_Object *obj) with gil:
    cdef GengridItem item = <object>data
    cdef object params
    cdef GengridItemClass itc

    if item is None:
        return

    params = item.params
    itc = params[0]

    func = itc._del_func
    if func is not None:
        try:
            o = Object_from_instance(obj)
            func(o, params[1])
        except Exception as e:
            traceback.print_exc()
    item._unset_obj()
    Py_DECREF(item)

cdef void _py_elm_gengrid_item_func(void *data, Evas_Object *obj, void *event_info) with gil:
    cdef GengridItem item = <object>data
    cdef object func = item.params[2]

    if func is not None:
        try:
            o = Object_from_instance(obj)
            func(item, o, item.params[1])
        except Exception as e:
            traceback.print_exc()

cdef class GengridItemClass:
    """Defines the behavior of each grid item.

    This class should be created and handled to the Gengrid itself.

    It may be subclassed, in this case the methods L{text_get()},
    L{content_get()}, L{state_get()} and C{delete()} will be used.

    It may also be instantiated directly, given getters to override as
    constructor parameters.

    """
    cdef Elm_Gengrid_Item_Class obj
    cdef readonly object _item_style
    cdef readonly object _text_get_func
    cdef readonly object _content_get_func
    cdef readonly object _state_get_func
    cdef readonly object _del_func

    def __cinit__(self, *a, **ka):
        self._item_style = "default"
        self._text_get_func = None
        self._content_get_func = None
        self._state_get_func = None
        self._del_func = None

        self.obj.item_style = NULL
        self.obj.func.text_get = _py_elm_gengrid_item_text_get
        self.obj.func.content_get = _py_elm_gengrid_item_content_get
        self.obj.func.state_get = _py_elm_gengrid_item_state_get
        self.obj.func.del_ = _py_elm_gengrid_object_item_del

    def __init__(self, item_style=None, text_get_func=None,
                 content_get_func=None, state_get_func=None, del_func=None):
        """GengridItemClass constructor.

        @param item_style: the string that defines the gengrid item
            theme to be used. The corresponding edje group will
            have this as suffix.

        @param text_get_func: if provided will override the
            behavior defined by L{text_get()} in this class. Its
            purpose is to return the label string to be used by a
            given part and row. This function should have the
            signature:
            C{func(obj, part, item_data) -> str}

        @param content_get_func: if provided will override the behavior
            defined by L{content_get()} in this class. Its purpose is
            to return the icon object to be used (swalloed) by a
            given part and row. This function should have the
            signature:
            C{func(obj, part, item_data) -> obj}

        @param state_get_func: if provided will override the
            behavior defined by L{state_get()} in this class. Its
            purpose is to return the boolean state to be used by a
            given part and row. This function should have the
            signature:
            C{func(obj, part, item_data) -> bool}

        @param del_func: if provided will override the behavior
            defined by C{delete()} in this class. Its purpose is to be
            called when item is deleted, thus finalizing resources
            and similar. This function should have the signature:
            C{func(obj, item_data)}

        @note: In all these signatures, 'obj' means Gengrid and
            'item_data' is the value given to Gengrid item append/prepend
            methods, it should represent your item model as you want.
        """
        if item_style:
            self._item_style = item_style

        if text_get_func and not callable(text_get_func):
            raise TypeError("text_get_func is not callable!")
        elif text_get_func:
            self._text_get_func = text_get_func
        else:
            self._text_get_func = self.text_get

        if content_get_func and not callable(content_get_func):
            raise TypeError("content_get_func is not callable!")
        elif content_get_func:
            self._content_get_func = content_get_func
        else:
            self._content_get_func = self.content_get

        if state_get_func and not callable(state_get_func):
            raise TypeError("state_get_func is not callable!")
        elif state_get_func:
            self._state_get_func = state_get_func
        else:
            self._state_get_func = self.state_get

        if del_func and not callable(del_func):
            raise TypeError("del_func is not callable!")
        elif del_func:
            self._del_func = del_func
        else:
            try:
                self._del_func = self.delete
            except AttributeError:
                pass

        self.obj.item_style = _cfruni(self._item_style)

    def __str__(self):
        return ("%s(item_style=%r, text_get_func=%s, content_get_func=%s, "
                "state_get_func=%s, del_func=%s)") % \
               (self.__class__.__name__,
                self._item_style,
                self._text_get_func,
                self._content_get_func,
                self._state_get_func,
                self._del_func)

    def __repr__(self):
        return ("%s(%#x, refcount=%d, Elm_Gengrid_Item_Class=%#x, "
                "item_style=%r, text_get_func=%s, content_get_func=%s, "
                "state_get_func=%s, del_func=%s)") % \
               (self.__class__.__name__,
                <unsigned long><void *>self,
                PY_REFCOUNT(self),
                <unsigned long>&self.obj,
                self._item_style,
                self._text_get_func,
                self._content_get_func,
                self._state_get_func,
                self._del_func)

    property item_style:
        def __get__(self):
            return self._item_style

    def text_get(self, evasObject obj, part, item_data):
        """To be called by Gengrid for each item to get its label.

        @param obj: the Gengrid instance
        @param part: the part that is being handled.
        @param item_data: the value given to gengrid append/prepend.

        @return: label to be used.
        @rtype: str or None
        """
        return None

    def content_get(self, evasObject obj, part, item_data):
        """To be called by Gengrid for each item to get its icon.

        @param obj: the Gengrid instance
        @param part: the part that is being handled.
        @param item_data: the value given to gengrid append/prepend.

        @return: icon object to be used and swallowed.
        @rtype: evas Object or None
        """
        return None

    def state_get(self, evasObject obj, part, item_data):
        """To be called by Gengrid for each item to get its state.

        @param obj: the Gengrid instance
        @param part: the part that is being handled.
        @param item_data: the value given to gengrid append/prepend.

        @return: boolean state to be used.
        @rtype: bool or None
        """
        return False


cdef class GengridItem(ObjectItem):

    """An item for the L{Gengrid} widget."""

    cdef int _set_obj(self, Elm_Object_Item *item) except 0:
        assert self.item == NULL, "Object must be clean"
        self.item = item
        Py_INCREF(self)
        return 1

    cdef void _unset_obj(self):
        assert self.item != NULL, "Object must wrap something"
        self.item = NULL

    def __str__(self):
        return "%s(item_class=%s, func=%s, item_data=%s)" % \
               (self.__class__.__name__,
                self.params[0].__class__.__name__,
                self.params[2],
                self.params[1])

    def __repr__(self):
        return ("%s(%#x, refcount=%d, Elm_Object_Item=%#x, "
                "item_class=%s, func=%s, item_data=%r)") % \
               (self.__class__.__name__,
                <unsigned long><void*>self,
                PY_REFCOUNT(self),
                <unsigned long>self.item,
                self.params[0].__class__.__name__,
                self.params[2],
                self.params[1])

    def data_get(self):
        (item_class, item_data, func) = self.params
        return item_data

    property data:
        def __get__(self):
            return self.data_get()

    def next_get(self):
        return _object_item_to_python(elm_gengrid_item_next_get(self.item))

    property next:
        def __get__(self):
            return self.next_get()

    def prev_get(self):
        return _object_item_to_python(elm_gengrid_item_prev_get(self.item))

    property prev:
        def __get__(self):
            return self.prev_get()

    def gengrid_get(self):
        return Object_from_instance(elm_object_item_widget_get(self.item))

    property gengrid:
        def __get__(self):
            return self.gengrid_get()

    def index_get(self):
        return elm_gengrid_item_index_get(self.item)

    property index:
        def __get__(self):
            return self.index_get()

    def delete(self):
        elm_object_item_del(self.item)

    def update(self):
        elm_gengrid_item_update(self.item)

    def selected_set(self, selected):
        elm_gengrid_item_selected_set(self.item, bool(selected))

    def selected_get(self):
        return bool(elm_gengrid_item_selected_get(self.item))

    property selected:
        def __get__(self):
            return self.selected_get()

        def __set__(self, selected):
            self.selected_set(selected)

    def show(self, scrollto_type = ELM_GENLIST_ITEM_SCROLLTO_IN):
        elm_gengrid_item_show(self.item, scrollto_type)

    def bring_in(self, scrollto_type = ELM_GENLIST_ITEM_SCROLLTO_IN):
        elm_gengrid_item_bring_in(self.item, scrollto_type)

    def pos_get(self):
        cdef unsigned int x, y
        elm_gengrid_item_pos_get(self.item, &x, &y)
        return (x, y)

    # XXX TODO elm_gengrid_item_item_class_update

    # XXX TODO elm_gengrid_item_item_class_get

    def tooltip_text_set(self, text):
        """Set the text to be shown in the tooltip object

        Setup the text as tooltip object. The object can have only one
        tooltip, so any previous tooltip data is removed.
        Internaly, this method call L{tooltip_content_cb_set}
        """
        elm_gengrid_item_tooltip_text_set(self.item, _cfruni(text))

    property tooltip_text:
        def __get__(self):
            return self.tooltip_text_get()

    def tooltip_content_cb_set(self, func, *args, **kargs):
        """Set the content to be shown in the tooltip object

        Setup the tooltip to object. The object can have only one tooltip,
        so any previews tooltip data is removed. C{func(args, kargs)} will
        be called every time that need show the tooltip and it should return a
        valid Evas_Object. This object is then managed fully by tooltip system
        and is deleted when the tooltip is gone.

        @param func: Function to be create tooltip content, called when
            need show tooltip.
        """
        if not callable(func):
            raise TypeError("func must be callable")

        cdef void *cbdata

        data = (func, self, args, kargs)
        Py_INCREF(data)
        cbdata = <void *>data
        elm_gengrid_item_tooltip_content_cb_set(self.item,
                                                _tooltip_item_content_create,
                                                cbdata,
                                                _tooltip_item_data_del_cb)

    def item_tooltip_unset(self):
        """ Unset tooltip from object

        Remove tooltip from object. If used the L{tooltip_text_set} the internal
        copy of label will be removed correctly. If used
        L{tooltip_content_cb_set}, the data will be unreferred but no freed.
        """
        elm_gengrid_item_tooltip_unset(self.item)

    def tooltip_style_set(self, style=None):
        """ Sets a different style for this object tooltip.

        @note: before you set a style you should define a tooltip with
            elm_gengrid_item_tooltip_content_cb_set() or
            elm_gengrid_item_tooltip_text_set()
        """
        if style:
            elm_gengrid_item_tooltip_style_set(self.item, _cfruni(style))
        else:
            elm_gengrid_item_tooltip_style_set(self.item, NULL)

    def tooltip_style_get(self):
        """ Get the style for this object tooltip.
        """
        return _ctouni(elm_gengrid_item_tooltip_style_get(self.item))

    property tooltip_style:
        def __get__(self):
            return self.tooltip_style_get()

        def __set__(self, value):
            self.tooltip_style_set(value)

    def tooltip_window_mode_set(self, disable):
        elm_gengrid_item_tooltip_window_mode_set(self.item, bool(disable))

    def tooltip_window_mode_get(self):
        return bool(elm_gengrid_item_tooltip_window_mode_get(self.item))

    property tooltip_window_mode:
        def __get__(self):
            return self.tooltip_window_mode_get()

        def __set__(self, value):
            self.tooltip_window_mode_set(value)

    def cursor_set(self, char *cursor):
        """Set the cursor to be shown when mouse is over the gengrid item

        Set the cursor that will be displayed when mouse is over the
        item. The item can have only one cursor set to it, so if
        this function is called twice for an item, the previous set
        will be unset.
        """
        elm_gengrid_item_cursor_set(self.item, _cfruni(cursor))

    def cursor_get(self):
        return _ctouni(elm_gengrid_item_cursor_get(self.item))

    property cursor:
        def __get__(self):
            return self.cursor_get()

        def __set__(self, value):
            self.cursor_set(value)

    def cursor_unset(self):
        """Unset the cursor to be shown when mouse is over the gengrid item."""
        elm_gengrid_item_cursor_unset(self.item)

    def cursor_style_set(self, style=None):
        """ Sets a different style for this object cursor.

        @note: before you set a style you should define a cursor with
            L{cursor_set()}
        """
        if style:
            elm_gengrid_item_cursor_style_set(self.item, _cfruni(style))
        else:
            elm_gengrid_item_cursor_style_set(self.item, NULL)

    def cursor_style_get(self):
        """Get the style for this object cursor."""
        return _ctouni(elm_gengrid_item_cursor_style_get(self.item))

    property cursor_style:
        def __get__(self):
            return self.cursor_style_get()

        def __set__(self, value):
            self.cursor_style_set(value)

    def cursor_engine_only_set(self, engine_only):
        """Sets cursor engine only usage for this object.

        @note: before you set engine only usage you should define a cursor with
            L{cursor_set()}
        """
        elm_gengrid_item_cursor_engine_only_set(self.item, bool(engine_only))

    def cursor_engine_only_get(self):
        """Get the engine only usage for this object."""
        return elm_gengrid_item_cursor_engine_only_get(self.item)

    property cursor_engine_only:
        def __get__(self):
            return self.cursor_engine_only_get()

        def __set__(self, value):
            self.cursor_engine_only_set(value)

    def select_mode_set(self, mode):
        elm_gengrid_item_select_mode_set(self.item, mode)

    def select_mode_get(self):
        return elm_gengrid_item_select_mode_get(self.item)

    property select_mode:
        def __get__(self):
            return self.select_mode_get()

        def __set__(self, value):
            self.select_mode_set(value)

cdef public class Gengrid(Object) [object PyElementaryGengrid, type PyElementaryGengrid_Type]:
    """Creates a generic, scalable and extensible grid widget.

    Like L{Genlist}, this widget allows more items while keeping
    performance. The items may have different look and feel,
    not being restricted only to icon and label.

    """
    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_gengrid_add(parent.obj))

    def clear(self):
        elm_gengrid_clear(self.obj)

    def multi_select_set(self, multi):
        elm_gengrid_multi_select_set(self.obj, bool(multi))

    def multi_select_get(self):
        return bool(elm_gengrid_multi_select_get(self.obj))

    property multi_select:
        def __get__(self):
            return self.multi_select_get()

        def __set__(self, value):
            self.multi_select_set(value)

    def horizontal_set(self, setting):
        elm_gengrid_horizontal_set(self.obj, bool(setting))

    def horizontal_get(self):
        return bool(elm_gengrid_horizontal_get(self.obj))

    property horizontal:
        def __get__(self):
            return self.horizontal_get()

        def __set__(self, value):
            self.horizontal_set(value)

    def bounce_set(self, h_bounce, v_bounce):
        elm_scroller_bounce_set(self.obj, bool(h_bounce), bool(v_bounce))

    def bounce_get(self):
        cdef Eina_Bool h_bounce, v_bounce
        elm_scroller_bounce_get(self.obj, &h_bounce, &v_bounce)
        return (h_bounce, v_bounce)

    property bounce:
        def __get__(self):
            return self.bouce_get()

        def __set__(self, value):
            self.bouce_set(*value)

    def item_append(self, GengridItemClass item_class not None,
                    item_data, func=None):
        """Append a new item (add as last item) to this gengrid.

        @param item_class: a valid instance that defines the
            behavior of this item. See L{GengridItemClass}.
        @param item_data: some data that defines the model of this
            item. This value will be given to methods of
            C{item_class} such as
            L{GengridItemClass.text_get()}. It will also be
            provided to C{func} as its last parameter.
        @param func: if not None, this must be a callable to be
            called back when the item is selected. The function
            signature is:
            C{func(item, obj, item_data)}
            Where C{item} is the handle, C{obj} is the Evas object
            that represents this item, and C{item_data} is the
            value given as parameter to this function.
        """
        cdef GengridItem ret = GengridItem()
        cdef Elm_Object_Item *item
        cdef Evas_Smart_Cb cb

        if func is None:
            cb = NULL
        elif callable(func):
            cb = _py_elm_gengrid_item_func
        else:
            raise TypeError("func is not None or callable")

        ret.params = (item_class, item_data, func)
        item = elm_gengrid_item_append( self.obj,
                                        &item_class.obj,
                                        <void*>ret,
                                        cb,
                                        <void*>ret)

        if item != NULL:
            ret._set_obj(item)
            return ret
        else:
            Py_DECREF(ret)
            return None

    def item_prepend(self, GengridItemClass item_class not None,
                     item_data, func=None):
        """Prepend a new item (add as first item) to this gengrid.

        @param item_class: a valid instance that defines the
            behavior of this item. See L{GengridItemClass}.
        @param item_data: some data that defines the model of this
            item. This value will be given to methods of
            C{item_class} such as
            L{GengridItemClass.text_get()}. It will also be
            provided to C{func} as its last parameter.
        @param func: if not None, this must be a callable to be
            called back when the item is selected. The function
            signature is:
            C{func(item, obj, item_data)}
            Where C{item} is the handle, C{obj} is the Evas object
            that represents this item, and C{item_data} is the
            value given as parameter to this function.
        """
        cdef GengridItem ret = GengridItem()
        cdef Elm_Object_Item *item
        cdef Evas_Smart_Cb cb

        if func is None:
            cb = NULL
        elif callable(func):
            cb = _py_elm_gengrid_item_func
        else:
            raise TypeError("func is not None or callable")

        ret.params = (item_class, item_data, func)
        item = elm_gengrid_item_prepend(self.obj,
                                        &item_class.obj,
                                        <void*>ret,
                                        cb,
                                        <void*>ret)
        if item != NULL:
            ret._set_obj(item)
            return ret
        else:
            Py_DECREF(ret)
            return None

    def item_insert_before(self, GengridItemClass item_class not None,
                           item_data, GengridItem before_item=None,
                           func=None):
        """Insert a new item before another item in this gengrid.

        @param item_class: a valid instance that defines the
            behavior of this item. See L{GengridItemClass}.
        @param item_data: some data that defines the model of this
            item. This value will be given to methods of
            C{item_class} such as
            L{GengridItemClass.text_get()}. It will also be
            provided to C{func} as its last parameter.
        @param before_item: a reference item to use, the new item
            will be inserted before it.
        @param func: if not None, this must be a callable to be
            called back when the item is selected. The function
            signature is:
            C{func(item, obj, item_data)}
            Where C{item} is the handle, C{obj} is the Evas object
            that represents this item, and C{item_data} is the
            value given as parameter to this function.
        """
        cdef GengridItem ret = GengridItem()
        cdef Elm_Object_Item *item, *before
        cdef Evas_Smart_Cb cb

        before = _object_item_from_python(before_item)

        if func is None:
            cb = NULL
        elif callable(func):
            cb = _py_elm_gengrid_item_func
        else:
            raise TypeError("func is not None or callable")

        (item_class, item_data, func)
        item = elm_gengrid_item_insert_before(  self.obj,
                                                &item_class.obj,
                                                <void*>ret,
                                                before,
                                                cb,
                                                <void*>ret)
        if item != NULL:
            ret._set_obj(item)
            return ret
        else:
            Py_DECREF(ret)
            return None

    def item_insert_after(self, GengridItemClass item_class not None,
                          item_data, GengridItem after_item=None,
                          func=None):
        """Insert a new item after another item in this gengrid.

        @param item_class: a valid instance that defines the
            behavior of this item. See L{GengridItemClass}.
        @param item_data: some data that defines the model of this
            item. This value will be given to methods of
            C{item_class} such as
            L{GengridItemClass.text_get()}. It will also be
            provided to C{func} as its last parameter.
        @param after_item: a reference item to use, the new item
            will be inserted after it.
        @param func: if not None, this must be a callable to be
            called back when the item is selected. The function
            signature is:
            C{func(item, obj, item_data)}
            Where C{item} is the handle, C{obj} is the Evas object
            that represents this item, and C{item_data} is the
            value given as parameter to this function.
        """
        cdef GengridItem ret = GengridItem()
        cdef Elm_Object_Item *item, *after
        cdef Evas_Smart_Cb cb

        after = _object_item_from_python(after_item)

        if func is None:
            cb = NULL
        elif callable(func):
            cb = _py_elm_gengrid_item_func
        else:
            raise TypeError("func is not None or callable")

        ret.params = (item_class, item_data, func)
        item = elm_gengrid_item_insert_after(   self.obj,
                                                &item_class.obj,
                                                <void*>ret,
                                                after,
                                                cb,
                                                <void*>ret)
        if item != NULL:
            ret._set_obj(item)
            return ret
        else:
            Py_DECREF(ret)
            return None

    # XXX TODO elm_gengrid_item_sorted_insert()

    def selected_item_get(self):
        return _object_item_to_python(elm_gengrid_selected_item_get(self.obj))

    property selected_item:
        def __get__(self):
            return self.selected_item_get()

    def selected_items_get(self):
        return _object_item_list_to_python(elm_gengrid_selected_items_get(self.obj))

    property selected_items:
        def __get__(self):
            return self.selected_items_get()

    def realized_items_get(self):
        return _object_item_list_to_python(elm_gengrid_realized_items_get(self.obj))

    property realized_items:
        def __get__(self):
            return self.realized_items_get()

    def realized_items_update(self):
        elm_gengrid_realized_items_update(self.obj)

    def first_item_get(self):
        return _object_item_to_python(elm_gengrid_first_item_get(self.obj))

    property first_item:
        def __get__(self):
            return self.first_item_get()

    def last_item_get(self):
        return _object_item_to_python(elm_gengrid_last_item_get(self.obj))

    property last_item:
        def __get__(self):
            return self.last_item_get()

    def scroller_policy_set(self, policy_h, policy_v):
        elm_scroller_policy_set(self.obj, policy_h, policy_v)

    def scroller_policy_get(self):
        cdef Elm_Scroller_Policy policy_h, policy_v
        elm_scroller_policy_get(self.obj, &policy_h, &policy_v)
        return (policy_h, policy_v)

    property scroller_policy:
        def __get__(self):
            return self.scroller_policy_get()

        def __set__(self, value):
            self.scroller_policy_set(*value)

    def item_size_set(self, w, h):
        elm_gengrid_item_size_set(self.obj, w, h)

    def item_size_get(self):
        cdef Evas_Coord x, y
        elm_gengrid_item_size_get(self.obj, &x, &y)
        return (x, y)

    def items_count(self):
        return elm_gengrid_items_count(self.obj)

    property item_size:
        def __get__(self):
            return self.item_size_get()

        def __set__(self, value):
            self.item_size_set(value)

    def group_item_size_set(self, w, h):
        elm_gengrid_group_item_size_set(self.obj, w, h)

    def group_item_size_get(self):
        cdef Evas_Coord w, h
        elm_gengrid_group_item_size_get(self.obj, &w, &h)
        return (w, h)

    property group_item_size:
        def __get__(self):
            return self.group_item_size_get()

        def __set__(self, value):
            self.group_item_size_set(*value)

    def align_set(self, align_x, align_y):
        elm_gengrid_align_set(self.obj, align_x, align_y)

    def align_get(self):
        cdef double align_x, align_y
        elm_gengrid_align_get(self.obj, &align_x, &align_y)
        return (align_x, align_y)

    property align:
        def __get__(self):
            return self.align_get()

        def __set__(self, value):
            self.align_set(*value)

    def reorder_mode_set(self, mode):
        elm_gengrid_reorder_mode_set(self.obj, bool(mode))

    def reorder_mode_get(self, mode):
        return bool(elm_gengrid_reorder_mode_get(self.obj))

    property reorder_mode:
        def __get__(self):
            return self.reorder_mode_get()

        def __set__(self, value):
            self.reorder_mode_set(value)

    def page_relative_set(self, h_pagerel, v_pagerel):
        elm_scroller_page_relative_set(self.obj, h_pagerel, v_pagerel)

    def page_relative_get(self):
        cdef double h_pagerel, v_pagerel
        elm_scroller_page_relative_get(self.obj, &h_pagerel, &v_pagerel)
        return (h_pagerel, v_pagerel)

    property page_relative:
        def __get__(self):
            return self.page_relative_get()

        def __set__(self, value):
            self.page_relative_set(*value)

    def page_size_set(self, h_pagesize, v_pagesize):
        elm_scroller_page_size_set(self.obj, h_pagesize, v_pagesize)

    def current_page_get(self):
        cdef int h_pagenum, v_pagenum
        elm_scroller_current_page_get(self.obj, &h_pagenum, &v_pagenum)
        return (h_pagenum, v_pagenum)

    property current_page:
        def __get__(self):
            return self.current_page_get()

    def last_page_get(self):
        cdef int h_pagenum, v_pagenum
        elm_scroller_last_page_get(self.obj, &h_pagenum, &v_pagenum)
        return (h_pagenum, v_pagenum)

    property last_page:
        def __get__(self):
            return self.last_page_get()

    def page_show(self, h_pagenum, v_pagenum):
        elm_gengrid_page_show(self.obj, h_pagenum, v_pagenum)

    def page_bring_in(self, h_pagenum, v_pagenum):
        elm_scroller_page_bring_in(self.obj, h_pagenum, v_pagenum)

    def filled_set(self, fill):
        elm_gengrid_filled_set(self.obj, bool(fill))

    def filled_get(self, fill):
        return bool(elm_gengrid_filled_get(self.obj))

    property filled:
        def __get__(self):
            return self.filled_get()

        def __set__(self, value):
            self.filled_set(value)

    def select_mode_set(self, mode):
        elm_gengrid_select_mode_set(self.obj, mode)

    def select_mode_get(self):
        return elm_gengrid_select_mode_get(self.obj)

    property select_mode:
        def __get__(self):
            return self.select_mode_get()

        def __set__(self, value):
            self.select_mode_set(value)

    def highlight_mode_set(self, highlight):
        elm_gengrid_highlight_mode_set(self.obj, bool(highlight))

    def highlight_mode_get(self, fill):
        return bool(elm_gengrid_highlight_mode_get(self.obj))

    property highlight_mode:
        def __get__(self):
            return self.highlight_mode_get()

        def __set__(self, value):
            self.highlight_mode_set(value)

    def callback_clicked_double_add(self, func, *args, **kwargs):
        self._callback_add_full("clicked,double", _cb_object_item_conv,
                                func, *args, **kwargs)

    def callback_clicked_double_del(self, func):
        self._callback_del_full("clicked,double", _cb_object_item_conv, func)

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add_full("clicked", _cb_object_item_conv,
                                func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del_full("clicked", _cb_object_item_conv, func)

    def callback_selected_add(self, func, *args, **kwargs):
        self._callback_add_full("selected", _cb_object_item_conv,
                                func, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del_full("selected",  _cb_object_item_conv, func)

    def callback_unselected_add(self, func, *args, **kwargs):
        self._callback_add_full("unselected", _cb_object_item_conv,
                                func, *args, **kwargs)

    def callback_unselected_del(self, func):
        self._callback_del_full("unselected", _cb_object_item_conv, func)

_elm_widget_type_register("gengrid", Gengrid)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryGengrid_Type # hack to install metaclass
_install_metaclass(&PyElementaryGengrid_Type, ElementaryObjectMeta)
