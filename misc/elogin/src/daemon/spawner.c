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

/**
 * write_elogind_pidfile - write the elogind pid to the specified pidfile
 * @pid - the pid_t variable received from the first fork called
 */
int
write_elogind_pidfile(pid_t pid)
{
   FILE *fp;
   int size, result = 1;
   char buf[PATH_MAX];

   size = snprintf(buf, PATH_MAX, "%d", pid);
   if ((fp = fopen(PIDFILE, "w+")))
   {
      fwrite(buf, sizeof(char), size, fp);
      fclose(fp);
      result = 0;
   }
   return (result);
}

/**
 * fork_and_exit - initial call in elogind, fork and exit for ease of use
 * To make this useable from startup scripts it's currently setup like this.
 * If this is incorrect or whatever please feel free to let me know.
 */
void
fork_and_exit(void)
{
   pid_t elogind_pid;

   switch (elogind_pid = fork())
   {
     case 0:
        break;
     default:
        if (write_elogind_pidfile(elogind_pid))
           fprintf(stderr, "%d is the pid, but I couldn't write to %s",
                   elogind_pid, PIDFILE);
        exit(0);
   }
   close(0);
   close(1);
   close(2);
}

/**
 * main - startup the elogind process
 * @argc - not used
 * @argv - not used
 * Elogind starts off by forking off a child process, writing the child's
 * pid to a pidfile, and returning.  The forked child begins a new X session
 * and then starts the elogin process.
 */
int
main(int argc, char **argv)
{
   int c;
    
   putenv("DISPLAY");
   /* get command line arguments */
   while (1)
   {
      c = getopt(argc, argv, "d:");
      if (c == -1)
         break;
      switch (c)
      {
        case 'd':              /* display */
	   setenv("DISPLAY", optarg, 1);
           break;
        default:
           exit(1);
      }
   }
    
   if(!getenv("DISPLAY"))
       setenv("DISPLAY", X_DISP, 1);

   fork_and_exit();
   /* register child signal handler */
   signal(SIGCHLD, elogin_exit);
   signal(SIGHUP, elogin_exit);

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
   memset(d, 0, sizeof(Spawner_Display));
   d->xprog = strdup(X_SERVER);
   d->attempts = 5;
   d->status = NOT_RUNNING;
   return (d);
}

/* spawn_elogin */
static void
spawn_elogin(void)
{
   int pid;

   switch (pid = fork())
   {
     case 0:
        if (execl(ELOGIN, ELOGIN, d->name, NULL) < 0)
           exit(0);
        break;
     case -1:
        fprintf(stderr, "Elogin: Could not fork() to spawn elogin process\n");
        exit(0);
        break;
     default:
        d->pid.client = pid;
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
      if (pid == d->pid.client)
      {
         if (d->display)
         {
            XSync(d->display, False);
            XCloseDisplay(d->display);
            d->display = NULL;
            kill(d->pid.x, SIGTERM);
         }
      }
      else if (pid == d->pid.x)
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
        d->pid.x = pid;
        break;
   }

   d->name = strdup(getenv("DISPLAY"));
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
