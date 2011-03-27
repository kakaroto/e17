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

static void
print_results(Esql_Res *res)
{
   Eina_Iterator *i;
   Esql_Row *r;

   i = esql_res_row_iterator_new(res);
   EINA_ITERATOR_FOREACH(i, r)
     {
        Eina_Inlist *l;
        Esql_Cell *c;
        l = esql_row_cells_get(r);
        EINA_INLIST_FOREACH(l, c)
          {
             printf("Column name: %s --- Type: ", c->colname);
             switch (c->type)
               {
                case ESQL_CELL_TYPE_TIMESTAMP:
                   printf("ESQL_CELL_TYPE_TIMESTAMP --- Value: timestamp\n");
                   break;
                case ESQL_CELL_TYPE_TIME:
                   printf("ESQL_CELL_TYPE_TIME --- Value: timeval\n");
                   break;
                case ESQL_CELL_TYPE_FLOAT:
                   printf("ESQL_CELL_TYPE_FLOAT --- Value: %f\n", c->value.f);
                   break;
                case ESQL_CELL_TYPE_DOUBLE:
                   printf("ESQL_CELL_TYPE_DOUBLE --- Value: %g\n", c->value.d);
                   break;
                case ESQL_CELL_TYPE_TINYINT:
                   printf("ESQL_CELL_TYPE_TINYINT --- Value: %i\n", c->value.c);
                   break;
                case ESQL_CELL_TYPE_SHORT:
                   printf("ESQL_CELL_TYPE_SHORT --- Value: %i\n", c->value.s);
                   break;
                case ESQL_CELL_TYPE_LONG:
                   printf("ESQL_CELL_TYPE_LONG --- Value: %i\n", c->value.i);
                   break;
                case ESQL_CELL_TYPE_LONGLONG:
                   printf("ESQL_CELL_TYPE_LONGLONG --- Value: %lli\n", c->value.l);
                   break;
                case ESQL_CELL_TYPE_STRING:
                   printf("ESQL_CELL_TYPE_STRING --- Value: %s\n", c->value.string);
                   break;
                case ESQL_CELL_TYPE_BLOB:
                   printf("ESQL_CELL_TYPE_BLOB --- Value: %*s\n", c->len, c->value.blob);
                   break;
                default:
                   printf("ESQL_CELL_TYPE_UNKNOWN --- Value: UNKNOWN\n");
                   break;
               }
          }
     }
}

static void
callback_(Esql_Res *res, char *data)
{
   printf("%i rows returned to callback!\n", esql_res_rows_count(res)); /**< could do more here, but it's a simple example so we just print the number of rows */
   printf("data stored: '%s'\n", data);
   printf("Query string: '%s'\n", esql_res_query_get(res));
   print_results(res);
   free(data);
   ecore_main_loop_quit();
}

static Eina_Bool
result_(void *data __UNUSED__, int type __UNUSED__, Esql_Res *res)
{
   printf("%i rows returned!\n", esql_res_rows_count(res)); /**< could do more here, but it's a simple example so we just print the number of rows */
   printf("data stored: '%s'\n", (char*)esql_res_data_get(res));
   printf("Query string: '%s'\n", esql_res_query_get(res));
   free(esql_res_data_get(res));
   print_results(res);
   ecore_main_loop_quit();
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
error_(void *data __UNUSED__, int type __UNUSED__, Esql *e)
{
   fprintf(stderr, "%s\n", esql_error_get(e)); /**< print error condition */
   printf("Query string: '%s'\n", esql_current_query_get(e));
   ecore_main_loop_quit();
   return ECORE_CALLBACK_RENEW;
}

static Eina_Bool
connect_(void *data __UNUSED__, int type __UNUSED__, Esql *e)
{
   Esql_Query_Id id;

   printf("Connected!\n");
   id = esql_query_args(e, strdup("test data"), "SELECT * FROM %s", "jobs");
   if (!id) /**< queue up a simple query */
     {
        fprintf(stderr, "Could not create query!\n");
        ecore_main_loop_quit();
     }
   else if (id % 2)
     esql_query_callback_set(id, (Esql_Query_Cb)callback_);
   return ECORE_CALLBACK_RENEW;
}

static void
connect_cb(Esql *e, void *data __UNUSED__)
{
   Esql_Query_Id id;

   printf("Connected using callback!\n");
   id = esql_query(e, strdup("test data"), "SELECT * FROM `diskconfig` WHERE disk='21' LIMIT 1");
   if (!id) /**< queue up a simple query */
     {
        fprintf(stderr, "Could not create query!\n");
        ecore_main_loop_quit();
     }
   if (id % 2)
     esql_query_callback_set(id, (Esql_Query_Cb)callback_);
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
   esql_connect_callback_set(e, connect_cb, NULL);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!esql_connect(e, "127.0.0.1:" ESQL_DEFAULT_PORT_POSTGRESQL, "zentific", "zentific"), 1); /**< connect to localhost at default port */
   ecore_main_loop_begin();
   esql_disconnect(e); /**< disconnect */
   eina_counter_stop(c, 1);

   eina_counter_start(c);
   esql_connect_callback_set(e, NULL, NULL);
   esql_type_set(e, ESQL_TYPE_MYSQL); /**< now switch to mysql! */
   esql_database_set(e, "zentific"); /**< use database named zentific on connect */
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!esql_connect(e, "127.0.0.1:" ESQL_DEFAULT_PORT_MYSQL, "zentific", "zentific"), 1); /**< connect to localhost at default port */
   ecore_main_loop_begin();
   esql_disconnect(e);
   eina_counter_stop(c, 2);

   eina_counter_start(c);
   esql_connect_callback_set(e, connect_cb, NULL);
   esql_database_set(e, "zentific"); /**< use database named zentific on connect */
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!esql_connect(e, "127.0.0.1:" ESQL_DEFAULT_PORT_MYSQL, "zentific", "zentific"), 1); /**< connect to localhost at default port */
   ecore_main_loop_begin();
   esql_disconnect(e);
   eina_counter_stop(c, 3);

   printf("Times:\n%s\n", eina_counter_dump(c)); /**< this leaks, who cares */
   esql_free(e);
   esql_shutdown();
   return 0;
}
