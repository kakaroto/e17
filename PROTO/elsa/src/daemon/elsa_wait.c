#include <stdio.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include "elsa_pam.h"

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

static void _elsa_wait_action(int sig, siginfo_t *si, void *data);
static void kill_wait();


static pid_t _x_pid = 0;
static int _restart = 1;

static void
_elsa_wait_action(int sig, siginfo_t * si __UNUSED__, void *data __UNUSED__)
{
    kill_wait();
    if (sig != SIGCHLD)
      {
         fprintf(stderr, "Child exit signal.\n");
         _restart = 0;
      }
}

static void
kill_wait()
{
   kill(_x_pid, SIGTERM);
}


int
main (int argc, char **argv)
{
   int status = 0;
   char *dname = ":0.0";
   char *user = NULL;
   char *command = NULL;
   char buf[4096];
   struct sigaction action;
   pid_t rpid;
   if (argc < 4) return 0;
   _x_pid = atoi(argv[1]);
   user = argv[2];
   dname = argv[3];
   command = argv[4];
   printf("%s pam setting with user %s on display %s\n", argv[0], user, dname);
   elsa_pam_init(PACKAGE, dname, user);
   elsa_pam_auth_set(user, NULL);

   action.sa_sigaction = _elsa_wait_action;
   action.sa_flags = SA_RESTART | SA_SIGINFO;
   sigemptyset(&action.sa_mask);
   sigaction(SIGCHLD, &action, NULL);
   sigaction(SIGQUIT, &action, NULL);
   sigaction(SIGTERM, &action, NULL);
   sigaction(SIGKILL, &action, NULL);
   sigaction(SIGINT, &action, NULL);
   sigaction(SIGHUP, &action, NULL);
   sigaction(SIGPIPE, &action, NULL);
   sigaction(SIGALRM, &action, NULL);

   printf("Wait for %d\n", _x_pid);
   while ((rpid = wait(&status)) != _x_pid)
     {
        if (rpid == -1)
          {
             fprintf(stderr, "waitpid(%d) failed: %s\n",
                     _x_pid, strerror(errno));
             if ((errno == ECHILD) || (errno == EINVAL))
               break;
             else
               fprintf(stderr, "waitpid(%d) returned: %d, retry...\n",
                       _x_pid, rpid);
          }
     }
   if (_x_pid == rpid)
     {
        if (command)
          {
             fprintf(stderr, PACKAGE": Session Shutdown\n");
             snprintf(buf, sizeof(buf),
                      "%s %s ", command, user);
             if (-1 == system(buf))
               fprintf(stderr, PACKAGE": Error on session stop command %s",
                       buf);
          }
        elsa_pam_close_session();
        if (WIFEXITED(status) && WEXITSTATUS(status))
          {
             exit(status);
          }
        else
          if (WIFSIGNALED(status))
            {
               kill(getpid(), WTERMSIG(status));
            }
          else
            {
               fprintf(stderr, "Wait done - child exited normally\n");
               elsa_pam_end();
               elsa_pam_shutdown();
               if (_restart)
                 execlp("elsa", "elsa", NULL);
               exit(0);
            }
     }
   elsa_pam_end();
   elsa_pam_shutdown();
   printf("wait error\n");
   return -1;
}

