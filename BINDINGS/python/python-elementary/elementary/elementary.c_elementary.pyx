# Copyright (c) 2008-2009 Simon Busch
#
# This file is part of python-elementary.
#
# python-elementary is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# python-elementary is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with python-elementary.  If not, see <http://www.gnu.org/licenses/>.
#

import sys
import evas.c_evas
cimport evas.c_evas as c_evas
from cpython cimport PyObject, Py_INCREF, Py_DECREF
from cpython cimport PyMem_Malloc, PyMem_Free
from cpython cimport bool
import traceback
import logging

logger = logging.getLogger("elementary")

cdef int PY_REFCOUNT(object o):
    cdef PyObject *obj = <PyObject *>o
    return obj.ob_refcnt

cdef _METHOD_DEPRECATED(self, replacement=None, message=None):
    stack = traceback.extract_stack()
    caller = stack[-1]
    caller_module, caller_line, caller_name, caller_code = caller
    if caller_code:
        msg = "%s:%s %s (class %s) is deprecated." % \
            (caller_module, caller_line, caller_code,
            self.__class__.__name__ if self else 'None')
    else:
        msg = "%s:%s %s.%s() is deprecated." % \
            (caller_module, caller_line,
            self.__class__.__name__ if self else 'None', caller_name)
    if replacement:
        msg += " Use %s() instead." % (replacement,)
    if message:
        msg += " " + message
    logging.warn(msg)

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
    cdef int argc, i, arg_len
    cdef char **argv, *arg
    argc = len(sys.argv)
    argv = <char **>PyMem_Malloc(argc * sizeof(char *))
    for i from 0 <= i < argc:
        if not sys.argv[i]:
            continue
        arg = _fruni(sys.argv[i])
        arg_len = len(arg)
        argv[i] = <char *>PyMem_Malloc(arg_len + 1)
        memcpy(argv[i], arg, arg_len + 1)

    elm_init(argc, argv)

def shutdown():
    elm_shutdown()

def run():
    with nogil:
        elm_run()

def exit():
    elm_exit()

def policy_set(policy, value):
    """Set new policy value.

   This will emit the ecore event ELM_EVENT_POLICY_CHANGED in the main
   loop giving the event information Elm_Event_Policy_Changed with
   policy identifier, new and old values.

   @param policy: policy identifier as in Elm_Policy.
   @param value: policy value, depends on identifiers, usually there is
          an enumeration with the same prefix as the policy name, for
          example: ELM_POLICY_QUIT and Elm_Policy_Quit
          (ELM_POLICY_QUIT_NONE, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED).

   @return: True on success or False on error (right
           now just invalid policy identifier, but in future policy
           value might be enforced).
    """
    return elm_policy_set(policy, value)

def policy_get(policy):
    """Gets the policy value set for given identifier.

    @param policy: policy identifier as in Elm_Policy.

    @return: policy value. Will be 0 if policy identifier is invalid.
    """
    return elm_policy_get(policy)

def coords_finger_size_adjust(times_w, w, times_h, h):
    cdef c_evas.Evas_Coord width
    cdef c_evas.Evas_Coord height
    width = w
    height = h
    elm_coords_finger_size_adjust(times_w, &width, times_h, &height)

def theme_overlay_add(item):
    elm_theme_overlay_add(NULL, item)

def theme_extension_add(item):
    elm_theme_extension_add(NULL, item)


cdef object _elm_widget_type_mapping

_elm_widget_type_mapping = {}

cdef _elm_widget_type_register(name, cls):
    if name in _elm_widget_type_mapping:
        raise ValueError("object type name '%s' already registered." % name)
    _elm_widget_type_mapping[name] = cls
    # TODO: Widget types become evas object types as they are being ported
    # to the new model. The class resolver can be removed when it's done.
    evas.c_evas._object_mapping_register("elm_"+name, cls)

cdef _elm_widget_type_unregister(name):
    _elm_widget_type_mapping.pop(name)
    # TODO: Widget types become evas object types as they are being ported
    # to the new model. The class resolver can be removed when it's done.
    evas.c_evas._object_mapping_unregister("elm_"+name)

cdef extern from "Python.h":
    ctypedef struct PyTypeObject:
        PyTypeObject *ob_type

cdef void _install_metaclass(PyTypeObject *ctype, object metaclass):
    Py_INCREF(metaclass)
    ctype.ob_type = <PyTypeObject*>metaclass

class ElementaryObjectMeta(type):
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

include "elementary.c_elementary_configuration.pxi"
include "elementary.c_elementary_object.pxi"
include "elementary.c_elementary_object_item.pxi"
include "elementary.c_elementary_layout_class.pxi"
include "elementary.c_elementary_layout.pxi"
include "elementary.c_elementary_image.pxi"
include "elementary.c_elementary_button.pxi"
include "elementary.c_elementary_window.pxi"
include "elementary.c_elementary_innerwindow.pxi"
include "elementary.c_elementary_background.pxi"
include "elementary.c_elementary_icon.pxi"
include "elementary.c_elementary_box.pxi"
include "elementary.c_elementary_frame.pxi"
include "elementary.c_elementary_flip.pxi"
include "elementary.c_elementary_scroller.pxi"
include "elementary.c_elementary_label.pxi"
include "elementary.c_elementary_table.pxi"
include "elementary.c_elementary_clock.pxi"
include "elementary.c_elementary_hover.pxi"
include "elementary.c_elementary_entry.pxi"
include "elementary.c_elementary_bubble.pxi"
include "elementary.c_elementary_photo.pxi"
include "elementary.c_elementary_hoversel.pxi"
include "elementary.c_elementary_toolbar.pxi"
include "elementary.c_elementary_list.pxi"
include "elementary.c_elementary_slider.pxi"
include "elementary.c_elementary_naviframe.pxi"
include "elementary.c_elementary_radio.pxi"
include "elementary.c_elementary_check.pxi"
include "elementary.c_elementary_genlist.pxi"
include "elementary.c_elementary_gengrid.pxi"
include "elementary.c_elementary_spinner.pxi"
include "elementary.c_elementary_notify.pxi"
include "elementary.c_elementary_fileselector.pxi"
include "elementary.c_elementary_fileselector_entry.pxi"
include "elementary.c_elementary_fileselector_button.pxi"
include "elementary.c_elementary_separator.pxi"
include "elementary.c_elementary_progressbar.pxi"
include "elementary.c_elementary_menu.pxi"
include "elementary.c_elementary_panel.pxi"
include "elementary.c_elementary_web.pxi"
include "elementary.c_elementary_actionslider.pxi"
include "elementary.c_elementary_calendar.pxi"
include "elementary.c_elementary_colorselector.pxi"
include "elementary.c_elementary_index.pxi"
include "elementary.c_elementary_ctxpopup.pxi"
include "elementary.c_elementary_grid.pxi"
include "elementary.c_elementary_video.pxi"
include "elementary.c_elementary_conformant.pxi"
include "elementary.c_elementary_dayselector.pxi"
include "elementary.c_elementary_panes.pxi"
include "elementary.c_elementary_thumb.pxi"
include "elementary.c_elementary_diskselector.pxi"
include "elementary.c_elementary_datetime.pxi"
include "elementary.c_elementary_map.pxi"
include "elementary.c_elementary_transit.pxi"
