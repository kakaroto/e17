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

Eina_Bool
esql_connect(Esql *e, const char *addr, const char *user,
                 const char *passwd)
{
   int ret, fd;
   
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e->backend.db, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(addr, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(user, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(passwd, EINA_FALSE);
   
   e->backend.setup(e, addr, user, passwd);
   if (e->connected)
     {
        ERR("Connection error: already connected!");
        return EINA_FALSE;
     }

   ret = e->backend.connect(e);
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

Eina_Bool
esql_disconnect(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e->backend.db, EINA_FALSE);

   ecore_main_fd_handler_del(e->fdh);
   e->fdh = NULL;
   e->backend.disconnect(e);
   return EINA_TRUE;
}

Eina_Bool
esql_database_set(Esql *e, const char *database_name)
{
   int ret;
   
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(e->backend.db, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(database_name, EINA_FALSE);
   if ((!e->fdh) || (!e->connected))
     {
        ERR("Esql object must be connected!");
        return EINA_FALSE;
     }
   if ((!e->backend_set_funcs) || (e->backend_set_funcs->data == esql_database_set))
     {
        e->backend.database_set(e, database_name);
        ret = e->backend.io(e);
        if (ret == ECORE_FD_ERROR)
          {
             ERR("Connection error: %s", e->backend.error_get(e));
             return EINA_FALSE;
          }
        ecore_main_fd_handler_active_set(e->fdh, ret);
        e->current = ESQL_CONNECT_TYPE_DATABASE_SET;
     }
   else
     {
        e->backend_set_funcs = eina_list_append(e->backend_set_funcs, esql_database_set);
        e->backend_set_params = eina_list_append(e->backend_set_params, strdup(database_name));
     }
   
   e->database = eina_stringshare_add(database_name);
   return EINA_TRUE;
}

const char *
esql_database_get(Esql *e)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(e, NULL);

   return e->database;
}
/** @} */
