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

# TODO: Handle callback remove

cdef void _fs_callback(void *cbt, c_evas.Evas_Object *obj, void *event_info) with gil:
    try:
        fs, func, args, kargs = <object>cbt
        selected = None
        if event_info != NULL:
            selected = <char*>event_info
        func(fs, selected, *args, **kargs)
    except Exception, e:
        traceback.print_exc()

cdef class Fileselector(Object):
    cdef object cbts

    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_fileselector_add(parent.obj))
        self.cbts = []

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

    property selected:
        def __set__(self, value):
            self._fs_callback_add("selected", value)

    property done:
        def __set__(self, value):
            self._fs_callback_add("done", value)

    def _fs_callback_add(self, event, value):
        args = []
        kwargs = {}
        if type(value) == tuple:
            args = value[1]
            if type(args) != tuple:
                args = (args,)
            if len(value) == 3:
                kwargs = value[2]
            cb = value[0]
        else:
            cb = value
        if not callable(cb):
            raise TypeError("callback is not callable")
        cbt = (self, cb, args, kwargs)
        self.cbts.append(cbt)
        c_evas.evas_object_smart_callback_add(self.obj, event,
                                              _fs_callback, <void*>cbt)

