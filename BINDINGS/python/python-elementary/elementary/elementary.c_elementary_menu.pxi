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

cdef class MenuItem:
    cdef Elm_Menu_Item *obj
    cdef object cbt

    def __del_cb(self):
        self.obj = NULL
        self.cbt = None
        Py_DECREF(self)

    def __init__(self, c_evas.Object menu, MenuItem parent, label, icon,
                 callback, *args, **kargs):
        cdef Elm_Menu_Item *parent_obj = NULL
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
        elm_menu_item_del_cb_set(self.obj, _menu_item_del_cb)

    def delete(self):
        """Delete the menu item"""
        if self.obj == NULL:
            raise ValueError("Object already deleted")
        elm_menu_item_del(self.obj)

    def label_set(self, label):
        elm_menu_item_label_set(self.obj, label)

    def label_get(self):
        cdef const_char_ptr l
        l = elm_menu_item_label_get(self.obj)
        if l == NULL:
            return None
        return l

    property label:
        def __get__(self):
            return self.label_get()

        def __set__(self, value):
            self.label_set(value)

    def icon_name_set(self, icon):
        elm_menu_item_object_icon_name_set(self.obj, icon)

    property icon_name_set:
        def __set__(self, icon):
            self.icon_name_set(icon)

    def icon_set(self, icon):
        _METHOD_DEPRECATED(self, "icon_name_set")
        self.icon_name_set(icon)

    property icon:
        def __set__(self, icon):
            self.icon_set(icon)

    def disabled_set(self, disabled):
        elm_menu_item_disabled_set(self.obj, disabled)

    property disabled:
        def __set__(self, disabled):
            elm_menu_item_disabled_set(self.obj, disabled)

    def data_get(self):
        """Returns the callback data given at creation time.

        @rtype: tuple of (args, kargs), args is tuple, kargs is dict.
        """
        cdef void* data
        data = elm_menu_item_data_get(self.obj)
        if data == NULL:
            return None
        else:
            (obj, callback, it, a, ka) = <object>data
            return (a, ka)

    property data:
        def __get__(self):
            return self.data_get()

    def subitems_get(self):
        cdef evas.c_evas.const_Eina_List *lst, *itr
        cdef void *data
        ret = []
        lst = elm_menu_item_subitems_get(self.obj)
        itr = lst
        while itr:
            data = elm_menu_item_data_get(<Elm_Menu_Item *>itr.data)
            if data != NULL:
                (o, callback, it, a, ka) = <object>data
                ret.append(it)
            itr = itr.next
        return ret

    property subitems:
        def __get__(self):
            return self.subitems_get()

cdef void _menu_item_separator_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
    it = <object>data
    it.__del_cb()

cdef class MenuItemSeparator:
    cdef Elm_Menu_Item *obj

    def __del_cb(self):
        self.obj = NULL
        Py_DECREF(self)

    def __init__(self, c_evas.Object menu, MenuItem parent):
        cdef Elm_Menu_Item *parent_obj = NULL

        if parent:
            parent_obj = parent.obj
        self.obj = elm_menu_item_separator_add(menu.obj, parent_obj)
        if not self.obj:
            raise RuntimeError("Error creating separator")

        elm_menu_item_data_set(self.obj, <void*>self)
        Py_INCREF(self)
        elm_menu_item_del_cb_set(self.obj, _menu_item_separator_del_cb)

    def delete(self):
        """Delete the menu item"""
        if self.obj == NULL:
            raise ValueError("Object already deleted")
        elm_menu_item_del(self.obj)


cdef class Menu(Object):
    def __init__(self, c_evas.Object parent, obj = None):
        if obj is None:
            Object.__init__(self, parent.evas)
            self._set_obj(elm_menu_add(parent.obj))
        else:
            self._set_obj(<c_evas.Evas_Object*>obj)

    def parent_set(self, c_evas.Object parent):
        elm_menu_parent_set(self.obj, parent.obj)

    property parent:
        def __set__(self, c_evas.Object parent):
            elm_menu_parent_set(self.obj, parent.obj)

    def move(self, x, y):
        elm_menu_move(self.obj, x, y)

    def item_add(self, parent = None, label = None, icon = None, callback = None, *args, **kwargs):
        return MenuItem(self, parent, label, icon, callback, *args, **kwargs)

    def item_separator_add(self, item = None):
        return MenuItemSeparator(self, item)


_elm_widget_type_register("menu", Menu)
