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

cdef class Layout(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_layout_add(parent.obj))

    def file_set(self, filename, group):
        elm_layout_file_set(self.obj, filename, group)

    def theme_set(self, clas, group, style):
        elm_layout_theme_set(self.obj, clas, group, style)

    def content_set(self, swallow, c_evas.Object content):
        cdef c_evas.Evas_Object *o
        if content is not None:
            o = content.obj
        else:
            o = NULL
        elm_layout_content_set(self.obj, swallow, o)

    def edje_get(self):
        cdef c_evas.Evas_Object *obj = elm_layout_edje_get(self.obj)
        return evas.c_evas._Object_from_instance(<long> obj)

    def sizing_eval(self):
        elm_layout_sizing_eval(self.obj)


_elm_widget_type_register("layout", Layout)
