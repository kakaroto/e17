# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
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

#!/usr/bin/env python2

import c_ecore

from c_ecore import shutdown, time_get, loop_time_get, timer_add, \
     main_loop_begin, main_loop_quit, main_loop_iterate, \
     main_loop_glib_integrate, event_handler_add, event_type_new, \
     animator_add, animator_frametime_set, animator_frametime_get, \
     idler_add, idle_enterer_add, idle_exiter_add, fd_handler_add, \
     Animator, Timer, Idler, IdleExiter, IdleEnterer, FdHandler, \
     Event, EventHandler, on_signal_user, \
     EventSignalUser, EventSignalUser1, EventSignalUser2, \
     on_signal_exit, EventSignalExit, EventSignalQuit, EventSignalTerminate, \
     EventSignalInterrupt, on_signal_hup, EventSignalHup, \
     on_signal_power, EventSignalPower, on_signal_realtime, \
     EventSignalRealtime, CustomEvent, QueuedEvent, \
     Exe, exe_run, exe_pipe_run, \
     exe_run_priority_set, exe_run_priority_get, \
     EventExeAdd, EventExeDel, EventExeData, \
     on_exe_add_event_add, on_exe_del_event_add, on_exe_data_event_add, \
     on_exe_error_event_add


ECORE_CALLBACK_CANCEL = 0
ECORE_CALLBACK_RENEW = 1

ECORE_FD_NONE = 0
ECORE_FD_READ = 1
ECORE_FD_WRITE = 2
ECORE_FD_ERROR = 4
ECORE_FD_ALL = 7

ECORE_EXE_PRIORITY_INHERIT = 9999

# Ecore Exe Flags (mask):
ECORE_EXE_PIPE_READ = 1 # Exe Pipe Read mask
ECORE_EXE_PIPE_WRITE = 2 # Exe Pipe Write mask
ECORE_EXE_PIPE_ERROR = 4 # Exe Pipe error mask
ECORE_EXE_PIPE_READ_LINE_BUFFERED = 8 # Reads are buffered until a newline
                                      # and delivered 1 event per line.
ECORE_EXE_PIPE_ERROR_LINE_BUFFERED = 16 # Errors are buffered until a newline
                                        # and delivered 1 event per line
ECORE_EXE_PIPE_AUTO = 32 # stdout and stderr are buffered automatically
ECORE_EXE_RESPAWN = 64 # Exe is restarted if it dies
ECORE_EXE_USE_SH = 128 # Use /bin/sh to run the command.
ECORE_EXE_NOT_LEADER = 256 # Do not use setsid() to have the executed process
                           # be its own session leader

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


#---------------------------------------------------------------------------
# let's try to warn users that ecore conflicts with signal and
# subprocess modules:
import warnings
import signal
import subprocess

class EcoreSignalWarning(Warning):
    pass

_orig_signal = None

def signal_warning(sig, action):

    if sig in (signal.SIGPIPE, signal.SIGALRM, signal.SIGCHLD, signal.SIGUSR1,
               signal.SIGUSR2, signal.SIGHUP,  signal.SIGQUIT, signal.SIGINT,
               signal.SIGTERM, signal.SIGPWR):
        warnings.warn(
        """\
Ecore already defines signal handlers for:

SIGPIPE
SIGALRM
SIGCHLD
SIGUSR1
SIGUSR2
SIGHUP
SIGQUIT
SIGINT
SIGTERM
SIGPWR
SIGRT*

Since you're defining a new signal handler, you might collide with
Ecore and bad things may happen!
""",
        EcoreSignalWarning)
    return _orig_signal(sig, action)


class EcoreSubprocessWarning(Warning):
    pass

_orig_subprocess = None
def subprocess_warning(*a, **ka):
    warnings.warn(
        """\
Using subprocess (Popen and derivates) with Ecore is a bad idea.

Ecore will set some signal handlers subprocess module depends and this
may cause this module to operate unexpectedly.

Instead of using subprocess.Popen(), please consider using Ecore's
Exe() class. See help(ecore.Exe)
""",
        EcoreSubprocessWarning)
    return _orig_subprocess(*a, **ka)


if signal.signal is not signal_warning:
    _orig_signal = signal.signal
    signal.signal = signal_warning
    signal_warning.__doc__ = _orig_signal.__doc__
    signal_warning.__name__ = _orig_signal.__name__

if subprocess.Popen is not subprocess_warning:
    _orig_subprocess = subprocess.Popen
    subprocess.Popen = subprocess_warning
    subprocess_warning.__doc__ = _orig_subprocess.__doc__
    subprocess_warning.__name__ = _orig_subprocess.__name__
