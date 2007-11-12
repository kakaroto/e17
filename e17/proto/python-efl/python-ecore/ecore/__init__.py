#!/usr/bin/env python2

import c_ecore

from c_ecore import shutdown, time_get, timer_add, \
     main_loop_begin, main_loop_quit, main_loop_iterate, event_handler_add, \
     animator_add, animator_frametime_set, animator_frametime_get, \
     idler_add, idle_enterer_add, idle_exiter_add, fd_handler_add, \
     Animator, Timer, Idler, IdleExiter, IdleEnterer, FdHandler, \
     Event, EventHandler


ECORE_FD_NONE = 0
ECORE_FD_READ = 1
ECORE_FD_WRITE = 2
ECORE_FD_ERROR = 4
ECORE_FD_ALL = 7


class MainLoop(object):
    @staticmethod
    def begin():
        main_loop_begin()

    @staticmethod
    def quit():
        main_loop_quit()

    @staticmethod
    def iterate():
        main_loop_iterate()

c_ecore.init()
