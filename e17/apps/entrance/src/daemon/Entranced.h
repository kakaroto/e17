#ifndef __ENTRANCED_H
#define __ENTRANCED_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/Xlib.h>
#include <limits.h>
#include <getopt.h>
#include <syslog.h>

#include <Ecore.h>
#include <Ecore_Data.h>

#include "../config.h"

#define ENTRANCED_DEBUG 1

#define DEFAULT_X_SERVER X_BINARIES"/X -quiet -nolisten tcp" ENTRANCE_VT_ARG
#define DEBUG_X_SERVER X_BINARIES"/Xnest"
#define X_DISP ":0"             /* only used if DISPLAY variable is NOT set */
#define ENTRANCE PACKAGE_BIN_DIR"/entrance_wrapper"
#define PIDFILE PACKAGE_STATE_DIR"/entranced.pid"
#define PIDDIR PACKAGE_STATE_DIR

/* display->status possible values */
#define NOT_RUNNING 0
#define LAUNCHING 1
#define RUNNING 2
#define EXITCODE 42

/* structs */
typedef struct _Entranced_Client {
   pid_t    pid;
   uid_t    uid;
   gid_t    gid;
   char    *homedir;
   char    *authfile;
   unsigned char connected;
} Entranced_Client;


/* Functions */
void Entranced_X_Killall();

#endif

