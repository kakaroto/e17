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
        {
           fprintf(stderr, "%d is the pid, but I couldn't write to %s\n",
                   elogind_pid, PIDFILE);
           kill(elogind_pid, SIGKILL);
           exit(1);
        }

        exit(0);
   }
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
   int nodaemon = 0;
   struct option d_opt[] = {
      {"nodaemon", 0, 0, 1},
      {"help", 0, 0, 2},
      {0, 0, 0, 0}
   };
   pid_t elogind_pid = getpid();

   putenv("DISPLAY");
   /* get command line arguments */
   while (1)
   {
      c = getopt_long_only(argc, argv, "d:", d_opt, NULL);
      if (c == -1)
         break;
      switch (c)
      {
        case 'd':              /* display */
           setenv("DISPLAY", optarg, 1);
           break;
        case 1:                /* nodaemon */
           nodaemon = 1;
           break;
        case 2:
           printf("Elogind - Launcher for the Elogin Display Manager\n");
           printf("Usage: %s [OPTION] ...\n\n", argv[0]);
           printf
              ("--------------------------------------------------------------------------\n");
           printf("  -d DISPLAY         Connect to an existing X server\n");
           printf("  -help              Display this help message\n");
           printf
              ("  -nodaemon          Don't fork to background (useful for init scripts)\n");
           printf
              ("==========================================================================\n\n");
           printf
              ("Note: if you're launching Elogin from within an existing X session, don't\n");
           printf
              ("try to use elogind or you may get unexpected results. Instead, launch\n");
           printf("elogin directly by typing \"elogin\".\n\n");
           exit(0);

        default:
           fprintf(stderr, "Warning: Unknown command line option\n");
      }
   }

   if (!getenv("DISPLAY"))
      setenv("DISPLAY", X_DISP, 1);

   if (nodaemon)
   {
      if (write_elogind_pidfile(elogind_pid))
      {
         fprintf(stderr, "%d is the pid, but I couldn't write to %s\n",
                 elogind_pid, PIDFILE);
         exit(1);
      }
   }
   else
   {
      fork_and_exit();
   }

   /* Check to make sure elogin binary is executable */
   if (access(ELOGIN, X_OK))
   {
      fprintf(stderr,
              "Elogin: Fatal Error: Cannot execute elogin binary. Aborting.\n");
      exit(1);
   }

   close(0);
   close(1);
   close(2);

   /* register child signal handler */
   signal(SIGCHLD, elogin_exit);
   signal(SIGHUP, elogin_exit);
   signal(SIGTERM, elogin_exit);

   /* setup a spawner context */
   d = spawner_display_new();

   /* run X */
   spawn_x();

   if (d->status == NOT_RUNNING)
   {
      free(d);
      fprintf(stderr, "Elogin: Could not start X server\n");
      exit(1);
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

   if (signum == SIGTERM)
   {
      kill(d->pid.client, SIGTERM);
      sleep(2);
      kill(d->pid.x, SIGTERM);
      exit(0);
   }

   while ((pid = waitpid(-1, &status, 0)) > 0)
   {
      if (pid == d->pid.client)
      {
         printf("INFO: Elogin process died.\n");
         if (d->display)
         {
            /* Die Hard Like Bruce Willis */
            kill(d->pid.x, SIGTERM);
            sleep(1);
            d->display = NULL;

            spawn_x();
            spawn_elogin();
         }
      }
      else if (pid == d->pid.x)
      {
         printf("INFO: X Server died.\n");
         if (d->display)
         {
            /* URM...don't try to XSync on a non-existent X process. SIGPIPE
               here */
            d->display = NULL;
            /* Die Hard 2 */
            kill(d->pid.x, SIGTERM);
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
