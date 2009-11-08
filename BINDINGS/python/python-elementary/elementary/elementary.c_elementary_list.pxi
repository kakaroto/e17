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
        (obj, callback, data, it) = <object>cbt
        if not callable(callback):
            raise TypeError("callback is not callable")
        callback(obj, "clicked" , data)
    except Exception, e:
        traceback.print_exc()

cdef class ListItem:
    """
    An item for the list widget
    """
    cdef Elm_List_Item *item
    cdef c_evas.Evas_Object *list
    cdef object cbt

    def __new__(self):
        self.item = NULL

    def __init__(self):
        pass

    def append(self, c_evas.Object list, label, c_evas.Object icon = None,
               c_evas.Object end = None, callback = None, data = None):
        if not self.item == NULL:
            raise Exception("Item was already created!")

        cdef c_evas.Evas_Object* icon_obj
        cdef c_evas.Evas_Object* end_obj
        icon_obj = NULL
        end_obj = NULL

        if icon is not None:
            icon_obj = icon.obj
        if end is not None:
            end_obj = end.obj

        if callback is not None:
            self.cbt = (list, callback, data, self)
            self.item = elm_list_item_append(list.obj, label, icon_obj, end_obj,
                                             _list_callback, <void*>self.cbt)
        else:
            self.item = elm_list_item_append(list.obj, label, icon_obj, end_obj,
                                             NULL, NULL)

    def prepend(self, c_evas.Object list, label, c_evas.Object icon = None,
                c_evas.Object end = None, callback = None, data = None):
        if not self.item == NULL:
            raise Exception("Item was already created!")

        cdef c_evas.Evas_Object* icon_obj
        cdef c_evas.Evas_Object* end_obj
        icon_obj = NULL
        end_obj = NULL

        if icon is not None:
            icon_obj = icon.obj
        if end is not None:
            end_obj = end.obj

        if callback is not None:
            self.cbt = (list, callback, data, self)
            self.item = elm_list_item_prepend(list.obj, label, icon_obj, end_obj,
                                              _list_callback, <void*>self.cbt)
        else:
            self.item = elm_list_item_prepend(list.obj, label, icon_obj, end_obj,
                                              NULL, NULL)

    def insert_before(self, c_evas.Object list, ListItem before, label,
                      c_evas.Object icon = None, c_evas.Object end = None,
                      callback = None, data = None):
        if not self.item == NULL:
            raise Exception("Item was already created!")

        if before == None:
            raise ValueError("need a valid before object to add an item before another item")

        cdef c_evas.Evas_Object* icon_obj
        cdef c_evas.Evas_Object* end_obj
        icon_obj = NULL
        end_obj = NULL

        if icon is not None:
            icon_obj = icon.obj
        if end is not None:
            end_obj = end.obj

        if callback is not None:
            self.cbt = (list, callback, data, self)
            self.item = elm_list_item_insert_before(list.obj, before.item, label,
                                                    icon_obj, end_obj,
                                                    _list_callback, <void*>self.cbt)
        else:
            self.item = elm_list_item_insert_before(list.obj, before.item, label,
                                                    icon_obj, end_obj, NULL, NULL)

    def insert_after(self, c_evas.Object list, ListItem after, label,
                     c_evas.Object icon = None, c_evas.Object end = None,
                     callback = None, data = None):
        if not self.item == NULL:
            raise Exception("Item was already created!")

        if after == None:
            raise ValueError("need a valid after object to add an item after another item")

        cdef c_evas.Evas_Object* icon_obj
        cdef c_evas.Evas_Object* end_obj
        icon_obj = NULL
        end_obj = NULL

        if icon is not None:
            icon_obj = icon.obj
        if end is not None:
            end_obj = end.obj

        if callback is not None:
            self.cbt = (list, callback, data, self)
            self.item = elm_list_item_insert_after(list.obj, after.item, label,
                                                   icon_obj, end_obj,
                                                   _list_callback, <void*>self.cbt)
        else:
            self.item = elm_list_item_insert_after(list.obj, after.item, label,
                                                   icon_obj, end_obj, NULL, NULL)

    def selected_set(self, selected):
        elm_list_item_selected_set(self.item, selected)

    def show(self):
        elm_list_item_show(self.item)

    def delete(self):
        elm_list_item_del(self.item)

    def data_get(self):
        cdef void* data
        data = elm_list_item_data_get(self.item)
        if data == NULL:
            return None
        else:
            return <object>data

    def icon_get(self):
        cdef c_evas.Evas_Object *icon
        icon = elm_list_item_icon_get(self.item)
        return evas.c_evas._Object_from_instance(<long> icon)

    # TODO
    def end_get(self):
        return None

    # TODO
    def base_get(self):
        return None

    def label_get(self):
        return elm_list_item_label_get(self.item)

    def label_set(self, label):
        elm_list_item_label_set(self.item, label)

    def prev(self):
        self.item = elm_list_item_prev(self.item)

    def next(self):
        self.item = elm_list_item_next(self.item)

cdef class List(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_list_add(parent.obj))

    def item_append(self, label, c_evas.Object icon = None,
                    c_evas.Object end = None, callback = None, data = None):
        item = ListItem()
        item.append(self, label, icon, end, callback, data)
        return item

    def item_prepend(self, label, c_evas.Object icon = None,
                     c_evas.Object end = None, callback = None, data = None):
        item = ListItem()
        item.prepend(self, label, icon, end, data)
        return item

    def item_insert_before(self, ListItem before, label, c_evas.Object icon = None,
                           c_evas.Object end = None, callback = None, data = None):
        item = ListItem()
        item.insert_before(self, before, label, icon, end, callback, data)
        return item

    def item_insert_after(self, ListItem after, label, c_evas.Object icon = None,
                          c_evas.Object end = None, callback = None, data = None):
        item = ListItem()
        item.insert_after(self, after, label, icon, end, callback, data)
        return item

    def clear(self):
        elm_list_clear(self.obj)

    def go(self):
        elm_list_go(self.obj)

    property clicked:
        def __set__(self, value):
            self._callback_add("clicked", value)

    property selected:
        def __set__(self, value):
            self._callback_add("selected", value)

    property unselected:
        def __set__(self, value):
            self._callback_add("unselected", value)
