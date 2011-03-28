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
 * @return Stringshared result
 */
const char *
esql_res_to_string(Esql_Res *res)
{
   Esql_Row *row;
   Esql_Cell *cell;
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(res->row_count != 1, NULL);
   row = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);
   cell = EINA_INLIST_CONTAINER_GET(row->cells, Esql_Cell);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(
     (cell->type != ESQL_CELL_TYPE_STRING) &&
     (cell->type != ESQL_CELL_TYPE_UNKNOWN), NULL);
   return eina_stringshare_add(cell->value.string);
}

/**
 * @brief Convert result to a binary blob
 * @param res Result
 * @return Allocated binary blob (must be freed)
 */
unsigned char *
esql_res_to_blob(Esql_Res *res)
{
   Esql_Row *row;
   Esql_Cell *cell;
   unsigned char *ret;
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(res->row_count != 1, NULL);
   row = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);
   cell = EINA_INLIST_CONTAINER_GET(row->cells, Esql_Cell);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(
     (cell->type != ESQL_CELL_TYPE_BLOB) &&
     (cell->type != ESQL_CELL_TYPE_UNKNOWN), NULL);
   ret = malloc(cell->len);
   memcpy(ret, cell->value.blob, cell->len);
   return ret;
}

/**
 * @brief Convert result to a long long int
 * @param res Result
 * @return The result
 */
long long int
esql_res_to_lli(Esql_Res *res)
{
   Esql_Row *row;
   Esql_Cell *cell;
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(res->row_count != 1, EINA_FALSE);
   row = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);
   cell = EINA_INLIST_CONTAINER_GET(row->cells, Esql_Cell);
   switch (cell->type)
     {
      case ESQL_CELL_TYPE_TINYINT:
        return cell->value.c;
      case ESQL_CELL_TYPE_SHORT:
        return cell->value.s;
      case ESQL_CELL_TYPE_LONG:
        return cell->value.i;
      case ESQL_CELL_TYPE_LONGLONG:
        return cell->value.l;
      default:
        return 0;
     }
}

/**
 * @brief Convert result to a double
 * @param res Result
 * @return The result
 */
double
esql_res_to_double(Esql_Res *res)
{
   Esql_Row *row;
   Esql_Cell *cell;
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, 0.0);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(res->row_count != 1, 0.0);
   row = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);
   cell = EINA_INLIST_CONTAINER_GET(row->cells, Esql_Cell);
   switch (cell->type)
     {
      case ESQL_CELL_TYPE_FLOAT:
        return cell->value.f;
        break;
      case ESQL_CELL_TYPE_DOUBLE:
        return cell->value.d;
        break;
      default:
        return 0.0;
     }
   return EINA_TRUE;
}

/**
 * @brief Convert result to a tm struct
 * @param res Result
 * @return Pointer to allocated struct tm (must be freed)
 */
struct tm *
esql_res_to_tm(Esql_Res *res)
{
   Esql_Row *row;
   Esql_Cell *cell;
   struct tm *ret;
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(res->row_count != 1, NULL);
   row = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);
   cell = EINA_INLIST_CONTAINER_GET(row->cells, Esql_Cell);
   EINA_SAFETY_ON_TRUE_RETURN_VAL((cell->type != ESQL_CELL_TYPE_TIMESTAMP) &&
     (cell->type != ESQL_CELL_TYPE_UNKNOWN), NULL);
   ret = calloc(1, sizeof(struct tm));
   memcpy(ret, &cell->value.tm, sizeof(struct tm));
   return ret;
}

/**
 * @brief Convert result to a timeval struct
 * @param res Result
 * @return Pointer to allocated struct timeval (must be freed)
 */
struct timeval *
esql_res_to_timeval(Esql_Res *res)
{
   Esql_Row *row;
   Esql_Cell *cell;
   struct timeval *ret;
   EINA_SAFETY_ON_NULL_RETURN_VAL(res, NULL);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(res->row_count != 1, NULL);
   row = EINA_INLIST_CONTAINER_GET(res->rows, Esql_Row);
   cell = EINA_INLIST_CONTAINER_GET(row->cells, Esql_Cell);
   EINA_SAFETY_ON_TRUE_RETURN_VAL((cell->type != ESQL_CELL_TYPE_TIME) &&
     (cell->type != ESQL_CELL_TYPE_UNKNOWN), NULL);
   ret = calloc(1, sizeof(struct timeval));
   memcpy(ret, &cell->value.tv, sizeof(struct timeval));
   return ret;
}

/*********************************************************************/

/**
 * @brief Convert cell to a long long int
 * @param cell Cell
 * @return The result
 */
long long int
esql_cell_to_lli(Esql_Cell *cell)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(cell, 0);
   switch (cell->type)
     {
      case ESQL_CELL_TYPE_TINYINT:
        return cell->value.c;
      case ESQL_CELL_TYPE_SHORT:
        return cell->value.s;
      case ESQL_CELL_TYPE_LONG:
        return cell->value.i;
      case ESQL_CELL_TYPE_LONGLONG:
        return cell->value.l;
      default:
        return 0;
     }
}

/**
 * @brief Convert cell to a double
 * @param cell Cell
 * @return The result
 */
double
esql_cell_to_double(Esql_Cell *cell)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(cell, 0.0);
   switch (cell->type)
     {
      case ESQL_CELL_TYPE_FLOAT:
        return cell->value.f;
        break;
      case ESQL_CELL_TYPE_DOUBLE:
        return cell->value.d;
        break;
      default:
        return 0.0;
     }
}
/** @} */
