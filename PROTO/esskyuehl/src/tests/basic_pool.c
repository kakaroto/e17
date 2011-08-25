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

#define QUERY "select salt, hash from users where uid='0'"
//#define QUERY2 "select * from jobs"
#define QUERY3 "select col, num from blah"

static void
callback_(Esql_Res *res, char *data)
{
   static int x;
   const char *q = (x % 2) ? QUERY3 : QUERY;
   if (esql_res_error_get(res)) fprintf(stderr, "ERROR: %s\n", esql_res_error_get(res)); /**< print error condition */
   else printf("%i rows returned to callback!\n", esql_res_rows_count(res)); /**< could do more here, but it's a simple example so we just print the number of rows */
   printf("data stored: '%s'\n", data);
   printf("Query string: '%s'\n", esql_res_query_get(res));
   if (++x == 4) { ecore_main_loop_quit(); return; }
   printf("adding query '%s'\n", q);
   esql_query_callback_set(esql_query(esql_res_esql_get(res), strdup("test data"), q), (Esql_Query_Cb)callback_);
}

static Eina_Bool
connect_(void *data __UNUSED__, int type __UNUSED__, Esql *e)
{
   Esql_Query_Id id;

   printf("Connected!\n");
   id = esql_query(e, strdup("test data"), QUERY3);
   if (!id) /**< queue up a simple query */
     {
        fprintf(stderr, "Could not create query!\n");
        ecore_main_loop_quit();
     }
   else esql_query_callback_set(id, (Esql_Query_Cb)callback_);
   return ECORE_CALLBACK_RENEW;
}

int
main(void)
{
   Esql *e;
   Eina_Counter *c;

   esql_init();

   eina_log_domain_level_set("esskyuehl", EINA_LOG_LEVEL_INFO);
   ecore_event_handler_add(ESQL_EVENT_CONNECT, (Ecore_Event_Handler_Cb)connect_, NULL);

   e = esql_pool_new(5, ESQL_TYPE_MYSQL); /**< new object for postgresql */
   esql_database_set(e, "zentific"); /**< use database named zentific on connect */
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!esql_connect(e, "127.0.0.1:" ESQL_DEFAULT_PORT_MYSQL, "zentific", "zentific"), 1); /**< connect to localhost at default port */
   ecore_main_loop_begin();

   esql_free(e);
   esql_shutdown();
   return 0;
}
