# Copyright (C) 2007-2009 Gustavo Sverzut Barbieri, Ulisses Furquim, Ivan Briano
#
# This file is part of Python-Edje.
#
# Python-Edje is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Edje is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Edje.  If not, see <http://www.gnu.org/licenses/>.

# This file is included verbatim by edje.c_edje.pyx

cdef class ExternalParam:
    property name:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            return self.obj.name

    property type:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            return self.obj.type

    property i:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            return self.obj.i

    property d:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            return self.obj.d

    property s:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            if self.obj.s != NULL:
                return self.obj.s

    property value:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            if self.obj.type == EDJE_EXTERNAL_PARAM_TYPE_INT:
                return self.obj.i
            elif self.obj.type == EDJE_EXTERNAL_PARAM_TYPE_DOUBLE:
                return self.obj.d
            elif self.obj.type == EDJE_EXTERNAL_PARAM_TYPE_STRING:
                if self.obj.s != NULL:
                    return self.obj.s

cdef ExternalParam ExternalParam_from_ptr(Edje_External_Param *param):
    cdef ExternalParam p
    p = ExternalParam()
    p.obj = param
    return p

# XXX: this should be C-only, but it would require edje_edit
# XXX: being able to use it.
def _ExternalParam_from_ptr(long ptr):
    return ExternalParam_from_ptr(<Edje_External_Param *>ptr)
