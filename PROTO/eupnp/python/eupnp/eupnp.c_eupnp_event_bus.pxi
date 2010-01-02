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


cdef Eina_Bool _event_cb(void *user_data,
                         Eupnp_Event_Type type,
                         void *event_data) with gil:
    cdef object t
    t = <object>user_data
    ret = t[0](<object>event_data, type, *t[1], **t[2])
    python.Py_DECREF(t) # take ref of <object> cast

    if not ret:
        python.Py_DECREF(t)
        return <Eina_Bool>0

    return <Eina_Bool>1


cdef class Bus:
    def event_type_new(self):
        return c_eupnp.eupnp_event_bus_event_type_new()

    def subscribe(self, Eupnp_Event_Type event_type, callback, *args, **kwargs):
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

        data = (callback, args, kwargs)
        python.Py_INCREF(data)
        handle = c_eupnp.eupnp_event_bus_subscribe(event_type, <Eupnp_Callback>_event_cb, <void*>data)
        return <long>handle

    def unsubscribe(self, handle):
        """ Cancels a bus subscription.

        @param handle Subscription handle (received on subscribe())
        """
        c_eupnp.eupnp_event_bus_unsubscribe(<Eupnp_Subscriber*>handle)

    def event_publish(self, Eupnp_Event_Type event_type, data):
        """ Publishes an event on the bus.

        @param event_type Event type
        @param data Event data object
        """
        c_eupnp.eupnp_event_bus_publish(event_type, <void*>data)
