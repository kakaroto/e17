#include "main.h"

static int _ipc_cb_client_add(void *data, int type, void *event);
static int _ipc_cb_client_del(void *data, int type, void *event);
static int _ipc_cb_client_data(void *data, int type, void *event);
static int _ipc_cb_timeout(void *data);
static void _ipc_cb_theme_exit_done(void *data);

static Ecore_Ipc_Server *_ipc_server = NULL;
static Ecore_Ipc_Client *_ipc_exit_client = NULL;
static Ecore_Timer *timeout = NULL;
static int timeout_seconds = 0;
int quitting;

void
ipc_init(void)
{
   ecore_ipc_init();
   if (getenv("EXQUISITE_IPC"))
     _ipc_server = ecore_ipc_server_add(ECORE_IPC_LOCAL_SYSTEM, getenv("EXQUISITE_IPC"), 0, NULL);
   else
     _ipc_server = ecore_ipc_server_add(ECORE_IPC_LOCAL_SYSTEM, "exquisite", 0, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, _ipc_cb_client_add, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, _ipc_cb_client_del, NULL);
   ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, _ipc_cb_client_data, NULL);
}

void
ipc_shutdown(void)
{
   ecore_ipc_server_del(_ipc_server);
   _ipc_server = NULL;
   ecore_ipc_shutdown();
}

static int
_ipc_cb_client_add(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Add *e;
   
   e = event;
   if (ecore_ipc_client_server_get(e->client) != _ipc_server) return 1;
   return 1;
}

static int
_ipc_cb_client_del(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Del *e;
   
   e = event;
   if (ecore_ipc_client_server_get(e->client) != _ipc_server) return 1;
   if (e->client == _ipc_exit_client)
     {
	theme_exit_abort();
	_ipc_exit_client = NULL;
     }
   ecore_ipc_client_del(e->client);
   return 1;
}

static int
_ipc_cb_client_data(void *data, int type, void *event)
{
   Ecore_Ipc_Event_Client_Data *e;
   
   if(quitting) return 1;

   e = event;
   if (ecore_ipc_client_server_get(e->client) != _ipc_server) return 1;
   
   if(timeout) 
    {
       ecore_timer_del(timeout);
       timeout = NULL;
       timeout = ecore_timer_add(timeout_seconds, _ipc_cb_timeout, NULL);
    }
   
   switch (e->major)
     {
      case EXIT_NOW:
	ecore_main_loop_quit();
	break;
      case EXIT:
	if (!_ipc_exit_client)
	  {
	     theme_exit(_ipc_cb_theme_exit_done, e->client);
	     _ipc_exit_client = e->client;
	  }
	break;
      case TITLE:
	if ((e->data) && ((char *)e->data)[e->size - 1] == 0)
	  theme_title_set(e->data);
	break;
      case MESSAGE:
	if ((e->data) && ((char *)e->data)[e->size - 1] == 0)
	  theme_message_set(e->data);
	break;
      case PROGRESS:
	theme_progress_set(e->ref / 100.0);
	break;
      case TICK:
	theme_tick();
	break;
      case PULSATE:
	theme_pulsate();
	break;
      case TEXT:
        if (!quiet && (e->data) && ((char *)e->data)[e->size -1] == 0)
          theme_text_add(e->data);
        break;
      case TEXT_URGENT:
        if ((e->data) && ((char *)e->data)[e->size -1] == 0)
          theme_text_add(e->data);
        break;
      case STATUS:
        if ((e->data) && ((char *)e->data)[e->size -1] == 0)
          theme_status_set(e->data, -1);
        break;
      case SUCCESS:
        if ((e->data) && ((char *)e->data)[e->size -1] == 0)
          theme_status_set(e->data, 1);
        break;
      case FAILURE:
        if ((e->data) && ((char *)e->data)[e->size -1] == 0)
          theme_status_set(e->data, 0);
        break;
      case CLEAR:
        theme_text_clear();
        break;
      case TIMEOUT:
        timeout_seconds = e->ref;
        if(timeout) 
          {
             ecore_timer_del(timeout);
             timeout = NULL;
          }
          timeout = ecore_timer_add(timeout_seconds, _ipc_cb_timeout, NULL);
        break;
      default:
	break;
     }
   return 1;
}

static int
_ipc_cb_timeout(void *data)
{
   theme_exit(_ipc_cb_theme_exit_done, data);
   return 0;
}

static void
_ipc_cb_theme_exit_done(void *data)
{
   if(data)
     {
        ecore_ipc_client_send(data, 0, 0, 0, 0, 0, NULL, 0);
        ecore_ipc_client_flush(data);
     }
   ecore_main_loop_quit();
   _ipc_exit_client = NULL;
}
