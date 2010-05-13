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

cdef void _toolbar_callback(void *cbt, c_evas.Evas_Object *obj, void *event_info) with gil:
    try:
        (toolbar, callback, it, a, ka) = <object>cbt
        callback(toolbar, it, *a, **ka)
    except Exception, e:
        traceback.print_exc()

cdef void _toolbar_item_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
    (obj, callback, it, a, ka) = <object>data
    it.__del_cb()

cdef class ToolbarItem:
    """
    A item for the toolbar
    """
    cdef Elm_Toolbar_Item *obj
    cdef object cbt

    def __del_cb(self):
        self.obj = NULL
        self.cbt = None
        Py_DECREF(self)


    def __init__(self, c_evas.Object toolbar, c_evas.Object icon, label,
                 callback, *args, **kargs):
        cdef c_evas.Evas_Object *ic = NULL
        cdef void* cbdata = NULL
        cdef void (*cb) (void *, c_evas.Evas_Object *, void *)
        cb = NULL

        if icon is not None:
           ic = icon.obj

        if callback:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _hoversel_callback

        self.cbt = (toolbar, callback, self, args, kargs)
        cbdata = <void*>self.cbt
        self.obj = elm_toolbar_item_add(toolbar.obj, ic, label, cb, cbdata)

        Py_INCREF(self)
        elm_toolbar_item_del_cb_set(self.obj, _toolbar_item_del_cb)

    def delete(self):
        """Delete the item"""
        if self.obj == NULL:
            raise ValueError("Object already deleted")
        elm_toolbar_item_del(self.obj)

    def icon_get(self):
        cdef c_evas.Evas_Object *icon
        icon = elm_toolbar_item_icon_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> icon)

    property icon:
        def __get__(self):
            return self.icon_get()

    def label_set(self, label):
        elm_toolbar_item_label_set(self.obj, label)

    def label_get(self):
        cdef char *l
        l = elm_toolbar_item_label_get(self.obj)
        if l == NULL:
            return None
        return l

    property label:
        def __get__(self):
            return self.label_get()

        def __set__(self, value):
            self.label_set(value)

    def select(self):
        """Select the item"""
        elm_toolbar_item_select(self.obj)

    def disabled_set(self, disabled):
        elm_toolbar_item_disabled_set(self.obj, disabled)

    def disabled_get(self):
        return elm_toolbar_item_disabled_get(self.obj)

    property disabled:
        def __set__(self, disabled):
            elm_toolbar_item_disabled_set(self.obj, disabled)

        def __get__(self):
            return elm_toolbar_item_disabled_get(self.obj)

    def separator_set(self, separator):
        elm_toolbar_item_separator_set(self.obj, separator)

    def separator_get(self):
        return elm_toolbar_item_separator_get(self.obj)

    property separator:
        def __set__(self, separator):
            elm_toolbar_item_separator_set(self.obj, separator)

        def __get__(self):
            return elm_toolbar_item_separator_get(self.obj)

    def menu_set(self, menu):
        elm_toolbar_item_menu_set(self.obj, menu)

    def menu_get(self):
        cdef c_evas.Evas_Object *menu
        menu = elm_toolbar_item_menu_get(self.obj)
        if menu == NULL:
            return None
        else:
            return Menu(None, <object>menu)

cdef class Toolbar(Object):
    """
    A toolbar
    """
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_toolbar_add(parent.obj))


    def scrollable_set(self, scrollable):
        """
        Set the scrollable property

        @parm: L{scrollable}
        """
        elm_toolbar_scrollable_set(self.obj, scrollable)

    property scrollable:
        def __set__(self, scrollable):
            elm_toolbar_scrollable_set(self.obj, scrollable)


    def menu_parent_set(self, c_evas.Object parent):
        elm_toolbar_menu_parent_set(self.obj, parent.obj)

    property menu_parent:
        def __set__(self, c_evas.Object parent):
            elm_toolbar_menu_parent_set(self.obj, parent.obj)


    def homogenous_set(self, homogenous):
        elm_toolbar_homogenous_set(self.obj, homogenous)

    property homogenous:
        def __set__(self, homogenous):
            elm_toolbar_homogenous_set(self.obj, homogenous)


    def align_set(self, align):
        elm_toolbar_align_set(self.obj, align)

    property align:
        def __set__(self, align):
            elm_toolbar_align_set(self.obj, align)


    def icon_size_set(self, icon_size):
        elm_toolbar_icon_size_set(self.obj, icon_size)

    def icon_size_get(self, icon_size):
        return elm_toolbar_icon_size_get(self.obj)

    property icon_size:
        def __set__(self, icon_size):
            elm_toolbar_icon_size_set(self.obj, icon_size)

        def __get__(self):
            return elm_toolbar_icon_size_get(self.obj)

    def item_unselect_all(self):
        elm_toolbar_item_unselect_all(self.obj)

    def item_add(self, c_evas.Object icon, label, callback = None, *args, **kargs):
        """
        Adds a new item to the toolbar

        @note: Never pass the the same icon object to more than one item. For
               a every item you must create a new icon!

        @parm: L{icon} icon for the item
        @parm: L{label} label for the item
        @parm: L{callback} function to click if the user clicked on the item
        """
        # Everything is done in the ToolbarItem class, because of wrapping the
        # C structures in python classes
        return ToolbarItem(self, icon, label, callback, *args, **kargs)

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)


_elm_widget_type_register("toolbar", Toolbar)
