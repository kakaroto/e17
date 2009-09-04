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
#include "eupnp_log.h"
#include "eupnp_event_bus.h"
#include "eupnp_private.h"


/*
 * Private API
 */

static Eina_List *subscribers = NULL;
static int _eupnp_event_bus_main_count = 0;
static int _log_dom = -1;
static int _event_max = EUPNP_EVENT_COUNT;

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
	ERROR_D(_log_dom, "failed to alloc subscriber");
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

/*
 * Initializes the event bus module.
 *
 * @return On error, returns 0. Otherwise, returns the number of times it's been
 * called.
 */
EAPI int
eupnp_event_bus_init(void)
{
   if (_eupnp_event_bus_main_count) return ++_eupnp_event_bus_main_count;

   if (!eina_error_init())
     {
	fprintf(stderr, "Failed to initialize eina error module.\n");
	return _eupnp_event_bus_main_count;
     }

   if (!eupnp_log_init())
     {
	fprintf(stderr, "Failed to initialize eupnp log module.\n");
	eina_error_shutdown();
	return _eupnp_event_bus_main_count;
     }

   if ((_log_dom = eina_log_domain_register("Eupnp.EventBus", EINA_COLOR_BLUE)) < 0)
     {
	ERROR("Failed to create event bus logging domain.");
	eupnp_log_shutdown();
	return _eupnp_event_bus_main_count;
     }

   if (!eina_list_init())
     {
	ERROR("Failed to initialize eina list module");
	eupnp_log_shutdown();
	return _eupnp_event_bus_main_count;
     }

   INFO_D(_log_dom, "Initializing event bus module.");

   return ++_eupnp_event_bus_main_count;
}

/*
 * Shuts down the event bus module.
 *
 * @return 0 if completely shutted down the module.
 */
EAPI int
eupnp_event_bus_shutdown(void)
{
   if (_eupnp_event_bus_main_count != 1) return --_eupnp_event_bus_main_count;

   INFO_D(_log_dom, "Shutting down event bus module.");

   eupnp_event_bus_clear_subscribers();
   eina_list_shutdown();
   eina_log_domain_unregister(_log_dom);
   eupnp_log_shutdown();
   eina_error_shutdown();

   return --_eupnp_event_bus_main_count;
}

/*
 * Publishes an event on the bus.
 *
 * @param event_type Event type to be published
 * @param event_data Event data
 */
EAPI void
eupnp_event_bus_publish(Eupnp_Event_Type event_type, void *event_data)
{
   if (event_type <= EUPNP_EVENT_NONE) return;
   if (event_type >= _event_max) return;
   Eina_List *l, *l_next;
   Eupnp_Subscriber *s;

   DEBUG_D(_log_dom, "Publishing event type %d", event_type);

   // Publish
   EINA_LIST_FOREACH(subscribers, l, s)
      if (s->type == event_type)
        {
	   DEBUG_D(_log_dom, "Found interested subscriber at %p", s);
	   if (!s->cb(s->user_data, event_type, event_data))
	     {
		DEBUG_D(_log_dom, "Subscriber %p requested unsubscribe (returned false)", s);
		s->deleted = EINA_TRUE;
	     }
	}

   // Perform unsubscriptions
   EINA_LIST_FOREACH_SAFE(subscribers, l, l_next, s)
      if (s->deleted)
	{
	   DEBUG_D(_log_dom, "Unsubscribing subscriber %p (%d)", s, s->type);
	   subscribers = eina_list_remove(subscribers, s);
	   eupnp_subscriber_free(s);
	}
}


/*
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
 */
EAPI Eupnp_Subscriber *
eupnp_event_bus_subscribe(Eupnp_Event_Type event_type, Eupnp_Callback cb, void *user_data)
{
   CHECK_NULL_RET_VAL(cb, NULL);
   if (event_type <= EUPNP_EVENT_NONE || event_type >= _event_max)
     {
	ERROR_D(_log_dom, "Failed to subscribe for event type %d, callback %p, data %p", event_type, cb, user_data);
	return NULL;
     }

   Eupnp_Subscriber *s;
   s = eupnp_subscriber_new(event_type, cb, user_data);

   subscribers = eina_list_append(subscribers, s);

   DEBUG_D(_log_dom, "Subscribing callback %p for events %d, data %p", cb, event_type, user_data);

   return s;
}

/*
 * Unsubscribes a callback from the bus.
 *
 * Removes a callback subscription from the bus, given the Eupnp_Subscriber
 * handler received during subscription.
 *
 * @param s subscription handler - an Eupnp_Subscriber instance.
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
	   DEBUG_D(_log_dom, "Unsubscribing subscriber %p", s);
	   subscribers = eina_list_remove(subscribers, it);
	   eupnp_subscriber_free(it);
	   return;
	}

   WARN_D(_log_dom, "Could not find subscriber %p on subscribers list", s);
}

EAPI Eupnp_Event_Type
eupnp_event_bus_event_type_new(void)
{
   _event_max++;
   return _event_max - 1;
}
