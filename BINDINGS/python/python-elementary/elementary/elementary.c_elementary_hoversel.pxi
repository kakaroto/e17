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

cdef void _hoversel_callback(void *cbt, c_evas.Evas_Object *obj, void *event_info) with gil:
    try:
        (hoversel, callback, it, a, ka) = <object>cbt
        callback(hoversel, it, *a, **ka)
    except Exception, e:
        traceback.print_exc()

cdef void _hoversel_item_del_cb(void *data, c_evas.Evas_Object *o, void *event_info) with gil:
    (obj, callback, it, a, ka) = <object>data
    it.__del_cb()


cdef class HoverselItem:
    """A item for the hoversel widget"""
    cdef Elm_Hoversel_Item *item
    cdef object cbt

    def __del_cb(self):
        self.item = NULL
        self.cbt = None
        Py_DECREF(self)

    def __init__(self, c_evas.Object hoversel, label, icon_file, icon_type,
                 callback, *args, **kargs):
        cdef char *i_f = NULL
        cdef void* cbdata = NULL
        cdef void (*cb) (void *, c_evas.Evas_Object *, void *)
        cb = NULL

        if icon_file:
           i_f = icon_file

        if callback:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _hoversel_callback

        self.cbt = (hoversel, callback, self, args, kargs)
        cbdata = <void*>self.cbt
        self.item = elm_hoversel_item_add(hoversel.obj, label, i_f, icon_type,
                                          cb, cbdata)

        Py_INCREF(self)
        elm_hoversel_item_del_cb_set(self.item, _hoversel_item_del_cb)

    def delete(self):
        """Delete the hoversel item"""
        if self.item == NULL:
            raise ValueError("Object already deleted")
        elm_hoversel_item_del(self.item)

    def icon_set(self, icon_file, icon_group, icon_type):
        elm_hoversel_item_icon_set(self.item, icon_file, icon_group, icon_type)

    def icon_get(self):
        cdef const_char_ptr cicon_file
        cdef const_char_ptr cicon_group
        cdef Elm_Icon_Type cicon_type
        icon_file = None
        icon_group = None
        elm_hoversel_item_icon_get(self.item, &cicon_file, &cicon_group, &cicon_type)
        if cicon_file != NULL:
            icon_file = cicon_file
        if cicon_group != NULL:
            icon_group = cicon_group
        return (icon_file, icon_group, cicon_type)

    def label_get(self):
        cdef const_char_ptr l
        l = elm_hoversel_item_label_get(self.item)
        if l == NULL:
            return None
        return l

cdef class Hoversel(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_hoversel_add(parent.obj))

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_selected_add(self, func, *args, **kwargs):
        self._callback_add("selected", func, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del("selected", func)

    def callback_dismissed_add(self, func, *args, **kwargs):
        self._callback_add("dismissed", func, *args, **kwargs)

    def callback_dismissed_del(self, func):
        self._callback_del("dismissed", func)


    def hover_parent_set(self, c_evas.Object parent):
        elm_hoversel_hover_parent_set(self.obj, parent.obj)

    def label_set(self, label):
        _METHOD_DEPRECATED(self, "text_set")
        self.text_set(label)

    def label_get(self):
        _METHOD_DEPRECATED(self, "text_get")
        return self.text_get()

    property label:
        def __get__(self):
            return self.label_get()

        def __set__(self, value):
            self.label_set(value)

    def icon_set(self, c_evas.Object icon):
        elm_hoversel_icon_set(self.obj, icon.obj)

    def icon_get(self):
        cdef c_evas.Evas_Object *icon
        icon = elm_hoversel_icon_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> icon)

    property icon:
        def __get__(self):
            return self.icon_get()

        def __set__(self, value):
            self.icon_set(value)

    def hover_begin(self):
        elm_hoversel_hover_begin(self.obj)

    def hover_end(self):
        elm_hoversel_hover_end(self.obj)

    def clear(self):
        elm_hoversel_clear(self.obj)

    def item_add(self, label, icon_file = None, icon_type = ELM_ICON_NONE, callback = None, *args, **kwargs):
        return HoverselItem(self, label, icon_file, icon_type, callback, *args, **kwargs)


_elm_widget_type_register("hoversel", Hoversel)
