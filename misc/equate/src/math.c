#include "Equate.h"

#include <signal.h>
#include <limits.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/socket.h>

int calc[2];
char *buffer;

void
err_msg (const char *str)
{
  fprintf (stderr, "%s\n", str);
  exit (1);
}
static void
sig_pipe (int signo)
{
  err_msg ("SIGPIPE caught\n");
}

char
*math_append (char *key)
{
  int n = strlen (buffer);
  int m = strlen (key);

  E (4, "Execing %s ...\n", key);
  memcpy (&buffer[n], key, m);
  buffer[n + m] = 0;
  E (5, "appended to: %s\n", buffer);

  return buffer;
}

char 
*math_exec (void)
{
  int n = strlen (buffer), i;
  char line[PATH_MAX];

  if (n <= 0)
    return;

  buffer[n] = '\n';
  buffer[++n] = 0;

  E (2, "Evaluating %d chars:%s", n, buffer);

  for (i = 0; i < n; i++)
    E (9, "(char %d is %d)\n", i, buffer[i]);

  write (calc[0], buffer, n);

  n = strlen (line);
  if ((n = read (calc[0], line, PATH_MAX)) < 0)
    {
      err_msg ("parent: read error from pipe");
    }
  if (n == 0)
    {
      err_msg ("parent: child closed pipe");
      return;
    }
  line[n - 1] = 0;		/* cut off the traing CR while delimiting the string */

  buffer[0] = 0;
  return line;
}


void
math_init (void)
{

  int n;
  pid_t pid;
  char line[PATH_MAX];

  if (signal (SIGPIPE, sig_pipe) == SIG_ERR)
    err_msg ("Signal Error");
  else if (socketpair (PF_LOCAL, SOCK_STREAM, 0, calc) < 0)
    err_msg ("Pipe Error");
  else if ((pid = fork ()) < 0)
    {
      err_msg ("Fork Failure");
    }
  else if (pid > 0)
    {
      close (calc[1]);
      /* now we continue as normal and load the ewl widgets etc */
    }
  else
    {
      close (calc[0]);
      if (calc[1] != STDIN_FILENO)
	{
	  if (dup2 (calc[1], STDIN_FILENO) != STDIN_FILENO)
	    {
	      err_msg ("Dup2 error to stdin");
	    }
	}
      if (calc[1] != STDOUT_FILENO)
	{
	  if (dup2 (calc[1], STDOUT_FILENO) != STDOUT_FILENO)
	    {
	      err_msg ("Dup2 error to stdout");
	    }
	}
      if (execl (BC_LOCATION, "bc", "-l", NULL) < 0)
	err_msg ("Execl error");
      exit (0);
    }

  buffer = (char *)malloc (BUFLEN);
  buffer[0] = 0;
}
