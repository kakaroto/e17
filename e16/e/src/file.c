/*
 * Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
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
#include "E.h"
#include <sys/stat.h>
#include <ctype.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>

char               *
FileExtension(char *file)
{
   char               *p;

   EDBUG(5, "FileExtension");

   p = strrchr(file, '.');
   if (p != NULL)
     {
	EDBUG_RETURN(p + 1);
     }
   EDBUG_RETURN("");
}

void
Etmp(char *s)
{
   static unsigned long n_calls = 0;

   EDBUG(9, "Etmp");
   if (!n_calls)
      n_calls = (unsigned long)time(NULL) + (unsigned long)getpid();
   Esnprintf(s, 1024, "%s/TMP_%lX", EDirUser(), n_calls);
   n_calls++;
   EDBUG_RETURN_;
}

void
E_md(const char *s)
{
   EDBUG(9, "md");
   if ((!s) || (!*s))
      EDBUG_RETURN_;
   mkdir(s, S_IRWXU);
   EDBUG_RETURN_;
}

int
exists(const char *s)
{
   struct stat         st;

   EDBUG(9, "exists");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (stat(s, &st) < 0)
      EDBUG_RETURN(0);
   EDBUG_RETURN(1);
}

void
mkdirs(const char *s)
{
   char                ss[FILEPATH_LEN_MAX];
   int                 i, ii;

   i = 0;
   ii = 0;
   while (s[i])
     {
	ss[ii++] = s[i];
	ss[ii] = 0;
	if (s[i] == '/')
	  {
	     if (!exists(ss))
		E_md(ss);
	     else if (!isdir(ss))
		return;
	  }
	i++;
     }
}

int
isfile(const char *s)
{
   struct stat         st;

   EDBUG(9, "isfile");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (stat(s, &st) < 0)
      EDBUG_RETURN(0);
   if (S_ISREG(st.st_mode))
      EDBUG_RETURN(1);
   EDBUG_RETURN(0);
}

int
isdir(const char *s)
{
   struct stat         st;

   EDBUG(9, "isdir");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (stat(s, &st) < 0)
      EDBUG_RETURN(0);
   if (S_ISDIR(st.st_mode))
      EDBUG_RETURN(1);
   EDBUG_RETURN(0);
}

int
isabspath(const char *path)
{
   return path[0] == '/';
}

char              **
E_ls(const char *dir, int *num)
{
   int                 i, dirlen;
   int                 done = 0;
   DIR                *dirp;
   char              **names;
   struct dirent      *dp;

   EDBUG(9, "ls");
   if ((!dir) || (!*dir))
      EDBUG_RETURN(0);
   dirp = opendir(dir);
   if (!dirp)
     {
	*num = 0;
	EDBUG_RETURN(NULL);
     }
   /* count # of entries in dir (worst case) */
   for (dirlen = 0; (dp = readdir(dirp)) != NULL; dirlen++);
   if (!dirlen)
     {
	closedir(dirp);
	*num = dirlen;
	EDBUG_RETURN(NULL);
     }
   names = (char **)Emalloc(dirlen * sizeof(char *));

   if (!names)
      EDBUG_RETURN(NULL);

   rewinddir(dirp);
   for (i = 0; i < dirlen;)
     {
	dp = readdir(dirp);
	if (!dp)
	   break;
	if ((strcmp(dp->d_name, ".")) && (strcmp(dp->d_name, "..")))
	  {
	     names[i] = Estrdup(dp->d_name);
	     i++;
	  }
     }

   if (i < dirlen)
      dirlen = i;		/* dir got shorter... */
   closedir(dirp);
   *num = dirlen;
   /* do a simple bubble sort here to alphanumberic it */
   while (!done)
     {
	done = 1;
	for (i = 0; i < dirlen - 1; i++)
	  {
	     if (strcmp(names[i], names[i + 1]) > 0)
	       {
		  char               *tmp;

		  tmp = names[i];
		  names[i] = names[i + 1];
		  names[i + 1] = tmp;
		  done = 0;
	       }
	  }
     }
   EDBUG_RETURN(names);
}

void
freestrlist(char **l, int num)
{
   EDBUG(9, "freestrlist");
   if (!l)
      EDBUG_RETURN_;
   while (num--)
      if (l[num])
	 Efree(l[num]);
   Efree(l);
   EDBUG_RETURN_;
}

void
E_rm(const char *s)
{
   EDBUG(9, "rm");
   if ((!s) || (!*s))
      EDBUG_RETURN_;
   unlink(s);
   EDBUG_RETURN_;
}

