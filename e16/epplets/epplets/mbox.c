/*
 * Copyright (C) 1996-8 Michael R. Elkins <me@cs.hmc.edu>
 * 
 *     This program is free software; you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation; either version 2 of the License, or
 *     (at your option) any later version.
 * 
 *     This program is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 * 
 *     You should have received a copy of the GNU General Public License
 *     along with this program; if not, write to the Free Software
 *     Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* Everything in this file was "borrowed" from gbuffy (by Brandon Long et al.) who
   "borrowed" from mutt.  I take less than no credit for this code, and thanks to
   the above people for their excellent work. -- mej */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <pwd.h>
#include <dirent.h>
#include <ctype.h>
#include <string.h>
#include <time.h>
#include <utime.h>
#include <malloc.h>
#include <limits.h>

#define ISSPACE(c) isspace((unsigned char) c)
#define SKIPWS(c) while (*(c) && isspace((unsigned char) *(c))) c++;
#define strfcpy(A,B,C) strncpy(A,B,C), *(A+(C)-1)=0
#ifndef TRUE
#  define TRUE (1)
#endif
#ifndef FALSE
#  define FALSE (0)
#endif
#define BUFFSIZE 256
#ifndef _POSIX_PATH_MAX
#  define _POSIX_PATH_MAX 255
#endif
#define NONULL(x)  ((x) ? (x) : "")

#if 0
#  define D(x) do {printf("%10s | %7d:  [debug] ", __FILE__, __LINE__); printf x; fflush(stdout);} while (0)
#else
#  define D(x) ((void) 0)
#endif

int                 ebiff_utimes(const char *file, struct timeval tvp[2]);
void                safe_free(void **p);
char               *safe_strdup(const char *s);
void                safe_realloc(void **p, size_t siz);
void               *safe_calloc(size_t nmemb, size_t size);
char               *read_rfc822_line(FILE * f, char *line, size_t * linelen);
static const char  *next_word(const char *s);
int                 check_month(const char *s);
static int          is_day_name(const char *s);
int                 is_from(const char *s, char *path, size_t pathlen);
int                 parse_mime_header(FILE * fp);
int                 mbox_folder_count(char *path, int force);
int                 maildir_folder_count(char *path, int force);

