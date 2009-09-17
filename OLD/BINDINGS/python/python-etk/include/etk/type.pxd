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

cdef extern from "etk_type.h":
    ####################################################################
    # Structures
    ctypedef struct Etk_Type

    ####################################################################
    # Functions
    Etk_Type* etk_type_get_from_name(char* name)
    void etk_type_shutdown()
    Etk_Signal *etk_type_signal_get(Etk_Type *type, int signal_code)
    Etk_Signal *etk_type_signal_get_by_name(Etk_Type *type, char *signal_name)
