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

cdef extern from "Ecore_X.h":
    int ecore_x_screensaver_event_available_get()
    void ecore_x_screensaver_idle_time_prefetch()
    void ecore_x_screensaver_idle_time_fetch()
    int ecore_x_screensaver_idle_time_get()
    void ecore_x_get_screensaver_prefetch()
    void ecore_x_get_screensaver_fetch()
    void ecore_x_screensaver_set(int timeout, int interval, int blank, int expose)
    void ecore_x_screensaver_timeout_set(int timeout)
    int ecore_x_screensaver_timeout_get()
    void ecore_x_screensaver_blank_set(int timeout)
    int ecore_x_screensaver_blank_get()
    void ecore_x_screensaver_expose_set(int timeout)
    int ecore_x_screensaver_expose_get()
    void ecore_x_screensaver_interval_set(int timeout)
    int ecore_x_screensaver_interval_get()
    void ecore_x_screensaver_event_listen_set(int on)
