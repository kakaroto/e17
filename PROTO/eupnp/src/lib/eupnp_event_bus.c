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

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>

#include <Eina.h>

#include "Eupnp.h"
#include "eupnp_private.h"


/**
 * @addtogroup Eupnp_Event_Bus Event Bus
 *
 * The event bus module abstracts communication between components by
 * implementing the Publish/subscribe pattern. On this paradigm, the publisher
 * publishes a message on the bus, which routes the messages to interested
 * subscribers (if any).
 *
 * @section eupnp_event_bus_initialize Initializing the Bus
 *
 * Before using the bus, it must be initialized by calling
 * eupnp_event_bus_init(). When the bus is not needed anymore, it must be shut
 * down with eupnp_event_bus_shutdown() so that resources allocated are freed.
 *
 * @section eupnp_event_bus_subscribe Subscribing the Bus
 *
 * A subscriber subscribes on the bus for a specific event type (see @ref
 * Eupnp_Event_Type) by calling eupnp_event_bus_subscribe(). Messages
 * published on the type are forwarded to the @ref Eupnp_Callback passed.
 *
 * Here we illustrate how to subscribe a callback to a built-in @ref Eupnp_Event_Type :
 *
 * @code
 * #include <Eupnp.h>
 *
 * static Eina_Bool
 * subscriber(void *user_data, Eupnp_Event_Type event_type, void *event_data)
 * {
 *     // Event posted on the bus
 * }
 *
 * int
 * main(int argc, char *argv[])
 * {
 *     eupnp_event_bus_init();
 *
 *     eupnp_event_bus_subscribe(EUPNP_EVENT_SERVICE_FOUND, EUPNP_CALLBACK(subscriber), NULL);
 *
 *     // Application main procedure
 *     main_loop();
 *
 *     // Shutdown procedure
 *     eupnp_event_bus_shutdown();
 *     return 0;
 * }
 * @endcode
 *
 * @section eupnp_event_bus_unsubscribe Unsubscribing
 *
 * When subscribing, eupnp_event_bus_subscribe() returns a @ref
 * Eupnp_Event_Subscriber object, which can be used for unsubscribing (see
 * eupnp_event_bus_unsubscribe()).
 *
 * During shutdown procedure, the bus automatically unsubscribes callbacks and
 * frees all resources allocated, so, it's not obligatory for the user to
 * unsubscribe every callback.
 *
 * @section eupnp_event_bus_publish Publishing to the Bus
 *
 * For publishing to the bus, one must use the eupnp_event_bus_publish()
 * function.
 *
 * @code
 * eupnp_event_bus_publish(EUPNP_EVENT_SERVICE_FOUND, service_object);
 * @endcode
 *
 * @subsection eupnp_event_bus_publish_custom Custom Event Types
 *
 * Custom event types can be created with the eupnp_event_bus_event_type_new()
 * function. Calls to this function always return a new event type.
 *
 * @code
 * // Create a custom event type
 * Eupnp_Event_Type ev = eupnp_event_bus_event_type_new();
 *
 * // Publishing an event with the custom type
 * eupnp_event_bus_publish(ev, some_object);
 * @endcode
 *
 * 
 *
 */

/*
 * Private API
 */

static Eina_List *subscribers = NULL;
static int _log_dom = -1;
static unsigned int _event_max = EUPNP_EVENT_COUNT;

#undef DBG
#undef INF
#undef WRN
#undef ERR
#undef CRIT
#define DBG(...) EINA_LOG_DOM_DBG(_log_dom, __VA_ARGS__)
#define INF(...) EINA_LOG_DOM_INFO(_log_dom, __VA_ARGS__)
#define WRN(...) EINA_LOG_DOM_ERR(_log_dom, __VA_ARGS__)
#define ERR(...) EINA_LOG_DOM_ERR(_log_dom, __VA_ARGS__)
#define CRIT(...) EINA_LOG_DOM_CRIT(_log_dom, __VA_ARGS__)

/*
 * Constructor for the Eupnp_Subscriber class.
 */
static Eupnp_Subscriber *
eupnp_subscriber_new(Eupnp_Event_Type event_type, Eupnp_Callback cb, void *user_data)
{
   Eupnp_Subscriber *s;

   s = malloc(sizeof(Eupnp_Subscriber));

   if (!s)
     {
	ERR("failed to alloc subscriber");
	eina_error_set(EINA_ERROR_OUT_OF_MEMORY);
	return NULL;
     }

   s->cb = cb;
   s->type = event_type;
   s->user_data = user_data;
   s->deleted = EINA_FALSE;
   return s;
}

/*
 * Destructor for the Eupnp_Subscriber class.
 */
static void
eupnp_subscriber_free(Eupnp_Subscriber *s)
{
   free(s);
}

/*
 * Walks on the subscribers list and destroys each of them. Used during
 * shutdown.
 */
static void
eupnp_event_bus_clear_subscribers(void)
{
   Eupnp_Subscriber *s;

   EINA_LIST_FREE(subscribers, s)
     eupnp_subscriber_free(s);
}

/*
 * Public API
 */

