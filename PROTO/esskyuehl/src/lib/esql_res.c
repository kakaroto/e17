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

#include "esql_private.h"

typedef struct Esql_Row_Iterator
{
   Eina_Iterator   iterator;
   Esql_Res *res;
   Esql_Row *current;
} Esql_Row_Iterator;

static Eina_Bool
esql_row_iterator_next(Esql_Row_Iterator *it,
                       Esql_Row         **r)
{
   Eina_Inlist *l;

   if (!it->current) return EINA_FALSE;

   *r = (Esql_Row *)it->current;

   l = EINA_INLIST_GET((Esql_Row *)it->current);
   it->current = l ? EINA_INLIST_CONTAINER_GET(l->next, Esql_Row) : NULL;

   return EINA_TRUE;
}

static Esql_Res *
esql_row_iterator_container_get(Esql_Row_Iterator *it)
{
   return it->res;
}

static void
eina_row_iterator_free(Esql_Row_Iterator *it)
{
   esql_res_unref(it->res);
   free(it);
}

static void
esql_row_free(Esql_Row *r)
{
   eina_value_flush(&(r->value));
   esql_row_mp_free(r);
}

static void
_esql_res_free(Esql_Res *res)
{
   Esql_Row *r;
   Eina_Inlist *l;

   DBG("res=%p (refcount=%d)", res, res->refcount);

   if (res->rows)
     EINA_INLIST_FOREACH_SAFE(res->rows, l, r)
       esql_row_free(r);

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

   if (res->e->pool)
     {
        Esql_Pool *ep = (Esql_Pool*)res->e;
        res->e = EINA_INLIST_CONTAINER_GET(ep->esqls, Esql);
     }
   res->e->backend.res_free(res);
   free(res->query);
   if (res->mempool) eina_mempool_del(res->mempool);
   esql_res_mp_free(res);
}

void
esql_res_free(void *data __UNUSED__,
              Esql_Res  *res)
{
   esql_res_unref(res);
}

/**
 * @defgroup Esql_Res Results
 * @brief Functions to use result objects
 * @{
 */

/**
 * @brief Retrieve the object from which the query was made
 * @param res The result object (NOT NULL)
 * @return The parent object (NEVER NULL)
 */
Esql *
esql_res_esql_get(const Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);

   if (res->e->pool_member)
     return (Esql*)res->e->pool_struct;
   return res->e;
}

/**
 * @brief Retrieve the error string associated with a result set
 * This function will return NULL in all cases where an error has not occurred.
 * @param res The result object (NOT NULL)
 * @return The error string, NULL if no error
 */
const char *
esql_res_error_get(const Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);

   return res->error;
}

/**
 * @brief Retrieve data pointer previously associated with esql_query or esql_query_args
 * @param res The result object (NOT NULL)
 * @return The data pointer
 */
void *
esql_res_data_get(const Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);

   return res->data;
}

/**
 * @brief Return the #Esql_Query_Id of a result
 * @param res The result object (NOT NULL)
 * @return The query id
 */
Esql_Query_Id
esql_res_query_id_get(const Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, 0);

   return res->qid;
}

/**
 * @brief Return the query string for the result
 * @param res The result object (NOT NULL)
 * @return The query string (NOT stringshared)
 */
const char *
esql_res_query_get(const Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);

   return res->query;
}
/**
 * @brief Retrieve the number of rows selected by a SELECT statement
 * This function has no effect for INSERT/UPDATE/etc statements.
 * @param res The result object (NOT NULL)
 * @return The number of rows, -1 on failure
 */
int
esql_res_rows_count(const Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, -1);

   return res->row_count;
}

/**
 * @brief Retrieve the column name given its index.
 * @param res The result object (NOT NULL)
 * @return column name or NULL on failure. Do not change the name,
 * it's an internal pointer!
 *
 * @see esql_res_rows_count()
 */
const char *
esql_res_col_name_get(const Esql_Res *res, unsigned int column)
{
   const Eina_Value_Struct_Member *member;
   unsigned int column_count;

   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);

   column_count = res->desc ? res->desc->member_count : 0;
   EINA_SAFETY_ON_FALSE_RETURN_VAL(column < column_count, NULL);

   member = res->desc->members + column;
   return member->name;
}

/**
 * @brief Retrieve the number of columns in a result set
 * This function has no effect for INSERT/UPDATE/etc statements.
 * @param res The result object (NOT NULL)
 * @return The number of columns, -1 on failure
 */
int
esql_res_cols_count(const Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, -1);

   if (!res->desc) return 0;
   return res->desc->member_count;
}

