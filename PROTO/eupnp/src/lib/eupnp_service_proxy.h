/* Eupnp - UPnP library
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
 */

#ifndef _EUPNP_SERVICE_PROXY_H
#define _EUPNP_SERVICE_PROXY_H

#include <Eina.h>

#include "Eupnp.h"
#include "eupnp_service_info.h"

typedef struct _Eupnp_Service_Proxy Eupnp_Service_Proxy;
typedef struct _Eupnp_Service_Action Eupnp_Service_Action;
typedef struct _Eupnp_Service_Action_Argument Eupnp_Service_Action_Argument;
typedef struct _Eupnp_State_Variable Eupnp_State_Variable;
typedef struct _Eupnp_State_Variable_Allowed_Value Eupnp_State_Variable_Allowed_Value;
typedef struct _Eupnp_Event_Subscriber Eupnp_Event_Subscriber;

typedef Eina_Bool (*Eupnp_State_Variable_Event_Cb)(Eupnp_State_Variable *var, void *buffer, int size, void *data);
typedef void (*Eupnp_Service_Proxy_Ready_Cb)(void *data, Eupnp_Service_Proxy *proxy);
typedef void (*Eupnp_Action_Response_Cb)(void *data, const char *response_data, int response_len);

#define EUPNP_STATE_VARIABLE_EVENT_CB(f) ((Eupnp_State_Variable_Event_Cb)f)
#define EUPNP_SERVICE_PROXY_READY_CB(f) ((Eupnp_Service_Proxy_Ready_Cb)f)
#define EUPNP_ACTION_RESPONSE_CB(f) ((Eupnp_Action_Response_Cb)f)

typedef enum {
   EUPNP_TYPE_INT = 1,
   EUPNP_TYPE_DOUBLE,
   EUPNP_TYPE_STRING
} EUPNP_TYPES;

struct _Eupnp_Service_Proxy {
   int spec_version_major;
   int spec_version_minor;

   Eina_Inlist *actions;     // List of actions
   Eina_Inlist *state_table; // List of state variables

   /* Private */
   const char *control_URL;
   const char *eventsub_URL;
   const char *base_URL;
   const char *service_type;
   const char *event_host;
   Eupnp_Server event_server;
   void *xml_parser;
   Eupnp_Service_Proxy_Ready_Cb ready_cb;
   void *ready_cb_data;
   int refcount;
};

struct _Eupnp_Service_Action {
   EINA_INLIST;
   const char *name;
   Eina_Inlist *arguments;
};

typedef enum {
   EUPNP_ARGUMENT_DIRECTION_IN,
   EUPNP_ARGUMENT_DIRECTION_OUT
} Eupnp_Argument_Direction;

struct _Eupnp_Service_Action_Argument {
   EINA_INLIST;
   const char *name;
   Eupnp_Argument_Direction direction;
   Eupnp_State_Variable *related_state_variable;
   void *retval; // Optional
};

typedef enum {
   EUPNP_DATA_TYPE_UI1,
   EUPNP_DATA_TYPE_UI2,
   EUPNP_DATA_TYPE_UI4,
   EUPNP_DATA_TYPE_I1,
   EUPNP_DATA_TYPE_I2,
   EUPNP_DATA_TYPE_I4,
   EUPNP_DATA_TYPE_INT,
   EUPNP_DATA_TYPE_R4,
   EUPNP_DATA_TYPE_R8,
   EUPNP_DATA_TYPE_NUMBER,
   EUPNP_DATA_TYPE_FIXED14_4,
   EUPNP_DATA_TYPE_FLOAT,
   EUPNP_DATA_TYPE_CHAR,
   EUPNP_DATA_TYPE_STRING,
   EUPNP_DATA_TYPE_DATE,
   EUPNP_DATA_TYPE_DATETIME,
   EUPNP_DATA_TYPE_DATETIME_TZ,
   EUPNP_DATA_TYPE_TIME,
   EUPNP_DATA_TYPE_TIME_TZ,
   EUPNP_DATA_TYPE_BOOLEAN,
   EUPNP_DATA_TYPE_BIN_BASE64,
   EUPNP_DATA_TYPE_BIN_HEX,
   EUPNP_DATA_TYPE_URI,
   EUPNP_DATA_TYPE_UUID
} Eupnp_Data_Type;

