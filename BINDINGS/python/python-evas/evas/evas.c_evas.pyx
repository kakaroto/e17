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

"""
What is Evas?
=============

Evas is a clean display canvas API for several target display systems
that can draw anti-aliased text, smooth super and sub-sampled scaled
images, alpha-blend objects and much more.

It abstracts any need to know much about what the characteristics of
your display system are or what graphics calls are used to draw them
and how. It deals on an object level where all you do is create and
manipulate objects in a canvas, set their properties, and the rest is
done for you.

Evas optimises the rendering pipeline to minimise effort in redrawing
changes made to the canvas and so takes this work out of the
programmers hand, saving a lot of time and energy.

It's small and lean, designed to work on embedded systems all the way
to large and powerful multi-cpu workstations. It can be compiled to
only have the features you need for your target platform if you so
wish, thus keeping it small and lean. It has several display
back-ends, letting it display on several display systems, making it
portable for cross-device and cross-platform development.

What Evas is not?
-----------------

Evas is not a widget set or widget toolkit, however it is their base. See
U{Elementary<http://docs.enlightenment.org/auto/elementary/>} for a toolkit
based on Evas, Edje, Ecore and other Enlightenment technologies.

It is not dependent or aware of main loops, input or output systems. Input
should be polled from various sources and fed to Evas. Similarly, it will
not create windows or report windows updates to your system, rather just
drawing the pixels and reporting to the user the areas that were changed. Of
course these operations are quite common and thus they are ready to use in
Ecore, particularly in
U{Ecore_Evas<http://docs.enlightenment.org/auto/ecore/>}.


How does Evas work?
===================

Evas is a canvas display library. This is markedly different from most
display and windowing systems as a canvas is structural and is also a
state engine, whereas most display and windowing systems are immediate
mode display targets. Evas handles the logic between a structural
display via its state engine, and controls the target windowing system
in order to produce rendered results of the current canvas' state on
the display.

Immediate mode display systems retain very little, or no state. A
program will execute a series of commands, as in the pseudo code::

    draw line from position (0, 0) to position (100, 200);

    draw rectangle from position (10, 30) to position (50, 500);

    bitmap_handle = create_bitmap();
    scale bitmap_handle to size 100 x 100;
    draw image bitmap_handle at position (10, 30);

The series of commands is executed by the windowing system and the
results are displayed on the screen (normally). Once the commands are
executed the display system has little or no idea of how to reproduce
this image again, and so has to be instructed by the application how
to redraw sections of the screen whenever needed. Each successive
command will be executed as instructed by the application and either
emulated by software or sent to the graphics hardware on the device to
be performed.

The advantage of such a system is that it is simple, and gives a
program tight control over how something looks and is drawn. Given the
increasing complexity of displays and demands by users to have better
looking interfaces, more and more work is needing to be done at this
level by the internals of widget sets, custom display widgets and
other programs. This means more and more logic and display rendering
code needs to be written time and time again, each time the
application needs to figure out how to minimise redraws so that
display is fast and interactive, and keep track of redraw logic. The
power comes at a high-price, lots of extra code and work.  Programmers
not very familiar with graphics programming will often make mistakes
at this level and produce code that is sub optimal. Those familiar
with this kind of programming will simply get bored by writing the
same code again and again.

For example, if in the above scene, the windowing system requires the
application to redraw the area from 0, 0 to 50, 50 (also referred as
"expose event"), then the programmer must calculate manually the
updates and repaint it again::

    Redraw from position (0, 0) to position (50, 50):

    // what was in area (0, 0, 50, 50)?

    // 1. intersection part of line (0, 0) to (100, 200)?
      draw line from position (0, 0) to position (25, 50);

    // 2. intersection part of rectangle (10, 30) to (50, 500)?
      draw rectangle from position (10, 30) to position (50, 50)

    // 3. intersection part of image at (10, 30), size 100 x 100?
      bitmap_subimage = subregion from position (0, 0) to position (40, 20)
      draw image bitmap_subimage at position (10, 30);

The clever reader might have noticed that, if all elements in the
above scene are opaque, then the system is doing useless paints: part
of the line is behind the rectangle, and part of the rectangle is
behind the image. These useless paints tend to be very costly, as
pixels tend to be 4 bytes in size, thus an overlapping region of 100 x
100 pixels is around 40000 useless writes! The developer could write
code to calculate the overlapping areas and avoid painting then, but
then it should be mixed with the "expose event" handling mentioned
above and quickly one realizes the initially simpler method became
really complex.

Evas is a structural system in which the programmer creates and
manages display objects and their properties, and as a result of this
higher level state management, the canvas is able to redraw the set of
objects when needed to represent the current state of the canvas.

For example, the pseudo code::

    line_handle = create_line();
    set line_handle from position (0, 0) to position (100, 200);
    show line_handle;

    rectangle_handle = create_rectangle();
    move rectangle_handle to position (10, 30);
    resize rectangle_handle to size 40 x 470;
    show rectangle_handle;

    bitmap_handle = create_bitmap();
    scale bitmap_handle to size 100 x 100;
    move bitmap_handle to position (10, 30);
    show bitmap_handle;

    render scene;

This may look longer, but when the display needs to be refreshed or
updated, the programmer only moves, resizes, shows, hides etc. the
objects that need to change. The programmer simply thinks at the
object logic level, and the canvas software does the rest of the work
for them, figuring out what actually changed in the canvas since it
was last drawn, how to most efficiently redraw the canvas and its
contents to reflect the current state, and then it can go off and do
the actual drawing of the canvas.

This lets the programmer think in a more natural way when dealing with
a display, and saves time and effort of working out how to load and
display images, render given the current display system etc. Since
Evas also is portable across different display systems, this also
gives the programmer the ability to have their code ported and
displayed on different display systems with very little work.

Evas can be seen as a display system that stands somewhere between a
widget set and an immediate mode display system. It retains basic
display logic, but does very little high-level logic such as
scrollbars, sliders, push buttons etc.


Next Steps
==========

After you understood what Evas is and installed it in your system you
should proceed understanding the programming interface for all
objects, then see the specific for the most used elements. We'd
recommend you to take a while to learn
U{Ecore<http://docs.enlightenment.org/auto/ecore/>} and
U{Edje<http://docs.enlightenment.org/auto/edje/>} as they will likely save
you tons of work compared to using just Evas directly.

Recommended reading:

    - L{Object}, where you'll get how to basically manipulate generic
        objects lying on an Evas canvas, handle canvas and object events, etc.
    - L{Rectangle}, to learn about the most basic object type on Evas --
        the rectangle.
    - L{Polygon}, to learn how to create polygon elements on the canvas.
    - L{Line}, to learn how to create line elements on the canvas.
    - L{Image}, to learn about image objects, over which Evas can do a
        plethora of operations.
    - L{Text}, to learn how to create textual elements on the canvas.
    - L{Textblock}, to learn how to create multiline textual elements on
        the canvas.
    - L{SmartObject}, to define new objects that provide B{custom}
        functions to handle clipping, hiding, moving, resizing, color
        setting and more. These could be as simple as a group of objects
        that move together (see L{SmartObjectClipped}) up to
        implementations of what ends to be a widget, providing some
        intelligence (thus the name) to Evas objects -- like a button or
        check box, for example.

@copyright: Evas is Copyright (C) 2000-2012 Carsten Haitzler and various contributors (see AUTHORS)

@license: Evas is licensed as-is (see its COPYING file), the python bindings LGPL-2.1

@author: U{Carsten Haitzler<mailto:raster@rasterman.com>}
@author: U{Till Adam<mailto:till@adam-lilienthal.de>}
@author: U{Steve Ireland<mailto:sireland@pobox.com>}
@author: U{Brett Nash<mailto:nash@nash.id.au>}
@author: U{Tilman Sauerbeck<mailto:tilman@code-monkey.de>}
@author: U{Corey Donohoe<mailto:atmos@atmos.org>}
@author: U{Yuri Hudobin<mailto:glassy_ape@users.sourceforge.net>}
@author: U{Nathan Ingersoll<mailto:ningerso@d.umn.edu>}
@author: U{Willem Monsuwe<mailto:willem@stack.nl>}
@author: U{Jose O Gonzalez<mailto:jose_ogp@juno.com>}
@author: U{Bernhard Nemec<mailto:Bernhard.Nemec@viasyshc.com>}
@author: U{Jorge Luis Zapata Muga<mailto:jorgeluis.zapata@gmail.com>}
@author: U{Cedric Bail<mailto:cedric.bail@free.fr>}
@author: U{Gustavo Sverzut Barbieri<mailto:barbieri@profusion.mobi>}
@author: U{Vincent Torri<mailto:vtorri@univ-evry.fr>}
@author: U{Tim Horton<mailto:hortont424@gmail.com>}
@author: U{Tom Hacohen<mailto:tom@stosb.com>}
@author: U{Mathieu Taillefumier<mailto:mathieu.taillefumier@free.fr>}
@author: U{Iván Briano<mailto:ivan@profusion.mobi>}
@author: U{Gustavo Lima Chaves<mailto:glima@profusion.mobi>}
@author: Samsung Electronics
@author: Samsung SAIT
@author: U{Sung W. Park<mailto:sungwoo@gmail.com>}
@author: U{Jiyoun Park<mailto:jy0703.park@samsung.com>}
@author: U{Myoungwoon Roy Kim(roy_kim) <mailto:myoungwoon.kim@samsung.com>} (U{alt<mailto:myoungwoon@gmail.com>})
@author: U{Thierry el Borgi<mailto:thierry@substantiel.fr>}
@author: U{Shilpa Singh<mailto:shilpa.singh@samsung.com>} (U{alt<mailto:shilpasingh.o@gmail.com>})
@author: U{ChunEon Park<mailto:hermet@hermet.pe.kr>}
@author: U{Christopher 'devilhorns' Michael<mailto:cpmichael1@comcast.net>}
@author: U{Seungsoo Woo<mailto:om101.woo@samsung.com>}
@author: U{Youness Alaoui<mailto:kakaroto@kakaroto.homelinux.net>}
@author: U{Jim Kukunas<mailto:james.t.kukunas@linux.intel.com>}
@author: U{Nicolas Aguirre<mailto:aguirre.nicolas@gmail.com>}
@author: U{Rafal Krypa<mailto:r.krypa@samsung.com>}
@author: U{Hyoyoung Chang<mailto:hyoyoung@gmail.com>}
@author: U{Jérôme Pinot<mailto:ngc891@gmail.com>}
@author: U{Rafael Antognolli<mailto:antognolli@profusion.mobi>}

@contact: U{Enlightenment developer mailing list<mailto:enlightenment-devel@lists.sourceforge.net>}

"""

