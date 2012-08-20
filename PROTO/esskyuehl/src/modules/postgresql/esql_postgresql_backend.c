/*
 * Copyright 2011, 2012 Mike Blumenkrantz <michael.blumenkrantz@gmail.com>
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

#include "esql_module.h"
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#include <c.h>
#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <libpq-fe.h>
#include <catalog/pg_type.h>
#include <inttypes.h>

static const char *esql_postgresql_error_get(Esql *e);
static void esql_postgresql_disconnect(Esql *e);
static int esql_postgresql_fd_get(Esql *e);
static int esql_postgresql_connect(Esql *e);
static int esql_postgresql_io(Esql *e);
static void esql_postgresql_setup(Esql *e, const char *addr, const char *user, const char *passwd);
static void esql_postgresql_query(Esql *e, const char *query, unsigned int len);
static void esql_postgresql_res_free(Esql_Res *res);
static void esql_postgresql_res(Esql_Res *res);
static char *esql_postgresql_escape(Esql *e, unsigned int *len, const char *fmt, va_list args);
static void esql_postgresql_row_init(Esql_Row *r, int row_num);
static void esql_postgresql_free(Esql *e);

static void
esql_module_setup_cb(PGresult *pres, int col, Eina_Value_Struct_Member *m)
{
   m->name = eina_stringshare_add(PQfname(pres, col));

   switch (PQftype(pres, col))
     {
      case TIMESTAMPOID:
      case ABSTIMEOID:
        m->type = EINA_VALUE_TYPE_TIMESTAMP;
        break;

      case BYTEAOID:
      case NAMEOID:
      case TEXTOID:
      case VARCHAROID:
      case BPCHAROID:
        m->type = EINA_VALUE_TYPE_STRING;
        break;

      case BOOLOID:
      case CHAROID:
        m->type = EINA_VALUE_TYPE_CHAR;
        break;

      case INT2OID:
        m->type = EINA_VALUE_TYPE_SHORT;
        break;

      case INT4OID:
        m->type = EINA_VALUE_TYPE_LONG;
        break;

      case INT8OID:
        m->type = EINA_VALUE_TYPE_INT64;
        break;

      case FLOAT4OID:
        m->type = EINA_VALUE_TYPE_FLOAT;
        break;

      case FLOAT8OID:
      case TINTERVALOID:
      case RELTIMEOID:
        m->type = EINA_VALUE_TYPE_DOUBLE;
        break;

      default:
        m->type = EINA_VALUE_TYPE_BLOB;
     }
}

static const char *
esql_postgresql_error_get(Esql *e)
{
   const char *p;

   p = PQerrorMessage(e->backend.db);
   if ((!p) || (!*p))
     p = PQresStatus(PQresultStatus(e->res->backend.res));

   return p;
}

static void
esql_postgresql_disconnect(Esql *e)
{
   if (!e->backend.db) return;
   PQfinish(e->backend.db);
   e->backend.db = NULL;
}

static int
esql_postgresql_fd_get(Esql *e)
{
   return PQsocket(e->backend.db);
}

static int
esql_postgresql_connect(Esql *e)
{
   e->backend.db = PQconnectStart(e->backend.conn_str);
   free(e->backend.conn_str);
   e->backend.conn_str = NULL;
   e->backend.conn_str_len = 0;
   EINA_SAFETY_ON_NULL_RETURN_VAL(e->backend.db, ECORE_FD_ERROR);
   if (PQstatus(e->backend.db) == CONNECTION_BAD)
     {
        ERR("%s", esql_postgresql_error_get(e));
        return ECORE_FD_ERROR;
     }
   return ECORE_FD_READ | ECORE_FD_WRITE;
}

static int
esql_postgresql_io(Esql *e)
{
   if (PQstatus(e->backend.db) == CONNECTION_BAD)
     {
        ERR("%s", esql_postgresql_error_get(e));
        return ECORE_FD_ERROR;
     }
   if (e->current == ESQL_CONNECT_TYPE_INIT)
     {
        switch (PQconnectPoll(e->backend.db))
          {
           case PGRES_POLLING_OK:
             return 0;
           case PGRES_POLLING_READING:
             return ECORE_FD_READ;
           case PGRES_POLLING_WRITING:
             return ECORE_FD_WRITE;
           default:
             ERR("%s", esql_postgresql_error_get(e));
             return ECORE_FD_ERROR;
          }
     }
   if (!PQconsumeInput(e->backend.db))
     {
        ERR("%s", esql_postgresql_error_get(e));
        return ECORE_FD_ERROR;
     }
   if (!PQisBusy(e->backend.db)) return 0;
   return ECORE_FD_READ | ECORE_FD_WRITE; /* psql does not provide a method to get read/write mode :( */
}

