# Copyright (c) 2011 Fabiano Fidêncio
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

def _fs_callback_conv(long addr):
    cdef const_char_ptr s = <const_char_ptr>addr
    if s == NULL:
        return None
    else:
        return s

cdef class FileselectorEntry(Object):
    cdef object _cbs

    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_fileselector_entry_add(parent.obj))
        self._cbs = {}

    def selected_get(self):
        cdef const_char_ptr p
        p = elm_fileselector_entry_selected_get(self.obj)
        if p == NULL:
            return None
        return p

    def button_icon_set(self, c_evas.Object icon):
        elm_fileselector_entry_button_icon_set(self.obj, icon.obj)

    def button_icon_get(self):
        cdef c_evas.Evas_Object *icon
        icon = elm_fileselector_entry_button_icon_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> icon)

    def button_icon_unset(self):
        cdef c_evas.Evas_Object *icon
        icon = elm_fileselector_entry_button_icon_unset(self.obj)
        return evas.c_evas._Object_from_instance(<long> icon)

    def window_title_set(self, title):
        elm_fileselector_entry_window_title_set(self.obj, title)

    def window_title_get(self):
        cdef const_char_ptr p
        p = elm_fileselector_entry_window_title_get(self.obj)
        if p == NULL:
            return None
        return p

    def window_size_set(self, width, height):
        elm_fileselector_entry_window_size_set(self.obj, width, height)

    def window_size_get(self):
        cdef c_evas.Evas_Coord w
        cdef c_evas.Evas_Coord h
        elm_fileselector_entry_window_size_get(self.obj, &w, &h)
        return (w, h)

    def folder_only_set(self, folder_only):
        elm_fileselector_entry_folder_only_set(self.obj, folder_only)

    def folder_only_get(self):
        cdef unsigned char r
        r = elm_fileselector_entry_folder_only_get(self.obj)
        if r == 0:
            return False
        return True

    def inwin_mode_set(self, inwin_mode):
        elm_fileselector_entry_inwin_mode_set(self.obj, inwin_mode)

    def inwin_mode_get(self):
        cdef unsigned char r
        r = elm_fileselector_entry_inwin_mode_get(self.obj)
        if r == 0:
            return False
        return True

    def path_set(self, path):
        elm_fileselector_entry_path_set(self.obj, path)

    def path_get(self):
        cdef const_char_ptr p
        p = elm_fileselector_entry_path_get(self.obj)
        if p == NULL:
            return None
        return p

    def expandable_set(self, expand):
        elm_fileselector_entry_expandable_set(self.obj, expand)

    def expandable_get(self):
        cdef unsigned char r
        r = elm_fileselector_entry_expandable_get(self.obj)
        if r == 0:
            return False
        return True

    def is_save_get(self):
        cdef unsigned char r
        r = elm_fileselector_entry_is_save_get(self.obj)
        if r == 0:
            return False
        return True

    def is_save_set(self, is_save):
        elm_fileselector_entry_is_save_set(self.obj, is_save)

    def callback_selected_add(self, func, *args, **kwargs):
        self._callback_add_full("selected", _fs_callback_conv,
                                func, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del_full("selected", _fs_callback_conv, func)

    def callback_done_add(self, func, *args, **kwargs):
        self._callback_add_full("done", _fs_callback_conv,
                                func, *args, **kwargs)

    def callback_done_del(self, func):
        self._callback_del_full("done", _fs_callback_conv, func)


_elm_widget_type_register("fileselector_entry", FileselectorEntry)
