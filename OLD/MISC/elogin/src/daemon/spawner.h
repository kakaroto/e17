#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/Xlib.h>
#include <limits.h>
#include <getopt.h>
#include "../config.h"

#define X_SERVER "/usr/X11R6/bin/X -terminate -ac -quiet"
/* #define X_SERVER "/usr/X11R6/bin/Xnest -terminate -geometry 640x480 -ac -full :1" */
#define X_DISP ":0"             /* only used if DISPLAY variable is NOT set */
#define ELOGIN PREFIX "/bin/elogin_wrapper"
#define PIDFILE "/var/run/elogind.pid"

/* display->status possible values */
#define NOT_RUNNING 0
#define LAUNCHING 1
#define RUNNING 2

/* structs */
typedef struct _Spawner_Display Spawner_Display;

struct _Spawner_Display
{
   Display *display;
   char *name;                  /* the name of the x display */
   char *xprog;                 /* the X execution string */
   int attempts;
   int status;
   struct
   {
      pid_t x, client;
   }
   pid;
};
