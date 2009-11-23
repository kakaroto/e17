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


cdef class ExternalParamInfo:
    property name:
        def __get__(self):
            return self.obj.name

    property type:
        def __get__(self):
            return self.obj.type

cdef class ExternalParamInfoInt(ExternalParamInfo):
    property min:
        def __get__(self):
            if self.obj.info.i.min == EDJE_EXTERNAL_INT_UNSET:
                return None
            return self.obj.info.i.min

    property max:
        def __get__(self):
            if self.obj.info.i.max == EDJE_EXTERNAL_INT_UNSET:
                return None
            return self.obj.info.i.max

    property step:
        def __get__(self):
            if self.obj.info.i.step == EDJE_EXTERNAL_INT_UNSET:
                return None
            return self.obj.info.i.step

cdef class ExternalParamInfoDouble(ExternalParamInfo):
    property min:
        def __get__(self):
            if isnan(self.obj.info.d.min):
                return None
            return self.obj.info.d.min

    property max:
        def __get__(self):
            if isnan(self.obj.info.d.max):
                return None
            return self.obj.info.d.max

    property step:
        def __get__(self):
            if isnan(self.obj.info.d.step):
                return None
            return self.obj.info.d.step

cdef class ExternalParamInfoString(ExternalParamInfo):
    property accept_format:
        def __get__(self):
            if self.obj.info.s.accept_fmt == NULL:
                return None
            return self.obj.info.s.accept_fmt

    property deny_format:
        def __get__(self):
            if self.obj.info.s.deny_fmt == NULL:
                return None
            return self.obj.info.s.deny_fmt

cdef ExternalParamInfo ExternalParamInfo_from_ptr(Edje_External_Param_Info *ptr):
    cdef ExternalParamInfo p
    if ptr.type == EDJE_EXTERNAL_PARAM_TYPE_INT:
        p = ExternalParamInfoInt()
    elif ptr.type == EDJE_EXTERNAL_PARAM_TYPE_DOUBLE:
        p = ExternalParamInfoDouble()
    elif ptr.type == EDJE_EXTERNAL_PARAM_TYPE_STRING:
        p = ExternalParamInfoString()
    else:
        return None
    p.obj = ptr
    return p

def external_param_info_get(char *type_name):
    cdef Edje_External_Param_Info *params
    cdef int i

    params = edje_external_param_info_get(type_name)
    if params == NULL:
        return None

    ret = []
    i = 0
    while params[i].name != NULL:
        ret.append(ExternalParamInfo_from_ptr(&params[i]))
        i += 1

    return ret
