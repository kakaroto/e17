# Copyright (C) 2007-2008 Caio Marcelo de Oliveira Filho, Gustavo Sverzut Barbieri
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

cdef extern from "etk_toplevel.h":
    ####################################################################
    # Enumerations
    ctypedef enum Etk_Pointer_Type:
        ETK_POINTER_NONE
        ETK_POINTER_DEFAULT
        ETK_POINTER_MOVE
        ETK_POINTER_H_DOUBLE_ARROW
        ETK_POINTER_V_DOUBLE_ARROW
        ETK_POINTER_RESIZE
        ETK_POINTER_RESIZE_TL
        ETK_POINTER_RESIZE_T
        ETK_POINTER_RESIZE_TR
        ETK_POINTER_RESIZE_R
        ETK_POINTER_RESIZE_BR
        ETK_POINTER_RESIZE_B
        ETK_POINTER_RESIZE_BL
        ETK_POINTER_RESIZE_L
        ETK_POINTER_TEXT_EDIT
        ETK_POINTER_DND_DROP

    ####################################################################
    # Structures
    ctypedef struct Etk_Toplevel:
        void (*evas_position_get)(Etk_Toplevel *toplevel, int *x, int *y)
        void (*screen_position_get)(Etk_Toplevel *toplevel, int *x, int *y)
        void (*size_get)(Etk_Toplevel *toplevel, int *w, int *h)
        void (*pointer_set)(Etk_Toplevel *toplevel, Etk_Pointer_Type pointer_type)

    ####################################################################
    # Functions
    Etk_Type* etk_toplevel_type_get()
    evas.c_evas.Evas_List* etk_toplevel_widgets_get()
    evas.c_evas.Evas* etk_toplevel_evas_get(Etk_Toplevel* __self)
    void etk_toplevel_evas_position_get(Etk_Toplevel* __self, int* x, int* y)
    Etk_Widget* etk_toplevel_focused_widget_get(Etk_Toplevel* __self)
    Etk_Widget* etk_toplevel_focused_widget_next_get(Etk_Toplevel* __self)
    Etk_Widget* etk_toplevel_focused_widget_prev_get(Etk_Toplevel* __self)
    void etk_toplevel_focused_widget_set(Etk_Toplevel* __self, Etk_Widget* widget)
    void etk_toplevel_pointer_pop(Etk_Toplevel* __self, int pointer_type)
    void etk_toplevel_pointer_push(Etk_Toplevel* __self, int pointer_type)
    void etk_toplevel_screen_position_get(Etk_Toplevel* __self, int* x, int* y)
    void etk_toplevel_size_get(Etk_Toplevel* __self, int* w, int* h)

#########################################################################
# Objects
cdef public class Toplevel(Bin) [object PyEtk_Toplevel, type PyEtk_Toplevel_Type]:
    pass

