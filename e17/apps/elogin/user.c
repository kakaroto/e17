/* from old elogin */

#include "auth.h"

Userinfo *
elogin_new_userinfo (char *user)
{
  Userinfo *u = (Userinfo *) malloc (sizeof (Userinfo));

  u->user = (char *) strdup (user);
  u->pw = getpwnam (user);
  endpwent ();
  if (!u->pw)
    {
      printf ("user %s not found in /etc/passwd\n", user);
    }
  u->env = (char **) malloc (sizeof (char *) * 2);
  u->env[0] = 0;

  return u;
}
