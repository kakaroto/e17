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
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>

#include <efsd_macros.h>
#include <efsd_debug.h>
#include <efsd_misc.h>
#include <efsd_fs.h>
#include <efsd_statcache.h>

static int data_copy(char *src_path, char *dst_path);
static int file_copy(char *src_path, char *dst_path);
static int dir_copy(char *src_path, char *dst_path);
static int file_move(char *src_path, char *dst_path);
static int dir_move(char *src_path, char *dst_path);
static int file_remove(char *path);


static int
data_copy(char *src_path, char *dst_path)
{
  int  src_fd, dst_fd, block_size = 4096;
  struct stat *dst_st = NULL;
  struct stat *src_st = NULL;
  char *buf, hole_at_end = FALSE, check_for_holes = FALSE;

  D_ENTER;

  D(("Data-copying %s to %s\n", src_path, dst_path));

  if ( (src_fd = open(src_path, O_RDONLY)) < 0)
    D_RETURN_(-1);

  umask(000);

  if ( (src_st = efsd_stat(src_path)) != NULL)
    {
      D(("Using src's permissions.\n"));

      if ( (dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC,
			  src_st->st_mode)) < 0)
	{
	  close(src_fd);
	  D_RETURN_(-1);
	}
    }
  else
    {
      if ( (dst_fd = open(dst_path, O_WRONLY | O_CREAT | O_TRUNC,
			  S_IRUSR | S_IWUSR)) < 0)
	{
	  close(src_fd);
	  D_RETURN_(-1);
	}
    }

  umask(077);

#if HAVE_ST_BLKSIZE
  if ( (dst_st = efsd_stat(dst_path)) != NULL)
    block_size = (int)dst_st->st_blksize;
#if HAVE_ST_BLOCKS
  if (src_st && S_ISREG(src_st->st_mode) &&
      (src_st->st_size / src_st->st_blksize > src_st->st_blocks))
    check_for_holes = TRUE;
#endif
#endif

  D(("Using holes: %i, buffer size %i\n", check_for_holes, block_size));

  buf = malloc(block_size);

  for ( ; ; )
    {
      int i;
      int num_read;
      char write_hole = FALSE;

      if ( (num_read = read(src_fd, buf, block_size)) < 0)
	{
	  if (errno == EINTR)
	    continue;

	  close(src_fd);
	  close(dst_fd);
	  D_RETURN_(-1);
	}

      if (num_read == 0) /* End of file */
	break;

      if (check_for_holes)
	{
	  write_hole = TRUE;
	  
	  for (i = 0; i < num_read; i++)
	    {
	      if (buf[i] != 0)
		{
		  write_hole = FALSE;
		  break;
		}
	    }
	}

      if (write_hole)
	{
	  if (lseek (dst_fd, (off_t)num_read, SEEK_CUR) < 0L)
	    {
	      close(src_fd);
	      close(dst_fd);
	      D_RETURN_(-1);
	    }

	  hole_at_end = TRUE;
	}
      else
	{
	  char *buf_ptr;
	  int   n;

	  /* Write out stuff, decreasing num_read again */

	  buf_ptr = buf;

	  while (num_read > 0)
	    {
	      if ( (n = write(dst_fd, buf_ptr, num_read)) < 0)
		{
		  if (errno == EINTR)
		    continue;
		  
		  close(src_fd);
		  close(dst_fd);
		  D_RETURN_(-1);
		}

	      num_read -= n;
	      buf_ptr += n;
	    }

	  hole_at_end = FALSE;
	}
    }
  
  /* Make sure things end properly at destination file */

  if (hole_at_end)
    {
      if (lseek(dst_fd, -1, SEEK_CUR) < 0L)
	{
	  D(("Fixing hole at eof.\n"));

	  for ( ; ; )
	    {
	      if (write(dst_fd, "", 1) < 0)
		{
		  if (errno == EINTR)
		    continue;
		  
		  close(src_fd);
		  close(dst_fd);
		  D_RETURN_(-1);
		}
	      
	      break;
	    }
	}
    }

  D(("Copy succeeded.\n"));
  
  close(src_fd);
  close(dst_fd);
  
  D_RETURN_(0);
}


