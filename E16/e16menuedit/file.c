#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/types.h>
#include <glib.h>
#include "file.h"

char *
homedir (int uid)
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

char *
field (char *s, int field)
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

void
fword (char *s, int num, char *wd)
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
