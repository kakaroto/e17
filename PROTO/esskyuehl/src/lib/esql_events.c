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

#define UPDATE_LISTS(TYPE) do {                                                                          \
       if (e->backend_set_funcs && (e->backend_set_funcs->data == esql_##TYPE))                          \
         {                                                                                               \
            free(e->backend_set_params->data);                                                           \
            e->backend_set_funcs = eina_list_remove_list(e->backend_set_funcs, e->backend_set_funcs);    \
            e->backend_set_params = eina_list_remove_list(e->backend_set_params, e->backend_set_params); \
            e->backend_ids = eina_list_remove_list(e->backend_ids, e->backend_ids);                      \
         }                                                                                               \
  } while (0)

static void
esql_fake_free(void *data  __UNUSED__,
               Esql *e)
{
   e->error = NULL;
}

static void
esql_call_complete(Esql *e)
{
   Esql_Set_Cb cb;

   DBG("(e=%p)", e);
   switch (e->current)
     {
      case ESQL_CONNECT_TYPE_INIT:
        INFO("Connected");
        e->connected = EINA_TRUE;

        if (e->connect_cb)
          e->connect_cb(e, e->connect_cb_data);
        else
          ecore_event_add(ESQL_EVENT_CONNECT, e, (Ecore_End_Cb)esql_fake_free, NULL);
        break;

      case ESQL_CONNECT_TYPE_DATABASE_SET:
        INFO("Working database is now '%s'", e->database);
        UPDATE_LISTS(database_set);
        break;

      case ESQL_CONNECT_TYPE_QUERY:
        UPDATE_LISTS(query);
        {
           Esql_Res *res;
           Esql_Query_Cb qcb;

           res = calloc(1, sizeof(Esql_Res));
           EINA_SAFETY_ON_NULL_GOTO(res, out);
           res->e = e;
           e->backend.res(res);
           res->data = e->cur_data;
           qcb = eina_hash_find(esql_query_callbacks, &e->cur_id);
           if (qcb)
             {
                INFO("Executing callback for current query");
                qcb(res, e->cur_data);
                
                eina_hash_del_by_key(esql_query_callbacks, &e->cur_id);
                esql_res_free(NULL, res);
             }
           else
             {
                INFO("Emitting event for current query");
                ecore_event_add(ESQL_EVENT_RESULT, res, (Ecore_End_Cb)esql_res_free, NULL);
             }
        }
        break;

      default:
        break;
     }
out:
   e->current = ESQL_CONNECT_TYPE_NONE;
   e->error = NULL;
   if (!e->backend_set_funcs)
     {
        INFO("No calls queued");
        return;
     }
   /* next call */
   cb = e->backend_set_funcs->data;
   if (cb == (Esql_Set_Cb)esql_database_set)
     {
        cb(e, e->backend_set_params->data);
        INFO("Next call: DB change");
     }
   else if (cb == (Esql_Set_Cb)esql_query)
     {
        void *data;

        data = eina_hash_find(esql_query_data, e->backend_ids->data);
        e->backend.query(e, e->backend_set_params->data);
        e->current = ESQL_CONNECT_TYPE_QUERY;
        e->cur_data = data;
        e->cur_id = *((Esql_Query_Id*)e->backend_ids->data);
        if (data) eina_hash_del_by_key(esql_query_data, e->backend_ids->data);
        INFO("Next call: query");
     }
   esql_connect_handler(e, e->fdh); /* have to call again to start next call */
}

Eina_Bool
esql_connect_handler(Esql             *e,
                     Ecore_Fd_Handler *fdh)
{
   DBG("(e=%p, fdh=%p)", e, fdh);

   if (!ecore_main_fd_handler_active_get(fdh, ECORE_FD_READ | ECORE_FD_WRITE))
     return ECORE_CALLBACK_RENEW;
   switch (e->backend.io(e))
     {
      case 0:
        esql_call_complete(e);
        ecore_main_fd_handler_active_set(fdh, ECORE_FD_WRITE);
        break;

      case ECORE_FD_READ | ECORE_FD_WRITE:
        ecore_main_fd_handler_active_set(fdh, ECORE_FD_READ | ECORE_FD_WRITE);
        break;

      case ECORE_FD_READ:
        ecore_main_fd_handler_active_set(fdh, ECORE_FD_READ);
        break;

      case ECORE_FD_WRITE:
        ecore_main_fd_handler_active_set(fdh, ECORE_FD_WRITE);
        break;

      default:
        e->error = e->backend.error_get(e);
        if (e->type == ESQL_CONNECT_TYPE_QUERY)
          {
             Esql_Query_Cb qcb;

             qcb = eina_hash_find(esql_query_callbacks, &e->cur_id);
             if (qcb)
               {
                  Esql_Res *res;

                  res = calloc(1, sizeof(Esql_Res));
                  EINA_SAFETY_ON_NULL_RETURN_VAL(res, ECORE_CALLBACK_RENEW);
                  res->e = e;
                  res->data = e->cur_data;
                  res->qid = e->cur_id;
                  e->res = res;
                  
                  res->error = e->error;
                  ERR("Connection error: %s", res->error);

                  INFO("Executing callback for current event");
                  qcb(res, e->cur_data);
                  
                  eina_hash_del_by_key(esql_query_callbacks, &e->cur_id);
                  esql_res_free(NULL, res);
               }
             else
               ecore_event_add(ESQL_EVENT_ERROR, e, (Ecore_End_Cb)esql_fake_free, NULL);
          }
        else
          {
             if (e->connect_cb)
               e->connect_cb(e, e->connect_cb_data);
             else
               ecore_event_add(ESQL_EVENT_ERROR, e, (Ecore_End_Cb)esql_fake_free, NULL);
          }

        e->fdh = NULL;
        return ECORE_CALLBACK_CANCEL;
     }
   return ECORE_CALLBACK_RENEW;
}

