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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Azy.h"
#include "azy_private.h"

/**
 * @defgroup Azy_Server_Module Server Module Functions
 * @brief Functions which affect #Azy_Server_Module objects
 * @{
 */

/**
 * @brief Return the data received from a client
 * 
 * This function returns the received data from a server module (client).
 * This data is set only when clients have called HTTP PUT, and will be handled by
 * the __upload__ directive in the server.
 * @param module The server module (NOT #NULL)
 * @return The module's received data
 */
Azy_Net_Data *
azy_server_module_recv_get(Azy_Server_Module *module)
{
   if (!AZY_MAGIC_CHECK(module, AZY_MAGIC_SERVER_MODULE))
     {
        AZY_MAGIC_FAIL(module, AZY_MAGIC_SERVER_MODULE);
        return NULL;
     }
   return &module->recv;
}

/**
 * @brief Return the private data of a server module
 * 
 * This function returns the private data of a server module.
 * This data is set only in the server module definition function,
 * and has the value specified in the __attrs__ section of the module
 * in a .azy file.
 * @param module The server module (NOT #NULL)
 * @return The module's data
 */
void *
azy_server_module_data_get(Azy_Server_Module *module)
{
   if (!AZY_MAGIC_CHECK(module, AZY_MAGIC_SERVER_MODULE))
     {
        AZY_MAGIC_FAIL(module, AZY_MAGIC_SERVER_MODULE);
        return NULL;
     }

   return module->data;
}

/**
 * @brief Return the #Azy_Net object of the current module's
 * connection
 * 
 * This function is used to return the current module's network information,
 * allowing parsing of headers.
 * @param module The server module (NOT #NULL)
 * @return The #Azy_Net object
 */
Azy_Net *
azy_server_module_net_get(Azy_Server_Module *module)
{
   if (!AZY_MAGIC_CHECK(module, AZY_MAGIC_SERVER_MODULE))
     {
        AZY_MAGIC_FAIL(module, AZY_MAGIC_SERVER_MODULE);
        return NULL;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(module->client, NULL);

   return module->client->current;
}

/**
 * @brief Return the #Azy_Content object of the current module's connection
 *
 * This function is used to return the current module's return content object,
 * allowing manipulation of the return value.
 * @note This should only be used on a suspended module.
 * @param module The server module (NOT #NULL)
 * @return The #Azy_Content object
 */
Azy_Content *
azy_server_module_content_get(Azy_Server_Module *module)
{
   if (!AZY_MAGIC_CHECK(module, AZY_MAGIC_SERVER_MODULE))
     {
        AZY_MAGIC_FAIL(module, AZY_MAGIC_SERVER_MODULE);
        return NULL;
     }
   return module->content;
}

/**
 * @brief Return the #Azy_Server_Module_Def of a server with a given name
 * 
 * This function finds the #Azy_Server_Module_Def with @p name in @p server and
 * returns it.
 * @param server The server object (NOT #NULL)
 * @param name The #Azy_Server_Module_Def's name (NOT #NULL)
 * @return The #Azy_Server_Module_Def, or NULL on failure
 */
Azy_Server_Module_Def *
azy_server_module_def_find(Azy_Server *server,
                            const char  *name)
{
   Azy_Server_Module_Def *def;
   Eina_List *l;

   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return NULL;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(name, NULL);
   if (!server->module_defs)
     return NULL;

   EINA_LIST_FOREACH(server->module_defs, l, def)
     if (!strcmp(def->name, name))
       {
          INFO("Found module with name: '%s'", name);
          return def;
       }

   INFO("Could not find module with name: '%s'", name);
   return NULL;
}

/**
 * @brief Add a module to the server object
 * 
 * This function adds @p module to @p server.  After calling this,
 * the module should not be freed until the server has stopped running.
 * @param server The server object (NOT #NULL)
 * @param module The module definition (NOT #NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_server_module_add(Azy_Server            *server,
                      Azy_Server_Module_Def *module)
{
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return EINA_FALSE;
     }
   if (!module)
     return EINA_FALSE;

   if (azy_server_module_def_find(server, module->name))
     /* avoid adding same module twice */
     return EINA_TRUE;

   INFO("Adding new module: '%s'", module->name);
   server->module_defs = eina_list_append(server->module_defs, module);
   return EINA_TRUE;
}

