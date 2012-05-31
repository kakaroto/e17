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

cdef class ToolbarItem(ObjectItem):
    """
    A item for the toolbar
    """
    cdef object cbt

    def __del_cb(self):
        self.obj = NULL
        self.cbt = None
        Py_DECREF(self)

    def __init__(self, c_evas.Object toolbar, icon, label,
                 callback, *args, **kargs):
        cdef c_evas.Evas_Object *ic = NULL
        cdef void* cbdata = NULL
        cdef void (*cb) (void *, c_evas.Evas_Object *, void *)
        cb = NULL

        if callback:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _hoversel_callback

        self.cbt = (toolbar, callback, self, args, kargs)
        cbdata = <void*>self.cbt
        self.obj = elm_toolbar_item_append(toolbar.obj, icon, label, cb, cbdata)

        Py_INCREF(self)
        elm_object_item_del_cb_set(self.obj, _toolbar_item_del_cb)

    def next_get(self):
        cdef Elm_Object_Item *it
        it = elm_toolbar_item_next_get(self.obj)
        return _elm_toolbar_item_to_python(it)

    property next:
        def __get__(self):
            return self.next_get()

    def prev_get(self):
        cdef Elm_Object_Item *it
        it = elm_toolbar_item_prev_get(self.obj)
        return _elm_toolbar_item_to_python(it)

    property prev:
        def __get__(self):
            return self.prev_get()

    def priority_set(self, priority):
        elm_toolbar_item_priority_set(self.obj, priority)

    def priority_get(self):
        return elm_toolbar_item_priority_get(self.obj)

    property priority:
        def __get__(self):
            return self.priority_get()

        def __set__(self, priority):
            self.priority_set(priority)

    def selected_set(self, selected):
        """Select the item"""
        elm_toolbar_item_selected_set(self.obj, selected)

    def selected_get(self):
        return elm_toolbar_item_selected_get(self.obj)

    property selected:
        def __set__(self, selected):
            elm_toolbar_item_selected_set(self.obj, selected)

        def __get__(self):
            return elm_toolbar_item_selected_get(self.obj)

    def icon_set(self, ic):
        elm_toolbar_item_icon_set(self.obj, ic)

    def icon_get(self):
        cdef const_char_ptr i
        i = elm_toolbar_item_icon_get(self.obj)
        if i == NULL:
            return None
        return i

    property icon:
        def __get__(self):
            return self.icon_get()

        def __set__(self, ic):
            self.icon_set(ic)

    def disabled_set(self, disabled):
        elm_object_item_disabled_set(self.obj, disabled)

    def disabled_get(self):
        return elm_object_item_disabled_get(self.obj)

    property disabled:
        def __get__(self):
            return self.disabled_get()

        def __set__(self, value):
            self.disabled_set(value)

    def object_get(self):
        cdef c_evas.Evas_Object *obj = elm_toolbar_item_object_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    def icon_object_get(self):
        cdef c_evas.Evas_Object *obj = elm_toolbar_item_icon_object_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    #TODO def icon_memfile_set(self, img, size, format, key):
        #elm_toolbar_item_icon_memfile_set(self.obj, img, size, format, key)

    def icon_file_set(self, file, key):
        elm_toolbar_item_icon_file_set(self.obj, file, key)

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

    property menu:
        def __get__(self):
            return self.menu_get()

        def __set__(self, value):
            self.menu_set(value)


    #TODO def state_add(self, icon, label, func, data):
        #return elm_toolbar_item_state_add(self.obj, icon, label, func, data)

    #TODO def state_del(self, state):
        #return bool(elm_toolbar_item_state_del(self.obj, state))

    #TODO def state_set(self, state):
        #return bool(elm_toolbar_item_state_set(self.obj, state))

    #TODO def state_unset(self):
        #elm_toolbar_item_state_unset(self.obj)

    #TODO def state_get(self):
        #return elm_toolbar_item_state_get(self.obj)

    #TODO def state_next(self):
        #return elm_toolbar_item_state_next(self.obj)

    #TODO def state_prev(self):
        #return elm_toolbar_item_state_prev(self.obj)

cdef _elm_toolbar_item_to_python(Elm_Object_Item *it):
    cdef void *data
    cdef object prm
    if it == NULL:
        return None
    data = elm_object_item_data_get(it)
    if data == NULL:
        return None
    prm = <object>data
    return prm[2]

