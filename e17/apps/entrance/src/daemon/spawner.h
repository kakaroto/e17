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
#include <Ecore_X.h>
#include <Ecore_Ipc.h>

#include "../config.h"

#define ENTRANCE_DEBUG 1

#define _DEBUG(x) if(ENTRANCE_DEBUG) printf(x); fflush(stdout);

#define X_SERVER "/usr/X11R6/bin/X -quiet"
/*#define X_SERVER "/usr/X11R6/bin/Xnest -terminate -geometry 640x480 -ac -full :1"*/
#define X_DISP ":0"             /* only used if DISPLAY variable is NOT set */
#define ENTRANCE PREFIX "/bin/entrance_wrapper"
#define PIDFILE "/var/run/entranced.pid"
#define EXITCODE 42

/* display->status possible values */
#define NOT_RUNNING 0
#define LAUNCHING 1
#define RUNNING 2

struct _Entranced_Spawner_Display
{
   Display *display;
   char *name;                  /* the name of the x display */
   char *xprog;                 /* the X execution string */
   char *config;                /* Config file for entrance binary */
   int attempts;
   int status;
   Ecore_Exe *e_exe, *x_exe;
   struct
   {
      pid_t x, client;
   }
   pid;
};

/* structs */
typedef struct _Entranced_Spawner_Display Entranced_Spawner_Display;

/* Functions */
int Entranced_Write_Pidfile (pid_t pid);
void Entranced_Fork_And_Exit(void);
Entranced_Spawner_Display *Entranced_Spawner_Display_New(void);
void Entranced_Spawn_X(Entranced_Spawner_Display *d);
pid_t Entranced_Start_Server_Once(Entranced_Spawner_Display *d);
int Entranced_X_Restart(Entranced_Spawner_Display *d);
void Entranced_X_Killall();
void Entranced_Spawn_Entrance(Entranced_Spawner_Display *d);
int Entranced_Respawn_Reset(void *data);
int Entranced_Exe_Exited(void *data, int type, void *event);
int Entranced_Signal_Exit(void *data, int type, void *event);

