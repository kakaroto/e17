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

cimport evas.python as python

__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )

def init():
    return ecore_init()


def shutdown():
    return ecore_shutdown()


def main_loop_quit():
    "Quit main loop, have L{main_loop_begin()} to return."
    ecore_main_loop_quit()


def main_loop_begin():
    "Enter main loop, this function will not return until L{main_loop_quit()}."
    python.Py_BEGIN_ALLOW_THREADS
    ecore_main_loop_begin()
    python.Py_END_ALLOW_THREADS


def main_loop_iterate():
    "Force main loop to process requests (timers, fd handlers, idlers, ...)"
    python.Py_BEGIN_ALLOW_THREADS
    ecore_main_loop_iterate()
    python.Py_END_ALLOW_THREADS


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
