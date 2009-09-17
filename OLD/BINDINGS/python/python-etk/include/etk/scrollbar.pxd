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

cdef extern from "etk_scrollbar.h":
    ####################################################################
    # Signals

    ####################################################################
    # Enumerations
    ####################################################################
    # Structures
    ctypedef struct Etk_VScrollbar
    ctypedef struct Etk_HScrollbar
    ctypedef struct Etk_Scrollbar

    ####################################################################
    # Functions
    Etk_Type* etk_hscrollbar_type_get()
    Etk_Type* etk_scrollbar_type_get()
    Etk_Type* etk_vscrollbar_type_get()

    Etk_Widget *etk_hscrollbar_new(double lower, double upper, double value,
                                   double step_increment, double page_increment,
                                   double page_size)
    Etk_Widget *etk_vscrollbar_new(double lower, double upper, double value,
                                   double step_increment, double page_increment,
                                   double page_size)


#########################################################################
# Objects
cdef public class Scrollbar(Range) [object PyEtk_Scrollbar, type PyEtk_Scrollbar_Type]:
    pass
cdef public class VScrollbar(Scrollbar) [object PyEtk_VScrollbar, type PyEtk_VScrollbar_Type]:
    pass
cdef public class HScrollbar(Scrollbar) [object PyEtk_HScrollbar, type PyEtk_HScrollbar_Type]:
    pass
