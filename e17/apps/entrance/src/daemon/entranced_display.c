#include <Ecore_Config.h>
#include "Entranced.h"
#include "Entranced_Display.h"
#include "auth.h"
#include "util.h"


static unsigned char x_ready = 0;
static struct sigaction _entrance_x_sa;

void 
Entranced_Display_XReady_Set(unsigned char i)
{
	x_ready = i;
}


/**
 * Create a new display context.
 * @return A pointer to an Entranced_Display handle for the new context
 */
Entranced_Display *
Entranced_Display_New(void)
{
   Entranced_Display *d;

   d = calloc(1, sizeof(Entranced_Display));

   ecore_config_string_default("/entranced/xserver", DEFAULT_X_SERVER);
   ecore_config_int_default("/entranced/attempts", 5);

   ecore_config_file_load(PACKAGE_CFG_DIR "/entrance_config.cfg");

   d->xprog = ecore_config_string_get("/entranced/xserver");
   d->attempts = ecore_config_int_get("/entranced/attempts");

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
Entranced_Display_Spawn_X(Entranced_Display * d)
{
   int i = 0;

   if (d == NULL)
      return;

   d->status = NOT_RUNNING;
   while ((i < d->attempts) && (d->status != RUNNING))
   {
      if ((d->pid = Entranced_Display_Start_Server_Once(d)) > 0)
         break;
      ++i;
   }
}

/**
 * Single attempt to start the X Server.
 * @param d The spawner display context that will handle this server
 * @return The status of the display context after the launch attempt
 */
pid_t
Entranced_Display_Start_Server_Once(Entranced_Display * d)
{
   double start_time;
   char x_cmd[PATH_MAX];

   /* Ecore_Exe *x_exe; */
   pid_t xpid;

   d->status = LAUNCHING;

   x_ready = 0;

   /* Create server auth cookie */

   if(d->auth_en)
   {
      if (!entranced_auth_display_secure(d))
      {
         syslog(LOG_CRIT, "Failed to generate auth cookie for X Server.");
         return -1;
      }
   
      snprintf(x_cmd, PATH_MAX, "%s -auth %s %s", d->xprog, d->authfile, d->name);
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
      /* FIXME: need to parse command and NOT go thru /bin/sh!!!! */
      /* why? some /bin/sh's wont pass on this SIGUSR1 thing... */
        execl("/bin/sh", "/bin/sh", "-c", x_cmd, NULL);
        syslog(LOG_WARNING, "Could not execute X server.");
        exit(1);
     default:
        start_time = ecore_time_get();

        while (!x_ready)
        {
           double current_time;

           usleep(100000);
           current_time = ecore_time_get();
           if ((current_time - start_time) > 5.0)
	     break;
        }

        if (!x_ready)
        {
           entranced_debug("Entranced_Start_Server_Once: Attempt to start X server failed.\n");
           d->status = NOT_RUNNING;
        }
        else
           d->status = RUNNING;

        return xpid;
   }
}

/**
 * Start a new Entrance session
 * @param d The spawner display context that this session will use
 */
void
Entranced_Display_Spawn_Entrance(Entranced_Display *d)
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
      snprintf(entrance_cmd, PATH_MAX, "%s -d %s -c \"%s\" -z %d", 
               ENTRANCE, d->name, d->config, getpid());
   else
      snprintf(entrance_cmd, PATH_MAX, "%s -d %s -z %d", ENTRANCE, d->name,
                                                         getpid());
   printf("Starting command: %s\n", entrance_cmd);
   d->e_exe = ecore_exe_run(entrance_cmd, d);
   d->client.pid = ecore_exe_pid_get(d->e_exe);
}

int
Entranced_Display_X_Restart(Entranced_Display * d)
{
   /* Attempt to restart X server */
   d->status = NOT_RUNNING;

   syslog(LOG_INFO, "Attempting to restart X server.");
   Entranced_Display_Spawn_X(d);
   if (d->status != RUNNING)
   {
      syslog(LOG_CRIT, "Failed to restart the X server. Aborting.");
      return 0;
   }
   else
      syslog(LOG_INFO, "Successfully restarted the X server.");
   return 1;
}

