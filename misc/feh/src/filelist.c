/* filelist.c
 *
 * Copyright (C) 1999 Tom Gilbert
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
 * USA
 */

#include "feh.h"

feh_file filelist = NULL;
feh_file current_file = NULL;
extern int errno;

static feh_file rm_filelist = NULL;

feh_file
filelist_newitem (char *filename)
{
  feh_file newfile;
  char *s;

  D (("In filelist_newitem\n"));

  newfile = (feh_file) emalloc (sizeof (_feh_file));
  newfile->filename = estrdup (filename);
  s = strrchr (filename, '/');
  if (s)
    newfile->name = estrdup (s + 1);
  newfile->next = NULL;
  newfile->prev = NULL;
  return newfile;
}

void
feh_file_free (feh_file file)
{
  D (("In feh_file_free\n"));
  if (!file)
    return;
  if (file->filename)
    free (file->filename);
  if (file->name)
    free (file->name);
  free (file);
}

feh_file
feh_file_rm_and_free (feh_file list, feh_file file)
{
  D (("In feh_file_rm_and_free\n"));
  unlink (file->filename);
  return filelist_remove_file (list, file);
}


feh_file
filelist_addtofront (feh_file root, feh_file newfile)
{
  D (("In filelist_addtofront\n"));
  newfile->next = root;
  newfile->prev = NULL;
  if (root)
    root->prev = newfile;
  return newfile;
}

int
filelist_length (feh_file file)
{
  int length;
  D (("In filelist_length\n"));
  length = 0;
  while (file)
    {
      length++;
      file = file->next;
    }
  D (("   length is %d\n", length));
  return length;
}

feh_file filelist_last (feh_file file)
{
  D (("In filelist_last\n"));
  if (file)
    {
      while (file->next)
	file = file->next;
    }
  return file;
}

feh_file filelist_first (feh_file file)
{
  D (("In filelist_first\n"));
  if (file)
    {
      while (file->prev)
	file = file->prev;
    }
  return file;
}

feh_file
filelist_reverse (feh_file list)
{
  feh_file last;

  last = NULL;
  while (list)
    {
      last = list;
      list = last->next;
      last->next = last->prev;
      last->prev = list;
    }

  return last;
}

int
filelist_num (feh_file list, feh_file file)
{
  int i = 0;

  D (("In filelist_num\n"));

  while (list)
    {
      if (list == file)
	return i;
      i++;
      list = list->next;
    }
  return -1;
}

feh_file
filelist_remove_file (feh_file list, feh_file file)
{
  D (("In filelist_remove_file\n"));
  if (!file)
    return list;
  if (file->prev)
    file->prev->next = file->next;
  else
    list = file->next;
  if (file->next)
    file->next->prev = file->prev;
  feh_file_free (file);
  return list;
}

/* Recursive */
void
add_file_to_filelist_recursively (char *path, unsigned char level)
{
  struct stat st;

  D (("In add_file_to_filelist_recursively file is %s\n", path));

  if (level == FILELIST_FIRST)
    {
      /* First time through, sort out pathname */
      int len = 0;
      len = strlen (path);
      if (path[len - 1] == '/')
	path[len - 1] = '\0';

      if ((!strncmp (path, "http://", 7)) || (!strncmp (path, "ftp://", 6)))
	{
	  /* Its a url */
	  D (("Adding url %s to filelist\n", path));
	  filelist = filelist_addtofront (filelist, filelist_newitem (path));
	  /* We'll download it later... */
	  return;
	}
    }

  errno = 0;
  if (stat (path, &st))
    {
      /* Display useful error message */
      switch (errno)
	{
	case ENOENT:
	case ENOTDIR:
	  weprintf ("%s does not exist - skipping", path);
	  break;
	case ELOOP:
	  weprintf ("%s - too many levels of symbolic links - skipping",
		    path);
	  break;
	case EACCES:
	  weprintf ("you don't have permission to open %s - skipping", path);
	  break;
	default:
	  weprintf ("couldn't open %s ", path);
	  break;
	}
      return;
    }

  if (S_ISDIR (st.st_mode))
    {
      D (("   It is a directory\n"));
      if (level != FILELIST_LAST)
	{
	  struct dirent *de;
	  DIR *dir;
	  if ((dir = opendir (path)) == NULL)
	    {
	      weprintf ("couldn't open directory %s, errno:%d :", path,
			errno);
	      return;
	    }
	  de = readdir (dir);
	  while (de != NULL)
	    {
	      if (strcmp (de->d_name, ".") && strcmp (de->d_name, ".."))
		{
		  char *file;

		  file = estrjoin ("", path, "/", de->d_name, NULL);

		  /* This ensures we go down one level even if not
		   * fully recursive - this way "feh some_dir" expands to
		   * some_dir's contents */
		  if (opt.recursive)
		    add_file_to_filelist_recursively (file,
						      FILELIST_CONTINUE);
		  else
		    add_file_to_filelist_recursively (file, FILELIST_LAST);

		  free (file);
		}
	      de = readdir (dir);
	    }
	  closedir (dir);
	}
    }
  else if (S_ISREG (st.st_mode))
    {
      D (("Adding regular file %s to filelist\n", path));
      filelist = filelist_addtofront (filelist, filelist_newitem (path));
    }
}

void
add_file_to_rm_filelist (char *file)
{
  D (("In add_file_to_rm_filelist\n"));

  rm_filelist = filelist_addtofront (rm_filelist, filelist_newitem (file));
}

void
delete_rm_files (void)
{
  feh_file file;

  D (("In delete_rm_files\n"));

  if (opt.keep_http)
    return;

  for (file = rm_filelist; file; file = file->next)
    unlink (file->filename);
}
