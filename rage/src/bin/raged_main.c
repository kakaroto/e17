#include "e.h"
#include "sha1.h"

/* FIXME: this protocol is NOT secure. it has no ecryption and no
 * public/private key checks to make sure who you connect to is really a
 * network member and not a honeypot key harvester. the fact that this is
 * an unknown service without any visibility as of now means you are pretty
 * safe - at worst someone will be able to read your videos as your private
 * key should never leave your home
 */

typedef enum _Op
{
   OP_VERSION,
   OP_VERSION_ERROR,
   OP_SYNC,
   OP_NODE_ADD,
   OP_USER_AUTH,
   OP_USER_AUTH_ERROR,
   OP_USER_WHO,
   OP_USER_IDENT,
   OP_MEDIA_ADD,
   OP_MEDIA_DEL,
   OP_MEDIA_LOCK_NOTIFY,
   OP_MEDIA_UNLOCK_NOTIFY,
   OP_MEDIA_LOCK,
   OP_MEDIA_UNLOCK,
   OP_MEDIA_GET,
   OP_MEDIA_GET_DATA,
   OP_MEDIA_PUT,
   OP_MEDIA_PUT_DATA,
   OP_MEDIA_DELETE,
   OP_THUMB_GET,
   OP_THUMB_GET_DATA
} Op;

typedef struct _Client Client;
typedef struct _Node Node;

struct _Client
{
   Ecore_Ipc_Client *client;
   char *ident;
   unsigned char local : 1;
   unsigned char version_ok : 1;
   unsigned char auth_ok : 1;
   unsigned char who_ok : 1;
   unsigned char private_ok : 1;
};

struct _Node
{
   Ecore_Ipc_Server *server;
   char *name;
   char *key;
   char *ident;
   unsigned char connected : 1;
   unsigned char version_ok : 1;
   unsigned char who_ok : 1;
};

static int _version = 10; /* current proto version */
static int _version_magic1 = 0x14f8ec67; /* magic number for version check */
static int _version_magic2 = 0x3b45ef56; /* magic number for version check */
static int _version_magic3 = 0x8ea9fca0; /* magic number for version check */
static char *_key_private = "private"; /* auth key for anyone with read and write access - i.e. your own home and devices only - hardcoded for now */
static char *_key_public = "public"; /* public members of the network - read and borrow rights only - hardcoded for now */
static char *_ident_info = "user=x;location=y;"; /* send as ident in response to a who - for now hardcoded */
static Ecore_Ipc_Server *_server_local = NULL;
static Ecore_Ipc_Server *_server_remote = NULL;
static Evas_List *_clients = NULL;
static Evas_List *_nodes = NULL;

static void
_client_del(Client *cl)
{
   ecore_ipc_client_data_set(cl->client, NULL);
   _clients = evas_list_remove(_clients, cl->client);
   ecore_ipc_client_del(cl->client);
   if (cl->ident) free(cl->ident);
   free(cl);
}

static void
_node_del(Node *nd)
{
   _nodes = evas_list_remove(_nodes, nd);
   if (nd->server) ecore_ipc_server_del(nd->server);
   if (nd->name) free(nd->name);
   if (nd->key) free(nd->key);
   free(nd);
}

static int
_client_cb_add(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Ipc_Event_Server_Add *e;
   Node *nd;
   
   nd = ecore_ipc_server_data_get(e->server);
   if (!nd) return 1;
   nd->connected = 1;
   return 1;
}

static int
_client_cb_del(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Ipc_Event_Server_Del *e;
   Node *nd;
   
   nd = ecore_ipc_server_data_get(e->server);
   if (!nd) return 1;
   _node_del(nd);
   return 1;
}

