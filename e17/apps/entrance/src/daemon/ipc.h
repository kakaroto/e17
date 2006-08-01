#ifndef __ENTRANCED_IPC_H
#define __ENTRANCED_IPC_H

#include <sys/types.h>

#include <Ecore.h>
#include <Ecore_Ipc.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <sys/types.h>
#include <unistd.h>

#include "../ipc.h"
#include "Entranced.h"
#include "Entranced_Display.h"

int  entranced_ipc_init(pid_t pid);
void entranced_ipc_shutdown(void);
void entranced_ipc_display_set(Entranced_Display *d);

#endif

