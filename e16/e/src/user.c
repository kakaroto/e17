/*
 * Copyright (C) 2000-2007 Carsten Haitzler, Geoff Harrison and various contributors
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
 */
#include "file.h"
#include "user.h"
#include "util.h"
#include <pwd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

char               *
username(int uid)
{
   char               *s;
   static int          usr_uid = -1;
   static char        *usr_s = NULL;
   struct passwd      *pwd;

   if (usr_uid < 0)
      usr_uid = getuid();
   if ((uid == usr_uid) && (usr_s))
      return Estrdup(usr_s);
   pwd = getpwuid(uid);
   if (pwd)
     {
	s = Estrdup(pwd->pw_name);
	if (uid == usr_uid)
	   usr_s = Estrdup(s);
	return s;
     }
   return Estrdup("unknown");
}

char               *
homedir(int uid)
{
   static int          usr_uid = -1;
   static char        *usr_s = NULL;
   char               *s;
   const char         *ss;
   struct passwd      *pwd;

   if (usr_uid < 0)
      usr_uid = getuid();
   if ((uid == usr_uid) && (usr_s))
     {
	return Estrdup(usr_s);
     }
   pwd = getpwuid(uid);
   if (pwd)
     {
	s = Estrdup(pwd->pw_dir);
	if (uid == usr_uid)
	   usr_s = Estrdup(s);
	return s;
     }
   ss = getenv("TMPDIR");
   if (!ss)
      ss = "/tmp";
   return Estrdup(ss);
}

char               *
usershell(int uid)
{
   char               *s;
   static int          usr_uid = -1;
   static char        *usr_s = NULL;
   struct passwd      *pwd;

   if (usr_uid < 0)
      usr_uid = getuid();
   if ((uid == usr_uid) && (usr_s))
      return Estrdup(usr_s);
   pwd = getpwuid(uid);
   if (pwd)
     {
	if (!pwd->pw_shell)
	   return Estrdup("/bin/sh");
	if (strlen(pwd->pw_shell) < 1)
	   return Estrdup("/bin/sh");
	if (!(canexec(pwd->pw_shell)))
	   return Estrdup("/bin/sh");
	s = Estrdup(pwd->pw_shell);
	if (uid == usr_uid)
	   usr_s = Estrdup(s);
	return s;
     }
   return Estrdup("/bin/sh");
}
