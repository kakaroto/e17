#include "spawner.h"

/* funcs */
Spawner_Display *spawner_display_new(void);
static void spawn_elogin(void);
static void spawn_x(void);
static int start_server_once(Spawner_Display *d);
void elogin_exit(int signum);
void x_exit(int signum);
double get_time(void);

/* globals */
Spawner_Display *d;
pid_t x_pid = 0;
pid_t elogin_pid = 0;
int redo = 0;

/* FIXME:" all "localhost:1" stuff in here and elgoin's code (ahtu.c) 
 * shodul be virtualised so it can be set depending on what display it is. 
 * for notn xnest stuff you really shoudl use :1 (or :0 NOt localhost)
 * as the its faster with connect response etc. as it doesnt have to hit the
 * tcp/ip stack - so localhost:whatever is ONLY a chak for xnest
 */
/* defines */
/* #define X_SERVER "/usr/X11R6/bin/X -terminate -ac -quiet" */
#define X_SERVER "/usr/X11R6/bin/Xnest"
#define X_SERVER_ARGS "-terminate -geometry 640x480 -ac -full"
#define ELOGIN "/home/chris/devel/elogin/src/elogin"
/* ================================================ */

/* main */
int main (int argc, char **argv)
{
   signal(SIGCHLD, elogin_exit);
   
   d = spawner_display_new();
   spawn_x();

   if (d->status == NOT_RUNNING)
   {
      free(d);
      printf("Could not start X server\n");
      exit(0);
   }
      
   spawn_elogin();
   
   printf("loop\n");
   for (;;) 
     {
	printf("just sit and wait...\n");
	pause();
	printf("a signal happened\n");
     }
   
   printf("out of here!\n");

   return 0;
}

/* display_new */
Spawner_Display *spawner_display_new(void)
{
   Spawner_Display *d;
   
   d = malloc(sizeof(Spawner_Display));
   d->display = NULL;
   d->name = NULL;
   d->xprog = malloc(strlen(X_SERVER) + 1);
   d->pid = 0;
   d->attempts = 5;
   d->status = NOT_RUNNING;

   strcpy (d->xprog, X_SERVER);

   return d;
}

/* spawn_elogin */
static void spawn_elogin(void)
{
   int pid;
   
   switch (pid = fork())
   {
      case 0:
	 execl(ELOGIN, ELOGIN, d->name, NULL);
	 break;
      case -1:
	 printf("Could not fork()\n");
	 exit(0);
	 break;
      default:
	 elogin_pid = pid;
	 printf("Elogin PID: %d\n", pid);
	 break;
   }
}

/* elogin_exit */
void elogin_exit(int signum)
{
   int status = 0;
   pid_t pid;

   printf("SIGCHLD\n");
   while((pid = waitpid(-1, &status, WNOHANG)) > 0)
   {
      printf("status = %i\n", status);
      if (pid == elogin_pid)
      {
	 printf("elogin exited.. close X connection!\n");
	 XSync(d->display, False);
	 XCloseDisplay(d->display);
      }
      else if (pid == x_pid)
      {
	 d->display = NULL;
	 printf("X exited... time to restart it all\n");
	 spawn_x();
	 spawn_elogin();
      }
   }
   printf("exit signal handler\n");
}

/* spawn_x */
static void spawn_x(void)
{
   int i = 0;

   d->status = NOT_RUNNING;
   while (i < d->attempts && d->status == NOT_RUNNING)
   {
      if (start_server_once(d) == RUNNING)
      {
	 d->status = RUNNING;
	 break;
      }
      i++;
   }
}


/* start_server_once */
static int start_server_once(Spawner_Display *d)
{
   double start_time = 0;
   int pid;
   int dspnum = 0;

   switch (pid = fork())
   {
      case 0:
	 execl("/bin/sh", "/bin/sh", "-c", d->xprog, d->xprog, NULL);
	 start_time = get_time();
	 break;
      case -1:
	 printf("Could not fork()\n");
	 exit(0);
	 break;
      default:
	 x_pid = pid;
	 printf("X PID: %d\n", pid);
	 break;
   }

   
   printf("looking for X on display %d\n", dspnum);
   while (!(d->display = XOpenDisplay(strcat(":", dspnum))))
   {
      double current_time = get_time();

      usleep(100000);
      if (start_time - current_time > 5.0 || dspnum > 2)
	 break;
      dspnum++;
   }

   printf("X display there...\n");
   d->name = strcat(":", dspnum);

   if (!d->display)
   {
      printf("Waited 5 seconds for X to connect but failed!\n");
      return NOT_RUNNING;
   }

   return RUNNING;
}

/* get_time */
double get_time(void)
{
   struct timeval timev;

   gettimeofday(&timev, NULL);
   return (double)timev.tv_sec + (((double)timev.tv_usec) / 1000000);
}
