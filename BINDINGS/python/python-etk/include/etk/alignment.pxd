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

cdef extern from "etk_alignment.h":
    ####################################################################
    # Signals

    ####################################################################
    # Enumerations
    ####################################################################
    # Structures
    ctypedef struct Etk_Alignment

    ####################################################################
    # Functions
    Etk_Type* etk_alignment_type_get()
    Etk_Widget* etk_alignment_new(float xalign, float yalign, float xscale, float yscale)
    void etk_alignment_get(Etk_Alignment* __self, float* xalign, float* yalign, float* xscale, float* yscale)
    void etk_alignment_set(Etk_Alignment* __self, float xalign, float yalign, float xscale, float yscale)

#########################################################################
# Objects
cdef public class Alignment(Bin) [object PyEtk_Alignment, type PyEtk_Alignment_Type]:
    pass