static void
esql_postgresql_setup(Esql *e, const char *addr, const char *user, const char *passwd)
{
   const char *port;
   char buf[4096];

   const char *db;

   if (e->backend.conn_str && (!addr) && (!user) && (!passwd)) return; /* reconnect attempt */
   db = e->database ? e->database : user;

   port = strchr(addr, ':');
   if (port)
     e->backend.conn_str_len = snprintf(buf, sizeof(buf), "host=%s port=%s dbname=%s user=%s password=%s connect_timeout=5",
                                                          strndupa(addr, port - addr), port + 1, db, user, passwd);
   else
     e->backend.conn_str_len = snprintf(buf, sizeof(buf), "host=%s dbname=%s user=%s password=%s connect_timeout=5",
                                                          addr, db, user, passwd);
   e->backend.conn_str = strdup(buf);
}

static void
esql_postgresql_query(Esql *e, const char *query, unsigned int len __UNUSED__)
{
   EINA_SAFETY_ON_FALSE_RETURN(PQsendQuery(e->backend.db, query));
}

static void
esql_postgresql_res_free(Esql_Res *res)
{
   PQclear(res->backend.res);
}

static void
esql_postgresql_res(Esql_Res *res)
{
   Esql_Row *r;
   PGresult *pres;
   int i;

   pres = res->backend.res = PQgetResult(res->e->backend.db);
   EINA_SAFETY_ON_NULL_RETURN(pres);

   switch (PQresultStatus(pres))
     {
      case PGRES_COMMAND_OK:
        {
           const char *a;

           a = PQcmdTuples(pres);
           if (a && (*a))
             res->affected = strtol(a, NULL, 10);
           res->id = PQoidValue(pres);
        }
        return;
      case PGRES_TUPLES_OK:
        break;
      default:
        res->error = PQresultErrorMessage(pres);
        ERR("Error %s:'%s'!", PQresStatus(PQresultStatus(pres)), res->error);
        return;
     }
   res->desc = esql_module_desc_get(PQntuples(pres), (Esql_Module_Setup_Cb)esql_module_setup_cb, res);
   for (i = 0; i < res->row_count; i++)
     {
        r = esql_row_calloc(1);
        EINA_SAFETY_ON_NULL_RETURN(r);
        r->res = res;
        esql_postgresql_row_init(r, i);
        res->rows = eina_inlist_append(res->rows, EINA_INLIST_GET(r));
     }
}

static char *
esql_postgresql_escape(Esql *e __UNUSED__, unsigned int *len, const char *fmt, va_list args)
{
   char *ret;

   ret = esql_query_escape(EINA_TRUE, len, fmt, args);
   return ret;
}

