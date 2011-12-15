
#include <Azy.h>
#include "Demo_HowAreYou.azy_server.h"


int main(int argc, char *argv[])
{
	eina_init();
	azy_init();


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
