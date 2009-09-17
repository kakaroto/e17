# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
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

cdef extern from "etk_label.h":
    ####################################################################
    # Enumerations
    ####################################################################
    # Structures
    ctypedef struct Etk_Label

    ####################################################################
    # Functions
    Etk_Type* etk_label_type_get()
    Etk_Widget* etk_label_new(char* text)
    void etk_label_alignment_get(Etk_Label* __self, float* xalign, float* yalign)
    void etk_label_alignment_set(Etk_Label* __self, float xalign, float yalign)
    char* etk_label_get(Etk_Label* __self)
    void etk_label_set(Etk_Label* __self, char* text)

#########################################################################
# Objects
cdef public class Label(Widget) [object PyEtk_Label, type PyEtk_Label_Type]:
    pass

