# Copyright 2012 Kai Huuhko <kai.huuhko@gmail.com>
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

cdef void _ctxpopup_callback(void *cbt, c_evas.Evas_Object *obj, void *event_info) with gil:
    try:
        (ctxpopup, callback, it, a, ka) = <object>cbt
        callback(ctxpopup, it, *a, **ka)
    except Exception, e:
        traceback.print_exc()

cdef class CtxpopupItem(ObjectItem):
    cdef object cbt

    def __del_cb(self):
        self.obj = NULL
        self.cbt = None
        Py_DECREF(self)

    def __init__(self, c_evas.Object ctxpopup, label, c_evas.Object icon = None, callback = None, *args, **kargs):
        cdef c_evas.Evas_Object* icon_obj
        cdef void* cbdata = NULL
        cdef void (*cb) (void *, c_evas.Evas_Object *, void *)
        icon_obj = NULL
        cb = NULL

        if icon is not None:
            icon_obj = icon.obj

        if callback:
            if not callable(callback):
                raise TypeError("callback is not callable")
            cb = _ctxpopup_callback

        self.cbt = (ctxpopup, callback, self, args, kargs)
        cbdata = <void*>self.cbt
        self.obj = elm_ctxpopup_item_append(ctxpopup.obj, label, icon_obj, cb, cbdata)

cdef class Ctxpopup(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_ctxpopup_add(parent.obj))

    def hover_parent_set(self, c_evas.Object parent):
        elm_ctxpopup_hover_parent_set(self.obj, parent.obj)

    def hover_parent_get(self):
        cdef Evas_Object *obj = elm_ctxpopup_hover_parent_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    def clear(self):
        elm_ctxpopup_clear(self.obj)

    def horizontal_set(self, horizontal):
        elm_ctxpopup_horizontal_set(self.obj, horizontal)

    def horizontal_get(self):
        return bool(elm_ctxpopup_horizontal_get(self.obj))

    property horizontal:
        def __get__(self):
            return self.horizontal_get()
        def __set__(self, horizontal):
            self.horizontal_set(horizontal)

    def item_append(self, label, c_evas.Object icon, func, *args, **kwargs):
        return CtxpopupItem(self, label, icon, func, *args, **kwargs)

    def direction_priority_set(self, first, second, third, fourth):
        elm_ctxpopup_direction_priority_set(self.obj, first, second, third, fourth)

    def direction_priority_get(self):
        cdef Elm_Ctxpopup_Direction first, second, third, fourth
        elm_ctxpopup_direction_priority_get(self.obj, &first, &second, &third, &fourth)
        return (first, second, third, fourth)

    def direction_get(self):
        return elm_ctxpopup_direction_get(self.obj)

    def dismiss(self):
        elm_ctxpopup_dismiss(self.obj)

    def callback_dismissed_add(self, func, *args, **kwargs):
        self._callback_add("dismissed", func, *args, **kwargs)

    def callback_dismissed_del(self, func):
        self._callback_del("dismissed", func)