from cpython cimport PyObject, Py_INCREF, Py_DECREF
from cpython cimport PyMethod_New
from cpython cimport PyMem_Malloc
from cpython cimport bool
import warnings

cdef int PY_REFCOUNT(object o):
    cdef PyObject *obj = <PyObject *>o
    return obj.ob_refcnt

cdef unicode _touni(char* s):
    return s.decode('UTF-8', 'strict')

cdef unicode _ctouni(const_char_ptr s):
    return s.decode('UTF-8', 'strict')

cdef char* _fruni(s):
    cdef char* c_string
    if not s:
        return NULL
    if isinstance(s, unicode):
        string = s.encode('UTF-8')
        c_string = string
    elif isinstance(s, str):
        c_string = s
    else:
        raise TypeError("Expected str or unicode object, got %s" % (type(s).__name__))
    return c_string

cdef const_char_ptr _cfruni(s):
    cdef const_char_ptr c_string
    if not s:
        return NULL
    if isinstance(s, unicode):
        string = s.encode('UTF-8')
        c_string = string
    elif isinstance(s, str):
        c_string = s
    else:
        raise TypeError("Expected str or unicode object, got %s" % (type(s).__name__))
    return c_string

def init():
    # when changing these, also change __init__.py!
    if evas_object_event_callbacks_len != EVAS_CALLBACK_LAST:
        raise SystemError("Number of object callbacks changed from %d to %d." %
                          (evas_object_event_callbacks_len, EVAS_CALLBACK_LAST))
    if evas_canvas_event_callbacks_len != EVAS_CALLBACK_LAST:
        raise SystemError("Number of canvas callbacks changed from %d to %d." %
                          (evas_canvas_event_callbacks_len, EVAS_CALLBACK_LAST))
    return evas_init()


