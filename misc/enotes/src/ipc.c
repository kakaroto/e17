/**************************************************/
/**               E  -  N O T E S                **/
/**                                              **/
/**  The contents of this file are released to   **/
/**  the public under the General Public Licence **/
/**  Version 2.                                  **/
/**                                              **/
/**  By  Thomas Fletcher (www.fletch.vze.com)    **/
/**                                              **/
/**************************************************/


#include "ipc.h"

Ecore_Ipc_Server *mysvr;

int
find_server(void)
{
	Ecore_Ipc_Server *p;

	p = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, IPC_NAME, IPC_PORT,
				     NULL);
	if (p == NULL) {
		return (0);
	} else {
		ecore_ipc_server_del(p);
		return (1);
	}
	return (-1);
}

void
setup_server(void)
{
	if (find_server() == 0) {
		if (mysvr == NULL) {
			mysvr = ecore_ipc_server_add(ECORE_IPC_LOCAL_USER,
						     IPC_NAME, IPC_PORT, NULL);
			if (mysvr != NULL) {
				ecore_event_handler_add
					(ECORE_IPC_EVENT_CLIENT_DATA,
					 ipc_svr_data_recv, NULL);
			}
		}
	}
	return;
}

void
send_to_server(char *msg)
{
	Ecore_Ipc_Server *p;

	if ((p =
	     ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, IPC_NAME, IPC_PORT,
				      NULL)) != NULL) {
		ecore_ipc_server_send(p, 0, 0, 0, 0, 0, optarg,
				      strlen(optarg) + 1);
		ecore_ipc_server_del(p);
	} else {
		return;
	}
	return;
}

int
ipc_svr_data_recv(void *data, int type, void *event)
{
	Ecore_Ipc_Event_Client_Data *e;
	RecvMsg        *p;

	NoteStor       *note;
	Ecore_Timer    *close;

	char           *msg;
	char           *content;

	if ((e = (Ecore_Ipc_Event_Client_Data *) event)) {
		p = parse_message(e->data);
		if (p != NULL) {
			if (p->cmd == NOTE) {
				note = (NoteStor *)
					get_notestor_from_value((char *) p->
								data);
				content = fix_newlines(note->content);
				new_note_with_values(note->width, note->height,
						     note->title, content);
				free(content);
				free_note_stor(note);
			}
		}
	}
	return (1);
}


RecvMsg        *
parse_message(char *msg)
{
	RecvMsg        *p = malloc(sizeof(RecvMsg));
	char           *tst;
	char           *ts;
	char           *one;
	char           *two;

	tst = strdup(msg);
	ts = tst;
	if (strsep(&tst, DEF_VALUE_SEPERATION) != NULL) {
		if (strsep(&tst, DEF_VALUE_SEPERATION) == NULL) {
			free(ts);
			free(p);
			return (NULL);
		}
	} else {
		free(ts);
		free(p);
		return (NULL);
	}

	one = strdup(strsep(&msg, DEF_VALUE_SEPERATION));
	two = strdup(msg);
	p->data = (void *) two;

	if (!strcmp(one, "NOTE")) {
		p->cmd = NOTE;
	} else {
		free(one);
		free(p);
		return (NULL);
	}

	free(one);
	return (p);
}

int
ipc_close_enotes(void *data)
{
	ecore_main_loop_quit();
	return (0);
}

char           *
fix_newlines(char *b)
{
	char           *a = strdup(b);
	char           *p = a;

	while ((p = strstr(p, "\\n"))) {
		memmove(p, p + 1, strlen(p));
		*p = '\n';
	}
	return a;
}
