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

static Eina_Bool
esql_row_iterator_next(Esql_Row_Iterator *it,
                       Esql_Row         **r)
{
   Eina_Inlist *l;
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, EINA_FALSE);
   if (!it->current) return EINA_FALSE;

   *r = (Esql_Row *)it->current;

   l = EINA_INLIST_GET((Esql_Row *)it->current);
   it->current = l ? EINA_INLIST_CONTAINER_GET(l->next, Esql_Row) : NULL;

   return EINA_TRUE;
}

static Esql_Row *
esql_row_iterator_container_get(Esql_Row_Iterator *it)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   return (Esql_Row *)it->head;
}

static void
eina_row_iterator_free(Esql_Row_Iterator *it)
{
   EINA_SAFETY_ON_NULL_RETURN(it);

   free(it);
}

void
esql_res_free(void *data __UNUSED__,
              Esql_Res  *res)
{
   Esql_Row *r;
   Eina_Inlist *l;

   if (!res) return;

   if (res->rows)
     EINA_INLIST_FOREACH_SAFE(res->rows, l, r)
       esql_row_free(r);
   res->e->backend.res_free(res);
   free(res->query);
   free(res);
}

void
esql_row_free(Esql_Row *r)
{
   Esql_Cell *cell;
   Eina_Inlist *l;

   if ((!r) || (!r->cells)) return;

   EINA_INLIST_FOREACH_SAFE(r->cells, l, cell)
     free(cell);

   free(r);
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
esql_res_esql_get(Esql_Res *res)
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
esql_res_error_get(Esql_Res *res)
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
esql_res_data_get(Esql_Res *res)
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
esql_res_query_id_get(Esql_Res *res)
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
esql_res_query_get(Esql_Res *res)
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
esql_res_rows_count(Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, -1);

   return res->row_count;
}

/**
 * @brief Retrieve the number of columns in a result set
 * This function has no effect for INSERT/UPDATE/etc statements.
 * @param res The result object (NOT NULL)
 * @return The number of columns, -1 on failure
 */
int
esql_res_cols_count(Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, -1);

   return res->num_cols;
}

/**
 * @brief Retrieve the number of rows affected by a non-SELECT statement
 * This function has no effect for SELECT statements.
 * @param res The result object (NOT NULL)
 * @return The number of rows affected, -1 on failure
 */
long long int
esql_res_rows_affected(Esql_Res *res)
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
esql_res_id(Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, -1);

   return res->id;
}

/**
 * @brief Create a new iterator for the rows in a result set
 * This function is used to create an iterator for easily managing the rows in a result set.
 * @note This function has no effect for non-SELECT statements.
 * @param res The result object (NOT NULL)
 * @return The iterator object, NULL on failure
 */
Eina_Iterator *
esql_res_row_iterator_new(Esql_Res *res)
{
   Esql_Row_Iterator *it;

   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(res->rows, NULL);

   it = calloc(1, sizeof(Esql_Row_Iterator));
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   EINA_MAGIC_SET(&it->iterator, EINA_MAGIC_ITERATOR);

   it->head = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);
   it->current = it->head;

   it->iterator.version = EINA_ITERATOR_VERSION;
   it->iterator.next = FUNC_ITERATOR_NEXT(esql_row_iterator_next);
   it->iterator.get_container = FUNC_ITERATOR_GET_CONTAINER(esql_row_iterator_container_get);
   it->iterator.free = FUNC_ITERATOR_FREE(eina_row_iterator_free);

   return &it->iterator;
}

/**
 * @brief Retrieve the list of cells in a row
 * This returns the inlist for the #Esql_Cell objects in row @p r.
 * @note This function has no effect for non-SELECT statements.
 * @param r The row object (NOT NULL)
 * @return The inlist, or NULL on failure
 */
Eina_Inlist *
esql_row_cells_get(Esql_Row *r)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(r, NULL);

   return r->cells;
}

/**
 * @brief Retrieve the number of cells in a row
 * This is equivalent to esql_res_cols_count.
 * @note This function has no effect for INSERT/UPDATE/etc statements.
 * @param r The row object (NOT NULL)
 * @return The number of cells, -1 on failure
 */
int
esql_row_cell_count(Esql_Row *r)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(r, -1);

   return r->num_cells;
}

/**
 * @brief Retrieve the parent object of a row
 * @param r The row object (NOT NULL)
 * @return The parent object (NEVER NULL)
 */
Esql_Res *
esql_row_res_get(Esql_Row *r)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(r, NULL);

   return r->res;
}

/** @} */
