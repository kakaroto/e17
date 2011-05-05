#include "server.h"

int main(int argc, char **argv)
{
   eina_init();
   eet_init();
   ecore_init();
   ecore_con_init();

   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_ADD, (Ecore_Event_Handler_Cb)server_new, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DEL, (Ecore_Event_Handler_Cb)server_lost, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_CLIENT_DATA, (Ecore_Event_Handler_Cb)server_gotdata, NULL);

   conn = ecore_con_server_add(ECORE_CON_REMOTE_TCP, "0.0.0.0", 9876, NULL);
   if (!conn)
     return 1;

   ecore_main_loop_begin();

   return 0;
}

Eina_Bool server_new(void *data, int type, Ecore_Con_Event_Client_Add *ev)
{
   if (ecore_con_client_server_get(ev->client) != conn)
     return ECORE_CALLBACK_PASS_ON;

   printf("Client with ip %s connected! instance ecore = %p\n",
          (char *)ecore_con_client_ip_get(ev->client),
          ev->client);

   return ECORE_CALLBACK_PASS_ON;
}

Eina_Bool server_lost(void *data, int type, Ecore_Con_Event_Client_Del *ev)
{
   return ECORE_CALLBACK_PASS_ON;
}

Eina_Bool server_gotdata(void *data, int type, Ecore_Con_Event_Client_Data *ev)
{
   if (ecore_con_client_server_get(ev->client) != conn)
     return ECORE_CALLBACK_PASS_ON;

   printf("Recv : %d\n", ev->size);

   return ECORE_CALLBACK_PASS_ON;
}