void
E_mv(const char *s, const char *ss)
{
   EDBUG(9, "mv");
   if ((!s) || (!ss) || (!*s) || (!*ss))
      EDBUG_RETURN_;
   rename(s, ss);
   EDBUG_RETURN_;
}

void
E_cp(const char *s, const char *ss)
{
   int                 i;
   FILE               *f, *ff;
   unsigned char       buf[1];

   EDBUG(9, "cp");
   if ((!s) || (!ss) || (!*s) || (!*ss))
      EDBUG_RETURN_;
   if (!exists(s))
      EDBUG_RETURN_;
   i = filesize(s);
   f = fopen(s, "r");
   if (!f)
      EDBUG_RETURN_;
   ff = fopen(ss, "w");
   if (!ff)
     {
	fclose(f);
	EDBUG_RETURN_;
     }
   while (fread(buf, 1, 1, f))
      fwrite(buf, 1, 1, ff);
   fclose(f);
   fclose(ff);
   EDBUG_RETURN_;
}

time_t
moddate(const char *s)
{
   struct stat         st;

   EDBUG(9, "moddate");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (stat(s, &st) < 0)
      EDBUG_RETURN(0);
   if (st.st_mtime > st.st_ctime)
      EDBUG_RETURN(st.st_mtime);
   EDBUG_RETURN(st.st_ctime);
}

int
filesize(const char *s)
{
   struct stat         st;

   EDBUG(9, "filesize");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (stat(s, &st) < 0)
      EDBUG_RETURN(0);
   EDBUG_RETURN((int)st.st_size);
}

int
fileinode(const char *s)
{
   struct stat         st;

   EDBUG(9, "filesize");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (stat(s, &st) < 0)
      EDBUG_RETURN(0);
   EDBUG_RETURN((int)st.st_ino);
}

int
filedev(const char *s)
{
   struct stat         st;

   EDBUG(9, "filesize");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (stat(s, &st) < 0)
      EDBUG_RETURN(0);
   EDBUG_RETURN((int)st.st_dev);
}

void
E_cd(const char *s)
{
   EDBUG(9, "cd");
   if ((!s) || (!*s))
      EDBUG_RETURN_;
   chdir(s);
   EDBUG_RETURN_;
}

char               *
cwd(void)
{
   char               *s;
   char                ss[FILEPATH_LEN_MAX];

   EDBUG(9, "cwd");
   getcwd(ss, FILEPATH_LEN_MAX);
   s = Estrdup(ss);
   EDBUG_RETURN(s);
}

int
permissions(const char *s)
{
   struct stat         st;

   EDBUG(9, "permissions");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (stat(s, &st) < 0)
      EDBUG_RETURN(0);
   EDBUG_RETURN(st.st_mode);
}

#if 0				/* Not used */
int
owner(const char *s)
{
   struct stat         st;

   EDBUG(9, "owner");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (stat(s, &st) < 0)
      EDBUG_RETURN(0);
   EDBUG_RETURN(st.st_uid);
}

int
group(const char *s)
{
   struct stat         st;

   EDBUG(9, "group");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (stat(s, &st) < 0)
      EDBUG_RETURN(0);
   EDBUG_RETURN(st.st_gid);
}
#endif

char               *
username(int uid)
{
   char               *s;
   static int          usr_uid = -1;
   static char        *usr_s = NULL;
   struct passwd      *pwd;

   EDBUG(9, "username");
   if (usr_uid < 0)
      usr_uid = getuid();
   if ((uid == usr_uid) && (usr_s))
      EDBUG_RETURN(Estrdup(usr_s));
   pwd = getpwuid(uid);
   if (pwd)
     {
	s = Estrdup(pwd->pw_name);
	if (uid == usr_uid)
	   usr_s = Estrdup(s);
	EDBUG_RETURN(s);
     }
   EDBUG_RETURN(Estrdup("unknown"));
}

char               *
homedir(int uid)
{
   char               *s;
   static int          usr_uid = -1;
   static char        *usr_s = NULL;
   struct passwd      *pwd;

   EDBUG(9, "homedir");
   if (usr_uid < 0)
      usr_uid = getuid();
   if ((uid == usr_uid) && (usr_s))
     {
	EDBUG_RETURN(Estrdup(usr_s));
     }
   pwd = getpwuid(uid);
   if (pwd)
     {
	s = Estrdup(pwd->pw_dir);
	if (uid == usr_uid)
	   usr_s = Estrdup(s);
	EDBUG_RETURN(s);
     }
   EDBUG_RETURN(Estrdup((getenv("TMPDIR") == NULL) ?
			"/tmp" : getenv("TMPDIR")));
}

