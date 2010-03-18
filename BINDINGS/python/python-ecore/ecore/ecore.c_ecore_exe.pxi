# Copyright (C) 2010 Gustavo Sverzut Barbieri
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

# This file is included verbatim by c_ecore.pyx

import traceback

# TODO: remove me after usage is update to new buffer api
cdef extern from "Python.h":
    int PyObject_AsReadBuffer(obj, void **buffer, Py_ssize_t *buffer_len) except -1


cdef exe_flags2str(int value):
    flags = []
    if value & ECORE_EXE_PIPE_READ:
        flags.append("PIPE_READ")
    if value & ECORE_EXE_PIPE_WRITE:
        flags.append("PIPE_WRITE")
    if value & ECORE_EXE_PIPE_ERROR:
        flags.append("PIPE_ERROR")
    if value & ECORE_EXE_PIPE_READ_LINE_BUFFERED:
        flags.append("PIPE_READ_LINE_BUFFERED")
    if value & ECORE_EXE_PIPE_ERROR_LINE_BUFFERED:
        flags.append("PIPE_ERROR_LINE_BUFFERED")
    if value & ECORE_EXE_PIPE_AUTO:
        flags.append("PIPE_AUTO")
    if value & ECORE_EXE_RESPAWN:
        flags.append("RESPAWN")
    if value & ECORE_EXE_USE_SH:
        flags.append("USE_SH")
    if value & ECORE_EXE_NOT_LEADER:
        flags.append("NOT_LEADER")
    return ", ".join(flags)


cdef int _exe_event_filter_cb(void *data, int type, void *event) with gil:
    cdef ExeEventFilter self = <ExeEventFilter>data
    cdef Ecore_Exe_Event_Add *e_add
    cdef Ecore_Exe_Event_Del *e_del
    cdef Ecore_Exe_Event_Data *e_data
    cdef Event e

    try:
        assert self.event_type == type, "event is not what we asked? impossible"
        if type == ECORE_EXE_EVENT_ADD:
            e_add = <Ecore_Exe_Event_Add *>event
            if e_add.exe != self.exe:
                return 1
            e = EventExeAdd()
        elif type == ECORE_EXE_EVENT_DEL:
            e_del = <Ecore_Exe_Event_Del *>event
            if e_del.exe != self.exe:
                return 1
            e = EventExeDel()
        elif type == ECORE_EXE_EVENT_DATA or type == ECORE_EXE_EVENT_ERROR:
            e_data = <Ecore_Exe_Event_Data *>event
            if e_data.exe != self.exe:
                return 1
            e = EventExeData()
        else:
            raise SystemError("unknown event type=%d" % type)

        r = e._set_obj(event)
        assert r != -1, "exe is not known?! impossible!"

        cb = tuple(self.callbacks) # copy, so we can change self.callbacks
        for func, args, kargs in cb:
            try:
                func(self.owner, e, *args, **kargs)
            except:
                traceback.print_exc()

    except:
        traceback.print_exc()

    return 1 # always return true, no matter what


cdef class ExeEventFilter:
    def __new__(self, *a, **ka):
        self.exe = NULL
        self.handler = NULL
        self.owner = None
        self.event_type = -1
        self.callbacks = []

    def __dealloc__(self):
        if self.handler != NULL:
            ecore_event_handler_del(self.handler)
            self.handler = NULL

        self.exe = NULL
        self.owner = None
        self.event_type = None
        self.callbacks = None

    def __init__(self, Exe exe not None, int event_type):
        self.exe = exe.exe
        self.owner = exe
        self.event_type = event_type
        self.callbacks = []

    def delete(self):
        if self.handler != NULL:
            ecore_event_handler_del(self.handler)
            self.handler = NULL
        self.callbacks = None

    def callback_add(self, func, args, kargs):
        if self.handler == NULL:
            self.handler = ecore_event_handler_add(
                self.event_type, _exe_event_filter_cb, <void *>self)
        self.callbacks.append((func, args, kargs))

    def callback_del(self, func, args, kargs):
        try:
            self.callbacks.remove((func, args, kargs))
        except ValueError, e:
            raise ValueError(
                "callback is not registered: %s, args=%s, kargs=%s" %
                (func, args, kargs))

        if self.callbacks:
            return

        if self.handler != NULL:
            ecore_event_handler_del(self.handler)
            self.handler = NULL



