/*

Copyright (C) 2000, 2001 Christian Kreibich <cK@whoop.org>.

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

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>

#ifdef __EMX__
#include <stdlib.h>
#include <malloc.h>
#endif

#include <efsd_misc.h>
#include <efsd_debug.h>
#include <efsd_macros.h>
#include <efsd_misc.h>


mode_t         mode_755 = (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP |
			   S_IXGRP | S_IROTH | S_IXOTH);

void 
efsd_misc_remove_trailing_slashes(char *s)
{
  int len;

  D_ENTER;

  if (!s || s[0] == '\0' || !strcmp(s, "/"))
    D_RETURN;

  len = strlen(s);

  while (len > 0 && s[len-1] == '/')
    {
      s[len-1] = '\0';
      len--;
    }
  D_RETURN;
}

int    
efsd_misc_file_exists(char *filename)
{
  struct stat st;

  D_ENTER;

  if (!filename)
    D_RETURN_(FALSE);

  if (lstat(filename, &st) < 0)
    D_RETURN_(FALSE);

  D_RETURN_(TRUE);
}

char *
misc_get_user_dir(void)
{
  static char    s[4096] = "\0";
  struct passwd *pw = NULL;

  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  if ( (pw = getpwuid(geteuid())))
    {
      snprintf(s, MAXPATHLEN, "%s/.e/efsd", pw->pw_dir);
    }
  else
    {
      snprintf(s, MAXPATHLEN, "/tmp/.e_%u/efsd", geteuid());
    }

  D_RETURN_(s);
}


char *
misc_get_sys_dir(void)
{
  D_ENTER;
  D_RETURN_(PACKAGE_DATA_DIR);
}


char *
misc_get_socket_file(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);
#ifndef __EMX__
  snprintf(s, sizeof(s), "/tmp/.efsd_socket_%u", geteuid());
#else
  snprintf(s, sizeof(s), "\\socket\\.efsd_socket_%u", geteuid());
#endif
  s[sizeof(s)-1] = '\0';
  D_RETURN_(s);
}
