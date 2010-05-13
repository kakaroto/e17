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

def Entry_markup_to_utf8(str):
    cdef char* string
    string = elm_entry_markup_to_utf8(str)
    if string == NULL:
        return None
    return string

def Entry_utf8_to_markup(str):
    cdef char* string
    string = elm_entry_utf8_to_markup(str)
    if string == NULL:
        return None
    return string


cdef class Entry(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_entry_add(parent.obj))

    def callback_changed_add(self, func, *args, **kwargs):
        self._callback_add("changed", func, *args, **kwargs)

    def callback_changed_del(self, func):
        self._callback_del("changed", func)

    def callback_selection_start_add(self, func, *args, **kwargs):
        self._callback_add("selection,start", func, *args, **kwargs)

    def callback_selection_start_del(self, func):
        self._callback_del("selection,start", func)

    def callback_selection_changed_add(self, func, *args, **kwargs):
        self._callback_add("selection,changed", func, *args, **kwargs)

    def callback_selection_changed_del(self, func):
        self._callback_del("selection,changed", func)

    def callback_selection_cleared_add(self, func, *args, **kwargs):
        self._callback_add("selection,cleared", func, *args, **kwargs)

    def callback_selection_cleared_del(self, func):
        self._callback_del("selection,cleared", func)

    def callback_selection_paste_add(self, func, *args, **kwargs):
        self._callback_add("selection,paste", func, *args, **kwargs)

    def callback_selection_paste_del(self, func):
        self._callback_del("selection,paste", func)

    def callback_selection_copy_add(self, func, *args, **kwargs):
        self._callback_add("selection,copy", func, *args, **kwargs)

    def callback_selection_copy_del(self, func):
        self._callback_del("selection,copy", func)

    def callback_selection_cut_add(self, func, *args, **kwargs):
        self._callback_add("selection,cut", func, *args, **kwargs)

    def callback_selection_cut_del(self, func):
        self._callback_del("selection,cut", func)

    def callback_cursor_changed_add(self, func, *args, **kwargs):
        self._callback_add("cursor,changed", func, *args, **kwargs)

    def callback_cursor_changed_del(self, func):
        self._callback_del("cursor,changed", func)

    def callback_anchor_clicked_add(self, func, *args, **kwargs):
        self._callback_add("anchor,clicked", func, *args, **kwargs)

    def callback_anchor_clicked_del(self, func):
        self._callback_del("anchor,clicked", func)

    def callback_activated_add(self, func, *args, **kwargs):
        self._callback_add("activated", func, *args, **kwargs)

    def callback_activated_del(self, func):
        self._callback_del("activated", func)

    def callback_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked", func, *args, **kwargs)

    def callback_clicked_del(self, func):
        self._callback_del("clicked", func)

    def callback_double_clicked_add(self, func, *args, **kwargs):
        self._callback_add("clicked,double", func, *args, **kwargs)

    def callback_double_clicked_del(self, func):
        self._callback_del("clicked,double", func)

    def callback_focused_add(self, func, *args, **kwargs):
        self._callback_add("focused", func, *args, **kwargs)

    def callback_focused_del(self, func):
        self._callback_del("focused", func)

    def callback_unfocused_add(self, func, *args, **kwargs):
        self._callback_add("unfocused", func, *args, **kwargs)

    def callback_unfocused_del(self, func):
        self._callback_del("unfocused", func)

    def single_line_set(self, single_line):
        elm_entry_single_line_set(self.obj, single_line)

    def password_set(self, password):
        elm_entry_password_set(self.obj, password)

    def entry_set(self, entry):
        elm_entry_entry_set(self.obj, entry)

    def entry_get(self):
        return elm_entry_entry_get(self.obj)

    def selection_get(self):
        cdef char* str
        str = elm_entry_selection_get(self.obj)
        if str == NULL:
            return ""
        return str

    def entry_insert(self, entry):
        elm_entry_entry_insert(self.obj, entry)

    def line_wrap_set(self, wrap):
        elm_entry_line_wrap_set(self.obj, wrap)

    def editable_set(self, editable):
        elm_entry_editable_set(self.obj, editable)

    def select_none(self):
        elm_entry_select_none(self.obj)

    def select_all(self):
        elm_entry_select_all(self.obj)

    def context_menu_disabled_set(self, disabled):
        elm_entry_context_menu_disabled_set(self.obj, disabled)

    markup_to_utf8 = staticmethod(Entry_markup_to_utf8)

    utf8_to_markup = staticmethod(Entry_utf8_to_markup)


_elm_widget_type_register("entry", Entry)