/**
 * @brief Retrieve the number of rows affected by a non-SELECT statement
 * This function has no effect for SELECT statements.
 * @param res The result object (NOT NULL)
 * @return The number of rows affected, -1 on failure
 */
long long int
esql_res_rows_affected(const Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, -1);

   return res->affected;
}

/**
 * @brief Retrieve the insert id for a query
 * This function has no effect for statements without an insert id.
 * @param res The result object (NOT NULL)
 * @return The insert id, -1 on failure
 */
long long int
esql_res_id(const Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, -1);

   return res->id;
}

/**
 * @brief Increase result reference count, preventing it from being deleted.
 * @param res The result object (NOT NULL)
 * @return The result @a res with incremented reference count.
 * @see esql_res_unref()
 */
Esql_Res *
esql_res_ref(Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);
   DBG("res=%p (was refcount=%d)", res, res->refcount);
   res->refcount++;
   return res;
}

/**
 * @brief Decrease result reference count, possibly deleting it.
 * @param res The result object (NOT NULL)
 * @see esql_res_ref()
 *
 * Do not trust @a res to be alive after this call, it may be
 * destroyed immediately.
 */
void
esql_res_unref(Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN(res);
   DBG("res=%p (was refcount=%d)", res, res->refcount);
   res->refcount--;
   if (res->refcount == 0)
     _esql_res_free(res);
}

/**
 * @brief Query result reference count.
 * @param res The result object (NOT NULL)
 * @return The result @a res reference count value.
 */
int
esql_res_refcount(const Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, -1);
   return res->refcount;
}

/**
 * @brief Create a new iterator for the rows in a result set
 * This function is used to create an iterator for easily managing the rows in a result set.
 * @note This function has no effect for non-SELECT statements.
 * @param res The result object (NOT NULL)
 * @return The iterator object, NULL on failure
 *
 * The container of the returned iterator is 'const Esql_Res *' with
 * the given @a res.
 *
 * The iterator will hold the given @a res alive by increasing its
 * reference count. It is automatically decreased when the iterator is
 * freed.
 */
Eina_Iterator *
esql_res_row_iterator_new(const Esql_Res *res)
{
   Esql_Row_Iterator *it;

   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);
   if (!res->rows) return NULL;

   it = calloc(1, sizeof(Esql_Row_Iterator));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   EINA_MAGIC_SET(&it->iterator, EINA_MAGIC_ITERATOR);

   it->res = esql_res_ref((Esql_Res *)res);
   it->current = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);

   it->iterator.version = EINA_ITERATOR_VERSION;
   it->iterator.next = FUNC_ITERATOR_NEXT(esql_row_iterator_next);
   it->iterator.get_container = FUNC_ITERATOR_GET_CONTAINER(esql_row_iterator_container_get);
   it->iterator.free = FUNC_ITERATOR_FREE(eina_row_iterator_free);

   return &it->iterator;
}

/**
 * @brief Retrieve the struct of cells in a row
 * @note This function has no effect for non-SELECT statements.
 * @param r The row object (NOT NULL)
 * @return Eina_Value on success or @c NULL on failure.
 *
 * Use eina_value_struct_get(), eina_value_struct_vget() or
 * eina_value_struct_pget() to get the contents of each member.
 *
 * The columns description can be accessed with
 * Eina_Value_Struct::desc, acquired with eina_value_get().
 */
EAPI const Eina_Value *
esql_row_value_struct_get(const Esql_Row *r)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(r, NULL);

   return &(r->value);
}

/**
 * @brief Retrieve the parent object of a row
 * @param r The row object (NOT NULL)
 * @return The parent object (NEVER NULL)
 */
Esql_Res *
esql_row_res_get(const Esql_Row *r)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(r, NULL);

   return r->res;
}

/**
 * @brief Retrieve the column value given its number.
 * @param r The row object (NOT NULL)
 * @param column column index, counting from zero and less than esql_res_cols_count()
 * @param value where to return the value. Its existing contents are ignored.
 * @return EINA_TRUE on success, EINA_FALSE on failure.
 *
 * This is a convenience to use the struct given the column number
 * instead of its name.
 *
 * @see esql_row_value_struct_get()
 */
Eina_Bool
esql_row_value_column_get(const Esql_Row *r, unsigned int column, Eina_Value *val)
{
   unsigned int column_count;
   const Eina_Value_Struct_Member *member;

   EINA_SAFETY_ON_NULL_RETURN_VAL(r, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(val, EINA_FALSE);

   column_count = r->res->desc ? r->res->desc->member_count : 0;
   EINA_SAFETY_ON_FALSE_RETURN_VAL(column < column_count, EINA_FALSE);

   member = r->res->desc->members + column;
   return eina_value_struct_member_value_get(&(r->value), member, val);
}

/** @} */
