# Copyright (c) 2010-2010 ProFUSION embedded systems
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


cdef class ScrolledEntry(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_scrolled_entry_add(parent.obj))

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
        elm_scrolled_entry_single_line_set(self.obj, single_line)

    def single_line_get(self):
        return elm_scrolled_entry_single_line_get(self.obj)

    def password_set(self, password):
        elm_scrolled_entry_password_set(self.obj, password)

    def password_get(self):
        return elm_scrolled_entry_password_get(self.obj)

    def entry_set(self, entry):
        elm_scrolled_entry_entry_set(self.obj, entry)

    def entry_get(self):
        return elm_scrolled_entry_entry_get(self.obj)

    def selection_get(self):
        cdef char* str
        str = elm_scrolled_entry_selection_get(self.obj)
        if str == NULL:
            return ""
        return str

    def entry_insert(self, entry):
        elm_scrolled_entry_entry_insert(self.obj, entry)

    def line_wrap_set(self, wrap):
        elm_scrolled_entry_line_wrap_set(self.obj, wrap)

    def line_char_wrap_set(self, wrap):
        elm_scrolled_entry_line_char_wrap_set(self.obj, wrap)

    def editable_set(self, editable):
        elm_scrolled_entry_editable_set(self.obj, editable)

    def editable_get(self):
        return elm_scrolled_entry_editable_get(self.obj)

    def select_none(self):
        elm_scrolled_entry_select_none(self.obj)

    def select_all(self):
        elm_scrolled_entry_select_all(self.obj)

    def cursor_next(self):
        return elm_scrolled_entry_cursor_next(self.obj)

    def cursor_prev(self):
        return elm_scrolled_entry_cursor_prev(self.obj)

    def cursor_up(self):
        return elm_scrolled_entry_cursor_up(self.obj)

    def cursor_down(self):
        return elm_scrolled_entry_cursor_down(self.obj)

    def cursor_begin_set(self):
        elm_scrolled_entry_cursor_begin_set(self.obj)

    def cursor_end_set(self):
        elm_scrolled_entry_cursor_end_set(self.obj)

    def cursor_line_begin_set(self):
        elm_scrolled_entry_cursor_line_begin_set(self.obj)

    def cursor_line_end_set(self):
        elm_scrolled_entry_cursor_line_end_set(self.obj)

    def cursor_selection_begin(self):
        elm_scrolled_entry_cursor_selection_begin(self.obj)

    def cursor_selection_end(self):
        elm_scrolled_entry_cursor_selection_end(self.obj)

    def cursor_is_format_get(self):
        return elm_scrolled_entry_cursor_is_format_get(self.obj)

    def cursor_is_visible_format_get(self):
        return elm_scrolled_entry_cursor_is_visible_format_get(self.obj)

    def cursor_content_get(self):
        return elm_scrolled_entry_cursor_content_get(self.obj)

    def selection_cut(self):
        elm_scrolled_entry_selection_cut(self.obj)

    def selection_copy(self):
        elm_scrolled_entry_selection_copy(self.obj)

    def selection_paste(self):
        elm_scrolled_entry_selection_paste(self.obj)

    def context_menu_clear(self):
        elm_scrolled_entry_context_menu_clear(self.obj)
    
    #TODO: void         elm_scrolled_entry_context_menu_item_add(evas.c_evas.Evas_Object *obj, const char *label, const char *icon_file, Elm_Icon_Type icon_type, Evas_Smart_Cb func, const void *data)

    def context_menu_disabled_set(self, value):
        elm_scrolled_entry_context_menu_disabled_set(self.obj, value)

    def context_menu_disabled_get(self):
        return elm_scrolled_entry_context_menu_disabled_get(self.obj)

    def scrollbar_policy_set(self, policy_h, policy_v):
        elm_scrolled_entry_scrollbar_policy_set(self.obj, policy_h, policy_v)

    def bounce_set(self, h, v):
        elm_scrolled_entry_bounce_set(self.obj, h, v)

    markup_to_utf8 = staticmethod(Entry_markup_to_utf8)

    utf8_to_markup = staticmethod(Entry_utf8_to_markup)


_elm_widget_type_register("scrolled_entry", ScrolledEntry)
