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
 * @param e The object to connect with (NOT NULL)
 * @param addr The address of the server (ie "127.0.0.1:3306") (NOT NULL)
 * @param user The username to connect with (NOT NULL)
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
   EINA_SAFETY_ON_NULL_RETURN_VAL(addr, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(user, EINA_FALSE);
   if (e->pool) return esql_pool_connect((Esql_Pool *)e, addr, user, passwd);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!e->backend.db && (e->type == ESQL_TYPE_MYSQL), EINA_FALSE);

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
 * @note Disconnecting is immediate, but an ESQL_EVENT_DISCONNECT is still emitted.
 * @param e The #Esql object (NOT NULL)
 */
void
esql_disconnect(Esql *e)
{
   DBG("(e=%p)", e);
   EINA_SAFETY_ON_NULL_RETURN(e);
   e->connected = EINA_FALSE;
   if (e->pool)
     {
        esql_pool_disconnect((Esql_Pool *)e);
        return;
     }
   EINA_SAFETY_ON_NULL_RETURN(e->backend.db);

   e->backend.disconnect(e);
   if (e->fdh) ecore_main_fd_handler_del(e->fdh);
   e->fdh = NULL;
   INFO("Disconnected");
   ecore_event_add(ESQL_EVENT_DISCONNECT, e, (Ecore_End_Cb)esql_fake_free, NULL);
}

/**
 * @brief Set a connected callback for an #Esql object
 * Use this function to set a callback to override the ESQL_EVENT_CONNECTED event,
 * calling @p cb with @p data instead.
 * @param e The #Esql object (NOT NULL)
 * @param cb The callback
 * @param data The data
 */
void
esql_connect_callback_set(Esql           *e,
                          Esql_Connect_Cb cb,
                          void           *data)
{
   EINA_SAFETY_ON_NULL_RETURN(e);

   e->connect_cb = cb;
   e->connect_cb_data = data;
}

/**
 * @brief Set the currently active database
 * This function calls all necessary functions to switch databases to
 * @p database_name. After it is called, all subsequent queries should be assumed
 * to be directed at database specified by @p database_name.
 * @note This function should always be called before esql_connect to ensure portability.
 * @param e The #Esql object (NOT NULL)
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
   if (e->database && (e->type == ESQL_TYPE_POSTGRESQL))
     {
        ERR("Database switching is not supported by this database type!");
        return EINA_FALSE;
     }
   eina_stringshare_replace(&e->database, database_name);

   if (e->pool) return esql_pool_database_set((Esql_Pool *)e, database_name);

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
        e->backend_ids = eina_list_append(e->backend_ids, e);
     }

   return EINA_TRUE;
}

/**
 * @brief Return the currently active database name
 * @param e The #Esql object (NOT NULL)
 * @return The stringshared database name, or NULL on failure
 */
const char *
esql_database_get(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);

   return e->database;
}

/**
 * @brief Set a timeout to disconnect after no activity
 * @param e The #Esql object (NOT NULL)
 * @param timeout The timeout in seconds
 *
 * Use this function to apply a timer policy to an Esql object and force disconnection
 * after @p timeout seconds of inactivity.
 * @note Setting a value <= 0 will disable this feature.
 */
void
esql_connect_timeout_set(Esql  *e,
                         double timeout)
{
   EINA_SAFETY_ON_NULL_RETURN(e);

   e->timeout = timeout;
   if (e->pool)
     {
        esql_pool_connect_timeout_set((Esql_Pool*)e, timeout);
        return;
     }
   if (timeout > 0.0)
     {
        if (e->timeout_timer) ecore_timer_delay(e->timeout_timer, e->timeout - ecore_timer_pending_get(e->timeout_timer));
        else e->timeout_timer = ecore_timer_add(timeout, (Ecore_Task_Cb)esql_timeout_cb, e);
     }
   else
     {
        ecore_timer_del(e->timeout_timer);
        e->timeout_timer = NULL;
        e->timeout = 0;
     }
}

/**
 * @brief Return the previously set timeout of an #Esql object
 * @param e The #Esql object (NOT NULL)
 * @return The timeout in seconds
 *
 * Use this function to return the inactivity timeout previously set with
 * esql_connect_timeout_set().
 */
double
esql_connect_timeout_get(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN(e);
   return e->timeout;
}

/**
 * @brief Set automatic reconnect mode
 * @param e The #Esql object (NOT NULL)
 * @param enable If EINA_TRUE, this feature is enabled.
 *
 * This function enables autoreconnect mode, where a server connection will automatically
 * re-establish itself if disconnection occurs for any reason.
 * @note If a ESQL_EVENT_DISCONNECT event is received, reconnection failed. Additionally,
 * calls to esql_disconnect() will override this feature.
 */
void
esql_reconnect_set(Esql     *e,
                   Eina_Bool enable)
{
   EINA_SAFETY_ON_NULL_RETURN(e);

   e->reconnect = enable;
   if (e->pool) esql_pool_reconnect_set((Esql_Pool*)e, enable);
}

/**
 * @brief Return the reconnect mode
 * @param e The #Esql object (NOT NULL)
 * @return If EINA_TRUE, this feature is enabled
 */
Eina_Bool
esql_reconnect_get(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   return e->reconnect;
}

/** @} */
