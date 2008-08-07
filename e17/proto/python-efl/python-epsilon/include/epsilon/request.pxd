# Copyright (C) 2007-2008 Gustavo Sverzut Barbieri
#
# This file is part of Python-Epsilon.
#
# Python-Epsilon is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Epsilon is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Epsilon.  If not, see <http://www.gnu.org/licenses/>.

cdef extern from "evas/python_evas_utils.h":
    int PY_REFCOUNT(object)


cdef extern from "Epsilon_Request.h":
    ctypedef struct Epsilon_Request:
        unsigned id
        unsigned size
        unsigned status
        char *path
        char *dest
        void *data

    int EPSILON_EVENT_DONE

    int epsilon_request_init()
    int epsilon_request_shutdown()
    Epsilon_Request *epsilon_request_add(char *path, int size, void *data)
    void epsilon_request_del(Epsilon_Request *thumb)


cdef extern from "Ecore.h":
    ctypedef struct Ecore_Event_Handler
    Ecore_Event_Handler *ecore_event_handler_add(int type, int (*func) (void *data, int type, void *event), void *data)


cdef class Request:
    cdef Epsilon_Request *obj
    cdef object func
