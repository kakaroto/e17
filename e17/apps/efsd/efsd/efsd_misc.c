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

#ifdef __EMX__
#include <stdlib.h>
#include <malloc.h>
#endif

#include <efsd_misc.h>
#include <efsd_debug.h>
#include <efsd_globals.h>
#include <efsd_macros.h>
#include <efsd_meta.h>
#include <efsd_misc.h>
#include <efsd_statcache.h>


mode_t         mode_755 = (S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP |
			   S_IXGRP | S_IROTH | S_IXOTH);

int    
efsd_misc_file_exists(char *filename)
{
  struct stat st;

  D_ENTER;

  if (!filename)
    D_RETURN_(FALSE);

  if (!efsd_lstat(filename, &st))
    D_RETURN_(FALSE);

  D_RETURN_(TRUE);
}


int 
efsd_misc_file_is_dir(char *filename)
{
  struct stat st;

  D_ENTER;

  if (!filename)
    D_RETURN_(FALSE);

  if (!efsd_stat(filename, &st))
    D_RETURN_(FALSE);

  D_RETURN_(S_ISDIR(st.st_mode));
}


int    
efsd_misc_file_writeable(char *filename)
{
  struct stat st;

  D_ENTER;

  if (!filename)
    D_RETURN_(FALSE);

  if (!efsd_stat(filename, &st))
    D_RETURN_(FALSE);

  if (st.st_uid == getuid())
    {
      if (st.st_mode & S_IWUSR)
	D_RETURN_(TRUE);
     }
  else if (st.st_gid == getgid())
    {
      if (st.st_mode & S_IWGRP)
	D_RETURN_(TRUE);
    }
  else
    {
      if (st.st_mode & S_IWOTH)
	D_RETURN_(TRUE);
    }

  D_RETURN_(FALSE);
}


int    
efsd_misc_file_execable(char *filename)
{
  struct stat st;

  D_ENTER;

  if (!filename)
    D_RETURN_(FALSE);

  if (!efsd_lstat(filename, &st))
    D_RETURN_(FALSE);

  if (st.st_uid == getuid())
    {
      if (st.st_mode & S_IXUSR)
	D_RETURN_(TRUE);
     }
  else if (st.st_gid == getgid())
    {
      if (st.st_mode & S_IXGRP)
	D_RETURN_(TRUE);
    }
  else
    {
      if (st.st_mode & S_IXOTH)
	D_RETURN_(TRUE);
    }

  D_RETURN_(FALSE);
}


int    
efsd_misc_file_is_dotfile(char *filename)
{
  char *slash = NULL;

  D_ENTER;
  
  if (!filename || filename[0] == '\0')
    D_RETURN_(FALSE);

  slash = strrchr(filename, '/');

  if (slash)
    filename = slash + 1;

  if (*filename == '.')
    D_RETURN_(TRUE);

  D_RETURN_(FALSE);
}


/* Checks if file paths are identical after making
   them chanonic -- returns < 0 on error, FALSE
   if files differ, TRUE otherwise.
*/
int
efsd_misc_files_identical(char *file1, char *file2)
{
  char real1[MAXPATHLEN];
  char real2[MAXPATHLEN];

  D_ENTER;

  D("Files %s and %s equal?\n", file1, file2);
  if (realpath(file1, real1) && realpath(file2, real2))
    {
      if (!strcmp(real1, real2))
	D_RETURN_(TRUE);

      D_RETURN_(FALSE);
    }

  D("Couldn't realpath files.\n");
  perror("Error:\n");
  D_RETURN_(-1);
}


int    
efsd_misc_remove(char *filename)
{
  D_ENTER;

  if (!filename || filename[0] == '\0')
    {
      errno = EINVAL;
      D_RETURN_(-1);
    }

  if (remove(filename) == 0)
    {
      char meta_file[MAXPATHLEN];

      /* File is removed -- now remove
	 any cached stat data ...
      */
      efsd_stat_remove(filename, TRUE);

      /* .. and any metadata. We don't
	 care about the result (maybe
	 no metadata existed etc).
      */
      if (efsd_meta_get_meta_file(filename, meta_file, MAXPATHLEN, FALSE))
	remove(meta_file);
      
      D_RETURN_(0);
    }

  D("Removing %s failed.\n", filename);
  
  D_RETURN_(-1);
}


