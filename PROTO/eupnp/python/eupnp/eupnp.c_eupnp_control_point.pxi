# Copyright (C) 2010 André Dieb Martins <andre.dieb@gmail.com>
#
# This file is part of Python-Eupnp.
#
# Python-Eupnp is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# Python-Eupnp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public License
# along with this Python-Eupnp.  If not, see <http://www.gnu.org/licenses/>.

cimport c_eupnp
cimport python_exc
import traceback


cdef class ControlPoint:
    cdef c_eupnp.Eupnp_Control_Point* _cp

    def __cinit__(self):
        self._cp = c_eupnp.eupnp_control_point_new()
        if self._cp is not NULL:
            python_exc.PyErr_NoMemory()

    def __dealoc__(self):
        if self._cp is not NULL:
            c_eupnp.eupnp_control_point_free(self._cp)

    def start(self):
        return c_eupnp.eupnp_control_point_start(self._cp)

    def stop(self):
        return c_eupnp.eupnp_control_point_stop(self._cp)

    cpdef discovery_send(self, mx, search_target):
        return c_eupnp.eupnp_control_point_discovery_request_send \
            (self._cp, mx, search_target)