cdef public class Toolbar(Object) [object PyElementaryToolbar, type PyElementaryToolbar_Type]:
    """
    A toolbar
    """
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_toolbar_add(parent.obj))

    def icon_size_set(self, icon_size):
        elm_toolbar_icon_size_set(self.obj, icon_size)

    def icon_size_get(self):
        return elm_toolbar_icon_size_get(self.obj)

    property icon_size:
        def __set__(self, icon_size):
            elm_toolbar_icon_size_set(self.obj, icon_size)

        def __get__(self):
            return elm_toolbar_icon_size_get(self.obj)

    def icon_order_lookup_set(self, order):
        elm_toolbar_icon_order_lookup_set(self.obj, order)

    def icon_order_lookup_get(self):
        return elm_toolbar_icon_order_lookup_get(self.obj)

    property icon_order_lookup:
        def __set__(self, order):
            elm_toolbar_icon_order_lookup_set(self.obj, order)

        def __get__(self):
            return elm_toolbar_icon_order_lookup_get(self.obj)

    def item_append(self, icon, label, callback = None, *args, **kargs):
        """Appends a new item to the toolbar

        @parm: L{icon} icon for the item
        @parm: L{label} label for the item
        @parm: L{callback} function to click if the user clicked on the item
        """
        # Everything is done in the ToolbarItem class, because of wrapping the
        # C structures in python classes
        return ToolbarItem(self, icon, label, callback, *args, **kargs)

    #TODO: def item_prepend(self, icon, label, callback = None, *args, **kargs):
        #return ToolbarItem(self, icon, label, callback, *args, **kargs)

    #TODO: def item_insert_before(self, before, icon, label, callback = None, *args, **kargs):
        #return ToolbarItem(self, icon, label, callback, *args, **kargs)

    #TODO: def item_insert_after(self, after, icon, label, callback = None, *args, **kargs):
        #return ToolbarItem(self, icon, label, callback, *args, **kargs)

    def menu_parent_set(self, c_evas.Object parent):
        elm_toolbar_menu_parent_set(self.obj, parent.obj)

    def menu_parent_get(self):
        cdef c_evas.Evas_Object *parent = elm_toolbar_menu_parent_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> parent)

    #TODO elm_toolbar_item_find_by_label(self, label):
        #return elm_toolbar_item_find_by_label(self.obj, label)

    property menu_parent:
        def __get__(self):
            return self.menu_parent_get()

        def __set__(self, value):
            self.menu_parent_set(value)

    def homogeneous_set(self, homogeneous):
        elm_toolbar_homogeneous_set(self.obj, homogeneous)

    def homogeneous_get(self):
        return elm_toolbar_homogeneous_get(self.obj)

    property homogeneous:
        def __set__(self, homogeneous):
            elm_toolbar_homogeneous_set(self.obj, homogeneous)

        def __get__(self):
            return elm_toolbar_homogeneous_get(self.obj)

    def shrink_mode_set(self, mode):
        elm_toolbar_shrink_mode_set(self.obj, mode)

    def shrink_mode_get(self):
        return elm_toolbar_shrink_mode_get(self.obj)

    property shrink_mode:
        def __get__(self):
            return self.shrink_mode_get()

        def __set__(self, value):
            self.shrink_mode_set(value)

    def horizontal_set(self, horizontal):
        elm_toolbar_horizontal_set(self.obj, horizontal)

    def horizontal_get(self):
        return elm_toolbar_horizontal_get(self.obj)

    property horizontal:
        def __set__(self, horizontal):
            elm_toolbar_horizontal_set(self.obj, horizontal)

        def __get__(self):
            return elm_toolbar_horizontal_get(self.obj)

    def align_set(self, align):
        elm_toolbar_align_set(self.obj, align)

    def align_get(self):
        return elm_toolbar_align_get(self.obj)

    property align:
        def __set__(self, align):
            elm_toolbar_align_set(self.obj, align)

        def __get__(self):
            return elm_toolbar_align_get(self.obj)

    def selected_item_get(self):
        cdef Elm_Object_Item *it
        it = elm_toolbar_selected_item_get(self.obj)
        return _elm_toolbar_item_to_python(it)

    property selected_item:
        def __get__(self):
            return self.selected_item_get()

    def first_item_get(self):
        cdef Elm_Object_Item *it
        it = elm_toolbar_first_item_get(self.obj)
        return _elm_toolbar_item_to_python(it)

    property first_item:
        def __get__(self):
            return self.first_item_get()

    def last_item_get(self):
        cdef Elm_Object_Item *it
        it = elm_toolbar_last_item_get(self.obj)
        return _elm_toolbar_item_to_python(it)

    property last_item:
        def __get__(self):
            return self.last_item_get()

    def items_count(self):
        return elm_toolbar_items_count(self.obj)

    def select_mode_set(self, mode):
        elm_toolbar_select_mode_set(self.obj, mode)

    def select_mode_get(self):
        return elm_toolbar_select_mode_get(self.obj)

    property select_mode:
        def __get__(self):
            return self.select_mode_get()

        def __set__(self, value):
            self.select_mode_set(value)

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_longpressed_add(self, func, *args, **kwargs):
        self._callback_add("longpressed", func, *args, **kwargs)

    def callback_longpressed_del(self, func):
        self._callback_del("longpressed", func)

    def callback_language_changed_add(self, func, *args, **kwargs):
        self._callback_add("language,changed", func, *args, **kwargs)

    def callback_language_changed_del(self, func):
        self._callback_del("language,changed", func)

_elm_widget_type_register("toolbar", Toolbar)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryToolbar_Type # hack to install metaclass
_install_metaclass(&PyElementaryToolbar_Type, ElementaryObjectMeta)
