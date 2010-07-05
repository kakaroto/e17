#include "ephoto.h"

int 
main(int argc, char **argv)
{
	ethumb_client_init();
	elm_need_efreet();
	elm_init(argc, argv);

	ephoto_create_main_window();

	elm_run();

	elm_shutdown();
	efreet_mime_shutdown();
	ethumb_client_shutdown();

	return 0;
}

