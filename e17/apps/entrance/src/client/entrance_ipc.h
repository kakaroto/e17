#ifndef ENTRANCE_IPC_H
#define ENTRANCE_IPC_H

#include <sys/types.h>

#include <Ecore.h>
#include <Ecore_Ipc.h>
#include <stdio.h>
#include <limits.h>
#include <sys/types.h>

#include "../ipc.h"
#include "entrance_session.h"

void entrance_ipc_shutdown(void);
int  entrance_ipc_init(int argc, const char **argv);
int  entrance_ipc_connected_get(void);
void entrance_ipc_session_set(Entrance_Session *session);
void entrance_ipc_request_xauth(char *homedir, uid_t uid, gid_t gid);

#endif