int    
efsd_misc_rename(char *file1, char *file2)
{
  D_ENTER;

  if (!file1 || file1[0] == '\0' ||
      !file2 || file2[0] == '\0')
    {
      errno = EINVAL;
      D_RETURN_(-1);
    }

  if (rename(file1, file2) == 0)
    {
      char meta_file1[MAXPATHLEN];
      char meta_file2[MAXPATHLEN];

      /* Update stat cache to new name ... */
      efsd_stat_change_filename(file1, file2);

      /* ... and metadata. */
      if ((efsd_meta_get_meta_file(file1, meta_file1, MAXPATHLEN, FALSE)) &&
	  (efsd_meta_get_meta_file(file2, meta_file2, MAXPATHLEN, TRUE)))
	rename(meta_file1, meta_file2);

      D_RETURN_(0);
    }
  
  D_RETURN_(-1);
}


int    
efsd_misc_mkdir(char *filename)
{
  D_ENTER;

  if (!filename)
    {
      errno = EINVAL;
      D_RETURN_(0);
    }

  if (mkdir(filename, mode_755) < 0)
    {
      D_RETURN_(0);
    }

  D_RETURN_(1);
}


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
efsd_misc_is_absolute_path(char *s)
{
  D_ENTER;

  if (!s || s[0] == '\0')
    D_RETURN_(0);

#ifndef __EMX__    
  if (s[0] == '/')
#else  
  if (_fnisabs(s))
#endif  
    D_RETURN_(1);

  D_RETURN_(0);
}


char **
efsd_misc_get_path_dirs(char *s, int *num_dirs)
{
  int     num = 1;
  char   *p, *q, old;
  char  **result;
#ifdef __EMX__  
  int    drive_present = 0;
#endif
  
  D_ENTER;

  if (!s || s[0] == '\0')
    {
      *num_dirs = 0;
      D_RETURN_(NULL);
    }

  efsd_misc_remove_trailing_slashes(s);
#ifdef __EMX__  
  efsd_slashify(s);	
#endif  
  while ((*s) == '/') s++;

  p = s;
  while ( (p = strchr(p, '/')) != NULL)
    {
      while ((*p) == '/') p++;
      num++;
    }

#ifdef __EMX__  
   q = strchr(s, ':');
   /* if colon is chars 0, 1, or 2 it is a drive letter for os/2 */
   if ((q) && (q - s) < 3) 
     { 
      num--; 
      drive_present = 1;
     } 
#endif
    
  *num_dirs = num;
  result = (char**) malloc(sizeof(char*) * num);

  p = q = s;
  num = 0;
#ifdef __EMX__  
  if (drive_present) 
    { p = strchr(p, ':');    
      if (p) 
        if (*(p+1) == '/') p += 2;
    }  
#endif      
  while ( (p = strchr(p, '/')) != NULL)
    {
      old = *p;
      *p = '\0';
      result[num] = strdup(q);
      *p = old;

      num++;
      while ((*p) == '/') p++;
      q = p;
    }

  result[num] = strdup(q);

  D_RETURN_(result);
}

#ifdef __EMX__  
void efsd_slashify(char *s)
{
  int	 	i;
 
  D_ENTER;
  
  if (!s || s[0] == '\0')
    D_RETURN;
  
  for (i = 0; i< strlen(s); i++) 
    {
      if (s[i] ==  '\\')
	s[i] = '/';
    }
  D_RETURN;
}
#endif


char  *
efsd_misc_get_filename_only(char *path)
{
  char *result = NULL;

  D_ENTER;

  if (!path || path[0] == '\0')
    D_RETURN_(NULL);

  if ((result = strrchr(path, '/')) != NULL)
    result++;

  D_RETURN_(result);
}


