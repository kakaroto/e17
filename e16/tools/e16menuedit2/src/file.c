/* Copyright (C) 2004 Andreas Volz and various contributors
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies of the Software, its documentation and marketing & publicity
 * materials, and acknowledgment shall be given in the documentation, materials
 * and software packages that this Software was used.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  File: file.c
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#include "file.h"

char *homedir (int uid)
{
  char *s;

  static int usr_uid = -1;
  static char *usr_s = NULL;
  struct passwd *pwd;

  if (usr_uid < 0)
    usr_uid = getuid ();
  if ((uid == usr_uid) && (usr_s))
  {
    return (g_strdup (usr_s));
  }
  pwd = getpwuid (uid);
  if (pwd)
  {
    s = g_strdup (pwd->pw_dir);
    if (uid == usr_uid)
      usr_s = g_strdup (s);
    return (s);
  }
  return (g_strdup
          ((getenv ("TMPDIR") == NULL) ? "/tmp" : getenv ("TMPDIR")));
}

char *field (char *s, int field)
{
  char buf[4096];

  buf[0] = 0;
  fword (s, field + 1, buf);
  if (buf[0])
  {
    if ((!strcmp (buf, "NULL")) || (!strcmp (buf, "(null)")))
      return (NULL);
    return (g_strdup (buf));
  }
  return NULL;
}

void fword (char *s, int num, char *wd)
{
  char *cur, *start, *end;
  int count, inword, inquote, len;

  if (!s)
    return;
  if (!wd)
    return;
  *wd = 0;
  if (num <= 0)
    return;
  cur = s;
  count = 0;
  inword = 0;
  inquote = 0;
  start = NULL;
  end = NULL;
  while ((*cur) && (count < num))
  {
    if (inword)
    {
      if (inquote)
      {
        if (*cur == '"')
        {
          inquote = 0;
          inword = 0;
          end = cur;
          count++;
        }
      }
      else
      {
        if (isspace (*cur))
        {
          end = cur;
          inword = 0;
          count++;
        }
      }
    }
    else
    {
      if (!isspace (*cur))
      {
        if (*cur == '"')
        {
          inquote = 1;
          start = cur + 1;
        }
        else
          start = cur;
        inword = 1;
      }
    }
    if (count == num)
      break;
    cur++;
  }
  if (!start)
    return;
  if (!end)
    end = cur;
  if (end <= start)
    return;
  len = (int) (end - start);
  if (len > 4000)
    len = 4000;
  if (len > 0)
  {
    strncpy (wd, start, len);
    wd[len] = 0;
  }
  return;
}

int mkdir_with_parent (const char *pathname, mode_t mode)
{
  int err;
  int i;
  char *left;
  char *tok;
  char *pathname_copy = strdup (pathname);

  i = 0;
  do
  {
    tok = strtok_left (pathname_copy, "/", i);
    if (!tok)
      break;
    left = strsplit (pathname_copy, NULL,
                     strlen (pathname_copy) - strlen (tok));
    err = mkdir (left, mode);

    i++;
  }
  while (tok != NULL);

  free (pathname_copy);

  return err;
}

char *strsplit (char *s, char **right, int count)
{
  char *s_org = strdup (s);

  if (count < strlen (s))
  {
    if (right)
      strcpy (*right, s + count);

    s_org[count]='\0';
  }

  return s_org;
}

char *strtok_left (char *s, const char *delim, unsigned int number)
{
  int i;
  char *pos = s;
  char *s_new;

  if (number == 0)
    return s;

  if (strcmp (s + strlen (s) - strlen (delim), delim))
  {
    s_new = malloc (strlen (s) + strlen (delim) + 1);
    strcpy (s_new, s);
    strcat (s_new, delim);
  }
  else
  {
    s_new = s;
  }

  for (i = 0; pos != NULL; i++)
  {
    pos = strstr (s_new, delim);

    if (pos)
    {
      s_new = pos + (1 * strlen (delim));

      if (i == number - 1)
      {
        pos = '\0';
        return s_new;
      }
    }
  }

  return NULL;
}

/* if ver1  < ver2 -> return -1
 * if ver1  > ver2 -> return 1
 * if ver1 == ver2 -> return 0
 */
int version_cmp (char *ver1, char *ver2)
{
  char *ver1_token;
  char *ver2_token;
  char *ver1_ptr = malloc (strlen (ver1)+1);
  char *ver2_ptr = malloc (strlen (ver2)+1);

  ver1_token = strtok_r (ver1, ".", &ver1_ptr);
  ver2_token = strtok_r (ver2, ".", &ver2_ptr);

  while ((ver1_token != NULL) || (ver2_token != NULL))
  {
    int ver1_i;
    int ver2_i;

    if (ver1_token == NULL)
      ver1_i = 0;
    else
      ver1_i = atoi (ver1_token);

    if (ver2_token == NULL)
      ver2_i = 0;
    else
      ver2_i = atoi (ver2_token);

    if (ver1_i < ver2_i)
      return -1;
    else if (ver1_i > ver2_i)
      return 1;

    ver1_token = strtok_r (NULL, ".", &ver1_ptr);
    ver2_token = strtok_r (NULL, ".", &ver2_ptr);
  }

  return 0;
}

