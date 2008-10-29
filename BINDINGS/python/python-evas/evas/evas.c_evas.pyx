# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Ulisses Furquim
#
# This file is part of Python-Evas.
#
# Python-Evas is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Evas is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Evas.  If not, see <http://www.gnu.org/licenses/>.

cimport evas.python as python

__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )

def init():
    return evas_init()


def shutdown():
    return evas_shutdown()


def render_method_lookup(char *name):
    """Lookup render method and return its id (> 0 if found).

    @parm: name
    @rtype: int
    """
    return evas_render_method_lookup(name)


def render_method_list():
    """Returns a list of render method names.

    @rtype: list of str
    """
    cdef Eina_List *lst

    ret = []
    lst = evas_render_method_list()
    while lst != NULL:
        ret.append(<char*> lst.data)
        lst = lst.next

    evas_render_method_list_free(lst)
    return ret

cdef object canvas_mapping

canvas_mapping = dict()

cdef Canvas Canvas_from_instance(Evas *evas):
    cdef void *data
    cdef Canvas c

    if evas == NULL:
        return None

    o = canvas_mapping.get(<long>evas, None)
    if o is not None:
        c = o
    else:
        c = Canvas.__new__(Canvas)
        c._set_obj(evas) # calls Canvas_remember()

    return c

cdef int Canvas_remember(long ptr, Canvas c) except 0:
    o = canvas_mapping.get(ptr, None)
    if o is not None:
        raise ValueError("Canvas 0x%x already registered for %s" % (ptr, o))
    canvas_mapping[ptr] = c
    return 1


cdef int Canvas_forget(long ptr) except 0:
    try:
        canvas_mapping.pop(ptr)
    except KeyError, e:
        raise ValueError("Canvas %#x is unknown" % ptr)
    return 1


# XXX: this should be C-only, but it would require ecore_evas
# XXX: being able to use it.
def _Canvas_from_instance(long ptr):
    return Canvas_from_instance(<Evas *>ptr)


cdef object object_mapping

object_mapping = {
    "image": Image,
    "text": Text,
    "rectangle": Rectangle,
    "line": Line,
    "gradient": Gradient,
    "polygon": Polygon,
    }


def _object_mapping_register(char *name, cls):
    if name in object_mapping:
        raise ValueError("object type name '%s' already registered." % name)
    object_mapping[name] = cls


def _object_mapping_unregister(char *name):
    del object_mapping[name]


cdef Object Object_from_instance(Evas_Object *obj):
    cdef void *data
    cdef Object o
    cdef char *t
    cdef Canvas c

    if obj == NULL:
        return None

    data = evas_object_data_get(obj, "python-evas")
    if data != NULL:
        o = <Object>data
    else:
        t = evas_object_type_get(obj)
        c = Canvas_from_instance(evas_object_evas_get(obj))
        cls = object_mapping.get(t, Object)
        o = cls.__new__(cls)
        o._set_evas(c)
        o._set_obj(obj)

    return o


# XXX: this should be C-only, but it would require edje
# XXX: being able to use it.
def _Object_from_instance(long ptr):
    return Object_from_instance(<Evas_Object *>ptr)


def color_parse(desc, is_premul=None):
    """Converts a color description to (r, g, b, a) in pre-multiply form.

    C{is_premul} default value will depend on desc type:
     - desc is string: C{is_premul=False}
     - desc is integer: C{is_premul=False}
     - desc is tuple: C{is_premul=True}

    @parm: B{desc} can be either a string, an integer or a tuple.
    @parm: B{is_premul} specifies if the color is in pre-multiply form. This
           is the format expected by evas.
    @return: (r, g, b, a) in pre-multiply form.
    @rtype: tuple of int
    """
    cdef unsigned long c, desc_len
    cdef int r, g, b, a

    r = 0
    g = 0
    b = 0
    a = 0

    if isinstance(desc, basestring):
        if not desc or desc[0] != "#":
            raise ValueError("Invalid color description")
        desc_len = len(desc)
        c = int(desc[1:], 16)
        r = (c >> 16) & 0xff
        g = (c >> 8) & 0xff
        b = c & 0xff

        if is_premul is None:
            is_premul = False

        if desc_len == 9:
            a = (c >> 24) & 0xff
        elif desc_len == 7:
            a = 255
        else:
            raise ValueError("Invalid color description")

    elif isinstance(desc, (int, long)):
        c = desc
        a = (c >> 24) & 0xff
        r = (c >> 16) & 0xff
        g = (c >> 8) & 0xff
        b = c & 0xff

        if is_premul is None:
            is_premul = False

    elif isinstance(desc, (list, tuple)):
        if is_premul is None:
            is_premul = True

        if len(desc) == 3:
            a = 255
            r, g, b = desc
        else:
            r, g, b, a = desc
    else:
        raise TypeError("Unsupported type %s for color description." %
                        type(desc))

    if is_premul is False:
        evas_color_argb_premul(a, &r, &g, &b)

    return (r, g, b, a)


