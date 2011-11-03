# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Carsten Haitzler, Ulisses Furquim
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

from cpython cimport PyObject, Py_INCREF, Py_DECREF
from cpython cimport PyMem_Malloc, PyMem_Free
from cpython cimport PyBuffer_FromObject
from cpython cimport bool
import evas.c_evas

__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )


cdef int PY_REFCOUNT(object o):
    cdef PyObject *obj = <PyObject *>o
    return obj.ob_refcnt


def init():
    return ecore_evas_init()


def shutdown():
    return ecore_evas_shutdown()

cdef object engine_id_mapping
engine_id_mapping = {
    "buffer": ECORE_EVAS_ENGINE_SOFTWARE_BUFFER,
    "software_x11": ECORE_EVAS_ENGINE_SOFTWARE_XLIB,
    "xrender_x11": ECORE_EVAS_ENGINE_XRENDER_X11,
    "opengl_x11": ECORE_EVAS_ENGINE_OPENGL_X11,
    "software_xcb": ECORE_EVAS_ENGINE_SOFTWARE_XCB,
    "xrender_xcb": ECORE_EVAS_ENGINE_XRENDER_XCB,
    "software_gdi": ECORE_EVAS_ENGINE_SOFTWARE_GDI,
    "software_ddraw": ECORE_EVAS_ENGINE_SOFTWARE_DDRAW,
    "direct3d": ECORE_EVAS_ENGINE_DIRECT3D,
    "opengl_glew": ECORE_EVAS_ENGINE_OPENGL_GLEW,
    "cocoa": ECORE_EVAS_ENGINE_OPENGL_COCOA,
    "sdl": ECORE_EVAS_ENGINE_SOFTWARE_SDL,
    "directfb": ECORE_EVAS_ENGINE_DIRECTFB,
    "fb": ECORE_EVAS_ENGINE_SOFTWARE_FB,
    "software_16_x11": ECORE_EVAS_ENGINE_SOFTWARE_16_X11,
    "software_16_ddraw": ECORE_EVAS_ENGINE_SOFTWARE_16_DDRAW,
    "software_16_wince": ECORE_EVAS_ENGINE_SOFTWARE_16_WINCE,
    }

cdef object engine_mapping
engine_mapping = {
  "software_x11": SoftwareX11,
  "xrender_x11": XRenderX11,
  "opengl_x11": GLX11,
  "software_xcb": SoftwareX11,
  "xrender_xcb": XRenderX11,
  "software_16_x11": SoftwareX11_16,
#  "directfb": DirectFB,
  "fb": FB,
#  "software_gdi": ,
#  "software_ddraw": ,
#  "direct3d": ,
#  "opengl_glew": ,
#  "software_16_ddraw": ,
#  "software_16_wince": ,
#  "software_16_wince_fb": ,
#  "software_16_wince_gapi": ,
#  "sdl": ,
#  "software_16_sdl": ,
#  "cocoa": ,
  "buffer": Buffer,
  }


def engine_type_from_name(char *method):
    """Retrive the type id for the engine given it's name.

    @return: >= 0 on success or -1 on failure.
    @rtype: int
    """
    return engine_id_mapping.get(method, -1)


def engine_name_from_type(int type_id):
    """Retrive the engine name given it's id.

    @rtype: str
    """
    for name, id in engine_id_mapping.itervalues():
        if id == type_id:
            return name
    return None


def engine_type_supported_get(method):
    """Check if engine is supported.

    @rtype: bool
    """
    cdef int method_id

    if isinstance(method, (int, long)):
        method_id = method
    elif isinstance(method, basestring):
        method_id = engine_type_from_name(method)
        if method_id < 0:
            raise ValueError("unknown method name %r" % method)
    else:
        return False

    return bool(ecore_evas_engine_type_supported_get
                (<Ecore_Evas_Engine_Type>method_id))


def engines_get():
    """List all supported engines.

    @rtype: list of str
    """
    cdef Eina_List *l, *orig_l
    cdef char *s
    lst = []
    orig_l = l = ecore_evas_engines_get()
    while l != NULL:
        s = <char *>l.data
        if s != NULL:
            lst.append(s)
        l = l.next
    ecore_evas_engines_free(orig_l)
    return lst


def new(engine_name=None, int x=0, int y=0, int w=320, int h=240, extra_options=None):
    """Creates a new EcoreEvas based on engine name.

       @parm: B{engine_name:} name of engine to use (as of engines_get()) or
              None to use auto discovery.
       @parm: B{x:} horizontal placement.
       @parm: B{y:} vertical placement.
       @parm: B{w:} window width.
       @parm: B{h:} window height.
       @parm: B{extra_options:} if not None, a textual description of
              parameters.

       @raise SystemError: if problems occur.
    """
    cdef Ecore_Evas *ee
    cdef EcoreEvas o
    cdef const_char_ptr en, eo

    if engine_name is None:
        en = NULL
    else:
        en = engine_name

    if extra_options is None:
        eo = NULL
    else:
        eo = extra_options

    ee = ecore_evas_new(en, x, y, w, h, eo)
    if ee == NULL:
        raise SystemError(("could not create engine named %r with geometry "
                           "%d,%d %dx%d and extra options %s") %
                          (engine_name, x, y, w, h, extra_options))

    en = ecore_evas_engine_name_get(ee)
    if en == NULL:
        raise SystemError("Ecore_Evas has no engine name!")

    cls = engine_mapping.get(en, None)
    if cls is None:
        raise SystemError("Ecore_Evas has no associated wrapper: %s" % en)

    o = cls.__new__(cls)
    o._set_obj(ee)
    return o


include "ecore.evas.c_ecore_evas_base.pxi"
include "ecore.evas.c_ecore_evas_base_x11.pxi"
include "ecore.evas.c_ecore_evas_software_x11.pxi"
include "ecore.evas.c_ecore_evas_gl_x11.pxi"
include "ecore.evas.c_ecore_evas_xrender_x11.pxi"
include "ecore.evas.c_ecore_evas_fb.pxi"
include "ecore.evas.c_ecore_evas_buffer.pxi"
include "ecore.evas.c_ecore_evas_software_x11_16.pxi"
