# Copyright (c)  2012 Davide Andreoli <dave@gurumeditation.it>
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

cdef Eina_Bool _multibuttonentry_filter_callback(Evas_Object *obj, const_char_ptr item_label, void *item_data, void *data) with gil:
    try:
        (mbe, callback, a, ka) = <object>data
        ret = callback(mbe, item_label, *a, **ka)
        if isinstance(ret, str):
            # TODO this is BROKEN! the doc say should work, but it's a const char*  :/
            # free(<void*>item_label)
            # item_label = strdup(ret)
            return 1
        elif ret:
            return 1
        else:
            return 0
    except Exception, e:
        traceback.print_exc()

cdef void _multibuttonentry_callback(void *cbt, Evas_Object *o, void *event_info) with gil:
    print "calling cb  :/ " # TODO this is never called :/
    try:
        (obj, callback, it, a, ka) = <object>cbt
        callback(obj, it, *a, **ka)
    except Exception, e:
        traceback.print_exc()

cdef void _multibuttonentry_item_del_cb(void *data, Evas_Object *o, void *event_info) with gil:
    (obj, callback, it, a, ka) = <object>data
    it.__del_cb()

cdef enum Elm_MultiButtonEntry_Item_Insert_Kind:
    ELM_MULTIBUTTONENTRY_INSERT_APPEND
    ELM_MULTIBUTTONENTRY_INSERT_PREPEND
    ELM_MULTIBUTTONENTRY_INSERT_BEFORE
    ELM_MULTIBUTTONENTRY_INSERT_AFTER

cdef class MultiButtonEntryItem(ObjectItem):

    """An item for the MultiButtonEntry widget."""

    def __init__(self, kind, evasObject mbe, label,
                       MultiButtonEntryItem before_after = None,
                       callback = None, *args, **kargs):
        cdef void* cbdata = NULL
        cdef void (*cb) (void *, Evas_Object *, void *)
        cb = NULL

        if callback:
            print "ok cb"
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _multibuttonentry_callback

        self.cbt = (mbe, callback, self, args, kargs)
        cbdata = <void*>self.cbt

        if kind == ELM_MULTIBUTTONENTRY_INSERT_PREPEND:
            self.item = elm_multibuttonentry_item_prepend(mbe.obj,
                                                    _cfruni(label), cb, cbdata)
        elif kind == ELM_MULTIBUTTONENTRY_INSERT_APPEND:
            self.item = elm_multibuttonentry_item_append(mbe.obj,
                                                    _cfruni(label), cb, cbdata)
        elif kind == ELM_MULTIBUTTONENTRY_INSERT_BEFORE:
            self.item = elm_multibuttonentry_item_insert_before(mbe.obj,
                                 before_after.item, _cfruni(label), cb, cbdata)
        elif kind == ELM_MULTIBUTTONENTRY_INSERT_AFTER:
            self.item = elm_multibuttonentry_item_insert_after(mbe.obj,
                                 before_after.item, _cfruni(label), cb, cbdata)

        if self.item != NULL:
            Py_INCREF(self)
            elm_object_item_del_cb_set(self.item, _multibuttonentry_item_del_cb)

    def delete(self):
        """Delete the hoversel item"""
        if self.item == NULL:
            raise ValueError("Object already deleted")
        elm_object_item_del(self.item)

    def selected_set(self, selected):
        elm_multibuttonentry_item_selected_set(self.item, bool(selected))

    def selected_get(self):
        return bool(elm_multibuttonentry_item_selected_get(self.item))

    property selected:
        def __get__(self):
            return self.selected_get()
        def __set__(self, value):
            self.selected_set(value)

    def prev_get(self):
        cdef Elm_Object_Item *obj
        cdef void *data
        obj = elm_multibuttonentry_item_prev_get(self.item)
        if obj == NULL:
            return None
        data = elm_object_item_data_get(obj)
        if data == NULL:
            return None
        (o, callback, it, a, ka) = <object>data
        return it

    property prev:
        def __get__(self):
            return self.prev_get()
    
    def next_get(self):
        cdef Elm_Object_Item *obj
        cdef void *data
        obj = elm_multibuttonentry_item_next_get(self.item)
        if obj == NULL:
            return None
        data = elm_object_item_data_get(obj)
        if data == NULL:
            return None
        (o, callback, it, a, ka) = <object>data
        return it

    property next:
        def __get__(self):
            return self.next_get()
    

