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

def _fs_callback_conv(long addr):
    cdef const_char_ptr s = <const_char_ptr>addr
    if s == NULL:
        return None
    else:
        return s

cdef public class Fileselector(Object) [object PyElementaryFileselector, type PyElementaryFileselector_Type]:
    cdef object _cbs

    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_fileselector_add(parent.obj))
        self._cbs = {}

    def is_save_get(self):
        return elm_fileselector_is_save_get(self.obj)

    def is_save_set(self, is_save):
        elm_fileselector_is_save_set(self.obj, is_save)

    property is_save:
        def __get__(self):
            return self.is_save_get()

        def __set__(self, value):
            self.is_save_set(value)

    def folder_only_set(self, folder_only):
        elm_fileselector_folder_only_set(self.obj, folder_only)

    def folder_only_get(self):
        return elm_fileselector_folder_only_get(self.obj)

    property folder_only:
        def __get__(self):
            return self.folder_only_get()

        def __set__(self, value):
            self.folder_only_set(value)

    def buttons_ok_cancel_set(self, buttons):
        elm_fileselector_buttons_ok_cancel_set(self.obj, buttons)

    def buttons_ok_cancel_get(self):
        return elm_fileselector_buttons_ok_cancel_get(self.obj)

    property buttons_ok_cancel:
        def __get__(self):
            return self.buttons_ok_cancel_get()

        def __set__(self, value):
            self.buttons_ok_cancel_set(value)

    def expandable_set(self, expand):
        elm_fileselector_expandable_set(self.obj, expand)

    def expandable_get(self):
        return elm_fileselector_expandable_get(self.obj)

    property expandable:
        def __get__(self):
            return self.expandable_get()

        def __set__(self, value):
            self.expandable_set(value)

    def path_set(self, path):
        elm_fileselector_path_set(self.obj, path)

    def path_get(self):
        cdef const_char_ptr p
        p = elm_fileselector_path_get(self.obj)
        return p if p != NULL else None

    property path:
        def __get__(self):
            return self.path_get()

        def __set__(self, value):
            self.path_set(value)

    def selected_set(self, path):
        return elm_fileselector_selected_set(self.obj, path)

    def selected_get(self):
        cdef const_char_ptr p
        p = elm_fileselector_selected_get(self.obj)
        return p if p != NULL else None

    property selected:
        def __get__(self):
            return self.selected_get()

        def __set__(self, value):
            self.selected_set(value)

    def mode_set(self, mode):
        elm_fileselector_mode_set(self.obj, mode)

    def mode_get(self):
        return elm_fileselector_mode_get(self.obj)

    property mode:
        def __get__(self):
            return self.mode_get()

        def __set__(self, value):
            self.mode_set(value)

    def callback_selected_add(self, func, *args, **kwargs):
        self._callback_add_full("selected", _fs_callback_conv,
                                func, *args, **kwargs)

    def callback_selected_del(self, func):
        self._callback_del_full("selected", _fs_callback_conv, func)

    def callback_directory_open_add(self, func, *args, **kwargs):
        self._callback_add_full("directory,open", _fs_callback_conv,
                                func, *args, **kwargs)

    def callback_directory_open_del(self, func):
        self._callback_del_full("directory,open", _fs_callback_conv, func)

    def callback_done_add(self, func, *args, **kwargs):
        self._callback_add_full("done", _fs_callback_conv,
                                func, *args, **kwargs)

    def callback_done_del(self, func):
        self._callback_del_full("done", _fs_callback_conv, func)

_elm_widget_type_register("fileselector", Fileselector)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryFileselector_Type # hack to install metaclass
_install_metaclass(&PyElementaryFileselector_Type, ElementaryObjectMeta)
