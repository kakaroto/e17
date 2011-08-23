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

cdef void _list_callback(void *cbt, c_evas.Evas_Object *o, void *event_info) with gil:
    try:
        (obj, callback, it, a, ka) = <object>cbt
        callback(obj, it, *a, **ka)
    except Exception, e:
        traceback.print_exc()

cdef void _list_item_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
    (obj, callback, it, a, ka) = <object>data
    it.__del_cb()

def _list_item_conv(long addr):
    cdef Elm_List_Item *it = <Elm_List_Item *>addr
    cdef void *data = elm_list_item_data_get(it)
    if data == NULL:
        return None
    else:
        cbt = <object>data
        return cbt[2]

cdef enum Elm_List_Item_Insert_Kind:
    ELM_LIST_ITEM_INSERT_APPEND
    ELM_LIST_ITEM_INSERT_PREPEND
    ELM_LIST_ITEM_INSERT_BEFORE
    ELM_LIST_ITEM_INSERT_AFTER

cdef class ListItem(WidgetItem):
    """
    An item for the list widget
    """
    cdef Elm_List_Item *item
    cdef object cbt

    def __cinit__(self):
        self.item = NULL

    def __del_cb(self):
        self.item = NULL
        self.cbt = None
        Py_DECREF(self)

    def __init__(self, kind, c_evas.Object list, label, c_evas.Object icon = None,
                 c_evas.Object end = None, ListItem before_after = None,
                 callback = None, *args, **kargs):
        cdef c_evas.Evas_Object* icon_obj
        cdef c_evas.Evas_Object* end_obj
        cdef void* cbdata
        cdef void (*cb) (void *, c_evas.Evas_Object *, void *)

        icon_obj = NULL
        end_obj = NULL
        cbdata = NULL
        cb = NULL

        if icon is not None:
            icon_obj = icon.obj
        if end is not None:
            end_obj = end.obj

        if callback is not None:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _list_callback
        self.cbt = (list, callback, self, args, kargs)
        cbdata = <void*>self.cbt

        if kind == ELM_LIST_ITEM_INSERT_APPEND:
            self.item = elm_list_item_append(list.obj, label, icon_obj, end_obj,
                                             cb, cbdata)
        elif kind == ELM_LIST_ITEM_INSERT_PREPEND:
            self.item = elm_list_item_prepend(list.obj, label, icon_obj, end_obj,
                                              cb, cbdata)
        else:
            if before_after == None:
                raise ValueError("need a valid after object to add an item before/after another item")
            if kind == ELM_LIST_ITEM_INSERT_BEFORE:
                self.item = elm_list_item_insert_before(list.obj, before_after.item, label,
                                                        icon_obj, end_obj,
                                                        cb, cbdata)
            else:
                self.item = elm_list_item_insert_after(list.obj, before_after.item, label,
                                                        icon_obj, end_obj,
                                                        cb, cbdata)

        Py_INCREF(self)
        elm_list_item_del_cb_set(self.item, _list_item_del_cb)

    def __str__(self):
        return ("%s(label=%r, icon=%s, end=%s, "
                "callback=%r, args=%r, kargs=%s)") % \
            (self.__class__.__name__, self.label_get(), bool(self.icon_get()),
             bool(self.end_get()), self.cbt[1], self.cbt[3], self.cbt[4])

    def __repr__(self):
        return ("%s(%#x, refcount=%d, Elm_List_Item=%#x, "
                "label=%r, icon=%s, end=%s, "
                "callback=%r, args=%r, kargs=%s)") % \
            (self.__class__.__name__, <unsigned long><void *>self,
             PY_REFCOUNT(self), <unsigned long><void *>self.item,
             self.label_get(), bool(self.icon_get()),
             bool(self.end_get()), self.cbt[1], self.cbt[3], self.cbt[4])

    def delete(self):
        if self.item == NULL:
            raise ValueError("Object already deleted")
        elm_list_item_del(self.item)

    def selected_set(self, selected):
        elm_list_item_selected_set(self.item, selected)

    property selected:
        def __get__(self):
            return elm_list_item_selected_get(self.item)

        def __set__(self, selected):
            elm_list_item_selected_set(self.item, selected)

    def show(self):
        elm_list_item_show(self.item)

    def data_get(self):
        """Returns the callback data given at creation time.

        @rtype: tuple of (args, kargs), args is tuple, kargs is dict.
        """
        cdef void* data
        data = elm_list_item_data_get(self.item)
        if data == NULL:
            return None
        else:
            (obj, callback, it, a, ka) = <object>data

            return (a, ka)

    property data:
        def __get__(self):
            return self.data_get()

    def icon_get(self):
        """Returns the icon object set for this list item at creation time.

        @rtype: evas.c_evas.Object
        """
        cdef c_evas.Evas_Object *icon
        icon = elm_list_item_icon_get(self.item)
        return evas.c_evas._Object_from_instance(<long> icon)

    property icon:
        def __get__(self):
            return self.icon_get()

    def label_get(self):
        """Returns the label string set for this list item.

        @rtype: str
        """
        cdef const_char_ptr l
        l = elm_list_item_label_get(self.item)
        if l == NULL:
            return None
        return l

    def label_set(self, char *label):
        """Set the label string for this list item."""
        elm_list_item_label_set(self.item, label)

    property label:
        def __get__(self):
            return self.label_get()

        def __set__(self, label):
            self.label_set(label)

    property prev:
        def __get__(self):
            return self.prev_get()

    def prev_get(self):
        cdef Elm_List_Item *item
        cdef void *data

        item = elm_list_item_prev(self.item)
        if item == NULL:
            return None

        data = elm_list_item_data_get(item)
        if data == NULL:
            return None

        (obj, callback, it, a, ka) = <object>data
        return it

    property next:
        def __get__(self):
            return self.next_get()

    def next_get(self):
        cdef Elm_List_Item *item
        cdef void *data

        item = elm_list_item_next(self.item)
        if item == NULL:
            return None

        data = elm_list_item_data_get(item)
        if data == NULL:
            return None

        (obj, callback, it, a, ka) = <object>data
        return it

    def end_get(self):
        """Returns the end object set for this list item at creation time.

        End object will be placed at the right side, may contain an
        action or status for this item.

        @rtype: evas.c_evas.Object
        """
        cdef c_evas.Evas_Object *obj
        cdef void *data

        obj = elm_list_item_end_get(self.item)
        if obj == NULL:
            return None
        return evas.c_evas._Object_from_instance(<long>obj)

    property end:
        def __get__(self):
            return self.end_get()

    def base_get(self):
        """Returns the base object set for this list item.

        Base object is the one that visually represents the list item
        row. It must not be changed in a way that breaks the list
        behavior (like deleting the base!), but it might be used to
        feed Edje signals to add more features to row representation.

        @rtype: edje.Edje
        """
        cdef c_evas.Evas_Object *obj
        cdef void *data

        obj = elm_list_item_object_get(self.item)
        if obj == NULL:
            return None
        return evas.c_evas._Object_from_instance(<long>obj)

    property base:
        def __get__(self):
            return self.base_get()

    def tooltip_text_set(self, char *text):
        """ Set the text to be shown in the tooltip object

        Setup the text as tooltip object. The object can have only one
        tooltip, so any previous tooltip data is removed.
        Internaly, this method call @tooltip_content_cb_set
        """
        elm_list_item_tooltip_text_set(self.item, text)

    def tooltip_content_cb_set(self, func, *args, **kargs):
        """ Set the content to be shown in the tooltip object

        @param: B{func} Function to be create tooltip content, called when
                need show tooltip.

        Setup the tooltip to object. The object can have only one tooltip,
        so any previews tooltip data is removed. @func(with @{args,kargs}) will
        be called every time that need show the tooltip and it should return a
        valid Evas_Object. This object is then managed fully by tooltip system
        and is deleted when the tooltip is gone.
        """
        if not callable(func):
            raise TypeError("func must be callable")

        cdef void *cbdata

        data = (func, self, args, kargs)
        Py_INCREF(data)
        cbdata = <void *>data
        elm_list_item_tooltip_content_cb_set(self.item,
                                             _tooltip_item_content_create,
                                             cbdata, _tooltip_item_data_del_cb)

    def item_tooltip_unset(self):
        """ Unset tooltip from object

        Remove tooltip from object. If used the @tool_text_set the internal
        copy of label will be removed correctly. If used
        @tooltip_content_cb_set, the data will be unreferred but no freed.
        """
        elm_list_item_tooltip_unset(self.item)

    def tooltip_style_set(self, style=None):
        """ Sets a different style for this object tooltip.

        @note before you set a style you should define a tooltip with
        elm_list_item_tooltip_content_cb_set() or
        elm_list_item_tooltip_text_set()
        """
        if style:
            elm_list_item_tooltip_style_set(self.item, style)
        else:
            elm_list_item_tooltip_style_set(self.item, NULL)

    def tooltip_style_get(self):
        """ Get the style for this object tooltip.
        """
        cdef const_char_ptr style
        style = elm_list_item_tooltip_style_get(self.item)
        if style == NULL:
            return None
        return style

    def cursor_set(self, char *cursor):
        """ Set the cursor to be shown when mouse is over the list item

        Set the cursor that will be displayed when mouse is over the
        item. The item can have only one cursor set to it, so if
        this function is called twice for an item, the previous set
        will be unset.
        """
        elm_list_item_cursor_set(self.item, cursor)

    def cursor_unset(self):
        """  Unset the cursor to be shown when mouse is over the list item
        """
        elm_list_item_cursor_unset(self.item)

    def cursor_style_set(self, style=None):
        """ Sets a different style for this object cursor.

        @note before you set a style you should define a cursor with
        elm_list_item_cursor_set()
        """
        if style:
            elm_list_item_cursor_style_set(self.item, style)
        else:
            elm_list_item_cursor_style_set(self.item, NULL)

    def cursor_style_get(self):
        """ Get the style for this object cursor.
        """
        cdef const_char_ptr style
        style = elm_list_item_cursor_style_get(self.item)
        if style == NULL:
            return None
        return style

    def cursor_engine_only_set(self, engine_only):
        """ Sets cursor engine only usage for this object.

        @note before you set engine only usage you should define a cursor with
        elm_list_item_cursor_set()
        """
        elm_list_item_cursor_engine_only_set(self.item, bool(engine_only))

    def cursor_engine_only_get(self):
        """ Get the engine only usage for this object.
        """
        return elm_list_item_cursor_engine_only_get(self.item)


