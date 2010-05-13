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
    cdef char *str = <char *>addr
    if str == NULL:
        return None
    else:
        return str

cdef class Fileselector(Object):
    cdef object _cbs

    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_fileselector_add(parent.obj))
        self._cbs = {}

    def selected_get(self):
        cdef char* path
        path = elm_fileselector_selected_get(self.obj)
        return path

    def path_set(self, path):
        elm_fileselector_path_set(self.obj, path)

    def path_get(self):
        cdef char *p
        p = elm_fileselector_path_get(self.obj)
        return p

    def expandable_set(self, expand):
        elm_fileselector_expandable_set(self.obj, expand)

    def is_save_get(self):
        cdef unsigned char r
        r = elm_fileselector_is_save_get(self.obj)
        if r == 0:
            return False
        return True

    def is_save_set(self, is_save):
        elm_fileselector_is_save_set(self.obj, is_save)

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


_elm_widget_type_register("fileselector", Fileselector)
