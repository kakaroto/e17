
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


#ifndef IPC_H
#define IPC_H 1

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Ecore.h>
#include <Ecore_Ipc.h>

#include "note.h"
#include "storage.h"
#include "debug.h"

#define IPC_NAME "enotes"
#define IPC_PORT 2323

typedef enum {			/* More to come. :-) */
	NOTE
} MessageType;

typedef struct {
	MessageType     cmd;
	void           *data;
} RecvMsg;

extern Ecore_Ipc_Server *mysvr;

/* High Level */
int             find_server(void);
void            setup_server(void);
void            send_to_server(char *msg);

/* Low Level */
int             ipc_svr_data_recv(void *data, int type, void *event);
RecvMsg        *parse_message(char *msg);
int             ipc_close_enotes(void *data);
char           *fix_newlines(char *b);

#endif
