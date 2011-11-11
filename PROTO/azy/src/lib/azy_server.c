/*
 * Copyright 2010, 2011 Mike Blumenkrantz <mike@zentific.com>
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

#include "azy_private.h"

/**
 * @defgroup Azy_Server Server Functions
 * @brief Functions which affect #Azy_Server objects
 * @{
 */

/**
 * @brief Stop a running server
 *
 * This function stops running the specified server and stops the
 * main loop.
 * @param server The server object (NOT NULL)
 */
void
azy_server_stop(Azy_Server *server)
{
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return;
     }

   ecore_con_server_del(server->server);
   server->server = NULL;
   ecore_main_loop_quit();
}

/**
 * @brief Create a new server object
 *
 * Create a new server object.  If @p secure is specified, the server will require
 * ssl/tls for connections.
 * @param secure If true, server will use ssl for connections
 * @return The new #Azy_Server object, or #NULL on failure
 */
Azy_Server *
azy_server_new(Eina_Bool secure)
{
   Azy_Server *server;

   if (!(server = calloc(1, sizeof(Azy_Server))))
     return NULL;

   if (!(server->add = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_ADD, (Ecore_Event_Handler_Cb)azy_server_client_handler_add, server)))
     goto err;
   server->security.secure = !!secure;

   AZY_MAGIC_SET(server, AZY_MAGIC_SERVER);
   return server;

err:
   free(server);
   return NULL;
}

/**
 * @brief Free the given server
 *
 * This function frees @p server.
 * Note that the #Azy_Server_Module_Def objects are not freed
 * with this function.
 * @param server The server object to free (NOT NULL)
 */
void
azy_server_free(Azy_Server *server)
{
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return;
     }

   eina_list_free(server->module_defs);
   AZY_MAGIC_SET(server, AZY_MAGIC_NONE);
   free(server);
}

/**
 * @brief Return the current client count of a server
 *
 * This function returns the number of clients currently connected to @p server.
 * @param server The server object (NOT NULL)
 */
unsigned long int
azy_server_clients_count(Azy_Server *server)
{
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return 0;
     }

   return server->clients;
}

/**
 * @brief Retrieve the list of #Azy_Server_Module_Def objects from a server
 *
 * This function will return an #Eina_List of #Azy_Server_Module_Def objects
 * present in @p server.  This list and the objects all belong to the server.
 * They must NOT be freed.
 * @param server The server object (NOT NULL)
 * @return The list of #Azy_Server_Module_Def objects
 */
Eina_List *
azy_server_module_defs_get(Azy_Server *server)
{
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return NULL;
     }

   return server->module_defs;
}

/**
 * @brief Set the address for a server to listen on
 *
 * This function sets the listen address for @p server to @p addr.
 * Examples: 127.0.0.1, 0.0.0.0, localhost, etc
 * @param server The server object (NOT NULL)
 * @param addr The listen address (NOT NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_server_addr_set(Azy_Server *server,
                    const char *addr)
{
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(addr, EINA_FALSE);

   server->addr = eina_stringshare_add(addr);
   return EINA_TRUE;
}

/**
 * @brief Get the address for a server to listen on
 *
 * This function gets the listen address for @p server.
 * Examples: 127.0.0.1, 0.0.0.0, localhost, etc
 * @param server The server object (NOT NULL)
 * @return The listen address
 */
const char *
azy_server_addr_get(Azy_Server *server)
{
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return NULL;
     }

   return server->addr;
}

/**
 * @brief Enable security on a server
 * This is used to enable SSL/TLS functionality on @p server.
 * It cannot be disabled once enabled.
 * @param server The server
 */
void
azy_server_ssl_enable(Azy_Server *server)
{
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return;
     }
   server->security.secure = EINA_TRUE;
}

/**
 * @brief Set the port for a server to listen on
 *
 * This function sets the listen port for @p server to @p port.
 * @param server The server object (NOT NULL)
 * @param port The listen port
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_server_port_set(Azy_Server *server,
                    int port)
{
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_TRUE_RETURN_VAL((port < 0) || (port > 65535), EINA_FALSE);

   server->port = port;
   return EINA_TRUE;
}

/**
 * @brief Get the port for a server to listen on
 *
 * This function gets the listen port for @p server.
 * @param server The server object (NOT NULL)
 * @return The listen port
 */
int
azy_server_port_get(Azy_Server *server)
{
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return -1;
     }

   return server->port;
}