def exe_run_priority_set(int pri):
    """Sets the priority at which to launch processes.

    This sets the priority of processes run by L{Exe}. If
    set to L{ECORE_EXE_PRIORITY_INHERIT} child processes inherit the
    priority of their parent. This is the default.

    @parm: B{pri} value -20 to 19 or L{ECORE_EXE_PRIORITY_INHERIT}
    """
    ecore_exe_run_priority_set(pri)


def exe_run_priority_get():
    """Gets the priority at which to launch processes.

    This gets ths priority of launched processes. See
    L{exe_run_priority_set()} for details. This just returns the value
    set by this call.

    @return: the value set by L{exe_run_priority_set()}
    """
    return ecore_exe_run_priority_get()


cdef object _ecore_exe_event_mapping
_ecore_exe_event_mapping = {}


cdef void _ecore_exe_pre_free_cb(void *data, Ecore_Exe *exe) with gil:
    cdef Exe obj
    try:
        if data == NULL:
            raise ValueError("data parameter is NULL")
        else:
            obj = <Exe>data
            obj._unset_obj()
    except Exception, e:
        traceback.print_exc()


cdef class Exe:
    """Spawns a child process with its stdin/out available for communication.

    This function forks and runs the given command using C{/bin/sh}.

    Note that the process handle is only valid until a child process
    terminated event is received.  After all handlers for the child
    process terminated event have been called, the handle will be
    freed by Ecore. In this case the Python wrapper becames "shallow"
    and all operations will fail or return bogus/dummy values,
    although it should not crash.

    This class behavior is configurable by means of given constructor
    flags, that will make Ecore monitor process' stdout and stderr,
    emitting events on main loop.

    To write use L{send()}.  To read listen to C{ECORE_EXE_EVENT_DATA}
    or C{ECORE_EXE_EVENT_ERROR} events (see below). Ecore may
    buffer read and error data until a newline character if asked for
    with the @C{flags}.  All data will be included in the events
    (newlines will be replaced with NULLS if line is buffered).

    C{ECORE_EXE_EVENT_DATA} events will only happen if the process is
    run with L{ECORE_EXE_PIPE_READ} enabled in the C{flags}.  The same
    with the error version.  Writing will only be allowed with
    L{ECORE_EXE_PIPE_WRITE} enabled in the C{flags}.

    Instance Event Handling
    =======================

    To make use easier, there are methods that automatically filter
    events for this instance and deletes theme when the L{Exe} is
    deleted:

     - L{on_add_event_add()}
     - L{on_add_event_del()}
     - L{on_del_event_add()}
     - L{on_del_event_del()}
     - L{on_data_event_add()}
     - L{on_data_event_del()}
     - L{on_error_event_add()}
     - L{on_error_event_del()}

    The callback signatures are:

            C{func(exe, event, *args, **kargs)}

    In contrast with C-api conformant functions. This only receives
    the events from this exact exe instance. The signature is also
    very different, the first parameter is the L{Exe} reference and
    the return value does B{not} removes the event listener!

    Using this method is likely more efficient than the C-api since it
    will not convert from C to Python lots of times, possibly useless.

    However, there are C-api conformat functions as well.

    Event Handling (C-api conformant)
    =================================

    Getting data from executed processed is done by means of event
    handling, which is also used to notify whenever this process
    really started or died.

    One should listen to events in the main loop, such as:

     - L{EventExeAdd} listen with L{on_exe_add_event_add()} to know
       when sub processes were started and ready to be used.

     - L{EventExeDel} listen with L{on_exe_del_event_add()} to know
       when sub processes died.

     - L{EventExeData} listen with L{on_exe_data_event_add()} to know
       when sub processes output data to their stdout.

     - L{EventExeError} listen with L{on_exe_error_event_add()} to
       know when sub processes output data to their stderr.

    Events will have the following signature, as explained in
    L{EventHandler}:

       C{func(event, *args, **kargs): bool}

    That mean once registered, your callback C{func} will be called
    for all known L{Exe} instances (that were created from
    Python!). You can query which instance created such event with
    C{event.exe} property. Thus you often need to filter if the event
    you got is from the instance you need! (This is designed to match
    C-api).

    Once your function returns evaluates to False (note: not returning
    means returning None, that evaluates to False!), your callback
    will not be called anymore and your handler is deleted.

    One may delete handlers explicitly with L{EventHandler.delete()}
    method.
    """
    def __new__(self, *a, **ka):
        self.exe = NULL
        self.__data = None
        self.__callbacks = {}

    def __init__(self, char *exe_cmd, int flags=0, data=None):
        """Constructor.

        @parm: B{exe_cmd} command to execute as subprocess.
        @parm: B{flags} if given (!= 0), should be bitwise OR of:
	 - ECORE_EXE_PIPE_READ: Exe Pipe Read mask
         - ECORE_EXE_PIPE_WRITE: Exe Pipe Write mask
         - ECORE_EXE_PIPE_ERROR: Exe Pipe error mask
         - ECORE_EXE_PIPE_READ_LINE_BUFFERED: Reads are buffered until
           a newline and delivered 1 event per line.
         - ECORE_EXE_PIPE_ERROR_LINE_BUFFERED: Errors are buffered
           until a newline and delivered 1 event per line
	 - ECORE_EXE_PIPE_AUTO: stdout and stderr are buffered automatically
         - ECORE_EXE_RESPAWN: Exe is restarted if it dies
         - ECORE_EXE_USE_SH: Use /bin/sh to run the command.
         - ECORE_EXE_NOT_LEADER Do not use setsid() to have the
           executed process be its own session leader
        @parm: B{data} extra data to be associated and available with
               L{data_get()}
        """
        if not exe_cmd:
            raise ValueError("exe_cmd must not be empty!")

        if flags is None:
            flags = 0

        self._set_obj(exe_cmd, flags)
        self.__data = data
        self.__callbacks = {}

    cdef int _set_obj(self, char *exe_cmd, int flags) except 0:
        cdef Ecore_Exe *exe

        assert self.exe == NULL, "Exe must be clean, not wrapping any Ecore_Exe"

        exe = ecore_exe_pipe_run(exe_cmd, <Ecore_Exe_Flags>flags, <void *>self)
        if exe == NULL:
            raise SystemError("could not run subprocess %r, flags=%#x" %
                              (exe_cmd, flags))

        Py_INCREF(self)
        self.exe = exe
        ecore_exe_callback_pre_free_set(exe, _ecore_exe_pre_free_cb)
        _ecore_exe_event_mapping[<long><void *>exe] = self
        return 1

    cdef int _unset_obj(self) except 0:
        assert self.exe != NULL, "Exe must wrap something"
        for t, filter in self.__callbacks.iteritems():
            filter.delete()
        self.__callbacks = None

        _ecore_exe_event_mapping.pop(<long><void *>self.exe)
        self.exe = NULL
        Py_DECREF(self)
        return 1

    def __str__(self):
        if self.exe == NULL:
            pid = None
            cmd = None
            flags = ""
            data = None
        else:
            pid = self.pid
            cmd = self.cmd
            flags = exe_flags2str(self.flags)
            data = None
        return "%s(pid=%s, cmd=%r, flags=[%s], data=%r)" % \
            (self.__class__.__name__, pid, cmd, flags, data)

    def __repr__(self):
        if self.exe == NULL:
            pid = None
            cmd = None
            flags = ""
            data = None
        else:
            pid = self.pid
            cmd = self.cmd
            flags = exe_flags2str(self.flags)
            data = None
        return ("%s(%#x, Ecore_Exe=%#x, refcount=%d, pid=%s, cmd=%r, "
                "flags=[%s], data=%r)") % \
                (self.__class__.__name__, <unsigned long><void *>self,
                 <unsigned long>self.exe, PY_REFCOUNT(self),
                 pid, cmd, flags, data)

    def delete(self):
        """Forcefully frees the given process handle.

        Note that the process that the handle represents is unaffected
        by this function, this just stops monitoring the stdout/stderr
        and emitting related events.

        To finish the process call L{terminate()} or L{kill()}.
        """
        if self.exe == NULL:
            raise ValueError("%s already deleted" % self.__class__.__name__)
        ecore_exe_free(self.exe)

    def free(self):
        """Alias for L{delete()} to keep compatibility with C-api."""
        self.delete()

    def send(self, buffer, long size=0):
        """Sends data to the executed process, which it recieves on stdin.

        This function writes to a child processes standard in, with
        unlimited buffering. This call will never block. It may fail
        if the system runs out of memory.

        @parm: B{buffer} object that implements buffer interface, such
               as strings (str).
        @parm: B{size} if greater than zero, then this will limit the
               size of given buffer. If None, then the exact buffer
               size is used.

        @raise ValueError: if size is larger than buffer size.
        @return: bool with success or failure.
        """
        cdef void *b_data
        cdef Py_ssize_t b_size

        # TODO: update to new buffer api
        PyObject_AsReadBuffer(buffer, &b_data, &b_size)
        if size <= 0:
            size = b_size
        elif size > b_size:
            raise ValueError(
                "given size (%d) is larger than buffer size (%d)." %
                (size, b_size))

        return bool(ecore_exe_send(self.exe, b_data, size))

    def close_stdin(self):
        """Close executed process' stdin.

        The stdin of the given child process will not be closed
        immediately. Instead it will be closed when the write buffer
        is empty.
        """
        ecore_exe_close_stdin(self.exe)

    def auto_limits_set(self, int start_bytes, int end_bytes,
                        int start_lines, int end_lines):
        """Sets the auto pipe limits for the given process handle

        @parm: B{start_bytes} limit of bytes at start of output to buffer.
        @parm: B{end_bytes} limit of bytes at end of output to buffer.
        @parm: B{start_lines} limit of lines at start of output to buffer.
        @parm: B{end_lines} limit of lines at end of output to buffer.
        """
        ecore_exe_auto_limits_set(self.exe, start_bytes, end_bytes,
                                  start_lines, end_lines)

    def event_data_get(self, int flags):
        pass
        # TODO:
        #Ecore_Exe_Event_Data *ecore_exe_event_data_get(Ecore_Exe *exe, Ecore_Exe_Flags flags)
        #void ecore_exe_event_data_free(Ecore_Exe_Event_Data *data)

    def cmd_get(self):
        """Retrieves the command of the executed process.

        @rtype: str or None
        @return: the command line string if execution succeeded, None otherwise.
        """
        cdef char *cmd = ecore_exe_cmd_get(self.exe)
        if cmd != NULL:
            return cmd
        return None

    property cmd:
        def __get__(self):
            return self.cmd_get()

    def pid_get(self):
        """Retrieves the process ID of the executed process.

        @rtype: int
        """
        return ecore_exe_pid_get(self.exe)

    property pid:
        def __get__(self):
            return self.pid_get()

    def tag_set(self, char *tag):
        """Sets the string tag for the given process.


        This is a string that is attached to this handle and may serve
        as further information.

        @note: not much useful in Python, but kept for compatibility
               with C-api.
        """
        cdef char *s
        if tag is None:
            s = NULL
        else:
            s = tag
        ecore_exe_tag_set(self.exe, s)

    def tag_get(self):
        """Retrieves the tag attached to the given process.

        This is a string that is attached to this handle and may serve
        as further information.

        @note: not much useful in Python, but kept for compatibility
               with C-api.

        @rtype: str or None
        """
        cdef char *tag = ecore_exe_tag_get(self.exe)
        if tag != NULL:
            return tag
        return None

    property tag:
        def __set__(self, char *tag):
            self.tag_set(tag)

        def __get__(self):
            return self.tag_get()

    def data_get(self):
        return self.__data

    property data:
        def __get__(self):
            return self.data_get()

    def flags_get(self):
        """Retrieves the flags attached to the given process handle.

	 - ECORE_EXE_PIPE_READ: Exe Pipe Read mask
         - ECORE_EXE_PIPE_WRITE: Exe Pipe Write mask
         - ECORE_EXE_PIPE_ERROR: Exe Pipe error mask
         - ECORE_EXE_PIPE_READ_LINE_BUFFERED: Reads are buffered until
           a newline and delivered 1 event per line.
         - ECORE_EXE_PIPE_ERROR_LINE_BUFFERED: Errors are buffered
           until a newline and delivered 1 event per line
	 - ECORE_EXE_PIPE_AUTO: stdout and stderr are buffered automatically
         - ECORE_EXE_RESPAWN: Exe is restarted if it dies
         - ECORE_EXE_USE_SH: Use /bin/sh to run the command.
         - ECORE_EXE_NOT_LEADER Do not use setsid() to have the
           executed process be its own session leader

        @return: set of masks, ORed.
        """
        return ecore_exe_flags_get(self.exe)

    property flags:
        def __get__(self):
            return self.flags_get()

    def signal(self, int num):
        """Send SIGUSR1 or SIGUSR2 to executed process.

        @parm: B{num} user signal number, either 1 or 2.

        @see: POSIX kill(2) and kill(1) man pages.
        @raise ValueError: if num is not 1 or 2.
        """
        if num not in (1, 2):
            raise ValueError("num must be either 1 or 2. Got %d." % num)
        ecore_exe_signal(self.exe, num)

    def pause(self):
        """Send pause signal (SIGSTOP) to executed process.

        In order to resume application execution, use L{continue_()}
        """
        ecore_exe_pause(self.exe)

    def stop(self):
        """Alias for L{pause()}"""
        self.pause()

    def continue_(self):
        """Send contine signal (SIGCONT) to executed process.

        This resumes application previously paused with L{pause()}

        @see: L{pause()}
        """
        ecore_exe_continue(self.exe)

    def resume(self):
        """Alias for L{continue_()}"""
        self.continue_()

    def interrupt(self):
        """Send interrupt signal (SIGINT) to executed process.

        @note: Python usually installs SIGINT hanlder to generate
               C{KeyboardInterrupt}, however Ecore will B{override}
               this handler with its own that generates
               C{ECORE_EVENT_SIGNAL_EXIT} in its main loop for the
               application to handle. Pay attention to this detail if
               your B{child} process is also using Ecore.
        """
        ecore_exe_interrupt(self.exe)

    def quit(self):
        """Send quit signal (SIGQUIT) to executed process."""
        ecore_exe_quit(self.exe)

    def terminate(self):
        """Send terminate signal (SIGTERM) to executed process."""
        ecore_exe_terminate(self.exe)

    def kill(self):
        """Send kill signal (SIGKILL) to executed process.

        This signal is fatal and will exit the application as it
        cannot be blocked.
        """
        ecore_exe_kill(self.exe)

    def hup(self):
        """Send hup signal (SIGHUP) to executed process."""
        ecore_exe_hup(self.exe)

    def on_add_event_add(self, func, *args, **kargs):
        """Adds event listener to know when this Exe was actually started.

        The given function will be called with the following signature
        every time this Exe receives an ECORE_EXE_EVENT_ADD signal:

            C{func(exe, event, *args, **kargs)}

        In contrast with L{on_exe_add_event_add()}, this only receives
        the events from this exact exe instance. The signature is also
        very different, the first parameter is the L{Exe} reference
        and the return value does B{not} removes the event listener!

        @see: on_add_event_del()
        @see: on_exe_add_event_add()
        """
        filter = self.__callbacks.get(ECORE_EXE_EVENT_ADD)
        if filter is None:
            filter = ExeEventFilter(self, ECORE_EXE_EVENT_ADD)
            self.__callbacks[ECORE_EXE_EVENT_ADD] = filter
        filter.callback_add(func, args, kargs)

    def on_add_event_del(self, func, *args, **kargs):
        """Removes the event listener registered with L{on_add_event_add()}.

        Parameters must be exactly the same.

        @raise ValueError: if parameters don't match an already
               registered callback.
        """
        filter = self.__callbacks.get(ECORE_EXE_EVENT_ADD)
        if filter is None:
            raise ValueError("callback not registered %s, args=%s, kargs=%s" %
                             (func, args, kargs))
        filter.callback_del(func, args, kargs)

    def on_del_event_add(self, func, *args, **kargs):
        """Adds event listener to know when this Exe was actually started.

        The given function will be called with the following signature
        every time this Exe receives an ECORE_EXE_EVENT_DEL signal:

            C{func(exe, event, *args, **kargs)}

        In contrast with L{on_exe_del_event_add()}, this only receives
        the events from this exact exe instance. The signature is also
        very different, the first parameter is the L{Exe} reference
        and the return value does B{not} removes the event listener!

        @see: on_del_event_del()
        @see: on_exe_del_event_add()
        """
        filter = self.__callbacks.get(ECORE_EXE_EVENT_DEL)
        if filter is None:
            filter = ExeEventFilter(self, ECORE_EXE_EVENT_DEL)
            self.__callbacks[ECORE_EXE_EVENT_DEL] = filter
        filter.callback_add(func, args, kargs)

    def on_del_event_del(self, func, *args, **kargs):
        """Removes the event listener registered with L{on_del_event_add()}.

        Parameters must be exactly the same.

        @raise ValueError: if parameters don't match an already
               registered callback.
        """
        filter = self.__callbacks.get(ECORE_EXE_EVENT_DEL)
        if filter is None:
            raise ValueError("callback not registered %s, args=%s, kargs=%s" %
                             (func, args, kargs))
        filter.callback_del(func, args, kargs)

    def on_data_event_add(self, func, *args, **kargs):
        """Adds event listener to know when this Exe was actually started.

        The given function will be called with the following signature
        every time this Exe receives an ECORE_EXE_EVENT_DATA signal:

            C{func(exe, event, *args, **kargs)}

        In contrast with L{on_exe_data_event_add()}, this only receives
        the events from this exact exe instance. The signature is also
        very different, the first parameter is the L{Exe} reference
        and the return value does B{not} removes the event listener!

        @see: on_data_event_del()
        @see: on_exe_data_event_add()
        """
        filter = self.__callbacks.get(ECORE_EXE_EVENT_DATA)
        if filter is None:
            filter = ExeEventFilter(self, ECORE_EXE_EVENT_DATA)
            self.__callbacks[ECORE_EXE_EVENT_DATA] = filter
        filter.callback_add(func, args, kargs)

    def on_data_event_del(self, func, *args, **kargs):
        """Removes the event listener registered with L{on_data_event_add()}.

        Parameters must be exactly the same.

        @raise ValueError: if parameters don't match an already
               registered callback.
        """
        filter = self.__callbacks.get(ECORE_EXE_EVENT_DATA)
        if filter is None:
            raise ValueError("callback not registered %s, args=%s, kargs=%s" %
                             (func, args, kargs))
        filter.callback_del(func, args, kargs)

    def on_error_event_add(self, func, *args, **kargs):
        """Adds event listener to know when this Exe was actually started.

        The given function will be called with the following signature
        every time this Exe receives an ECORE_EXE_EVENT_ERROR signal:

            C{func(exe, event, *args, **kargs)}

        In contrast with L{on_exe_error_event_add()}, this only receives
        the events from this exact exe instance. The signature is also
        very different, the first parameter is the L{Exe} reference
        and the return value does B{not} removes the event listener!

        @see: on_error_event_del()
        @see: on_exe_error_event_add()
        """
        filter = self.__callbacks.get(ECORE_EXE_EVENT_ERROR)
        if filter is None:
            filter = ExeEventFilter(self, ECORE_EXE_EVENT_ERROR)
            self.__callbacks[ECORE_EXE_EVENT_ERROR] = filter
        filter.callback_add(func, args, kargs)

    def on_error_event_del(self, func, *args, **kargs):
        """Removes the event listener registered with L{on_error_event_add()}.

        Parameters must be exactly the same.

        @raise ValueError: if parameters don't match an already
               registered callback.
        """
        filter = self.__callbacks.get(ECORE_EXE_EVENT_ERROR)
        if filter is None:
            raise ValueError("callback not registered %s, args=%s, kargs=%s" %
                             (func, args, kargs))
        filter.callback_del(func, args, kargs)


