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

cdef public class Layout(Object) [object PyElementaryLayout, type PyElementaryLayout_Type]:
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_layout_add(parent.obj))

    def file_set(self, filename, group):
        return bool(elm_layout_file_set(self.obj, filename, group))

    def theme_set(self, clas, group, style):
        return bool(elm_layout_theme_set(self.obj, clas, group, style))

    def signal_emit(self, emission, source):
        elm_layout_signal_emit(self.obj, emission, source)

    #def signal_callback_add(self, emission, source, func, data):
        #elm_layout_signal_callback_add(self.obj, emission, source, Edje_Signal_Cb func, void *data)

    #def signal_callback_del(self, emission, source, func):
        #elm_layout_signal_callback_del(self.obj, emission, source, Edje_Signal_Cb func)

    def box_append(self, part, c_evas.Object child):
        return bool(elm_layout_box_append(self.obj, part, child.obj))

    def box_prepend(self, part, c_evas.Object child):
        return bool(elm_layout_box_prepend(self.obj, part, child.obj))

    def box_insert_before(self, part, c_evas.Object child, c_evas.Object reference):
        return bool(elm_layout_box_insert_before(self.obj, part, child.obj, reference.obj))

    def box_insert_at(self, part, c_evas.Object child, pos):
        return bool(elm_layout_box_insert_at(self.obj, part, child.obj, pos))

    def box_remove(self, part, c_evas.Object child):
        cdef c_evas.Evas_Object *obj = elm_layout_box_remove(self.obj, part, child.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    def box_remove_all(self, part, clear):
        return bool(elm_layout_box_remove_all(self.obj, part, clear))

    def table_pack(self, part, c_evas.Object child_obj, col, row, colspan, rowspan):
        return bool(elm_layout_table_pack(self.obj, part, child_obj.obj, col, row, colspan, rowspan))

    def table_unpack(self, part, c_evas.Object child_obj):
        cdef c_evas.Evas_Object *obj = elm_layout_table_unpack(self.obj, part, child_obj.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    def table_clear(self, part, clear):
        return bool(elm_layout_table_clear(self.obj, part, clear))

    def edje_get(self):
        cdef c_evas.Evas_Object *obj = elm_layout_edje_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    property edje:
        def __get__(self):
            return self.edje_get()

    def data_get(self, key):
        return elm_layout_data_get(self.obj, key)

    def sizing_eval(self):
        elm_layout_sizing_eval(self.obj)

    def part_cursor_set(self, part_name, cursor):
        return bool(elm_layout_part_cursor_set(self.obj, part_name, cursor))

    def part_cursor_get(self, part_name):
        return elm_layout_part_cursor_get(self.obj, part_name)

    def part_cursor_unset(self, part_name):
        return bool(elm_layout_part_cursor_unset(self.obj, part_name))

    def part_cursor_style_set(self, part_name, style):
        return bool(elm_layout_part_cursor_style_set(self.obj, part_name, style))

    def part_cursor_style_get(self, part_name):
        return elm_layout_part_cursor_style_get(self.obj, part_name)

    def part_cursor_engine_only_set(self, part_name, engine_only):
        return bool(elm_layout_part_cursor_engine_only_set(self.obj, part_name, engine_only))

    def part_cursor_engine_only_get(self, part_name):
        return bool(elm_layout_part_cursor_engine_only_get(self.obj, part_name))

    def content_set(self, swallow, c_evas.Object content):
        cdef c_evas.Evas_Object *o
        if content is not None:
            o = content.obj
        else:
            o = NULL
        elm_object_part_content_set(self.obj, swallow, o)

    def content_get(self, swallow):
        cdef c_evas.const_Evas_Object *obj = elm_object_part_content_get(self.obj, swallow)
        return evas.c_evas._Object_from_instance(<long> obj)

    def content_unset(self, swallow):
        cdef c_evas.Evas_Object *obj = elm_object_part_content_unset(self.obj, swallow)
        return evas.c_evas._Object_from_instance(<long> obj)

    def text_set(self, part, text):
        elm_layout_text_set(self.obj, part, text)

    def text_get(self, part):
        return elm_layout_text_get(self.obj, part)

    def icon_set(self, c_evas.Object icon):
        elm_layout_icon_set(self.obj, icon.obj if icon else NULL)

    def icon_get(self):
        cdef c_evas.const_Evas_Object *obj = elm_layout_icon_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    property icon:
        def __get__(self):
            return self.icon_get()
        def __set__(self, value):
            self.icon_set(value)

    def end_set(self, c_evas.Object end):
        elm_layout_end_set(self.obj, end.obj if end else NULL)

    def end_get(self):
        cdef c_evas.const_Evas_Object *obj = elm_layout_end_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    property end:
        def __get__(self):
            return self.end_get()
        def __set__(self, value):
            self.end_set(value)

    def callback_theme_changed_add(self, func, *args, **kwargs):
        self._callback_add("theme,changed", func, *args, **kwargs)

    def callback_theme_changed_del(self, func):
        self._callback_del("theme,changed", func)

_elm_widget_type_register("layout", Layout)

cdef extern from "Elementary.h": # hack to force type to be known
    cdef PyTypeObject PyElementaryLayout_Type # hack to install metaclass
_install_metaclass(&PyElementaryLayout_Type, ElementaryObjectMeta)
