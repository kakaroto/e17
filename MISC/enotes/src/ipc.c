
/**************************************************
 **               E  -  N O T E S                **
 **                                              **
 **  The contents of this file are released to   **
 **  the public under the General Public Licence **
 **  Version 2.                                  **
 **                                              **
 **  By  Thomas Fletcher (www.fletch.vze.com)    **
 **                                              **
 **************************************************/


#include "ipc.h"

Ecore_Ipc_Server *mysvr;
Ecore_Event_Handler *listenev;

/**
 * @return: Integer stating whether a server is running.
 *          -1 = wtf happened there?
 *           0 = No server found.
 *           1 = Theres already a running server.
 * @brief: Checks whether an enotes ipc server is running.
 */
int
find_server()
{
	Ecore_Ipc_Server *p;

	p = ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, IPC_NAME, IPC_PORT,
				     NULL);
	if (!p)
		return (0);
	else {
		ecore_ipc_server_del(p);
		return (1);
	}
	return (-1);
}

/**
 * @brief: Sets up the enotes server and gets it listening
 *         for IPC signals.
 */
void
setup_server()
{
	if (find_server() == 0)
		if (mysvr == NULL) {
			mysvr = ecore_ipc_server_add(ECORE_IPC_LOCAL_USER,
						     IPC_NAME, IPC_PORT, NULL);
			if (mysvr != NULL) {
				listenev = ecore_event_handler_add
					(ECORE_IPC_EVENT_CLIENT_DATA,
					 ipc_svr_data_recv, mysvr);
			}
		}
}

/**
 * @param msg: The message to be sent.
 * @brief: Sends the contents of msg over IPC
 *         to the running enotes server.
 */
void
send_to_server(char *msg)
{
	if ((mysvr =
	     ecore_ipc_server_connect(ECORE_IPC_LOCAL_USER, IPC_NAME, IPC_PORT,
				      NULL)) != NULL) {
		ipc_send_message_with_mysvr(msg);
		ecore_ipc_server_del(mysvr);
	}
}

/**
 * @param data: Not used, supplied by the ecore callback, can be
 *              set when the callback is set if required.
 * @param type: Not used, supplied by the ecore callback.
 * @param event: Event information including the message itself.
 * @brief: Callback for signal recieve.  Gets the message
 *         from "event", unwraps it with the parsing function
 *         so it can be used for whatever purpose is required.
 */
int
ipc_svr_data_recv(void *data, int type, void *event)
{
	Ecore_Ipc_Event_Client_Data *e;
	Ecore_Ipc_Server *server;

	e = (Ecore_Ipc_Event_Client_Data *) event;
	server = (Ecore_Ipc_Server *) data;
	if (server != ecore_ipc_client_server_get(e->client))
		return (1);
	if (e)
		handle_ipc_message((void *) e->data);
	return (1);
}

void
handle_ipc_message(void *data)
{
	RecvMsg        *p;
	NoteStor       *note;
	char           *content;

	p = parse_message((char *) data);

	if (p) {
		if (p->cmd == NOTE) {
			if (p->data) {
				note = (NoteStor *)
					get_notestor_from_value((char *)
								p->data);
				content = fix_newlines(note->content);
				new_note_with_values(note->x, note->y,
						     note->width, note->height,
						     note->shaded, content);
				free(content);
				free_note_stor(note);
			}
		} else if (p->cmd == CLOSE) {
			ecore_main_loop_quit();
		} else if (p->cmd == CONTROLCENTRECLOSE) {
			if (controlcentre != NULL) {
				ecore_evas_free(controlcentre->win);
				free(controlcentre);
				controlcentre = NULL;
			} else {
				new_note_with_values(0, 0, 325, 0, 0,
						     "An IPC command was recieved which\nwants to close the controlcentre.\n\nSince the control centre isn't currently\nopen, it wasn't possible to do so!");
			}
		} else if (p->cmd == CONTROLCENTREOPEN) {
			if (controlcentre == NULL) {
				setup_cc();
			} else {
				new_note_with_values(0, 0, 325, 0, 0,
						     "An IPC command was recieved which\nwants to open the control centre, but the\ncontrol centre is already open!");
			}
		} else if (p->cmd == DEFNOTE) {
			new_note();
		}
	}
}

/**
 * @param msg: The message to be sent to the connected host.
 * @brief:     Send a char message to the host.
 */
void
ipc_send_message_with_mysvr(char *msg)
{
	if (msg != NULL && mysvr != NULL)
		ecore_ipc_server_send(mysvr, 0, 0, 0, 0, 0, msg,
				      strlen(msg) + 1);
}


/**
 * @param msg: Message to parse.
 * @return: Returns the RecvMsg variable containing the individual
 *          options and information.
 * @brief: Unwraps the msg into individual values.  Into two pieces really,
 *         the command ("NOTE", "CLOSE" or whatever) and the information
 *         supplied.
 */
RecvMsg        *
parse_message(char *msg)
{
	RecvMsg        *p = malloc(sizeof(RecvMsg));
	char           *tst;
	char           *ts;
	char           *one;
	char           *two;
	int             noinfo = 0;

	tst = strdup(msg);
	ts = tst;
	if (strsep(&tst, DEF_VALUE_SEPERATION) != NULL) {
		if (strsep(&tst, DEF_VALUE_SEPERATION) == NULL) {
			free(ts);
			noinfo = 1;
		}
	} else {
		free(ts);
		free(p);
		error_msg
			("An incorrect IPC signal was recieved and flushed (no command)");
		return (NULL);
	}

	one = strdup(strsep(&msg, DEF_VALUE_SEPERATION));
	if (noinfo == 0) {
		two = strdup(msg);
		p->data = (void *) two;
	} else {
		two = NULL;
		p->data = NULL;
	}

	/* Set the command */
	if (!strcmp(one, "NOTE")) {
		p->cmd = NOTE;
	} else if (!strcmp(one, "DEFNOTE")) {
		p->cmd = DEFNOTE;
	} else if (!strcmp(one, "CLOSE")) {
		p->cmd = CLOSE;
	} else if (!strcmp(one, "CONTROLCENTREOPEN")) {
		p->cmd = CONTROLCENTREOPEN;
	} else if (!strcmp(one, "CONTROLCENTRECLOSE")) {
		p->cmd = CONTROLCENTRECLOSE;
	} else {
		free(one);
		free(p);
		error_msg
			("An incorrect IPC signal was recieved and flushed (incorrect command)");
		return (NULL);
	}

	free(one);
	return (p);
}

/**
 * @param data: Not used, but can be set during the setting of the callback.
 * @return: Returns 0 to end the timer.  Its a timer because if theres an event
 *          when the main loop is ended there can be trouble.
 * @brief: Closes enotes.
 */
int
ipc_close_enotes(void *data)
{
	ecore_main_loop_quit();
	return (0);
}

/**
 * @param b: The original string to "fix".
 * @return: The "fixed" string.
 * @brief: This function takes b, and replaces all of the "\n" substrings
 *         (not newline, a "\" and then an "n") with a newline character (\n).
 *         The compiler never sees the "\n" when supplied via IPC so its never
 *         converted into a newline, so we do it ourselves.
 */
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