static int
_client_cb_data(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Ipc_Event_Server_Data *e;
   Node *nd;
   
   nd = ecore_ipc_server_data_get(e->server);
   if (!nd) return 1;
   switch (e->major)
     {
      case OP_VERSION: /* version info from client */
	if ((e->minor != _version) ||
	    (e->ref != _version_magic1) ||
	    (e->ref_to != _version_magic2) ||
	    (e->response != _version_magic3))
	  /* client version not matching ours or magic wrong */
	  {
	     ecore_ipc_server_send(nd->server, OP_VERSION_ERROR, _version,
				   0, 0, 0, NULL, 0);
	     ecore_ipc_server_flush(nd->server);
	     _node_del(nd);
	  }
	else
	  {
	     nd->version_ok = 1;
	     ecore_ipc_server_send(nd->server, OP_USER_AUTH, 0,
				   0, 0, 0, nd->key, strlen(nd->key) + 1);
	     ecore_ipc_server_send(nd->server, OP_USER_WHO, 0,
				   0, 0, 0, NULL, 0);
	  }
	break;
      case OP_VERSION_ERROR: /* client does not like our version */
	ecore_ipc_server_flush(nd->server);
	_node_del(nd);
	break;
      case OP_SYNC: /* client requested a sync - reply with sync in e->minor */
	ecore_ipc_server_send(nd->server, OP_SYNC, e->minor,
			      0, 0, 0, NULL, 0);
	break;
      case OP_NODE_ADD: /* client lists all network nodes it knows of */
	if ((e->data) && (e->size > 1) && (((char *)e->data)[e->size - 1] == 0))
	  {
	     Node *nd2;
	     
	     nd2 = calloc(1, sizeof(Node));
	     if (nd2)
	       {
		  /* FIXME: need to avoid self-connect */
		  nd2->server = ecore_ipc_server_connect(ECORE_IPC_REMOTE_SYSTEM,
							 e->data, 9889, nd2);
		  if (!nd2->server)
		    {
		       free(nd2);
		    }
		  else
		    {
		       nd2->name = strdup(e->data);
		       nd2->key = strdup(nd->key);
		       _nodes = evas_list_append(_nodes, nd2);
		    }
	       }
	  }
	break;
      case OP_USER_AUTH: /* client should not get this */
	break;
      case OP_USER_AUTH_ERROR:
	_node_del(nd);
	break;
      case OP_USER_WHO:
	ecore_ipc_server_send(nd->server, OP_USER_IDENT, 0,
			      0, 0, 0, _ident_info, strlen(_ident_info) + 1);
	break;
      case OP_USER_IDENT:
	if ((e->data) && (e->size > 1) && (((char *)e->data)[e->size - 1] == 0))
	  {
	     if (nd->ident) free(nd->ident);
	     nd->ident = strdup(e->data);
	  }
	break; 
      case OP_MEDIA_ADD:
	break;
      case OP_MEDIA_DEL:
	break;
      case OP_MEDIA_LOCK_NOTIFY:
	break;
      case OP_MEDIA_UNLOCK_NOTIFY:
	break;
      case OP_MEDIA_LOCK: /* client should not get this */
	break;
      case OP_MEDIA_UNLOCK:
	break;
      case OP_MEDIA_GET: /* client should not get this */
	break;
      case OP_MEDIA_GET_DATA:
	break;
      case OP_MEDIA_PUT: /* client should not get this */
	break;
      case OP_MEDIA_PUT_DATA: /* client should not get this */
	break;
      case OP_MEDIA_DELETE: /* client should not get this */
	break;
      case OP_THUMB_GET: /* client should not get this */
	break;
      case OP_THUMB_GET_DATA:
	break;
      default:
	break;
     }
   return 1;
}

static int
_server_cb_add(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Ipc_Event_Client_Add *e;
   Client *cl;
   
   e = event;
   if (!((ecore_ipc_client_server_get(e->client) == _server_local) ||
	 (ecore_ipc_client_server_get(e->client) == _server_remote))) 
     return 1;
   cl = calloc(1, sizeof(Client));
   if (!cl) return 1;
   
   cl->client = e->client;
   if (ecore_ipc_client_server_get(e->client) == _server_local)
     cl->local = 1;
   ecore_ipc_client_data_set(e->client, cl);
   _clients = evas_list_append(_clients, cl);

   ecore_ipc_client_send(cl->client, OP_VERSION, _version,
			 _version_magic1, _version_magic2, _version_magic3,
			 NULL, 0);
   return 1;
}

static int
_server_cb_del(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Ipc_Event_Client_Del *e;
   Client *cl;
   
   e = event;
   if (!((ecore_ipc_client_server_get(e->client) == _server_local) ||
	 (ecore_ipc_client_server_get(e->client) == _server_remote))) 
     return 1;
   /* delete client sruct */
   cl = ecore_ipc_client_data_get(e->client);
   if (!cl) return 1;
   _client_del(cl);
   return 1;
}              

