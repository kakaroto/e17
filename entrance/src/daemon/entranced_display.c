#include <Ecore_Config.h>
#include "Entranced.h"
#include "Entranced_Display.h"
#include "auth.h"
#include "util.h"
#include <errno.h>
#include <string.h>


static unsigned char x_ready = 0;
static struct sigaction _entrance_x_sa;

static pid_t _start_server_once(Entranced_Display *);


/**
 * Create a new display context.
 * @return A pointer to an Entranced_Display handle for the new context
 */
Entranced_Display *
edd_new(void)
{
   Entranced_Display *d;

   d = calloc(1, sizeof(Entranced_Display));

   ecore_config_string_default("/entranced/xserver", DEFAULT_X_SERVER);
   ecore_config_int_default("/entranced/attempts", 5);

   ecore_config_file_load(PACKAGE_CFG_DIR "/entrance_config.cfg");

   d->xprog = ecore_config_string_get("/entranced/xserver");
   d->attempts = ecore_config_int_get("/entranced/attempts");

   d->dispnum = 0;
   d->status = NOT_RUNNING;
   d->auth_en = 1;
   d->auths = ecore_list_new();
   d->client.uid = -1;
   d->client.gid = -1;
   return d;
}

/**
 * Launch a new X server
 * @param d The spawner display context that will handle this server
 */
void
edd_spawn_x(Entranced_Display * d)
{
   int i = 0;

   if (d == NULL)
      return;

   d->status = NOT_RUNNING;
   while ((i < d->attempts) && (d->status != RUNNING))
   {
      if ((d->pid = _start_server_once(d)) > 0)
         break;
      ++i;
   }
}


/**
 * Start a new Entrance session
 * @param d The spawner display context that this session will use
 */
void
edd_spawn_entrance(Entranced_Display * d)
{
   char entrance_cmd[PATH_MAX];

   d->client.pid = 0;
   d->client.uid = -1;
   d->client.gid = -1;
   if (d->client.homedir)
      free(d->client.homedir);
   d->client.homedir = NULL;

   snprintf(entrance_cmd, PATH_MAX, "%s -d %s", ENTRANCE, d->name);
   if (d->config)
      snprintf(entrance_cmd, PATH_MAX, "%s -d %s -c \"%s\" -z %d", ENTRANCE,
               d->name, d->config, getpid());
   else
      snprintf(entrance_cmd, PATH_MAX, "%s -d %s -z %d", ENTRANCE, d->name,
               getpid());
   /* printf("Starting command: %s\n", entrance_cmd); */
   d->e_exe = ecore_exe_run(entrance_cmd, d);
   d->client.pid = ecore_exe_pid_get(d->e_exe);
}

int
edd_x_restart(Entranced_Display * d)
{
   /* Attempt to restart X server */
   d->status = NOT_RUNNING;

   syslog(LOG_INFO, "Attempting to restart X server.");
   edd_spawn_x(d);
   if (d->status != RUNNING)
   {
      syslog(LOG_CRIT, "Failed to restart the X server. Aborting.");
      return 0;
   }
   else
      syslog(LOG_INFO, "Successfully restarted the X server.");
   return 1;
}

void
edd_x_ready_set(unsigned char i)
{
   x_ready = i;
}

/*privates*/
/**
 * Single attempt to start the X Server.
 * @param d The spawner display context that will handle this server
 * @return The status of the display context after the launch attempt
 */
static pid_t
_start_server_once(Entranced_Display * d)	/* seems 
	   private */
{
   double start_time;
   char x_cmd[PATH_MAX];

   int i;
   char *x_cmd_argv[32];

   for (i = 0; i < 32; i++)
      x_cmd_argv[i] = NULL;

   /* Ecore_Exe *x_exe; */
   pid_t xpid;

   d->status = LAUNCHING;

   x_ready = 0;

   /* Create server auth cookie */

   if (d->auth_en)
   {
      if (!entranced_auth_display_secure(d))
      {
         syslog(LOG_CRIT, "Failed to generate auth cookie for X Server.");
         return -1;
      }

      snprintf(x_cmd, PATH_MAX, "%s -auth %s %s", d->xprog, d->authfile,
               d->name);
   }
   else
   {
      snprintf(x_cmd, PATH_MAX, "%s %s", d->xprog, d->name);
   }
   entranced_debug("Entranced_Start_Server_Once: Executing %s\n", x_cmd);

   /* x_exe = ecore_exe_run(d->xprog, d); */
   switch (xpid = fork())
   {
     case -1:
        syslog(LOG_WARNING, "fork() to start X server failed.");
        return -1;
     case 0:
        _entrance_x_sa.sa_handler = SIG_IGN;
        _entrance_x_sa.sa_flags = 0;
        sigemptyset(&_entrance_x_sa.sa_mask);
        sigaction(SIGUSR1, &_entrance_x_sa, NULL);

        x_cmd_argv[0] = strtok(x_cmd, " ");
        i = 1;

        while ((x_cmd_argv[i] = strtok(NULL, " ")) != NULL)
        {
           i++;
        }

        execvp(x_cmd_argv[0], x_cmd_argv);
        syslog(LOG_CRIT, "X server failed: %s", strerror(errno));
        exit(1);
     default:
        start_time = ecore_time_get();

        while (!x_ready)
        {
           double current_time;
           int status;
           pid_t rpid;

           sleep(10);
           rpid = waitpid(xpid, &status, WNOHANG);
           if (rpid == -1)
           {
              syslog(LOG_CRIT, "waitpid failed: %s", strerror(errno));
              x_ready = 0;
              break;
           }

           if ((rpid == xpid) && WIFEXITED(status))
           {
              syslog(LOG_CRIT, "X server exited unexpectedly: %d", WEXITSTATUS(status));
              x_ready = 0;
              xpid = -1;
              break;
           }

           current_time = ecore_time_get();
           if ((current_time - start_time) > 60.0)
              break;
        }

        if (!x_ready)
        {
           entranced_debug
              ("Entranced_Start_Server_Once: Attempt to start X server failed.\n");
           if (xpid >= 0)
           {
              kill(xpid, SIGTERM);
              xpid = -1;
           }
           d->status = NOT_RUNNING;
        }
        else
           d->status = RUNNING;

        return xpid;
   }
}