char               *
usershell(int uid)
{
   char               *s;
   static int          usr_uid = -1;
   static char        *usr_s = NULL;
   struct passwd      *pwd;

   EDBUG(9, "usershell");
   if (usr_uid < 0)
      usr_uid = getuid();
   if ((uid == usr_uid) && (usr_s))
      return Estrdup(usr_s);
   pwd = getpwuid(uid);
   if (pwd)
     {
	if (!pwd->pw_shell)
	   EDBUG_RETURN(Estrdup("/bin/sh"));
	if (strlen(pwd->pw_shell) < 1)
	   EDBUG_RETURN(Estrdup("/bin/sh"));
	if (!(canexec(pwd->pw_shell)))
	   EDBUG_RETURN(Estrdup("/bin/sh"));
	s = Estrdup(pwd->pw_shell);
	if (uid == usr_uid)
	   usr_s = Estrdup(s);
	EDBUG_RETURN(s);
     }
   EDBUG_RETURN(Estrdup("/bin/sh"));
}

char               *
atword(char *s, int num)
{
   int                 cnt, i;

   EDBUG(9, "atword");
   if (!s)
      EDBUG_RETURN(NULL);
   cnt = 0;
   i = 0;

   while (s[i])
     {
	if ((s[i] != ' ') && (s[i] != '\t'))
	  {
	     if (i == 0)
		cnt++;
	     else if ((s[i - 1] == ' ') || (s[i - 1] == '\t'))
		cnt++;
	     if (cnt == num)
		EDBUG_RETURN(&s[i]);
	  }
	i++;
     }
   EDBUG_RETURN(NULL);
}

char               *
atchar(char *s, char c)
{
   int                 i;

   EDBUG(9, "atchar");
   if (!s)
      EDBUG_RETURN(NULL);
   i = 0;
   while (s[i] != 0)
     {
	if (s[i] == c)
	   EDBUG_RETURN(&s[i]);
	i++;
     }
   EDBUG_RETURN(NULL);
}

char               *
getword(char *s, int num)
{

   /* *********FIXME**************
    * This function is broken but it isn't in use so I'll fix it later
    * (DO NOT USE UNTIL FIXED
    */
   int                 cnt, i;
   char               *start, *finish, *ss, *w;
   char               *wd = NULL;

   EDBUG(9, "getword");
   if (!s)
      EDBUG_RETURN(NULL);
   if (!wd)
      EDBUG_RETURN(NULL);
   if (num <= 0)
     {
	*wd = 0;
	EDBUG_RETURN(NULL);
     }
   cnt = 0;
   i = 0;
   start = NULL;
   finish = NULL;
   ss = NULL;
   w = wd;

   while (s[i])
     {
	if ((cnt == num) && ((s[i] == ' ') || (s[i] == '\t')))
	  {
	     finish = &s[i];
	     break;
	  }
	if ((s[i] != ' ') && (s[i] != '\t'))
	  {
	     if (i == 0)
	       {
		  cnt++;
		  if (cnt == num)
		     start = &s[i];
	       }
	     else if ((s[i - 1] == ' ') || (s[i - 1] == '\t'))
	       {
		  cnt++;
		  if (cnt == num)
		     start = &s[i];
	       }
	  }
	i++;
     }
   if (cnt == num)
     {
	if ((start) && (finish))
	  {
	     for (ss = start; ss < finish; ss++)
		*wd++ = *ss;
	  }
	else if (start)
	  {
	     for (ss = start; *ss != 0; ss++)
		*wd++ = *ss;
	  }
	*wd = 0;
     }
   EDBUG_RETURN(wd);
}

void
word(char *s, int num, char *wd)
{
   int                 cnt, i;
   char               *start, *finish, *ss, *w;

   EDBUG(9, "word");
   if (!s)
      EDBUG_RETURN_;
   if (!wd)
      EDBUG_RETURN_;
   if (num <= 0)
     {
	*wd = 0;
	EDBUG_RETURN_;
     }
   cnt = 0;
   i = 0;
   start = NULL;
   finish = NULL;
   ss = NULL;
   w = wd;

   while (s[i])
     {
	if ((cnt == num) && ((s[i] == ' ') || (s[i] == '\t')))
	  {
	     finish = &s[i];
	     break;
	  }
	if ((s[i] != ' ') && (s[i] != '\t'))
	  {
	     if (i == 0)
	       {
		  cnt++;
		  if (cnt == num)
		     start = &s[i];
	       }
	     else if ((s[i - 1] == ' ') || (s[i - 1] == '\t'))
	       {
		  cnt++;
		  if (cnt == num)
		     start = &s[i];
	       }
	  }
	i++;
     }
   if (cnt == num)
     {
	if ((start) && (finish))
	  {
	     for (ss = start; ss < finish; ss++)
		*wd++ = *ss;
	  }
	else if (start)
	  {
	     for (ss = start; *ss != 0; ss++)
		*wd++ = *ss;
	  }
	*wd = 0;
     }
   EDBUG_RETURN_;
}

