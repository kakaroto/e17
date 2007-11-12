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
