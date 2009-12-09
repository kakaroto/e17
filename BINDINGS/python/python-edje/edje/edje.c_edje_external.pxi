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

    property b:
        def __get__(self):
            if self.obj == NULL:
                raise ValueError("Object uninitialized")
            return self.obj.i

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
            elif self.obj.type == EDJE_EXTERNAL_PARAM_TYPE_BOOL:
                return self.obj.i

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

    def validate(self, value):
        return True

cdef class ExternalParamInfoInt(ExternalParamInfo):
    property default:
        def __get__(self):
            if self.obj.info.i.default == EDJE_EXTERNAL_INT_UNSET:
                return None
            return self.obj.info.i.default
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

    def validate(self, value):
        min = self.min
        max = self.max
        if min is not None and value < min:
            return False
        if max is not None and value > max:
            return False
        return True

cdef class ExternalParamInfoDouble(ExternalParamInfo):
    property default:
        def __get__(self):
            if self.obj.info.d.default == EDJE_EXTERNAL_DOUBLE_UNSET:
                return None
            return self.obj.info.d.default
    property min:
        def __get__(self):
            if self.obj.info.d.min == EDJE_EXTERNAL_DOUBLE_UNSET:
                return None
            return self.obj.info.d.min

    property max:
        def __get__(self):
            if self.obj.info.d.max == EDJE_EXTERNAL_DOUBLE_UNSET:
                return None
            return self.obj.info.d.max

    property step:
        def __get__(self):
            if self.obj.info.d.step == EDJE_EXTERNAL_DOUBLE_UNSET:
                return None
            return self.obj.info.d.step

    def validate(self, value):
        min = self.min
        max = self.max
        if min is not None and value < min:
            return False
        if max is not None and value > max:
            return False
        return True

cdef class ExternalParamInfoString(ExternalParamInfo):
    property default:
        def __get__(self):
            if self.obj.info.s.default == NULL:
                return None
            return self.obj.info.s.default
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

cdef class ExternalParamInfoBool(ExternalParamInfo):
    property default:
        def __get__(self):
            if self.obj.info.b.default == NULL:
                return None
            return self.obj.info.b.default

    property false_string:
        def __get__(self):
            if self.obj.info.b.false_str == NULL:
                return None
            return self.obj.info.b.false_str

    property true_string:
        def __get__(self):
            if self.obj.info.b.true_str == NULL:
                return None
            return self.obj.info.b.true_str

cdef ExternalParamInfo ExternalParamInfo_from_ptr(Edje_External_Param_Info *ptr):
    cdef ExternalParamInfo p
    if ptr.type == EDJE_EXTERNAL_PARAM_TYPE_INT:
        p = ExternalParamInfoInt()
    elif ptr.type == EDJE_EXTERNAL_PARAM_TYPE_DOUBLE:
        p = ExternalParamInfoDouble()
    elif ptr.type == EDJE_EXTERNAL_PARAM_TYPE_STRING:
        p = ExternalParamInfoString()
    elif ptr.type == EDJE_EXTERNAL_PARAM_TYPE_BOOL:
        p = ExternalParamInfoBool()
    else:
        return None
    p.obj = ptr
    return p

def external_param_info_get(char *type_name):
    cdef Edje_External_Param_Info *params
    cdef int i

    ret = []
    params = edje_external_param_info_get(type_name)
    if params == NULL:
        return ret

    i = 0
    while params[i].name != NULL:
        ret.append(ExternalParamInfo_from_ptr(&params[i]))
        i += 1

    return ret

cdef class ExternalType:
    property name:
        def __get__(self):
            return self._name

    property module:
        def __get__(self):
            if self._obj.module == NULL:
                return None
            return self._obj.module

    def label_get(self):
        cdef char *l
        if self._obj.label_get == NULL:
            return None
        l = self._obj.label_get(self._obj.data)
        if l == NULL:
            return None
        ret = l
        return ret

    def description_get(self):
        cdef char *l
        if self._obj.description_get == NULL:
            return None
        l = self._obj.description_get(self._obj.data)
        if l == NULL:
            return None
        ret = l
        return ret

    def icon_add(self, evas.c_evas.Canvas canvas not None):
        cdef evas.c_evas.Evas_Object *icon
        if self._obj.icon_add == NULL:
            return None
        icon = self._obj.icon_add(self._obj.data, canvas.obj)
        if icon == NULL:
            return None
        return evas.c_evas._Object_from_instance(<long>icon)

    def preview_add(self, evas.c_evas.Canvas canvas not None):
        cdef evas.c_evas.Evas_Object *preview
        if self._obj.preview_add == NULL:
            return None
        preview = self._obj.preview_add(self._obj.data, canvas.obj)
        if preview == NULL:
            return None
        return evas.c_evas._Object_from_instance(<long>preview)

cdef class ExternalIterator:
    cdef evas.c_evas.Eina_Iterator *obj

    def __init__(self):
        cdef evas.c_evas.Eina_Iterator *it
        it = edje_external_iterator_get()
        self._set_obj(it)

    cdef _set_obj(self, evas.c_evas.Eina_Iterator *ptr):
        self.obj = ptr

    def __iter__(self):
        return self

    def __next__(self):
        cdef evas.c_evas.Eina_Hash_Tuple *tuple
        cdef ExternalType t
        if evas.c_evas.eina_iterator_next(self.obj, <void **>&tuple):
            t = ExternalType()
            t._name = <char*>tuple.key
            t._obj = <Edje_External_Type*>tuple.data
            return t
        else:
            raise StopIteration

    def __del__(self):
        if self.obj:
            evas.c_evas.eina_iterator_free(self.obj)

