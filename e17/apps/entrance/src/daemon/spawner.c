#include "spawner.h"

/* funcs */
Spawner_Display *spawner_display_new(void);
static void spawn_entrance(void);
static void spawn_x(void);
static int start_server_once(Spawner_Display * d);
void entrance_exit(int signum);
void x_exit(int signum);
double get_time(void);

/* globals */
Spawner_Display *d;
sigset_t d_sig;

/**
 * write_entranced_pidfile - write the entranced pid to the specified pidfile
 * @pid - the pid_t variable received from the first fork called
 */
int
write_entranced_pidfile(pid_t pid)
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
 * fork_and_exit - initial call in entranced, fork and exit for ease of use
 * To make this useable from startup scripts it's currently setup like this.
 * If this is incorrect or whatever please feel free to let me know.
 */
void
fork_and_exit(void)
{
   pid_t entranced_pid;

   switch (entranced_pid = fork())
   {
     case 0:
        break;
     default:
        if (write_entranced_pidfile(entranced_pid))
        {
           fprintf(stderr, "%d is the pid, but I couldn't write to %s\n",
                   entranced_pid, PIDFILE);
           kill(entranced_pid, SIGKILL);
           exit(1);
        }

        exit(0);
   }
}

static int
x_error_handler_ignore(Display * d, XErrorEvent * e)
{
   /* do nothing */
   return 0;
}

/**
 * x_server_killall - Kill all X clients
 *
 * This function will attempt to reset the X server by killing
 * all client windows, prior to respawning the entrance client.
 */
static void
x_server_killall(void)
{
   int screens, i, j;

   if (!d || !(d->display))
      return;

   /* Don't want entranced barfing over a BadWindow error or sth */
   XSetErrorHandler(x_error_handler_ignore);

   XGrabServer(d->display);
   screens = ScreenCount(d->display);

   /* Traverse window tree starting from root, drag them * all before the
      firing squad */
   for (i = 0; i < screens; ++i)
   {
      Window root_r;
      Window parent_r;
      Window *children_r = NULL;
      int num_children = 0;
      Window root = RootWindow(d->display, i);

      while (XQueryTree
             (d->display, root, &root_r, &parent_r, &children_r,
              &num_children) && num_children > 0)
      {

         for (j = 0; j < num_children; ++j)
         {
            XKillClient(d->display, children_r[j]);
         }

         XFree(children_r);
      }
   }

   XUngrabServer(d->display);
   XSync(d->display, False);
}


/**
 * main - startup the entranced process
 * @argc - not used
 * @argv - not used
 * Entranced starts off by forking off a child process, writing the child's
 * pid to a pidfile, and returning.  The forked child begins a new X session
 * and then starts the entrance process.
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
   pid_t entranced_pid = getpid();

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
           printf("Entranced - Launcher for the Entrance Display Manager\n");
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
              ("Note: if you're launching Entrance from within an existing X session, don't\n");
           printf
              ("try to use entranced or you may get unexpected results. Instead, launch\n");
           printf("entrance directly by typing \"entrance\".\n\n");
           exit(0);

        default:
           fprintf(stderr, "Warning: Unknown command line option\n");
      }
   }

   if (!getenv("DISPLAY"))
      setenv("DISPLAY", X_DISP, 1);

   if (nodaemon)
   {
      if (write_entranced_pidfile(entranced_pid))
      {
         fprintf(stderr, "%d is the pid, but I couldn't write to %s\n",
                 entranced_pid, PIDFILE);
         exit(1);
      }
   }
   else
   {
      fork_and_exit();
   }

   /* Set up signals */
   sigemptyset(&d_sig);
   sigaddset(&d_sig, SIGTERM);
   sigaddset(&d_sig, SIGCHLD);
   sigaddset(&d_sig, SIGHUP);
   sigaddset(&d_sig, SIGUSR1);

   /* Check to make sure entrance binary is executable */
   if (access(ENTRANCE, X_OK))
   {
      fprintf(stderr,
              "Entranced: Fatal Error: Unable to launch entrance binary. Aborting.\n");
      exit(1);
   }

   if (!nodaemon)
   {
      close(0);
      close(1);
      close(2);
   }

   /* register child signal handler */
   signal(SIGCHLD, entrance_exit);
   signal(SIGUSR1, entrance_exit);
   signal(SIGTERM, entrance_exit);

   /* setup a spawner context */
   d = spawner_display_new();

   /* run X */
   printf("INIT: Starting X server.\n");
   spawn_x();

   if (d->status == NOT_RUNNING)
   {
      free(d);
      fprintf(stderr, "Entranced: Could not start X server\n");
      exit(1);
   }

   /* run entrance */
   printf("INIT: Starting Entrance.\n");
   spawn_entrance();

   for (;;)
   {
      pause();
   }

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

