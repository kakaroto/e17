
#include <Azy.h>
#include "Demo_HowAreYou.azy_server.h"


int main(int argc, char *argv[])
{
	azy_init();

        //eina_log_domain_level_set("azy", EINA_LOG_LEVEL_DBG);

        //Define the list of module used by the server.
	Azy_Server_Module_Def *modules[] = {
			Demo_HowAreYou_module_def(),
			NULL
	};

	//launch the server
	azy_server_basic_run(3412, AZY_SERVER_LOCAL, NULL, modules);


	azy_shutdown();
	ecore_shutdown();
	eina_shutdown();

	return 0;
}