/* gets word number [num] in the string [s] and copies it into [wd] */
/* wd is NULL terminated. If word [num] does not exist wd = "" */
/* NB: this function now handles quotes so for a line: */
/* Hello to "Welcome sir - may I Help" Shub Foo */
/* Word 1 = Hello */
/* Word 2 = to */
/* Word 3 = Welcome sir - may I Help */
/* Word 4 = Shub */
/* word 5 = Foo */
void
fword(char *s, int num, char *wd)
{
   char               *cur, *start, *end;
   int                 count, inword, inquote, len;

   EDBUG(9, "word");
   if (!s)
      EDBUG_RETURN_;
   if (!wd)
      EDBUG_RETURN_;
   *wd = 0;
   if (num <= 0)
      EDBUG_RETURN_;
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
		  if (isspace(*cur))
		    {
		       end = cur;
		       inword = 0;
		       count++;
		    }
	       }
	  }
	else
	  {
	     if (!isspace(*cur))
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
      EDBUG_RETURN_;
   if (!end)
      end = cur;
   if (end <= start)
      EDBUG_RETURN_;
   len = (int)(end - start);
   if (len > 4000)
      len = 4000;
   if (len > 0)
     {
	strncpy(wd, start, len);
	wd[len] = 0;
     }
   EDBUG_RETURN_;
}

char               *
field(char *s, int fieldno)
{
   char                buf[4096];

   EDBUG(9, "field");
   buf[0] = 0;
   fword(s, fieldno + 1, buf);
   if (buf[0])
     {
	if ((!strcmp(buf, "NULL")) || (!strcmp(buf, "(null)")))
	   EDBUG_RETURN(NULL);
	EDBUG_RETURN(Estrdup(buf));
     }
   EDBUG_RETURN(NULL);
}

int
fillfield(char *s, int fieldno, char *buf)
{
   EDBUG(9, "fillfield");
   if (!buf)
      EDBUG_RETURN(0);
   buf[0] = 0;
   fword(s, fieldno + 1, buf);
   if (buf[0])
     {
	if ((!strcmp(buf, "NULL")) || (!strcmp(buf, "(null)")))
	  {
	     buf[0] = 0;
	     EDBUG_RETURN(0);
	  }
	EDBUG_RETURN(1);
     }
   EDBUG_RETURN(0);
}

int
canread(const char *s)
{
   EDBUG(9, "canread");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);

   if (!(permissions(s) & (S_IRUSR | S_IRGRP | S_IROTH)))
      EDBUG_RETURN(0);

   EDBUG_RETURN(1 + access(s, R_OK));
}

int
canwrite(const char *s)
{
   EDBUG(9, "canwrite");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);

   if (!(permissions(s) & (S_IWUSR | S_IWGRP | S_IWOTH)))
      EDBUG_RETURN(0);

   EDBUG_RETURN(1 + access(s, W_OK));
}

int
canexec(const char *s)
{
   EDBUG(9, "canexec");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);

   if (!(permissions(s) & (S_IXUSR | S_IXGRP | S_IXOTH)))
      EDBUG_RETURN(0);

   EDBUG_RETURN(1 + access(s, X_OK));
}

char               *
fileof(const char *s)
{
   char                ss[1024];
   int                 i, p1, p2;

   EDBUG(9, "fileof");
   i = 0;
   p1 = -1;
   p2 = -1;
   for (i = strlen(s) - 1; i >= 0; i--)
     {
	if ((s[i] == '.') && (p2 < 0) && (p1 < 0))
	   p2 = i - 1;
	if ((s[i] == '/') && (p1 < 0))
	   p1 = i + 1;
     }
   if (p2 < 0)
      p2 = strlen(s) - 1;
   if (p1 < 0)
      p1 = 0;
   if (p2 <= 0)
      EDBUG_RETURN(Estrdup(""));
   for (i = 0; i <= (p2 - p1); i++)
      ss[i] = s[p1 + i];
   ss[i] = 0;
   EDBUG_RETURN(Estrdup(ss));
}

