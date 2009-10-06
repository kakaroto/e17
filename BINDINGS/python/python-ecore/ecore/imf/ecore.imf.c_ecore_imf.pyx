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

cimport ecore.c_ecore

def init():
    return ecore_imf_init()

def shutdown():
    return ecore_imf_shutdown()

def available_ids():
    cdef ecore.c_ecore.Eina_List *lst, *itr
    cdef char *data
    ret = []

    lst = itr = ecore_imf_context_available_ids_get()
    while itr != NULL:
        data = <char *>itr.data
        ret.append(data)
        itr = itr.next
    ecore.c_ecore.eina_list_free(lst)

    return ret

def available_ids_by_canvas_type(char *canvas_type):
    cdef ecore.c_ecore.Eina_List *lst, *itr
    cdef char *data
    ret = []

    lst = itr = ecore_imf_context_available_ids_by_canvas_type_get(canvas_type)
    while itr != NULL:
        data = <char *>itr.data
        ret.append(data)
        itr = itr.next
    ecore.c_ecore.eina_list_free(lst)

    return ret

def default_id():
    return ecore_imf_context_default_id_get()

def default_id_by_canvas_type(char* canvas_type):
    return ecore_imf_context_default_id_by_canvas_type_get(canvas_type)

def info_by_id(char *id):
    cdef Ecore_IMF_Context_Info *info
    ret = {}

    # Raise exception???
    if id != NULL:
        info = ecore_imf_context_info_by_id_get(id)

        # Raise exception???
        if info != NULL:
            if info.id != NULL:
                ret["id"] = info.id

            if info.description != NULL:
                ret["description"] = info.description

            if info.default_locales != NULL:
                ret["default_locales"] = info.default_locales

            if info.canvas_type != NULL:
                ret["canvas_type"] = info.canvas_type

            ret["canvas_required"] = bool(info.canvas_required)

    return ret

#include "ecore.imf.c_ecore_imf_context.pxi"
