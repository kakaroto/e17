#include "Evoak.h"
#include "evoak_private.h"


#define EVOAK_MAGIC                  0x6834bca1
#define EVOAK_OBJECT_MAGIC           0x715682fe
#define EVOAK_OBJECT_RECTANGLE_MAGIC 0x28a8bcf4
#define EVOAK_OBJECT_IMAGE_MAGIC     0x9874fedc
#define EVOAK_OBJECT_TEXT_MAGIC      0xab53cd82
#define EVOAK_OBJECT_LINE_MAGIC      0x924f8e9c
#define EVOAK_OBJECT_POLYGON_MAGIC   0xa73bc84a
#define EVOAK_OBJECT_GRADIENT_MAGIC  0xf73de985
#define EVOAK_OBJECT_EDJE_MAGIC      0x152efc93


static void
_evoak_ref(Evoak *ev)
{
   ev->ref++;
}

static void
_evoak_unref(Evoak *ev)
{
   ev->ref--;
   if (ev->ref <= 0)
     {
	if (ev->client_name) free(ev->client_name);
	if (ev->client_class) free(ev->client_class);
	ecore_ipc_server_del(ev->server);
	while (ev->font_path)
	  {
	     free(ev->font_path->data);
	     ev->font_path = evas_list_remove(ev->font_path, ev->font_path->data);
	  }
	while (ev->layers)
	  {
	     Evoak_Layer *lay;
	     
	     lay = ev->layers->data;
	     while (lay)
	       {
		  Evoak_Object *eo;
		  
		  eo = lay->objects->data;
		  _evoak_object_unref(eo);
		  if (!ev->layers) lay = NULL;
		  else lay = ev->layers->data;
	       }
	  }
	free(ev);
     }
}

static void
_evoak_request_id_inc(Evoak *ev)
{
   ev->request_id++;
}

static void
_evoak_request_id_unuse(Evoak *ev, int id)
{
}

static void
_evoak_object_id_inc(Evoak *ev)
{
   ev->object_id++;
}

static void
_evoak_object_ref(Evoak_Object *o)
{
   o->ref++;
   _evoak_ref(o->evoak);
}

static void
_evoak_object_unref(Evoak_Object *o)
{
   Evoak *ev;
   
   o->ref--;
   ev = o->evoak;
   if (o->ref <= 0)
     {
	_evoak_hash_unstore(&(ev->object_hash), o->id);
	_evoak_object_layer_del(o);
	if (o->clip)
	  o->clip->clipees = evas_list_remove(o->clip->clipees, o);
	while (o->clipees)
	  {
	     Evoak_Object *o2;
	     
	     o2 = o->clipees->data;
	     o2->clip = NULL;
	     o->clipees = evas_list_remove(o->clipees, o2);
	  }
	if (o->name)
	  {
	     o->evoak->object_names = evas_hash_del(o->evoak->object_names, o->name, o);
	     free(o->name);
	  }
	while (o->data)
	  {
	     Evoak_Data *ed;
	     
	     ed = o->data->data;
	     o->data = evas_list_remove(o->data, ed);
	     free(ed->key);
	     free(ed);
	  }
	while (o->callbacks)
	  {
	     Evoak_Callback *cb;
	     
	     cb = o->callbacks->data;
	     o->callbacks = evas_list_remove(o->callbacks, cb);
	     free(cb);
	  }
	_evoak_request_id_unuse(ev, o->id);
	if ((o->meta) && (o->meta_free)) o->meta_free(o->meta);
	o->magic = 0xf1f2fffe;
	free(o);
     }
   _evoak_unref(ev);
}

/****************************************************************************/

/*
 * Init any systems needed to run evoak
 * @return Greater than 0 if successful, 0 if not
 * 
 * This will intialize any data, variables, structs, files etc. needed for
 * an application to use evoak. It should be called first before any other
 * evoak calls are used.
 */
int
evoak_init(void)
{
   _evoak_init_count++;
   if (_evoak_init_count > 1) return _evoak_init_count;
   ecore_init();
   ecore_ipc_init();
   _evoak_ipc_cb_server_add_handle = ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_ADD, _evoak_ipc_cb_server_add, NULL);
   _evoak_ipc_cb_server_del_handle = ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DEL, _evoak_ipc_cb_server_del, NULL);
   _evoak_ipc_cb_server_data_handle = ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, _evoak_ipc_cb_server_data, NULL);
   _evoak_protocol_init();
   if (!EVOAK_EVENT_CANVAS_INFO)
     {
	EVOAK_EVENT_CANVAS_INFO = ecore_event_type_new();
	EVOAK_EVENT_SYNC_REPLY = ecore_event_type_new();
	EVOAK_EVENT_DISCONNECT = ecore_event_type_new();
     }
   evas_imaging_image_cache_set(0); /* flush cache */
   evas_imaging_image_cache_set(21 * 1024); /* 32kb image cache for image metadata */
   evas_imaging_font_cache_set(0); /* flush cache */
   evas_imaging_font_cache_set(256 * 1024); /* 256kb font cache for image metadata */
   edje_init();
   return _evoak_init_count;
}

/*
 * Shut down the evoak system
 * @return 0 if the system completely shut down successfully
 * 
 * This will shut down Evoak. No evoak calls should be called after this, until
 * another evoak_init() is called.
 */
