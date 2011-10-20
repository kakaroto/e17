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
#include <time.h>

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

#define ESQL_DEFAULT_PORT_MYSQL      "3306" /**< Convenience define for default MySQL port */
#define ESQL_DEFAULT_PORT_POSTGRESQL "5432" /**< Convenience define for default PostgreSQL port */

/**
 * @defgroup Esql_Events Esql Events
 * @brief Events that are emitted from the library
 * @{
 */
extern int ESQL_EVENT_ERROR; /**< Event emitted on error, ev object is #Esql */
extern int ESQL_EVENT_CONNECT; /**< Event emitted on connection to db, ev object is #Esql */
extern int ESQL_EVENT_DISCONNECT; /**< Event emitted on disconnection from db, ev object is #Esql */
extern int ESQL_EVENT_RESULT; /**< Event emitted on query completion, ev object is #Esql_Res */
/** @} */
/**
 * @defgroup Esql_Typedefs Esql types
 * @brief These types are used throughout the library
 * @{
 */

/**
 * @typedef Esql
 * Base Esskyuehl object for connecting to servers
 */
typedef struct Esql     Esql;
/**
 * @typedef Esql_Res
 * Esskyuehl result set object for managing query results
 */
typedef struct Esql_Res Esql_Res;
/**
 * @typedef Esql_Row
 * Esskyuehl row object for accessing result rows
 */
typedef struct Esql_Row Esql_Row;

/**
 * @typedef Esql_Query_Id
 * Id to use as a reference for a query
 */
typedef unsigned long Esql_Query_Id;

/**
 * @typedef Esql_Query_Cb
 * Callback to use with a query
 * @see esql_query_callback_set
 */
typedef void (*Esql_Query_Cb)(Esql_Res *, void *);

/**
 * @typedef Esql_Connect_Cb
 * Callback to use with a query
 * @see esql_connect_callback_set
 */
typedef void (*Esql_Connect_Cb)(Esql *, void *);

/**
 * @typedef Esql_Type
 * Convenience enum for determining server backend type
 */
typedef enum
{
   ESQL_TYPE_NONE,
#define ESQL_TYPE_DRIZZLE ESQL_TYPE_MYSQL /**< Drizzle supports the mysql protocol */
   ESQL_TYPE_MYSQL,
   ESQL_TYPE_POSTGRESQL
} Esql_Type;

/**
 * @typedef Esql_Cell_Type
 * This type determines which value to use from an #Esql_Cell
 */
typedef enum
{
   ESQL_CELL_TYPE_UNKNOWN,
   ESQL_CELL_TYPE_TIME,
   ESQL_CELL_TYPE_TIMESTAMP,
   ESQL_CELL_TYPE_STRING,
   ESQL_CELL_TYPE_BLOB,
   ESQL_CELL_TYPE_TINYINT,
   ESQL_CELL_TYPE_SHORT,
   ESQL_CELL_TYPE_LONG,
   ESQL_CELL_TYPE_LONGLONG,
   ESQL_CELL_TYPE_FLOAT,
   ESQL_CELL_TYPE_DOUBLE
} Esql_Cell_Type;

/**
 * @typedef Esql_Cell
 * Low-level object for managing cells in an #Esql_Row
 * @note No value in this object is allocated, and all members belong
 * to other objects.
 */
typedef struct Esql_Cell
{
   EINA_INLIST; /**< use to iterate through cells */
   Esql_Row      *row; /**< parent row */
   Esql_Cell_Type type;
   const char    *colname; /**< NOT stringshared */

   union
   {
      char                 c; /**< ESQL_CELL_TYPE_TINYINT */
      short                s; /**< ESQL_CELL_TYPE_SHORT */
      int                  i; /**< ESQL_CELL_TYPE_LONG */
      long long int        l; /**< ESQL_CELL_TYPE_LONGLONG */
      float                f; /**< ESQL_CELL_TYPE_FLOAT */
      double               d; /**< ESQL_CELL_TYPE_DOUBLE */
      const char          *string; /**< ESQL_CELL_TYPE_STRING */
      const unsigned char *blob; /**< ESQL_CELL_TYPE_BLOB */
      struct tm            tm; /**< ESQL_CELL_TYPE_TIMESTAMP */
      struct timeval       tv; /**< ESQL_CELL_TYPE_TIME */
      /** ESQL_CELL_TYPE_UNKNOWN == NULL */
   } value;
   size_t len; /**< only valid with ESQL_CELL_TYPE_BLOB and ESQL_CELL_TYPE_STRING */
} Esql_Cell;
/** @} */
/* lib */
EAPI int esql_init(void);
EAPI int esql_shutdown(void);

