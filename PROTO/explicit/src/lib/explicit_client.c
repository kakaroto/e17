#include <stdlib.h>

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include "explicit_client.h"

static inline void
_explicit_eet_connection_close(Eet_Connection *conn)
{
   eet_connection_close(conn, NULL);
}

static void
_explicit_context_cancel_request(void *data)
{
   Explicit_Client_Request *request;

   request = data;

   if (request->dying)
     return ;

   request->dying = EINA_TRUE;

   /* Cancel all clients */
   while (request->object_requests)
     _explicit_object_cancel(eina_list_data_get(request->object_requests),
			     EXPLICIT_CANCEL_LINK_DOWN);

   eina_stringshare_del(request->file);
   free(request);
}

static Eina_Bool
_explicit_read_cb(const void *eet_data, size_t size, void *user_data)
{
   /* FIXME: Handle eet packet here. */
   return EINA_FALSE;
}

static Eina_Bool
_explicit_write_cb(const void *data, size_t size, void *user_data)
{
   Explicit *context = user_data;

   if (ecore_con_server_send(context->server, data, size) != (int) size)
     return EINA_FALSE;
   return EINA_TRUE;
}

static Eina_Bool
_explicit_context_add(void *data, __UNUSED__ int type, void *event)
{
   Ecore_Con_Event_Server_Add *ev;
   Explicit *context;

   context = data;
   ev = event;

   if (ev->server != context->server)
     return EINA_TRUE;

   /* Send waiting request. */
   context->connected = EINA_TRUE;

   return EINA_FALSE;
}

static Eina_Bool
_explicit_restart_idler(void *data)
{
   Explicit *context = data;

   if (context->server || context->conn)
     return EINA_FALSE;

   context->server = ecore_con_server_connect(ECORE_CON_REMOTE_TCP,
					      context->remote,
					      context->port,
					      context);
   context->conn = eet_connection_new(_explicit_read_cb,
				      _explicit_write_cb,
				      context);
   context->idler = NULL;

   return EINA_FALSE;
}

static void
_explicit_restart(Explicit *context)
{
   /* At that point, we need to reinitialize everything.
    * But to prevent flood, we will delay this until next idle state.
    */
   if (context->idler)
     return ;

   context->idler = ecore_idler_add(_explicit_restart_idler, context);
}

static Eina_Bool
_explicit_context_del(void *data, __UNUSED__ int type, void *event)
{
   Ecore_Con_Event_Server_Del *ev;
   Explicit *context;

   context = data;
   ev = event;

   if (ev->server != context->server)
     return EINA_TRUE;

   /* Cancel all download. */
   eina_hash_free(context->requests_lookup);
   context->requests_lookup = eina_hash_int32_new(_explicit_context_cancel_request);

   context->server = NULL;
   ECLOSE(_explicit_eet_connection_close, context->conn);

   _explicit_restart(context);

   return EINA_FALSE;
}

static Eina_Bool
_explicit_context_data(void *data, __UNUSED__ int type, void *event)
{
   Ecore_Con_Event_Server_Data *ev;
   Explicit *context;

   context = data;
   ev = event;

   if (ev->server != context->server)
     return EINA_TRUE;

   /* Let eet rebuild the message */
   if (eet_connection_received(context->conn, ev->data, ev->size))
     {
	/* On failure we close everything ! */
	_explicit_eet_connection_close(context->conn);
	eina_hash_free(context->requests_lookup);

	context->requests_lookup = eina_hash_int32_new(_explicit_context_cancel_request);

	ECLOSE(ecore_con_server_del, context->server);

	_explicit_restart(context);
     }

   return EINA_FALSE;
}

EAPI Explicit *
explicit_init(const char *name)
{
   Ecore_Con_Server *server = NULL;
   Explicit *context = NULL;
   const char *remote = NULL;
   int port = 0;

   context = calloc(1, sizeof (Explicit));
   if (!context) return NULL;

   ecore_con_init();

   context->connected = EINA_FALSE;

   remote = getenv("EXPLICIT_REMOTE_SERVER");
   if (!remote) remote = EXPLICIT_REMOTE_SERVER;

   if (getenv("EXPLICIT_REMOTE_PORT"))
     port = atoi(getenv("EXPLICIT_REMOTE_PORT"));
   if (!port) port = EXPLICIT_REMOTE_PORT;

   server = ecore_con_server_connect(ECORE_CON_REMOTE_TCP, remote, port, context);
   if (!server) goto on_error;

   context->server = server;
   context->handler.add = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, _explicit_context_add, context);
   context->handler.del = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, _explicit_context_del, context);
   context->handler.data = ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, _explicit_context_data, context);
   if (!context->handler.add
       || !context->handler.del
       || !context->handler.data)
     goto on_error;

   context->requests_lookup = eina_hash_int32_new(_explicit_context_cancel_request);
   if (!context->requests_lookup) goto on_error;

   if (!context->conn) goto on_error;

   context->name = eina_stringshare_add(name);
   context->remote = eina_stringshare_add(remote);
   context->port = port;

   EINA_MAGIC_SET(context, EINA_MAGIC_EXPLICIT);

   explicit_edd_conn_init();

   return context;

 on_error:
   ECLOSE(ecore_con_server_del, context->server);
   ECLOSE(_explicit_eet_connection_close, context->conn);

   if (context->handler.add) ecore_event_handler_del(context->handler.add);
   if (context->handler.del) ecore_event_handler_del(context->handler.del);
   if (context->handler.data) ecore_event_handler_del(context->handler.data);
   ECLOSE(eina_hash_free, context->requests_lookup);
   ECLOSE(free, context);

   ecore_con_shutdown();

   return NULL;
}

