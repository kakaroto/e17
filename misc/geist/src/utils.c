/* utils.c
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

#include "utils.h"
#include "geist.h"

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

  t = (char *) emalloc (strlen (s) + 1);
  if (t == NULL)
    eprintf ("estrdup(\"%.20s\") failed:", s);
  strcpy (t, s);
  return t;
}

/* emalloc: malloc and report if error */
void *
_emalloc (size_t n)
{
  void *p;

  p = g_malloc (n);
  if (p == NULL)
    eprintf ("g_malloc of %u bytes failed:", n);
  return p;
}

/* erealloc: realloc and report if error */
void *
_erealloc (void *ptr, size_t n)
{
  void *p;

  p = g_realloc (ptr, n);
  if (p == NULL)
    eprintf ("g_realloc of %p by %u bytes failed:", ptr, n);
  return p;
}

/* efree: just do the free for now */
void
_efree (void *p)
{
  g_free (p);
}

char *
stroflen(char c, int l)
{
   static char buf[1024];
   int i = 0;

   buf[0] = '\0';
   while (l--)
      buf[i++] = c;
   buf[i] = '\0';
   return buf;
}
