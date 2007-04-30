cimport python
import evas.c_evas


def init():
    return ecore_evas_init()


def shutdown():
    return ecore_evas_shutdown()

cdef char *engines[6]
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_X11] = "software_x11"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_FB] = "software_fb"
engines[<int>ECORE_EVAS_ENGINE_GL_X11] = "gl_x11"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_BUFFER] = "software_buffer"
engines[<int>ECORE_EVAS_ENGINE_XRENDER_X11] = "xrender_x11"
engines[<int>ECORE_EVAS_ENGINE_DIRECTFB] = "directfb"
cdef int engines_len
engines_len = 6


def engine_type_from_name(char *method):
    cdef int i

    for i from 0 <= i < engines_len:
        if python.strcmp(method, engines[i]) == 0:
            return i
    return -1


def engine_name_from_type(int type_id):
    if type_id >= 0 and type_id < engines_len:
        return engines[type_id]
    return None


def engine_type_supported_get(method):
    cdef Ecore_Evas_Engine_Type method_id

    if isinstance(method, (int, long)):
        method_id = method
    elif isinstance(method, basestring):
        method_id = engine_type_from_name(method)
    else:
        return False

    return bool(ecore_evas_engine_type_supported_get(method_id))



include "c_ecore_evas_base.pyx"
include "c_ecore_evas_software_x11.pyx"
include "c_ecore_evas_gl_x11.pyx"
include "c_ecore_evas_xrender_x11.pyx"
include "c_ecore_evas_fb.pyx"
#include "c_ecore_evas_directfb.pyx"
include "c_ecore_evas_buffer.pyx"