/**
 * @brief Add an RSA certificate to an Azy server
 *
 * This function stores a list of certificate files to be loaded
 * upon calling azy_server_run.
 * @param server The server object
 * @param cert_file The full path to the certificate
 * @return #EINA_TRUE on success, else #EINA_FALSE
 */
Eina_Bool
azy_server_cert_add(Azy_Server *server,
                    const char *cert_file)
{
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return EINA_FALSE;
     }

   server->security.cert_files = eina_list_append(server->security.cert_files, eina_stringshare_add(cert_file));
   server->security.secure = EINA_TRUE;
   return EINA_TRUE;
}

/**
 * @brief Run the specified server
 *
 * This function starts the specified server, calling ecore_main_loop_begin.
 * If the server's listen address has been previously specified, @p type should be
 * #AZY_SERVER_NONE, otherwise it will be detected based on @p type.
 * @param server The server object (NOT NULL)
 * @return EINA_TRUE on success (after main loop exits), else the function will return
 * immediately with EINA_FALSE
 */
Eina_Bool
azy_server_run(Azy_Server *server)
{
   int ecore = ECORE_CON_REMOTE_NODELAY;
   Eina_List *l;
   const char *f;

   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(server->addr, EINA_FALSE);

   if (server->security.secure)
     ecore |= ECORE_CON_USE_MIXED;

   if (server->security.cert_files)
     ecore |= ECORE_CON_LOAD_CERT;

   server->server = ecore_con_server_add(ecore, server->addr, server->port, server);
   if (!server->server) return EINA_FALSE;

   ecore_con_server_data_set(server->server, server);

   EINA_LIST_FOREACH(server->security.cert_files, l, f)
     {
        if (!ecore_con_ssl_server_cert_add(server->server, f))
          return EINA_FALSE;
        if (!ecore_con_ssl_server_privkey_add(server->server, f))
          return EINA_FALSE;
     }

   ecore_main_loop_begin();

   return EINA_TRUE;
}

/**
 * @brief Run a server in a single call with no prior setup
 *
 * This function will create a server and run it all in one call, but
 * provides less functionality and flexibility.
 * It will call ecore_main_loop_begin, set up server modules, and
 * free all allocated data upon return.
 * @param port The port to listen on (-1 < port < 65536)
 * @param type The #Azy_Server_Type of the server; #AZY_SERVER_LOCAL for localhost,
 * or #AZY_SERVER_BROADCAST for 0.0.0.0.  To listen on another address you will need
 * to manually set up your server object and use azy_server_run
 * @param cert The ssl certificate file to use; #AZY_SERVER_TLS must
 * be specified with @p type (NOT NULL)
 * @param modules An array of module definitions (NOT NULL)
 * @return EINA_TRUE on success (after main loop exits), else the function will return
 * immediately with EINA_FALSE
 */
Eina_Bool
azy_server_basic_run(int                     port,
                     int                     type,
                     const char             *cert,
                     Azy_Server_Module_Def **modules)
{
   Azy_Server *server;
   Azy_Server_Module_Def **mods;
   Eina_Bool secure = EINA_FALSE;
   int az, ecore = ECORE_CON_REMOTE_NODELAY;

   if ((port < 0) || (port > 65535) || (!modules) || !(*modules))
     return EINA_FALSE;

   az = type & AZY_SERVER_TYPE;

   if (az != type)
     {
        secure = EINA_TRUE;
        ecore |= ECORE_CON_USE_MIXED;
     }
   if (!(server = azy_server_new(secure)))
     return EINA_FALSE;

   if (az == AZY_SERVER_LOCAL)
     server->addr = eina_stringshare_add("127.0.0.1");
   else
     server->addr = eina_stringshare_add("0.0.0.0");

   for (mods = modules; mods && *mods; mods++)
     {
        if (!azy_server_module_add(server, *mods))
          goto error;
     }

   if (cert) ecore |= ECORE_CON_LOAD_CERT;

   if (!(server->server = ecore_con_server_add(ecore, server->addr, port, server)))
     goto error;

   ecore_con_server_data_set(server->server, server);

   if (secure && cert)
     {
        if (!ecore_con_ssl_server_cert_add(server->server, cert))
          goto error;
        if (!ecore_con_ssl_server_privkey_add(server->server, cert))
          goto error;
     }

   ecore_main_loop_begin();

   azy_server_free(server);
   return EINA_TRUE;
error:
   for (; modules && *modules; modules++)
     azy_server_module_del(server, *modules);
   if (server->server)
     ecore_con_server_del(server->server);
   azy_server_free(server);
   return EINA_FALSE;
}

/** @} */