int
evoak_shutdown(void)
{
   _evoak_init_count--;
   if (_evoak_init_count > 0) return _evoak_init_count;
   edje_shutdown();
   _evoak_protocol_shutdown();
   if (_evoak_ipc_cb_server_add_handle)
     {
	ecore_event_handler_del(_evoak_ipc_cb_server_add_handle);
	_evoak_ipc_cb_server_add_handle = NULL;
     }
   if (_evoak_ipc_cb_server_del_handle)
     {
	ecore_event_handler_del(_evoak_ipc_cb_server_del_handle);
	_evoak_ipc_cb_server_del_handle = NULL;
     }
   if (_evoak_ipc_cb_server_data_handle)
     {
	ecore_event_handler_del(_evoak_ipc_cb_server_data_handle);
	_evoak_ipc_cb_server_data_handle = NULL;
     }
   ecore_ipc_shutdown();
   ecore_shutdown();
   return _evoak_init_count;
}

/*
 * Connect to an evoak shared canvas
 * @param server The string ID of the server to connect to
 * @param client_name The string name of the client
 * @param client_class The string class of the client
 * @return A pointer to an Evoak connection, or NULL on failure
 * 
 * This connects to an evoak canvas shared server. 1 server shares 1 canvas
 * only. If @p server is NULL then the default server will be used. The
 * default is determined by the EVOAK_SERVER environment variable or if not
 * set then the local users "evoak" named server running over unix sockets
 * under instance 0 will be connected to. This parameter is currently unused.
 * 
 * NULL will be returned if connection setup fails or memory cannot be
 * allocated for the connection. Otherwise a pointer to the connection will
 * be returned and should be used to addess this particular shared canvas from
 * now until evoak_disconnect() is called on the handle.
 * 
 * This does NOT mean the connection is completely successful. Authentication
 * may fail or the server may be full, and so a callback will be called later
 * indicating this. The client will recieve a series of events after connecting
 * giving information about the shared canvas. It is suggested the client waits
 * until these events have arrived before doing anything.
 * 
 * The @p client_name is a string identification to help identify this client
 * and tell it apart from others. It is suggested the application name be used
 * as this is normally unique. This should really never shange for a client
 * so it can always be readily identified as the same client name in future.
 * 
 * The @p client_class is a string indicating what type of client it is. For
 * example, it may be a "background" client or a "panel" client, a
 * "application" client or a "custom" client. The fill list of class names
 * is not covered here and is not yet defined.
 */
Evoak *
evoak_connect(char *server, char *client_name, char *client_class)
{
   Evoak *ev;
   
   ev = calloc(1, sizeof(Evoak));
   if (!ev) return NULL;
   ev->magic = EVOAK_MAGIC;
   /* FIXME: parse "server" string to determine local, remote, SSL and */
   /* instance number */
   ev->server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER,
					 "evoak",
					 0,
					 ev);
   if (!ev->server)
     {
	free(ev);
	return NULL;
     }
   ev->client_name = strdup(client_name);
   ev->client_class = strdup(client_class);
   ev->request_id = 1;
   ev->object_id = 1;
   _evoak_ref(ev);
   return ev;
}

/*
 * Disconnect from an evoak shared canvas
 * @param ev The shared canvas handle to disconnect from
 * 
 * This disconnects from a shared canvas @p ev, flushing any buffers of unsent
 * requests and terminating the connection and freeing all memory and resources
 * on the client side, associated with this shared canvas. No more requests
 * should be sent to this shared canvas handle as it is now invalid.
 */
void
evoak_disconnect(Evoak *ev)
{
   Evas_List *l, *ll, *tmpl = NULL;
   
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   for (l = ev->layers; l; l = l->next)
     {
	Evoak_Layer *lay;
	
	lay = l->data;
	for (ll = lay->objects; ll; ll = ll->next)
	  {
	     Evoak_Object *eo;
	     
	     eo = ll->data;
	     tmpl = evas_list_append(tmpl, eo);
	  }
     }
   while (tmpl)
     {
	Evoak_Object *eo;
	
	eo = tmpl->data;
	tmpl = evas_list_remove_list(tmpl, tmpl);
	_evoak_object_unref(eo);
     }
   _evoak_unref(ev);
}

void
evoak_output_size_get(Evoak *ev, Evoak_Coord *w, Evoak_Coord *h)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   if (w) *w = ev->w;
   if (h) *h = ev->h;
}

/*
 * Send a sync request with a given numeric Id to the server
 * @param ev The evoak server connection
 * @param id The numeric integer ID to send
 * 
 * This sends a sync request to the evoak server that will elicit a reply at
 * some point in the future of an event of type EVOAK_EVENT_SYNC_REPLY, so the
 * client should have an event handler callback for this type of event if it
 * wants the reply. When the reply is recieved the the id memeber of the
 * Evoak_Event_Sync_Reply event structure will have the same numeric ID sent
 * to the server by this call. This will allow the client to know that all
 * calls called before the sync call that produced this reply have been
 * completed. It is suggested that the *p id is chosen in a unique way so that
 * no other outstanding sync requests (that have not recieved replied) are
 * using the same ID value, so the sync can be identified uniquely.
 */
void
evoak_sync(Evoak *ev, int id)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return;
   _evoak_ipc_send(ev, EVOAK_PR_SYNC, 0, id, NULL, 0);
}

void
evoak_freeze(Evoak *ev)
{
   ev->freeze++;
   if (ev->freeze == 1)
     {
	_evoak_ipc_send(ev, EVOAK_PR_CLIENT_FREEZE, 0, 0, NULL, 0);
     }
}

void
evoak_thaw(Evoak *ev)
{
   ev->freeze--;
   if (ev->freeze == 0)
     {
	_evoak_ipc_send(ev, EVOAK_PR_CLIENT_THAW, 0, 0, NULL, 0);
     }
}
