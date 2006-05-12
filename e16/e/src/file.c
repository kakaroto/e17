/*
 * Copyright (C) 2000-2006 Carsten Haitzler, Geoff Harrison and various contributors
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

const char         *
FileExtension(const char *file)
{
   const char         *p;

   p = strrchr(file, '.');
   if (p != NULL)
     {
	return p + 1;
     }
   return "";
}

void
Etmp(char *s)
{
   static unsigned int n_calls = 0;

   Esnprintf(s, 1024, "%s/TMP_%d_%d", EDirUser(), getpid(), n_calls++);
}

void
E_md(const char *s)
{
   if ((!s) || (!*s))
      return;
   mkdir(s, S_IRWXU);
}

int
exists(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   return 1;
}

int
isfile(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   if (S_ISREG(st.st_mode))
      return 1;
   return 0;
}

int
isdir(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   if (S_ISDIR(st.st_mode))
      return 1;
   return 0;
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

   if ((!dir) || (!*dir))
      return NULL;
   dirp = opendir(dir);
   if (!dirp)
     {
	*num = 0;
	return NULL;
     }
   /* count # of entries in dir (worst case) */
   for (dirlen = 0; (dp = readdir(dirp)) != NULL; dirlen++)
      ;
   if (!dirlen)
     {
	closedir(dirp);
	*num = dirlen;
	return NULL;
     }
   names = (char **)Emalloc(dirlen * sizeof(char *));

   if (!names)
      return NULL;

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
   return names;
}

void
E_rm(const char *s)
{
   if ((!s) || (!*s))
      return;
   unlink(s);
}

void
E_mv(const char *s, const char *ss)
{
   if ((!s) || (!ss) || (!*s) || (!*ss))
      return;
   rename(s, ss);
}

#if 0				/* Unused */
void
E_cp(const char *s, const char *ss)
{
   int                 i;
   FILE               *f, *ff;
   unsigned char       buf[1];

   if ((!s) || (!ss) || (!*s) || (!*ss))
      return;
   if (!exists(s))
      return;
   i = filesize(s);
   f = fopen(s, "r");
   if (!f)
      return;
   ff = fopen(ss, "w");
   if (!ff)
     {
	fclose(f);
	return;
     }
   while (fread(buf, 1, 1, f))
      fwrite(buf, 1, 1, ff);
   fclose(f);
   fclose(ff);
}
#endif

time_t
moddate(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   if (st.st_mtime > st.st_ctime)
      return st.st_mtime;
   return st.st_ctime;
}

#if 0				/* Unused */
int
filesize(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   return (int)st.st_size;
}
#endif

int
fileinode(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   return (int)st.st_ino;
}

int
filedev_map(int dev)
{
#ifdef __linux__
   /* device numbers in the anonymous range can't be relied
    * upon, so map them all on a single one */
   switch (dev >> 8)
     {
     default:
	return dev;
     case 0:
     case 144:
     case 145:
     case 146:
	return 1;
     }
#else
   return dev;
#endif
}

int
filedev(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   return filedev_map((int)st.st_dev);
}

#if 0				/* Unused */
char               *
cwd(void)
{
   char               *s;
   char                ss[FILEPATH_LEN_MAX];

   getcwd(ss, FILEPATH_LEN_MAX);
   s = Estrdup(ss);
   return s;
}
#endif

static int
permissions(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   return st.st_mode;
}

#if 0				/* Not used */
int
owner(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   return st.st_uid;
}

int
group(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   return st.st_gid;
}
#endif

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

const char         *
atword(const char *s, int num)
{
   int                 cnt, i;

   if (!s)
      return NULL;
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
		return &s[i];
	  }
	i++;
     }
   return NULL;
}

