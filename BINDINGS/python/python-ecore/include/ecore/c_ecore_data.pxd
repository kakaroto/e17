# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Ecore.
#
# Python-Ecore is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Ecore is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Ecore.  If not, see <http://www.gnu.org/licenses/>.

cdef extern from "Ecore_Data.h":

    ctypedef struct Ecore_List

    # Retrieve the current position in the list
    void *ecore_list_current(Ecore_List * list)
    void *ecore_list_first(Ecore_List * list)
    void *ecore_list_last(Ecore_List * list)
    int   ecore_list_index(Ecore_List * list)
    int   ecore_list_count(Ecore_List * list)

    # Traversing the list
    void *ecore_list_first_goto(Ecore_List * list)
    void *ecore_list_last_goto(Ecore_List * list)
    void *ecore_list_index_goto(Ecore_List * list, int index)
    void *ecore_list_goto(Ecore_List * list, void *_data)

    # Traversing the list and returning data
    void *ecore_list_next(Ecore_List * list)

    # Free the list and it's contents
    void ecore_list_destroy(Ecore_List *lst)