static int 
file_copy(char *src_path, char *dst_path)
{
  struct stat *src_st = NULL;

  D_ENTER;

  if (efsd_misc_file_exists(dst_path) &&
      efsd_misc_files_identical(src_path, dst_path))
    D_RETURN_(-1);
  
  D(("Copying file %s to %s\n", src_path, dst_path));
  
  if ( (src_st = efsd_lstat(src_path)) == NULL)
    D_RETURN_(-1);

  if (S_ISLNK(src_st->st_mode))
    {
      char realfile[MAXPATHLEN];

      if (readlink(src_path, realfile, MAXPATHLEN) < 0)
	D_RETURN_(-1);
      
      if (symlink(realfile, dst_path) == 0)
	D_RETURN_(0);
    }
  else if (S_ISFIFO(src_st->st_mode))
    {
      if (mkfifo(dst_path, src_st->st_mode) == 0)
	D_RETURN_(0);
    }
  else if (S_ISCHR(src_st->st_mode) ||
	   S_ISBLK(src_st->st_mode) ||
	   S_ISSOCK(src_st->st_mode))
    {
      if (mknod(dst_path, src_st->st_mode, src_st->st_dev) == 0)
	D_RETURN_(0);
    }
  else
    D_RETURN_(data_copy(src_path, dst_path));
  
  /* Whatever we have here as src (e.g a directory),
     we cannot copy onto a normal file.
  */
  
  D_RETURN_(-1);
}


static int 
dir_copy(char *src_path, char *dst_path)
{
  char           src[MAXPATHLEN];
  char           dst[MAXPATHLEN];
  char          *src_ptr, *dst_ptr;
  int            src_len, dst_len;
  struct stat   *src_st = NULL;
  DIR           *dir;
  struct dirent *dp;

  D_ENTER;
  D(("Copying directory %s to %s\n", src_path, dst_path));

  /* When this is called, src_path is a dir,
     and dst_path doesn't exist yet.
  */
  
  if ( (src_st = efsd_stat(src_path)) == NULL)
    D_RETURN_(-1);

  umask(000);

  if (mkdir(dst_path, src_st->st_mode) < 0)
    D_RETURN_(-1);

  umask(077);

  D(("Directory %s created.\n", dst_path));
  
  if ( (dir = opendir(src_path)) == NULL)
    D_RETURN_(-1);

  snprintf(src, MAXPATHLEN, "%s/", src_path);
  snprintf(dst, MAXPATHLEN, "%s/", dst_path);
  src_len = strlen(src);
  dst_len = strlen(dst);
  src_ptr = src + src_len;
  dst_ptr = dst + dst_len;
  
  for (dp = readdir(dir); dp; dp = readdir(dir))
    {
      if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
	continue;

      snprintf(src_ptr, MAXPATHLEN - src_len, "%s", dp->d_name);
      snprintf(dst_ptr, MAXPATHLEN - dst_len, "%s", dp->d_name);

      if ( (src_st = efsd_stat(src)) == NULL)
	continue;

      if (S_ISDIR(src_st->st_mode))
	dir_copy(src, dst);
      else
	file_copy(src, dst);
    }

  closedir(dir);

  D_RETURN_(0);
}


static int 
file_move(char *src_path, char *dst_path)
{
  D_ENTER;

  D(("Moving file %s to %s\n", src_path, dst_path));

  if (rename(src_path, dst_path) != 0)
    {
      D(("Rename failed -- copying, then removing.\n"));
      if (file_copy(src_path, dst_path) >= 0)
	D_RETURN_(remove(src_path));

      D_RETURN_(-1);
    }
  
  D_RETURN_(0);
}


static int 
dir_move(char *src_path, char *dst_path)
{
  char           src[MAXPATHLEN];
  char           dst[MAXPATHLEN];
  char          *src_ptr, *dst_ptr;
  int            src_len, dst_len;
  struct stat   *src_st = NULL;
  DIR           *dir;
  struct dirent *dp;

  D_ENTER;
  D(("Moving directory %s to %s\n", src_path, dst_path));

  /* When this is called, src_path is a dir,
     and dst_path doesn't exist yet.
  */
  
  if ( (src_st = efsd_stat(src_path)) == NULL)
    D_RETURN_(-1);

  umask(000);

  if (mkdir(dst_path, src_st->st_mode) < 0)
    D_RETURN_(-1);

  umask(077);

  D(("Directory %s created.\n", dst_path));
  
  if ( (dir = opendir(src_path)) == NULL)
    D_RETURN_(-1);

  snprintf(src, MAXPATHLEN, "%s/", src_path);
  snprintf(dst, MAXPATHLEN, "%s/", dst_path);
  src_len = strlen(src);
  dst_len = strlen(dst);
  src_ptr = src + src_len;
  dst_ptr = dst + dst_len;
  
  for (dp = readdir(dir); dp; dp = readdir(dir))
    {
      if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
	continue;

      snprintf(src_ptr, MAXPATHLEN - src_len, "%s", dp->d_name);
      snprintf(dst_ptr, MAXPATHLEN - dst_len, "%s", dp->d_name);

      if (file_move(src, dst) == 0)
	continue;

      if ( (src_st = efsd_stat(src)) == NULL)
	{
	  closedir(dir);
	  D_RETURN_(-1);
	}

      if (S_ISDIR(src_st->st_mode))
	{
	  if (dir_move(src, dst) < 0)
	    {
	      closedir(dir);
	      D_RETURN_(-1);
	    }
	}
      else
	{
	  closedir(dir);
	  D_RETURN_(-1);
	}
    }

  /* Close dir. It's empty now, so also remove it. */

  closedir(dir);
  D_RETURN_(remove(src_path));
}


