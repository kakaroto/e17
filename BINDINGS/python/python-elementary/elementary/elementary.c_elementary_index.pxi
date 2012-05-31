# Copyright 2012 Kai Huuhko <kai.huuhko@gmail.com>
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

cdef void _index_callback(void *cbt, c_evas.Evas_Object *o, void *event_info) with gil:
    try:
        (obj, callback, it, a, ka) = <object>cbt
        callback(obj, it, *a, **ka)
    except Exception, e:
        traceback.print_exc()

cdef void _index_item_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
    (obj, callback, it, a, ka) = <object>data
    it.__del_cb()

def _index_item_conv(long addr):
    cdef Elm_Object_Item *it = <Elm_Object_Item *>addr
    cdef void *data = elm_object_item_data_get(it)
    if data == NULL:
        return None
    else:
        cbt = <object>data
        return cbt[2]

cdef enum Elm_Index_Item_Insert_Kind:
    ELM_INDEX_ITEM_INSERT_APPEND
    ELM_INDEX_ITEM_INSERT_PREPEND
    ELM_INDEX_ITEM_INSERT_BEFORE
    ELM_INDEX_ITEM_INSERT_AFTER
    ELM_INDEX_ITEM_INSERT_SORTED

cdef class IndexItem(ObjectItem):
    def __init__(self, kind, c_evas.Object index, const_char_ptr letter, IndexItem before_after = None,
                 callback = None, *args, **kargs):
        cdef void* cbdata
        cdef void (*cb) (void *, c_evas.Evas_Object *, void *)

        cbdata = NULL
        cb = NULL

        if callback is not None:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _index_callback
        self.cbt = (index, callback, self, args, kargs)
        cbdata = <void*>self.cbt

        if kind == ELM_INDEX_ITEM_INSERT_APPEND:
            self.obj = elm_index_item_append(index.obj, letter, cb, cbdata)
        elif kind == ELM_INDEX_ITEM_INSERT_PREPEND:
            self.obj = elm_index_item_prepend(index.obj, letter, cb, cbdata)
        #elif kind == ELM_INDEX_ITEM_INSERT_SORTED:
            #self.obj = elm_index_item_sorted_insert(index.obj, letter, cb, cbdata, cmp_f, cmp_data_f)
        else:
            if before_after == None:
                raise ValueError("need a valid after object to add an item before/after another item")
            if kind == ELM_INDEX_ITEM_INSERT_BEFORE:
                self.obj = elm_index_item_insert_before(index.obj, before_after.obj, letter, cb, cbdata)
            else:
                self.obj = elm_index_item_insert_after(index.obj, before_after.obj, letter, cb, cbdata)

        Py_INCREF(self)
        elm_object_item_del_cb_set(self.obj, _index_item_del_cb)

    def selected_set(self, selected):
        elm_index_item_selected_set(self.obj, selected)

    def letter_get(self):
        return elm_index_item_letter_get(self.obj)

cdef Elm_Object_Item *_elm_index_item_from_python(IndexItem item):
    if item is None:
        return NULL
    else:
        return item.obj

cdef public class Index(Object) [object PyElementaryIndex, type PyElementaryIndex_Type]:
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_index_add(parent.obj))

    def autohide_disabled_set(self, disabled):
        elm_index_autohide_disabled_set(self.obj, disabled)

    def autohide_disabled_get(self):
        return bool(elm_index_autohide_disabled_get(self.obj))

    def item_level_set(self, level):
        elm_index_item_level_set(self.obj, level)

    def item_level_get(self):
        return elm_index_item_level_get(self.obj)

    def selected_item_get(self, level):
        cdef Elm_Object_Item *obj
        cdef void *d
        obj = elm_index_item_find(self.obj, <void*>level)
        if obj == NULL:
            return None
        d = elm_object_item_data_get(obj)
        if d == NULL:
            return None
        else:
            (o, callback, it, a, ka) = <object>d
            return it

    def item_append(self, letter, callback = None, *args, **kargs):
        return IndexItem(ELM_INDEX_ITEM_INSERT_APPEND, self, letter,
                        None, callback, *args, **kargs)

    def item_prepend(self, letter, callback = None, *args, **kargs):
        return IndexItem(ELM_INDEX_ITEM_INSERT_PREPEND, self, letter,
                        None, callback, *args, **kargs)

    def item_insert_before(self, IndexItem before, letter, callback = None, *args, **kargs):
        return IndexItem(ELM_INDEX_ITEM_INSERT_BEFORE, self, letter,
                        before, callback, *args, **kargs)

    def item_insert_after(self, IndexItem after, letter, callback = None, *args, **kargs):
        return IndexItem(ELM_INDEX_ITEM_INSERT_AFTER, self, letter,
                        after, callback, *args, **kargs)

    #def item_sorted_insert(self, letter, callback = None, *args, **kargs):
        #return IndexItem(ELM_INDEX_ITEM_INSERT_SORTED, self, letter,
                        #None, callback, *args, **kargs)

    def item_find(self, data):
        cdef Elm_Object_Item *obj
        cdef void *d
        obj = elm_index_item_find(self.obj, <void*>data)
        if obj == NULL:
            return None
        d = elm_object_item_data_get(obj)
        if d == NULL:
            return None
        else:
            (o, callback, it, a, ka) = <object>d
            return it

    def item_clear(self):
        elm_index_item_clear(self.obj)

    def level_go(self, level):
        elm_index_level_go(self.obj, level)

    def indicator_disabled_set(self, disabled):
        elm_index_indicator_disabled_set(self.obj, disabled)

    def indicator_disabled_get(self):
        return bool(elm_index_indicator_disabled_get(self.obj))

    def horizontal_set(self, horizontal):
        elm_index_horizontal_set(self.obj, horizontal)

    def horizontal_get(self):
        return bool(elm_index_horizontal_get(self.obj))

    def callback_changed_add(self, func, *args, **kwargs):
        self._callback_add_full("changed", _index_item_conv, func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del_full("changed",  _index_item_conv, func)

    def callback_changed_delay_add(self, func, *args, **kwargs):
        self._callback_add_full("changed,delay", _index_item_conv, func, *args, **kwargs)

    def callback_changed_delay_del(self, func):
        self._callback_del_full("changed,delay",  _index_item_conv, func)

    def callback_selected_add(self, func, *args, **kwargs):
        self._callback_add_full("selected", _index_item_conv, func, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del_full("selected",  _index_item_conv, func)

    def callback_level_up_add(self, func, *args, **kwargs):
        self._callback_add("level,up", func, *args, **kwargs)

    def callback_level_up_del(self, func):
        self._callback_del("level,up", func)

    def callback_level_down_add(self, func, *args, **kwargs):
        self._callback_add("level,down", func, *args, **kwargs)

    def callback_level_down_del(self, func):
        self._callback_del("level,down", func)

_elm_widget_type_register("index", Index)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryIndex_Type # hack to install metaclass
_install_metaclass(&PyElementaryIndex_Type, ElementaryObjectMeta)