/**
 * @brief Remove a module from the server object
 * 
 * This function removes @p module from @p server.  Once a module
 * has been removed, its methods can no longer be called.
 * Note that this function only removes the module from the server's list
 * and does not actually free the module.
 * @param server The server object (NOT #NULL)
 * @param module The module definition (NOT #NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_server_module_del(Azy_Server            *server,
                       Azy_Server_Module_Def *module)
{
   DBG("server=%p, module=%p", server, module);
   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(module, EINA_FALSE);

   if (!azy_server_module_def_find(server, module->name))
     return EINA_TRUE;

   server->module_defs = eina_list_remove(server->module_defs, module);
   return EINA_TRUE;
}

/**
 * @brief Create a new module definition with the given name
 * 
 * This function creates a blank #Azy_Server_Module_Def with @p name.
 * @param name The name of the module (NOT #NULL)
 * @return The new #Azy_Server_Module_Def, or #NULL on failure
 */
Azy_Server_Module_Def *
azy_server_module_def_new(const char *name)
{
   Azy_Server_Module_Def *def;

   EINA_SAFETY_ON_NULL_RETURN_VAL(name, NULL);

   def = calloc(1, sizeof(Azy_Server_Module_Def));
   EINA_SAFETY_ON_NULL_RETURN_VAL(def, NULL);

   def->name = eina_stringshare_add(name);
   AZY_MAGIC_SET(def, AZY_MAGIC_SERVER_MODULE_DEF);
   return def;
}

/**
 * @brief Free the given #Azy_Server_Module_Def
 * 
 * This function frees the given #Azy_Server_Module_Def, and should only
 * be called after the module will no longer be used.
 * @param def The #Azy_Server_Module_Def to free (NOT #NULL)
 */
void
azy_server_module_def_free(Azy_Server_Module_Def *def)
{
   Azy_Server_Module_Method *method;
   
   if (!AZY_MAGIC_CHECK(def, AZY_MAGIC_SERVER_MODULE_DEF))
     {
        AZY_MAGIC_FAIL(def, AZY_MAGIC_SERVER_MODULE_DEF);
        return;
     }

   eina_stringshare_del(def->name);
   EINA_LIST_FREE(def->methods, method)
     azy_server_module_method_free(method);
     
   AZY_MAGIC_SET(def, AZY_MAGIC_NONE);
   free(def);
}

/**
 * @brief Set the __init__ and __shutdown__ callback functions for a #Azy_Server_Module_Def
 * 
 * This function sets the callbacks called upon module load and module shutdown for @p def.
 * @param def The module definition (NOT #NULL)
 * @param init The callback function to call upon module init
 * @param shutdown The callback function to call upon module shutdown
 */
void
azy_server_module_def_init_shutdown_set(Azy_Server_Module_Def *def, Azy_Server_Module_Cb init, Azy_Server_Module_Shutdown_Cb shutdown)
{
   if (!AZY_MAGIC_CHECK(def, AZY_MAGIC_SERVER_MODULE_DEF))
     {
        AZY_MAGIC_FAIL(def, AZY_MAGIC_SERVER_MODULE_DEF);
        return;
     }
   def->init = init;
   def->shutdown = shutdown;
}

/**
 * @brief Set the __pre__ and __post__ callback functions for a #Azy_Server_Module_Def
 * 
 * This function sets the callbacks called before and after method calls for @p def.
 * @param def The module definition (NOT #NULL)
 * @param pre The callback function to call immediately before method calls
 * @param post The callback function to call immediately after method calls
 */
void
azy_server_module_def_pre_post_set(Azy_Server_Module_Def *def, Azy_Server_Module_Pre_Cb pre, Azy_Server_Module_Content_Cb post)
{
   if (!AZY_MAGIC_CHECK(def, AZY_MAGIC_SERVER_MODULE_DEF))
     {
        AZY_MAGIC_FAIL(def, AZY_MAGIC_SERVER_MODULE_DEF);
        return;
     }
   def->pre = pre;
   def->post = post;
}

/**
 * @brief Set the __download__ and __upload__ callback functions for a #Azy_Server_Module_Def
 * 
 * This function sets the callbacks called before and after method calls for @p def.
 * @param def The module definition (NOT #NULL)
 * @param download The callback function to call for HTTP GET requests
 * @param upload The callback function to call for HTTP PUT requests
 */
