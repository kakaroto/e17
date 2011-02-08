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

void
esql_res_free(Esql_Res *res)
{
   Esql_Row *r;
   Eina_Inlist *l, *ll;
   
   if (!res) return;

   for (l = res->rows, ll = l->next; ll; l = ll, ll = ll->next)
     {
        r = EINA_INLIST_CONTAINER_GET(l, Esql_Row);
        free(r);
     }
   free(res);
}

void
esql_row_free(Esql_Row *r)
{
   Esql_Cell *cell;
   Eina_Inlist *l, *ll;

   
   if (!r) return;

   for (l = r->cells, ll = l->next; ll; l = ll, ll = ll->next)
     {
        cell = EINA_INLIST_CONTAINER_GET(l, Esql_Cell);
        free(cell);
     }
   free(r);
}

static Eina_Bool
esql_row_iterator_next(Esql_Row_Iterator *it, Esql_Row **r)
{
   Eina_Inlist *l;
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, EINA_FALSE);
   if (!it->current) return EINA_FALSE;

   *r = (Esql_Row*)it->current;

   l = EINA_INLIST_GET((Esql_Row*)it->current);
   it->current = l ? EINA_INLIST_CONTAINER_GET(l->next, Esql_Row) : NULL;

   return EINA_TRUE;
}

static Esql_Row *
esql_row_iterator_container_get(Esql_Row_Iterator *it)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(it, NULL);

   return (Esql_Row*)it->head;
}

static void
eina_row_iterator_free(Esql_Row_Iterator *it)
{
   EINA_SAFETY_ON_NULL_RETURN(it);

   free(it);
}

/**
 * @defgroup Esql_Res Results
 * @brief Functions to use result objects
 * @{
 */

Esql *
esql_res_esql_get(Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);

   return res->e;
}

const char *
esql_res_error_get(Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);

   return res->error;
}

int
esql_res_rows_count(Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, -1);

   return res->row_count;
}

int
esql_res_cols_count(Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, -1);

   return res->num_cols;
}

long long int
esql_res_rows_affected(Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, -1);

   return res->affected;
}

long long int
esql_res_id(Esql_Res *res)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, -1);

   return res->id;
}

Eina_Iterator *
esql_res_row_iterator_new(Esql_Res *res)
{
   Esql_Row_Iterator *it;

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

Eina_Inlist *
esql_row_cells_get(Esql_Row *r)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(r, NULL);

   return r->cells;
}

int
esql_row_cell_count(Esql_Row *r)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(r, -1);

   return r->num_cells;
}

Esql_Res *
esql_row_res_get(Esql_Row *r)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(r, NULL);

   return r->res;
}
/** @} */
