/*
 *  Copyright (C) 2000-2004 Carsten Haitzler, Geoff Harrison and various contributors
 *  
 *  Permission is hereby granted, free of charge, to any person obtaining a copy
 *  of this software and associated documentation files (the "Software"), to
 *  deal in the Software without restriction, including without limitation the
 *  rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 *  sell copies of the Software, and to permit persons to whom the Software is
 *  furnished to do so, subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in
 *  all copies of the Software, its documentation and marketing & publicity
 *  materials, and acknowledgment shall be given in the documentation, materials
 *  and software packages that this Software was used.
 *  
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 *  THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *  */

#include "dox.h"
#ifdef HAVE_WCTYPE_H
# include <wctype.h>
#endif

char               *
FileExtension(char *file)
{
   char               *p;

   p = strrchr(file, '.');
   if (p != NULL)
     {
	return (p + 1);
     }
   return ("");
}

void
md(char *s)
{
   if ((!s) || (!*s))
      return;
   mkdir(s, S_IRWXU);
   return;
}

int
exists(char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return (0);
   if (stat(s, &st) < 0)
      return (0);
   return (1);
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

   if ((!s) || (!*s))
      return (0);
   if (stat(s, &st) < 0)
      return (0);
   if (S_ISREG(st.st_mode))
      return (1);
   return (0);
}

int
isdir(char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return (0);
   if (stat(s, &st) < 0)
      return (0);
   if (S_ISDIR(st.st_mode))
      return (1);
   return (0);
}

char              **
ls(char *dir, int *num)
{
   int                 i, dirlen;
   int                 done = 0;
   DIR                *dirp;
   char              **names;
   struct dirent      *dp;

   if ((!dir) || (!*dir))
      return (0);
   dirp = opendir(dir);
   if (!dirp)
     {
	*num = 0;
	return (NULL);
     }
   /* count # of entries in dir (worst case) */
   for (dirlen = 0; (dp = readdir(dirp)) != NULL; dirlen++);
   if (!dirlen)
     {
	closedir(dirp);
	*num = dirlen;
	return (NULL);
     }
   names = (char **)malloc(dirlen * sizeof(char *));

   if (!names)
      return (NULL);

   rewinddir(dirp);
   for (i = 0; i < dirlen;)
     {
	dp = readdir(dirp);
	if (!dp)
	   break;
	if ((strcmp(dp->d_name, ".")) && (strcmp(dp->d_name, "..")))
	  {
	     names[i] = strdup(dp->d_name);
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
   return (names);
}

void
freestrlist(char **l, int num)
{
   if (!l)
      return;
   while (num--)
      if (l[num])
	 free(l[num]);
   free(l);
   return;
}

void
rm(char *s)
{
   if ((!s) || (!*s))
      return;
   unlink(s);
   return;
}

void
mv(char *s, char *ss)
{
   if ((!s) || (!ss) || (!*s) || (!*ss))
      return;
   rename(s, ss);
   return;
}

void
cp(char *s, char *ss)
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
   return;
}

time_t
moddate(char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return (0);
   if (!stat(s, &st) < 0)
      return (0);
   if (st.st_mtime > st.st_ctime)
     {
	return (st.st_mtime);
     }
   else
      return (st.st_ctime);
   return (0);
}

int
filesize(char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return (0);
   if (stat(s, &st) < 0)
      return (0);
   return ((int)st.st_size);
}

int
fileinode(char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return (0);
   if (stat(s, &st) < 0)
      return (0);
   return ((int)st.st_ino);
}

int
filedev(char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return (0);
   if (stat(s, &st) < 0)
      return (0);
   return ((int)st.st_dev);
}

void
cd(char *s)
{
   if ((!s) || (!*s))
      return;
   chdir(s);
   return;
}

char               *
cwd(void)
{
   char               *s;
   char                ss[FILEPATH_LEN_MAX];

   getcwd(ss, FILEPATH_LEN_MAX);
   s = strdup(ss);
   return (s);
}

int
permissions(char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return (0);
   if (!stat(s, &st) < 0)
      return (0);
   return (st.st_mode);
}

int
owner(char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return (0);
   if (!stat(s, &st) < 0)
      return (0);
   return (st.st_uid);
}

int
group(char *s)
{
   struct stat         st;

   if ((!s) || (!*s))
      return (0);
   if (!stat(s, &st) < 0)
      return (0);
   return (st.st_gid);
}

char               *
username(int uid)
{
   static int          usr_uid = -1;
   static char        *usr_s = NULL;
   char               *s;
   struct passwd      *pwd;

   if (usr_uid < 0)
      usr_uid = getuid();
   if ((uid == usr_uid) && (usr_s))
      return strdup(usr_s);
   pwd = getpwuid(uid);
   if (pwd)
     {
	s = strdup(pwd->pw_name);
	if (uid == usr_uid)
	   usr_s = strdup(s);
	return (s);
     }
   return (strdup("unknown"));
}

char               *
homedir(int uid)
{
   static int          usr_uid = -1;
   static char        *usr_s = NULL;
   char               *s;
   struct passwd      *pwd;

   if (usr_uid < 0)
      usr_uid = getuid();
   if ((uid == usr_uid) && (usr_s))
      return strdup(usr_s);
   pwd = getpwuid(uid);
   if (pwd)
     {
	s = strdup(pwd->pw_dir);
	if (uid == usr_uid)
	   usr_s = strdup(s);
	return (s);
     }
   return (strdup("/tmp"));
}

char               *
usershell(int uid)
{
   static int          usr_uid = -1;
   static char        *usr_s = NULL;
   char               *s;
   struct passwd      *pwd;

   if (usr_uid < 0)
      usr_uid = getuid();
   if ((uid == usr_uid) && (usr_s))
      return strdup(usr_s);
   pwd = getpwuid(uid);
   if (pwd)
     {
	s = strdup(pwd->pw_shell);
	if (uid == usr_uid)
	   usr_s = strdup(s);
	return (s);
     }
   return (strdup("/bin/sh"));
}

char               *
atword(char *s, int num)
{
   int                 cnt, i;

   if (!s)
      return (NULL);
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
		return (&s[i]);
	  }
	i++;
     }
   return (NULL);
}

