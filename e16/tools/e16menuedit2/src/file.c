/* Copyright (C) 2004 Andreas Volz and various contributors
 * 
 * Permission  is  hereby granted, free of charge, to any person ob-
 * taining a copy of  this  software  and  associated  documentation
 * files  (the "Software"), to deal in the Software without restric-
 * tion, including without limitation the rights to use, copy, modi-
 * fy, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is  fur-
 * nished to do so, subject to the following conditions:
 * 
 * The  above  copyright  notice and this permission notice shall be
 * included in all copies of the  Software,  its  documentation  and
 * marketing & publicity materials, and acknowledgment shall be giv-
 * en in the documentation, materials  and  software  packages  that
 * this Software was used.
 * 
 * THE  SOFTWARE  IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO  THE  WARRANTIES
 * OF  MERCHANTABILITY,  FITNESS FOR A PARTICULAR PURPOSE AND NONIN-
 * FRINGEMENT. IN NO EVENT SHALL  THE  AUTHORS  BE  LIABLE  FOR  ANY
 * CLAIM,  DAMAGES  OR OTHER LIABILITY, WHETHER IN AN ACTION OF CON-
 * TRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR  IN  CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 *  File: file.c
 *  Created by: Andreas Volz <linux@brachttal.net>
 *
 */

#include "file.h"

extern struct global_variables gv;

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

void compile_regex ()
{
  int status;
  char err_buf[ERR_BUF_SIZE];

  char pattern_mark[] = "\"([^\"]*)\"[[:space:]]*";
  char pattern_char[] = "([^[[:space:]]*)[[:space:]]*";
  char pattern_space[] = "[[:space:]]*(.*)";

  status = regcomp (&gv.re_mark, pattern_mark, REG_EXTENDED);
  if (status != 0)
  {
    regerror (status, &gv.re_mark, err_buf, (size_t) ERR_BUF_SIZE);
    printf ("regex compile: %s\n", err_buf);
  }
  status = regcomp (&gv.re_char, pattern_char, REG_EXTENDED);
  if (status != 0)
  {
    regerror (status, &gv.re_mark, err_buf, (size_t) ERR_BUF_SIZE);
    printf ("regex compile: %s\n", err_buf);
  }
  status = regcomp (&gv.re_space, pattern_space, REG_EXTENDED);
  if (status != 0)
  {
    regerror (status, &gv.re_mark, err_buf, (size_t) ERR_BUF_SIZE);
    printf ("regex compile: %s\n", err_buf);
  }

  gv.pattern_mark = strdup (pattern_mark);
  gv.pattern_char = strdup (pattern_char);
  gv.pattern_space = strdup (pattern_space);
}

int menu_regex (char *line, gchar ***data_ptr)
{
  struct substrings *ss = NULL;
  int numsub = 0;
  int maxsub = 1;
  int status;
  char err_buf[ERR_BUF_SIZE];
  char **data;
  int i, n;

  data = malloc (4 * sizeof (*data));

  *data_ptr = data;

  line = strdup (line);

  /* cut spaces */
  if (line[0] == ' ')
  {
    status = regex_sub2 (&gv.re_space, line, gv.pattern_space,
                         &ss, maxsub, &numsub);

    if (numsub >= 1)
    {
      free (line);
      line = strdup (ss[1].match);
    }

    for (i = 0; i < numsub; i++)
    {
      free (ss[i].match);
      free (ss[i].start);
      free (ss[i].end);
    }
    free (ss);
  }

  /* parse data array */
  for (n = 0; n < 4; n++)
  {
    data[n] = strdup ("");

    if (line[0] == '\"')
    {
      status = regex_sub2 (&gv.re_mark, line, gv.pattern_mark,
                           &ss, maxsub, &numsub);
      if (status != 0)
      {
        regerror (status, &gv.re_mark, err_buf, (size_t) ERR_BUF_SIZE);
        printf ("regex exec: %s\n", err_buf);
      }

      if (numsub >= 1)
      {
        free (line);
        line = strdup (ss[0].end);

        free (data[n]);
        data[n] = to_utf8 (ss[1].match);
      }

      for (i = 0; i < numsub; i++)
      {
        free (ss[i].match);
        free (ss[i].start);
        free (ss[i].end);
      }
      free (ss);
    }
    else if (line[0] != ' ')
    {
      status = regex_sub2 (&gv.re_char, line, gv.pattern_char,
                           &ss, maxsub, &numsub);
      if (status != 0)
      {
        regerror (status, &gv.re_char, err_buf, (size_t) ERR_BUF_SIZE);
        printf ("regex exec: %s\n", err_buf);
      }

      if (numsub >= 1)
      {

        free (line);
        line = strdup (ss[0].end);
        free (data[n]);
        data[n] = to_utf8 (ss[1].match);
      }

      for (i = 0; i < numsub; i++)
      {
        free (ss[i].match);
        free (ss[i].start);
        free (ss[i].end);
      }
      free (ss);
    }
  }

  return 0;
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

/* returns a version number of a pkg-config package
 * the return char* could be freed after use
 */
char *e16_version ()
{
  gboolean spawn;
  const int buf_len = 1024;
  gchar buf[buf_len];
  gchar *argv_child[4];
  gint stdout_child;
  gint stderr_child;
  int ret_val;
  gchar buf2[buf_len];

  argv_child[0] = g_strdup ("eesh");
  argv_child[1] = g_strdup ("-ewait");
  argv_child[2] = g_strdup ("ver");;
  argv_child[3] = NULL;

  spawn = g_spawn_async_with_pipes (NULL, argv_child, NULL,
                                    G_SPAWN_SEARCH_PATH, NULL,
                                    NULL, NULL,  NULL,
                                    &stdout_child, &stderr_child, NULL);

  g_free (argv_child[0]);
  g_free (argv_child[1]);

  ret_val = read (stdout_child, buf, buf_len);

  sscanf (buf, "Enlightenment-Version: enlightenment-%s\n", buf2);

  if (ret_val == 0)
    return 0;
  else
    return strdup (buf2);
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
  char *locale = NULL;
  gboolean locale_failed = FALSE;
  char *locale_fallback = NULL;
  char *locale_tmp;
  gboolean help_missing = TRUE;
  struct stat buf;

  /* Is there a better way to get the users current language? */
  locale = setlocale (LC_MESSAGES, NULL);

  /* If no locale is available use 'C' and free it later */
  if (!locale)
  {
    locale_failed = TRUE;
    locale = g_strdup_printf ("C");
  }

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
        g_free (locale_fallback);
	#ifdef DEBUG
        DEBUG_OUTPUT printf ("Try help fallback4: %s\n", params);
        #endif /* DEBUG */  
        help_missing = stat (params, &buf);
      }
      else
      {
        g_free (locale_fallback);
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

    g_free (params);
  }

  if (locale_failed)
    g_free (locale);

  return 0;
}
