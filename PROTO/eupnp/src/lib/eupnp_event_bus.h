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

#ifndef _EUPNP_EVENT_BUS_H
#define _EUPNP_EVENT_BUS_H

#include <Eina.h>
#include "Eupnp.h"

typedef struct _Eupnp_Subscriber Eupnp_Subscriber;

/**
 * @enum Eupnp_Event_Type
 *
 * Built-in event types.
 *
 * @see eupnp_event_bus_event_type_new()
 */
typedef enum {
   EUPNP_EVENT_NONE,
   EUPNP_EVENT_DEVICE_FOUND,
   EUPNP_EVENT_DEVICE_GONE,
   EUPNP_EVENT_SERVICE_FOUND,
   EUPNP_EVENT_SERVICE_GONE,
   EUPNP_EVENT_DEVICE_READY,
   EUPNP_EVENT_COUNT,
} Eupnp_Event_Type;

/**
 * @typedef Eupnp_Callback
 *
 * A generic callback used in many parts of the Eupnp library.
 *
 * @see EUPNP_CALLBACK()
 */
typedef Eina_Bool (*Eupnp_Callback) (void *user_data, Eupnp_Event_Type event_type, void *event_data);

/**
 * @def EUPNP_CALLBACK(func)
 *
 * Converts a function to an Eupnp_Callback callback type.
 *
 * @param func Function to convert to an Eupnp_Callback
 */
#define EUPNP_CALLBACK(func) ((Eupnp_Callback) func)

struct _Eupnp_Subscriber {
   Eupnp_Event_Type type;
   Eupnp_Callback cb;
   Eina_Bool deleted:1;
   void *user_data;
};

EAPI void               eupnp_event_bus_publish(Eupnp_Event_Type event_type, void *event_data);
EAPI Eupnp_Subscriber  *eupnp_event_bus_subscribe(Eupnp_Event_Type event_type, Eupnp_Callback cb, void *user_data) EINA_ARG_NONNULL(2);
EAPI void               eupnp_event_bus_unsubscribe(Eupnp_Subscriber *s) EINA_ARG_NONNULL(1);

EAPI Eupnp_Event_Type   eupnp_event_bus_event_type_new(void);


#endif /* _EUPNP_EVENT_BUS_H */