def exe_run(char *exe_cmd, data=None):
    """L{Exe} factory, for C-api compatibility."""
    return Exe(exe_cmd, None, data)


def exe_pipe_run(char *exe_cmd, int flags=0, data=None):
    """L{Exe} factory, for C-api compatibility."""
    return Exe(exe_cmd, flags, data)


cdef class EventExeAdd(Event):
    """"Represents Ecore_Exe_Event_Add event from C-api.

    This event notifies the process created with L{Exe} was started.

    See property C{exe} for L{Exe} instance.
    """
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_Exe_Event_Add *obj
        obj = <Ecore_Exe_Event_Add*>o
        self.exe = _ecore_exe_event_mapping.get(<long>obj.exe)
        if self.exe is None:
            return -1
        return 1

    def __str__(self):
        return "%s(exe=%s)" % (self.__class__.__name__, self.exe)

    def __repr__(self):
        return "%s(exe=%r)" % (self.__class__.__name__, self.exe)


cdef class EventExeDel(Event):
    """"Represents Ecore_Exe_Event_Del from C-api.

    This event notifies the process created with L{Exe} is now dead.

    See property C{exe} for L{Exe} instance.
    """
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_Exe_Event_Del *obj
        obj = <Ecore_Exe_Event_Del*>o
        self.exe = _ecore_exe_event_mapping.get(<long>obj.exe)
        if self.exe is None:
            return -1
        self.pid = obj.pid
        self.exit_code = obj.exit_code
        self.exit_signal = obj.exit_signal
        self.exited = bool(obj.exited)
        self.signalled = bool(obj.signalled)
        return 1

    def __str__(self):
        return ("%s(pid=%s, exit_code=%s, exit_signal=%s, exited=%s, "
                "signalled=%s, exe=%s)") % \
                (self.__class__.__name__, self.pid, self.exit_code,
                 self.exit_signal, self.exited, self.signalled, self.exe)

    def __repr__(self):
        return ("%s(pid=%s, exit_code=%s, exit_signal=%s, exited=%s, "
                "signalled=%s, exe=%r)") % \
                (self.__class__.__name__, self.pid, self.exit_code,
                 self.exit_signal, self.exited, self.signalled, self.exe)


