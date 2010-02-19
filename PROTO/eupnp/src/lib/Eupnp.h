/**
 * @page eupnp_license Copyright Information
 *
 * Eupnp - UPnP library
 *
 * Copyright (C) 2009 Andre Dieb Martins <andre.dieb@gmail.com>
 *
 * This file is part of Eupnp.
 *
 * Eupnp is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Eupnp is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Eupnp.  If not, see <http://www.gnu.org/licenses/>.
 *
 * @author André Dieb Martins
 * @date 2009
 *
 */

#ifndef _EUPNP_H
#define _EUPNP_H

#include <Eina.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
# ifdef E_UPNP_BUILD
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
# else
#  define EAPI __declspec(dllimport)
# endif /* ! E_UPNP_BUILD */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif /* ! _WIN32 */

#include "eupnp_core.h"
#include "eupnp_udp_transport.h"
#include "eupnp_http_message.h"

/**
 * @mainpage
 *
 * @section toc Table Of Contents
 *
 * <ol>
 *
 * <li> @subpage eupnp_about </li>
 *     <ol>
 *         <li> @ref eupnp_subsec_about_whatis </li>
 *         <li> @subpage eupnp_license </li>
 *         <li> @ref eupnp_subsec_features </li>
 *         <li> @ref eupnp_subsec_requirements </li>
 *         <li> @ref eupnp_subsec_todo </li>
 *     </ol>
 *
 * <li> @subpage eupnp_app_writing </li>
 *     <ol>
 *         <li> @ref eupnp_sec_initializing </li>
 *         <li> @ref Eupnp_Event_Bus
 *             <ul>
 *                 <li> @ref eupnp_event_bus_subscribe </li>
 *             </ul>
 *         </li>
 *
 *     </ol>
 *
 * <li> @subpage eupnp_advanced_usage </li>
 *     <ol>
 *         <li> @ref Eupnp_Event_Bus
 *             <ul>
 *                 <li> @ref eupnp_event_bus_unsubscribe </li>
 *                 <li> @ref eupnp_event_bus_publish </li>
 *                 <li> @ref eupnp_event_bus_publish_custom </li>
 *             </ul>
 *         <li> @ref Eupnp_SSDP_Client_Module </li>
 *             <ul>
 *                 <li> @ref eupnp_ssdp_initialize </li>
 *                 <li> @ref eupnp_ssdp_instance </li>
 *                 <li> @ref eupnp_ssdp_events </li>
 *             </ul>
 *     </ol>
 * </ol>
 *
 */

/**
 * @page eupnp_advanced_usage Advanced Usage
 *
 * Though Eupnp provides a high level for building UPnP applications, most of its
 * modules are independent. In this chapter we describe each module and how to
 * use it.
 *
 * @li @ref Eupnp_SSDP_Client_Module
 * @li @ref Eupnp_Event_Bus
 * @li @ref Eupnp_UDP_Transport
 */

/**
 * @page eupnp_about About Eupnp
 *
 * @section eupnp_subsec_about_whatis What is Eupnp?
 *
 * Eupnp (aka libeupnp) is a lightweight event-driven UPnP (Universal Plug n' Play) library written in C. 
 *
 * @section eupnp_subsec_features Features
 *
 * @li SSDP client implementation (Simple Service Discovery Protocol)
 * @li UPnP stack implementation: discovery, description and control of UPnP services and devices
 * @li Toolkit independent. Core interface for integration with any toolkit (Ecore, Glib, Qt) - already integrated with Ecore (aka libeupnp-ecore)
 *
 * @section eupnp_subsec_requirements Requirements
 *
 * @li Eina
 * @li Ecore, Ecore-con (with curl support)
 * @li libxml-2.0
 *
 */

#define EUPNP_ST_SSDP_ALL           "ssdp:all"
#define EUPNP_ST_UPNP_ROOTDEVICE    "upnp:rootdevice"
#define EUPNP_SSDP_NOTIFY_ALIVE     "ssdp:alive"
#define EUPNP_SSDP_NOTIFY_BYEBYE    "ssdp:byebye"

/**
 * @enum Eupnp_Event_Type
 *
 * Built-in event types.
 *
 * @see eupnp_event_bus_event_type_new()
 */
typedef enum {
   EUPNP_EVENT_DEVICE_FOUND,
   EUPNP_EVENT_DEVICE_GONE,
   EUPNP_EVENT_SERVICE_FOUND,
   EUPNP_EVENT_SERVICE_GONE,
   EUPNP_EVENT_DEVICE_READY,
   EUPNP_EVENT_COUNT,
} Eupnp_Event_Type;

typedef enum {
   EUPNP_TYPE_INT = 1,
   EUPNP_TYPE_DOUBLE,
   EUPNP_TYPE_STRING
} Eupnp_Types;

