/*
 * vim:ts=8:sw=3:sts=8:noexpandtab:cino=>3
 */
#include "term.h"

struct winsize *get_font_dim(Term *term)
{
   static struct winsize w;
   w.ws_row = term->rows;
   w.ws_col = term->cols;
   w.ws_xpixel = w.ws_ypixel = 0;
   return &w;
}

int get_pty(Term *term)
{
   /* do we need this here? */
   /* extern char *ptsname(); */
   int fd;
   char *ptydev;

   if((fd = getpt()) >= 0)
     {
	if(grantpt(fd) == 0 && unlockpt(fd) == 0)
	  {
	     ptydev = ptsname(fd);
	     if((term->slave.sys = open(ptydev, O_RDWR | O_NOCTTY)) < 0)
	       {
		  fprintf(stderr, "Error opening slave pty: %m\n");
		  return -1;
	       }
	     fcntl(fd, F_SETFL, O_NDELAY);
	     return fd;
	  }
	close(fd);
     }
   fprintf(stderr, "Can't open a pseudo-tty\n");
   return -1;
}

int get_tty(Term *term)
{
   int i;

   for(i = 0; i < 100 ; i++)
     if(i != term->slave.sys)
       close(i);

   term->cmd_fd.sys = term->slave.sys;

   setsid(); /* create a new process group */

   dup2(term->cmd_fd.sys, 0);
   dup2(term->cmd_fd.sys, 1);
   dup2(term->cmd_fd.sys, 2);

   if(ioctl(term->cmd_fd.sys, TIOCSCTTY, NULL) < 0)
     {
	fprintf(stderr, "Couldn't set controlling terminal: %m\n");
	return -1;
     }

   return 0;
}

void sigchld_handler(int a) {
   int status = 0;

   if(waitpid(pid, &status, 0) < 0) {
      fprintf(stderr, "Waiting for pid %hd failed: %m\n", pid);
      exit(1);
   }

   if(WIFEXITED(status)) {
      /* Child exited by itself */

      if(WEXITSTATUS(status))
	exit(WEXITSTATUS(status));
   }
   else if(WIFSIGNALED(status))  /* Child was killed by a signal */
     exit(1);
   else                          /* Something strange happened */
     exit(1);

   exit(0);
}

struct passwd *find_user(void)
{
   uid_t uid;
   struct passwd *pw;

   if((uid = getuid()) == -1) {
      fprintf(stderr, "Couldn't find current uid: %m\n");
      exit(2);
   }

   if((pw = getpwuid(uid)) == NULL) {
      fprintf(stderr, "Couldn't find password entry for user %d: %m\n", uid);
      exit(2);
   }
   return pw;
}

int execute_command(Term *term)//, int argc, const char **argv)
{
   char **args;
   struct passwd *pw;

   pw = find_user();

   if((term->cmd_fd.sys = get_pty(term)) < 0)
     return -1;

   if((pid = fork()) < 0) {
      fprintf(stderr, "Couldn't fork: %m\n");
      return -1;
   }
   if(!pid) {
      /* child */
      get_tty(term);

      putenv("TERM=xterm");
      chdir(pw->pw_dir);

      args = calloc(3, sizeof(char*));
      args[0] = malloc(strlen(pw->pw_shell) + 1);
      strcpy(args[0], pw->pw_shell);
      args[1] = "-i";
      args[2] = NULL;

      execvp(pw->pw_shell, args);

      /* shouldn't be here */
      fprintf(stderr, "Error executing %s: %m\n", pw->pw_shell);
      exit(1);
   }

   /* parent */
   close(term->slave.sys);
   signal(SIGCHLD, sigchld_handler);

   return 0;
}

