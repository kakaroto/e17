#include "util.h"

struct passwd *
struct_passwd_dup(struct passwd *pwent)
{
   struct passwd *result = NULL;

   if (pwent)
   {
      result = (struct passwd *) malloc(sizeof(struct passwd));
      memset(result, 0, sizeof(struct passwd));
      if (pwent->pw_name)
         result->pw_name = strdup(pwent->pw_name);
      if (pwent->pw_passwd)
         result->pw_passwd = strdup(pwent->pw_passwd);
      if (pwent->pw_gecos)
         result->pw_gecos = strdup(pwent->pw_gecos);
      if (pwent->pw_shell)
         result->pw_shell = strdup(pwent->pw_shell);
      if (pwent->pw_dir)
         result->pw_dir = strdup(pwent->pw_dir);

      result->pw_uid = pwent->pw_uid;
      result->pw_gid = pwent->pw_gid;
   }

   return (result);
}

void *
struct_passwd_free(struct passwd *pwent)
{
   if (pwent)
   {
      if (pwent->pw_name)
         free(pwent->pw_name);
      if (pwent->pw_passwd)
         free(pwent->pw_passwd);
      if (pwent->pw_gecos)
         free(pwent->pw_gecos);
      if (pwent->pw_shell)
         free(pwent->pw_shell);
      if (pwent->pw_dir)
         free(pwent->pw_dir);
      free(pwent);
   }
   return (NULL);
}
