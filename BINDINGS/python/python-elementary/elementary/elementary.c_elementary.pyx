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
            (caller_module, caller_line, caller_code, self.__class__.__name__)
    else:
        msg = "%s:%s %s.%s() is deprecated." % \
            (caller_module, caller_line, self.__class__.__name__, caller_name)
    if replacement:
        msg += " Use %s() instead." % (replacement,)
    if message:
        msg += " " + message
    logging.warn(msg)


def init():
    cdef int argc, i, arg_len
    cdef char **argv, *arg
    argc_orig = argc = len(sys.argv)
    argv = <char **>PyMem_Malloc(argc * sizeof(char *))
    for i from 0 <= i < argc:
        arg = sys.argv[i]
        arg_len = len(sys.argv[i])
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

   @param policy policy identifier as in Elm_Policy.
   @param value policy value, depends on identifiers, usually there is
          an enumeration with the same prefix as the policy name, for
          example: ELM_POLICY_QUIT and Elm_Policy_Quit
          (ELM_POLICY_QUIT_NONE, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED).

   @return @c EINA_TRUE on success or @c EINA_FALSE on error (right
           now just invalid policy identifier, but in future policy
           value might be enforced).
    """
    return elm_policy_set(policy, value)

def policy_get(policy):
    """Gets the policy value set for given identifier.

    @param policy policy identifier as in Elm_Policy.

    @return policy value. Will be 0 if policy identifier is invalid.
    """
    return elm_policy_get(policy)

def scale_get():
    return elm_scale_get()

def scale_set(scale):
    elm_scale_set(scale)

def finger_size_get():
    return elm_finger_size_get()

def finger_size_set(size):
    elm_finger_size_set(size)

def tooltip_delay_get():
    return elm_tooltip_delay_get()

def tooltip_delay_set(delay):
    elm_tooltip_delay_set(delay)

def cursor_engine_only_get():
    return elm_cursor_engine_only_get()

def cursor_engine_only_set(engine_only):
    elm_cursor_engine_only_set(engine_only)

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

def focus_highlight_enabled_get():
    return elm_focus_highlight_enabled_get()

def focus_highlight_enabled_set(enabled):
    elm_focus_highlight_enabled_set(enabled)

def focus_highlight_animate_get():
    return elm_focus_highlight_animate_get()

def focus_highlight_animate_set(animate):
    elm_focus_highlight_animate_set(animate)

cdef object _elm_widget_type_mapping

_elm_widget_type_mapping = {}

cdef _elm_widget_type_register(char *name, cls):
    if name in _elm_widget_type_mapping:
        raise ValueError("object type name '%s' already registered." % name)
    _elm_widget_type_mapping[name] = cls

cdef _elm_widget_type_unregister(char *name):
    _elm_widget_type_mapping.pop(name)


include "elementary.c_elementary_object.pxi"
include "elementary.c_elementary_widget_item.pxi"
include "elementary.c_elementary_window.pxi"
include "elementary.c_elementary_innerwindow.pxi"
include "elementary.c_elementary_background.pxi"
include "elementary.c_elementary_icon.pxi"
include "elementary.c_elementary_box.pxi"
include "elementary.c_elementary_frame.pxi"
include "elementary.c_elementary_flip.pxi"
include "elementary.c_elementary_button.pxi"
include "elementary.c_elementary_scroller.pxi"
include "elementary.c_elementary_label.pxi"
include "elementary.c_elementary_toggle.pxi"
include "elementary.c_elementary_table.pxi"
include "elementary.c_elementary_clock.pxi"
include "elementary.c_elementary_layout.pxi"
include "elementary.c_elementary_hover.pxi"
include "elementary.c_elementary_entry.pxi"
include "elementary.c_elementary_scrolled_entry.pxi"
include "elementary.c_elementary_anchorview.pxi"
include "elementary.c_elementary_anchorblock.pxi"
include "elementary.c_elementary_bubble.pxi"
include "elementary.c_elementary_photo.pxi"
include "elementary.c_elementary_hoversel.pxi"
include "elementary.c_elementary_toolbar.pxi"
include "elementary.c_elementary_list.pxi"
include "elementary.c_elementary_slider.pxi"
include "elementary.c_elementary_pager.pxi"
include "elementary.c_elementary_radio.pxi"
include "elementary.c_elementary_check.pxi"
include "elementary.c_elementary_image.pxi"
include "elementary.c_elementary_genlist.pxi"
include "elementary.c_elementary_gengrid.pxi"
include "elementary.c_elementary_spinner.pxi"
include "elementary.c_elementary_notify.pxi"
include "elementary.c_elementary_fileselector.pxi"
include "elementary.c_elementary_separator.pxi"
include "elementary.c_elementary_progressbar.pxi"
include "elementary.c_elementary_menu.pxi"
include "elementary.c_elementary_panel.pxi"
