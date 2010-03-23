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


cdef extern from "Ecore.h":
    ctypedef enum Ecore_Fd_Handler_Flags:
        ECORE_FD_READ = 1
        ECORE_FD_WRITE = 2
        ECORE_FD_ERROR = 4

    ctypedef enum Ecore_Exe_Flags:
        ECORE_EXE_PIPE_READ = 1
        ECORE_EXE_PIPE_WRITE = 2
        ECORE_EXE_PIPE_ERROR = 4
        ECORE_EXE_PIPE_READ_LINE_BUFFERED = 8
        ECORE_EXE_PIPE_ERROR_LINE_BUFFERED = 16
        ECORE_EXE_PIPE_AUTO = 32
        ECORE_EXE_RESPAWN = 64
        ECORE_EXE_USE_SH = 128
        ECORE_EXE_NOT_LEADER = 256

    ctypedef struct Ecore_Exe

    ctypedef struct Ecore_Exe_Event_Add:
        Ecore_Exe *exe
        void      *ext_data

    ctypedef struct Ecore_Exe_Event_Del:
        int          pid
        int          exit_code
        Ecore_Exe   *exe
        int          exit_signal
        unsigned int exited
        unsigned int signalled
        void        *ext_data

    ctypedef struct Ecore_Exe_Event_Data_Line:
        char *line
        int   size

    ctypedef struct Ecore_Exe_Event_Data:
        Ecore_Exe                 *exe
        void                      *data
        int                        size
        Ecore_Exe_Event_Data_Line *lines

    ctypedef struct Ecore_Event_Signal_User:
        int number

    ctypedef struct Ecore_Event_Signal_Hup

    ctypedef struct Ecore_Event_Signal_Exit:
        unsigned int interrupt
        unsigned int quit
        unsigned int terminate

    ctypedef struct Ecore_Event_Signal_Power

    ctypedef struct Ecore_Event_Signal_Realtime:
        int num

    int ECORE_EVENT_SIGNAL_USER
    int ECORE_EVENT_SIGNAL_HUP
    int ECORE_EVENT_SIGNAL_EXIT
    int ECORE_EVENT_SIGNAL_POWER
    int ECORE_EVENT_SIGNAL_REALTIME

    int ECORE_EXE_EVENT_ADD
    int ECORE_EXE_EVENT_DEL
    int ECORE_EXE_EVENT_DATA
    int ECORE_EXE_EVENT_ERROR

    cdef struct Ecore_Timer
    cdef struct Ecore_Animator
    cdef struct Ecore_Idler
    cdef struct Ecore_Idle_Enterer
    cdef struct Ecore_Idle_Exiter
    ctypedef struct Ecore_Fd_Handler
    ctypedef void Ecore_Event_Handler
    ctypedef struct Ecore_Event

    ctypedef struct Eina_List:
        void      *data
        Eina_List *next
        Eina_List *prev
        void      *accounting

    Eina_List *eina_list_free(Eina_List *list)

    int ecore_init()
    int ecore_shutdown()

    void ecore_main_loop_iterate() nogil
    void ecore_main_loop_begin() nogil
    void ecore_main_loop_quit()

    int ecore_main_loop_glib_integrate()

    double ecore_time_get()
    double ecore_loop_time_get()

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
    int ecore_event_type_new()
    Ecore_Event *ecore_event_add(int type, void *ev, void (*free_func)(void *data, void *ev), void *data)
    void *ecore_event_del(Ecore_Event *ev)

    void ecore_exe_run_priority_set(int pri)
    int ecore_exe_run_priority_get()

    Ecore_Exe *ecore_exe_pipe_run(char *exe_cmd, Ecore_Exe_Flags flags, void *data)
    void ecore_exe_callback_pre_free_set(Ecore_Exe *exe, void (*func)(void *data, Ecore_Exe *exe))

    int ecore_exe_send(Ecore_Exe *exe, void *data, int size)
    void ecore_exe_close_stdin(Ecore_Exe *exe)
    void ecore_exe_auto_limits_set(Ecore_Exe *exe, int start_bytes, int end_bytes, int start_lines, int end_lines)
    Ecore_Exe_Event_Data *ecore_exe_event_data_get(Ecore_Exe *exe, Ecore_Exe_Flags flags)
    void ecore_exe_event_data_free(Ecore_Exe_Event_Data *data)
    void *ecore_exe_free(Ecore_Exe *exe)
    int ecore_exe_pid_get(Ecore_Exe *exe)
    void ecore_exe_tag_set(Ecore_Exe *exe, char *tag)
    char *ecore_exe_tag_get(Ecore_Exe *exe)
    char *ecore_exe_cmd_get(Ecore_Exe *exe)
    void *ecore_exe_data_get(Ecore_Exe *exe)
    Ecore_Exe_Flags ecore_exe_flags_get(Ecore_Exe *exe)
    void ecore_exe_pause(Ecore_Exe *exe)
    void ecore_exe_continue(Ecore_Exe *exe)
    void ecore_exe_interrupt(Ecore_Exe *exe)
    void ecore_exe_quit(Ecore_Exe *exe)
    void ecore_exe_terminate(Ecore_Exe *exe)
    void ecore_exe_kill(Ecore_Exe *exe)
    void ecore_exe_signal(Ecore_Exe *exe, int num)
    void ecore_exe_hup(Ecore_Exe *exe)



