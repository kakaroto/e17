# Copyright (C) 2007-2008 Caio Marcelo de Oliveira Filho
#
# This file is part of Python-Etk.
#
# Python-Etk is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Etk is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Etk.  If not, see <http://www.gnu.org/licenses/>.

cdef public class Canvas(Container) [object PyEtk_Canvas, type PyEtk_Canvas_Type]:
    def __init__(self, **kargs):
        if self.obj == NULL:
            self._set_obj(<Etk_Object*>etk_canvas_new())
        self._set_common_params(**kargs)

    def child_position_get(self, Widget widget):
        cdef int x
        cdef int y
        etk_canvas_child_position_get(<Etk_Canvas*>self.obj, <Etk_Widget*>widget.obj, &x, &y)
        return (x, y)

    def move(self, Widget widget, int x, int y):
        etk_canvas_move(<Etk_Canvas*>self.obj, <Etk_Widget*>widget.obj, x, y)

    def object_add(self, evas.c_evas.Object evas_object):
        __ret = Object_from_instance(<Etk_Object*>etk_canvas_object_add(<Etk_Canvas*>self.obj, <evas.c_evas.Evas_Object*>evas_object.obj))
        return (__ret)

    def put(self, Widget widget, int x, int y):
        etk_canvas_put(<Etk_Canvas*>self.obj, <Etk_Widget*>widget.obj, x, y)


class CanvasEnums:
    pass
