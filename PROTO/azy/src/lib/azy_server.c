/*
 * Copyright 2010 Mike Blumenkrantz <mike@zentific.com>
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "Azy.h"
#include "azy_private.h"

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

void *
azy_server_module_data_get(Azy_Server_Module *module)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(module, NULL);

   return module->data;
}

Azy_Net *
azy_server_module_net_get(Azy_Server_Module *module)
{
   EINA_SAFETY_ON_NULL_RETURN_VAL(module, NULL);
   EINA_SAFETY_ON_NULL_RETURN_VAL(module->client, NULL);

   return module->client->net;
}

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

void
azy_server_module_def_pre_post_set(Azy_Server_Module_Def *def, Azy_Server_Module_Content_Cb pre, Azy_Server_Module_Content_Cb post)
{
   if (!AZY_MAGIC_CHECK(def, AZY_MAGIC_SERVER_MODULE_DEF))
     {
        AZY_MAGIC_FAIL(def, AZY_MAGIC_SERVER_MODULE_DEF);
        return;
     }
   def->pre = pre;
   def->post = post;
}

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

Azy_Server_Module_Method *
azy_server_module_method_new(const char *name, Azy_Server_Module_Content_Cb cb)
{
   Azy_Server_Module_Method *method;
   
   if ((!name) || (!cb))
     return NULL;

   method = calloc(1, sizeof(Azy_Server_Module_Method));
   EINA_SAFETY_ON_NULL_RETURN_VAL(method, NULL);

   method->name = eina_stringshare_add(name);
   method->method = cb;

   AZY_MAGIC_SET(method, AZY_MAGIC_SERVER_MODULE_METHOD);
   return method;
}

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

Azy_Server *
azy_server_new(Eina_Bool secure)
{
   Azy_Server *server;
   azy_init();

   if (!(server = calloc(1, sizeof(Azy_Server))))
     return NULL;

   if (!(server->add = ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_ADD, (Ecore_Event_Handler_Cb)azy_server_client_handler_add, server)))
     goto err;
   server->security.secure = secure;

   AZY_MAGIC_SET(server, AZY_MAGIC_SERVER);
   return server;

err:
   free(server);
   return NULL;
}

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

Eina_Bool
azy_server_client_send(Azy_Net      *net,
                       unsigned char *data,
                       int            length)
{
   Eina_Strbuf *header;

   if (!AZY_MAGIC_CHECK(net, AZY_MAGIC_NET))
     {
        AZY_MAGIC_FAIL(net, AZY_MAGIC_NET);
        return EINA_FALSE;
     }
   EINA_SAFETY_ON_NULL_RETURN_VAL(data, 0);

   EINA_SAFETY_ON_TRUE_RETURN_VAL(length < 1, 0);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!ecore_con_server_connected_get(net->conn), 0);
   EINA_SAFETY_ON_TRUE_RETURN_VAL(!(header = azy_net_header_create(net)), 0);

   if (!ecore_con_client_send(net->conn, eina_strbuf_string_get(header), eina_strbuf_length_get(header)))
     {
        ERR("Could not queue header for sending!");
        goto error;
     }

   EINA_SAFETY_ON_TRUE_GOTO(!ecore_con_server_send(net->conn, data, length), error);

error:
   eina_strbuf_free(header);
   return EINA_TRUE;
}

Eina_Bool
azy_server_run(Azy_Server     *server,
                int              type,
                int              port)
{
   const char *name;
   int az, ecore = ECORE_CON_REMOTE_NODELAY;

   if (!AZY_MAGIC_CHECK(server, AZY_MAGIC_SERVER))
     {
        AZY_MAGIC_FAIL(server, AZY_MAGIC_SERVER);
        return EINA_FALSE;
     }
   if (port < 1)
     return EINA_FALSE;

   az = type & AZY_SERVER_TYPE;

   if (az == AZY_SERVER_LOCAL)
     name = "127.0.0.1";
   else
     name = "0.0.0.0";

   if (az != type)
     {
        server->security.secure = EINA_TRUE;
        ecore |= ECORE_CON_USE_MIXED;
     }

   if (server->security.cert)
     ecore |= ECORE_CON_LOAD_CERT;

   server->server = ecore_con_server_add(ecore, name, port, server);
   if (!server->server)
     return EINA_FALSE;

   ecore_con_server_data_set(server->server, server);

   ecore_main_loop_begin();

   return EINA_TRUE;
}

Eina_Bool
azy_server_basic_run(int                      port,
                      int                      type,
                      const char              *cert,
                      Azy_Server_Module_Def **modules)
{
   Azy_Server *server;
   Azy_Server_Module_Def **mods;
   Eina_Bool secure = EINA_FALSE;
   const char *name;
   int az, ecore = ECORE_CON_REMOTE_NODELAY;

   if ((port < 1) || (port > 65535) || (!modules) || !(*modules))
     return EINA_FALSE;

   az = type & AZY_SERVER_TYPE;

   if (az == AZY_SERVER_LOCAL)
     name = "127.0.0.1";
   else
     name = "0.0.0.0";

   if (az != type)
     {
        secure = EINA_TRUE;
        ecore |= ECORE_CON_USE_MIXED;
     }
   if (!(server = azy_server_new(secure)))
     return EINA_FALSE;

   for (mods = modules; mods && *mods; mods++)
     {
        if (!azy_server_module_add(server, *mods))
          goto error;
     }

   if (cert)
     ecore |= ECORE_CON_LOAD_CERT;

   if (!(server->server = ecore_con_server_add(ecore, name, port, server)))
     goto error;

   ecore_con_server_data_set(server->server, server);

   if (secure && cert)
     {
        if (!(server->security.cert = ecore_con_ssl_server_cert_add(server->server, cert)))
          goto error;
        if (!(server->security.cert = ecore_con_ssl_server_privkey_add(server->server, cert)))
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

