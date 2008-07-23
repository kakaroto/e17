cimport evas.python as python
import evas.c_evas

__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )

def init():
    return ecore_evas_init()


def shutdown():
    return ecore_evas_shutdown()

cdef char *engines[7]
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_X11] = "software_x11"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_FB] = "software_fb"
engines[<int>ECORE_EVAS_ENGINE_OPENGL_X11] = "gl_x11"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_BUFFER] = "software_buffer"
engines[<int>ECORE_EVAS_ENGINE_XRENDER_X11] = "xrender_x11"
engines[<int>ECORE_EVAS_ENGINE_DIRECTFB] = "directfb"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_16_X11] = "software_x11_16"
cdef int engines_len
engines_len = 7


def engine_type_from_name(char *method):
    """Retrive the type id for the engine given it's name.

    @return: >= 0 on success or -1 on failure.
    @rtype: int
    """
    cdef int i

    for i from 0 <= i < engines_len:
        if python.strcmp(method, engines[i]) == 0:
            return i
    return -1


def engine_name_from_type(int type_id):
    """Retrive the engine name given it's id.

    @rtype: str
    """
    if type_id >= 0 and type_id < engines_len:
        return engines[type_id]
    return None


def engine_type_supported_get(method):
    """Check if engine is supported.

    @rtype: bool
    """
    cdef Ecore_Evas_Engine_Type method_id

    if isinstance(method, (int, long)):
        method_id = method
    elif isinstance(method, basestring):
        method_id = engine_type_from_name(method)
    else:
        return False

    return bool(ecore_evas_engine_type_supported_get(method_id))



include "ecore.evas.c_ecore_evas_base.pxi"
include "ecore.evas.c_ecore_evas_base_x11.pxi"
include "ecore.evas.c_ecore_evas_software_x11.pxi"
include "ecore.evas.c_ecore_evas_gl_x11.pxi"
include "ecore.evas.c_ecore_evas_xrender_x11.pxi"
include "ecore.evas.c_ecore_evas_fb.pxi"
#include "ecore.evas.c_ecore_evas_directfb.pxi"
include "ecore.evas.c_ecore_evas_buffer.pxi"
include "ecore.evas.c_ecore_evas_software_x11_16.pxi"