/* spawn_entrance */
static void
spawn_entrance(void)
{
   pid_t pid, ppid;

   sigprocmask(SIG_BLOCK, &d_sig, NULL);
   ppid = getpid();

   /* First fork */
   switch (pid = fork())
   {
     case 0:
        pid = fork();
        if (pid)
           exit(0);
        else
        {
           /* Declare independence from the colonial masters */
           if (setsid() == -1)
           {
              perror("setsid");
              kill(ppid, SIGTERM);
              exit(1);
           }

           /* Restore SIGCHLD default handling */
           signal(SIGCHLD, SIG_DFL);
           /* I will not die before my children */
           signal(SIGHUP, SIG_IGN);
           signal(SIGTERM, SIG_IGN);
           signal(SIGUSR1, SIG_IGN);

           sigprocmask(SIG_UNBLOCK, &d_sig, NULL);

           /* Then fork again. woohoo */
           if ((pid = fork()) == -1)
           {
              fprintf(stderr,
                      "Entranced: FATAL: Could not fork() entrance process\n");
              exit(1);
           }
           if (pid)
           {
              /* Wait for client session process to die, then destroy this
                 process group */
              pid_t chld;
              int status;

              while ((chld = waitpid(-1, &status, 0)) > 0)
              {
                 if (chld == pid
                     && (WIFEXITED(status) || WIFSIGNALED(status)))
                 {
                    /* Tell daemon that this session is done. */
                    kill(ppid, SIGUSR1);

                    /* Die hard */
                    kill(0, SIGKILL);
                    exit(0);
                 }
              }
           }
           else
           {
              /* Launch entrance client */
              if (execl
                  ("/bin/sh", "/bin/sh", "-c", ENTRANCE, ENTRANCE, d->name,
                   NULL) < 0)
                 exit(1);
           }
        }
        break;
     case -1:
        fprintf(stderr,
                "Entranced: FATAL: Could not fork() entrance process\n");
        exit(1);
        break;
     default:
        d->pid.client = pid;
        break;
   }
   sigprocmask(SIG_UNBLOCK, &d_sig, NULL);

}

/* entrance_exit */
void
entrance_exit(int signum)
{
   int status = 0;
   pid_t pid;

   /* Prevent signal collision */
   sigprocmask(SIG_BLOCK, &d_sig, NULL);

   /* Terminate X session */
   if (signum == SIGTERM)
   {
      printf("Received SIGTERM, closing session\n");
      kill(d->pid.x, SIGTERM);
      sleep(1);
      kill(d->pid.x, SIGKILL);
      exit(0);
   }

   /* A pause, in case X died as well...allow things * to settle down */
   usleep(500000);

   /* Try to determine if X has died */
   do
   {
      pid = waitpid(-1, &status, WNOHANG);
      if (pid == d->pid.x)
         break;
   }
   while (pid > 0);

   /* The session process has died */
   if (signum == SIGUSR1 && pid != d->pid.x)
   {
      printf("INFO: Entrance session has apparently ended.\n");
      x_server_killall();

      /* Attend to any waiting zombies */
      while (waitpid(-1, &status, WNOHANG) > 0);

      spawn_entrance();
      sigprocmask(SIG_UNBLOCK, &d_sig, NULL);
      return;
   }

   /* SIGCHLD received. This most likely means that X died. */
   if (pid == d->pid.x)
   {
      printf("INFO: X Server died.\n");

      /* Die Harder! */
      kill(d->pid.x, SIGTERM);
      d->display = NULL;

      /* Attend to any waiting zombies before proceeding */
      while (waitpid(-1, &status, WNOHANG) > 0);

      /* Check if X died while trying to launch. */
      if (d->status == LAUNCHING)
      {
         d->status = NOT_RUNNING;
         fprintf(stderr,
                 "Entranced: X died mysteriously whilst launching.\n"
                 "        Waiting 10 seconds before trying again.\n");
         sleep(10);
      }
      d->status = NOT_RUNNING;

      spawn_x();
      if (d->status == NOT_RUNNING)
      {
         free(d);
         fprintf(stderr, "Entranced: Could not start X server\n");
         exit(1);
      }

      printf("Started new X server, spawning entrance...\n");
      spawn_entrance();
   }
   sigprocmask(SIG_UNBLOCK, &d_sig, NULL);
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
        fprintf(stderr, "Entranced: Could not fork() to spawn X process\n");
        perror("Entranced");
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