def color_argb_premul(int r, int g, int b, int a):
    """Convert color to pre-multiplied format.

    @note: Evas works with pre-multiplied colors internally, so every
           color that comes from or goes to it must be in this format.

    @parm: r
    @parm: g
    @parm: b
    @parm: a
    @return: pre-multiplied (r, g, b, a)
    @rtype: tuple of int
    """
    evas_color_argb_premul(a, &r, &g, &b)
    return (r, g, b, a)

def color_argb_unpremul(int r, int g, int b, int a):
    """Convert color to regular (no pre-multiplied) format.

    @note: Evas works with pre-multiplied colors internally, so every
           color that comes from or goes to it must be in this format.

    @parm: r
    @parm: g
    @parm: b
    @parm: a
    @return: (r, g, b, a)
    @rtype: tuple of int
    """
    evas_color_argb_unpremul(a, &r, &g, &b)
    return (r, g, b, a)

def color_hsv_to_rgb(float h, float s, float v):
    """Convert color from HSV to RGB format.

    @parm: h
    @parm: s
    @parm: v
    @return: (r, g, b)
    @rtype: tuple of int
    """
    cdef int r, g, b
    evas_color_hsv_to_rgb(h, s, v, &r, &g, &b)
    return (r, g, b)

def color_rgb_to_hsv(int r, int g, int b):
    """Convert color from RGB to HSV format.

    @parm: r
    @parm: g
    @parm: b
    @return: (h, s, v)
    @rtype: tuple of int
    """
    cdef float h, s, v
    evas_color_rgb_to_hsv(r, g, b, &h, &s, &v)
    return (h, s, v)


class EvasLoadError(Exception):
    def __init__(self, int code, filename, key):
        if code == EVAS_LOAD_ERROR_NONE:
            msg = "No error on load"
        elif code == EVAS_LOAD_ERROR_GENERIC:
            msg = "A non-specific error occured"
        elif code == EVAS_LOAD_ERROR_DOES_NOT_EXIST:
            msg = "File (or file path) does not exist"
        elif code == EVAS_LOAD_ERROR_PERMISSION_DENIED:
            msg = "Permission deinied to an existing file (or path)"
        elif code == EVAS_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED:
            msg = "Allocation of resources failure prevented load"
        elif code == EVAS_LOAD_ERROR_CORRUPT_FILE:
            msg = "File corrupt (but was detected as a known format)"
        elif code == EVAS_LOAD_ERROR_UNKNOWN_FORMAT:
            msg = "File is not a known format"
        self.code = code
        self.file = filename
        self.key = key
        Exception.__init__(self, "%s (file=%s, key=%s)" % (msg, filename, key))


cdef void _install_metaclass(python.PyTypeObject *ctype, object metaclass):
    python.Py_INCREF(metaclass)
    ctype.ob_type = <python.PyTypeObject*>metaclass


class EvasObjectMeta(type):
    def __init__(cls, name, bases, dict_):
        type.__init__(cls, name, bases, dict_)
        cls._fetch_evt_callbacks()

    def _fetch_evt_callbacks(cls):
        if "__evas_event_callbacks__" in cls.__dict__:
            return

        cls.__evas_event_callbacks__ = []
        append = cls.__evas_event_callbacks__.append

        for name in dir(cls):
            val = getattr(cls, name)
            if not callable(val) or not hasattr(val, "evas_event_callback"):
                continue
            evt = getattr(val, "evas_event_callback")
            append((name, evt))


include "evas.c_evas_rect.pxi"
include "evas.c_evas_canvas.pxi"
include "evas.c_evas_object_events.pxi"
include "evas.c_evas_object_callbacks.pxi"
include "evas.c_evas_object.pxi"
include "evas.c_evas_object_smart.pxi"
include "evas.c_evas_object_rectangle.pxi"
include "evas.c_evas_object_line.pxi"
include "evas.c_evas_object_image.pxi"
include "evas.c_evas_object_gradient.pxi"
include "evas.c_evas_object_polygon.pxi"
include "evas.c_evas_object_text.pxi"
