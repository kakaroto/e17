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

cdef public class Flip(Object) [object PyElementaryFlip, type PyElementaryFlip_Type]:
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_flip_add(parent.obj))

    def front_visible_get(self):
        return elm_flip_front_visible_get(self.obj)

    property front_visible:
        def __get__(self):
            return self.front_visible_get()

    def perspective_set(self, foc, x, y):
        elm_flip_perspective_set(self.obj, foc, x, y)

    def go(self, flip_mode):
        elm_flip_go(self.obj, flip_mode)

    def interaction_set(self, mode):
        elm_flip_interaction_set(self.obj, mode)

    def interaction_get(self):
        return elm_flip_interaction_get(self.obj)

    property interaction:
        def __get__(self):
            return self.interactione_get()

        def __set__(self, value):
            self.interaction_set(value)

    def interaction_direction_enabled_set(self, direction, enable):
        elm_flip_interaction_direction_enabled_set(self.obj, direction, enable)

    def interaction_direction_enabled_get(self, direction):
        return elm_flip_interaction_direction_enabled_get(self.obj, direction)

    def interaction_direction_hitsize_set(self, direction, hitsize):
        elm_flip_interaction_direction_hitsize_set(self.obj, direction, hitsize)

    def interaction_direction_hitsize_get(self, direction):
        return elm_flip_interaction_direction_hitsize_get(self.obj, direction)

    def content_front_set(self, c_evas.Object content):
        elm_object_part_content_set(self.obj, "front", content.obj)

    def content_back_set(self, c_evas.Object content):
        elm_object_part_content_set(self.obj, "back", content.obj)

    def callback_animate_begin_add(self, func, *args, **kwargs):
        self._callback_add("animate,begin", func, *args, **kwargs)

    def callback_animate_begin_del(self, func):
        self._callback_del("animate,begin", func)

    def callback_animate_done_add(self, func, *args, **kwargs):
        self._callback_add("animate,done", func, *args, **kwargs)

    def callback_animate_done_del(self, func):
        self._callback_del("animate,done", func)

_elm_widget_type_register("flip", Flip)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryFlip_Type # hack to install metaclass
_install_metaclass(&PyElementaryFlip_Type, ElementaryObjectMeta)
