/*==========================================================================
 * Filename: entrance_ipc.c
 *========================================================================*/
#include "entrance_ipc.h"

static Ecore_Ipc_Server *server = NULL;
static Entrance_Session *_session = NULL;
static char *ipc_title = NULL;

/**
 * _entrance_ipc_server_add - when we connect to the ipc daemon
 * @data -
 * @type - 
 * @event -
 */
static int
_entrance_ipc_server_add(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Server_Add *e;

   e = (Ecore_Ipc_Event_Server_Add *) event;
   fprintf(stderr, "_entrance_ipc_server_add: Received event\n");
   return TRUE;
}

/**
 * _entrance_ipc_server_del - when we disconnect from the ipc daemon
 * @data -
 * @type - 
 * @event -
 */
static int
_entrance_ipc_server_del(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Server_Del *e;

   e = (Ecore_Ipc_Event_Server_Del *) event;
   fprintf(stderr, "_entrance_ipc_server_del: Received event\n");
   return TRUE;
}

/**
 * _entrance_ipc_server_del - when we disconnect from the ipc daemon
 * @data -
 * @type - 
 * @event -
 */
static int
_entrance_ipc_server_data(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Server_Data *e;

   if ((e = (Ecore_Ipc_Event_Server_Data *) event) && _session)
   {
      printf("_entrance_ipc_server_data: Received [%i] [%i] (%i) \"%s\"\n",
             e->major, e->minor, e->size, (char *) e->data);

      if (e->major == E_XAUTH_ACK)
      {
         printf("_entrance_ipc_server_data: Xauthority write success\n");
         ecore_main_loop_quit();
      }
      else if (e->major == E_XAUTH_NAK)
      {
         printf("_entrance_ipc_server_data: Xauthority write failure!\n");
         /* For now we'll attempt to start the user session regardless */
         ecore_main_loop_quit();
      }
      else
      {
         printf("_entrance_ipc_server_data: Invalid message received\n");
      }
   }
         
   return TRUE;
}

/**
 * _entrance_ipc_client_add - 
 * @data -
 * @type - 
 * @event -
 */
static int
_entrance_ipc_client_add(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Add *e;

   e = (Ecore_Ipc_Event_Client_Add *) event;
   fprintf(stderr, "_entrance_ipc_client_add: Received event\n");
   return TRUE;
}

/**
 * _entrance_ipc_client_del - 
 * @data -
 * @type - 
 * @event -
 */
static int
_entrance_ipc_client_del(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Del *e;

   e = (Ecore_Ipc_Event_Client_Del *) event;
   fprintf(stderr, "_entrance_ipc_client_del: Received event\n");
   return TRUE;
}

/**
 * _entrance_ipc_client_data - 
 * @data -
 * @type - 
 * @event -
 */
static int
_entrance_ipc_client_data(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Data *e;

   e = (Ecore_Ipc_Event_Client_Data *) event;

   printf("_entrance_ipc_client_data: Received [%i] [%i] (%i) \"%s\"\n", 
          e->major, e->minor, e->size, (char *) e->data);

   return TRUE;
}

int
entrance_ipc_init(pid_t server_pid)
{
   char buf[80];

   /* we definitely fail if we can't connect to ecore_ipc */
   if (ecore_ipc_init() < 1)
      return FALSE;

   memset(buf, 0, sizeof(buf));
   snprintf(buf, 80, "%s_%d", PACKAGE_STATE_DIR"/"IPC_TITLE, server_pid);
   if (ipc_title)
      free(ipc_title);
   ipc_title = strdup(buf);
   
   if ((server =
        ecore_ipc_server_connect(ECORE_IPC_LOCAL_SYSTEM, IPC_TITLE, 0, NULL)))
   {
      ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, 
                              _entrance_ipc_client_add, NULL);
      
      ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL,
                              _entrance_ipc_client_del, NULL);
      
      ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA,
                              _entrance_ipc_client_data, NULL);
      
      ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA,
                              _entrance_ipc_server_data, NULL);
      
      ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_ADD,
                              _entrance_ipc_server_add, NULL);
      
      ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DEL,
                              _entrance_ipc_server_del, NULL);
      
      
      fprintf(stderr, "entrance_ipc_init: Success\n");
   } 
   else
      fprintf(stderr, "entrance_ipc_init: connect to daemon failed.\n");
   
   return TRUE;
}

void
entrance_ipc_shutdown(void)
{
   if (server)
      ecore_ipc_server_del(server);
   server = NULL;
   ecore_ipc_shutdown();
   fprintf(stderr, "entrance_ipc_shutdown: Success\n");
}

int
entrance_ipc_connected_get(void)
{
   return server && ecore_ipc_server_connected_get(server);
}

void
entrance_ipc_session_set(Entrance_Session *session)
{
   _session = session;
}

void
entrance_ipc_request_xauth(char *homedir, uid_t uid, gid_t gid)
{
   ecore_ipc_server_send(server, E_XAUTH_REQ, E_UID, 0, 0, (int) uid, "", 0);
   ecore_ipc_server_send(server, E_XAUTH_REQ, E_GID, 0, 0, (int) gid, "", 0);
   ecore_ipc_server_send(server, E_XAUTH_REQ, E_HOMEDIR, 0, 0, 0, homedir,
                         strlen(homedir));
}