cdef class EventExeData(Event):
    """Represents Ecore_Exe_Event_Data from C-api.

    This event is issued by L{Exe} instances created with flags that
    allow reading from either stdout or stderr.

    See properties:

      - B{exe} instance of L{Exe} that created this event.
      - B{data} the raw string buffer with binary data from child process.
      - B{size} the size of B{data} (same as C{len(data)})
      - B{lines} list of strings with all text lines
    """
    cdef int _set_obj(self, void *o) except 0:
        cdef Ecore_Exe_Event_Data *obj
        cdef int i
        obj = <Ecore_Exe_Event_Data*>o
        self.exe = _ecore_exe_event_mapping.get(<long>obj.exe)
        if self.exe is None:
            return -1
        self.data = PyString_FromStringAndSize(<char*>obj.data, obj.size)
        self.size = obj.size
        self.lines = []

        line_append = self.lines.append
        if obj.lines:
            i = 0
            while obj.lines[i].line != NULL:
                line_append(PyString_FromStringAndSize(
                        obj.lines[i].line, obj.lines[i].size))
                i += 1

        return 1

    def __str__(self):
        if self.lines is None:
            count = None
        else:
            count = len(self.lines)
        return "%s(size=%d, lines=#%d, exe=%s)" % \
            (self.__class__.__name__, self.size, count, self.exe)

    def __repr__(self):
        if self.lines is None:
            count = None
        else:
            count = len(self.lines)
        return "%s(size=%d, lines=#%d, exe=%r)" % \
            (self.__class__.__name__, self.size, count, self.exe)


