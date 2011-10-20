/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
 */

#ifndef ESQL_PRIV_H
#define ESQL_PRIV_H

#include <Esskyuehl.h>
#include <Ecore.h>
#include <time.h>

#ifdef HAVE_ALLOCA_H
#include <alloca.h>
#elif defined __GNUC__
#define alloca __builtin_alloca
#elif defined _AIX
#define alloca __alloca
#else
#include <stddef.h>
void *alloca (size_t);
#endif

extern int esql_log_dom;
extern Eina_Hash *esql_query_callbacks;
extern Eina_Hash *esql_query_data;

#define DBG(...)            EINA_LOG_DOM_DBG(esql_log_dom, __VA_ARGS__)
#define INFO(...)           EINA_LOG_DOM_INFO(esql_log_dom, __VA_ARGS__)
#define WARN(...)           EINA_LOG_DOM_WARN(esql_log_dom, __VA_ARGS__)
#define ERR(...)            EINA_LOG_DOM_ERR(esql_log_dom, __VA_ARGS__)
#define CRI(...)            EINA_LOG_DOM_CRIT(esql_log_dom, __VA_ARGS__)

#if !defined(strdupa)
# define strdupa(str)       strcpy(alloca(strlen(str) + 1), str)
#endif

#if !defined(strndupa)
# define strndupa(str, len) strncpy(alloca(len + 1), str, len)
#endif

#ifndef EINA_INLIST_FOREACH_SAFE
#define EINA_INLIST_FOREACH_SAFE(list, list2, l)                                                                                         \
  for (l = (list ? _EINA_INLIST_CONTAINER(l, list) : NULL), list2 = l ? ((EINA_INLIST_GET(l) ? EINA_INLIST_GET(l)->next : NULL)) : NULL; \
       l;                                                                                                                                \
       l = _EINA_INLIST_CONTAINER(l, list2), list2 = list2 ? list2->next : NULL)
#endif

typedef enum
{
   ESQL_CONNECT_TYPE_NONE,
   ESQL_CONNECT_TYPE_INIT,
   ESQL_CONNECT_TYPE_DATABASE_SET,
   ESQL_CONNECT_TYPE_QUERY
} Esql_Connect_Type;

typedef const char *           (*Esql_Error_Cb)(Esql *);
typedef void                   (*Esql_Cb)(Esql *);
typedef Ecore_Fd_Handler_Flags (*Esql_Connection_Cb)(Esql *);
typedef void                   (*Esql_Setup_Cb)(Esql *, const char *, const char *, const char *);
typedef void                   (*Esql_Set_Cb)(); /* yes this is intentionally variable args */
typedef int                    (*Esql_Fd_Cb)(Esql *);
typedef char *                 (*Esql_Escape_Cb)(Esql *, const char *, va_list);
typedef void                   (*Esql_Res_Cb)(Esql_Res *);
typedef Esql_Row *             (*Esql_Row_Cb)(Esql_Res *);

typedef const char *           (*Esql_Row_Col_Name_Cb)(Esql_Row *);

typedef struct Esql_Pool
{
   EINA_INLIST;
   const char     *error;
   Eina_Bool       pool : 1;
   Eina_Bool       connected : 1;
   const char     *database;
   Esql_Type       type;
   Esql_Connect_Cb connect_cb;
   void           *connect_cb_data;
   void           *data;
   char           *cur_query;
   double          timeout;
   Esql_Query_Id   cur_id;
   Eina_Bool       reconnect : 1;
   Eina_Bool       pool_member : 1;
   /* non-esql */
   int             size;
   int             e_connected;
   Eina_Inlist    *esqls;
} Esql_Pool;

struct Esql
{
   EINA_INLIST;
   const char     *error;
   Eina_Bool       pool : 1;
   Eina_Bool       connected : 1;
   const char     *database;
   Esql_Type       type;
   Esql_Connect_Cb connect_cb;
   void           *connect_cb_data;
   void           *data;
   char           *cur_query;
   double          timeout;
   Esql_Query_Id   cur_id;
   Eina_Bool       reconnect : 1;
   Eina_Bool       pool_member : 1;