static int
_server_cb_data(void *data __UNUSED__, int type __UNUSED__, void *event)
{
   Ecore_Ipc_Event_Client_Data *e;
   Client *cl;
   
   e = event;
   if (!((ecore_ipc_client_server_get(e->client) == _server_local) ||
	 (ecore_ipc_client_server_get(e->client) == _server_remote))) 
     return 1;
   cl = ecore_ipc_client_data_get(e->client);
   if (!cl) return 1;
   switch (e->major)
     {
      case OP_VERSION: /* version info from client */
	if ((e->minor != _version) ||
	    (e->ref != _version_magic1) ||
	    (e->ref_to != _version_magic2) ||
	    (e->response != _version_magic3))
	  /* client version not matching ours or magic wrong */
	  {
	     ecore_ipc_client_send(cl->client, OP_VERSION_ERROR, _version,
				   0, 0, 0, NULL, 0);
	     ecore_ipc_client_flush(cl->client);
	     _client_del(cl);
	  }
	else
	  {
	     cl->version_ok = 1;
	  }
	break;
      case OP_VERSION_ERROR: /* client does not like our version */
	ecore_ipc_client_flush(cl->client);
	_client_del(cl);
	break;
      case OP_SYNC: /* client requested a sync - reply with sync in e->minor */
	ecore_ipc_client_send(cl->client, OP_SYNC, e->minor,
			      0, 0, 0, NULL, 0);
	break;
      case OP_NODE_ADD: /* client lists all network nodes it knows of */
	if ((e->data) && (e->size > 1) && (((char *)e->data)[e->size - 1] == 0))
	  {
	     Node *nd;
	     
	     nd = calloc(1, sizeof(Node));
	     if (nd)
	       {
		  /* FIXME: need to avoid self-connect */
/* no need to connect - just list known nodes - we are a server
 * clients would do this though - above client code is a snippet
 *
		  nd->server = ecore_ipc_server_connect(ECORE_IPC_REMOTE_SYSTEM,
							e->data, 9889, nd);
		  if (!nd->server)
		    {
		       free(nd);
		    }
		  else
*/ 		    {
		       nd->name = strdup(e->data);
		       if (cl->private_ok) nd->key = strdup(_key_private);
		       else nd->key = strdup(_key_public);
		       _nodes = evas_list_append(_nodes, nd);
		    }
	       }
	  }
	break;
      case OP_USER_AUTH:
	if (!cl->version_ok)
	  {
	     _client_del(cl);
	  }
	else
	  {
	     if ((e->data) && (e->size > 1) && (((char *)e->data)[e->size - 1] == 0))
	       {
		  if (!strcmp(e->data, _key_private))
		    {
		       cl->auth_ok = 1;
		       cl->private_ok = 1;
		       ecore_ipc_client_send(cl->client, OP_USER_WHO, 0,
					     0, 0, 0, NULL, 0);
		    }
		  else if (!strcmp(e->data, _key_public))
		    {
		       cl->auth_ok = 1;
		       ecore_ipc_client_send(cl->client, OP_USER_WHO, 0,
					     0, 0, 0, NULL, 0);
		    }
		  else
		    {
		       ecore_ipc_client_send(cl->client, OP_USER_AUTH_ERROR, 0,
					     0, 0, 0, NULL, 0);
		       ecore_ipc_client_flush(cl->client);
		       _client_del(cl);
		    }
	       }
	     else
	       {
		  ecore_ipc_client_send(cl->client, OP_USER_AUTH_ERROR, 0,
					0, 0, 0, NULL, 0);
		  ecore_ipc_client_flush(cl->client);
		  _client_del(cl);
	       }
	  }
	break;
      case OP_USER_AUTH_ERROR: /* server should never get this */
	break;
      case OP_USER_WHO:
	ecore_ipc_client_send(cl->client, OP_USER_IDENT, 0,
			      0, 0, 0, _ident_info, strlen(_ident_info) + 1);
	break;
      case OP_USER_IDENT:
	if ((e->data) && (e->size > 1) && (((char *)e->data)[e->size - 1] == 0))
	  {
	     if (cl->ident) free(cl->ident);
	     cl->ident = strdup(e->data);
	  }
	break;
      case OP_MEDIA_ADD: /* server should never get this */
	break;
      case OP_MEDIA_DEL: /* server should never get this */
	break;
      case OP_MEDIA_LOCK_NOTIFY: /* server should never get this */
	break;
      case OP_MEDIA_UNLOCK_NOTIFY: /* server should never get this */
	break;
      case OP_MEDIA_LOCK:
	break;
      case OP_MEDIA_UNLOCK:
	break;
      case OP_MEDIA_GET:
	break;
      case OP_MEDIA_GET_DATA: /* server should never get this */
	break;
      case OP_MEDIA_PUT:
	break;
      case OP_MEDIA_PUT_DATA:
	break;
      case OP_MEDIA_DELETE:
	break;
      case OP_THUMB_GET:
	break;
      case OP_THUMB_GET_DATA: /* server should never get this */
	break;
      default:
	break;
     }
   return 1;
}


static int
_server_init(void)
{
   _server_local = ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, "rage", 0, NULL);
   if (!_server_local) return 0;
   _server_remote = ecore_ipc_server_add(ECORE_IPC_REMOTE_SYSTEM, "", 9889, NULL);
   if (!_server_remote)
     {
	ecore_ipc_server_del(_server_local);
	_server_local = NULL;
	return 0;
     }
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, _server_cb_add, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, _server_cb_del, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, _server_cb_data, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_ADD, _client_cb_add, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DEL, _client_cb_del, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, _client_cb_data, NULL);
   return 1;
}

static void
_server_shutdown(void)
{
   if (_server_local)
     {
	ecore_ipc_server_del(_server_local);
	_server_local = NULL;
     }
   if (_server_remote)
     {
	ecore_ipc_server_del(_server_remote);
	_server_remote = NULL;
     }
}

int
main(int argc, char **argv)
{
   eet_init();
   ecore_init();
   ecore_file_init();
   ecore_con_init();
   ecore_ipc_init();
   
   ecore_app_args_set(argc, (const char **)argv);
   
   if (!_server_init())
     {
	printf("Raged: ERROR - cannot listen on sockets\n");
	exit(-1);
     }
   
   ecore_main_loop_begin();

   _server_shutdown();
   
   ecore_ipc_shutdown();
   ecore_con_shutdown();
   ecore_file_shutdown();
   ecore_shutdown();
   eet_shutdown();
   return 0;
}
