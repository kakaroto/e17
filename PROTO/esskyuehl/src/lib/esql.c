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

/**
 * @defgroup Esql_Library Library
 * @brief Functions to control initialization of the library
 * @{*/
int esql_log_dom = -1;
static int esql_init_count_ = 0;

EAPI int ESQL_EVENT_ERROR = 0;
EAPI int ESQL_EVENT_CONNECT = 0;
EAPI int ESQL_EVENT_RESULT = 0;
EAPI int ESQL_EVENT_DISCONNECT = 0;

static Eina_Inlist *esql_modules = NULL;

static Eina_Bool module_check(Eina_Module *m, void *d __UNUSED__)
{
   const char *filename;
   Esql_Module_Cb cb;
   Esql_Module *mod;

   filename = eina_module_file_get(m);
   filename = strrchr(filename, '/') + 1;
   if (strcmp(filename + (strlen(filename) - 3), SHARED_LIB_SUFFIX))
     return EINA_FALSE;

   if (!eina_module_load(m))
     {
        ERR("Could not verify module named '%s'. Error: '%s'", filename, eina_error_msg_get(eina_error_get()));
        return EINA_FALSE;
     }

   cb = eina_module_symbol_get(m, "esql_module_init");
   if (!cb)
     {
        eina_module_unload(m);
        return EINA_FALSE;
     }

   mod = calloc(1, sizeof(Esql_Module));
   if (mod)
     {
        mod->type = cb(NULL);
        mod->init = cb;
        mod->module = m;
        esql_modules = eina_inlist_append(esql_modules, EINA_INLIST_GET(mod));
     }

   return EINA_TRUE;
}


/**
 * @brief Initialize Esql
 *
 * This function initializes modules, events, and logging functions for Esql.
 * It must be called prior to using any other Esql functions.
 * @return The number of times the function has been called, or -1 on failure
 */
int
esql_init(void)
{
   Eina_Array *mods;
   if (++esql_init_count_ != 1)
     return esql_init_count_;
   if (!eina_init()) return 0;
   esql_log_dom = eina_log_domain_register("esskyuehl", EINA_COLOR_BLUE);
   if (esql_log_dom < 0)
     {
        ERR("Could not register 'esskyuehl' log domain!");
        goto eina_fail;
     }
   if (!ecore_init()) goto fail;
   if (!esql_mempool_init()) goto memfail;
   mods = eina_module_list_get(NULL, ESQL_MODULE_PATH, EINA_FALSE, (Eina_Module_Cb)module_check, NULL);
   if (!mods) goto module_fail;
   eina_array_free(mods);

   ESQL_EVENT_ERROR = ecore_event_type_new();
   ESQL_EVENT_RESULT = ecore_event_type_new();
   ESQL_EVENT_CONNECT = ecore_event_type_new();
   ESQL_EVENT_DISCONNECT = ecore_event_type_new();

   return esql_init_count_;

module_fail:
   esql_mempool_shutdown();
memfail:
   ecore_shutdown();
fail:
   eina_log_domain_unregister(esql_log_dom);
   esql_log_dom = -1;
eina_fail:
   eina_shutdown();
   return 0;
}

/**
 * @brief Shut down Esql
 *
 * This function uninitializes memory allocated by esql_init.
 * Call when no further Esql functions will be used.
 * @return The number of times the esql_init has been called, or -1 if
 * all occurrences of esql have been shut down
 */
int
esql_shutdown(void)
{
   Esql_Module *mod;
   Eina_Inlist *ll;
   if (--esql_init_count_ != 0)
     return esql_init_count_;

   if (esql_query_callbacks) eina_hash_free(esql_query_callbacks);
   esql_query_callbacks = NULL;
   if (esql_query_data) eina_hash_free(esql_query_data);
   esql_query_data = NULL;
   EINA_INLIST_FOREACH_SAFE(esql_modules, ll, mod)
     {
        eina_module_free(mod->module);
        free(mod);
     }
   esql_modules = NULL;
   eina_log_domain_unregister(esql_log_dom);
   ecore_shutdown();
   esql_mempool_shutdown();
   eina_shutdown();
   esql_log_dom = -1;
   return esql_init_count_;
}