cdef public class MultiButtonEntry(Object) [object PyElementaryMultiButtonEntry, type PyElementaryMultiButtonEntry_Type]:

    """A Multibuttonentry is a widget to allow a user enter text and manage it as a number
    of buttons. Each text button is inserted by pressing the "return" key.
    If there is no space in the current row, a new button is added to the next row.
    When a text button is pressed, it will become focused.
    Backspace removes the focus.
    When the Multibuttonentry loses focus items longer than one line are shrunk
    to one line.

    Typical use case of multibuttonentry is, composing emails/messages to a group
    of addresses, each of which is an item that can be clicked for further actions.

    Default text parts of the multibuttonentry widget that you can use for are:
    @li "default" - A label of the multibuttonentry
 
    Default text parts of the multibuttonentry items that you can use for are:
    @li "default" - A label of the multibuttonentry item
 
    """
    
    def __init__(self, evasObject parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_multibuttonentry_add(parent.obj))

    def entry_get(self):
        """Get the entry of the multibuttonentry object

        @return: The child entry
        @rtype: Entry

        """
        cdef Evas_Object *obj = elm_multibuttonentry_entry_get(self.obj)
        return Object_from_instance(obj)

    property entry:
        """The Entry object child of the multibuttonentry.

        @type: Entry

        """
        def __get__(self):
            return self.entry_get()

    def expanded_set(self, enabled):
        """Set/Unset the multibuttonentry to expanded state.

        In expanded state, the complete entry will be displayed.
        Otherwise, only single line of the entry will be displayed.

        @param enabled: A boolean to set if the entry should be expanded
        @type enabled: bool

        """
        elm_multibuttonentry_expanded_set(self.obj, bool(enabled))

    def expanded_get(self):
        """Get the value of expanded state.
        In expanded state, the complete entry will be displayed.
        Otherwise, only single line of the entry will be displayed.

        @return: A boolean to indicating if the entry is expanded
        @rtype: bool

        """
        return bool(elm_multibuttonentry_expanded_get(self.obj))

    property expanded:
        """The expanded state of the multibuttonentry.

        @type: bool

        """
        def __get__(self):
            return self.expanded_get()
        def __set__(self, value):
            self.expanded_set(value)

    def item_prepend(self, label, func = None, *args, **kwargs):
        return MultiButtonEntryItem(ELM_MULTIBUTTONENTRY_INSERT_PREPEND,
                                    self, label, None, func, *args, **kwargs)

    def item_append(self, label, func = None, *args, **kwargs):
        return MultiButtonEntryItem(ELM_MULTIBUTTONENTRY_INSERT_APPEND,
                                    self, label, None, func, *args, **kwargs)

    def item_insert_before(self, MultiButtonEntryItem before, label, func = None, *args, **kwargs):
        return MultiButtonEntryItem(ELM_MULTIBUTTONENTRY_INSERT_BEFORE,
                                    self, label, before, func, *args, **kwargs)

    def item_insert_after(self, MultiButtonEntryItem after, label, func = None, *args, **kwargs):
        return MultiButtonEntryItem(ELM_MULTIBUTTONENTRY_INSERT_AFTER,
                                    self, label, after, func, *args, **kwargs)

    def items_get(self):
        cdef const_Eina_List *lst, *itr
        cdef void *data
        ret = []
        lst = elm_multibuttonentry_items_get(self.obj)
        itr = lst
        while itr:
            data = elm_object_item_data_get(<Elm_Object_Item *>itr.data)
            if data != NULL:
                (o, callback, it, a, ka) = <object>data
                ret.append(it)
            itr = itr.next
        return ret

    property items:
        def __get__(self):
            return self.items_get()

    def first_item_get(self):
        cdef Elm_Object_Item *obj
        cdef void *data
        obj = elm_multibuttonentry_first_item_get(self.obj)
        if obj == NULL:
            return None
        data = elm_object_item_data_get(obj)
        if data == NULL:
            return None
        (o, callback, it, a, ka) = <object>data
        return it

    property first_item:
        def __get__(self):
            return self.first_item_get()

    def last_item_get(self):
        cdef Elm_Object_Item *obj
        cdef void *data
        obj = elm_multibuttonentry_last_item_get(self.obj)
        if obj == NULL:
            return None
        data = elm_object_item_data_get(obj)
        if data == NULL:
            return None
        (o, callback, it, a, ka) = <object>data
        return it

    property last_item:
        def __get__(self):
            return self.last_item_get()

    def selected_item_get(self):
        cdef Elm_Object_Item *obj
        cdef void *data
        obj = elm_multibuttonentry_selected_item_get(self.obj)
        if obj == NULL:
            return None
        data = elm_object_item_data_get(obj)
        if data == NULL:
            return None
        (o, callback, it, a, ka) = <object>data
        return it

    property selected_item:
        def __get__(self):
            return self.selected_item_get()

    def clear(self):
        elm_multibuttonentry_clear(self.obj)

    def filter_append(self, func, *args, **kwargs):
        cbdata = (self, func, args, kwargs)
        elm_multibuttonentry_item_filter_append(self.obj,
                            _multibuttonentry_filter_callback, <void *>cbdata)
        Py_INCREF(cbdata)

    def filter_prepend(self, func, *args, **kwargs):
        cbdata = (self, func, args, kwargs)
        elm_multibuttonentry_item_filter_prepend(self.obj,
                            _multibuttonentry_filter_callback, <void *>cbdata)
        Py_INCREF(cbdata)

    def filter_remove(self, func, *args, **kwargs):
        #TODO
        pass

    def callback_item_selected_add(self, func, *args, **kwargs):
        self._callback_add("item,selected", func, *args, **kwargs)
    
    def callback_item_selected_del(self, func):
        self._callback_del("item,selected", func)

    def callback_item_added_add(self, func, *args, **kwargs):
        self._callback_add("item,added", func, *args, **kwargs)
    
    def callback_item_added_del(self, func):
        self._callback_del("item,added", func)

    def callback_item_deleted_add(self, func, *args, **kwargs):
        self._callback_add("item,deleted", func, *args, **kwargs)
    
    def callback_item_deleted_del(self, func):
        self._callback_del("item,deleted", func)

    def callback_item_clicked_add(self, func, *args, **kwargs):
        self._callback_add("item,clicked", func, *args, **kwargs)
    
    def callback_item_clicked_del(self, func):
        self._callback_del("item,clicked", func)

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)
    
    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_focused_add(self, func, *args, **kwargs):
        self._callback_add("focused", func, *args, **kwargs)
    
    def callback_focused_del(self, func):
        self._callback_del("focused", func)

    def callback_unfocused_add(self, func, *args, **kwargs):
        self._callback_add("unfocused", func, *args, **kwargs)
    
    def callback_unfocused_del(self, func):
        self._callback_del("unfocused", func)

    def callback_expanded_add(self, func, *args, **kwargs):
        self._callback_add("expanded", func, *args, **kwargs)
    
    def callback_expanded_del(self, func):
        self._callback_del("expanded", func)

    def callback_contracted_add(self, func, *args, **kwargs):
        self._callback_add("contracted", func, *args, **kwargs)
    
    def callback_contracted_del(self, func):
        self._callback_del("contracted", func)

    def callback_expand_state_changed_add(self, func, *args, **kwargs):
        self._callback_add("expand,state,changed", func, *args, **kwargs)
    
    def callback_expand_state_changed_del(self, func):
        self._callback_del("expand,state,changed", func)


_elm_widget_type_register("multibuttonentry", MultiButtonEntry)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryMultiButtonEntry_Type # hack to install metaclass
_install_metaclass(&PyElementaryMultiButtonEntry_Type, ElementaryObjectMeta)