cdef class Timer:
    cdef Ecore_Timer *obj
    cdef double _interval
    cdef readonly object func
    cdef readonly object args
    cdef readonly object kargs

    cdef object _exec(self)


cdef class Animator:
    cdef Ecore_Animator *obj
    cdef readonly object func
    cdef readonly object args
    cdef readonly object kargs

    cdef object _exec(self)


cdef class Idler:
    cdef Ecore_Idler *obj
    cdef readonly object func
    cdef readonly object args
    cdef readonly object kargs

    cdef object _exec(self)


cdef class IdleEnterer:
    cdef Ecore_Idle_Enterer *obj
    cdef readonly object func
    cdef readonly object args
    cdef readonly object kargs

    cdef object _exec(self)


cdef class IdleExiter:
    cdef Ecore_Idle_Exiter *obj
    cdef readonly object func
    cdef readonly object args
    cdef readonly object kargs

    cdef object _exec(self)


cdef class FdHandler:
    cdef Ecore_Fd_Handler *obj
    cdef readonly object func
    cdef readonly object args
    cdef readonly object kargs
    cdef readonly object _prepare_callback

    cdef object _exec(self)


cdef class ExeEventFilter:
    cdef Ecore_Exe *exe
    cdef Ecore_Event_Handler *handler
    cdef readonly object owner
    cdef readonly object event_type
    cdef object callbacks



cdef class Exe:
    cdef Ecore_Exe *exe
    cdef readonly object __data
    cdef object __callbacks

    cdef int _set_obj(self, char *exe_cmd, int flags) except 0
    cdef int _unset_obj(self) except 0


cdef public class Event [object PyEcoreEvent, type PyEcoreEvent_Type]:
    cdef int _set_obj(self, void *obj) except 0


cdef class EventSignalUser(Event):
    cdef readonly object number


cdef class EventSignalHup(Event):
    pass


cdef class EventSignalExit(Event):
    cdef readonly object interrupt
    cdef readonly object quit
    cdef readonly object terminate


cdef class EventSignalPower(Event):
    pass


cdef class EventSignalRealtime(Event):
    cdef readonly object num


cdef class EventExeAdd(Event):
    cdef readonly object exe


cdef class EventExeDel(Event):
    cdef readonly object exe
    cdef readonly object pid
    cdef readonly object exit_code
    cdef readonly object exit_signal
    cdef readonly object exited
    cdef readonly object signalled


cdef class EventExeData(Event):
    cdef readonly object exe
    cdef readonly object data
    cdef readonly object size
    cdef readonly object lines


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


cdef class EventHandlerSignalUser(EventHandler):
    pass


cdef class EventHandlerSignalExit(EventHandler):
    pass


cdef class EventHandlerExe(EventHandler):
    pass

cdef class CustomEvent(Event):
    cdef readonly object obj

cdef class QueuedEvent:
    cdef Ecore_Event *obj
    cdef readonly object args

    cdef int _set_obj(self, Ecore_Event *ev) except 0
    cdef int _unset_obj(self) except 0
