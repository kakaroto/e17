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
    (hoversel, callback, data, it) = <object>cbt
    callback(data, hoversel, it)

cdef class HoverselItem:
    """A item for the hoversel widget"""
    cdef Elm_Hoversel_Item *item
    cdef object cbt

    def __init__(self, c_evas.Object hoversel, label, icon_file, icon_type, callback, data = None):
        cdef char *i_f = NULL
        if icon_file:
           i_f = icon_file

        if callback:
            self.cbt = (hoversel, callback, data, self)
            self.item = elm_hoversel_item_add(hoversel.obj, label, i_f,
                                              icon_type,_hoversel_callback,
                                              <void*>self.cbt)
        else:
            self.item = elm_hoversel_item_add(hoversel.obj, label, i_f,
                                              icon_type, NULL, NULL)


    def delete(self):
        """Delete the hoversel item"""
        elm_hoversel_item_del(self.item)
        self.cbt = None

    def icon_set(self, icon_file, icon_group, icon_type):
        elm_hoversel_item_icon_set(self.item, icon_file, icon_group, icon_type)

    def icon_get(self):
        cdef char *cicon_file
        cdef char *cicon_group
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
        return elm_hoversel_item_label_get(self.item)

cdef class Hoversel(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_hoversel_add(parent.obj))

    property clicked:
        def __set__(self, value):
            self._callback_add("clicked",value)

    property selected:
        def __set__(self, value):
            self._callback_add("selected", value)

    property dismissed:
        def __set__(self, value):
            self._callback_add("dismissed", value)

    def hover_parent_set(self, c_evas.Object parent):
        elm_hoversel_hover_parent_set(self.obj, parent.obj)

    def label_set(self, label):
        elm_hoversel_label_set(self.obj, label)

    def label_get(self):
        return elm_hoversel_label_get(self.obj)

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

    def item_add(self, label, icon_file = None, icon_type = ELM_ICON_NONE, callback = None, data = None):
        return HoverselItem(self, label, icon_file, icon_type, callback, data)


