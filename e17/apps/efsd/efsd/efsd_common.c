/*

Copyright (C) 2000, 2001 Christian Kreibich <kreibich@aciri.org>.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to
deal in the Software without restriction, including without limitation the
rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies of the Software and its documentation and acknowledgment shall be
given in the documentation and software packages that this Software was
used.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/
#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <unistd.h>

#ifdef __EMX__
#include <strings.h>
#endif

#include <efsd_debug.h>
#include <efsd_misc.h>


char *
efsd_get_efsd_dir(void)
{
  char         *dir = NULL;
  static char  s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  dir = getenv("HOME");

  /* I'm not using getenv("TMPDIR") --
   * I don't see TMPDIR on Linux, FreeBSD
   * or Solaris here...
   */

  if (!dir)
    dir = "/tmp";

  snprintf(s, sizeof(s), "%s/.e", dir);
  if (!efsd_misc_file_is_dir(s))
    efsd_misc_mkdir(s);

  D_RETURN_(s);
}


char *
efsd_get_socket_file(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);
#ifndef __EMX__
  snprintf(s, sizeof(s), "%s/efsd_socket", efsd_get_efsd_dir());
#else
  snprintf(s, sizeof(s), "\\socket\\%s/efsd_socket", efsd_get_efsd_dir());
#endif
  s[sizeof(s)-1] = '\0';
  D_RETURN_(s);
}


void
efsd_remove_socket_file(void)
{
  D_ENTER;
  unlink(efsd_get_socket_file());
  D_RETURN;
}


char   *
efsd_get_magic_db(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  snprintf(s, sizeof(s), "%s/magic.db", efsd_get_efsd_dir());
  s[sizeof(s)-1] = '\0';

  D_RETURN_(s);
}


char   *
efsd_get_patterns_db(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  snprintf(s, sizeof(s), "%s/patterns.db", efsd_get_efsd_dir());
  s[sizeof(s)-1] = '\0';

  D_RETURN_(s);
}
