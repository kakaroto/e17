cimport ecore.c_ecore_data

def init():
    return ecore_imf_init()

def shutdown():
    return ecore_imf_shutdown()

def available_ids():
    cdef Ecore_List *lst
    cdef char *data
    ret = []

    lst = ecore_imf_context_available_ids_get()
    if lst != NULL:
        ecore.c_ecore_data.ecore_list_first_goto(lst)
        data = <char *> ecore.c_ecore_data.ecore_list_next(lst)

        while data != NULL:
            ret.append(data)
            data = <char*> ecore.c_ecore_data.ecore_list_next(lst)

        ecore.c_ecore_data.ecore_list_destroy(lst)

    return ret

def available_ids_by_canvas_type(char *canvas_type):
    cdef Ecore_List *lst
    cdef char *data
    ret = []

    lst = ecore_imf_context_available_ids_by_canvas_type_get(canvas_type)
    if lst != NULL:
        ecore.c_ecore_data.ecore_list_first_goto(lst)
        data = <char *> ecore.c_ecore_data.ecore_list_next(lst)

        while data != NULL:
            ret.append(data)
            data = <char*> ecore.c_ecore_data.ecore_list_next(lst)

        ecore.c_ecore_data.ecore_list_destroy(lst)

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
