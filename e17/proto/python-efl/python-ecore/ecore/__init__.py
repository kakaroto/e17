#!/usr/bin/env python2

import c_ecore

init = c_ecore.init
shutdown = c_ecore.shutdown

main_loop_begin = c_ecore.main_loop_begin
main_loop_quit = c_ecore.main_loop_quit
main_loop_iterate = c_ecore.main_loop_iterate

time_get = c_ecore.time_get
timer_add = c_ecore.timer_add

animator_add = c_ecore.animator_add
animator_frametime_set = c_ecore.animator_frametime_set
animator_frametime_get = c_ecore.animator_frametime_get

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


class Timer(c_ecore.Timer):
    def __new__(type, timeout, func, *args, **kargs):
        obj = timer_add(timeout, func, *args, **kargs)
        return obj


class Animator(c_ecore.Animator):
    def __new__(type, func, *args, **kargs):
        obj = animator_add(func, *args, **kargs)
        return obj
