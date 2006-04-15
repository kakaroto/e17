#include "entice.h"
#include "ipc.h"
#include <Ecore_Ipc.h>
#include <limits.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define IPC_TITLE "entice"

static unsigned int entice_ipc_client_request_pending = 0;

/**
 * ipc_server_add - when we connect to the ipc daemon
 * @data -
 * @type - 
 * @event - the Ecore_Ipc_Event_Server_Add that triggered us
 */
static int
ipc_server_add(void *data, int type, void *event)
{
   return 1;
}

/**
 * ipc_server_del - when we disconnect from the ipc daemon
 * @data -
 * @type - 
 * @event - the Ecore_Ipc_Event_Server_Del that triggered us
 */
static int
ipc_server_del(void *data, int type, void *event)
{
   return 1;
}

/**
 * ipc_server_data - 
 * @data -
 * @type - 
 * @event - the Ecore_Ipc_Event_Server_Data that triggered us
 */
static int
ipc_server_data(void *data, int type, void *event)
{
	Ecore_Ipc_Event_Server_Data *e = (Ecore_Ipc_Event_Server_Data *)event;
	/* printf("ipc_server_data: %p [%i] [%i] [%i] (%s)\n", 
	       e->server, e->major, e->minor, e->size, (char *)e->data); */
	if (!--entice_ipc_client_request_pending)
		ecore_main_loop_quit();
	return 1;
}

/**
 * ipc_client_add - 
 * @data -
 * @type - 
 * @event - the Ecore_Ipc_Event_Server_Add that triggered us
 */
static int
ipc_client_add(void *data, int type, void *event)
{
   return 1;
}

/**
 * ipc_client_del - 
 * @data -
 * @type - 
 * @event - the Ecore_Ipc_Event_Server_Del that triggered us
 */
static int
ipc_client_del(void *data, int type, void *event)
{
   return 1;
}

/**
 * ipc_client_data - when we get sent stuff by an entice client
 * @data -
 * @type - 
 * @event - the Ecore_Ipc_Event_Server_Data that triggered us
 */
#define IPC_CLIENT_ACK_MSG "ACK: Pants On!"
static int
ipc_client_data(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Data *e= (Ecore_Ipc_Event_Client_Data *)event;
   char buf[e->size];

   if (!e)
     return 1;

   ecore_ipc_client_send(e->client, 3, 4, 0, 0, 0, 
	 IPC_CLIENT_ACK_MSG, strlen(IPC_CLIENT_ACK_MSG) + 1);

   snprintf(buf, e->size, "%s", (char *) e->data);
   switch (e->major)
     {
      case IPC_FILE_APPEND:
	 entice_file_add(buf);
	 break;
      case IPC_FILE_DISPLAY:
	 entice_current_image_set(buf);
	 break;
      default:
     	 fprintf(stderr, "Unknown major code sent by client(%d)\n",
 	       e->major);
	 break;
     }
   /* printf("!! Client sent: [%i] [%i] (%i) \"%s\"\n", e->major,
      e->minor, e->size, buf); */
   return 1;
}

/**
 * entice_ipc_init - returns 0 if the app should continue, 1 if a process
 * already is running
 * @argc - the number of arguments passed to entice
 * @argv - the string arguments passed to entice
 * Attempts to connect to an existing entice ipc server.
 * If it successful, it sends the filenames to the current entice daemon
 * and exits. Else, it registers itself as the server and the app should
 * then start up.
 */
int 
entice_ipc_init(int argc, const char **argv) 
{ 
   int i; 
   char buf[PATH_MAX];
   Ecore_Ipc_Server *server = NULL;

   if (ecore_ipc_init() < 1)
      return 1;

   server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, IPC_TITLE, 0, NULL);
   /* If we can connect to a currently running entice process */
   if (server)
     {
	if (! (server = entice_ipc_client_init()) )
	  return 1;
	for (i = 1; i < argc; i++)
	  {
	     entice_file_add_job(server, argv[i], IPC_FILE_APPEND);
	     if (i == 1)
	       entice_file_add_job(server, argv[1], IPC_FILE_DISPLAY);
	     /* Flush every now an again */
	     if ((i & 0xf) == 0x1)
		entice_ipc_client_wait(server);
	  }
	entice_ipc_client_wait(server);
	entice_ipc_client_shutdown(server);
     	return 1;
     }

   /* Otherwise we create an entice server */
   snprintf(buf, PATH_MAX, "%s/.ecore/entice/0", getenv("HOME"));
   unlink(buf);
   server = ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, IPC_TITLE, 0, NULL);
   if (!server)
     {
	printf("creating new IPC server failed\n");
	return 1;
     }
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, ipc_client_add,
	 NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, ipc_client_del,
	 NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, ipc_client_data,
	 NULL);
   return 0;
}

/**
 * entice_ipc_shutdown - close our entice ipc handles
 */
void
entice_ipc_shutdown(void)
{
   ecore_ipc_shutdown();
}

/**
 * entice_ipc_client_init - initilaise client ipc
 * Must be called before entice_ipc_client_send
 * Returns handle to entice server
 */
Ecore_Ipc_Server *
entice_ipc_client_init(void)
{
   Ecore_Ipc_Server *server;

   if (!ecore_init())
     {
	fprintf(stderr, "unable to init ecore\n");
	return NULL;
     }
	
   if (!ecore_ipc_init())
     {
	fprintf(stderr, "unable to init ecore_con\n");
	goto out_ecore;
     }

   server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, IPC_TITLE, 0, NULL);
   if (!server)
     {
	fprintf(stderr, "unable to connect to server\n");
	goto out_ecore_ipc;
     }
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_ADD, ipc_server_add, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DEL, ipc_server_del, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, ipc_server_data, NULL);

   return server;

out_ecore_ipc:
   ecore_ipc_shutdown();
out_ecore:
   ecore_shutdown();
   return NULL;
}

/**
 * entice_ipc_client_shutdown - shutdown client
 * @server - handle to entice server
 */
void 
entice_ipc_client_shutdown(Ecore_Ipc_Server *server)
{
   ecore_ipc_server_del(server);
   ecore_ipc_shutdown();
   ecore_shutdown();
}

/**
 * entice_ipc_client_wait - wait for all pending messages to be procecced
 * Must be called after issuing one or more entice_ipc_client_send
 * @server - handle to entice server
 */
void 
entice_ipc_client_wait(Ecore_Ipc_Server *server)
{
   if (entice_ipc_client_request_pending)
     ecore_main_loop_begin();
}

/**
 * entice_ipc_client_request_image_load - send a message to a running
 * entice process telling it to load the file
 * @server - handle to entice server
 * @file - the absolute path to file
 * @major - major number of request
 */
void
entice_ipc_client_request_image_load(Ecore_Ipc_Server *server, 
                                      const char *file, int major)
{
   entice_ipc_client_request_pending++;
   ecore_ipc_server_send(server, major, 6, 0, 0, 0, (void *)file, 
	 strlen(file) + 1);
}