typedef enum {
   EUPNP_ARGUMENT_DIRECTION_IN,
   EUPNP_ARGUMENT_DIRECTION_OUT
} Eupnp_Argument_Direction;

typedef struct _Eupnp_Device_Info Eupnp_Device_Info;
typedef struct _Eupnp_Service_Info Eupnp_Service_Info;
typedef struct _Eupnp_Service_Proxy Eupnp_Service_Proxy;
typedef struct _Eupnp_Service_Action_Argument Eupnp_Service_Action_Argument;
typedef struct _Eupnp_Event_Subscriber Eupnp_Event_Subscriber;
typedef struct _Eupnp_Control_Point Eupnp_Control_Point;
typedef struct _Eupnp_SSDP_Client Eupnp_SSDP_Client;
typedef struct _Eupnp_Subscriber Eupnp_Subscriber;
typedef struct _Eupnp_State_Variable Eupnp_State_Variable;

typedef Eina_Bool (*Eupnp_State_Variable_Event_Cb)(Eupnp_State_Variable *var, void *buffer, int size, void *data);
typedef void (*Eupnp_Service_Proxy_Ready_Cb)(void *data, Eupnp_Service_Proxy *proxy);
typedef void (*Eupnp_Action_Response_Cb)(void *data, Eina_Inlist *evented_vars);
typedef Eina_Bool (*Eupnp_Callback) (void *user_data, Eupnp_Event_Type event_type, void *event_data);

struct _Eupnp_Control_Point {
   Eupnp_SSDP_Client *ssdp_client;
};

struct _Eupnp_SSDP_Client {
   Eupnp_UDP_Transport *udp_transport;

   /* Private */
   Eupnp_Fd_Handler socket_handler;
};

struct _Eupnp_Subscriber {
   Eupnp_Event_Type type;
   Eupnp_Callback cb;
   Eina_Bool deleted:1;
   void *user_data;
};

struct _Eupnp_Service_Info {
   EINA_INLIST;

   const char *udn;
   const char *location;
   const char *service_type;
   const char *id;
   const char *control_url;
   const char *scpd_url;
   const char *eventsub_url;

   /* Private */
   int refcount;
   void *_resource; /* Shared resource */
   void (*_resource_free)(void *resource); /* Resource free function */
};

struct _Eupnp_Device_Info {
   EINA_INLIST;

   const char *udn;
   const char *location;
   const char *base_url;
   const char *device_type;
   const char *friendly_name;
   const char *manufacturer;
   const char *manufacturer_url;
   const char *model_description;
   const char *model_name;
   const char *model_number;
   const char *model_url;
   const char *serial_number;
   const char *upc;
   const char *presentation_url;
   int spec_version_major;
   int spec_version_minor;

   Eina_Inlist *icons;            /* List of Eupnp_Device_Icon */
   Eina_Inlist *services;         /* List of Eupnp_Service_Info */
   Eina_Inlist *embedded_devices; /* List of Eupnp_Device_Info */

   /* Private */
   void *xml_parser;
   int refcount;
   void *_resource; /* Shared resource */
   void (*_resource_free)(void *resource); /* Resource free function */
};

struct _Eupnp_Service_Action_Argument {
   EINA_INLIST;
   const char *name;
   const char *value;
   Eupnp_Argument_Direction direction;
   Eupnp_State_Variable *related_state_variable;
   void *retval; // Optional
};

/**
 * @def EUPNP_CALLBACK(f)
 *
 * Converts a function to an Eupnp_Callback callback type.
 *
 * @param func Function to convert to an Eupnp_Callback
 */
#define EUPNP_CALLBACK(f) ((Eupnp_Callback) f)
#define EUPNP_STATE_VARIABLE_EVENT_CB(f) ((Eupnp_State_Variable_Event_Cb)f)
#define EUPNP_SERVICE_PROXY_READY_CB(f) ((Eupnp_Service_Proxy_Ready_Cb)f)
#define EUPNP_ACTION_RESPONSE_CB(f) ((Eupnp_Action_Response_Cb)f)


EAPI int eupnp_init(void);
EAPI int eupnp_shutdown(void);

EAPI Eupnp_Control_Point *eupnp_control_point_new(void);
EAPI void                 eupnp_control_point_free(Eupnp_Control_Point *c) EINA_ARG_NONNULL(1);
EAPI Eina_Bool            eupnp_control_point_start(Eupnp_Control_Point *c) EINA_ARG_NONNULL(1);
EAPI Eina_Bool            eupnp_control_point_stop(Eupnp_Control_Point *c) EINA_ARG_NONNULL(1);
EAPI Eina_Bool            eupnp_control_point_discovery_request_send(Eupnp_Control_Point *c, int mx, const char *search_target) EINA_ARG_NONNULL(1,2,3);