struct _Eupnp_State_Variable {
   EINA_INLIST;
   Eina_Bool send_events;
   const char *name;
   Eupnp_Data_Type data_type;
   void *default_value;
   Eina_Inlist *allowed_value_list;
   char *range_min;
   char *range_max;
   char *range_step;
};

struct _Eupnp_State_Variable_Allowed_Value {
   EINA_INLIST;
   const char *value;
};

EAPI int                    eupnp_service_proxy_init(void);
EAPI int                    eupnp_service_proxy_shutdown(void);

EAPI Eupnp_Service_Action  *eupnp_service_action_new(void);
EAPI void                   eupnp_service_action_free(Eupnp_Service_Action *a);
EAPI Eina_Bool              eupnp_service_action_argument_add(Eupnp_Service_Action *action, Eupnp_Service_Action_Argument *arg);
EAPI void                   eupnp_service_action_dump(Eupnp_Service_Action *a);

EAPI Eupnp_State_Variable  *eupnp_service_state_variable_new(const char *name, int name_len);
EAPI Eupnp_State_Variable  *eupnp_service_state_variable_new1();
EAPI void                   eupnp_service_state_variable_free(Eupnp_State_Variable *st);
EAPI void                   eupnp_service_state_variable_dump(Eupnp_State_Variable *st);
EAPI Eina_Bool              eupnp_service_state_variable_allowed_value_add(Eupnp_State_Variable *st, Eupnp_State_Variable_Allowed_Value *v);

EAPI Eupnp_State_Variable_Allowed_Value *eupnp_service_state_variable_allowed_value_new(void);
EAPI void                                eupnp_service_state_variable_allowed_value_free(Eupnp_State_Variable_Allowed_Value *value);

EAPI Eupnp_Service_Action_Argument   *eupnp_service_action_argument_new(void);
EAPI void                             eupnp_service_action_argument_free(Eupnp_Service_Action_Argument *arg);
EAPI void                             eupnp_service_action_argument_dump(Eupnp_Service_Action_Argument *arg);

EAPI void                   eupnp_service_proxy_new(Eupnp_Service_Info *service, Eupnp_Service_Proxy_Ready_Cb ready_cb, void *data);
EAPI void                   eupnp_service_proxy_free(Eupnp_Service_Proxy *proxy);
EAPI Eupnp_Service_Proxy   *eupnp_service_proxy_ref(Eupnp_Service_Proxy *proxy);
EAPI void                   eupnp_service_proxy_unref(Eupnp_Service_Proxy *proxy);
EAPI void                   eupnp_service_proxy_fetch(Eupnp_Service_Proxy *proxy, const char *base_url, const char *scpd_url);
EAPI Eina_Bool              eupnp_service_proxy_action_add(Eupnp_Service_Proxy *proxy, Eupnp_Service_Action *action);
EAPI Eina_Bool              eupnp_service_proxy_state_variable_add(Eupnp_Service_Proxy *proxy, Eupnp_State_Variable *st);
EAPI Eupnp_State_Variable  *eupnp_service_proxy_state_variable_get(Eupnp_Service_Proxy *proxy, const char *name, int name_len);
EAPI void                   eupnp_service_proxy_dump(Eupnp_Service_Proxy *proxy);
EAPI void                   eupnp_service_proxy_actions_dump(Eupnp_Service_Proxy *proxy);
EAPI void                   eupnp_service_proxy_state_table_dump(Eupnp_Service_Proxy *proxy);

EAPI Eina_Bool              eupnp_service_proxy_has_action(Eupnp_Service_Proxy *proxy, const char *action);
EAPI Eina_Bool              eupnp_service_proxy_action_send(Eupnp_Service_Proxy *proxy, const char *action, Eupnp_Action_Response_Cb response_cb, void *data, ...);

EAPI Eina_Bool               eupnp_service_proxy_has_variable(Eupnp_Service_Proxy *proxy, const char *variable_name);
EAPI Eupnp_Event_Subscriber *eupnp_service_proxy_state_variable_events_subscribe(Eupnp_Service_Proxy *proxy, const char *var_name, Eupnp_State_Variable_Event_Cb cb, Eina_Bool auto_renew, Eina_Bool infinite_subscription, int timeout, void *data);
EAPI Eina_Bool               eupnp_service_proxy_state_variable_events_unsubscribe(Eupnp_Event_Subscriber *subscriber);

#endif /* _EUPNP_SERVICE_PROXY_H */
