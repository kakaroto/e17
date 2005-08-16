#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "../config.h"

#include "entrance_auth.h"

#ifdef HAVE_PAM
int
entrance_end_user_session(Entrance_Auth * e)
{
   int pamerr;

   if (!e->pam.handle)
      return ERROR_NO_PAM_INIT;

   syslog(LOG_INFO, "Ending PAM session for user \"%s\".", e->user);

   if ((pamerr = pam_close_session(e->pam.handle, PAM_SILENT)) != PAM_SUCCESS)
   {
      syslog(LOG_CRIT, "PAM: %s.", pam_strerror(e->pam.handle, pamerr));
      return ERROR_NO_PAM_INIT;
   }

   return E_SUCCESS;
}
#endif

int
main(int argc, char **argv)
{
   pid_t pid = -1;
   char *user = NULL;
   char *display = NULL;

#ifdef HAVE_PAM
   Entrance_Auth *e = NULL;
#endif

   openlog("entrance_login", LOG_PID, LOG_DAEMON);

#ifdef HAVE_PAM
   if (argc != 4)
#else
   if (argc != 2)
#endif
   {
      syslog(LOG_CRIT, "Wrong number of arguments: %d!", argc);
      return 0;
   }

   if (getuid() != 0)
   {
      syslog(LOG_CRIT, "Not running as root!");
      exit(1);
   }

   pid = atoi(argv[1]);
#if HAVE_PAM
   user = argv[2];
   display = argv[3];

   if (user && display)
   {
      e = entrance_auth_new();
      if (entrance_auth_user_set(e, user))
      {
         syslog(LOG_CRIT, "Can't set user %s!", user);
         exit(1);
      }
      entrance_auth_pam_initialize(e, display);
   }
#endif

   if (waitpid(pid, NULL, 0) == pid)
   {
#ifdef HAVE_PAM
      if (e)
      {
         if (entrance_end_user_session(e) != E_SUCCESS)
            syslog(LOG_INFO, "Error Shutting down PAM");
         entrance_auth_free(e);
      }
#endif
      closelog();
      exit(0);
   }
   return -1;
}
