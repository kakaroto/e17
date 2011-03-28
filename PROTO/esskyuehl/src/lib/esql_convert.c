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

/**
 * @defgroup Esql_Convert Conversion
 * @brief Utility functions for converting single cell results
 * @{*/

/**
 * @brief Convert result to a string
 * @param res Result
 * @param str Pointer to stringshare string to
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
esql_res_to_string(Esql_Res *res, const char **str)
{
   Esql_Row *row;
   Esql_Cell *cell;
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(str, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(res->row_count != 1, EINA_FALSE);
   row = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);
   cell = EINA_INLIST_CONTAINER_GET(row->cells, Esql_Cell);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(cell->type != ESQL_CELL_TYPE_STRING, EINA_FALSE);
   *str = eina_stringshare_add(cell->value.string);
   return EINA_TRUE;
}

/**
 * @brief Convert result to a long long int
 * @param res Result
 * @param i Pointer to store int at
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
esql_res_to_lli(Esql_Res *res, long long int *i)
{
   Esql_Row *row;
   Esql_Cell *cell;
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(i, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(res->row_count != 1, EINA_FALSE);
   row = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);
   cell = EINA_INLIST_CONTAINER_GET(row->cells, Esql_Cell);
   switch (cell->type)
     {
      case ESQL_CELL_TYPE_TINYINT:
        *i = cell->value.c;
        break;
      case ESQL_CELL_TYPE_SHORT:
        *i = cell->value.s;
        break;
      case ESQL_CELL_TYPE_LONG:
        *i = cell->value.i;
        break;
      case ESQL_CELL_TYPE_LONGLONG:
        *i = cell->value.l;
        break;
      default:
        ERR("cell from res %p has invalid type!", res);
	       return EINA_FALSE;
     }
   return EINA_TRUE;
}

/**
 * @brief Convert result to a double
 * @param res Result
 * @param d Pointer to store double at
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
esql_res_to_double(Esql_Res *res, double *d)
{
   Esql_Row *row;
   Esql_Cell *cell;
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(d, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(res->row_count != 1, EINA_FALSE);
   row = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);
   cell = EINA_INLIST_CONTAINER_GET(row->cells, Esql_Cell);
   switch (cell->type)
     {
      case ESQL_CELL_TYPE_FLOAT:
        *d = cell->value.f;
        break;
      case ESQL_CELL_TYPE_DOUBLE:
        *d = cell->value.d;
        break;
      default:
        ERR("cell from res %p has invalid type!", res);
	       return EINA_FALSE;
     }
   return EINA_TRUE;
}

/**
 * @brief Convert result to a tm struct
 * @param res Result
 * @param t Pointer to copy values to
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
esql_res_to_tm(Esql_Res *res, struct tm *t)
{
   Esql_Row *row;
   Esql_Cell *cell;
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(t, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(res->row_count != 1, EINA_FALSE);
   row = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);
   cell = EINA_INLIST_CONTAINER_GET(row->cells, Esql_Cell);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(cell->type != ESQL_CELL_TYPE_TIMESTAMP, EINA_FALSE);
   memcpy(t, &cell->value.tm, sizeof(struct tm));
   return EINA_TRUE;
}

/**
 * @brief Convert result to a timeval struct
 * @param res Result
 * @param tv Pointer to copy values to
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
esql_res_to_timeval(Esql_Res *res, struct timeval *tv)
{
   Esql_Row *row;
   Esql_Cell *cell;
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(tv, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(res->row_count != 1, EINA_FALSE);
   row = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);
   cell = EINA_INLIST_CONTAINER_GET(row->cells, Esql_Cell);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(cell->type != ESQL_CELL_TYPE_TIMESTAMP, EINA_FALSE);
   memcpy(tv, &cell->value.tv, sizeof(struct timeval));
   return EINA_TRUE;
}

/*********************************************************************/

/**
 * @brief Convert cell to a long long int
 * @param cell Cell
 * @param i Pointer to store int at
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
esql_cell_to_lli(Esql_Cell *cell, long long int *i)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(i, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(cell, EINA_FALSE);
   switch (cell->type)
     {
      case ESQL_CELL_TYPE_TINYINT:
        *i = cell->value.c;
        break;
      case ESQL_CELL_TYPE_SHORT:
        *i = cell->value.s;
        break;
      case ESQL_CELL_TYPE_LONG:
        *i = cell->value.i;
        break;
      case ESQL_CELL_TYPE_LONGLONG:
        *i = cell->value.l;
        break;
      default:
        ERR("cell %p has invalid type!", cell);
	       return EINA_FALSE;
     }
   return EINA_TRUE;
}

/**
 * @brief Convert cell to a double
 * @param cell Cell
 * @param d Pointer to store double at
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
esql_cell_to_double(Esql_Cell *cell, double *d)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(d, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(cell, EINA_FALSE);
   switch (cell->type)
     {
      case ESQL_CELL_TYPE_FLOAT:
        *d = cell->value.f;
        break;
      case ESQL_CELL_TYPE_DOUBLE:
        *d = cell->value.d;
        break;
      default:
        ERR("cell %p has invalid type!", cell);
	       return EINA_FALSE;
     }
   return EINA_TRUE;
}
/** @} */