char               *
fullfileof(const char *s)
{
   char                ss[1024];
   int                 i, p1, p2;

   EDBUG(9, "fullfileof");
   i = 0;
   p1 = -1;
   for (i = strlen(s) - 1; i >= 0; i--)
     {
	if ((s[i] == '/') && (p1 < 0))
	   p1 = i + 1;
     }
   if (p1 < 0)
      p1 = 0;
   p2 = strlen(s);
   for (i = 0; i < (p2 - p1); i++)
      ss[i] = s[p1 + i];
   ss[i] = 0;
   EDBUG_RETURN(Estrdup(ss));
}

char               *
pathtoexec(const char *file)
{
   char               *p, *cp, *ep;
   char               *s;
   int                 len, exelen;

   EDBUG(9, "pathtoexec");

   if (isabspath(file))
     {
	if (canexec(file))
	   EDBUG_RETURN(Estrdup(file));
	EDBUG_RETURN(NULL);
     }
   p = getenv("PATH");
   if (!p)
      EDBUG_RETURN(Estrdup(file));
   if (!file)
      EDBUG_RETURN(NULL);

   cp = p;
   exelen = strlen(file);
   while ((ep = strchr(cp, ':')))
     {
	len = ep - cp;
	s = Emalloc(len + 1);
	if (s)
	  {
	     strncpy(s, cp, len);
	     s[len] = 0;
	     s = Erealloc(s, len + 2 + exelen);
	     strcat(s, "/");
	     strcat(s, file);
	     if (canexec(s))
		EDBUG_RETURN(s);
	     Efree(s);
	  }
	cp = ep + 1;
     }
   len = strlen(cp);
   s = Emalloc(len + 1);
   if (s)
     {
	strncpy(s, cp, len);
	s[len] = 0;
	s = Erealloc(s, len + 2 + exelen);
	strcat(s, "/");
	strcat(s, file);
	if (canexec(s))
	   EDBUG_RETURN(s);
	Efree(s);
     }
   EDBUG_RETURN(NULL);
}

char               *
pathtofile(const char *file)
{
   char               *p, *cp, *ep;
   char               *s;
   int                 len, exelen;

   EDBUG(9, "pathtofile");

   if (isabspath(file))
     {
	if (exists(file))
	   EDBUG_RETURN(Estrdup(file));
     }
   p = getenv("PATH");
   if (!p)
      EDBUG_RETURN(Estrdup(file));
   if (!file)
      EDBUG_RETURN(NULL);
   cp = p;
   exelen = strlen(file);
   while ((ep = strchr(cp, ':')))
     {
	len = ep - cp;
	s = Emalloc(len + 1);
	if (s)
	  {
	     strncpy(s, cp, len);
	     s[len] = 0;
	     s = Erealloc(s, len + 2 + exelen);
	     strcat(s, "/");
	     strcat(s, file);
	     if (exists(s))
		EDBUG_RETURN(s);
	     Efree(s);
	  }
	cp = ep + 1;
     }
   len = strlen(cp);
   s = Emalloc(len + 1);
   if (s)
     {
	strncpy(s, cp, len);
	s[len] = 0;
	s = Erealloc(s, len + 2 + exelen);
	strcat(s, "/");
	strcat(s, file);
	if (exists(s))
	   EDBUG_RETURN(s);
	Efree(s);
     }
   EDBUG_RETURN(NULL);
}

int
findLocalizedFile(char *fname)
{
   char               *tmp, *lang, *p[3];
   int                 i;

   if (!(lang = setlocale(LC_MESSAGES, NULL)))
      return 0;

   tmp = Estrdup(fname);
   lang = Estrdup(lang);	/* lang may be in static space, thus it must
				 * * * be duplicated before we change it below */
   p[0] = lang + strlen(lang);
   p[1] = strchr(lang, '.');
   p[2] = strchr(lang, '_');

   for (i = 0; i < 3; i++)
     {
	if (p[i] == NULL)
	   continue;

	*p[i] = '\0';
	sprintf(fname, "%s.%s", tmp, lang);
	if (isfile(fname))
	  {
	     free(tmp);
	     free(lang);
	     return 1;
	  }
     }
   strcpy(fname, tmp);
   free(tmp);
   free(lang);

   return 0;
}

#if 0				/* Not used */
void
rmrf(const char *path)
{
   char                s[FILEPATH_LEN_MAX];

   Esnprintf(s, sizeof(s), "/bin/rm -rf %s", path);
   system(s);
}
#endif