static int 
file_remove(char *path)
{
  char           s[MAXPATHLEN];
  char          *s_ptr;
  int            s_len;
  DIR           *dir;
  struct dirent *dp;
  struct stat   *st;

  D_ENTER;

  /* Simply try if it works. */
  if (remove(path) == 0)
    D_RETURN_(0);

  /* No. Okay. Check if it's a directory. */
  if ( (st = efsd_lstat(path)) == NULL)
    D_RETURN_(-1);

  if (S_ISDIR(st->st_mode))
    {
      /* It's a directory. Remove everything in it recursively,
	 then try removing the empty directory.
      */

      if ( (dir = opendir(path)) == NULL)
	D_RETURN_(-1);
      
      snprintf(s, MAXPATHLEN, "%s/", path);
      s_len = strlen(s);
      s_ptr = s + s_len;
      
      for (dp = readdir(dir); dp; dp = readdir(dir))
	{
	  if (!strcmp(dp->d_name, ".") || !strcmp(dp->d_name, ".."))
	    continue;
	  
	  snprintf(s_ptr, MAXPATHLEN - s_len, "%s", dp->d_name);
	  
	  if (remove(s) == 0)
	    continue;

	  if ( (st = efsd_lstat(s)) == NULL)
	    {
	      /* We couldn't stat it and we couldn't
		 remove it -- report error.
	      */

	      closedir(dir);
	      D_RETURN_(-1);
	    }
	  
	  if (S_ISDIR(st->st_mode))
	    {
	      /* It's a directoy -- remove recursively */
	      if (file_remove(s) < 0)
		{
		  closedir(dir);
		  D_RETURN_(-1);
		}
	    }
	  else
	    {
	      closedir(dir);
	      D_RETURN_(-1);
	    }
	}

      /* We deleted everything in the directory.
	 Report result of removing directory itself.
      */

      closedir(dir);            
      D_RETURN_(remove(path));
    }

  /* It's not a directory either. Report error. */

  D_RETURN_(-1);
}


int 
efsd_fs_cp(char *src_path, char *dst_path, EfsdFsOps ops)
{
  char s[MAXPATHLEN];
  struct stat *dst_st = NULL;
  struct stat *src_st = NULL;

  D_ENTER;

  if (!src_path || src_path[0] == '\0' ||
      !dst_path || dst_path[0] == '\0')
    D_RETURN_(-1);

  if (efsd_misc_file_exists(dst_path) &&
      efsd_misc_files_identical(src_path, dst_path))
    {
      D(("src and dst are equal in copy -- doing nothing.\n"));
      
      if (ops & EFSD_FS_OP_FORCE)
	D_RETURN_(0);
      
      D_RETURN_(-1);
    }

  D(("Copying %s to %s, rec [%s], force [%s]\n", src_path, dst_path,
     (ops & EFSD_FS_OP_RECURSIVE) ? "X" : " ",
     (ops & EFSD_FS_OP_FORCE) ? "X" : " "));
     
  if ( (src_st = efsd_stat(src_path)) == NULL)
    {
      D(("Could not stat source.\n"));
      D_RETURN_(-1);
    }

  dst_st = efsd_stat(dst_path);
  
  /* If dest exists and is a dir, adjust dest by
     appending last component of file name.
  */

  if (dst_st)
    {
      if (S_ISDIR(dst_st->st_mode))
	{
	  snprintf(s, MAXPATHLEN, "%s/%s", dst_path,
		   efsd_misc_get_filename_only(src_path));
	  dst_path = s;
	  D(("Adjusted dest to: %s\n", dst_path));
	}
  
      /* Otherwise: Check target -- if it exists and FORCE is
	 not used, abort. Also abort if FORCE is given but
	 target cannot be removed.
      */
      else
	{
	  if ((ops & EFSD_FS_OP_FORCE) == 0)
	    {
	      D(("Dest exists and no force used -- aborting.\n"));
	      D_RETURN_(-1);
	    }
	  
	  if (efsd_fs_rm(dst_path, EFSD_FS_OP_RECURSIVE) < 0)
	    {
	      D(("Could not remove existing dest.\n"));
	      D_RETURN_(-1);
	    }
	}
    }

  /* If it's a dir and we're recursive, copy directory. */
  if (S_ISDIR(src_st->st_mode))
    {
      if ((ops & EFSD_FS_OP_RECURSIVE) == 0)
	{
	  D(("src is directory but we're not recursive -- aborting.\n"));
	  D_RETURN_(-1);
	}

      D_RETURN_(dir_copy(src_path, dst_path));
    }

  /* Otherwise, copy single regular file. */
      
  D_RETURN_(file_copy(src_path, dst_path));
}


