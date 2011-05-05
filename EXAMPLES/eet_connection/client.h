#include <Eina.h>
#include <Eet.h>
#include <Ecore.h>
#include <Ecore_Con.h>

#include "global.h"
#include "serialisation.h"

Eina_List *lclothes;
Ecore_Con_Server *conn;
Eet_Connection *econn;

Eina_Bool client_connected(void *data, int type, Ecore_Con_Event_Server_Add *ev);
Eina_Bool client_disconnected(void *data, int type, Ecore_Con_Event_Server_Del *ev);
Eina_Bool client_data(void *data, int type, Ecore_Con_Event_Server_Data *ev);
static Eina_Bool _client_read_cb(const void *eet_data, size_t size, void *user_data);
static Eina_Bool _client_write_cb(const void *data, size_t size, void *user_data);
Eina_Bool client_sendcloth(clothing *cloth);
Eina_Bool client_tailor(void);
