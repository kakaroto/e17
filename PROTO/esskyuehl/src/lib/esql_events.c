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

#define UPDATE_LISTS(TYPE) do { \
  if (e->backend_set_funcs && (e->backend_set_funcs->data == e->backend.TYPE)) \
    { \
       free(e->backend_set_params->data); \
       e->backend_set_funcs = eina_list_remove_list(e->backend_set_funcs, e->backend_set_funcs); \
       e->backend_set_params = eina_list_remove_list(e->backend_set_params, e->backend_set_params); \
    } \
} while (0)

static void
esql_fake_free(void *data __UNUSED__, void *data2 __UNUSED__)
{
}

static void
esql_call_complete(Esql *e)
{
   Esql_Set_Cb cb;
   int ret;
   
   switch (e->type)
     {
      case ESQL_CONNECT_TYPE_INIT:
        e->connected = EINA_TRUE;
        break;
      case ESQL_CONNECT_TYPE_DATABASE_SET:
        UPDATE_LISTS(database_set);
        break;
      case ESQL_CONNECT_TYPE_QUERY:
        UPDATE_LISTS(query);
        {
           Esql_Res *res;

           res = calloc(1, sizeof(Esql_Res));
           EINA_SAFETY_ON_NULL_GOTO(res, out);
           res->e = e;
           e->backend.res(res);
           ecore_event_add(ESQL_EVENT_RESULT, res, (Ecore_End_Cb)esql_res_free, NULL);
        }
        break;
      default:
        break;
     }
out:
   if (!e->backend_set_funcs) return;
   /* next call */
   cb = e->backend_set_funcs->data;
   cb(e, e->backend_set_params->data);
   ret = e->backend.io(e);
   if (ret == ECORE_FD_ERROR)
     {
        ERR("Connection error: %s", e->backend.error_get(e));
        return;
     }
   ecore_main_fd_handler_active_set(e->fdh, ret);
   if (cb == (Esql_Set_Cb)esql_database_set)
     e->current = ESQL_CONNECT_TYPE_DATABASE_SET;
   else if (cb == (Esql_Set_Cb)esql_query)
     e->current = ESQL_CONNECT_TYPE_QUERY;
}

Eina_Bool
esql_connect_handler(Esql *e, Ecore_Fd_Handler *fdh)
{
   switch (e->backend.io(e))
     {
      case 0:
        esql_call_complete(e);
        break;
      case ECORE_FD_READ:
        ecore_main_fd_handler_active_set(fdh, ECORE_FD_READ);
        break;
      case ECORE_FD_WRITE:
        ecore_main_fd_handler_active_set(fdh, ECORE_FD_WRITE);
        break;
      default:
        ERR("Connection error: %s", e->backend.error_get(e));
        {
           Esql_Res *res;

           res = calloc(1, sizeof(Esql_Res));
           EINA_SAFETY_ON_NULL_GOTO(res, out);
           res->e = e;
           res->error = e->backend.error_get(e);
           ecore_event_add(ESQL_EVENT_ERROR, res, (Ecore_End_Cb)esql_res_free, NULL);
        }
        return ECORE_CALLBACK_CANCEL;
     }
   return ECORE_CALLBACK_RENEW;
}