/**
 * Initializes the event bus module.
 *
 * @return On error, returns 0. Otherwise, returns the number of times it's been
 *         called.
 */
Eina_Bool
eupnp_event_bus_init(void)
{
   if ((_log_dom =
	eina_log_domain_register("Eupnp.EventBus", EINA_COLOR_BLUE)) < 0)
     {
	EINA_LOG_DOM_ERR
	  (EUPNP_LOGGING_DOM_GLOBAL,
	   "Failed to create event bus logging domain.");
	return EINA_FALSE;
     }

   INF("Initializing event bus module.");

   return EINA_TRUE;
}

/**
 * Shuts down the event bus module.
 *
 * @return 0 if completely shutted down the module.
 */
Eina_Bool
eupnp_event_bus_shutdown(void)
{
   INF("Shutting down event bus module.");
   eupnp_event_bus_clear_subscribers();
   eina_log_domain_unregister(_log_dom);
   return EINA_TRUE;
}

/**
 * Publishes an event on the bus.
 *
 * @param event_type Event type to be published
 * @param event_data Event data
 *
 * @see eupnp_event_bus_subscribe(), eupnp_event_bus_unsubscribe(), eupnp_event_bus_event_type_new()
 */
EAPI void
eupnp_event_bus_publish(Eupnp_Event_Type event_type, void *event_data)
{
   if (event_type >= _event_max) return;
   Eina_List *l, *l_next;
   Eupnp_Subscriber *s;

   DBG("Publishing event type %d", event_type);

   // Publish
   EINA_LIST_FOREACH(subscribers, l, s)
      if (s->type == event_type)
        {
	   DBG("Found interested subscriber at %p", s);
	   if (!s->cb(s->user_data, event_type, event_data))
	     {
		DBG("Subscriber %p requested unsubscribe (returned false)", s);
		s->deleted = EINA_TRUE;
	     }
	}

   // Perform unsubscriptions
   EINA_LIST_FOREACH_SAFE(subscribers, l, l_next, s)
      if (s->deleted)
	{
	   DBG("Unsubscribing subscriber %p (%d)", s, s->type);
	   subscribers = eina_list_remove(subscribers, s);
	   eupnp_subscriber_free(s);
	}
}


/**
 * Subscribes a callback on the bus.
 *
 * Subscribes a callback on the bus for a specific event type. Whenever events
 * of this type are published, the subscribed callbacks receive the event.
 *
 * @param event_type Event type this callback expects
 * @param cb Callback to receive events of type event_type
 * @param user_data Additional user data passed to the callback
 *
 * @note Unsubscribing is a feature, not an obligation. During bus shutdown
 *       all remaining subscribers are unsubscribed and destroyed.
 *
 * @return A handler that can be used for unsubscribing - an Eupnp_Subscriber
 *         instance.
 *
 * @see eupnp_event_bus_unsubscribe(), eupnp_event_bus_event_type_new()
 */
EAPI Eupnp_Subscriber *
eupnp_event_bus_subscribe(Eupnp_Event_Type event_type, Eupnp_Callback cb, void *user_data)
{
   CHECK_NULL_RET(cb, NULL);

   if (event_type >= _event_max)
     {
	ERR("Failed to subscribe for event type %d, callback %p, data %p", event_type, cb, user_data);
	return NULL;
     }

   Eupnp_Subscriber *s;
   s = eupnp_subscriber_new(event_type, cb, user_data);

   subscribers = eina_list_append(subscribers, s);

   DBG("Subscribing callback %p for events %d, data %p", cb, event_type, user_data);

   return s;
}

/**
 * Unsubscribes a callback from the bus.
 *
 * Removes a callback subscription from the bus, given the Eupnp_Subscriber
 * handler received during subscription.
 *
 * @param s subscription handler - an Eupnp_Subscriber instance.
 *
 * @see eupnp_event_bus_subscribe()
 */
EAPI void
eupnp_event_bus_unsubscribe(Eupnp_Subscriber *s)
{
   CHECK_NULL_RET(s);

   Eina_List *l, *l_next;
   Eupnp_Subscriber *it;

   EINA_LIST_FOREACH_SAFE(subscribers, l, l_next, it)
      if (it == s)
	{
	   DBG("Unsubscribing subscriber %p", s);
	   subscribers = eina_list_remove(subscribers, it);
	   eupnp_subscriber_free(it);
	   return;
	}

   WRN("Could not find subscriber %p on subscribers list", s);
}

/**
 * Creates a new event type.
 *
 * Creates a new event type that can be posted on the bus.
 *
 * @return Event type created
 */
EAPI Eupnp_Event_Type
eupnp_event_bus_event_type_new(void)
{
   _event_max++;
   return _event_max - 1;
}

/**
 * Returns whether the type has subscribers.
 *
 * @return EINA_TRUE if the type @p type has any subscribers, EINA_FALSE
 * otherwise.
 */
EAPI Eina_Bool
eupnp_event_bus_type_has_subscriber(Eupnp_Event_Type type)
{
   Eina_List *l;
   Eupnp_Subscriber *s;

   EINA_LIST_FOREACH(subscribers, l, s)
     if (s->type == type) return EINA_TRUE;

   return EINA_FALSE;
}
