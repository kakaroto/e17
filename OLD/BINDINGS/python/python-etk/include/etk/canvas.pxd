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

cdef extern from "etk_canvas.h":
    ####################################################################
    # Signals

    ####################################################################
    # Enumerations
    ####################################################################
    # Structures
    ctypedef struct Etk_Canvas

    ####################################################################
    # Functions
    Etk_Type* etk_canvas_type_get()
    Etk_Widget* etk_canvas_new()
    void etk_canvas_child_position_get(Etk_Canvas* __self, Etk_Widget* widget, int* x, int* y)
    void etk_canvas_move(Etk_Canvas* __self, Etk_Widget* widget, int x, int y)
    Etk_Widget* etk_canvas_object_add(Etk_Canvas* __self, evas.c_evas.Evas_Object* evas_object)
    void etk_canvas_put(Etk_Canvas* __self, Etk_Widget* widget, int x, int y)

#########################################################################
# Objects
cdef public class Canvas(Container) [object PyEtk_Canvas, type PyEtk_Canvas_Type]:
    pass

