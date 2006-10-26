#include <Ecore_File.h>

#include "ipc.h"
#include "auth.h"
#include "util.h"

static Ecore_Ipc_Server *server = NULL;
static Ecore_List *clients = NULL;
static char *ipc_title = NULL;

static Entranced_Display *_display = NULL;

static int
_entranced_ipc_client_add(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Add *e;

   if (!(e = (Ecore_Ipc_Event_Client_Add *) event))
      return 1;
   if (_display)
      _display->client.connected = 1;

   entranced_debug("_entranced_ipc_client_add: Received event\n");
   return 1;
}

static int
_entranced_ipc_client_del(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Del *e;

   if (!(e = (Ecore_Ipc_Event_Client_Del *) event))
      return 1;

   entranced_debug("_entranced_ipc_client_del: Received event\n");
   return 1;
}

static int
_entranced_ipc_client_data(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Data *e;

   if (!(e = (Ecore_Ipc_Event_Client_Data *) event) || !_display)
      return 1;

   /* FIXME: This assumes there is only one client ... client will need to
      pass its pid or other verification data in the case of multiple clients 
      (XDMCP) */
   if (e->major == E_XAUTH_REQ)
   {
      switch (e->minor)
      {
        case E_UID:
           _display->client.uid = e->response;
           break;
        case E_GID:
           _display->client.gid = e->response;
           break;
        case E_HOMEDIR:
           _display->client.homedir = strdup(e->data);
           break;
        default:
           syslog(LOG_WARNING, "Received invalid request %x from client %d",
                  e->minor, _display->client.pid);
      }

      if ((_display->client.uid >= 0) && (_display->client.gid >= 0)
          && (_display->client.homedir))
      {
         if (!(_display->auth_en)
             || entranced_auth_user_add(_display,
                                        (const char *) _display->client.
                                        homedir))
         {
            ecore_ipc_client_send(e->client, E_XAUTH_ACK, 0, 0, 0, 0, NULL,
                                  0);
            entranced_debug
               ("_entranced_ipc_client_data: All tokens received; wrote cookie to user dir %s\n",
                _display->client.homedir);
         }
         else
         {
            ecore_ipc_client_send(e->client, E_XAUTH_NAK, 0, 0, 0, 0, NULL,
                                  0);
            entranced_debug
               ("_entranced_ipc_client_data: Failed to write cookie to %s\n",
                _display->client.homedir);
         }
      }
   }

   entranced_debug
      ("_entranced_ipc_client_data: Received [%i] [%i] [%i] [%i] \"%s\"\n",
       e->major, e->minor, e->size, e->response, (char *) e->data);

   return 1;
}

int
entranced_ipc_init(pid_t pid)
{
   char buf[PATH_MAX];

   if (!(ecore_ipc_init()))
      return 0;

   /* Make sure this path exists */
   ecore_file_mkpath(PACKAGE_STATE_DIR);

   memset(buf, 0, sizeof(buf));
   snprintf(buf, PATH_MAX, "%s_%d", PACKAGE_STATE_DIR "/" IPC_TITLE, pid);
   if (ipc_title)
      free(ipc_title);
   ipc_title = strdup(buf);

   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD,
                           _entranced_ipc_client_add, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL,
                           _entranced_ipc_client_del, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA,
                           _entranced_ipc_client_data, NULL);

   if (!
       (server =
        ecore_ipc_server_add(ECORE_IPC_LOCAL_SYSTEM, ipc_title, 0, NULL)))
   {
      entranced_debug("entranced_ipc_init: Failed to open socket %s.\n",
                      ipc_title);
      return 0;
   }

   if (clients)
      ecore_list_destroy(clients);
   clients = ecore_list_new();

   entranced_debug("entranced_ipc_init: Success\n");

   return 1;
}

void
entranced_ipc_shutdown(void)
{
   if (clients)
      ecore_list_destroy(clients);
   clients = NULL;

   ecore_ipc_shutdown();
   if (ipc_title)
      free(ipc_title);
   ipc_title = NULL;

   entranced_debug("entranced_ipc_shutdown: Success\n");
}

void
entranced_ipc_display_set(Entranced_Display * d)
{
   _display = d;
}
