# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri, Ulisses Furquim
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

from python_ref cimport PyObject, Py_INCREF, Py_DECREF
from python_mem cimport PyMem_Malloc, PyMem_Free
from python_string cimport PyString_FromStringAndSize

__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )

cdef int _ecore_events_registered = 0

cdef int PY_REFCOUNT(object o):
    cdef PyObject *obj = <PyObject *>o
    return obj.ob_refcnt

def init():
    r = ecore_init()
    global _ecore_events_registered
    if r > 0 and _ecore_events_registered == 0:
        _ecore_events_registered = 1

        global _event_type_mapping
        _event_type_mapping = {
            ECORE_EVENT_SIGNAL_USER: EventSignalUser,
            ECORE_EVENT_SIGNAL_HUP: EventSignalHup,
            ECORE_EVENT_SIGNAL_EXIT: EventSignalExit,
            ECORE_EVENT_SIGNAL_POWER: EventSignalPower,
            ECORE_EVENT_SIGNAL_REALTIME: EventSignalRealtime,
            ECORE_EXE_EVENT_ADD: EventExeAdd,
            ECORE_EXE_EVENT_DEL: EventExeDel,
            ECORE_EXE_EVENT_DATA: EventExeData,
            ECORE_EXE_EVENT_ERROR: EventExeData,
            }

    return r


def shutdown():
    return ecore_shutdown()


def main_loop_quit():
    "Quit main loop, have L{main_loop_begin()} to return."
    ecore_main_loop_quit()


def main_loop_begin():
    "Enter main loop, this function will not return until L{main_loop_quit()}."
    with nogil:
        ecore_main_loop_begin()

def main_loop_iterate():
    "Force main loop to process requests (timers, fd handlers, idlers, ...)"
    with nogil:
        ecore_main_loop_iterate()

def main_loop_glib_integrate():
    """Ask Ecore to integrate with GLib, running its default GMainContext.

    After this call, Ecore will act like GLib's main loop and also
    dispatch GLib's timers, fd-handlers and idlers. It makes possible
    to run Ecore-based applications with libraries that depends on
    GLib main loop, like GConf, GTK, GUPnP and others.

    @raises SystemError: if failed to integrate or no glib support.
    """
    if not ecore_main_loop_glib_integrate():
        raise SystemError("failed to integrate GLib main loop into ecore.")

def time_get():
    """Get current time, in seconds.

       @rtype: float
    """
    return ecore_time_get()


def loop_time_get():
    """Retrieves the time at which the last loop stopped waiting for
       timeouts or events.

       This gets the time (since Jan 1st, 1970, 12:00AM) that the main
       loop ceased waiting for timouts and/or events to come in or for
       signals or any other interrupt source. This should be
       considered a reference point fo all time based activity that
       should calculate its timepoint from the return of
       loop_time_get(). Use this UNLESS you absolutely must get the
       current actual timepoint - then use L{ecore.time_get()}. If
       this is called before any loop has ever been run, then it will
       call L{ecore.time_get()} for you the first time and thus have
       an initial time reference.

       @rtype: float
    """
    return ecore_loop_time_get()


def animator_frametime_set(double frametime):
    """Set time between frames (M{\frac{1}{frames-per-second}}).

    @parm: B{frametime} in seconds.
    """
    ecore_animator_frametime_set(frametime)


def animator_frametime_get():
    "@rtype: float"
    return ecore_animator_frametime_get()

include "ecore.c_ecore_timer.pxi"
include "ecore.c_ecore_animator.pxi"
include "ecore.c_ecore_idler.pxi"
include "ecore.c_ecore_idle_enterer.pxi"
include "ecore.c_ecore_idle_exiter.pxi"
include "ecore.c_ecore_fd_handler.pxi"
include "ecore.c_ecore_events.pxi"
include "ecore.c_ecore_exe.pxi"
