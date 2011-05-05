#include <time.h>
#include <stdlib.h>

#include "client.h"

int main(int argc, char **argv)
{
   eina_init();
   eet_init();
   ecore_init();
   ecore_con_init();

   lclothes = NULL;
   serialisation_init();

   ecore_event_handler_add(ECORE_CON_EVENT_SERVER_ADD, (Ecore_Event_Handler_Cb)client_connected, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DEL, (Ecore_Event_Handler_Cb)client_disconnected, NULL);
   ecore_event_handler_add(ECORE_CON_EVENT_SERVER_DATA, (Ecore_Event_Handler_Cb)client_data, NULL);

   conn = ecore_con_server_connect(ECORE_CON_REMOTE_TCP, "127.0.0.1", 9876, NULL);
   econn = eet_connection_new(_client_read_cb, _client_write_cb, NULL);

   client_tailor();
   ecore_main_loop_begin();

   return 0;
}

static Eina_Bool
_client_timed_send(void *data)
{
   clothing *found_cloth = data;

   client_sendcloth(found_cloth);
   free(found_cloth);

   return ECORE_CALLBACK_CANCEL;
}

/*
 * NEEDED FOR SOCKET MANAGEMENT
 */
Eina_Bool client_connected(void *data, int type, Ecore_Con_Event_Server_Add *ev)
{
   clothing *found_cloth;

   if (ev->server != conn)
     return ECORE_CALLBACK_PASS_ON;

   srandom(time(NULL));

   // Once we are connected to the server, we must serialise items using
   // Eet_connection to send them on the network
   EINA_LIST_FREE(lclothes, found_cloth)
     {
        ecore_timer_add((double) random() * 5.0 / RAND_MAX, _client_timed_send, found_cloth);
     }

   return ECORE_CALLBACK_PASS_ON;
}

Eina_Bool client_disconnected(void *data, int type, Ecore_Con_Event_Server_Del *ev)
{
   return ECORE_CALLBACK_PASS_ON;
}

Eina_Bool client_data(void *data, int type, Ecore_Con_Event_Server_Data *ev)
{
   return ECORE_CALLBACK_PASS_ON;
}

static Eina_Bool _client_read_cb(const void *eet_data, size_t size, void *user_data)
{
   /* FIXME: Handle eet packet here. */
   return EINA_FALSE;
}

static Eina_Bool _client_write_cb(const void *data, size_t size, void *user_data)
{
   printf("Data to send of size %d\n", size);

   if (ecore_con_server_send(conn, data, size) != (int) size)
     return EINA_FALSE;

   return EINA_TRUE;
}

struct {
   const char *name;
   Color color;
} match_color[] = {
  { "White", COLOR_WHITE },
  { "Black", COLOR_BLACK },
  { "Pink", COLOR_PINK },
  { "Brown", COLOR_BROWN },
  { "Blue", COLOR_BLUE },
  { NULL, 0 }
};

/*
 * NEEDED FOR SERIALISATION OF DATA
 */
Eina_Bool client_sendcloth(clothing *cloth)
{
   int i;

   for (i = 0; match_type[i].name != NULL; i++)
     if (match_type[i].type == cloth->type)
       {
          printf("Sending a %s [%p]\n", match_type[i].name, &cloth->type);
          break;
       }

   if (cloth->type == CLOTHING_TYPE_PULLOVER)
     {
        printf("\tSize : %s\n", cloth->data.spullover.size);
        for (i = 0; match_color[i].name != NULL; i++)
          if (match_color[i].color == cloth->data.spullover.color)
            {
               printf("\tColor : %s\n", match_color[i].name);
               break;
            }

        if (match_color[i].name == NULL)
          printf("\tColor : Unknown\n");
     }

   eet_connection_send(econn, unified, cloth, NULL);
   return ECORE_CALLBACK_PASS_ON;
}


/*
 * NEEDED FOR GENERATION OF DATA
 */
Eina_Bool client_tailor(void)
{
   clothing *tshirt_girl,
     *tshirt_boy,
     *pullover,
     *jean,
     *underwear_boy,
     *underwear_girl;

   tshirt_girl = calloc(1, sizeof (clothing));
   tshirt_girl->type = CLOTHING_TYPE_TSHIRT;
   tshirt_girl->data.stshirt.size = eina_stringshare_add("S");
   tshirt_girl->data.stshirt.long_sleeve = EINA_TRUE;
   tshirt_girl->data.stshirt.color = COLOR_PINK;
   lclothes = eina_list_append(lclothes, tshirt_girl);
   printf("Making a female tshirt of size S, Pink color and with long sleeves [%p]\n", &tshirt_girl->type);

   tshirt_boy = calloc(1, sizeof (clothing));
   tshirt_boy->type = CLOTHING_TYPE_TSHIRT;
   tshirt_boy->data.stshirt.size = eina_stringshare_add("XL");
   tshirt_boy->data.stshirt.long_sleeve = EINA_FALSE;
   tshirt_boy->data.stshirt.color = COLOR_BROWN;
   lclothes = eina_list_append(lclothes, tshirt_boy);
   printf("Making a male tshirt of size XL, Brown color and without long sleeves [%p]\n", &tshirt_boy->type);

   pullover = calloc(1, sizeof (clothing));
   pullover->type = CLOTHING_TYPE_PULLOVER;
   pullover->data.spullover.size = eina_stringshare_add("XL");
   pullover->data.spullover.color = COLOR_BLACK;
   lclothes = eina_list_append(lclothes, pullover);
   printf("Making an unisex pullover of size XL, Black color [%p]\n", &pullover->type);

   jean = calloc(1, sizeof (clothing));
   jean->type = CLOTHING_TYPE_JEAN;
   jean->data.sjean.size = 44;
   jean->data.sjean.color = COLOR_BLUE;
   lclothes = eina_list_append(lclothes, jean);
   printf("Making a male jean of size 44, Blue color [%p]\n", &jean->type);

   underwear_boy = calloc(1, sizeof(clothing));
   underwear_boy->type = CLOTHING_TYPE_UNDERWEAR;
   underwear_boy->data.sunderwear.type = UNDERWEAR_BOXER;
   underwear_boy->data.sunderwear.size = 5;
   underwear_boy->data.sunderwear.color = COLOR_WHITE;
   lclothes = eina_list_append(lclothes, underwear_boy);
   printf("Making a male underwear of size 5, White color and of type Boxer [%p]\n", &underwear_boy->type);

   underwear_girl = calloc(1, sizeof(clothing));
   underwear_girl->type = CLOTHING_TYPE_UNDERWEAR;
   underwear_girl->data.sunderwear.type = UNDERWEAR_STRING;
   underwear_girl->data.sunderwear.size = 4;
   underwear_girl->data.sunderwear.color = COLOR_BLACK;
   lclothes = eina_list_append(lclothes, underwear_girl);
   printf("Making a girl underwear of size 4, Black color and of type String [%p]\n", &underwear_girl->type);

   return EINA_TRUE;
}

