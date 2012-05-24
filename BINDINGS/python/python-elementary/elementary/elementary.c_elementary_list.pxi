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
    cdef Elm_Object_Item *it = <Elm_Object_Item *>addr
    cdef void *data = elm_object_item_data_get(it)
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
    ELM_LIST_ITEM_INSERT_SORTED

cdef class ListItem(ObjectItem):
    """
    An item for the list widget
    """
    cdef Elm_Object_Item *item
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
        #elif kind == ELM_LIST_ITEM_INSERT_SORTED:
            #self.item = elm_list_item_sorted_insert(list.obj, label, icon_obj, end_obj, cb, cbdata, cmp_f)
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
        elm_object_item_del_cb_set(self.item, _list_item_del_cb)

    def __str__(self):
        return ("%s(label=%r, icon=%s, end=%s, "
                "callback=%r, args=%r, kargs=%s)") % \
            (self.__class__.__name__, self.label_get(), bool(self.icon_get()),
             bool(self.end_get()), self.cbt[1], self.cbt[3], self.cbt[4])

    def __repr__(self):
        return ("%s(%#x, refcount=%d, Elm_Object_Item=%#x, "
                "label=%r, icon=%s, end=%s, "
                "callback=%r, args=%r, kargs=%s)") % \
            (self.__class__.__name__, <unsigned long><void *>self,
             PY_REFCOUNT(self), <unsigned long><void *>self.item,
             self.label_get(), bool(self.icon_get()),
             bool(self.end_get()), self.cbt[1], self.cbt[3], self.cbt[4])

    def selected_set(self, selected):
        elm_list_item_selected_set(self.item, selected)

    def selected_get(self):
        return bool(elm_list_item_selected_get(self.item))

    property selected:
        def __get__(self):
            return self.selected_get()
        def __set__(self, selected):
            self.selected_set(selected)

    def separator_set(self, separator):
        elm_list_item_separator_set(self.item, separator)

    def separator_get(self):
        return bool(elm_list_item_separator_get(self.item))

    property separator:
        def __get__(self):
            return self.separator_get()
        def __set__(self, separator):
            self.separator_set(separator)

    def show(self):
        elm_list_item_show(self.item)

    def bring_in(self):
        elm_list_item_bring_in(self.item)

    def object_get(self):
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

    property prev:
        def __get__(self):
            return self.prev_get()

    def prev_get(self):
        cdef Elm_Object_Item *item
        cdef void *data

        item = elm_list_item_prev(self.item)
        if item == NULL:
            return None

        data = elm_object_item_data_get(item)
        if data == NULL:
            return None

        (obj, callback, it, a, ka) = <object>data
        return it

    property next:
        def __get__(self):
            return self.next_get()

    def next_get(self):
        cdef Elm_Object_Item *item
        cdef void *data

        item = elm_list_item_next(self.item)
        if item == NULL:
            return None

        data = elm_object_item_data_get(item)
        if data == NULL:
            return None

        (obj, callback, it, a, ka) = <object>data
        return it

