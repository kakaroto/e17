/* ultils.c
 *
 * Copyright (C) 1999 Tom Gilbert
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "feh.h"

/* eprintf: print error message and exit */
void
eprintf (char *fmt, ...)
{
  va_list args;

  fflush (stdout);
  fprintf (stderr, "%s ERROR: ", PACKAGE);

  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);

  if (fmt[0] != '\0' && fmt[strlen (fmt) - 1] == ':')
    fprintf (stderr, "%s", strerror (errno));
  fprintf (stderr, "\n");
  exit (2);
}

/* weprintf: print warning message and continue */
void
weprintf (char *fmt, ...)
{
  va_list args;

  fflush (stdout);
  fprintf (stderr, "%s WARNING: ", PACKAGE);

  va_start (args, fmt);
  vfprintf (stderr, fmt, args);
  va_end (args);

  if (fmt[0] != '\0' && fmt[strlen (fmt) - 1] == ':')
    fprintf (stderr, "%s", strerror (errno));
  fprintf (stderr, "\n");
}

/* estrdup: duplicate a string, report if error */
char *
estrdup (char *s)
{
  char *t;

  t = (char *) malloc (strlen (s) + 1);
  if (t == NULL)
    eprintf ("estrdup(\"%.20s\") failed:", s);
  strcpy (t, s);
  return t;
}

/* emalloc: malloc and report if error */
void *
emalloc (size_t n)
{
  void *p;

  p = malloc (n);
  if (p == NULL)
    eprintf ("malloc of %u bytes failed:", n);
  return p;
}

/* erealloc: realloc and report if error */
void *
erealloc (void *ptr, size_t n)
{
  void *p;

  p = realloc (ptr, n);
  if (p == NULL)
    eprintf ("realloc of %p by %u bytes failed:", ptr, n);
  return p;
}

char *
strjoin (const char *separator, ...)
{
  char *string, *s;
  va_list args;
  int len;
  int separator_len;

  if (separator == NULL)
    separator = "";

  separator_len = strlen (separator);
  va_start (args, separator);
  s = va_arg (args, char *);

  if (s)
    {
      len = strlen (s);
      s = va_arg (args, char *);
      while (s)
        {
          len += separator_len + strlen (s);
          s = va_arg (args, char *);
        }
      va_end (args);
      string = malloc (sizeof (char) * (len + 1));
      *string = 0;
      va_start (args, separator);
      s = va_arg (args, char *);
      strcat (string, s);
      s = va_arg (args, char *);
      while (s)
        {
          strcat (string, separator);
          strcat (string, s);
          s = va_arg (args, char *);
        }
    }
  else
    string = estrdup ("");
  va_end (args);

  return string;
}
