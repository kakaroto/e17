#include <string.h>
#include <stdlib.h>
#include "entrance.h"
#include "util.h"
#include <Evas.h>

/**
@file util.c
@brief Functions that do things that don't quite fit anywhere else

*/

/**
 * struct_passwd_dup : duplicate a struct passwd *
 * @param pwent - the struct passwd pointer we want to copy
 * @return a valid pointer to an allocated struct passwd * 
 */
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

/**
 * struct_passwd_free: free a struct passwd *
 * @param pwent - the struct passwd pointer we want to free
 * @return NULL
 */
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

/**
 * entrance_debug: print a message if debugging is on
 * @param msg - the character string you want printed in debug mode
 */
void
entrance_debug(char *msg)
{
   if (ENTRANCE_DEBUG)
      printf("%s\n", msg);
}

char *
theme_normalize_path(char *theme, const char *filename)
{
   char *t = strchr((const char *) filename, '/');

   if (t)
      theme = strdup(filename);
   else
   {
      theme = calloc(1, PATH_MAX);
      t = strrchr((const char *) filename, '.');
      if (t && !strcmp(t, ".edj"))
         snprintf(theme, PATH_MAX, "%s/themes/%s", PACKAGE_DATA_DIR,
                  filename);
      else
         snprintf(theme, PATH_MAX, "%s/themes/%s.edj", PACKAGE_DATA_DIR,
                  filename);
   }

   return theme;
}


char *
gstr_is_valid(const char *gstr)
{
   char *t = strchr((const char *) gstr, 'x');

   if (!t || t >= (gstr + strlen(gstr)))
   {
      return NULL;
   }

   return strdup(t);
}

void
atog(const char *gstr, int *x, int *y)
{
   char *sep = gstr_is_valid(gstr);

   if (sep)
   {
      *x = atoi((const char *) gstr);
      *y = atoi((const char *) (sep + 1));
      free(sep);
   }
}
