#include "auth.h"

void elogin_auth_user(void)
{
   uid_t uid = 1000;
   int status;

   e_sync();
   XCloseDisplay(e_display_get());
   status = setuid(uid);
   if (status == -1)
      printf("Error attempting to set uid to %d\n", uid);
     {
	char env[4096];
	
	sprintf(env, "DISPLAY=%s", ":0.0");
	putenv(env);
     }
   execl("/bin/sh", "/bin/sh", "-c", "/usr/bin/X11/xterm", NULL);
}
