#include <Ecore.h>
#include <Ecore_Config.h>
#include <Ecore_File.h>
#include "Entranced.h"
#include "Entranced_Display.h"
#include "auth.h"
#include "util.h"
#include "ipc.h"

/* Globals */
/* Entranced_Display *d; */
static Ecore_Event_Handler *_e_handler = NULL;
static Ecore_Event_Handler *_d_handler = NULL;
static Ecore_Event_Filter *_e_filter = NULL;

static struct sigaction _entrance_d_sa;

static unsigned char is_respawning = 0;
static unsigned char exev = 0;
static Ecore_Timer *respawn_timer = NULL;

static int _write_pidfile(pid_t);
static void _fork_and_exit(void);

static int _timer_cb_respawn_reset(void *);

static void *_filter_cb_start(void *);
static int _filter_cb_loop(void *, void *, int, void *);
static void _filter_cb_end(void *, void *);


static void _sigaction_cb_sigusr(int);

static int _event_cb_exited(void *, int, void *);
static int _event_cb_signal_exit(void *, int, void *);


void
usage(char *name)
{
   /* This should probably in a separate usage function, but bleh */
   printf("Entranced - Launcher for the Entrance Display Manager\n");
   printf("Usage: %s [OPTION] ...\n\n", name);
   printf("--------------------------------------------------------------------------\n");
   printf("  -c CONFIG          Specify config file for greeter\n");
   printf("  -d DISPLAY         Connect to an existing X server\n");
   printf("  -help              Display this help message\n");
   printf("  -verbose           Display extra debugging info\n");
   printf("  -nodaemon          Don't fork to background (useful for init scripts)\n");
   printf("==========================================================================\n\n");
   printf("Note: if you're launching Entrance from within an existing X session, don't\n");
   printf("try to use entranced or you may get unexpected results. Instead, launch\n");
   printf("entrance directly by typing \"entrance\".\n\n");
   exit(0);
}

/*
 * Main function
 */

