/*==========================================================================
 * Filename: entrance_ipc.c
 *========================================================================*/
#include<Ecore.h>
#include<Ecore_Ipc.h>
#include<limits.h>
#include<stdio.h>

#define IPC_TITLE "entrance"
static Ecore_Ipc_Server *server = NULL;

/**
 * ipc_server_add - when we connect to the ipc daemon
 * @data -
 * @type - 
 * @event -
 */
static int
ipc_server_add(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Server_Add *e;

   e = (Ecore_Ipc_Event_Server_Add *) event;
   fprintf(stderr, "Server add\n");
   return (1);
}

/**
 * ipc_server_del - when we disconnect from the ipc daemon
 * @data -
 * @type - 
 * @event -
 */
static int
ipc_server_del(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Server_Del *e;

   e = (Ecore_Ipc_Event_Server_Del *) event;
   fprintf(stderr, "Server delete\n");
   return (1);
}

/**
 * ipc_server_del - when we disconnect from the ipc daemon
 * @data -
 * @type - 
 * @event -
 */
static int
ipc_server_data(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Server_Data *e;

   e = (Ecore_Ipc_Event_Server_Data *) event;
   printf("!! Client sent: [%i] [%i] (%i) \"%s\"\n", e->major, e->minor,
          e->size, (char *) e->data);
   return (1);
}

/**
 * ipc_client_add - 
 * @data -
 * @type - 
 * @event -
 */
static int
ipc_client_add(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Add *e;

   e = (Ecore_Ipc_Event_Client_Add *) event;
   fprintf(stderr, "Client Connected!!!\n");
   return (1);
}

/**
 * ipc_client_del - 
 * @data -
 * @type - 
 * @event -
 */
static int
ipc_client_del(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Del *e;

   e = (Ecore_Ipc_Event_Client_Del *) event;
   fprintf(stderr, "Client Disconnected!!!\n");
   return (1);
}

/**
 * ipc_client_data - 
 * @data -
 * @type - 
 * @event -
 */
static int
ipc_client_data(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Data *e;

   e = (Ecore_Ipc_Event_Client_Data *) event;
   printf("!! Client sent: [%i] [%i] (%i) \"%s\"\n", e->major, e->minor,
          e->size, (char *) e->data);
   return (1);
}

/**
 * entrance_ipc_init - returns 1 if the app should continue, 0 if a process
 * already is running
 * @argc - the number of arguments passed to entrance
 * @argv - the string arguments passed to entrance
 * Basically it tries to connect to an already running entrance ipc server.
 * If it successfully connects, it sends the filenames to the current entrance
 * daemon and should exit.  If it does not successfully connect it registers
 * itself as the server and the app should then start up.
 */
int
entrance_ipc_init(int argc, const char **argv)
{
   /* we definitely fail if we can't connect to ecore_ipc */
   if (ecore_ipc_init() < 1)
      return (0);

   if ((server =
        ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, IPC_TITLE, 0, NULL)))
   {
      ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, ipc_client_add,
                              NULL);
      ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, ipc_client_del,
                              NULL);
      ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, ipc_client_data,
                              NULL);
      ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, ipc_server_data,
                              NULL);
      ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_ADD, ipc_server_add,
                              NULL);
      ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DEL, ipc_server_del,
                              NULL);
      fprintf(stderr, "Listener Started\n");
   }
   return (1);
}

void
entrance_ipc_shutdown(void)
{
   if (server)
      ecore_ipc_server_del(server);
   server = NULL;
   ecore_ipc_shutdown();
}
