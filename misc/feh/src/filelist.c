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
#include "filelist.h"

static void
add_file_to_filelist (char *file)
{
  D (("In add_file_to_filelist\n"));
  file_num++;
  if (files)
    files = erealloc (files, file_num * sizeof (char *));
  else
    files = emalloc (file_num * sizeof (char *));
  files[file_num - 1] = file;
}

/* Recursive */
void
add_file_to_filelist_recursively (char *path, unsigned char enough)
{
  struct stat st;

  D (("In add_file_to_filelist_recursively file is %s\n", path));

  if (!enough)
    {
      /* First time through, sort out pathname */
      int len = 0;
      len = strlen (path);
      if (path[len - 1] == '/')
	path[len - 1] = '\0';
    }

  if (stat (path, &st))
    {
      weprintf ("%s : ", path);
      return;
    }
  if (S_ISDIR (st.st_mode))
    {
      D (("   It is a directory\n"));
      if (!enough)
	{
	  struct dirent *de;
	  DIR *dir;
	  if ((dir = opendir (path)) == NULL)
	      eprintf ("Error opening dir %s", path);
	  de = readdir (dir);
	  while (de != NULL)
	    {
	      if (strcmp (de->d_name, ".") && strcmp (de->d_name, ".."))
		{
		  char *file;
		  int len = 0;

		  /* Add one for the "/" and one for the '/0' */
		  len = strlen (path) + strlen (de->d_name) + 2;
		  file = emalloc (len);
		  /* Remember NOT to free this. add_file_to_filelist doesn't
		   * duplicate and we need the filelist for the lifetime of the
		   * app. */
		  snprintf (file, len, "%s/%s", path, de->d_name);

		  /* This ensures we only go down one level if not
		   * recursive */
		  if (opt.recursive)
		    add_file_to_filelist_recursively (file, 0);
		  else
		    add_file_to_filelist_recursively (file, 1);
		}
	      de = readdir (dir);
	    }
	}
      else
	{
	  D (("   But I am ignoring it as recurse is not set\n"));
	}
    }
  else if (S_ISREG (st.st_mode))
    {
      D (("Adding regular file %s to filelist\n", path));
      add_file_to_filelist (path);
    }
  else
    {
      /* Ignore this strange thing :) */
      D (("Non dir, non regular file encountered - %s\n", path));
    }
}
