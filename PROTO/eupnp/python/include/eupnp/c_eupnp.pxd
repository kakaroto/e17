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

ctypedef int Eina_Bool
ctypedef struct Eina_Inlist:
    Eina_Inlist *prev
    Eina_Inlist *next
    Eina_Inlist *last

ctypedef object (*build_callback_t)(void *event_data)


cdef extern from "Eupnp.h":
    int eupnp_init()
    int eupnp_shutdown()

cdef extern from "eupnp_ecore.h":
    int eupnp_ecore_init()
    int eupnp_ecore_shutdown()

cdef extern from "eupnp_control_point.h":
    ctypedef struct Eupnp_Control_Point

    Eupnp_Control_Point *eupnp_control_point_new()
    void eupnp_control_point_free(Eupnp_Control_Point *c)
    Eina_Bool eupnp_control_point_start(Eupnp_Control_Point *c)
    Eina_Bool eupnp_control_point_stop(Eupnp_Control_Point *c)
    Eina_Bool eupnp_control_point_discovery_request_send(Eupnp_Control_Point *c, int mx, char *search_target)

cdef extern from "eupnp_event_bus.h":
    ctypedef struct Eupnp_Subscriber
    ctypedef enum Eupnp_Event_Type:
        EUPNP_EVENT_DEVICE_FOUND
        EUPNP_EVENT_DEVICE_GONE
        EUPNP_EVENT_SERVICE_FOUND
        EUPNP_EVENT_SERVICE_GONE
        EUPNP_EVENT_DEVICE_READY

    ctypedef Eina_Bool (*Eupnp_Callback)(void *user_data, Eupnp_Event_Type event_type, void *event_data)

    void eupnp_event_bus_publish(Eupnp_Event_Type event_type, void *event_data)
    Eupnp_Subscriber *eupnp_event_bus_subscribe(Eupnp_Event_Type event_type, Eupnp_Callback cb, void *user_data)
    void eupnp_event_bus_unsubscribe(Eupnp_Subscriber *s)
    Eupnp_Event_Type eupnp_event_bus_event_type_new()

cdef extern from "eupnp_device_info.h":
    ctypedef struct Eupnp_Device_Info:
        Eina_Inlist *services
        Eina_Inlist *devices

    ctypedef struct Eupnp_Device_Icon

cdef extern from "eupnp_service_info.h":
    ctypedef struct Eupnp_Service_Info:
        char *id

cdef extern from "eupnp_service_proxy.h":
    ctypedef struct Eupnp_Service_Proxy
    ctypedef struct Eupnp_Service_Action
    ctypedef struct Eupnp_Service_Action_Argument
    ctypedef struct Eupnp_State_Variable
    ctypedef struct Eupnp_State_Variable_Allowed_Value
    ctypedef struct Eupnp_Event_Subscriber


    ctypedef Eina_Bool (*Eupnp_State_Variable_Event_Cb)(Eupnp_State_Variable *var, void *buffer, int size, void *data)
    ctypedef void (*Eupnp_Service_Proxy_Ready_Cb)(void *data, Eupnp_Service_Proxy *proxy)
    ctypedef void (*Eupnp_Action_Response_Cb)(void *data, Eina_Inlist *evented_vars)

    void eupnp_service_proxy_new(Eupnp_Service_Info *service, Eupnp_Service_Proxy_Ready_Cb ready_cb, void *data)
    Eupnp_Service_Proxy *eupnp_service_proxy_ref(Eupnp_Service_Proxy *proxy)
    void eupnp_service_proxy_unref(Eupnp_Service_Proxy *proxy)
    Eupnp_State_Variable *eupnp_service_proxy_state_variable_get(Eupnp_Service_Proxy *proxy, char *name, int name_len)
    Eina_Bool eupnp_service_proxy_has_action(Eupnp_Service_Proxy *proxy, char *action)
    Eina_Bool eupnp_service_proxy_has_variable(Eupnp_Service_Proxy *proxy, char *variable_name)
    Eina_Bool eupnp_service_proxy_action_send(Eupnp_Service_Proxy *proxy, char *action, Eupnp_Action_Response_Cb response_cb, void *data, ...)
    Eupnp_Event_Subscriber *eupnp_service_proxy_state_variable_events_subscribe(Eupnp_Service_Proxy *proxy, char *var_name, Eupnp_State_Variable_Event_Cb cb, Eina_Bool auto_renew, Eina_Bool infinite_subscription, int timeout, void *data)
    Eina_Bool eupnp_service_proxy_state_variable_events_unsubscribe(Eupnp_Event_Subscriber *subscriber)
