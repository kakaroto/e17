# Copyright (C) 2010 Boris Faure
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

from cpython cimport PyObject, Py_INCREF, Py_DECREF
from cpython cimport PyMem_Malloc, PyMem_Free
from cpython cimport bool

def init():
    ecore_win32_init()

def shutdown():
    ecore_win32_shutdown()

def screen_depth_get():
    return ecore_win32_screen_depth_get()

def current_time_get():
    return ecore_win32_current_time_get()

def message_loop_begin():
    ecore_win32_message_loop_begin()

include "ecore.win32.c_ecore_win32_window.pxi"