EAPI void
explicit_shutdown(Explicit *context)
{
   if (!EINA_MAGIC_CHECK(context, EINA_MAGIC_EXPLICIT))
     {
	EINA_MAGIC_FAIL(context, EINA_MAGIC_EXPLICIT);
	return ;
     }

   explicit_edd_conn_shutdown();

   EINA_MAGIC_SET(context, 0);

   eina_hash_free(context->requests_lookup);

   ecore_event_handler_del(context->handler.add);
   ecore_event_handler_del(context->handler.del);
   ecore_event_handler_del(context->handler.data);

   ECLOSE(eina_stringshare_del, context->remote);
   ECLOSE(ecore_con_server_del, context->server);
   ECLOSE(_explicit_eet_connection_close, context->conn);
   ECLOSE(ecore_idler_del, context->idler);

   free(context);

   ecore_con_shutdown();
}

EAPI Eina_Bool
explicit_callback_add(Explicit *context, Explicit_Callback_Type type, Explicit_Callback cb, const void *data)
{
   Explicit_Callback_Object *callback;

   if (!EINA_MAGIC_CHECK(context, EINA_MAGIC_EXPLICIT))
     {
	EINA_MAGIC_FAIL(context, EINA_MAGIC_EXPLICIT);
	return EINA_FALSE;
     }

   if (!cb) return EINA_FALSE;
   if (type > EXPLICIT_CALLBACK_APPROVAL) return EINA_FALSE;

   callback = calloc(1, sizeof (Explicit_Callback_Object));
   if (!callback) return EINA_FALSE;

   callback->type = type;
   callback->cb = cb;
   callback->data = data;

   context->callbacks = eina_list_append(context->callbacks, callback);

   return EINA_TRUE;
}

EAPI Eina_Bool
xplicit_callback_del(Explicit *context, Explicit_Callback_Type type, Explicit_Callback cb, const void *data)
{
   Explicit_Callback_Object *callback;
   Eina_List *l;

   if (!EINA_MAGIC_CHECK(context, EINA_MAGIC_EXPLICIT))
     {
	EINA_MAGIC_FAIL(context, EINA_MAGIC_EXPLICIT);
	return EINA_FALSE;
     }

   if (!cb) return EINA_FALSE;
   if (type > EXPLICIT_CALLBACK_APPROVAL) return EINA_FALSE;

   EINA_LIST_FOREACH(context->callbacks, l, callback)
     if (callback->type == type
	 && callback->cb == cb
	 && callback->data == data)
       {
	  context->callbacks = eina_list_remove_list(context->callbacks, l);
	  free(callback);

	  return EINA_TRUE;
       }

   return EINA_FALSE;
}

/* We expect file to be a stringshare. */
Eina_Bool
_explicit_request_file(Explicit *context, const char *file, Explicit_Object_Request *request_object)
{
   Explicit_Client_Request *client = NULL;
   Eina_Iterator *it;
   Explicit_Request request;

   if (!context->conn)
     return EINA_FALSE;

   /* Look if we need to add a request. */
   it = eina_hash_iterator_data_new(context->requests_lookup);

   EINA_ITERATOR_FOREACH(it, client)
     /* Taking advantage of stringshare. */
     if (client->file == file)
       {
	  /* We found a pending download. */
	  request_object->client = client;
	  client->object_requests = eina_list_append(client->object_requests, request_object);

	  eina_iterator_free(it);
	  return EINA_TRUE;
       }

   eina_iterator_free(it);

   client = calloc(1, sizeof (Explicit_Client_Request));
   if (!client)
     return EINA_FALSE;

   if (!eina_hash_add(context->requests_lookup, &client->id, client))
     {
	free(client);
	return EINA_FALSE;
     }

   client->id = context->next_transaction++;
   client->file = eina_stringshare_ref(file);
   client->object_requests = eina_list_append(client->object_requests, request_object);
   request_object->client = client;

   request.id = client->id;
   request.url = file;
   request.cancel = EINA_FALSE;

   if (!eet_connection_send(context->conn, _explicit_request_descriptor, &request, NULL))
     goto on_error;

   return EINA_TRUE;

 on_error:
   eina_list_free(client->object_requests);
   eina_stringshare_del(client->file);
   eina_hash_del(context->requests_lookup, &client->id, client);

   ECLOSE(free, request_object->client);

   return EINA_FALSE;
}

Eina_Bool
_explicit_request_cancel(Explicit_Object_Request *request)
{
   Explicit_Client_Request *client;
   Explicit_Request cancel;
   Eina_Bool ret = EINA_FALSE;

   if (!request->obj->context->conn)
     {
	/* Connection is closed, so we are cleaning up our mess */
	client = request->client;
	client->object_requests = eina_list_remove(client->object_requests, request);

	eina_hash_del(request->obj->context->requests_lookup, &client->id, client);
	return EINA_TRUE;
     }

   client = request->client;
   client->object_requests = eina_list_remove(client->object_requests, request);

   if (eina_list_count(client->object_requests) > 0)
     return EINA_TRUE;

   cancel.id = client->id;
   cancel.url = NULL;
   cancel.cancel = EINA_TRUE;

   if (!eet_connection_send(request->obj->context->conn, _explicit_request_descriptor, &cancel, NULL))
     return EINA_FALSE;

   eina_hash_del(request->obj->context->requests_lookup, &client->id, client);

   return ret;
}


