/*
 * Copyright 2011 Mike Blumenkrantz <mike@zentific.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 2.1 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Esskyuehl.h>
#include <esql_private.h>

static Esql *
esql_pool_idle_find_(Esql_Pool *ep)
{
   Esql *e, *use = NULL;
   double time, cur = 0.0;

   EINA_INLIST_FOREACH(ep->esqls, e)
     {
        if (!e->current) /* idle! */
          return e;
     }
   /* no free connections :( */
   time = ecore_time_get();
   EINA_INLIST_FOREACH(ep->esqls, e)
     {
        if (time - e->query_start > cur) /* assume older query start time means faster return (obviously not always true) */
          {
             cur = time - e->query_start;
             use = e;
          }
     }
   if (!use) /* this should never happen in a reasonable setting */
     use = EINA_INLIST_CONTAINER_GET(ep->esqls, Esql);  /* use first one */
   return use;
}

Eina_Bool
esql_pool_rebalance(Esql_Pool *ep, Esql *e /* idle connection */)
{
   Esql *it;

   DBG("(ep=%p, e=%p)", ep, e);
   EINA_INLIST_FOREACH(ep->esqls, it)
     {
        if (it->backend_set_funcs && (it->backend_set_funcs->data == (Esql_Set_Cb)esql_query)) /* queued call */
          {
             INFO("Load balancing: moving query (%lu) from %u to %u", (Esql_Query_Id)((uintptr_t)it->backend_ids->data), it->pool_id, e->pool_id);
#ifdef HAVE_EINA_LIST_MOVE_LIST
             eina_list_move_list(&e->backend_set_funcs, &it->backend_set_funcs, it->backend_set_funcs);
             eina_list_move_list(&e->backend_set_params, &it->backend_set_params, it->backend_set_params);
             eina_list_move_list(&e->backend_ids, &it->backend_ids, it->backend_ids);
#else
#define LIST_MOVE(X) \
  e->X = eina_list_append(e->X, it->X->data); \
  it->X = eina_list_remove_list(it->X, it->X)
             LIST_MOVE(backend_set_funcs);
             LIST_MOVE(backend_set_params);
             LIST_MOVE(backend_ids);
#undef LIST_MOVE
#endif
             INFO("Current queue:  #%u:(%lu):%i | #%u:(%lu):%i", e->pool_id, (Esql_Query_Id)((uintptr_t)e->backend_ids->data), eina_list_count(e->backend_ids),
               it->pool_id, it->backend_ids ? (Esql_Query_Id)((uintptr_t)it->backend_ids->data) : 0, eina_list_count(it->backend_ids));
             return EINA_TRUE;
          }
     }
   INFO("No rebalancing necessary");
   return EINA_FALSE;
}

void
esql_pool_free(Esql_Pool *ep)
{
   Esql *e;
   Eina_Inlist *l;

   EINA_INLIST_FOREACH_SAFE(ep->esqls, l, e)
     esql_free(e);

   eina_stringshare_del(ep->database);
   free(ep);
}

Eina_Bool
esql_pool_type_set(Esql_Pool *ep,
                   Esql_Type  type)
{
   Esql *e;
   Eina_Bool ret = EINA_TRUE;

   if (type == ep->type) return EINA_TRUE;

   EINA_INLIST_FOREACH(ep->esqls, e)
     if (!esql_type_set(e, type)) ret = EINA_FALSE;

   if (ret) ep->type = type;
   return ret;
}

Eina_Bool
esql_pool_database_set(Esql_Pool  *ep,
                       const char *database_name)
{
   Esql *e;

   EINA_INLIST_FOREACH(ep->esqls, e)
     esql_database_set(e, database_name);
   return EINA_TRUE;
}

Eina_Bool
esql_pool_connect(Esql_Pool  *ep,
                  const char *addr,
                  const char *user,
                  const char *passwd)
{
   Esql *e;
   Eina_Bool ret = EINA_TRUE;

   EINA_INLIST_FOREACH(ep->esqls, e)
     {
        if (!e->connected)
          {
             if (!esql_connect(e, addr, user, passwd))
               ret = EINA_FALSE;
          }
     }

   return ret;
}

void
esql_pool_disconnect(Esql_Pool *ep)
{
   Esql *e;

   EINA_INLIST_FOREACH(ep->esqls, e)
     if (e->connected) esql_disconnect(e);
   ep->connected = EINA_FALSE;
   ep->e_connected = 0;
   ecore_event_add(ESQL_EVENT_DISCONNECT, ep, (Ecore_End_Cb)esql_fake_free, NULL);
}

Esql_Query_Id
esql_pool_query_args(Esql_Pool  *ep,
                     void       *data,
                     const char *fmt,
                     va_list     args)
{
   Esql *e;

   e = esql_pool_idle_find_(ep);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, 0);
   return esql_query_vargs(e, data, fmt, args);
}

Esql_Query_Id
esql_pool_query(Esql_Pool  *ep,
                void       *data,
                const char *query)
{
   Esql *e;

   e = esql_pool_idle_find_(ep);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, 0);
   return esql_query(e, data, query);
}

void
esql_pool_connect_timeout_set(Esql_Pool *ep,
                              double     timeout)
{
   Esql *e;

   EINA_INLIST_FOREACH(ep->esqls, e)
     {
        e->timeout = timeout;
        if (timeout > 0.0)
          {
             if (e->timeout_timer) ecore_timer_delay(e->timeout_timer, e->timeout - ecore_timer_pending_get(e->timeout_timer));
             else e->timeout_timer = ecore_timer_add(timeout, (Ecore_Task_Cb)esql_timeout_cb, e);
          }
        else
          {
             ecore_timer_del(e->timeout_timer);
             e->timeout_timer = NULL;
             e->timeout = 0;
          }
     }
}

void
esql_pool_reconnect_set(Esql_Pool *ep,
                        Eina_Bool  enable)
{
   Esql *e;

   EINA_INLIST_FOREACH(ep->esqls, e)
     e->reconnect = enable;
}

/* API */

/**
 * @brief Create an Esql connection pool
 * This function creates a connection pool with @p size connections to @p type
 * of database. The returned object can be used with all functions that an #Esql object
 * can be used in. The internal objects are never accessible, and events/callbacks will
 * always return the pool object.
 * The CONNECTED event/callback will be called once all connections have been made successfully,
 * but ERROR events will occur individually for each connection, returning the pool object as the event.
 * @note esql_current_query_id_get will not work with pools for obvious reasons!
 * @param size The number of connections for the pool
 * @param type The database type for the pool
 * @return The #Esql object representing a connection pool
 */
Esql *
esql_pool_new(int       size,
              Esql_Type type)
{
   int i;
   Esql *e;
   Esql_Pool *ep;

   EINA_SAFETY_ON_TRUE_RETURN_VAL(size < 1, NULL);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(type == ESQL_TYPE_NONE, NULL);
   ep = calloc(1, sizeof(Esql_Pool));
   EINA_SAFETY_ON_NULL_RETURN_VAL(ep, NULL);

   for (i = 0; i < size; i++)
     {
        e = esql_new(type);
        EINA_SAFETY_ON_NULL_GOTO(e, error);
        e->pool_member = EINA_TRUE;
        e->pool_struct = ep;
        e->pool_id = i;
        ep->esqls = eina_inlist_append(ep->esqls, EINA_INLIST_GET(e));
     }
   ep->pool = EINA_TRUE;
   ep->type = type;
   ep->size = size;
   return (Esql *)ep;

error:
   EINA_INLIST_FOREACH(ep->esqls, e)
     esql_free(e);
   free(ep);
   return NULL;
}

