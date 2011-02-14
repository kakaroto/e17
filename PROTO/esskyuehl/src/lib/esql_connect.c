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
 * @defgroup Esql_Connect Connection
 * @brief Functions to manage/setup connections to databases
 * @{*/
/**
 * @brief Connect to an sql server
 * Use this function to connect to an sql server with @p e after
 * setting its type with esql_type_set.
 * @param e The object to connect with (NOT #NULL)
 * @param addr The address of the server (ie "127.0.0.1:3306") (NOT #NULL)
 * @param user The username to connect with (NOT #NULL)
 * @param passwd The password for @p user
 * @return EINA_TRUE if the connection could be started, else EINA_FALSE
 */
Eina_Bool
esql_connect(Esql       *e,
             const char *addr,
             const char *user,
             const char *passwd)
{
   int ret, fd;

   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(e->type == ESQL_TYPE_NONE, EINA_FALSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!e->backend.db && (e->type == ESQL_TYPE_MYSQL), EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(addr, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(user, EINA_FALSE);

   if (e->connected)
     {
        ERR("Connection error: already connected!");
        return EINA_FALSE;
     }
   e->backend.setup(e, addr, user, passwd);
   ret = e->backend.connect(e);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e->backend.db, EINA_FALSE);
   if (ret == ECORE_FD_ERROR)
     {
        ERR("Connection error: %s", e->backend.error_get(e));
        return EINA_FALSE;
     }
   fd = e->backend.fd_get(e);
   e->fdh = ecore_main_fd_handler_add(fd, ECORE_FD_READ | ECORE_FD_WRITE, (Ecore_Fd_Cb)esql_connect_handler, e, NULL, NULL);
   ecore_main_fd_handler_active_set(e->fdh, ret);
   e->current = ESQL_CONNECT_TYPE_INIT;

   return EINA_TRUE;
}

/**
 * @brief Disconnect from the currently connected server
 * This function calls all necessary functions to cleanly disconnect from the server
 * previously connected to by @p e.
 * @note Disconnecting is immediate.
 * @param e The #Esql object (NOT #NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
esql_disconnect(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e->backend.db, EINA_FALSE);

   if (e->fdh) ecore_main_fd_handler_del(e->fdh);
   e->fdh = NULL;
   e->backend.disconnect(e);
   e->connected = EINA_FALSE;
   return EINA_TRUE;
}

/**
 * @brief Set the currently active database
 * This function calls all necessary functions to switch databases to
 * @p database_name. After it is called, all subsequent queries should be assumed
 * to be directed at database specified by @p database_name.
 * @param e The #Esql object (NOT #NULL)
 * @param database_name The database name
 * @return EINA_TRUE on successful queue of the action, else EINA_FALSE
 */
Eina_Bool
esql_database_set(Esql       *e,
                  const char *database_name)
{
   DBG("(e=%p, database_name='%s')", e, database_name);

   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(database_name, EINA_FALSE);
   if (e->database && (!e->backend.database_set))
     {
        ERR("Database switching is not supported by this database type!");
        return EINA_FALSE;
     }
   eina_stringshare_replace(&e->database, database_name);

   if (!e->backend.database_set) return EINA_TRUE;
   if ((!e->current) && e->connected)
     {
        e->backend.database_set(e, database_name);
        e->current = ESQL_CONNECT_TYPE_DATABASE_SET;
     }
   else
     {
        e->backend_set_funcs = eina_list_append(e->backend_set_funcs, esql_database_set);
        e->backend_set_params = eina_list_append(e->backend_set_params, strdup(database_name));
     }

   return EINA_TRUE;
}

/**
 * @brief Return the currently active database name
 * @param e The #Esql object (NOT #NULL)
 * @return The stringshared database name, or #NULL on failure
 */
const char *
esql_database_get(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);

   return e->database;
}

/** @} */
