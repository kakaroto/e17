/*
 * Copyright 2012 Mike Blumenkrantz <michael.blumenkrantz@gmail.com>
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

#include "esql_private.h"
#include <sqlite3.h>

static const char *esql_sqlite_error_get(Esql *e);
static void esql_sqlite_disconnect(Esql *e);
static int esql_sqlite_fd_get(Esql *e);
static int esql_sqlite_connect(Esql *e);
static int esql_sqlite_io(Esql *e);
static void esql_sqlite_setup(Esql *e, const char *addr, const char *user, const char *passwd);
static void esql_sqlite_query(Esql *e, const char *query, unsigned int len);
static void esql_sqlite_res_free(Esql_Res *res);
static void esql_sqlite_res(Esql_Res *res);
static char *esql_sqlite_escape(Esql *e, unsigned int *len, const char *fmt, va_list args);
static void esql_sqlite_row_add(Esql_Res *res);
static void esql_sqlite_free(Esql *e);


static const char *
esql_sqlite_error_get(Esql *e)
{
   if (sqlite3_errcode(e->backend.db))
     return sqlite3_errmsg(e->backend.db);
   return NULL;
}

static void
esql_sqlite_disconnect(Esql *e)
{
   if (!e->backend.db) return;
   while (sqlite3_close(e->backend.db));
   e->backend.db = NULL;
   free(e->backend.conn_str);
   e->backend.conn_str = NULL;
   e->backend.conn_str_len = 0;
}

static int
esql_sqlite_fd_get(Esql *e __UNUSED__)
{
   /* dummy return */
   return -1;
}

static void
esql_sqlite_thread_end_cb(Esql *e, Ecore_Thread *et __UNUSED__)
{
   DBG("(e=%p)", e);
   e->backend.thread = NULL;
   if (e->backend.stmt) sqlite3_finalize(e->backend.stmt);
   e->backend.stmt = NULL;
   esql_call_complete(e);
}

static void
esql_sqlite_thread_cancel_cb(Esql *e, Ecore_Thread *et __UNUSED__)
{
   DBG("(e=%p)", e);
   e->backend.thread = NULL;
   esql_event_error(e);
}

static void
esql_sqlite_connect_cb(Esql *e, Ecore_Thread *et)
{
   if (sqlite3_open_v2(e->backend.conn_str, (struct sqlite3 **)&e->backend.db, SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE | SQLITE_OPEN_NOMUTEX, NULL))
     {
        ERR("%s", esql_sqlite_error_get(e));
        ecore_thread_cancel(et);
        e->backend.thread = NULL;
     }
   else
     e->current = ESQL_CONNECT_TYPE_INIT;
}

static int
esql_sqlite_connect(Esql *e)
{
   e->backend.thread = ecore_thread_run((Ecore_Thread_Cb)esql_sqlite_connect_cb,
                                      (Ecore_Thread_Cb)esql_sqlite_thread_end_cb,
                                      (Ecore_Thread_Cb)esql_sqlite_thread_cancel_cb, e);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e->backend.thread, ECORE_FD_ERROR);
   return ECORE_FD_READ | ECORE_FD_WRITE;
}

/* TODO: move esql_sqlite_desc_ops to common? */
static void *
esql_sqlite_desc_alloc(const Eina_Value_Struct_Operations *ops __UNUSED__, const Eina_Value_Struct_Desc *desc)
{
   /* TODO: mempool? */
   return malloc(desc->size);
}

static void
esql_sqlite_desc_free(const Eina_Value_Struct_Operations *ops __UNUSED__, const Eina_Value_Struct_Desc *desc __UNUSED__, void *memory)
{
   /* TODO: mempool? */
   free(memory);
}

static const Eina_Value_Struct_Member *
esql_sqlite_desc_find_member(const Eina_Value_Struct_Operations *ops __UNUSED__, const Eina_Value_Struct_Desc *desc, const char *name)
{
   const Eina_Value_Struct_Member *itr, *itr_end;

   itr = desc->members;
   itr_end = itr + desc->member_count;

   /* assumes name is stringshared.
    *
    * we do this because it's the recommended usage pattern, moreover
    * we expect to find the member, as users shouldn't look for
    * non-existent members!
    */
   for (; itr < itr_end; itr++)
     if (itr->name == name)
       return itr;

   itr = desc->members;
   name = eina_stringshare_add(name);
   eina_stringshare_del(name); /* we'll not use the contents, this is fine */
   for (; itr < itr_end; itr++)
     if (itr->name == name)
       return itr;

   return NULL;
}

