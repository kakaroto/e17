# Copyright (c) 2008-2009 Simon Busch
# Copyright (c) 2010-2010 ProFUSION embedded systems
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

cdef class Naviframe(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_naviframe_add(parent.obj))

    def item_simple_push(self, c_evas.Object content):
        elm_naviframe_item_simple_push(self.obj, content.obj);

    def item_pop(self):
        elm_naviframe_item_pop(self.obj)

    def item_simple_promote(self, c_evas.Object content):
        elm_naviframe_item_simple_promote(self.obj, content.obj)

    def bottom_item_get(self):
        cdef c_evas.Evas_Object* o
        cdef Object obj
        cdef Elm_Object_Item *it

        it = elm_naviframe_bottom_item_get(self.obj)
        o = elm_object_item_part_content_get(it, NULL)
        if o == NULL:
            return None
        obj = <Object>c_evas.evas_object_data_get(o, "python-evas")

        return obj

    def top_item_get(self):
        cdef c_evas.Evas_Object* o
        cdef Object obj
        cdef Elm_Object_Item *it

        it = elm_naviframe_top_item_get(self.obj)
        o = elm_object_item_part_content_get(it, NULL)
        if o == NULL:
            return None
        obj = <Object>c_evas.evas_object_data_get(o, "python-evas")

        return obj


_elm_widget_type_register("naviframe", Naviframe)