EAPI void                 eupnp_event_bus_publish(Eupnp_Event_Type event_type, void *event_data);
EAPI Eupnp_Subscriber    *eupnp_event_bus_subscribe(Eupnp_Event_Type event_type, Eupnp_Callback cb, void *user_data) EINA_ARG_NONNULL(2);
EAPI void                 eupnp_event_bus_unsubscribe(Eupnp_Subscriber *s) EINA_ARG_NONNULL(1);
EAPI Eupnp_Event_Type     eupnp_event_bus_event_type_new(void);
EAPI Eina_Bool            eupnp_event_bus_type_has_subscriber(Eupnp_Event_Type type);


EAPI Eupnp_Service_Info         *eupnp_service_info_ref(Eupnp_Service_Info *service_info) EINA_ARG_NONNULL(1);
EAPI void                        eupnp_service_info_unref(Eupnp_Service_Info *service_info) EINA_ARG_NONNULL(1);

EAPI Eupnp_Device_Info          *eupnp_device_info_ref(Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI void                        eupnp_device_info_unref(Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI void                        eupnp_device_info_fetch(Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const Eupnp_Service_Info   *eupnp_device_info_service_get_by_type(const Eupnp_Device_Info *device_info, const char *service_type) EINA_ARG_NONNULL(1,2);

EAPI const char                 *eupnp_device_info_udn_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_location_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_base_url_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_device_type_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_friendly_name_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_manufacturer_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_manufacturer_url_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_model_description_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_model_name_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_model_number_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_model_url_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_serial_number_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_upc_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI const char                 *eupnp_device_info_presentation_url_get(const Eupnp_Device_Info *device_info) EINA_ARG_NONNULL(1);
EAPI void                        eupnp_device_info_spec_version_get(const Eupnp_Device_Info *device_info, int *minor, int *major) EINA_ARG_NONNULL(1,2,3);

EAPI void                        eupnp_service_proxy_new(const Eupnp_Service_Info *service, Eupnp_Service_Proxy_Ready_Cb ready_cb, void *data) EINA_ARG_NONNULL(1,2);
EAPI Eupnp_Service_Proxy        *eupnp_service_proxy_ref(Eupnp_Service_Proxy *proxy) EINA_ARG_NONNULL(1);
EAPI void                        eupnp_service_proxy_unref(Eupnp_Service_Proxy *proxy) EINA_ARG_NONNULL(1);
EAPI Eina_Bool                   eupnp_service_proxy_has_action(const Eupnp_Service_Proxy *proxy, const char *action) EINA_ARG_NONNULL(1,2);
EAPI Eina_Bool                   eupnp_service_proxy_has_variable(const Eupnp_Service_Proxy *proxy, const char *variable_name) EINA_ARG_NONNULL(1,2);
EAPI Eina_Bool                   eupnp_service_proxy_action_send(Eupnp_Service_Proxy *proxy, const char *action, Eupnp_Action_Response_Cb response_cb, void *data, ...) EINA_ARG_NONNULL(1,2,3);
EAPI Eupnp_Event_Subscriber     *eupnp_service_proxy_state_variable_events_subscribe(Eupnp_Service_Proxy *proxy, const char *var_name, Eupnp_State_Variable_Event_Cb cb, Eina_Bool auto_renew, Eina_Bool infinite_subscription, int timeout, void *data) EINA_ARG_NONNULL(1,2,3);
EAPI Eina_Bool                   eupnp_service_proxy_state_variable_events_unsubscribe(Eupnp_Event_Subscriber *subscriber) EINA_ARG_NONNULL(1);
EAPI const Eupnp_State_Variable *eupnp_service_proxy_state_variable_get(const Eupnp_Service_Proxy *proxy, const char *name, int name_len) EINA_ARG_NONNULL(1,2);

EAPI Eupnp_SSDP_Client   *eupnp_ssdp_client_new(void);
EAPI void                 eupnp_ssdp_client_free(Eupnp_SSDP_Client *c) EINA_ARG_NONNULL(1);
EAPI Eina_Bool            eupnp_ssdp_client_start(Eupnp_SSDP_Client *c) EINA_ARG_NONNULL(1);
EAPI Eina_Bool            eupnp_ssdp_client_stop(Eupnp_SSDP_Client *c) EINA_ARG_NONNULL(1);
EAPI Eina_Bool            eupnp_ssdp_discovery_request_send(Eupnp_SSDP_Client *c, int mx, const char *search_target) EINA_ARG_NONNULL(1,2,3);

#endif /* _EUPNP_H */
