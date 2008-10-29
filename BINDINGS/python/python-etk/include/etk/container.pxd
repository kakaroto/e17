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

cdef extern from "etk_container.h":
    ####################################################################
    # Signals
    int ETK_CONTAINER_CHILD_ADDED_SIGNAL
    int ETK_CONTAINER_CHILD_REMOVED_SIGNAL

    ####################################################################
    # Enumerations
    ####################################################################
    # Structures
    ctypedef struct Etk_Container:
        void (*child_add)(Etk_Container *container, Etk_Widget *widget)
        void (*child_remove)(Etk_Container *container, Etk_Widget *widget)
        Eina_List *(*children_get)(Etk_Container *container)


    ####################################################################
    # Functions
    void etk_container_child_space_fill(Etk_Widget* child, Etk_Geometry* child_space, int hfill, int vfill, float xalign, float yalign)
    void etk_container_remove(Etk_Widget* widget)
    Etk_Type* etk_container_type_get()
    void etk_container_add(Etk_Container* __self, Etk_Widget* widget)
    int etk_container_border_width_get(Etk_Container* __self)
    void etk_container_border_width_set(Etk_Container* __self, int border_width)
    evas.c_evas.Eina_List* etk_container_children_get(Etk_Container* __self)
    void etk_container_for_each_data(Etk_Container* __self, void (*for_each_cb)(Etk_Widget *child, void *data), void *data)
    int etk_container_is_child(Etk_Container* __self, Etk_Widget* widget)
    void etk_container_remove_all(Etk_Container* __self)

#########################################################################
# Objects
cdef public class Container(Widget) [object PyEtk_Container, type PyEtk_Container_Type]:
    pass

