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

cdef class Box(Object):
    def __init__(self, c_evas.Object parent):
        Object.__init__(self, parent.evas)
        self._set_obj(elm_box_add(parent.obj))

    def horizontal_set(self, horizontal):
        elm_box_horizontal_set(self.obj, horizontal)

    def horizontal_get(self):
        return elm_box_horizontal_get(self.obj)

    property horizontal:
        def __get__(self):
            return self.horizontal_get()

        def __set__(self, value):
            self.horizontal_set(value)

    def homogeneous_set(self, homogeneous):
        elm_box_homogeneous_set(self.obj, homogeneous)

    def homogeneous_get(self):
        return elm_box_homogeneous_get(self.obj)

    property homogeneous:
        def __get__(self):
            return self.homogeneous_get()

        def __set__(self, value):
            self.homogeneous_set(value)

    def pack_start(self, c_evas.Object obj):
        elm_box_pack_start(self.obj, obj.obj)

    def pack_end(self, c_evas.Object obj):
        elm_box_pack_end(self.obj, obj.obj)

    def pack_before(self, c_evas.Object obj, c_evas.Object before):
        elm_box_pack_before(self.obj, obj.obj, before.obj)

    def pack_after(self, c_evas.Object obj, c_evas.Object after):
        elm_box_pack_after(self.obj, obj.obj, after.obj)

    def clear(self):
        elm_box_clear(self.obj)

    def unpack(self, c_evas.Object obj):
        elm_box_unpack(self.obj, obj.obj)

    def unpack_all(self):
        elm_box_unpack_all(self.obj)

    def children_get(self):
        cdef c_evas.Evas_Object *o
        cdef Object obj
        cdef evas.c_evas.const_Eina_List *lst
        
        ret = []
        lst = elm_box_children_get(self.obj)
        while lst:
            o = <c_evas.Evas_Object *> lst.data
            obj = <Object>c_evas.evas_object_data_get(o, "python-evas")
            ret.append(obj)
            lst = lst.next
        return ret

    property children:
        def __get__(self):
            return self.children_get()

    def padding_set(self, horizontal, vertical):
        elm_box_padding_set(self.obj, horizontal, vertical)

    def padding_get(self):
        cdef int horizontal, vertical

        elm_box_padding_get(self.obj, &horizontal, &vertical)
        return (horizontal, vertical)

    property padding:
        def __get__(self):
            return self.padding_get()

        def __set__(self, value):
            self.padding_set(*value)

    def align_set(self, horizontal, vertical):
        elm_box_align_set(self.obj, horizontal, vertical)

    def align_get(self):
        cdef double horizontal, vertical

        elm_box_align_get(self.obj, &horizontal, &vertical)
        return (horizontal, vertical)

    property align:
        def __get__(self):
            return self.align_get()

        def __set__(self, value):
            self.align_set(*value)

    def recalculate(self):
        elm_box_recalculate(self.obj)

    # XXX TODO elm_box_layout_*
    
    # XXX TODO elm_box_transition_*


_elm_widget_type_register("box", Box)
