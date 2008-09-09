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

cdef extern from "etk_range.h":
    ####################################################################
    # Signals
    int ETK_RANGE_VALUE_CHANGED_SIGNAL

    ####################################################################
    # Enumerations
    ####################################################################
    # Structures
    ctypedef struct Etk_Range

    ####################################################################
    # Functions
    Etk_Type* etk_range_type_get()
    void etk_range_increments_get(Etk_Range* __self, double* step, double* page)
    void etk_range_increments_set(Etk_Range* __self, double step, double page)
    double etk_range_page_size_get(Etk_Range* __self)
    void etk_range_page_size_set(Etk_Range* __self, double page_size)
    void etk_range_range_get(Etk_Range* __self, double* lower, double* upper)
    void etk_range_range_set(Etk_Range* __self, double lower, double upper)
    double etk_range_value_get(Etk_Range* __self)
    int etk_range_value_set(Etk_Range* __self, double value)

#########################################################################
# Objects
cdef public class Range(Widget) [object PyEtk_Range, type PyEtk_Range_Type]:
    pass