/* returns a version number of a pkg-config package
 * the return char* could be freed after use
 */
char *pkg_config_version (char *package)
{
  gboolean spawn;
  const int buf_len = 128;
  gchar buf[buf_len];
  gchar *argv_child[4];
  gint stdout_child;
  gint stderr_child;
  int ret_val;

  argv_child[0] = g_strdup ("pkg-config");
  argv_child[1] = g_strdup ("--modversion");
  argv_child[2] = package;
  argv_child[3] = NULL;

  spawn = g_spawn_async_with_pipes (NULL, argv_child, NULL,
                                    G_SPAWN_SEARCH_PATH, NULL,
                                    NULL, NULL,  NULL,
                                    &stdout_child, &stderr_child, NULL);

  g_free (argv_child[0]);
  g_free (argv_child[1]);

  ret_val = read (stdout_child, buf, buf_len);

  if (ret_val == 0)
    return 0;
  else
    return strdup (buf);
}

char *get_fallback_locale (char *locale)
{
  char *locale_dup;
  char *pos = NULL;

  locale_dup = strdup (locale);
  pos = strrchr (locale_dup, '@');

  if (pos)
  {
    strcpy (pos, "\0");
  }
  else
  {
    pos = strrchr (locale_dup, '_');

    if (pos)
    {
      strcpy (pos, "\0");
    }
    else
    {
      pos = locale_dup;
      strcpy (pos, "C");
    }
  }

  return locale_dup;
}

int run_help (char *help_app, char *help_dir, char *help_file)
{
  gboolean spawn;
  gchar *argv_child[3];
  gchar *params;
  char *locale;
  char *locale_fallback;
  char *locale_tmp;
  gboolean help_missing = TRUE;
  struct stat buf;

  locale = setlocale (LC_ALL, NULL);

  params = g_strdup_printf ("%s/%s/%s", help_dir,
                            locale, help_file);
#ifdef DEBUG
  DEBUG_OUTPUT printf ("%s\n", params);
#endif /* DEBUG */  
  help_missing = stat (params, &buf);

  /* locale fallback 1 */
  if (help_missing)
  {
    locale_fallback = get_fallback_locale (locale);
    g_free (params);
    params = g_strdup_printf ("%s/%s/%s", help_dir,
                              locale_fallback, help_file);
    #ifdef DEBUG
    DEBUG_OUTPUT printf ("Try help fallback1: %s\n", params);
    #endif /* DEBUG */  
    help_missing = stat (params, &buf);

    /* locale fallback 2 */
    if (help_missing)
    {
      locale_tmp = strdup (locale_fallback);
      g_free (locale_fallback);
      locale_fallback = get_fallback_locale (locale_tmp);
      g_free (locale_tmp);
      g_free (params);
      params = g_strdup_printf ("%s/%s/%s", help_dir,
                                locale_fallback, help_file);
      #ifdef DEBUG
      DEBUG_OUTPUT printf ("Try help fallback2: %s\n", params);
      #endif /* DEBUG */  
      help_missing = stat (params, &buf);

      /* locale fallback 3 */
      if (help_missing)
      {
        locale_tmp = strdup (locale_fallback);
        g_free (locale_fallback);
        locale_fallback = get_fallback_locale (locale_tmp);
        g_free (locale_tmp);
        g_free (params);
        params = g_strdup_printf ("%s/%s/%s", help_dir,
                                  locale_fallback, help_file);
	#ifdef DEBUG
        DEBUG_OUTPUT printf ("Try help fallback4: %s\n", params);
        #endif /* DEBUG */  
        help_missing = stat (params, &buf);
      }
    }
  }

  if (help_missing)
  {
    /* no help files found */
    return 1;
  }
  else
  {
    argv_child[0] = help_app;
    argv_child[1] = params;
    argv_child[2] = NULL;
  
    #ifdef DEBUG
    DEBUG_OUTPUT printf ("Try exec help: %s %s\n", help_app, params);
    #endif /* DEBUG */

    spawn = g_spawn_async (NULL, argv_child, NULL,
                           G_SPAWN_SEARCH_PATH, NULL,
                           NULL, NULL, NULL);

    if (!spawn)
    {
      /* no help application found */
      return 2;
    }

    //g_free (argv_child[0]);
    g_free (params);
    g_free (locale_fallback);
  }

  return 0;
}