/* esql */
EAPI Esql         *esql_new(Esql_Type type);
EAPI Esql         *esql_pool_new(int       size,
                                 Esql_Type type);
EAPI void         *esql_data_get(Esql *e);
EAPI void          esql_data_set(Esql *e,
                                 void *data);
EAPI Esql_Query_Id esql_current_query_id_get(Esql *e);
EAPI const char   *esql_current_query_get(Esql *e);
EAPI const char   *esql_error_get(Esql *e);
EAPI Eina_Bool     esql_type_set(Esql     *e,
                                 Esql_Type type);
EAPI Esql_Type     esql_type_get(Esql *e);
EAPI void          esql_free(Esql *e);

/* connect */
EAPI Eina_Bool esql_connect(Esql       *e,
                            const char *addr,
                            const char *user,
                            const char *passwd);
EAPI void esql_disconnect(Esql *e);
EAPI void esql_connect_callback_set(Esql           *e,
                                    Esql_Connect_Cb cb,
                                    void           *data);
EAPI Eina_Bool   esql_database_set(Esql       *e,
                                   const char *database_name);
EAPI const char *esql_database_get(Esql *e);
EAPI void        esql_connect_timeout_set(Esql  *e,
                                          double timeout);
EAPI double      esql_connect_timeout_get(Esql *e);
EAPI void        esql_reconnect_set(Esql     *e,
                                    Eina_Bool enable);
EAPI Eina_Bool   esql_reconnect_get(Esql *e);

/* query */
EAPI Esql_Query_Id esql_query(Esql       *e,
                              void       *data,
                              const char *query);
EAPI Esql_Query_Id esql_query_args(Esql       *e,
                                   void       *data,
                                   const char *fmt,
                                   ...);
EAPI Esql_Query_Id esql_query_vargs(Esql       *e,
                                    void       *data,
                                    const char *fmt,
                                    va_list     args);
EAPI Eina_Bool esql_query_callback_set(Esql_Query_Id id,
                                       Esql_Query_Cb callback);

/* res */
EAPI Esql          *esql_res_esql_get(Esql_Res *res);
EAPI const char    *esql_res_error_get(Esql_Res *res);
EAPI void          *esql_res_data_get(Esql_Res *res);
EAPI Esql_Query_Id  esql_res_query_id_get(Esql_Res *res);
EAPI const char    *esql_res_query_get(Esql_Res *res);
EAPI int            esql_res_rows_count(Esql_Res *res);
EAPI int            esql_res_cols_count(Esql_Res *res);
EAPI long long int  esql_res_rows_affected(Esql_Res *res);
EAPI long long int  esql_res_id(Esql_Res *res);
EAPI Eina_Iterator *esql_res_row_iterator_new(Esql_Res *res);

/* convert */
EAPI const char *    esql_res_to_string(Esql_Res *res);
EAPI unsigned char  *esql_res_to_blob(Esql_Res *res);
EAPI long long int   esql_res_to_lli(Esql_Res *res);
EAPI double          esql_res_to_double(Esql_Res *res);
EAPI struct tm      *esql_res_to_tm(Esql_Res *res);
EAPI struct timeval *esql_res_to_timeval(Esql_Res *res);
EAPI long long int   esql_cell_to_lli(Esql_Cell *cell);
EAPI double          esql_cell_to_double(Esql_Cell *cell);

/* row */
EAPI Eina_Inlist *esql_row_cells_get(Esql_Row *r);
EAPI int          esql_row_cell_count(Esql_Row *r);
EAPI Esql_Res    *esql_row_res_get(Esql_Row *r);
#endif