const char         *Weekdays[] =
   { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
const char         *Months[] =
   { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct",
   "Nov", "Dec", "ERR"
};

extern unsigned long new_cnt, total_cnt;
extern size_t       file_size;
extern time_t       file_mtime;

int
ebiff_utimes(const char *file, struct timeval tvp[2])
{
   struct utimbuf      ut;
   time_t              now;

   now = time((time_t *) NULL);
   if (tvp == (struct timeval *)NULL)
     {
	ut.actime = now;
	ut.modtime = now;
     }
   else
     {
	ut.actime = tvp[0].tv_sec;
	ut.modtime = tvp[1].tv_sec;
     }
   return (utime(file, &ut));
}

void
safe_free(void **p)
{
   if (*p)
     {
	free(*p);
	*p = 0;
     }
}

char               *
safe_strdup(const char *s)
{
   char               *p;
   size_t              l;

   if (!s || !*s)
      return 0;
   l = strlen(s) + 1;
   p = (char *)malloc(l);
   memcpy(p, s, l);
   return (p);
}

void
safe_realloc(void **p, size_t siz)
{
   void               *r;

   if (siz == 0)
     {
	if (*p)
	  {
	     free(*p);
	     *p = NULL;
	  }
	return;
     }
   if (*p)
      r = (void *)realloc(*p, siz);
   else
     {
	/* realloc(NULL, nbytes) doesn't seem to work under SunOS 4.1.x */
	r = (void *)malloc(siz);
     }
   *p = r;
}

void               *
safe_calloc(size_t nmemb, size_t size)
{
   void               *p;

   if (!nmemb || !size)
      return NULL;
   p = (void *)calloc(nmemb, size);
   return p;
}

/* Reads an arbitrarily long header field, and looks ahead for continuation
 * lines.  ``line'' must point to a dynamically allocated string; it is
 * increased if more space is required to fit the whole line.
 */
char               *
read_rfc822_line(FILE * f, char *line, size_t * linelen)
{
   char               *buf = line;
   char                ch;
   size_t              offset = 0;

   for (; 1;)
     {
	if (fgets(buf, *linelen - offset, f) == NULL ||	/* end of file or */
	    (ISSPACE(*line) && !offset))
	  {			/* end of headers */
	     *line = 0;
	     return (line);
	  }
	buf += strlen(buf) - 1;
	if (*buf == '\n')
	  {
	     /* we did get a full line. remove trailing space */
	     while (ISSPACE(*buf))
		*buf-- = 0;	/* we cannot come beyond line's beginning because
				 * it begins with a non-space */

	     /* check to see if the next line is a continuation line */
	     if ((ch = fgetc(f)) != ' ' && ch != '\t')
	       {
		  ungetc(ch, f);
		  return (line);	/* next line is a separate header field or EOH */
	       }
	     /* eat tabs and spaces from the beginning of the continuation line */
	     while ((ch = fgetc(f)) == ' ' || ch == '\t');
	     ungetc(ch, f);
	     *++buf = ' ';	/* string is still terminated because we removed
				 * at least one whitespace char above */
	  }
	buf++;
	offset = buf - line;
	if (*linelen < offset + BUFFSIZE)
	  {
	     /* grow the buffer */
	     *linelen += BUFFSIZE;
	     safe_realloc((void **)&line, *linelen);
	     buf = line + offset;
	  }
     }
   /* not reached */
}

static const char  *
next_word(const char *s)
{
   while (*s && !ISSPACE(*s))
      s++;
   SKIPWS(s);
   return s;
}

int
check_month(const char *s)
{
   int                 i;

   for (i = 0; i < 12; i++)
      if (strncasecmp(s, Months[i], 3) == 0)
	 return (i);
   return (-1);			/* error */
}

static int
is_day_name(const char *s)
{
   int                 i;

   if (!ISSPACE(*(s + 3)))
      return 0;
   for (i = 0; i < 7; i++)
      if (strncasecmp(s, Weekdays[i], 3) == 0)
	 return 1;
   return 0;
}

/*
 * A valid message separator looks like:
 *
 * From [ <return-path> ] <weekday> <month> <day> <time> [ <timezone> ] <year>
 */
int
is_from(const char *s, char *path, size_t pathlen)
{
   struct tm           tm;
   int                 yr;

   if (path)
      *path = 0;

   if (strncmp("From ", s, 5) != 0)
      return 0;

   s = next_word(s);		/* skip over the From part. */
   if (!*s)
      return 0;

   if (!is_day_name(s))
     {
	const char         *p;
	size_t              len;

	/* looks like we got the return-path, so extract it  */
	if (*s == '"')
	  {
	     /* sometimes we see bogus addresses like
	      *      From "/foo/bar baz/"@dumbdar.com Sat Nov 22 15:29:32 PST 1997
	      */
	     p = s;
	     p++;		/* skip over the quote */
	     do
	       {
		  if (!(p = strpbrk(p, "\\\"")))
		     return 0;
		  if (*p == '\\')
		     p += 2;
	       }
	     while (*p != '"');
	     while (*p && !ISSPACE(*p))
		p++;
	  }
	else
	  {
	     if ((p = strchr(s, ' ')) == NULL)
		return 0;
	  }
	if (path)
	  {
	     len = (size_t) (p - s);
	     if (len + 1 > pathlen)
		len = pathlen - 1;
	     memcpy(path, s, len);
	     path[len] = 0;
	  }
	s = p + 1;
	SKIPWS(s);
	if (!*s)
	   return 0;

	if (!is_day_name(s))
	  {
	     return 0;
	  }
     }
   s = next_word(s);
   if (!*s)
      return 0;

   /* do a quick check to make sure that this isn't really the day of the week.
    * this could happen when receiving mail from a local user whose login name
    * is the same as a three-letter abbreviation of the day of the week.
    */
   if (is_day_name(s))
     {
	s = next_word(s);
	if (!*s)
	   return 0;
     }
   /* now we should be on the month. */
   if ((tm.tm_mon = check_month(s)) < 0)
      return 0;

   /* day */
   s = next_word(s);
   if (!*s)
      return 0;
   if (sscanf(s, "%d", &tm.tm_mday) != 1)
      return 0;

   /* time */
   s = next_word(s);
   if (!*s)
      return 0;

   /* Accept either HH:MM or HH:MM:SS */
   if (sscanf(s, "%d:%d:%d", &tm.tm_hour, &tm.tm_min, &tm.tm_sec) == 3);
   else if (sscanf(s, "%d:%d", &tm.tm_hour, &tm.tm_min) == 2)
      tm.tm_sec = 0;
   else
      return 0;

   s = next_word(s);
   if (!*s)
      return 0;

   /* timezone? */
   if (isalpha((unsigned char)*s) || *s == '+' || *s == '-')
     {
	s = next_word(s);
	if (!*s)
	   return 0;

	/*
	 * some places have two timezone fields after the time, e.g.
	 *      From xxxx@yyyyyyy.fr Wed Aug  2 00:39:12 MET DST 1995
	 */
	if (isalpha((unsigned char)*s))
	  {
	     s = next_word(s);
	     if (!*s)
		return 0;
	  }
     }
   /* year */
   if (sscanf(s, "%d", &yr) != 1)
      return 0;
   tm.tm_year = yr > 1900 ? yr - 1900 : yr;

   tm.tm_isdst = 0;
   tm.tm_yday = 0;
   tm.tm_wday = 0;

/*  return (mutt_mktime (&tm, 0)); */
   return 1;
}

int
parse_mime_header(FILE * fp)
{
   static char        *buffer = NULL;
   static size_t       buflen = BUFFSIZE;
   int                 status = FALSE;
   int                 is_new = FALSE;

   if (buffer == NULL)
      buffer = (char *)malloc(buflen);

   while (*(buffer = read_rfc822_line(fp, buffer, &buflen)) != 0)
     {
	if (!strncmp(buffer, "Status:", 7))
	  {
	     status = TRUE;
	     if (!strchr(buffer, 'R') && !strchr(buffer, 'O'))
	       {
		  new_cnt++;
		  is_new = TRUE;
	       }
	  }
     }
   if (status == FALSE)
     {
	new_cnt++;
	is_new = TRUE;
     }
   else
     {
	status = FALSE;
     }
   return 0;
}

/*
 * Return 0 on no change/failure, 1 on change
 */
int
mail_folder_count(char *path, int force)
{
   struct stat         s;

   D(("mail_folder_count(%s, %d) called.\n", NONULL(path), force));

   if (stat(path, &s) != 0)
     {
	D((" -> Unable to stat mailbox.\n"));
	file_size = 0;
	file_mtime = 0;
	return 0;
     }

   if (S_ISDIR(s.st_mode))
     {
	/* Assume maildir */
	return maildir_folder_count(path, force);
     }
   else
     {
	/* Assume mbox */
	return mbox_folder_count(path, force);
     }
}

/*
 * Return 0 on no change/failure, 1 on change
 */
int
mbox_folder_count(char *path, int force)
{
   FILE               *fp;
   char                buffer[BUFFSIZE];
   char                garbage[BUFFSIZE];
   struct stat         s;
   struct timeval      t[2];

   D(("mbox_folder_count(%s, %d) called.\n", NONULL(path), force));

   if (path == NULL)
      return 0;

   if (stat(path, &s) != 0)
     {
	D((" -> Unable to stat mailbox.\n"));
	file_size = 0;
	file_mtime = 0;
	return 0;
     }
   if (!force && ((size_t) s.st_size == file_size)
       && (s.st_mtime == file_mtime))
     {
	D((" -> Mailbox unchanged.\n"));
	return 0;
     }

   if ((s.st_size == 0) || (!S_ISREG(s.st_mode)))
     {
	D((" -> Mailbox has zero size or is not a regular file.\n"));
	if (file_size == 0 && file_mtime == 0 && total_cnt == 0)
	  {
	     return 0;
	  }
	else
	  {
	     file_size = 0;
	     file_mtime = 0;
	     total_cnt = 0;
	     new_cnt = 0;
	     return 1;
	  }
     }
   if ((fp = fopen(path, "r")) == NULL)
     {
	D((" -> Mailbox cannot be opened for reading.\n"));
	return 0;
     }

   /* Check if a folder by checking for From as first thing in file */
   fgets(buffer, sizeof(buffer), fp);
   if (!is_from(buffer, garbage, sizeof(garbage)))
     {
	D((" -> Mailbox does not appear to be in mbox format.\n"));
	return 0;
     }

   total_cnt = 1;
   new_cnt = 0;
   file_mtime = s.st_mtime;
   file_size = s.st_size;

   parse_mime_header(fp);
   while (fgets(buffer, sizeof(buffer), fp) != 0)
     {
	if (is_from(buffer, garbage, sizeof(garbage)))
	  {
	     total_cnt++;
	     parse_mime_header(fp);
	  }
     }
   fclose(fp);

   /* Restore the access time of the mailbox for other checking programs */
   t[0].tv_sec = s.st_atime;
   t[0].tv_usec = 0;
   t[1].tv_sec = s.st_mtime;
   t[1].tv_usec = 0;

   ebiff_utimes(path, t);
   D((" -> Mailbox check complete.  Found %lu new messages of %lu total.\n",
      new_cnt, total_cnt));
   return 1;
}

/* Counts the number of messages (files) in a directory, not including "." and
 * ".." entries.
 * Returns the count of messages dir, or ULONG_MAX on error
 */
unsigned long
maildir_count_dir(char *dir)
{
   DIR                *dp;
   struct dirent      *dent;
   unsigned long       count = 0;

   dp = opendir(dir);

   if (!dp)
     {
	D((" -> Unable to opendir %s.\n", dir));
	return ULONG_MAX;
     }

   while ((dent = readdir(dp)) != NULL)
      count++;

   /* Discard . and .. - maybe we should check each file name as we read them?
    * That would be slower, however :-( */
   count -= 2;

   closedir(dp);

   return count;
}

/*
 * Return 0 on no change/failure, 1 on change
 */
int
maildir_folder_count(char *path, int force)
{
   char               *curdir, *newdir;
   time_t              last_update;
   unsigned long       new_msgs, old_msgs;
   struct stat         s;

   D(("maildir_folder_count(%s, %d) called.\n", NONULL(path), force));

   if (path == NULL)
      return 0;

   if (stat(path, &s) != 0)
     {
	D((" -> Unable to stat maildir.\n"));
	file_size = 0;
	file_mtime = 0;
	return 0;
     }

   last_update = s.st_mtime;

   curdir = (char *)malloc(strlen(path) + 5);
   if (!curdir)
     {
	D((" -> Unable to allocate memory.\n"));
	return 0;
     }
   newdir = (char *)malloc(strlen(path) + 5);
   if (!newdir)
     {
	D((" -> Unable to allocate memory.\n"));
	free(curdir);
	return 0;
     }

   sprintf(curdir, "%s/cur", path);
   sprintf(newdir, "%s/new", path);

   if (stat(curdir, &s) != 0)
     {
	D((" -> Unable to stat cur directory - is this a maildir?\n"));
	return 0;
     }
   if (s.st_mtime > last_update)
      last_update = s.st_mtime;

   if (stat(newdir, &s) != 0)
     {
	D((" -> Unable to stat new directory - is this a maildir?\n"));
	return 0;
     }
   if (s.st_mtime > last_update)
      last_update = s.st_mtime;

   if (!force && (last_update == file_mtime))
     {
	D((" -> Mailbox unchanged.\n"));
	return 0;
     }

   old_msgs = maildir_count_dir(curdir);
   if (old_msgs == ULONG_MAX)
      return 0;

   new_msgs = maildir_count_dir(newdir);
   if (new_msgs == ULONG_MAX)
      return 0;

   new_cnt = new_msgs;
   total_cnt = new_msgs + old_msgs;
   file_mtime = last_update;

   return 1;
}