char               *
atchar(char *s, char c)
{
   int                 i;

   if (!s)
      return (NULL);
   i = 0;
   while (s[i] != 0)
     {
	if (s[i] == c)
	   return (&s[i]);
	i++;
     }
   return (NULL);
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

   if (!s)
      return (NULL);
   if (!wd)
      return (NULL);
   if (num <= 0)
     {
	*wd = 0;
	return (NULL);
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
   return (wd);
}

void
word(char *s, int num, char *wd)
{
   int                 cnt, i;
   char               *start, *finish, *ss, *w;

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
   return;
}

#ifdef HAVE_WCTYPE_H
void
word_mb(char *s, int num, char *wd, int *spaceflag)
{
   int                 cnt, i;
   char               *start, *finish, *ss, *w;

   int                 wcflg, mbflg;
   struct char_class
   {
      char               *name;
      wctype_t            wt;
   }                  *cc, char_class_tbl[] =
   {
#ifdef linux
      /* Will be supported on glibc 2.1.3 or later */
      {
      "jspace", 0},
      {
      "jhira", 0},
      {
      "jkata", 0},
      {
      "jkanji", 0},
      {
      "jdigit", 0},		/* Japanese */
      {
      "hangul", 0},
      {
      "hanja", 0},		/* Korean    */
	 /* {"?????"}, {"?????"}, *//* Chinese   */
#endif
#ifdef sgi
	 /* SGI IRIX (Japanese, Chinese, Korean, etc..) */
      {
      "special", 0},
      {
      "phonogram", 0},
      {
      "ideogram", 0},
#endif
#ifdef sun
	 /* {"?????"}, {"?????"}, */
#endif
#ifdef hpux
	 /* {"?????"}, {"?????"}, */
#endif
      {
      NULL, 0}
   };

   if (!s)
      return;
   if (!wd)
      return;
   if (num <= 0)
     {
	*wd = 0;
	return;
     }

   /*  Check multibyte character class is available or not */
   wcflg = 0;
   for (cc = char_class_tbl; cc->name != NULL; cc++)
     {
	cc->wt = wctype(cc->name);
	if (cc->wt != (wctype_t) 0)
	   wcflg = 1;
     }

   cnt = 0;
   i = 0;
   start = NULL;
   finish = NULL;
   ss = NULL;
   w = wd;
   *spaceflag = 0;

   while (s[i])
     {
	int                 len, oldflg = 1;

	len = mblen(s + i, MB_CUR_MAX);
	if (len < 0)
	  {
	     i++;
	     continue;
	  }

	/*  Check multibyte character class */
	if (wcflg)
	  {
	     wchar_t             wc;

	     mbflg = 1;
	     if ((mbtowc(&wc, s + i, strlen(s + i))) != -1)
	       {
		  for (cc = char_class_tbl; cc->name != NULL; cc++)
		    {
		       if (cc->wt == (wctype_t) 0)
			  continue;

		       if (iswctype(wc, cc->wt) != 0)
			 {
			    mbflg = 2;
			    break;
			 }
		    }
	       }
	  }
	else
	   mbflg = len;

	if ((cnt == num) && ((s[i] == ' ') || (s[i] == '\t') ||
			     (oldflg != mbflg) || (mbflg > 1)))
	  {
	     finish = &s[i];
	     break;
	  }

	if ((s[i] != ' ') && (s[i] != '\t'))
	  {
	     if ((i == 0) ||
		 (s[i - 1] == ' ') || (s[i - 1] == '\t') ||
		 ((oldflg > 1) && (mbflg > 1)) || (oldflg != mbflg))
	       {
		  cnt++;
		  if (cnt == num)
		    {
		       start = &s[i];
		       if ((s[i - 1] == ' ') || (s[i - 1] == '\t'))
			  *spaceflag = 1;
		    }
	       }
	  }
	i += len;
	oldflg = mbflg;
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
   return;
}
#endif

int
canread(char *s)
{
   if ((!s) || (!*s))
      return (0);

   if (!(permissions(s) & (S_IRUSR | S_IRGRP | S_IROTH)))
      return (0);

   return (1 + access(s, R_OK));
}

int
canwrite(char *s)
{
   if ((!s) || (!*s))
      return (0);

   if (!(permissions(s) & (S_IWUSR | S_IWGRP | S_IWOTH)))
      return (0);

   return (1 + access(s, W_OK));
}

int
canexec(char *s)
{
   if ((!s) || (!*s))
      return (0);

   if (!(permissions(s) & (S_IXUSR | S_IXGRP | S_IXOTH)))
      return (0);

   return (1 + access(s, X_OK));
}

char               *
fileof(char *s)
{
   char                ss[1024];
   int                 i, p1, p2;

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
   return (strdup(ss));
}

char               *
fullfileof(char *s)
{
   char                ss[1024];
   int                 i, p1, p2;

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
   return (strdup(ss));
}

char               *
pathtoexec(char *file)
{
   char               *p, *cp, *ep;
   char               *s;
   int                 len, exelen;

   if (file[0] == '/')
     {
	if (canexec(file))
	   return (strdup(file));
     }
   p = getenv("PATH");
   if (!p)
      return (strdup(file));
   if (!file)
      return (NULL);
   cp = p;
   exelen = strlen(file);
   while ((ep = strchr(cp, ':')))
     {
	len = ep - cp;
	s = malloc(len + 1);
	if (s)
	  {
	     strncpy(s, cp, len);
	     s[len] = 0;
	     s = realloc(s, len + 2 + exelen);
	     strcat(s, "/");
	     strcat(s, file);
	     if (canexec(s))
		return (s);
	     free(s);
	  }
	cp = ep + 1;
     }
   len = strlen(cp);
   s = malloc(len + 1);
   if (s)
     {
	strncpy(s, cp, len);
	s[len] = 0;
	s = realloc(s, len + 2 + exelen);
	strcat(s, "/");
	strcat(s, file);
	if (canexec(s))
	   return (s);
	free(s);
     }
   return (NULL);
}

char               *
pathtofile(char *file)
{
   char               *p, *cp, *ep;
   char               *s;
   int                 len, exelen;

   if (file[0] == '/')
     {
	if (exists(file))
	   return (strdup(file));
     }
   p = getenv("PATH");
   if (!p)
      return (strdup(file));
   if (!file)
      return (NULL);
   cp = p;
   exelen = strlen(file);
   while ((ep = strchr(cp, ':')))
     {
	len = ep - cp;
	s = malloc(len + 1);
	if (s)
	  {
	     strncpy(s, cp, len);
	     s[len] = 0;
	     s = realloc(s, len + 2 + exelen);
	     strcat(s, "/");
	     strcat(s, file);
	     if (exists(s))
		return (s);
	     free(s);
	  }
	cp = ep + 1;
     }
   len = strlen(cp);
   s = malloc(len + 1);
   if (s)
     {
	strncpy(s, cp, len);
	s[len] = 0;
	s = realloc(s, len + 2 + exelen);
	strcat(s, "/");
	strcat(s, file);
	if (exists(s))
	   return (s);
	free(s);
     }
   return (NULL);
}

int
findLocalizedFile(char *fname)
{
   char               *tmp, *lang, *p[3];
   int                 i;

   if (!(lang = setlocale(LC_MESSAGES, NULL)))
      return 0;

   tmp = strdup(fname);
   lang = strdup(lang);		/* lang may be in static space, thus it must
				 * be duplicated before we change it below */
   p[0] = lang + strlen(lang);
   p[1] = strchr(lang, '.');
   p[2] = strchr(lang, '_');

   for (i = 0; i < 3; i++)
     {
	if (p[i] == NULL)
	   continue;

	*p[i] = '\0';
	sprintf(fname, "%s.%s", tmp, lang);
	if (exists(fname))
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
