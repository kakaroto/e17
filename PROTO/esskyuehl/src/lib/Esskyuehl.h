/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library;
 * if not, see <http://www.gnu.org/licenses/>.
 */

#ifndef ESQL_H
#define ESQL_H

#include <Eina.h>

#ifdef EAPI
# undef EAPI
#endif

#ifdef _WIN32
#  ifdef DLL_EXPORT
#   define EAPI __declspec(dllexport)
#  else
#   define EAPI
#  endif /* ! DLL_EXPORT */
#else
# ifdef __GNUC__
#  if __GNUC__ >= 4
#   define EAPI __attribute__ ((visibility("default")))
#  else
#   define EAPI
#  endif
# else
#  define EAPI
# endif
#endif /* ! _WIN32 */

extern int ESQL_EVENT_ERROR;
extern int ESQL_EVENT_CONNECT;
extern int ESQL_EVENT_RESULT;

typedef struct Esql Esql;
typedef struct Esql_Res Esql_Res;
typedef struct Esql_Row Esql_Row;
typedef struct Esql_Cell Esql_Cell;

typedef enum
{
   ESQL_TYPE_NONE,
   ESQL_TYPE_MYSQL
} Esql_Type;


typedef enum
{
   ESQL_CELL_TYPE_UNKNOWN,
   ESQL_CELL_TYPE_TIME,
   ESQL_CELL_TYPE_TIMESTAMP,
   ESQL_CELL_TYPE_STRING,
   ESQL_CELL_TYPE_TINYINT,
   ESQL_CELL_TYPE_SHORT,
   ESQL_CELL_TYPE_LONG,
   ESQL_CELL_TYPE_LONGLONG,
   ESQL_CELL_TYPE_FLOAT,
   ESQL_CELL_TYPE_DOUBLE
} Esql_Cell_Type;

struct Esql_Cell
{
   EINA_INLIST;
   Esql_Row *row; /* parent row */
   Esql_Cell_Type type;
   const char *colname;

   union
   {
      char c;
      short s;
      int i;
      long long int l;
      float f;
      double d;
      const char *string;
      struct tm *tm;
      struct timeval *tv;
   } value;
};

/* lib */
EAPI int esql_init(void);
EAPI int esql_shutdown(void);

/* esql */
Esql *esql_new(void);
EAPI void *esql_data_get(Esql *e);
EAPI void esql_data_set(Esql *e, void *data);
EAPI Eina_Bool esql_type_set(Esql *e, Esql_Type type);
EAPI Esql_Type esql_type_get(Esql *e);
EAPI void esql_free(Esql *e);

/* connect */
EAPI Eina_Bool esql_connect(Esql *e, const char *addr, const char *user,
                 const char *passwd);
EAPI Eina_Bool esql_disconnect(Esql *e);
EAPI Eina_Bool esql_database_set(Esql *e, const char *database_name);
EAPI const char *esql_database_get(Esql *e);

/* query */
EAPI Eina_Bool esql_query(Esql *e, const char *fmt, ...);

/* res */
EAPI Esql *esql_res_esql_get(Esql_Res *res);
EAPI const char *esql_res_error_get(Esql_Res *res);
EAPI int esql_res_rows_count(Esql_Res *res);
EAPI int esql_res_cols_count(Esql_Res *res);
EAPI long long int esql_res_rows_affected(Esql_Res *res);
EAPI long long int esql_res_id(Esql_Res *res);
EAPI Eina_Iterator *esql_res_row_iterator_new(Esql_Res *res);

/* row */
EAPI Eina_Inlist *esql_row_cells_get(Esql_Row *r);
EAPI int esql_row_cell_count(Esql_Row *r);
EAPI Esql_Res *esql_row_res_get(Esql_Row *r);
#endif