cdef class List(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_list_add(parent.obj))

    def item_append(self, label, c_evas.Object icon = None,
                    c_evas.Object end = None, callback = None, *args, **kargs):
        return ListItem(ELM_LIST_ITEM_INSERT_APPEND, self, label, icon, end,
                        None, callback, *args, **kargs)

    def item_prepend(self, label, c_evas.Object icon = None,
                     c_evas.Object end = None, callback = None, *args, **kargs):
        return ListItem(ELM_LIST_ITEM_INSERT_PREPEND, self, label, icon, end,
                        None, callback, *args, **kargs)

    def item_insert_before(self, ListItem before, label, c_evas.Object icon = None,
                           c_evas.Object end = None, callback = None, *args, **kargs):
        return ListItem(ELM_LIST_ITEM_INSERT_BEFORE, self, label, icon, end,
                        before, callback, *args, **kargs)

    def item_insert_after(self, ListItem after, label, c_evas.Object icon = None,
                          c_evas.Object end = None, callback = None, *args, **kargs):
        return ListItem(ELM_LIST_ITEM_INSERT_AFTER, self, label, icon, end,
                        after, callback, *args, **kargs)

    def clear(self):
        elm_list_clear(self.obj)

    def go(self):
        elm_list_go(self.obj)

    def multi_select_set(self, multi):
        elm_list_multi_select_set(self.obj, multi)

    property multi_select:
        def __get__(self):
            return elm_list_multi_select_get(self.obj)

        def __set__(self, multi):
            elm_list_multi_select_set(self.obj, multi)

    def mode_set(self, Elm_List_Mode mode):
        elm_list_mode_set(self.obj, mode)

    property mode:
        def __get__(self):
            return elm_list_mode_get(self.obj)

        def __set__(self, Elm_List_Mode mode):
            elm_list_mode_set(self.obj, mode)

    property horizontal:
        def __get__(self):
            return elm_list_horizontal_get(self.obj)

        def __set__(self, horizontal):
            elm_list_horizontal_set(self.obj, horizontal)

    def always_select_mode_set(self, always_select):
        elm_list_always_select_mode_set(self.obj, always_select)

    def bounce_set(self, h, v):
        elm_list_bounce_set(self.obj, h, v)

    def selected_item_get(self):
        cdef Elm_List_Item *obj
        cdef void *data
        obj = elm_list_selected_item_get(self.obj)
        if obj == NULL:
            return None
        data = elm_list_item_data_get(obj)
        if data == NULL:
            return None
        else:
            (o, callback, it, a, ka) = <object>data
            return it

    def selected_items_get(self):
        cdef evas.c_evas.const_Eina_List *lst, *itr
        cdef void *data
        ret = []
        lst = elm_list_selected_items_get(self.obj)
        itr = lst
        while itr:
            data = elm_list_item_data_get(<Elm_List_Item *>itr.data)
            if data != NULL:
                (o, callback, it, a, ka) = <object>data
                ret.append(it)
            itr = itr.next
        return ret

    def items_get(self):
        cdef evas.c_evas.const_Eina_List *lst, *itr
        cdef void *data
        ret = []
        lst = elm_list_items_get(self.obj)
        itr = lst
        while itr:
            data = elm_list_item_data_get(<Elm_List_Item *>itr.data)
            if data != NULL:
                (o, callback, it, a, ka) = <object>data
                ret.append(it)
            itr = itr.next
        return ret

    def callback_clicked_double_add(self, func, *args, **kwargs):
        self._callback_add_full("clicked,double", _list_item_conv,
                                func, *args, **kwargs)

    def callback_clicked_double_del(self, func):
        self._callback_del_full("clicked,double",  _list_item_conv, func)

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add_full("clicked", _list_item_conv,
                                func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del_full("clicked",  _list_item_conv, func)

    def callback_selected_add(self, func, *args, **kwargs):
        self._callback_add_full("selected", _list_item_conv,
                                func, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del_full("selected", _list_item_conv, func)

    def callback_unselected_add(self, func, *args, **kwargs):
        self._callback_add_full("unselected", _list_item_conv,
                                func, *args, **kwargs)

    def callback_unselected_del(self, func):
        self._callback_del_full("unselected", _list_item_conv, func)

    def callback_longpressed_add(self, func, *args, **kwargs):
        self._callback_add_full("longpressed", _list_item_conv,
                                func, *args, **kwargs)

    def callback_longpressed_del(self, func):
        self._callback_del_full("longpressed", _list_item_conv, func)

    def scroller_policy_set(self, policy_h, policy_v):
        elm_list_scroller_policy_set(self.obj, policy_h, policy_v)


_elm_widget_type_register("list", List)
