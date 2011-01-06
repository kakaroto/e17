#include "elsa.h"
#include <wait.h>
#include <unistd.h>
#include <xcb/xcb.h>

typedef struct Elsa_Xserver_
{
   const char *dname;
   Elsa_X_Cb start;
   pid_t pid;
   xcb_connection_t *display;
} Elsa_Xserver;

Elsa_Xserver *_xserver;
Ecore_Event_Handler *_handler_start;

/*
 * man Xserver
 * SIGUSR1 This  signal  is  used  quite  differently  from  either of the
 * above.  When the server starts, it checks to see if it has inherite
 * SIGUSR1 as SIG_IGN instead of the usual SIG_DFL.  In this case, the server
 * sends a SIGUSR1 to its parent process after it has set up the various
 * connection schemes.  Xdm uses this feature to recognize when connecting to
 * the server is possible.
 * */
static void
_env_set(const char *dname)
{
   char buf[PATH_MAX];
   snprintf(buf, sizeof(buf), "DISPLAY=%s", dname);
   putenv(buf);
}

static void
_xserver_start()
{
   char *buf = NULL;
   char **args = NULL;
   pid_t pid;

   pid = fork();
   if (!pid)
     {
        char *token;
        int num_token = 0;
        elsa_close_log();
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        signal(SIGUSR1, SIG_IGN);

        if (!(buf = strdup(elsa_config->command.xinit_args)))
          return;
        token = strtok(buf, " ");
        while(token)
          {
            ++num_token;
            token = strtok(NULL, " ");
          }
        if (buf) free(buf);
        if (num_token)
          {
             int i;
             if (!(buf = strdup(elsa_config->command.xinit_args)))
               return;
             if (!(args = calloc(num_token + 2, sizeof(char *))))
               {
                  if (buf) free(buf);
                  return;
               }
             args[0] = (char *)elsa_config->command.xinit_path;
             token = strtok(buf, " ");
             ++num_token;
             for(i = 1; i < num_token; ++i)
               {
                  if (token)
                    args[i] = token;
                  token = strtok(NULL, " ");
               }
             args[num_token] = NULL;
          }
        else
          {
             if (!(args = calloc(2, sizeof(char*))))
               return;
             args[0] = (char *)elsa_config->command.xinit_path;
             args[1] = NULL;
          }
        execv(args[0], args);
        if (buf) free(buf);
        if (args) free(args);
        fprintf(stderr, PACKAGE": Couldn't launch Xserver ...\n");
     }
   else
     {
        _xserver->pid = pid;
     }
}

static Eina_Bool
_xserver_stop()
{
   fprintf(stderr, PACKAGE": Xserver have been terminated\n");
   return ECORE_CALLBACK_PASS_ON;
}

void
elsa_xserver_wait()
{
   int status;
   pid_t pid = _xserver->pid;
   char buf[64];
   _xserver->pid = pid;
   pid_t wpid = -1;
   fprintf(stderr, PACKAGE": waiting Xserver with pid %d\n", pid);
   snprintf(buf, sizeof(buf), "%d", pid);
   execl("/usr/bin/elsa_wait", "/usr/bin/elsa_wait", buf, NULL);
   while (wpid != pid)
     {
//        pid = waitpid(pid, &status, 0);
        pid = wait(&status);
     }
   _xserver_stop();
}

static Eina_Bool
_xserver_started(void *data __UNUSED__, int type __UNUSED__, void *event __UNUSED__)
{
   xcb_connection_t *disp = NULL;
//   char buf[4];
//   snprintf(buf, sizeof(buf), ":%d\0", atoi(_xserver->dname));
//   disp = xcb_connect(buf, NULL);
   disp = xcb_connect(":0.0", NULL);
   if (disp)
     {
        _env_set(_xserver->dname);
        _xserver->display = disp;
        _xserver->start();
     }
   return ECORE_CALLBACK_PASS_ON;
}


void
elsa_xserver_init(Elsa_X_Cb start, const char *dname)
{
   sigset_t newset;
   sigemptyset(&newset);

   _xserver = calloc(1, sizeof(Elsa_Xserver));
   _xserver->dname = eina_stringshare_add(dname);
   _xserver->start = start;
   _xserver_start();
   _handler_start = ecore_event_handler_add(ECORE_EVENT_SIGNAL_USER,
                                            _xserver_started,
                                            NULL);
}

void
elsa_xserver_shutdown()
{
   if (_xserver->display) xcb_disconnect(_xserver->display);
   kill(_xserver->pid, SIGTERM);
   eina_stringshare_del(_xserver->dname);
   free(_xserver);
   ecore_event_handler_del(_handler_start);
}

int
elsa_xserver_pid_get()
{
   return _xserver->pid;
}
