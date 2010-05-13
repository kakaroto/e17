# Copyright (c) 2008-2009 ProFUSION embedded systems
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
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary. If not, see <http://www.gnu.org/licenses/>.

cdef class Notify(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_notify_add(parent.obj))

    def content_set(self, c_evas.Object content):
        cdef c_evas.Evas_Object *o
        if content is not None:
            o = content.obj
        else:
            o = NULL
        elm_notify_content_set(self.obj, o)

    def orient_set(self, int orient):
        elm_notify_orient_set(self.obj, orient)

    def timeout_set(self, int timeout):
        elm_notify_timeout_set(self.obj, timeout)

    def timer_init(self):
        elm_notify_timer_init(self.obj)

    def repeat_events_set(self, repeat):
        elm_notify_repeat_events_set(self.obj, repeat)

    def parent_set(self, c_evas.Object parent):
        cdef c_evas.Evas_Object *o
        if parent is not None:
            o = parent.obj
        else:
            o = NULL
        elm_notify_parent_set(self.obj, o)


_elm_widget_type_register("notify", Notify)