   struct
   {
      void              *db; /* db object pointer */
      char              *conn_str; /* unused for some db types */
      int                conn_str_len;
      Esql_Error_Cb      error_get;
      Esql_Connection_Cb connect;
      Esql_Cb            disconnect;
      Esql_Cb            free;
      Esql_Setup_Cb      setup;
      Esql_Set_Cb        database_set;
      Esql_Set_Cb        query;
      Esql_Connection_Cb database_send;
      Esql_Connection_Cb io;
      Esql_Fd_Cb         fd_get;
      Esql_Escape_Cb     escape;
      Esql_Res_Cb        res;
      Esql_Res_Cb        res_free;
   } backend;

   Esql_Pool        *pool_struct;
   unsigned int      pool_id;

   Ecore_Fd_Handler *fdh;
   Esql_Res         *res; /* current working result */
   Ecore_Timer      *timeout_timer;

   Esql_Connect_Type current;
   double            query_start;
   double            query_end;
   Eina_List        *backend_set_funcs; /* Esql_Set_Cb */
   Eina_List        *backend_set_params; /* char * */
   Eina_List        *backend_ids; /* Esql_Query_Id * */
   void             *cur_data;
};

struct Esql_Res
{
   const char   *error;
   Esql         *e; /* parent object */
   void         *data;

   Eina_Inlist  *rows;
   int           row_count;
   int           num_cols;
   long long int affected;
   long long int id;
   Esql_Query_Id qid;
   char         *query;

   struct
   {
      void *res; /* backend res type */
   } backend;
};

struct Esql_Row
{
                EINA_INLIST;

   Esql_Res    *res;

   Eina_Inlist *cells;
   int          num_cells;
   struct
   {
      void *row;
   } backend;
};

typedef struct Esql_Row_Iterator
{
   Eina_Iterator   iterator;

   const Esql_Row *head;
   const Esql_Row *current;
} Esql_Row_Iterator;

static inline void
esql_fake_free(void *data __UNUSED__,
               Esql      *e)
{
   e->error = NULL;
}


void      esql_mysac_init(Esql *e);
void      esql_postgresql_init(Esql *e);

void      esql_res_free(void *data __UNUSED__,
                        Esql_Res  *res);
void      esql_row_free(Esql_Row *r);

Eina_Bool esql_connect_handler(Esql             *e,
                               Ecore_Fd_Handler *fdh);

char *esql_query_escape(Eina_Bool   backslashes,
                        size_t     *len,
                        const char *fmt,
                        va_list     args);
char *esql_string_escape(Eina_Bool   backslashes,
                         const char *s);
Eina_Bool esql_timeout_cb(Esql *e);

Eina_Bool
esql_pool_rebalance(Esql_Pool *ep, Esql *e);
Esql_Query_Id
esql_pool_query(Esql_Pool  *ep,
                void       *data,
                const char *query);
Esql_Query_Id
esql_pool_query_args(Esql_Pool  *ep,
                     void       *data,
                     const char *fmt,
                     va_list     args);
void
esql_pool_disconnect(Esql_Pool *ep);
Eina_Bool
esql_pool_connect(Esql_Pool *ep,
                  const char *addr,
                  const char *user,
                  const char *passwd);
Eina_Bool
esql_pool_database_set(Esql_Pool  *ep,
                       const char *database_name);
Eina_Bool
esql_pool_type_set(Esql_Pool *ep,
                   Esql_Type  type);
void
esql_pool_connect_timeout_set(Esql_Pool *ep,
                              double     timeout);
void
esql_pool_reconnect_set(Esql_Pool *ep,
                        Eina_Bool  enable);
void
esql_pool_free(Esql_Pool *ep);
#endif
