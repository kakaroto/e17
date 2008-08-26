#include <Ecore.h>
#include <Ecore_Ipc.h>
#include <stdio.h>
#include <Etk.h>
#include <limits.h>

#define ENTROPY_QUEUER_IPC "entropy_queuer_ipc"
Ecore_Ipc_Server* server;
Etk_Widget* list;
Etk_Tree_Col* col;
Ecore_Exe* runner = NULL;

void entropy_queuer_run()
{
	if (!runner) {
		Etk_Tree_Row* row;

		row = etk_tree_first_row_get(ETK_TREE(list));
		if (row) {
			char* val1;
			char val2[PATH_MAX];
			etk_tree_row_fields_get(row, col, &val1, NULL);
			if (val1) {
				snprintf(val2, sizeof(val2), "evfscat \"%s\" | mpg123 -", val1);
				etk_tree_row_del(row);
				printf("Running %s\n", val2);
				runner = ecore_exe_run(val2, NULL);
			}
		}
	}
}

static Etk_Bool
_etk_window_deleted_cb (Etk_Object * object, void *data)
{
	exit(0);
}

int
exe_exit(void* data, int type, void* event)
{
	runner = NULL;
	entropy_queuer_run();
}

int
ipc_client_data(void *data , int type, void *event)
{

   Ecore_Ipc_Event_Client_Data *e = (Ecore_Ipc_Event_Client_Data *) event;

   if (e->size < PATH_MAX)
	   etk_tree_append(ETK_TREE(list), col, e->data, NULL);

   ecore_ipc_client_send(e->client, 0, 0, 0, 0,
                0, "REC", 4);

   entropy_queuer_run();
}

int
ipc_server_data(void *data , int type, void *event)
{
	ecore_shutdown();
	exit(0);
}

void entropy_queuer_spawn()
{
	const char* server_exe="/usr/local/bin/entropy_queuer";

	setsid();
	if (fork() == 0) {
		execl(server_exe, NULL);
	}
}

int main(int argc, char** argv)
{
	Etk_Widget* window;
	int attempts = 0;
	int i;
	
	ecore_init();
	ecore_ipc_init();

	if (argc == 1 || strcmp(argv[1], "--enqueue")) {
		etk_init(NULL,NULL);
		window = etk_window_new();
		etk_widget_size_request_set(window, 450, 300);
	
		list = etk_tree_new();
		col = etk_tree_col_new(ETK_TREE(list), "Song URI", 
			etk_tree_model_text_new(ETK_TREE(list)), 450);
		etk_tree_build(ETK_TREE(list));
		etk_container_add(ETK_CONTAINER(window), list);

		  etk_signal_connect ("delete_event", ETK_OBJECT (window),
		      ETK_CALLBACK (_etk_window_deleted_cb),NULL );
		
		etk_widget_show_all(window);

		/*Server setup*/
		if (server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, ENTROPY_QUEUER_IPC,0,NULL)) {
			printf("Entropy audio queuer already running - abort\n");
			return (1);
		} else {
			server = ecore_ipc_server_add(ECORE_IPC_LOCAL_USER, ENTROPY_QUEUER_IPC, 0, NULL);
			/*ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_ADD, ipc_client_add,
                                NULL);
			ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DEL, ipc_client_del,
                                NULL);*/
        		ecore_event_handler_add(ECORE_IPC_EVENT_CLIENT_DATA, ipc_client_data,
	                        NULL);
			ecore_event_handler_add(ECORE_EXE_EVENT_DEL, exe_exit, NULL);

		}

		etk_main();
	} else if (!strcmp(argv[1], "--enqueue")) {
		retry_connect:
		printf("Attempt %d\n", attempts);
		if (attempts < 5) {
			if (!(server = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, 
				ENTROPY_QUEUER_IPC,0,NULL))) {
					if (!attempts) entropy_queuer_spawn();
					attempts++;

					usleep(1000000 * attempts);
					goto retry_connect;
			} else {
				printf("Connected to server..\n");
	        		ecore_event_handler_add(ECORE_IPC_EVENT_SERVER_DATA, ipc_server_data,
	                        NULL);
			}
		} else {
			printf("Could not connect to server after 5 attempts\n");
			exit(0);
		}

		ecore_ipc_server_send(server, 0, 0, 0, 0,
                         0, argv[2], strlen(argv[2])+1);

		ecore_main_loop_begin();
	}
}