int 
efsd_fs_mv(char *src_path, char *dst_path, EfsdFsOps ops)
{
  char s[MAXPATHLEN];
  struct stat *dst_st = NULL;
  struct stat *src_st = NULL;

  D_ENTER;

  if (!src_path || src_path[0] == '\0' ||
      !dst_path || dst_path[0] == '\0')
    D_RETURN_(-1);

  D(("Moving %s to %s, rec [%s], force [%s]\n", src_path, dst_path,
     (ops & EFSD_FS_OP_RECURSIVE) ? "X" : " ",
     (ops & EFSD_FS_OP_FORCE) ? "X" : " "));
     
  if ( (src_st = efsd_stat(src_path)) == NULL)
    {
      D(("Could not stat source.\n"));
      D_RETURN_(-1);
    }

  dst_st = efsd_stat(dst_path);
  
  /* If dest exists and is a dir, adjust dest by
     appending last component of file name.
  */

  if (dst_st)
    {
      if (S_ISDIR(dst_st->st_mode))
	{
	  snprintf(s, MAXPATHLEN, "%s/%s", dst_path,
		   efsd_misc_get_filename_only(src_path));
	  dst_path = s;
	  D(("Adjusted dest to: %s\n", dst_path));
	}
  
      /* Otherwise: Check target -- if it exists and FORCE is
	 not used, abort. Also abort if FORCE is given but
	 target cannot be removed.
      */
      else
	{
	  if ((ops & EFSD_FS_OP_FORCE) == 0)
	    {
	      D(("Dest exists and no force used -- aborting.\n"));
	      D_RETURN_(-1);
	    }
	  
	  if (efsd_fs_rm(dst_path, EFSD_FS_OP_RECURSIVE) < 0)
	    {
	      D(("Could not remove existing dest.\n"));
	      D_RETURN_(-1);
	    }
	}
    }

  /* Try and see if simple rename() is enough. */

  if (rename(src_path, dst_path) < 0)
    {
      /* If it's a dir and we're recursive, move directory. */
      if (S_ISDIR(src_st->st_mode))
	{
	  if ((ops & EFSD_FS_OP_RECURSIVE) == 0)
	    {
	      D(("src is directory but we're not recursive -- aborting.\n"));
	      D_RETURN_(-1);
	    }
	  
	  dir_move(src_path, dst_path);
	}
      else
	{
	  /* Otherwise, move single regular file. */
	  file_move(src_path, dst_path);
	}

      D_RETURN_(efsd_fs_rm(src_path, ops));      
    }

  /* Yay! A simple rename was enough ... */

  D_RETURN_(0);
}


int 
efsd_fs_rm(char *path, EfsdFsOps ops)
{
  struct stat *st;

  D_ENTER;

  if ( (st = efsd_stat(path)) == NULL)
    {
      /* File doesn't exist -- that's okay if we're
	 using force, otherwise it's an error.
      */

      if (ops & EFSD_FS_OP_FORCE)
	D_RETURN_(0);
      
      D_RETURN_(-1);
    }

  /* If it's a directory and we're not
     recursive, it's an error.
  */
  if (S_ISDIR(st->st_mode) && !(ops & EFSD_FS_OP_RECURSIVE))
    D_RETURN_(-1);

  /* Otherwise, try to remove and report result. */

  D_RETURN_(file_remove(path));
}
