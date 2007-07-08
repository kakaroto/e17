

#include "engy.h"

#define R(a) engy_utils_report((a),1)

int
handler_signal_exit(void *data, int ev_type, void *ev)
{
	Ecore_Event_Signal_Exit *e;

	e = ev;
	if (e->interrupt) printf("exit: interrupt\n");
	if (e->quit)      printf("exit: quit\n");
	if (e->terminate) printf("exit: terminate\n");

	ecore_main_loop_quit();
	return 1;
}


int main1(int argc, char *argv[])
{
	
	int res;
	Evas * evas;
	
//	res = engy_pro_args_parse(argc, argv);
//	R(res);

	if (!ecore_init()) return -1;
	ecore_app_args_set(argc, (const char **)argv);
	
	ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT,
			handler_signal_exit, NULL);

	engy_window_init();
	
	engy_cl_init();
	
	engy_edje_init();

	ecore_main_loop_begin();

	return 0;
}