int
main(int argc, char **argv)
{
   int c;
   int nodaemon = 0;            /* TODO: Config-ize this variable */
   Entranced_Display *d;
   char *str = NULL;
   struct option d_opt[] = {
      {"config", 1, 0, 'c'},
      {"display", 1, 0, 'd'},
      {"nodaemon", 0, 0, 'n'},
      {"help", 0, 0, 'h'},
      {"disable-xauth", 0, 0, 'a'},
      {"verbose", 0, 0, 'v'},
      {"debug", 0, 0, 'D'},
      {0, 0, 0, 0}
   };
   pid_t entranced_pid = getpid();

   /* Initialize Ecore */
   ecore_init();
   if (ecore_config_init("entrance") != ECORE_CONFIG_ERR_SUCC)
   {
      ecore_shutdown();
      return -1;
   }
   ecore_app_args_set(argc, (const char **) argv);

   openlog("entranced", LOG_NOWAIT, LOG_DAEMON);

   /* Set up a spawner context */
   d = edd_new();
   entranced_ipc_display_set(d);

   /* Parse command-line options */
   while (1)
   {
      c = getopt_long_only(argc, argv, "c:d:nhvD", d_opt, NULL);
      if (c == -1)
         break;
      switch (c)
      {
        case 'c':
           d->config = strdup(optarg);
           break;
        case 'd':
           d->name = strdup(optarg);
           break;
        case 'a':
           d->auth_en = 0;
           break;
        case 'n':
           nodaemon = 1;
           break;
        case 'h':
           usage(argv[0]);
           break;
        case 'v':
           entranced_debug_flag = 1;
           entranced_debug("Verbose output active.\n");
           break;
        case 'D':
           d->xprog = DEBUG_X_SERVER;
           break;
      }
   }

   if (!d->name)
      d->name = strdup(X_DISP);

   str = strchr(d->name, ':');

   if (str)
      d->dispnum = atoi(str + 1);

   entranced_debug("entranced: main: display number is %d\n", d->dispnum);

   entranced_pid = getpid();
   if (nodaemon)
   {
      if (_write_pidfile(entranced_pid))
      {
         syslog(LOG_CRIT, "%d is the pid, but I couldn't write to %s.",
                entranced_pid, PIDFILE);
         if (! entranced_debug_flag) {
             exit(1);
         }
      }
   }
   else
   {
      _fork_and_exit();
   }

   /* Check to make sure entrance binary is executable */
   if (access(ENTRANCE, X_OK))
   {
       entranced_debug("Execute permission denied for " ENTRANCE " binary.\n");
       syslog(LOG_CRIT, "Fatal Error:  Unable to launch entrance binary.");
       exit(1);
   }

   /* Init IPC */
   if (!entranced_ipc_init(getpid())) {
       entranced_debug("Unable to initialize IPC.  Aborting.\n");
       syslog(LOG_CRIT, "Fatal Error:  Unable to initialize IPC.");
       exit(1);
   }

   /* Daemonize */
   if (!nodaemon)
   {
      /* This causes socket communication issues, yet unidentified */
      /* 
         close(0); close(1); close(2); */
      freopen("/dev/null", "r", stdin);
      freopen("/dev/null", "w", stdout);
      freopen("/dev/null", "w", stderr);
   }

   /* Event filter */
   _e_filter = ecore_event_filter_add(_filter_cb_start, _filter_cb_loop, _filter_cb_end, NULL);

   /* Set up event handlers */
   _e_handler = ecore_event_handler_add(ECORE_EXE_EVENT_DEL, _event_cb_exited, d);
   _d_handler = ecore_event_handler_add(ECORE_EVENT_SIGNAL_EXIT, _event_cb_signal_exit, NULL);
/*    _sigusr1_handler = ecore_event_handler_add(ECORE_EVENT_SIGNAL_USER, _sigaction_cb_sigusr, NULL); */

   /* Manually add signal handler for SIGUSR1 */
   _entrance_d_sa.sa_handler = _sigaction_cb_sigusr;
   _entrance_d_sa.sa_flags = SA_RESTART;
   sigemptyset(&_entrance_d_sa.sa_mask);
   sigaction(SIGUSR1, &_entrance_d_sa, NULL);

   /* Launch X Server */
   syslog(LOG_INFO, "Starting X server.");
   edd_spawn_x(d);

   if (d->status == NOT_RUNNING)
   {
      free(d);
      syslog(LOG_CRIT, "Could not start X server.");
      fprintf(stderr, "Entrance could not start the X server. Please check your config.\n");
      exit(1);
   }

   /* Run Entrance */
   syslog(LOG_INFO, "Starting Entrance.");
   edd_spawn_entrance(d);

   /* Main program loop */
   entranced_debug("Entering main loop.\n");
   ecore_main_loop_begin();

   /* Shut down */
   entranced_debug("Exited main loop! Shutting down...\n");
   if (d->e_exe)
      ecore_exe_terminate(d->e_exe);
   kill(d->pid, SIGTERM);
   sleep(5);
   /* Die harder */
   if (d->e_exe)
      ecore_exe_kill(d->e_exe);
   kill(d->pid, SIGKILL);

   entranced_auth_user_remove(d);

   if (d->authfile)
      unlink(d->authfile);

   closelog();
   entranced_ipc_shutdown();
   ecore_config_shutdown();
   ecore_shutdown();
   exit(0);
}


/* privates */

/**
 * Write the entranced pid to the defined pidfile.
 * @param pid The spawner's process ID, which is the pid after the fork if there was one
 * @return 0 if the operation was successful, 1 otherwise.
 */
static int
_write_pidfile(pid_t pid)
{
   FILE *fp;
   int size, result = 1;
   char buf[PATH_MAX];

   ecore_file_mkpath(PIDDIR);
   size = snprintf(buf, PATH_MAX, "%d", pid);
   if ((fp = fopen(PIDFILE, "w+")))
   {
      fwrite(buf, sizeof(char), size, fp);
      fclose(fp);
      result = 0;
   }

   return result;
}


/**
 * Make entranced a daemon by fork-and-exit. This is the default behavior.
 */
