/*
 Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to
 deal in the Software without restriction, including without limitation the
 rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 sell copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies of the Software, its documentation and marketing & publicity
 materials, and acknowledgment shall be given in the documentation, materials
 and software packages that this Software was used.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "E.h"

void
md(char *s)
{
   EDBUG(9, "md");
   if ((!s) || (!*s))
      EDBUG_RETURN_;
   mkdir(s, S_IRWXU);
   EDBUG_RETURN_;
}

int
exists(char *s)
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
mkdirs(char *s)
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
		md(ss);
	     else if (!isdir(ss))
		return;
	  }
	i++;
     }
}

int
isfile(char *s)
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
isdir(char *s)
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

char              **
ls(char *dir, int *num)
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
   /* load up the entries, now that we know how many to make */
   names = (char **)Emalloc(dirlen * sizeof(char *));

   if (!names)
      EDBUG_RETURN(NULL);

   rewinddir(dirp);
   for (i = 0; i < dirlen;)
     {
	dp = readdir(dirp);
	if (!dp)
	   break;
	names[i] = (char *)Emalloc(strlen(dp->d_name) + 1);
	if (!names)
	   EDBUG_RETURN(NULL);
	strcpy(names[i], dp->d_name);
	i++;
     }

   if (i < dirlen)
      dirlen = i;		/* dir got shorter... */
   closedir(dirp);
   *num = dirlen;
   /* do a simple bubble sort here to alphanumberic it */
   while (!done)
     {
	done = 1;
	for (i = 0; i < dirlen - 2; i++)
	  {
	     if (strcmp(names[i], names[i + 1]) < 0)
	       {
		  char               *temp;

		  temp = names[i];
		  names[i] = names[i + 1];
		  names[i + 1] = temp;
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
rm(char *s)
{
   EDBUG(9, "rm");
   if ((!s) || (!*s))
      EDBUG_RETURN_;
   unlink(s);
   EDBUG_RETURN_;
}

void
mv(char *s, char *ss)
{
   EDBUG(9, "mv");
   if ((!s) || (!ss) || (!*s) || (!*ss))
      EDBUG_RETURN_;
   rename(s, ss);
   EDBUG_RETURN_;
}

void
cp(char *s, char *ss)
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
moddate(char *s)
{
   struct stat         st;

   EDBUG(9, "moddate");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (!stat(s, &st) < 0)
      EDBUG_RETURN(0);
   if (st.st_mtime > st.st_ctime)
     {
	EDBUG_RETURN(st.st_mtime);
     }
   else
      EDBUG_RETURN(st.st_ctime);
   EDBUG_RETURN(0);
}

int
filesize(char *s)
{
   struct stat         st;

   EDBUG(9, "filesize");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (stat(s, &st) < 0)
      EDBUG_RETURN(0);
   EDBUG_RETURN((int)st.st_size);
}

void
cd(char *s)
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
   s = duplicate(ss);
   EDBUG_RETURN(s);
}

int
permissions(char *s)
{
   struct stat         st;

   EDBUG(9, "permissions");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (!stat(s, &st) < 0)
      EDBUG_RETURN(0);
   EDBUG_RETURN(st.st_mode);
}

int
owner(char *s)
{
   struct stat         st;

   EDBUG(9, "owner");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (!stat(s, &st) < 0)
      EDBUG_RETURN(0);
   EDBUG_RETURN(st.st_uid);
}

int
group(char *s)
{
   struct stat         st;

   EDBUG(9, "group");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   if (!stat(s, &st) < 0)
      EDBUG_RETURN(0);
   EDBUG_RETURN(st.st_gid);
}

char               *
username(int uid)
{
   char               *s;

#ifndef __EMX__
   struct passwd      *pwd;

   EDBUG(9, "username");
   pwd = getpwuid(uid);
   if (pwd)
     {
	s = duplicate(pwd->pw_name);
/*      Efree(pwd); */
	EDBUG_RETURN(s);
     }
#else
   if ((s = getenv("USER")) != NULL)
      EDBUG_RETURN(duplicate(s));
#endif
   EDBUG_RETURN(duplicate("unknown"));
}

char               *
homedir(int uid)
{
   char               *s;

#ifndef __EMX__
   struct passwd      *pwd;

   EDBUG(9, "homedir");
   pwd = getpwuid(uid);
   if (pwd)
     {
	s = duplicate(pwd->pw_dir);
/*      Efree(pwd); */
	EDBUG_RETURN(s);
     }
#else
   if ((s = getenv("HOME")) != NULL)
      EDBUG_RETURN(duplicate(s));
   if ((s = getenv("TMP")) != NULL)
      EDBUG_RETURN(duplicate(s));
#endif
   EDBUG_RETURN(duplicate("/tmp"));
}

char               *
usershell(int uid)
{
#ifndef __EMX__
   char               *s;
   struct passwd      *pwd;

   EDBUG(9, "usershell");
   pwd = getpwuid(uid);
   if (pwd)
     {
	s = duplicate(pwd->pw_shell);
/*      Efree(pwd); */
	EDBUG_RETURN(s);
     }
   EDBUG_RETURN(duplicate("/bin/sh"));
#else
   EDBUG_RETURN(duplicate("sh.exe"));
#endif
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

int
canread(char *s)
{
   EDBUG(9, "canread");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   EDBUG_RETURN(1 + access(s, R_OK));
}

int
canwrite(char *s)
{
   EDBUG(9, "canwrite");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   EDBUG_RETURN(1 + access(s, W_OK));
}

int
canexec(char *s)
{
   EDBUG(9, "canexec");
   if ((!s) || (!*s))
      EDBUG_RETURN(0);
   EDBUG_RETURN(1 + access(s, X_OK));
}

char               *
fileof(char *s)
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
	   p2 = i;
	if ((s[i] == '/') && (p1 < 0))
	   p1 = i;
     }
   if (p2 < 0)
      p2 = strlen(s);
   if (p1 < 0)
      p1 = 0;
   for (i = 0; i < (p2 - p1 - 1); i++)
      ss[i] = s[p1 + 1 + i];
   ss[i] = 0;
   EDBUG_RETURN(duplicate(ss));
}

char               *
fullfileof(char *s)
{
   char                ss[1024];
   int                 i, p1, p2;

   EDBUG(9, "fullfileof");
   i = 0;
   p1 = -1;
   for (i = strlen(s) - 1; i >= 0; i--)
     {
	if ((s[i] == '/') && (p1 < 0))
	   p1 = i;
     }
   p2 = strlen(s);
   for (i = 0; i < (p2 - p1 - 1); i++)
      ss[i] = s[p1 + 1 + i];
   ss[i] = 0;
   EDBUG_RETURN(duplicate(ss));
}

char               *
pathtoexec(char *file)
{
   char               *p, *cp, *ep;
   char               *s;
   int                 len, exelen;

   EDBUG(9, "pathtoexec");
#ifndef __EMX__
   if (file[0] == '/')
#else
   if (_fnisabs(file))
#endif
     {
	if (canexec(file))
	   EDBUG_RETURN(duplicate(file));
     }
   p = getenv("PATH");
   if (!p)
      EDBUG_RETURN(duplicate(file));
   if (!file)
      EDBUG_RETURN(NULL);
   cp = p;
   exelen = strlen(file);
#ifndef __EMX__
   while ((ep = strchr(cp, ':')))
#else
   while ((ep = strchr(cp, ';')))
#endif
     {
	len = ep - cp;
	s = Emalloc(len + 1);
	if (s)
	  {
	     strncpy(s, cp, len);
	     s[len] = 0;
	     s = Erealloc(s, len + 2 + exelen);
#ifdef __EMX__
	     if (s[len - 1] != '/')
#endif
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
#ifdef __EMX__
	if (s[len - 1] != '/')
#endif
	   strcat(s, "/");
	strcat(s, file);
	if (canexec(s))
	   EDBUG_RETURN(s);
	Efree(s);
     }
   EDBUG_RETURN(NULL);
}

char               *
pathtofile(char *file)
{
   char               *p, *cp, *ep;
   char               *s;
   int                 len, exelen;

   EDBUG(9, "pathtofile");
#ifndef __EMX__
   if (file[0] == '/')
#else
   if (_fnisabs(file))
#endif
     {
	if (exists(file))
	   EDBUG_RETURN(duplicate(file));
     }
   p = getenv("PATH");
   if (!p)
      EDBUG_RETURN(duplicate(file));
   if (!file)
      EDBUG_RETURN(NULL);
   cp = p;
   exelen = strlen(file);
#ifndef __EMX__
   while ((ep = strchr(cp, ':')))
#else
   while ((ep = strchr(cp, ';')))
#endif
     {
	len = ep - cp;
	s = Emalloc(len + 1);
	if (s)
	  {
	     strncpy(s, cp, len);
	     s[len] = 0;
	     s = Erealloc(s, len + 2 + exelen);
#ifdef __EMX__
	     if (s[len - 1] != '/')
#endif
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
#ifdef __EMX__
	if (s[len - 1] != '/')
#endif
	   strcat(s, "/");
	strcat(s, file);
	if (exists(s))
	   EDBUG_RETURN(s);
	Efree(s);
     }
   EDBUG_RETURN(NULL);
}
