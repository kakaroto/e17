#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <X11/Xlib.h>

/* defines */
#define NOT_RUNNING 0
#define RUNNING 1

/* structs */
typedef struct _Spawner_Display Spawner_Display;

struct _Spawner_Display
{
   Display *display;
   char *name;
   char *xprog;
   char *xargs;
   int pid;
   int attempts;
   int status;
};
