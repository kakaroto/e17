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
#include <Ecore.h>

static Eina_Bool
result_(void *data __UNUSED__, int type __UNUSED__, Esql_Res *res)
{
   printf("%i rows returned!\n", esql_res_rows_count(res)); /**< could do more here, but it's a simple example so we just print the number of rows */
   ecore_main_loop_quit();
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
error_(void *data __UNUSED__, int type __UNUSED__, Esql_Res *res)
{
   fprintf(stderr, "%s\n", esql_res_error_get(res)); /**< print error condition */
   ecore_main_loop_quit();
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
connect_(void *data __UNUSED__, int type __UNUSED__, Esql *e)
{
   printf("Connected!\n");
   if (!esql_query_args(e, "SELECT * FROM %s", "jobs")) /**< queue up a simple query */
     {
        fprintf(stderr, "Could not create query!\n");
        ecore_main_loop_quit();
     }
   return ECORE_CALLBACK_RENEW;
}

int
main(void)
{
   Esql *e;
   Eina_Counter *c;
   esql_init();

   eina_log_domain_level_set("esskyuehl", EINA_LOG_LEVEL_DBG);
   ecore_event_handler_add(ESQL_EVENT_CONNECT, (Ecore_Event_Handler_Cb)connect_, NULL);
   ecore_event_handler_add(ESQL_EVENT_RESULT, (Ecore_Event_Handler_Cb)result_, NULL);
   ecore_event_handler_add(ESQL_EVENT_ERROR, (Ecore_Event_Handler_Cb)error_, NULL);

   e = esql_new(ESQL_TYPE_POSTGRESQL); /**< new object for postgresql */
   c = eina_counter_new("esql");
   eina_counter_start(c);
   esql_database_set(e, "zentific"); /**< use database named zentific on connect */
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!esql_connect(e, "127.0.0.1:" ESQL_DEFAULT_PORT_POSTGRESQL, "zentific", "zentific"), 1); /**< connect to localhost at default port */
   ecore_main_loop_begin();
   esql_disconnect(e); /**< disconnect */
   eina_counter_stop(c, 0);
   eina_counter_start(c);
   esql_type_set(e, ESQL_TYPE_MYSQL); /**< now switch to mysql! */
   esql_database_set(e, "zentific"); /**< use database named zentific on connect */
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!esql_connect(e, "127.0.0.1:" ESQL_DEFAULT_PORT_MYSQL, "zentific", "zentific"), 1); /**< connect to localhost at default port */
   ecore_main_loop_begin();
   esql_disconnect(e);
   eina_counter_stop(c, 1);
   printf("Times:\n%s\n", eina_counter_dump(c));
   esql_free(e);
   esql_shutdown();
   return 0;
}
