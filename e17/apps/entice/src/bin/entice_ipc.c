#include "entice.h"
#include <Ecore_Ipc.h>
#include <limits.h>

#define IPC_TITLE "entice"

/**
 * ipc_server_add - when we connect to the ipc daemon
 * @data -
 * @type - 
 * @event - the Ecore_Ipc_Event_Server_Add that triggered us
 */ 
int
ipc_server_add (void *data, int type, void *event)
{
  Ecore_Ipc_Event_Server_Add *e;
  e = (Ecore_Ipc_Event_Server_Add *) event;
  // fprintf(stderr, "Server add\n");
  return (1);
}
/**
 * ipc_server_del - when we disconnect from the ipc daemon
 * @data -
 * @type - 
 * @event - the Ecore_Ipc_Event_Server_Del that triggered us
 */ 
int
ipc_server_del (void *data, int type, void *event)
{
  Ecore_Ipc_Event_Server_Del *e;
  e = (Ecore_Ipc_Event_Server_Del *) event;
  // fprintf(stderr, "Server delete\n");
  return (1);
}
/**
 * ipc_server_data - 
 * @data -
 * @type - 
 * @event - the Ecore_Ipc_Event_Server_Data that triggered us
 */ 
int
ipc_server_data (void *data, int type, void *event)
{
  Ecore_Ipc_Event_Server_Data *e;
  e = (Ecore_Ipc_Event_Server_Data *) event;
  printf ("!! Server sent: [%i] [%i] (%i) \"%s\"\n", e->major, e->minor,
	  e->size, e->data);
  return (1);
}

/**
 * ipc_client_add - 
 * @data -
 * @type - 
 * @event -
 */
int
ipc_client_add (void *data, int type, void *event)
{
  Ecore_Ipc_Event_Client_Add *e;
  e = (Ecore_Ipc_Event_Client_Add *) event;
  // fprintf(stderr, "Client Connected!!!\n");
  return (1);
}

/**
 * ipc_client_del - 
 * @data -
 * @type - 
 * @event -
 */
int
ipc_client_del (void *data, int type, void *event)
{
  Ecore_Ipc_Event_Client_Del *e;
  e = (Ecore_Ipc_Event_Client_Del *) event;
  // fprintf(stderr, "Client Disconnected!!!\n");
  return (1);
}

/**
 * ipc_client_data - when we get sent stuff by an entice client
 * @data -
 * @type - 
 * @event -
 */
int
ipc_client_data (void *data, int type, void *event)
{
  Ecore_Ipc_Event_Client_Data *e;
  e = (Ecore_Ipc_Event_Client_Data *) event;
  /*printf ("!! Client sent: [%i] [%i] (%i) \"%s\"\n", e->major, e->minor,
	  e->size, e->data);*/
  image_add_from_ipc(e->data);
  return (1);
}

/**
 * entice_ipc_init - returns 0 if the app should continue, 1 if a process
 * already is running
 * @argc - the number of arguments passed to entice
 * @argv - the string arguments passed to entice
 * Basically it tries to connect to an already running entice ipc server.
 * If it successfully connects, it sends the filenames to the current entice
 * daemon and should exit.  If it does not successfully connect it registers
 * itself as the server and the app should then start up.
 */
int
entice_ipc_init (int argc, const char **argv)
{
  int i;
  Ecore_Ipc_Server *server = NULL;

  /* we definitely fail if we can't connect to ecore_ipc */
  if(ecore_ipc_init () <  1) return(1);

  /* If we can connect to a currently running entice process */
  if((server=ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER,IPC_TITLE, 0,NULL)))
  {
      char buf[PATH_MAX];
      
      printf("found existing entice running... sending it the imagelist\n");
      ecore_event_handler_add (ECORE_IPC_EVENT_SERVER_ADD, ipc_server_add,
			       NULL);
      ecore_event_handler_add (ECORE_IPC_EVENT_SERVER_DEL, ipc_server_del,
			       NULL);
      ecore_event_handler_add (ECORE_IPC_EVENT_SERVER_DATA, ipc_server_data,
			       NULL);
      
      for (i = 1; i < argc; i++)
	{
	  snprintf (buf, PATH_MAX, "%s", argv[i]);
	  ecore_ipc_server_send(server, 5, 6, 0, 0, 0, buf, sizeof(buf));
	  memset(buf, 0, sizeof(buf));
	}
      ecore_ipc_server_del(server);
      return(1);
    }
  /* Otherwise we create it */
  else
    {
      // printf("creating new server\n");
      server = ecore_ipc_server_add (ECORE_IPC_LOCAL_USER, IPC_TITLE, 0, NULL);
      if (server == NULL) printf("creating new IPC server failed\n");
      ecore_event_handler_add (ECORE_IPC_EVENT_CLIENT_ADD, ipc_client_add,
			       NULL);
      ecore_event_handler_add (ECORE_IPC_EVENT_CLIENT_DEL, ipc_client_del,
			       NULL);
      ecore_event_handler_add (ECORE_IPC_EVENT_CLIENT_DATA, ipc_client_data,
			       NULL);
      // fprintf(stderr, "Listener Started\n");
    }
    return (0);
}

void
entice_ipc_shutdown(void)
{
  // printf("shutting down IPC\n");
    ecore_ipc_shutdown();
}