static Eina_Value_Struct_Operations esql_sqlite_desc_ops = {
  EINA_VALUE_STRUCT_OPERATIONS_VERSION,
  esql_sqlite_desc_alloc,
  esql_sqlite_desc_free,
  NULL, /* no copy */
  NULL, /* no compare */
  esql_sqlite_desc_find_member
};

static Eina_Value_Struct_Desc *
esql_sqlite_desc_get(const Esql *e)
{
   Eina_Value_Struct_Desc *desc;
   sqlite3_stmt *stmt = e->backend.stmt;
   int i, cols;
   unsigned int offset;

   cols = sqlite3_column_count(stmt);
   if (cols < 1) return NULL;

   desc = malloc(sizeof(*desc) + cols * sizeof(Eina_Value_Struct_Member));
   EINA_SAFETY_ON_NULL_RETURN_VAL(desc, NULL);

   desc->version = EINA_VALUE_STRUCT_DESC_VERSION;
   desc->ops = &esql_sqlite_desc_ops;
   desc->members = (void *)((char *)desc + sizeof(*desc));
   desc->member_count = cols;
   desc->size = 0;

   offset = 0;
   for (i = 0; i < cols; i++)
     {
        Eina_Value_Struct_Member *m = (Eina_Value_Struct_Member *)desc->members + i;
        unsigned int size;

        m->name = eina_stringshare_add(sqlite3_column_name(stmt, i));
        m->offset = offset;

        switch (sqlite3_column_type(stmt, i))
          {
           case SQLITE_TEXT:
              m->type = EINA_VALUE_TYPE_STRING;
              break;
           case SQLITE_INTEGER:
              m->type = EINA_VALUE_TYPE_INT64;
              break;
           case SQLITE_FLOAT:
              m->type = EINA_VALUE_TYPE_DOUBLE;
              break;
           default:
              m->type = EINA_VALUE_TYPE_BLOB;
          }

        size = m->type->value_size;
        if (size % sizeof(void *) != 0)
          size += size - (size % sizeof(void *));

        offset += size;
     }

   desc->size = offset;
   return desc;
}

static void
esql_sqlite_query_cb(Esql *e, Ecore_Thread *et)
{
   int ret, tries = 0;

   while (++tries < 1000)
     {
        if (!e->backend.stmt)
          return;
        ret = sqlite3_step(e->backend.stmt);
        switch (ret)
          {
           case SQLITE_BUSY:
             break;
           case SQLITE_DONE:
             if (!e->res)
               {
                  e->res = esql_res_calloc(1);
                  if (!e->res) goto out;
                  e->res->e = e;
                  e->res->desc = esql_sqlite_desc_get(e);
                  e->res->affected = sqlite3_changes(e->backend.db);
                  INFO("res %p desc=%p", e->res, e->res->desc);
               }
             sqlite3_finalize(e->backend.stmt);
             e->backend.stmt = NULL;
             return;
           case SQLITE_ROW:
             if (!e->res)
               {
                  e->res = esql_res_calloc(1);
                  if (!e->res) goto out;
                  e->res->e = e;
                  e->res->desc = esql_sqlite_desc_get(e);
                  e->res->affected = sqlite3_changes(e->backend.db);
                  INFO("res %p desc=%p", e->res, e->res->desc);
               }
             esql_sqlite_row_add(e->res);
           default:
             goto out;
          }
     }
   /* something crazy is going on */
out:
   sqlite3_finalize(e->backend.stmt);
   e->backend.stmt = NULL;
   ecore_thread_cancel(et);
}

static int
esql_sqlite_io(Esql *e)
{
   DBG("(e=%p)", e);
   if (e->backend.thread) return ECORE_FD_ERROR;
   e->backend.thread = ecore_thread_run((Ecore_Thread_Cb)esql_sqlite_query_cb,
                                      (Ecore_Thread_Cb)esql_sqlite_thread_end_cb,
                                      (Ecore_Thread_Cb)esql_sqlite_thread_cancel_cb, e);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e->backend.thread, ECORE_FD_ERROR);
   return ECORE_FD_READ | ECORE_FD_WRITE;
}

static void
esql_sqlite_setup(Esql *e, const char *addr, const char *user __UNUSED__, const char *passwd __UNUSED__)
{
   e->backend.conn_str = strdup(addr);
}