def shutdown():
    return evas_shutdown()


def render_method_lookup(char *name):
    """render_method_lookup(name)

    Lookup render method and return its id (> 0 if found).

    @param name: Render method
    @type name: string
    @return: ID
    @rtype: int

    """
    return evas_render_method_lookup(name)


def render_method_list():
    """render_method_list()

    Returns a list of render method names.

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


def async_events_fd_get():
    """async_events_fd_get()

    Retrive the file descriptor used to poll for asynchronous events.

    If this is >= 0 it represents the file descriptor to poll in your
    main loop. When there is something to read you should call
    L{async_events_process()}.

    @note: ecore.evas already does this for you, you just need this if
    you are not using ecore.evas.

    @return: -1 on failure or the file descriptor on success (>= 0).

    """
    return evas_async_events_fd_get()


def async_events_process():
    """Process pending asynchronous events.

    You should monitor the file descriptor returned by
    L{async_events_fd_get()} and call this function when there is
    something to read.

    @return: number of processed events.
    """
    return evas_async_events_process()

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
"""Object mapping is a dictionary into which object type names can be
registered. These can be used to find a bindings class for an object using
the Object_from_instance function."""

object_mapping = {
    "image": Image,
    "text": Text,
    "textblock": Textblock,
    "rectangle": Rectangle,
    "line": Line,
    "polygon": Polygon,
    }

def _object_mapping_register(name, cls):
    if name in object_mapping:
        raise ValueError("object type name '%s' already registered." % name)
    object_mapping[name] = cls

def _object_mapping_unregister(name):
    object_mapping.pop(name)

cdef object extended_object_mapping
"""Extended object mapping is a dictionary into which class resolver
functions can be registered. These can be used to find a bindings class for
an object using the Object_from_instance function."""

extended_object_mapping = {}

def _extended_object_mapping_register(name, cls_resolver):
    if name in extended_object_mapping:
        raise ValueError("object type name '%s' already registered." % name)
    extended_object_mapping[name] = cls_resolver

def _extended_object_mapping_unregister(name):
    extended_object_mapping.pop(name)

cdef Object Object_from_instance(Evas_Object *obj):
    """Create a python object from a C Evas object."""
    cdef void *data
    cdef Object o
    cdef const_char_ptr t
    cdef Canvas c

    if obj == NULL:
        return None

    data = evas_object_data_get(obj, _cfruni("python-evas"))
    if data != NULL:
        o = <Object>data
    else:
        t = evas_object_type_get(obj)
        if t == NULL:
            raise ValueError("Evas object %#x does not have a type!" %
                             <long>obj)
        ot = _ctouni(t)
        c = Canvas_from_instance(evas_object_evas_get(obj))
        cls = object_mapping.get(ot, None)
        if cls is None:
            cls_resolver = extended_object_mapping.get(ot, None)
            if cls_resolver is None:
                warnings.warn(
                    ("Evas_Object %#x of type %s has no direct or "
                     "extended mapping! Using generic wrapper.") %
                    (<unsigned long>obj, ot))
                cls = Object
            else:
                cls = cls_resolver(<unsigned long>obj)
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

    @param desc: can be either a string, an integer or a tuple.
    @type desc: string, int or tuple
    @param is_premul: specifies if the color is in pre-multiply form. This
        is the format expected by evas.
    @type is_premul: bool
    @return: (r, g, b, a) in pre-multiply form.
    @rtype: tuple of ints

    """
    cdef unsigned long c, desc_len
    cdef int r, g, b, a

    r = 0
    g = 0
    b = 0
    a = 0

    if isinstance(desc, str):
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

    @param r: Red
    @type r: int
    @param g: Green
    @type g: int
    @param b: Blue
    @type b: int
    @param a: Alpha
    @type a: int
    @return: Pre-multiplied (r, g, b, a)
    @rtype: tuple of ints

    """
    evas_color_argb_premul(a, &r, &g, &b)
    return (r, g, b, a)

def color_argb_unpremul(int r, int g, int b, int a):
    """Convert color to regular (no pre-multiplied) format.

    @note: Evas works with pre-multiplied colors internally, so every
           color that comes from or goes to it must be in this format.

    @param r: Red
    @type r: int
    @param g: Green
    @type g: int
    @param b: Blue
    @type b: int
    @param a: Alpha
    @type a: int
    @return: (r, g, b, a)
    @rtype: tuple of ints

    """
    evas_color_argb_unpremul(a, &r, &g, &b)
    return (r, g, b, a)

def color_hsv_to_rgb(float h, float s, float v):
    """Convert color from HSV to RGB format.

    @param h: Hue
    @type h: float
    @param s: Saturation
    @type s: float
    @param v: Value
    @type v: float
    @return: (r, g, b)
    @rtype: tuple of ints

    """
    cdef int r, g, b
    evas_color_hsv_to_rgb(h, s, v, &r, &g, &b)
    return (r, g, b)

def color_rgb_to_hsv(int r, int g, int b):
    """Convert color from RGB to HSV format.

    @param r: Red
    @type r: int
    @param g: Green
    @type g: int
    @param b: Blue
    @type b: int
    @return: (h, s, v)
    @rtype: tuple of ints

    """
    cdef float h, s, v
    evas_color_rgb_to_hsv(r, g, b, &h, &s, &v)
    return (h, s, v)


class EvasLoadError(Exception):
    def __init__(self, int code, filename, key):
        if code == EVAS_LOAD_ERROR_NONE:
            msg = "No error on load"
        elif code == EVAS_LOAD_ERROR_GENERIC:
            msg = "A non-specific error occurred"
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


cdef extern from "Python.h":
    ctypedef struct PyTypeObject:
        PyTypeObject *ob_type

cdef void _install_metaclass(PyTypeObject *ctype, object metaclass):
    """Installs the metaclass to the object, which is otherwise not
    supported for cdef classes by cython."""
    Py_INCREF(metaclass)
    ctype.ob_type = <PyTypeObject*>metaclass


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
include "evas.c_evas_map.pxi"
include "evas.c_evas_canvas_callbacks.pxi"
include "evas.c_evas_canvas.pxi"
include "evas.c_evas_object_events.pxi"
include "evas.c_evas_object_callbacks.pxi"
include "evas.c_evas_object.pxi"
include "evas.c_evas_object_smart.pxi"
include "evas.c_evas_object_rectangle.pxi"
include "evas.c_evas_object_line.pxi"
include "evas.c_evas_object_image.pxi"
include "evas.c_evas_object_polygon.pxi"
include "evas.c_evas_object_text.pxi"
include "evas.c_evas_object_textblock.pxi"
include "evas.c_evas_object_box.pxi"
