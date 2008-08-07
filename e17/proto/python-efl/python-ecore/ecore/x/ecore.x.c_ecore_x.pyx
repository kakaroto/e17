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

cimport evas.python as python

__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )


def init(name=None):
    """Initialize the X display connection to the given display.

    @parm name: display target name, if None, default will be used.
    @rtype: int
    """
    cdef char *s
    cdef int i
    if name is None:
        s = NULL
    else:
        s = name
    i = ecore_x_init(s)
    x_events_register()
    return i


def shutdown():
    """Shuts down the Ecore X library.

    In shutting down the library, the X display connection is terminated
    and any event handlers for it are removed.

    @rtype: int
    """
    return ecore_x_shutdown()


def disconnect():
    """Shuts down the Ecore X library.

    As ecore_x_shutdown, except do not close Display, only connection.

    @rtype: int
    """
    return ecore_x_disconnect()


def fd_get():
    """Retrieves the X display file descriptor.

    @rtype: int
    """
    return ecore_x_fd_get()


def double_click_time_set(double t):
    """Sets the timeout for a double and triple clicks to be flagged.

    This sets the time between clicks before the double_click flag is
    set in a button down event. If 3 clicks occur within double this
    time, the triple_click flag is also set.
    """
    ecore_x_double_click_time_set(t)

def double_click_time_get():
    "@rtype: float"
    return ecore_x_double_click_time_get()


def flush():
    "Sends all X commands in the X Display buffer."
    ecore_x_flush()


def sync():
    "Flushes the command buffer and waits until all requests have been"
    ecore_x_sync()


def current_time_get():
    "Return the last event time."
    return ecore_x_current_time_get()


def error_request_get():
    """Get the request code that caused the error.

    @rtype: int
    """
    return ecore_x_error_request_get()


def error_code_get():
    """Get the error code from the error.

    @rtype: int
    """
    return ecore_x_error_code_get()


def window_focus_get():
    """Returns the window that has the focus.

    @rtype: L{Window}
    """
    cdef Ecore_X_Window xid
    xid = ecore_x_window_focus_get()
    return Window_from_xid(xid)


cdef int _skip_list_build(skip_list, Ecore_X_Window **pskips, int *pskip_num) except 0:
    cdef Window win
    cdef int i

    if skip_list:
        pskip_num[0] = len(skip_list)
    else:
        pskip_num[0] = 0

    if pskip_num[0] == 0:
        pskips[0] = NULL
        return 1
    else:
        pskips[0] = <Ecore_X_Window *>python.PyMem_Malloc(pskip_num[0] * sizeof(Ecore_X_Window))
        i = 0
        try:
            for w in skip_list:
                win = w
                pskips[0][i] = win.xid
                i += 1
        except:
            pskip_num[0] = 0
            python.PyMem_Free(<void*>pskips[0])
            raise
    return 1


def window_shadow_tree_at_xy_with_skip_get(Window base, int x, int y, skip_list=None):
    """Retrieves the top, visible window at the given location,
       but skips the windows in the list. This uses a shadow tree built from the
       window tree that is only updated the first time
       L{shadow_tree_at_xy_with_skip_get()} is called, or the next time
       it is called after a L{shadow_tree_flush()}.

       @parm base: Window to use as base, or None to use root window.
       @parm x: The given X position.
       @parm y: The given Y position.
       @rtype: Window
    """
    cdef Ecore_X_Window base_xid, ret_xid, *skips
    cdef int skip_num
    if base is <Window>None:
        base_xid = 0
    else:
        base_xid = base.xid

    _skip_list_build(skip_list, &skips, &skip_num)
    ret_xid = ecore_x_window_shadow_tree_at_xy_with_skip_get(base_xid, x, y,
                                                             skips, skip_num)
    if skips != NULL:
        python.PyMem_Free(<void*>skips)

    return Window_from_xid(ret_xid)


def window_shadow_tree_flush():
    "Flushes the window shadow tree so nothing is stored."
    ecore_x_window_shadow_tree_flush()


def window_at_xy_get(int x, int y):
    """Retrieves the top, visible window at the given location.

    @parm x: horizontal position.
    @parm y: vertical position.
    @rtype: Window
    """
    cdef Ecore_X_Window xid
    xid = ecore_x_window_at_xy_get(x, y)
    return Window_from_xid(xid)


def window_at_xy_with_skip_get(int x, int y, skip_list=None):
    """Retrieves the top, visible window at the given location.

    @parm x: horizontal position.
    @parm y: vertical position.
    @rtype: Window
    """
    cdef Ecore_X_Window xid, *skips
    cdef int skip_num

    _skip_list_build(skip_list, &skips, &skip_num)
    xid = ecore_x_window_at_xy_with_skip_get(x, y, skips, skip_num)

    if skips != NULL:
        python.PyMem_Free(<void*>skips)

    return Window_from_xid(xid)


def window_at_xy_begin_get(Window begin, int x, int y):
    """Retrieves the top, visible window at the given location, starting from
       begin.

    @parm begin: Window to start at.
    @parm x: horizontal position.
    @parm y: vertical position.
    @rtype: Window
    """
    cdef Ecore_X_Window xid, begin_xid
    if begin is <Window>None:
        begin_xid = 0
    else:
        begin_xid = begin.xid
    xid = ecore_x_window_at_xy_begin_get(begin_xid, x, y)
    return Window_from_xid(xid)


include "ecore.x.c_ecore_x_window.pxi"
include "ecore.x.c_ecore_x_events.pxi"
