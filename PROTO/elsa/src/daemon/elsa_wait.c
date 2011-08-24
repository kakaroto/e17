#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

static void _elsa_wait_action(int sig, siginfo_t *si, void *data);
static void kill_wait();


static pid_t _x_pid = 0;

static void
_elsa_wait_action(int sig, siginfo_t * si __UNUSED__, void *data __UNUSED__)
{
    kill_wait();
    if (sig != SIGCHLD)
      setenv("ELSA_QUIT", "1", 1);
}

static void
kill_wait()
{
   kill(_x_pid, SIGTERM);
}

int
main (int argc __UNUSED__, char **argv __UNUSED__)
{
   int status = 0;
   char *pid;
   struct sigaction action;

   pid_t rpid;
   pid = getenv("ELSA_XPID");
   if (!pid) return -1;
   _x_pid = atoi(pid);

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

   while ((rpid = wait(&status)) != _x_pid)
     {
        if (rpid == -1)
          {
             if ((errno == ECHILD) || (errno == EINVAL))
               break;
          }
     }
   if (_x_pid == rpid)
     {
        if (WIFEXITED(status) && WEXITSTATUS(status))
          setenv("ELSA_QUIT", "1", 1);
        execlp("elsa", "elsa", "--nodaemon", NULL);
     }
   return -1;
}

