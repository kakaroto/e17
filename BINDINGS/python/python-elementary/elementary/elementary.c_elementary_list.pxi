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

cdef enum Elm_List_Item_Insert_Kind:
    ELM_LIST_ITEM_INSERT_APPEND
    ELM_LIST_ITEM_INSERT_PREPEND
    ELM_LIST_ITEM_INSERT_BEFORE
    ELM_LIST_ITEM_INSERT_AFTER

cdef class ListItem:
    """
    An item for the list widget
    """
    cdef Elm_List_Item *item
    cdef object cbt

    def __new__(self):
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

    def delete(self):
        if self.item == NULL:
            raise ValueError("Object already deleted")
        elm_list_item_del(self.item)

    def selected_set(self, selected):
        elm_list_item_selected_set(self.item, selected)

    def show(self):
        elm_list_item_show(self.item)

    def data_get(self):
        cdef void* data
        data = elm_list_item_data_get(self.item)
        if data == NULL:
            return None
        else:
            (obj, callback, it, a, ka) = <object>data

            return (a, ka)

    def icon_get(self):
        cdef c_evas.Evas_Object *icon
        icon = elm_list_item_icon_get(self.item)
        return evas.c_evas._Object_from_instance(<long> icon)

    def label_get(self):
        return elm_list_item_label_get(self.item)

    def label_set(self, label):
        elm_list_item_label_set(self.item, label)

    def prev(self):
        import warnings
        warnings.warn("use prev_get() instead.", DeprecationWarning)
        self.item = elm_list_item_prev(self.item)

    def prev_get(self):
        cdef Elm_List_Item *item
        cdef void *data

        item = elm_list_item_prev(self.item)

        if item == NULL:
            return None
        data = elm_list_item_data_get(item)
        if data == NULL:
            return None
        (obj, callback, d, it) = <object>data
        return it

    def next(self):
        import warnings
        warnings.warn("use next_get() instead.", DeprecationWarning)
        self.item = elm_list_item_next(self.item)

    def next_get(self):
        cdef Elm_List_Item *item
        cdef void *data

        item = elm_list_item_next(self.item)

        if item == NULL:
            return None
        data = elm_list_item_data_get(item)
        if data == NULL:
            return None
        (obj, callback, d, it) = <object>data
        return it

    def end_get(self):
        cdef c_evas.Evas_Object *obj
        cdef void *data

        obj = elm_list_item_end_get(self.item)
        if obj == NULL:
            return None
        data = c_evas.evas_object_data_get(obj, "python-evas")
        if data == NULL:
            return None
        return <Object>data

    def base_get(self):
        cdef c_evas.Evas_Object *obj
        cdef void *data

        obj = elm_list_item_base_get(self.item)
        if obj == NULL:
            return None
        data = c_evas.evas_object_data_get(obj, "python-evas")
        if data == NULL:
            return None
        return <Object>data


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

    def horizontal_mode_set(self, Elementary_List_Mode mode):
        elm_list_horizontal_mode_set(self.obj, mode)

    def always_select_mode_set(self, always_select):
        elm_list_always_select_mode_set(self.obj, always_select)

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

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_remove(self, func = None, *args, **kwargs):
        self._callback_remove("clicked", func, *args, **kwargs)


    def callback_selected_add(self, func, *args, **kwargs):
        self._callback_add("selected", func, *args, **kwargs)

    def callback_selected_remove(self, func = None, *args, **kwargs):
        self._callback_remove("selected", func, *args, **kwargs)


    def callback_unselected_add(self, func, *args, **kwargs):
        self._callback_add("unselected", func, *args, **kwargs)

    def callback_unselected_remove(self, func = None, *args, **kwargs):
        self._callback_remove("unselected", func, *args, **kwargs)
