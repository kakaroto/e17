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
cimport python


cdef void _proxy_ready_cb(void *data, Eupnp_Service_Proxy *proxy):
    cdef object t
    cdef object self
    cdef Eupnp_Service_Info* s

    t = <object> data
    self = t[0]
    s = <Eupnp_Service_Info*> t[1]

    self._services[str(s.id)] = <long>c_eupnp.eupnp_service_proxy_ref(proxy)
    python.Py_DECREF(t)


cdef class Device:
    cdef c_eupnp.Eupnp_Device_Info *_root

    def __cinit__(self, ready=False, gone=False):
        self._ready = ready
        self._gone = gone

    cdef _set_obj(self, void *obj):
        self._root = <Eupnp_Device_Info*> obj


cdef class Service:
    cdef c_eupnp.Eupnp_Service_Info* _service

    def __cinit__(self, gone=False):
        self._gone = gone

    cdef _set_obj(self, void *obj):
        self._service = <Eupnp_Service_Info*> obj