static void
_fork_and_exit(void)
{
   pid_t entranced_pid;

   switch (entranced_pid = fork())
   {
     case 0:
        break;
     default:
        if (_write_pidfile(entranced_pid))
        {
           syslog(LOG_CRIT, "%d is the pid, but I couldn't write to %s.",
                  entranced_pid, PIDFILE);
           kill(entranced_pid, SIGKILL);
           exit(1);
        }
        exit(0);
   }
}


static int
_timer_cb_respawn_reset(void *data)
{
   entranced_debug("Respawn timer reset.\n");
   is_respawning = 0;
   ecore_timer_del(respawn_timer);
   respawn_timer = NULL;
   return 0;
}

/* Event Filters */
static void *
_filter_cb_start(void *data)
{
   return &exev;
}

static int
_filter_cb_loop(void *data, void *loop_data, int type, void *event)
{

   /* Filter out redundant exit events */
   if (type == ECORE_EXE_EVENT_DEL)
   {
      if (exev)
         return 0;
      else
         exev = 1;
   }

   return 1;
}

static void
_filter_cb_end(void *data, void *loop_data)
{
   exev = 0;
}

/* Event handlers */

/*int _sigaction_cb_sigusr(void *data, int type, void *event) {*/
static void
_sigaction_cb_sigusr(int sig)
{
/*    Ecore_Event_Signal_User *e = (Ecore_Event_Signal_User *) event; */

   entranced_debug("SIGUSR event triggered.\n");

   /* X sends SIGUSR1 to let us know it is ready */
/*    if (e->number == 1)*/
/*   x_ready = 1; this becomes below */
   edd_x_ready_set(1);
/*    return 1; */
}

static int
_event_cb_exited(void *data, int type, void *event)
{
   Ecore_Exe_Event_Del *e = (Ecore_Exe_Event_Del *) event;
   Entranced_Display *d = (Entranced_Display *) data;

   entranced_debug("Ecore_Exe_Event_Del triggered.\n");

   if (is_respawning)
   {
      entranced_debug("Event ignored.\n");
      return 1;
   }
   else
   {
      entranced_debug("Processing Event.\n");
   }

   is_respawning = 1;
   respawn_timer = ecore_timer_add(1.0, _timer_cb_respawn_reset, d);

   if (e->exe == d->e_exe || e->pid == ecore_exe_pid_get(d->e_exe))
   {
      /* Entrance GUI failed to initialize correctly */
      if (!d->client.connected)
      {
         syslog(LOG_CRIT, "Entrance GUI initialization failure. Aborting.");
         fprintf(stderr,
                 "Entrance has detected that the GUI is failing to launch properly.\n");
         fprintf(stderr, "Please check your installation. Aborting.\n\n");
         ecore_main_loop_quit();
      }

      /* Session exited or crashed */
      if (e->exited)
      {
         syslog(LOG_INFO, "The session has ended normally.");
         if (e->exit_code == EXITCODE)
         {
            ecore_main_loop_quit();
            return 0;
         }

      }
      else if (e->signalled)
         syslog(LOG_INFO, "The session was terminated with signal %d.",
                e->exit_signal);

      kill(d->pid, SIGHUP);
      sleep(3);
      if (waitpid(d->pid, NULL, WNOHANG) > 0)
      {
         syslog(LOG_INFO, "The X Server apparently died as well.");
         if (!edd_x_restart(d))
            exit(1);
      }

   }
   else if (e->pid == d->pid)
   {
      /* X terminated for some reason */
      if (e->exited)
         syslog(LOG_INFO, "The X Server terminated for some reason.");
      else if (e->signalled)
         syslog(LOG_INFO, "The X server was terminated with signal %d.",
                e->exit_signal);

      sleep(2);
      kill(d->pid, SIGKILL);
      if (!edd_x_restart(d))
         exit(1);

   }
   else
   {
      return 1;
   }

   d->client.connected = 0;
   entranced_auth_user_remove(d);
   edd_spawn_entrance(d);

   return 1;
}

static int
_event_cb_signal_exit(void *data, int type, void *event)
{
   entranced_debug("Ecore_Signal_Exit_Triggered\n");
   syslog(LOG_INFO, "Caught exit signal.");
   syslog(LOG_INFO, "Display and display manager are shutting down.");
   ecore_main_loop_quit();
   return 0;
}