static void
esql_postgresql_row_init(Esql_Row *r, int row_num)
{
   PGresult *pres;
   Esql_Res *res;
   Eina_Value *sval;
   unsigned int i, cols;
   res = r->res;
   pres = res->backend.res;
   cols = res->desc->member_count;

   sval = &(r->value);
   eina_value_struct_setup(sval, res->desc);

   for (i = 0; i < cols; i++)
     {
        const Eina_Value_Struct_Member *m = res->desc->members + i;
        Eina_Value val;
        const char *str;
        struct tm tm;

        if (PQgetisnull(pres, row_num, i))
          goto out;

        str = PQgetvalue(pres, row_num, i);
        switch (PQftype(pres, i))
          {
           case TIMESTAMPOID:
             strptime(str, "%Y-%m-%d %H:%M:%S", &tm);
             eina_value_setup(&val, EINA_VALUE_TYPE_ULONG);
             eina_value_set(&val, (long)mktime(&tm));
             break;
           case ABSTIMEOID:
             {
                time_t t;

                t = strtoumax(str, NULL, 10);
                localtime_r(&t, &tm);
                eina_value_setup(&val, EINA_VALUE_TYPE_ULONG);
                eina_value_set(&val, (long)mktime(&tm));
                break;
             }

           case BYTEAOID:
           case NAMEOID:
           case TEXTOID:
           case VARCHAROID:
           case BPCHAROID:
             eina_value_setup(&val, EINA_VALUE_TYPE_STRING);
             eina_value_set(&val, str);
             //cell->len = PQgetlength(pres, row_num, i);
             break;

           case BOOLOID:
           case CHAROID:
             eina_value_setup(&val, EINA_VALUE_TYPE_CHAR);
             eina_value_set(&val, (char)strtol(str, NULL, 10));
             break;

           case INT2OID:
             eina_value_setup(&val, EINA_VALUE_TYPE_SHORT);
             eina_value_set(&val, (short)strtol(str, NULL, 10));
             break;

           case INT4OID:
             eina_value_setup(&val, EINA_VALUE_TYPE_LONG);
             eina_value_set(&val, strtol(str, NULL, 10));
             break;

           case INT8OID:
             eina_value_setup(&val, EINA_VALUE_TYPE_INT64);
             eina_value_set(&val, strtoll(str, NULL, 10));
             break;

           case FLOAT4OID:
             eina_value_setup(&val, EINA_VALUE_TYPE_FLOAT);
             eina_value_set(&val, strtof(str, NULL));
             break;

           case FLOAT8OID:
           case TINTERVALOID:
           case RELTIMEOID:
             eina_value_setup(&val, EINA_VALUE_TYPE_DOUBLE);
             eina_value_set(&val, strtod(str, NULL));
             break;

           default:
             {
                Eina_Value_Blob blob;

                blob.ops = NULL;
                blob.memory = str;
                blob.size = PQgetlength(pres, row_num, i);
                eina_value_setup(&val, EINA_VALUE_TYPE_BLOB);
                eina_value_set(&val, &blob);
                //WARN("Unsupported type passed with Oid %u in column '%s': '%*s'!", PQftype(pres, i), cell->colname, blob.size, (char*)str);
                break;
             }
          }
out:
        eina_value_struct_member_value_set(sval, m, &val);
        eina_value_flush(&val);
     }
}

static void
esql_postgresql_free(Esql *e)
{
   if (!e->backend.db) return;
   PQfinish(e->backend.db);
   e->backend.db = NULL;
   e->backend.free = NULL;
}

static void
esql_postgresql_init(Esql *e)
{
   INFO("Esql type for %p set to PostgreSQL", e);
   e->type = ESQL_TYPE_POSTGRESQL;
   e->backend.connect = esql_postgresql_connect;
   e->backend.disconnect = esql_postgresql_disconnect;
   e->backend.error_get = esql_postgresql_error_get;
   e->backend.setup = esql_postgresql_setup;
   e->backend.io = esql_postgresql_io;
   e->backend.fd_get = esql_postgresql_fd_get;
   e->backend.escape = esql_postgresql_escape;
   e->backend.query = esql_postgresql_query;
   e->backend.res = esql_postgresql_res;
   e->backend.res_free = esql_postgresql_res_free;
   e->backend.free = esql_postgresql_free;
}

EAPI Esql_Type
esql_module_init(Esql *e)
{
   if (e) esql_postgresql_init(e);
   return ESQL_TYPE_POSTGRESQL;
}
