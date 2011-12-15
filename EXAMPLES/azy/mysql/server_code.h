
#ifndef SERVER_CODE
#define SERVER_CODE

#include <Esskyuehl.h>
#include "Demo_Common_Azy.h"
#include "Demo_Message.azy_server.h"

typedef struct Test_Data 
{
	Demo_Message *msg;
	Azy_Server_Module *m;
	Esql *e;
} Test_Data;

void _connect(Esql *e, void *data);

#endif