static void
esql_sqlite_query(Esql *e, const char *query, unsigned int len)
{
   EINA_SAFETY_ON_TRUE_RETURN(sqlite3_prepare_v2(e->backend.db, query, len, (struct sqlite3_stmt**)&e->backend.stmt, NULL));
}

static void
esql_sqlite_res_free(Esql_Res *res)
{
   if (res->desc)
     {
        /* memset is not needed, but leave it here to find if people
         * kept reference to values after row is removed, see below.
         */
        memset(res->desc, 0, sizeof(res->desc));
        free(res->desc);

        /* NOTE: after this point, if users are still holding 'desc' they will
         * have problems. This can be done if user calls eina_value_copy()
         * on some esql_row_value_struct_get()
         *
         * If this is an use case, add Eina_Value_Struct_Desc to some other
         * struct and do reference counting on it, increment on
         * alloc/copy, decrement on free.
         *
         * Remember that struct is created/ref on thread, and it is free'd
         * on main thread, then needs locking!
         */
     }
}

static void
esql_sqlite_res(Esql_Res *res __UNUSED__)
{
}

static char *
esql_sqlite_escape(Esql *e __UNUSED__, unsigned int *len, const char *fmt, va_list args)
{
   return esql_query_escape(EINA_TRUE, len, fmt, args);
}

static void
esql_sqlite_row_add(Esql_Res *res)
{
   Esql_Row *r;
   Eina_Value *val;
   unsigned int i;

   r = esql_row_calloc(1);
   EINA_SAFETY_ON_NULL_RETURN(r);
   r->res = res;
   res->row_count++;
   res->rows = eina_inlist_append(res->rows, EINA_INLIST_GET(r));

   val = &(r->value);
   eina_value_struct_setup(val, res->desc);

   for (i = 0; i < res->desc->member_count; i++)
     {
        const Eina_Value_Struct_Member *m = res->desc->members + i;
        Eina_Value inv;

        INFO("col %u %s\n", i, m->name);
        switch (sqlite3_column_type(res->e->backend.stmt, i))
          {
           case SQLITE_TEXT:
              eina_value_setup(&inv, EINA_VALUE_TYPE_STRING);
              eina_value_set(&inv, sqlite3_column_text(res->e->backend.stmt, i));
             break;

           case SQLITE_INTEGER:
              eina_value_setup(&inv, EINA_VALUE_TYPE_INT64);
              eina_value_set(&inv, sqlite3_column_int64(res->e->backend.stmt, i));
             break;

           case SQLITE_FLOAT:
              eina_value_setup(&inv, EINA_VALUE_TYPE_DOUBLE);
              eina_value_set(&inv, sqlite3_column_double(res->e->backend.stmt, i));
             break;

           default:
             {
                Eina_Value_Blob blob;

                blob.ops = NULL;
                blob.memory = sqlite3_column_blob(res->e->backend.stmt, i);
                blob.size = sqlite3_column_bytes(res->e->backend.stmt, i);

                eina_value_setup(&inv, EINA_VALUE_TYPE_BLOB);
                eina_value_set(&inv, &blob);
             }
          }

        eina_value_struct_member_value_set(val, m, &inv);
        eina_value_flush(&inv);
     }
}

static void
esql_sqlite_free(Esql *e)
{
   if (!e->backend.db) return;
   while (sqlite3_close(e->backend.db));
   e->backend.db = NULL;
   e->backend.free = NULL;
   free(e->backend.conn_str);
   e->backend.conn_str = NULL;
   e->backend.conn_str_len = 0;
}

static void
esql_sqlite_init(Esql *e)
{
   INFO("Esql type for %p set to SQLite", e);
   e->type = ESQL_TYPE_SQLITE;
   e->backend.connect = esql_sqlite_connect;
   e->backend.disconnect = esql_sqlite_disconnect;
   e->backend.error_get = esql_sqlite_error_get;
   e->backend.setup = esql_sqlite_setup;
   e->backend.io = esql_sqlite_io;
   e->backend.fd_get = esql_sqlite_fd_get;
   e->backend.escape = esql_sqlite_escape;
   e->backend.query = esql_sqlite_query;
   e->backend.res = esql_sqlite_res;
   e->backend.res_free = esql_sqlite_res_free;
   e->backend.free = esql_sqlite_free;
}

EAPI Esql_Type
esql_module_init(Esql *e)
{
   if (e) esql_sqlite_init(e);
   sqlite3_initialize();
   return ESQL_TYPE_SQLITE;
}
