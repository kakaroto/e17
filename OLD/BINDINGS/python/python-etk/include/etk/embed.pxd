# Copyright (C) 2007-2008 Youness Alaoui
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

cdef extern from "etk_embed.h":
    ####################################################################
    # Enumerations
    ####################################################################
    # Structures
    ctypedef struct Etk_Embed
    ctypedef void  (*pointer_set_cb)(void *pointer_data, Etk_Pointer_Type pointer_type)
    ctypedef void  (*position_get_cb)(void *position_data, int *x, int *y)

    ####################################################################
    # Functions
    Etk_Type* etk_embed_type_get()
    Etk_Widget* etk_embed_new(evas.c_evas.Evas* evas)
    evas.c_evas.Evas_Object* etk_embed_object_get(Etk_Embed* __self)
    void etk_embed_pointer_method_set(Etk_Embed* __self, pointer_set_cb func, void *data)
    void etk_embed_position_method_set(Etk_Embed* __self, position_get_cb func, void *data)

#########################################################################
# Objects
cdef public class Embed(Toplevel) [object PyEtk_Embed, type PyEtk_Embed_Type]:
    pass

