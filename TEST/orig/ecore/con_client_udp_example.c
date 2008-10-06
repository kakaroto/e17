/* Ecore_Con Usage Example
 */

#include <Ecore.h>
#include <Ecore_Con.h>

#include <stdio.h>
#include <unistd.h>
#include <string.h>

Ecore_Con_Server *svr;
Ecore_Con_Server *mcast;

#define SOCKET_NAME "con_example"
#define SOCKET_PORT 0

char *msg = "EFL UDP CLIENT";

typedef int (*Handler_Func) (void *data, int type, void *event);

int
server_data (void *data,
	     int ev_type,
	     Ecore_Con_Event_Server_Data *ev) {
  printf("Data received from the server!  Data was:\n");
  printf("%d, %s \n", ev->size, ev->data);

  ecore_con_server_send(svr, ev->data, ev->size);

  return 1;
}

int
main (int argc,
      char *argv[]) {
  ecore_con_init();

  // Try to conect to server.
  svr = ecore_con_server_connect(ECORE_CON_REMOTE_UDP, "127.0.0.1",6767, NULL);
  if (NULL == svr) {
    printf("*** This really shouldn't happen.  Bad port?  DNS lookup couldn't fork? \n");
    return 0;
  }
  printf("Sending Datagrams to localhost:6767 \n");

  // Try to conect to server.
  mcast = ecore_con_server_connect(ECORE_CON_REMOTE_UDP, "239.255.2.1", 1199, NULL);
  if (NULL == svr) {
    printf("*** This really shouldn't happen.  Bad port?  DNS lookup couldn't fork? \n");
    return 0;
  }
  printf("Sending Datagrams to 239.255.2.1:1199 \n");

  ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA,
			  (Handler_Func)server_data, NULL);

  ecore_con_server_send(svr, msg, strlen(msg));

  ecore_con_server_send(mcast, msg, strlen(msg));

  ecore_main_loop_begin();

  ecore_con_server_del(svr);

  ecore_con_shutdown();
  return 0;
}
