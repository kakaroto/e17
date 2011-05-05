#include <Eina.h>
#include <Eet.h>
#include <Ecore.h>
#include <Ecore_Con.h>

#include "global.h"

Ecore_Con_Server *conn;

Eina_Bool server_new(void *data, int type, Ecore_Con_Event_Client_Add *ev);
Eina_Bool server_lost(void *data, int type, Ecore_Con_Event_Client_Del *ev);
Eina_Bool server_gotdata(void *data, int type, Ecore_Con_Event_Client_Data *ev);