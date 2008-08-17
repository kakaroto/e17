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

cdef extern from "evas/python_evas_utils.h":
    int PY_REFCOUNT(object)


cdef extern from "Ecore.h":
    ctypedef enum Ecore_Fd_Handler_Flags:
        ECORE_FD_READ = 1
        ECORE_FD_WRITE = 2
        ECORE_FD_ERROR = 4


    cdef struct Ecore_Timer
    cdef struct Ecore_Animator
    cdef struct Ecore_Idler
    cdef struct Ecore_Idle_Enterer
    cdef struct Ecore_Idle_Exiter
    ctypedef struct Ecore_Fd_Handler
    ctypedef void Ecore_Event_Handler

    int ecore_init()
    int ecore_shutdown()

    void ecore_main_loop_iterate()
    void ecore_main_loop_begin()
    void ecore_main_loop_quit()

    double ecore_time_get()

    Ecore_Timer *ecore_timer_add(double t, int (*func) (void *data), void *data)
    void *ecore_timer_del(Ecore_Timer *timer)
    void ecore_timer_interval_set(Ecore_Timer *timer, double t)

    Ecore_Animator *ecore_animator_add(int (*func) (void *data), void *data)
    void *ecore_animator_del(Ecore_Animator *animator)
    void ecore_animator_frametime_set(double frametime)
    double ecore_animator_frametime_get()

    Ecore_Idler *ecore_idler_add(int (*func) (void *data), void *data)
    void *ecore_idler_del(Ecore_Idler *idler)

    Ecore_Idle_Enterer *ecore_idle_enterer_add(int (*func) (void *data), void *data)
    void *ecore_idle_enterer_del(Ecore_Idle_Enterer *idle_enterer)

    Ecore_Idle_Exiter *ecore_idle_exiter_add(int (*func) (void *data), void *data)
    void *ecore_idle_exiter_del(Ecore_Idle_Exiter *idle_exiter)

    Ecore_Fd_Handler *ecore_main_fd_handler_add(int fd, Ecore_Fd_Handler_Flags flags, int (*func) (void *data, Ecore_Fd_Handler *fd_handler), void *data, int (*buf_func) (void *buf_data, Ecore_Fd_Handler *fd_handler), void *buf_data)
    void ecore_main_fd_handler_prepare_callback_set(Ecore_Fd_Handler *fd_handler, void (*func) (void *data, Ecore_Fd_Handler *fd_handler), void *data)
    void *ecore_main_fd_handler_del(Ecore_Fd_Handler *fd_handler)
    int ecore_main_fd_handler_fd_get(Ecore_Fd_Handler *fd_handler)
    int ecore_main_fd_handler_active_get(Ecore_Fd_Handler *fd_handler, Ecore_Fd_Handler_Flags flags)
    void ecore_main_fd_handler_active_set(Ecore_Fd_Handler *fd_handler, Ecore_Fd_Handler_Flags flags)

    Ecore_Event_Handler *ecore_event_handler_add(int type, int (*func) (void *data, int type, void *event), void *data)
    void *ecore_event_handler_del(Ecore_Event_Handler *event_handler)



cdef class Timer:
    cdef Ecore_Timer *obj
    cdef double _interval
    cdef object func
    cdef object args
    cdef object kargs

    cdef object _exec(self)


cdef class Animator:
    cdef Ecore_Animator *obj
    cdef object func
    cdef object args
    cdef object kargs

    cdef object _exec(self)


cdef class Idler:
    cdef Ecore_Idler *obj
    cdef object func
    cdef object args
    cdef object kargs

    cdef object _exec(self)


cdef class IdleEnterer:
    cdef Ecore_Idle_Enterer *obj
    cdef object func
    cdef object args
    cdef object kargs

    cdef object _exec(self)


cdef class IdleExiter:
    cdef Ecore_Idle_Exiter *obj
    cdef object func
    cdef object args
    cdef object kargs

    cdef object _exec(self)


cdef class FdHandler:
    cdef Ecore_Fd_Handler *obj
    cdef object func
    cdef object args
    cdef object kargs
    cdef object _prepare_callback

    cdef object _exec(self)


cdef public class Event [object PyEcoreEvent, type PyEcoreEvent_Type]:
    cdef int _set_obj(self, void *obj) except 0


cdef class EventHandler:
    cdef Ecore_Event_Handler *obj
    cdef readonly int type
    cdef readonly object event_cls
    cdef readonly object func
    cdef readonly object args
    cdef readonly object kargs

    cdef int _set_obj(self, Ecore_Event_Handler *obj) except 0
    cdef int _unset_obj(self) except 0
    cdef int _exec(self, void *event) except 2
