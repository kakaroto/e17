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
#include "file.h"
#include "util.h"
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

void
Etmp(char *s)
{
   static unsigned int n_calls = 0;

   Esnprintf(s, 1024, "%s/TMP_%d_%d", EDirUser(), getpid(), n_calls++);
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
E_md(const char *s)
{
   if ((!s) || (!*s))
      return;
   mkdir(s, S_IRWXU);
}

void
E_mv(const char *s, const char *ss)
{
   if ((!s) || (!ss) || (!*s) || (!*ss))
      return;
   rename(s, ss);
}

void
E_rm(const char *s)
{
   if ((!s) || (!*s))
      return;
   unlink(s);
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
filedev(const char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return 0;
   if (stat(s, &st) < 0)
      return 0;
   return filedev_map((int)st.st_dev);
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
isabspath(const char *path)
{
   return path[0] == '/';
}

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
