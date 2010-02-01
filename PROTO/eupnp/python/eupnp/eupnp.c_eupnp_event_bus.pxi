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

import traceback


cdef object _build_device_found_event(void *event_data) with gil:
    cdef Device device
    device = Device()
    device._set_obj(event_data)
    return device

cdef object _build_device_gone_event(void *event_data) with gil:
    cdef Device device
    device = Device()
    device._set_obj(event_data)
    return device

cdef object _build_device_ready_event(void *event_data) with gil:
    cdef Device device
    device = Device(ready=True)
    device._set_obj(event_data)
    return device

cdef object _build_service_found_event(void *event_data) with gil:
    cdef Service service
    service = Service()
    service._set_obj(event_data)
    return service

cdef object _build_service_gone_event(void *event_data) with gil:
    cdef Service service
    service = Service(gone=True)
    service._set_obj(event_data)
    return service

cdef object _generic_build(void *event_data) with gil:
    return <object>event_data


cdef int cbs_len
cdef build_callback_t cbs[5]
cbs_len = 5
cbs[<int>c_eupnp.EUPNP_EVENT_DEVICE_FOUND] = _build_device_found_event
cbs[<int>c_eupnp.EUPNP_EVENT_DEVICE_GONE] = _build_device_gone_event
cbs[<int>c_eupnp.EUPNP_EVENT_DEVICE_READY] = _build_device_ready_event
cbs[<int>c_eupnp.EUPNP_EVENT_SERVICE_FOUND] = _build_service_found_event
cbs[<int>c_eupnp.EUPNP_EVENT_SERVICE_GONE] = _build_service_gone_event


cdef int _event_cb(void *user_data,
                   Eupnp_Event_Type type,
                   void *event_data) with gil:
    cdef object bus
    cdef object args
    cdef object kwargs
    cdef object user_cb
    cdef build_callback_t cb
    cdef bool ret = True

    (bus, user_cb, args, kwargs) = <object>user_data

    # Find specific build callbacks, otherwise just cast to a python object
    cb = _generic_build if <int>type >= cbs_len else cbs[<int>type]

    # Get callback response, False means unsubscription
    try:
        ret = user_cb(cb(event_data), *args, **kwargs)
    except Exception, e:
        traceback.print_exc()

    if not ret:
        try:
            bus._subscriptions[type].remove(<object>user_data)
        except ValueError, v:
            traceback.print_exc()
        return <Eina_Bool>0
    return <Eina_Bool>1



cdef class Bus:
    EVENT_DEVICE_FOUND = c_eupnp.EUPNP_EVENT_DEVICE_FOUND
    EVENT_DEVICE_GONE = c_eupnp.EUPNP_EVENT_DEVICE_GONE
    EVENT_DEVICE_READY = c_eupnp.EUPNP_EVENT_DEVICE_READY
    cdef public dict _subscriptions

    def __cinit__(self, *args, **kwargs):
        self._subscriptions = {}

    def event_type_new(self):
        return c_eupnp.eupnp_event_bus_event_type_new()

    def subscribe(self, int event_type, callback, *args, **kwargs):
        """ Subscribes a callback for events of type specified.

        @param event_type Event type to subscribe on
        @param cb Callback to receive events, prototyped "bool cb(event_data,
                  *args, **kwargs)" which should return False to indicate no
                  longer interest, that is, unsubscription.

        @param args User data tuple/list, forwarded to the callback
        @param kwargs User data dict, forwarded to the callback
        """
        cdef object data
        cdef Eupnp_Subscriber* handle

        if not callable(callback):
            raise TypeError("cb must be callable")

        data = (self, callback, args, kwargs)
        lst = self._subscriptions.setdefault(event_type, [])
        lst.append(data)
        handle = c_eupnp.eupnp_event_bus_subscribe(<Eupnp_Event_Type>event_type, _event_cb, <void*>data)
        return <long>handle

    cpdef unsubscribe(self, handle):
        """ Cancels a bus subscription.

        @param handle Subscription handle (received on subscribe())
        """
        c_eupnp.eupnp_event_bus_unsubscribe(<Eupnp_Subscriber*>handle)

    cpdef event_publish(self, Eupnp_Event_Type event_type, data):
        """ Publishes an event on the bus.

        @param event_type Event type
        @param data Event data object
        """
        c_eupnp.eupnp_event_bus_publish(event_type, <void*>data)
