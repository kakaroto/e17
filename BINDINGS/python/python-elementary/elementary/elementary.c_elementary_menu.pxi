# Copyright (c) 2010 Boris Faure
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

cdef void _menu_callback(void *cbt, c_evas.Evas_Object *obj, void *event_info) with gil:
    try:
        (menu, callback, it, a, ka) = <object>cbt
        callback(menu, it, *a, **ka)
    except Exception, e:
        traceback.print_exc()

cdef void _menu_item_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
    (obj, callback, it, a, ka) = <object>data
    it.__del_cb()

cdef class MenuItem(ObjectItem):
    cdef object cbt

    def __del_cb(self):
        self.obj = NULL
        self.cbt = None
        Py_DECREF(self)

    def __init__(self, c_evas.Object menu, MenuItem parent, label, icon,
                 callback, *args, **kargs):
        cdef Elm_Object_Item *parent_obj = NULL
        cdef void* cbdata = NULL
        cdef void (*cb) (void *, c_evas.Evas_Object *, void *)
        cb = NULL

        if parent:
            parent_obj = parent.obj

        if callback:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _menu_callback

        self.cbt = (menu, callback, self, args, kargs)
        cbdata = <void*>self.cbt
        self.obj = elm_menu_item_add(menu.obj, parent_obj, icon, label,
                                          cb, cbdata)

        Py_INCREF(self)
        elm_object_item_del_cb_set(self.obj, _menu_item_del_cb)

    def object_get(self):
        return <Object>elm_menu_item_object_get(self.obj)

    def icon_name_set(self, icon):
        elm_menu_item_icon_name_set(self.obj, icon)

    def icon_name_get(self):
        return elm_menu_item_icon_name_get(self.obj)

    property icon_name:
        def __get__(self):
            return self.icon_name_get()
        def __set__(self, icon):
            self.icon_name_set(icon)

    def selected_set(self, selected):
        elm_menu_item_selected_set(self.obj, selected)

    def selected_get(self):
        return elm_menu_item_selected_get(self.obj)

    property selected:
        def __get__(self):
            return self.selected_get()
        def __set__(self, selected):
            self.selected_set(selected)

    def is_separator(self):
        return False

    def subitems_get(self):
        cdef evas.c_evas.const_Eina_List *lst, *itr
        cdef void *data
        ret = []
        lst = elm_menu_item_subitems_get(self.obj)
        itr = lst
        while itr:
            data = elm_object_item_data_get(<Elm_Object_Item *>itr.data)
            if data != NULL:
                (o, callback, it, a, ka) = <object>data
                ret.append(it)
            itr = itr.next
        return ret

    property subitems:
        def __get__(self):
            return self.subitems_get()

    def index_get(self):
        return elm_menu_item_index_get(self.obj)

    def next_get(self):
        cdef Elm_Object_Item *it
        it = elm_menu_item_next_get(self.obj)
        return _elm_menu_item_to_python(it)

    def prev_get(self):
        cdef Elm_Object_Item *it
        it = elm_menu_item_prev_get(self.obj)
        return _elm_menu_item_to_python(it)

cdef _elm_menu_item_to_python(Elm_Object_Item *it):
    cdef void *data
    cdef object prm
    if it == NULL:
        return None
    data = elm_object_item_data_get(it)
    if data == NULL:
        return None
    prm = <object>data
    return prm[2]

cdef void _menu_item_separator_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
    it = <object>data
    it.__del_cb()

cdef class MenuItemSeparator:
    cdef Elm_Object_Item *obj

    def __del_cb(self):
        self.obj = NULL
        Py_DECREF(self)

    def __init__(self, c_evas.Object menu, MenuItem parent):
        cdef Elm_Object_Item *parent_obj = NULL

        if parent:
            parent_obj = parent.obj
        self.obj = elm_menu_item_separator_add(menu.obj, parent_obj)
        if not self.obj:
            raise RuntimeError("Error creating separator")

        elm_object_item_data_set(self.obj, <void*>self)
        Py_INCREF(self)
        elm_object_item_del_cb_set(self.obj, _menu_item_separator_del_cb)

    def is_separator(self):
        return True

cdef class Menu(Object):
    def __init__(self, c_evas.Object parent, obj = None):
        if obj is None:
            Object.__init__(self, parent.evas)
            self._set_obj(elm_menu_add(parent.obj))
        else:
            self._set_obj(<c_evas.Evas_Object*>obj)

    def parent_set(self, c_evas.Object parent):
        elm_menu_parent_set(self.obj, parent.obj)

    def parent_get(self):
        cdef c_evas.Evas_Object *o
        o = elm_menu_parent_get(self.obj)
        return evas.c_evas._Object_from_instance(<long>o)

    property parent:
        def __get__(self):
            return self.parent_get()
        def __set__(self, parent):
            self.parent_set(parent)

    def move(self, x, y):
        elm_menu_move(self.obj, x, y)

    def close(self):
        elm_menu_close(self.obj)

    def items_get(self):
        cdef Elm_Object_Item *it
        cdef c_evas.const_Eina_List *lst
        cdef void *data
        cdef object prm

        lst = elm_menu_items_get(self.obj)
        ret = []
        ret_append = ret.append
        while lst:
            it = <Elm_Object_Item *>lst.data
            lst = lst.next
            o = _elm_menu_item_to_python(it)
            if o is not None:
                ret_append(o)
        return ret

    def item_add(self, parent = None, label = None, icon = None, callback = None, *args, **kwargs):
        return MenuItem(self, parent, label, icon, callback, *args, **kwargs)

    def item_separator_add(self, item = None):
        return MenuItemSeparator(self, item)

    def selected_item_get(self):
        cdef Elm_Object_Item *it
        it = elm_menu_selected_item_get(self.obj)
        return _elm_toolbar_item_to_python(it)

    property selected_item:
        def __get__(self):
            return self.selected_item_get()

    def last_item_get(self):
        cdef Elm_Object_Item *it
        it = elm_menu_last_item_get(self.obj)
        return _elm_menu_item_to_python(it)

    property last_item:
        def __get__(self):
            return self.last_item_get()

    def first_item_get(self):
        cdef Elm_Object_Item *it
        it = elm_menu_first_item_get(self.obj)
        return _elm_menu_item_to_python(it)

    property first_item:
        def __get__(self):
            return self.first_item_get()

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

_elm_widget_type_register("menu", Menu)
