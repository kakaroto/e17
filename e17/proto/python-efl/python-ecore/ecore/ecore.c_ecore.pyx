cimport python

def init():
    return ecore_init()


def shutdown():
    return ecore_shutdown()


def main_loop_quit():
    ecore_main_loop_quit()


def main_loop_begin():
    ecore_main_loop_begin()


def main_loop_iterate():
    ecore_main_loop_iterate()


def time_get():
    return ecore_time_get()


def animator_frametime_set(double frametime):
    ecore_animator_frametime_set(frametime)


def animator_frametime_get():
    return ecore_animator_frametime_get()

include "ecore.c_ecore_timer.pxi"
include "ecore.c_ecore_animator.pxi"
include "ecore.c_ecore_idler.pxi"
include "ecore.c_ecore_idle_enterer.pxi"
include "ecore.c_ecore_idle_exiter.pxi"
include "ecore.c_ecore_fd_handler.pxi"