cdef class List(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_list_add(parent.obj))

    def go(self):
        elm_list_go(self.obj)

    def multi_select_set(self, multi):
        elm_list_multi_select_set(self.obj, multi)

    def multi_select_get(self):
        return bool(elm_list_multi_select_get(self.obj))

    property multi_select:
        def __get__(self):
            return elm_list_multi_select_get(self.obj)
        def __set__(self, multi):
            elm_list_multi_select_set(self.obj, multi)

    def mode_set(self, Elm_List_Mode mode):
        elm_list_mode_set(self.obj, mode)

    def mode_get(self):
        return elm_list_mode_get(self.obj)

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

    def select_mode_set(self, mode):
        elm_list_select_mode_set(self.obj, mode)

    def select_mode_get(self):
        return elm_list_select_mode_get(self.obj)

    def bounce_set(self, h, v):
        elm_list_bounce_set(self.obj, h, v)

    def bounce_get(self):
        cdef c_evas.Eina_Bool h, v
        elm_list_bounce_get(self.obj, &h, &v)
        return (h, v)

    def scroller_policy_set(self, policy_h, policy_v):
        elm_list_scroller_policy_set(self.obj, policy_h, policy_v)

    def scroller_policy_get(self):
        cdef Elm_Scroller_Policy policy_h, policy_v
        elm_list_scroller_policy_get(self.obj, &policy_h, &policy_v)
        return (policy_h, policy_v)

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

    #def item_sorted_insert(self, label, c_evas.Object icon = None,
                    #c_evas.Object end = None, callback = None, *args, **kargs):
        #return ListItem(ELM_LIST_ITEM_INSERT_SORTED, self, label, icon, end,
                        #None, callback, *args, **kargs)

    def clear(self):
        elm_list_clear(self.obj)

    def items_get(self):
        cdef evas.c_evas.const_Eina_List *lst, *itr
        cdef void *data
        ret = []
        lst = elm_list_items_get(self.obj)
        itr = lst
        while itr:
            data = elm_object_item_data_get(<Elm_Object_Item *>itr.data)
            if data != NULL:
                (o, callback, it, a, ka) = <object>data
                ret.append(it)
            itr = itr.next
        return ret

    def selected_item_get(self):
        cdef Elm_Object_Item *obj
        cdef void *data
        obj = elm_list_selected_item_get(self.obj)
        if obj == NULL:
            return None
        data = elm_object_item_data_get(obj)
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
            data = elm_object_item_data_get(<Elm_Object_Item *>itr.data)
            if data != NULL:
                (o, callback, it, a, ka) = <object>data
                ret.append(it)
            itr = itr.next
        return ret

    def first_item_get(self):
        cdef Elm_Object_Item *obj
        cdef void *data
        obj = elm_list_first_item_get(self.obj)
        if obj == NULL:
            return None
        data = elm_object_item_data_get(obj)
        if data == NULL:
            return None
        else:
            (o, callback, it, a, ka) = <object>data
            return it

    property first_item:
        def __get__(self):
            return self.first_item_get()

    def last_item_get(self):
        cdef Elm_Object_Item *obj
        cdef void *data
        obj = elm_list_last_item_get(self.obj)
        if obj == NULL:
            return None
        data = elm_object_item_data_get(obj)
        if data == NULL:
            return None
        else:
            (o, callback, it, a, ka) = <object>data
            return it

    property last_item:
        def __get__(self):
            return self.last_item_get()

    def callback_activated_add(self, func, *args, **kwargs):
        self._callback_add_full("activated", _list_item_conv,
                                func, *args, **kwargs)

    def callback_activated_del(self, func):
        self._callback_del_full("activated",  _list_item_conv, func)

    def callback_clicked_double_add(self, func, *args, **kwargs):
        self._callback_add_full("clicked,double", _list_item_conv,
                                func, *args, **kwargs)

    def callback_clicked_double_del(self, func):
        self._callback_del_full("clicked,double",  _list_item_conv, func)

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

    def callback_edge_top_add(self, func, *args, **kwargs):
        self._callback_add("edge,top", func, *args, **kwargs)

    def callback_edge_top_del(self, func):
        self._callback_del("edge,top",  func)

    def callback_edge_bottom_add(self, func, *args, **kwargs):
        self._callback_add("edge,bottom", func, *args, **kwargs)

    def callback_edge_bottom_del(self, func):
        self._callback_del("edge,bottom",  func)

    def callback_edge_left_add(self, func, *args, **kwargs):
        self._callback_add("edge,left", func, *args, **kwargs)

    def callback_edge_left_del(self, func):
        self._callback_del("edge,left",  func)

    def callback_edge_right_add(self, func, *args, **kwargs):
        self._callback_add("edge,right", func, *args, **kwargs)

    def callback_edge_right_del(self, func):
        self._callback_del("edge,right",  func)

    def callback_language_changed_add(self, func, *args, **kwargs):
        self._callback_add("language,changed", func, *args, **kwargs)

    def callback_language_changed_del(self, func):
        self._callback_del("language,changed",  func)

_elm_widget_type_register("list", List)
