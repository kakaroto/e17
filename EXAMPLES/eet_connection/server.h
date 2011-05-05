#include <Eina.h>
#include <Eet.h>
#include <Ecore.h>
#include <Ecore_Con.h>

#include "global.h"
#include "serialisation.h"

Ecore_Con_Server *conn;
Eet_Connection *econn;

Eina_Bool server_new(void *data, int type, Ecore_Con_Event_Client_Add *ev);
Eina_Bool server_lost(void *data, int type, Ecore_Con_Event_Client_Del *ev);
Eina_Bool server_gotdata(void *data, int type, Ecore_Con_Event_Client_Data *ev);

static Eina_Bool _server_read_cb(const void *eet_data, size_t size, void *user_data);
static Eina_Bool _server_write_cb(const void *data, size_t size, void *user_data);