int
efsd_misc_get_path_only(const char *filename, char *path, int size)
{
  char *result = NULL;

  if (!filename || filename[0] == '\0' || !path)
    return(0);

  if ((result = strrchr(filename, '/')) == NULL)
    return(0);

  if (result - filename + 1 > size)
    return(0);

  memcpy(path, filename, (result - filename));
  path[result - filename] = '\0';

  return(1);
}


void    
efsd_misc_create_efsd_dir(void)
{
  char *dir = NULL;
  char  s[MAXPATHLEN];

  D_ENTER;

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

  snprintf(s, sizeof(s), "%s/.e/efsd", dir);

  if (!efsd_misc_file_is_dir(s))
    efsd_misc_mkdir(s);

  D_RETURN;
}


void
efsd_misc_remove_socket_file(void)
{
  D_ENTER;

  if (unlink(efsd_misc_get_socket_file()) < 0)
    {
      if (errno != ENOENT)
	{
	  D("Could not remove socket file.\n");
	}
    }
  else
    {
      D("Socket file removed.\n");
    }

  D_RETURN;
}


char *
efsd_misc_get_user_dir(void)
{
  char         *dir = NULL;
  static char   s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);

  dir = getenv("HOME");

  /* I'm not using getenv("TMPDIR") --
   * I don't see TMPDIR on Linux, FreeBSD
   * or Solaris here...
   */

  /* FIXME -- I need to properly handle the case
     where I cannot determine the home directory.
     This will break if multiple users run E on the
     same machine:
  */

  if (!dir)
    dir = "/tmp";

  snprintf(s, sizeof(s), "%s/.e/efsd", dir);

  D_RETURN_(s);
}


char *
efsd_misc_get_sys_dir(void)
{
  D_ENTER;
  D_RETURN_(PACKAGE_DATA_DIR);
}


char *
efsd_misc_get_socket_file(void)
{
  static char s[4096] = "\0";
  
  D_ENTER;

  if (s[0] != '\0')
    D_RETURN_(s);
#ifndef __EMX__
  snprintf(s, sizeof(s), "%s/efsd_socket", efsd_misc_get_user_dir());
#else
  snprintf(s, sizeof(s), "\\socket\\%s/efsd_socket", efsd_misc_get_user_dir());
#endif
  s[sizeof(s)-1] = '\0';
  D_RETURN_(s);
}


void
efsd_misc_quicksort(char **a, int l, int r)
{
  int                 i, j, m;
  void               *v, *t;
  
  if (r > l)
    {      
      m = (r + l) / 2 + 1;
      if (strcmp(a[l], a[r]) > 0)
	{
	  t = a[l];
	  a[l] = a[r];
	  a[r] = t;
	}
      if (strcmp(a[l], a[m]) > 0)
	{
	  t = a[l];
	  a[l] = a[m];
	  a[m] = t;
	}
      if (strcmp(a[r], a[m]) > 0)
	{
	  t = a[r];
	  a[r] = a[m];
	  a[m] = t;
	}
      
      v = a[r];
      i = l - 1;
      j = r;
      
      for ( ; ; )
	{
	  while (strcmp(a[++i], v) < 0);
	  while (strcmp(a[--j], v) > 0);
	  if (i >= j)
	    break;
	  t = a[i];
	  a[i] = a[j];
	  a[j] = t;
	}
      t = a[i];
      a[i] = a[r];
      a[r] = t;
      efsd_misc_quicksort(a, l, i - 1);
      efsd_misc_quicksort(a, i + 1, r);
    }
}


char   *
efsd_misc_strcat(char *source, char *suffix, int suffix_len)
{
  char *result = NULL;
  int   len = 0;

  D_ENTER;

  if (source)
    len = strlen(source);

  result = malloc(sizeof(char) * (suffix_len + 1 + len));
  if (result)
    {
      if (source)
	{
	  memcpy(result, source, len);
	  FREE(source);
	}

      memcpy(result + len, suffix, suffix_len);
      result[len + suffix_len] = '\0';
      
      D_RETURN_(result);
    }
  
  D_RETURN_(NULL);
}


