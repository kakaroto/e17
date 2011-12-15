
#include <Azy.h>
#include "Demo_Message.azy_server.h"
#include <Esskyuehl.h>


static Eina_Bool
error_(void *data, int type, Esql_Res *res)
{
   fprintf(stderr, "Error : %s\n", esql_res_error_get(res)); /**< print error condition */
   ecore_main_loop_quit();
   return ECORE_CALLBACK_RENEW;
}

int main(int argc, char *argv[])
{
	azy_init();

	//Init Esskyuehl.h
	esql_init();
	ecore_event_handler_add(ESQL_EVENT_ERROR, (Ecore_Event_Handler_Cb)error_, NULL);
	//


        //Define the list of module used by the server.
	Azy_Server_Module_Def *modules[] = {
			Demo_Message_module_def(),
			NULL
	};

	//launch the server
	azy_server_basic_run(3412, AZY_SERVER_LOCAL, NULL, modules);

	esql_shutdown();
	azy_shutdown();
	ecore_shutdown();
	eina_shutdown();

	return 0;
}
