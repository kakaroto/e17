#include "elogin.h"
#include "auth.h"

Userinfo *elogin_new_userinfo(char *user)
{
   Userinfo *u = malloc(sizeof(Userinfo));

   u->user = strdup(user);
   u->pw = getpwnam(user);
   endpwent();
   if (!u->pw) {
      printf("user %s not found in /etc/passwd\n", user);
   }
   u->env = malloc(sizeof(char *) * 2);
   u->env[0] = 0;

   return u;
}