void
azy_server_module_def_download_upload_set(Azy_Server_Module_Def *def, Azy_Server_Module_Cb download, Azy_Server_Module_Cb upload)
{
   if (!AZY_MAGIC_CHECK(def, AZY_MAGIC_SERVER_MODULE_DEF))
     {
        AZY_MAGIC_FAIL(def, AZY_MAGIC_SERVER_MODULE_DEF);
        return;
     }
   def->download = download;
   def->upload = upload;
}

/**
 * @brief Set the __fallback__ callback function for a #Azy_Server_Module_Def
 * 
 * This function sets the callback that is called any time a user attempts
 * to call an undefined rpc method.
 * @param def The module definition (NOT #NULL)
 * @param fallback The callback function to call when an undefined method is requested
 */
void
azy_server_module_def_fallback_set(Azy_Server_Module_Def *def, Azy_Server_Module_Content_Cb fallback)
{
   if (!AZY_MAGIC_CHECK(def, AZY_MAGIC_SERVER_MODULE_DEF))
     {
        AZY_MAGIC_FAIL(def, AZY_MAGIC_SERVER_MODULE_DEF);
        return;
     }
   def->fallback = fallback;
}

/**
 * @brief Add a method to a module
 * 
 * This function adds a callable rpc method to module @p def.  After adding,
 * @p method should be considered as belonging to @p def until the module is unloaded.
 * @param def The module definition (NOT #NULL)
 * @param method The method to add (NOT #NULL)
 */
void
azy_server_module_def_method_add(Azy_Server_Module_Def *def, Azy_Server_Module_Method *method)
{
   if (!AZY_MAGIC_CHECK(def, AZY_MAGIC_SERVER_MODULE_DEF))
     {
        AZY_MAGIC_FAIL(def, AZY_MAGIC_SERVER_MODULE_DEF);
        return;
     }

   if (!AZY_MAGIC_CHECK(method, AZY_MAGIC_SERVER_MODULE_METHOD))
     {
        AZY_MAGIC_FAIL(method, AZY_MAGIC_SERVER_MODULE_METHOD);
        return;
     }
   def->methods = eina_list_append(def->methods, method);
}

/**
 * @brief Remove a method from a module
 * 
 * This function removes a callable rpc method from module @p def.  After
 * removal, @p method will no longer be callable.
 * Note that this does not free the method object.
 * @param def The module definition (NOT #NULL)
 * @param method The method to remove (NOT #NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_server_module_def_method_del(Azy_Server_Module_Def *def, Azy_Server_Module_Method *method)
{
   Eina_List *l;
   if (!AZY_MAGIC_CHECK(def, AZY_MAGIC_SERVER_MODULE_DEF))
     {
        AZY_MAGIC_FAIL(def, AZY_MAGIC_SERVER_MODULE_DEF);
        return EINA_FALSE;
     }

   if (!AZY_MAGIC_CHECK(method, AZY_MAGIC_SERVER_MODULE_METHOD))
     {
        AZY_MAGIC_FAIL(method, AZY_MAGIC_SERVER_MODULE_METHOD);
        return EINA_FALSE;
     }
   l = eina_list_data_find_list(def->methods, method);
   if (l)
     {
        def->methods = eina_list_remove_list(def->methods, l);
        return EINA_TRUE;
     }
   return EINA_FALSE;
}

/**
 * @brief Create a new method object with specified name and callback
 * 
 * This function creates a new method object with stringshared @p name and
 * callback @p cb.
 * @param name The name of the method
 * @param cb The callback of the method
 * @return The new #Azy_Server_Module_Method object, or #NULL on failure
 */