cdef class EventHandlerExe(EventHandler):
    """Specialized event handler that creates specialized event instances.

    This class is responsible by filtering out the events created from
    C without associated Python wrappers.
    """
    cdef int _exec(self, void *event) except 2:
        cdef Event e
        e = self.event_cls()
        if e._set_obj(event) == -1: # no exe
            return True
        return bool(self.func(e, *self.args, **self.kargs))


def on_exe_add_event_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EXE_EVENT_ADD

       @see: L{EventHandler}
       @see: L{EventHandlerExe}
    """
    return EventHandlerExe(ECORE_EXE_EVENT_ADD, func, *args, **kargs)


def on_exe_del_event_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EXE_EVENT_DEL

       @see: L{EventHandler}
       @see: L{EventHandlerExe}
    """
    return EventHandlerExe(ECORE_EXE_EVENT_DEL, func, *args, **kargs)


def on_exe_data_event_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EXE_EVENT_DATA

       @see: L{EventHandler}
       @see: L{EventHandlerExe}
    """
    return EventHandlerExe(ECORE_EXE_EVENT_DATA, func, *args, **kargs)


def on_exe_error_event_add(func, *args, **kargs):
    """Create an ecore event handler for ECORE_EXE_EVENT_ERROR

       @see: L{EventHandler}
       @see: L{EventHandlerExe}
    """
    return EventHandlerExe(ECORE_EXE_EVENT_ERROR, func, *args, **kargs)
