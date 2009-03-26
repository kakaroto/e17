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

cimport evas.python as python
import evas.c_evas

__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )

def init():
    return ecore_evas_init()


def shutdown():
    return ecore_evas_shutdown()

cdef char *engines[16]
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_BUFFER] = "buffer"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_X11] = "software_x11"
engines[<int>ECORE_EVAS_ENGINE_XRENDER_X11] = "xrender_x11"
engines[<int>ECORE_EVAS_ENGINE_OPENGL_X11] = "opengl_x11"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_XCB] = "software_xcb"
engines[<int>ECORE_EVAS_ENGINE_XRENDER_XCB] = "xrender_xcb"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_DDRAW] = "software_ddraw"
engines[<int>ECORE_EVAS_ENGINE_DIRECT3D] = "direct3d"
engines[<int>ECORE_EVAS_ENGINE_OPENGL_GLEW] = "opengl_glew"
engines[<int>ECORE_EVAS_ENGINE_QUARTZ] = "quartz"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_SDL] = "sdl"
engines[<int>ECORE_EVAS_ENGINE_DIRECTFB] = "directfb"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_FB] = "fb"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_16_X11] = "software_16_x11"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_16_DDRAW] = "software_16_ddraw"
engines[<int>ECORE_EVAS_ENGINE_SOFTWARE_16_WINCE] = "software_16_wince"
cdef int engines_len
engines_len = sizeof(engines)/sizeof(engines[0])

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
#  "software_ddraw": ,
#  "direct3d": ,
#  "opengl_glew": ,
#  "software_16_ddraw": ,
#  "software_16_wince": ,
#  "software_16_wince_fb": ,
#  "software_16_wince_gapi": ,
#  "sdl": ,
#  "software_16_sdl": ,
#  "quartz": ,
  "buffer": Buffer,
  }


def engine_type_from_name(char *method):
    """Retrive the type id for the engine given it's name.

    @return: >= 0 on success or -1 on failure.
    @rtype: int
    """
    cdef int i

    for i from 0 <= i < engines_len:
        if engines[i] == NULL:
            raise ValueError(("Ecore_Evas_Engine_Type changed and bindings are "
                              "now invalid, position %d is now NULL!") % i)
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
    cdef char *en, *eo

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
                          engine_name, x, y, w, h, extra_options)

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