Azy_Server_Module_Method *
azy_server_module_method_new(const char *name, Azy_Server_Module_Content_Cb cb)
{
   Azy_Server_Module_Method *method;

   EINA_SAFETY_ON_NULL_RETURN_VAL(name, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(cb, NULL);

   method = calloc(1, sizeof(Azy_Server_Module_Method));
   EINA_SAFETY_ON_NULL_RETURN_VAL(method, NULL);

   method->name = eina_stringshare_add(name);
   method->method = cb;

   AZY_MAGIC_SET(method, AZY_MAGIC_SERVER_MODULE_METHOD);
   return method;
}

/**
 * @brief Free a method object
 * 
 * This function frees a method object.  After calling, the method will no
 * longer be callable.  This function must only be called AFTER
 * azy_server_module_def_method_del to avoid undefined methods remaining
 * in the module's method list after they've been freed.
 * @param method The method to free (NOT #NULL)
 */
void
azy_server_module_method_free(Azy_Server_Module_Method *method)
{
   if (!AZY_MAGIC_CHECK(method, AZY_MAGIC_SERVER_MODULE_METHOD))
     {
        AZY_MAGIC_FAIL(method, AZY_MAGIC_SERVER_MODULE_METHOD);
        return;
     }

   AZY_MAGIC_SET(method, AZY_MAGIC_NONE);
   eina_stringshare_del(method->name);
   free(method);
}

/**
 * @brief Return the size of the private data of a module
 * 
 * This function is equivalent to calling sizeof(Azy_Server_Module).
 * It returns the total size of the __attrs__ section of a module.
 * @param def The module def (NOT #NULL)
 * @return The size of the module, or -1 on failure
 */
int
azy_server_module_def_size_get(Azy_Server_Module_Def *def)
{
   if (!AZY_MAGIC_CHECK(def, AZY_MAGIC_SERVER_MODULE_DEF))
     {
        AZY_MAGIC_FAIL(def, AZY_MAGIC_SERVER_MODULE_DEF);
        return -1;
     }

   return def->data_size;
}

/**
 * @brief Set the size of the private data of a module
 * 
 * This function should never be called by users.
 * @param def The module def (NOT #NULL)
 * @param size The size of the module
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_server_module_size_set(Azy_Server_Module_Def *def, int size)
{
   if (!AZY_MAGIC_CHECK(def, AZY_MAGIC_SERVER_MODULE_DEF))
     {
        AZY_MAGIC_FAIL(def, AZY_MAGIC_SERVER_MODULE_DEF);
        return EINA_FALSE;
     }

   def->data_size = size;
   return EINA_TRUE;
}


/**
 * @brief Send data to a client
 * 
 * This function is used to queue arbitrary data to send to a client through its module.  It will automatically
 * generate all http header strings from @p net including the content-length (based on @p data).
 * @param module The client's #Azy_Server_Module object (NOT #NULL)
 * @param net An #Azy_Net object containing http information to use (NOT #NULL)
 * @param data The data to send (NOT #NULL)
 * @return EINA_TRUE on success, else EINA_FALSE
 */
Eina_Bool
azy_server_module_send(Azy_Server_Module  *module,
                       Azy_Net            *net,
                       const Azy_Net_Data *data)
{
   Eina_Strbuf *header;

   if (!AZY_MAGIC_CHECK(module, AZY_MAGIC_SERVER_MODULE))
     {
        AZY_MAGIC_FAIL(module, AZY_MAGIC_SERVER_MODULE);
        return EINA_FALSE;
     }

   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(data, EINA_FALSE);
   EINA_SAFETY_ON_NULL_RETURN_VAL(data->data, EINA_FALSE);

   azy_net_message_length_set(net, data->size);
   if (!net->http.res.http_code)
     azy_net_code_set(net, 200); /* OK */
   if (!net->type)
     azy_net_type_set(net, AZY_NET_TYPE_RESPONSE);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!(header = azy_net_header_create(net)), 0);

   if (!ecore_con_client_send(module->client->current->conn, eina_strbuf_string_get(header), eina_strbuf_length_get(header)))
     {
        ERR("Could not queue header for sending!");
        goto error;
     }

   EINA_SAFETY_ON_TRUE_GOTO(!ecore_con_client_send(net->conn, data->data, data->size), error);

error:
   eina_strbuf_free(header);
   return EINA_TRUE;
}

/**
 * @brief Return the state of an #Azy_Server_Module object
 * The return value of this function represents the connection state of the associated client.
 * @param module The module (NOT #NULL)
 * @return EINA_TRUE if the client is connected, else EINA_FALSE
 */
Eina_Bool
azy_server_module_active_get(Azy_Server_Module *module)
{
   if (!AZY_MAGIC_CHECK(module, AZY_MAGIC_SERVER_MODULE))
     {
        AZY_MAGIC_FAIL(module, AZY_MAGIC_SERVER_MODULE);
        return EINA_FALSE;
     }

   return !module->client->dead;
}

/** @} */