/** @} */
/**
 * @defgroup Esql_Object Objects
 * @brief Functions to control Esql objects
 * @{*/

/**
 * @brief Create a new #Esql object
 * @param The database type to use
 * This function does nothing but allocate a generic Esskyuehl struct.
 * @return The new object, or NULL on failure
 */
Esql *
esql_new(Esql_Type type)
{
   Esql *e;

   e = calloc(1, sizeof(Esql));
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);
   esql_type_set(e, type);

   return e;
}

/**
 * @brief Retrieve data previously associated with an object
 * @param e The #Esql object (NOT NULL)
 * @return The data
 */
void *
esql_data_get(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);
   return e->data;
}

/**
 * @brief Associate data with an #Esql object for later use
 * @param e The #Esql object (NOT NULL)
 * @param data The data to associate
 */
void
esql_data_set(Esql *e,
              void *data)
{
   EINA_SAFETY_ON_NULL_RETURN(e);
   e->data = data;
}

/**
 * @brief Set the database type of an #Esql object
 * This function sets up all necessary function hooks to use @p e with a database
 * type specified by @p type.
 * @note If this function is called on an object multiple times, any connections associated
 * with the object will be immediately lost.
 * @param e The #Esql object (NOT NULL)
 * @param type The type of database to use.
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
esql_type_set(Esql     *e,
              Esql_Type type)
{
   Esql_Module *mod;
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   if (e->pool) return esql_pool_type_set((Esql_Pool *)e, type);

   if ((type != e->type) && e->backend.db && e->backend.free)
     e->backend.free(e);

   memset(&e->backend, 0, sizeof(e->backend));
   EINA_INLIST_FOREACH(esql_modules, mod)
     {
        if (type != mod->type) continue;
        mod->init(e);
     }
   if (e->type == type) return EINA_TRUE;

   ERR("No module present for SQL type %u!", type);
   return EINA_FALSE;
}

/**
 * @brief Return the database type of an #Esql object
 * @see esql_type_set
 * @param e The #Esql object (NOT NULL)
 * @return The database type currently used by @p e
 */
Esql_Type
esql_type_get(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, ESQL_TYPE_NONE);

   return e->type;
}

/**
 * @brief Return the #Esql_Query_Id of the current active query
 * @param e The #Esql object (NOT NULL)
 * @return The query id, or 0 if no query is active
 * @note For obvious reasons, this cannot be used reliably with
 * connection pools unless a callback is set
 */
Esql_Query_Id
esql_current_query_id_get(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, 0);

   return e->cur_id;
}

/**
 * @brief Return the query string for the current query
 * @param e The #Esql object (NOT NULL)
 * @return The query string (NOT stringshared)
 * @note For obvious reasons, this cannot be used reliably with
 * connection pools unless a callback is set
 */
const char *
esql_current_query_get(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);

   return e->cur_query;
}

/**
 * @brief Retrieve the error string associated with an #Esql object
 * This function will return NULL in all cases where an error has not occurred.
 * @param res The #Esql object (NOT NULL)
 * @return The error string, NULL if no error
 */
const char *
esql_error_get(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);

   return e->error;
}

/**
 * @brief Free an #Esql object
 * This function frees an #Esql object, shutting down all
 * open connections and freeing all db-related data.
 * @param e The #Esql object (NOT NULL)
 * @note If @p e has pending events, the object will not be freed
 * until after the events have completed.
 */
void
esql_free(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN(e);
   if (e->event_count)
     {
        e->dead = EINA_TRUE;
        return;
     }
   if (e->pool)
     {
        esql_pool_free((Esql_Pool *)e);
        return;
     }
   if (esql_modules)
     {
        if (e->connected) esql_disconnect(e);
        if (e->backend.free) e->backend.free(e);
     }
   if (e->backend_ids) eina_list_free(e->backend_set_funcs);
   if (e->backend_set_params) eina_list_free(e->backend_set_params);
   if (e->backend_ids) eina_list_free(e->backend_ids);
   free(e->cur_query);
   free(e);
}

/** @} */