void
word(const char *s, int num, char *wd)
{
   int                 cnt, i;
   const char         *start, *finish, *ss;

   if (!s)
      return;
   if (!wd)
      return;
   if (num <= 0)
     {
	*wd = 0;
	return;
     }
   cnt = 0;
   i = 0;
   start = NULL;
   finish = NULL;
   ss = NULL;

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
      return;
   if (!end)
      end = cur;
   if (end <= start)
      return;
   len = (int)(end - start);
   if (len > 4000)
      len = 4000;
   if (len > 0)
     {
	strncpy(wd, start, len);
	wd[len] = 0;
     }
}

char               *
field(char *s, int fieldno)
{
   char                buf[4096];

   buf[0] = 0;
   fword(s, fieldno + 1, buf);
   if (buf[0])
     {
	if ((!strcmp(buf, "NULL")) || (!strcmp(buf, "(null)")))
	   return NULL;
	return Estrdup(buf);
     }
   return NULL;
}

int
canread(const char *s)
{
   if ((!s) || (!*s))
      return 0;

   if (!(permissions(s) & (S_IRUSR | S_IRGRP | S_IROTH)))
      return 0;

   return 1 + access(s, R_OK);
}

int
canwrite(const char *s)
{
   if ((!s) || (!*s))
      return 0;

   if (!(permissions(s) & (S_IWUSR | S_IWGRP | S_IWOTH)))
      return 0;

   return 1 + access(s, W_OK);
}

int
canexec(const char *s)
{
   if ((!s) || (!*s))
      return 0;

   if (!(permissions(s) & (S_IXUSR | S_IXGRP | S_IXOTH)))
      return 0;

   return 1 + access(s, X_OK);
}

char               *
fileof(const char *path)
{
   const char         *s1, *s2;

   s1 = strrchr(path, '/');
   s1 = (s1) ? s1 + 1 : path;
   s2 = strrchr(s1, '.');
   if (!s2)
      return Estrdup(s1);

   return Estrndup(s1, s2 - s1);
}

char               *
fullfileof(const char *path)
{
   const char         *s;

   s = strrchr(path, '/');
   return Estrdup((s) ? s + 1 : path);
}

char               *
pathtoexec(const char *file)
{
   char               *p, *cp, *ep;
   char               *s;
   int                 len, exelen;

   if (isabspath(file))
     {
	if (canexec(file))
	   return Estrdup(file);
	return NULL;
     }
   p = getenv("PATH");
   if (!p)
      return Estrdup(file);
   if (!file)
      return NULL;

   cp = p;
   exelen = strlen(file);
   while ((ep = strchr(cp, ':')) != NULL)
     {
	len = ep - cp;
	s = Emalloc(len + 1);
	if (s)
	  {
	     strncpy(s, cp, len);
	     s[len] = 0;
	     s = Erealloc(s, len + 2 + exelen);
	     if (!s)
		return NULL;
	     strcat(s, "/");
	     strcat(s, file);
	     if (canexec(s))
		return s;
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
	if (!s)
	   return NULL;
	strcat(s, "/");
	strcat(s, file);
	if (canexec(s))
	   return s;
	Efree(s);
     }
   return NULL;
}

char               *
pathtofile(const char *file)
{
   char               *p, *cp, *ep;
   char               *s;
   int                 len, exelen;

   if (isabspath(file))
     {
	if (exists(file))
	   return Estrdup(file);
     }
   p = getenv("PATH");
   if (!p)
      return Estrdup(file);
   if (!file)
      return NULL;
   cp = p;
   exelen = strlen(file);
   while ((ep = strchr(cp, ':')) != NULL)
     {
	len = ep - cp;
	s = Emalloc(len + 1);
	if (s)
	  {
	     strncpy(s, cp, len);
	     s[len] = 0;
	     s = Erealloc(s, len + 2 + exelen);
	     if (!s)
		return NULL;
	     strcat(s, "/");
	     strcat(s, file);
	     if (exists(s))
		return s;
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
	if (!s)
	   return NULL;
	strcat(s, "/");
	strcat(s, file);
	if (exists(s))
	   return s;
	Efree(s);
     }
   return NULL;
}
