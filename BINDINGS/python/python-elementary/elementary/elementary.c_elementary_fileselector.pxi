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

cdef void _fs_callback(void *cbtl, c_evas.Evas_Object *obj, void *event_info) with gil:
    l = <object>cbtl
    for cbt in l:
        try:
            fs, func, args, kwargs = cbt
            selected = None
            if event_info != NULL:
                selected = <char*>event_info
            func(fs, selected, *args, **kwargs)
        except Exception, e:
            traceback.print_exc()

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

    def _fs_callback_add(self, event, func, *args, **kwargs):
        cdef object cbt

        if not callable(func):
            raise TypeError("callback is not callable")

        cbt = (self, func, args, kwargs)

        if self._cbs.has_key(event):
            self._cbs[event].append(cbt)
        else:
            self._cbs[event] = [cbt]
            # register callback
            c_evas.evas_object_smart_callback_add(self.obj, event,
                                                  _fs_callback,
                                                  <void *>self._cbs[event])

    def _fs_callback_remove(self, event, func = None, *args, **kwargs):
        if self._cbs and self._cbs.has_key(event):
            if func is None:
                c_evas.evas_object_smart_callback_del(self.obj, event,
                                                      _fs_callback)
                self._cbs[event] = None
            else:
                for i, cbt in enumerate(self._cbs[event]):
                    if cbt is not None and (self, func, args, kwargs) == <object>cbt:
                        self._cbs[event][i] = None
                if not self._cbs[event]:
                     c_evas.evas_object_smart_callback_del(self.obj, event,
                                                           _fs_callback)

    def callback_selected_add(self, func, *args, **kwargs):
        self._fs_callback_add("selected", func, *args, **kwargs)

    def callback_selected_remove(self, func = None, *args, **kwargs):
        self._fs_callback_remove("selected", func, *args, **kwargs)

    def callback_done_add(self, func, *args, **kwargs):
        self._fs_callback_add("done", func, *args, **kwargs)

    def callback_done_remove(self, func = None, *args, **kwargs):
        self._fs_callback_remove("done", func, *args, **kwargs)

