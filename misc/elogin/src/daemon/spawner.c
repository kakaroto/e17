#include "spawner.h"

/* funcs */
Spawner_Display *spawner_display_new(void);
static void spawn_elogin(void);
static void spawn_x(void);
static int start_server_once(Spawner_Display * d);
void elogin_exit(int signum);
void x_exit(int signum);
double get_time(void);

/* globals */
Spawner_Display *d;
pid_t x_pid = 0;
pid_t elogin_pid = 0;
int starting_x = 0;

/* FIXME:" all "localhost:1" stuff in here and elgoin's code (ahtu.c) 
 * shodul be virtualised so it can be set depending on what display it is. 
 * for notn xnest stuff you really shoudl use :1 (or :0 NOt localhost)
 * as the its faster with connect response etc. as it doesnt have to hit the
 * tcp/ip stack - so localhost:whatever is ONLY a chak for xnest
 */
/* defines */
#define X_SERVER "/usr/X11R6/bin/X -terminate -ac -quiet"
/* #define X_SERVER "/usr/X11R6/bin/Xnest -terminate -geometry 640x480 -ac -full :1" */
#define X_DISP "localhost:0"
#define ELOGIN "/usr/local/bin/elogin"
/* ================================================ */

/* main */
int
main(int argc, char **argv)
{
   /* register child signal handler */
   signal(SIGCHLD, elogin_exit);

   /* setup a spawner context */
   d = spawner_display_new();

   /* run X */
   spawn_x();

   if (d->status == NOT_RUNNING)
   {
      free(d);
      fprintf(stderr, "Elogin: Could not start X server\n");
      exit(0);
   }

   /* run elogin */
   spawn_elogin();

   for (;;)
      pause();

   return 0;
}

/* display_new */
Spawner_Display *
spawner_display_new(void)
{
   Spawner_Display *d;

   d = malloc(sizeof(Spawner_Display));
   d->display = NULL;
   d->name = NULL;
   d->xprog = strdup(X_SERVER);
   d->pid = 0;
   d->attempts = 5;
   d->status = NOT_RUNNING;
   return d;
}

/* spawn_elogin */
static void
spawn_elogin(void)
{
   int pid;

   switch (pid = fork())
   {
     case 0:
        if(execl(ELOGIN, ELOGIN, d->name) < 0)
	    exit(0); 
        break;
     case -1:
        fprintf(stderr, "Elogin: Could not fork() to spawn elogin process\n");
        exit(0);
        break;
     default:
        elogin_pid = pid;
        break;
   }
}

/* elogin_exit */
void
elogin_exit(int signum)
{
   int status = 0;
   pid_t pid;

   while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
   {
      if (pid == elogin_pid)
      {
         if (d->display)
         {
            XSync(d->display, False);
            XCloseDisplay(d->display);
            d->display = NULL;
            kill(x_pid, SIGTERM);
         }
      }
      else if (pid == x_pid)
      {
         if (d->display)
         {
            XSync(d->display, False);
            XCloseDisplay(d->display);
            d->display = NULL;
         }
         if (d->status == LAUNCHING)
         {
            d->status = NOT_RUNNING;
            fprintf(stderr,
                    "Elogin: X died mysteriously whilst launching.\n"
                    "        Waiting 10 seconds before trying again.\n");
            sleep(10);
         }
         d->status = NOT_RUNNING;

         spawn_x();
         spawn_elogin();
      }
   }
}

/* spawn_x */
static void
spawn_x(void)
{
   int i = 0;

   d->status = NOT_RUNNING;
   while ((i < d->attempts) && (d->status != RUNNING))
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
static int
start_server_once(Spawner_Display * d)
{
   double start_time = 0;
   int pid;
   int dspnum = 0;

   d->status = LAUNCHING;
   switch (pid = fork())
   {
     case 0:
        execl("/bin/sh", "/bin/sh", "-c", d->xprog, d->xprog, NULL);
        start_time = get_time();
        break;
     case -1:
        fprintf(stderr, "Elogin: Could not fork() to spawn X process\n");
        perror("Elogin");
        exit(0);
        break;
     default:
        x_pid = pid;
        break;
   }

   d->name = strdup(X_DISP);
   while (!(d->display = XOpenDisplay(d->name)))
   {
      double current_time;

      current_time = get_time();
      usleep(100000);
      if (((start_time - current_time) > 5.0) || (dspnum > 2))
         break;
   }

   if (!d->display)
      return NOT_RUNNING;

   return RUNNING;
}

/* get_time */
double
get_time(void)
{
   struct timeval timev;

   gettimeofday(&timev, NULL);
   return (double) timev.tv_sec + (((double) timev.tv_usec) / 1000000);
}
