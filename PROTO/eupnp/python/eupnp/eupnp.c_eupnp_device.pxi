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
    cdef bool _ready
    cdef bool _gone

    def __cinit__(self, ready=False, gone=False):
        self._ready = ready
        self._gone = gone
        self._root = NULL

    def __dealloc__(self):
        if self._root:
            self._unset_obj()

    cdef _set_obj(self, void *obj):
        self._root = c_eupnp.eupnp_device_info_ref(<Eupnp_Device_Info*>obj)

    cdef _unset_obj(self):
        c_eupnp.eupnp_device_info_unref(self._root)
        self._root = NULL

    def __str__(self):
        return '%s(ready=%s, gone=%s, udn="%s", device_type="%s", location="%s", '\
               'friendly_name="%s")' % \
               (self.__class__.__name__, self._ready, self._gone, self.udn,
                self.device_type, self.location, self.friendly_name)

    def udn_get(self):
        return self._root.udn

    property udn:
        def __get__(self):
            return self.udn_get()

    def location_get(self):
        return self._root.location

    property location:
        def __get__(self):
            return self.location_get()

    def base_url_get(self):
        return self._root.base_URL

    property base_url:
        def __get__(self):
            return self.base_url_get()

    def device_type_get(self):
        return self._root.device_type

    property device_type:
        def __get__(self):
            return self.device_type_get()

    def friendly_name_get(self):
        return self._root.friendly_name

    property friendly_name:
        def __get__(self):
            return self.friendly_name_get()

    def manufacturer_get(self):
        return self._root.manufacturer

    property manufacturer:
        def __get__(self):
            return self.manufacturer_get()

    def manufacturer_url_get(self):
        return self._root.manufacturer_URL

    property manufacturer_URL:
        def __get__(self):
            return self.manufacturer_url_get()

    def model_description_get(self):
        return self._root.model_description

    property model_description:
        def __get__(self):
            return self.model_description_get()

    def model_name_get(self):
        return self._root.model_name

    property model_name:
        def __get__(self):
            return self.model_name_get()

    def model_number_get(self):
        return self._root.model_number

    property model_number:
        def __get__(self):
            return self.model_number_get()

    def model_url_get(self):
        return self._root.model_URL

    property model_url:
        def __get__(self):
            return self.model_url_get()

    def serial_number_get(self):
        return self._root.serial_number

    property serial_number:
        def __get__(self):
            return self.serial_number_get()

    def upc_get(self):
        return self._root.upc

    property upc:
        def __get__(self):
            return self.upc_get()

    def presentation_url_get(self):
        return self._root.presentation_URL

    property presentation_url:
        def __get__(self):
            return self.presentation_url_get()

    def spec_version_major_get(self):
        return self._root.spec_version_major

    property spec_version_major:
        def __get__(self):
            return self.spec_version_major_get()

    def spec_version_minor_get(self):
        return self._root.spec_version_minor

    property spec_version_minor:
        def __get__(self):
            return self.spec_version_minor_get()


cdef class Service:
    cdef c_eupnp.Eupnp_Service_Info* _service
    cdef bool _gone

    def __cinit__(self, gone=False):
        self._gone = gone

    cdef _set_obj(self, void *obj):
        self._service = <Eupnp_Service_Info*> obj

    def udn_get(self):
        return self._service.udn

    property udn:
        def __get__(self):
            return self.udn_get()

    def location_get(self):
        return self._service.location

    property location:
        def __get__(self):
            return self.location_get()

    def service_type_get(self):
        return self._service.service_type

    property service_type:
        def __get__(self):
            return self.service_type_get()

    def id_get(self):
        return self._service.id

    property id:
        def __get__(self):
            return self.id_get()

    def control_url_get(self):
        return self._service.control_URL

    property control_url:
        def __get__(self):
            return self.control_url_get()

    def scpd_url_get(self):
        return self._service.scpd_URL

    property scpd_url:
        def __get__(self):
            return self.scpd_url_get()

    def eventsub_url_get(self):
        return self._service.eventsub_URL

    property eventsub_URL:
        def __get__(self):
            return self.eventsub_url_get()
