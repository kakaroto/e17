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

__extra_epydoc_fields__ = (
    ("parm", "Parameter", "Parameters"), # epydoc don't support pyrex properly
    )

def event_available_get():
    return bool(ecore_x_screensaver_event_available_get())

def idle_time_get():
    """Get the number of seconds since the last input was received.

    @rtype: int
    """
    return ecore_x_screensaver_idle_time_get()

def screensaver_set(int timeout, int interval, int prefer_blanking,
                    int allow_exposures):
    """Configure screensaver.

    @parm timeout: how long to wait until screen saver turns on (in seconds).
    @parm interval: time between screen saver alterations (in seconds).
    @parm prefer_blanking: True to enable blank, False to disable.
    @parm allow_exposures: if to allow expose generation event or not.
    """
    ecore_x_screensaver_set(timeout, interval, prefer_blanking,
                            allow_exposures)

def timeout_set(int timeout):
    ecore_x_screensaver_timeout_set(timeout)

def timeout_get():
    return ecore_x_screensaver_timeout_get()

def interval_set(int timeout):
    ecore_x_screensaver_interval_set(timeout)

def interval_get():
    return ecore_x_screensaver_interval_get()

def blank_set(int timeout):
    ecore_x_screensaver_blank_set(timeout)

def blank_get():
    return bool(ecore_x_screensaver_blank_get())

def expose_set(int timeout):
    ecore_x_screensaver_expose_set(timeout)

def expose_get():
    return bool(ecore_x_screensaver_expose_get())

def event_listen_set(int on):
    ecore_x_screensaver_event_listen_set(on)
