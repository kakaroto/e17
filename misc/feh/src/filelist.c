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

feh_file *filelist = NULL;
feh_file *current_file = NULL;
extern int errno;

static feh_file *rm_filelist = NULL;

feh_file *
filelist_newitem (char *filename)
{
  feh_file *newfile;
  char *s;

  D (("In filelist_newitem\n"));

  newfile = (feh_file *) emalloc (sizeof (feh_file));
  newfile->filename = estrdup (filename);
  s = strrchr (filename, '/');
  if (s)
    newfile->name = estrdup (s + 1);
  else
    newfile->name = estrdup (filename);
  newfile->info = NULL;
  newfile->next = NULL;
  newfile->prev = NULL;
  return newfile;
}

void
feh_file_free (feh_file * file)
{
  D (("In feh_file_free\n"));
  if (!file)
    return;
  if (file->filename)
    free (file->filename);
  if (file->name)
    free (file->name);
  if (file->info)
    feh_file_info_free (file->info);
  free (file);
}

feh_file_info *
feh_file_info_new (void)
{
  feh_file_info *info;

  D (("In feh_file_info_new\n"));

  info = (feh_file_info *) emalloc (sizeof (feh_file_info));

  info->width = 0;
  info->height = 0;
  info->size = 0;
  info->pixels = 0;
  info->has_alpha = 0;
  info->format = NULL;
  info->extension = NULL;

  return info;
}

void
feh_file_info_free (feh_file_info * info)
{
  D (("In feh_file_info_free\n"));
  if (!info)
    return;
  if (info->format)
    free (info->format);
  if (info->extension)
    free (info->extension);
  free (info);
}

feh_file *
feh_file_rm_and_free (feh_file * list, feh_file * file)
{
  D (("In feh_file_rm_and_free\n"));
  unlink (file->filename);
  return filelist_remove_file (list, file);
}


feh_file *
filelist_addtofront (feh_file * root, feh_file * newfile)
{
  D (("In filelist_addtofront\n"));
  if (!newfile)
    return root;
  newfile->next = root;
  newfile->prev = NULL;
  if (root)
    root->prev = newfile;
  return newfile;
}

int
filelist_length (feh_file * file)
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

feh_file *
filelist_last (feh_file * file)
{
  D (("In filelist_last\n"));
  if (file)
    {
      while (file->next)
	file = file->next;
    }
  return file;
}

feh_file *
filelist_first (feh_file * file)
{
  D (("In filelist_first\n"));
  if (file)
    {
      while (file->prev)
	file = file->prev;
    }
  return file;
}

feh_file *
filelist_reverse (feh_file * list)
{
  feh_file *last;

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

feh_file *
filelist_randomize (feh_file * list)
{
  int len, r, i;
  feh_file **farray, *f;

  len = filelist_length (list);
  D (("filelist_randomize(%8p):  List has %d items.\n", list, len));
  farray = (feh_file **) malloc (sizeof (feh_file *) * len);
  for (f = list, i = 0; f; f = f->next, i++)
    {
      D (
	 ("filelist_randomize():  farray[%d] <- %8p (%s)\n", i, f,
	  f->filename));
      farray[i] = f;
    }
  srand (getpid () * time (NULL) % ((unsigned int) -1));
  for (i = 0; i < len - 1; i++)
    {
      r =
	(int) ((len - i - 1) * ((float) rand ()) / (RAND_MAX + 1.0)) + i + 1;
      D (("i == %d, r == %d\n", i, r));
      if (i == r)
	abort ();
      D (
	 ("Swapping farray[%d] (%8p, %s) with farray[%d] (%8p, %s)\n", i,
	  farray[i], farray[i]->filename, r, farray[r], farray[r]->filename));
      SWAP (farray[i], farray[r]);
      D (("New values are %8p and %8p\n", farray[i], farray[r]));
    }
  list = farray[0];
  list->prev = NULL;
  list->next = farray[1];
  for (i = 1, f = farray[1]; i < len - 1; i++, f = f->next)
    {
      f->prev = farray[i - 1];
      f->next = farray[i + 1];
      D (
	 ("Rebuilding list.  At farray[%d], f == %8p %s, f->prev == %8p %s, f->next == %8p %s\n",
	  i, f, f->filename, f->prev, f->prev->filename, f->next,
	  f->next->filename));
    }
  f->prev = farray[len - 2];
  f->next = NULL;
  return list;
}

int
filelist_num (feh_file * list, feh_file * file)
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

feh_file *
filelist_remove_file (feh_file * list, feh_file * file)
{
  D (("In filelist_remove_file\n"));
  if (!file)
    return list;
  if (!list)
    return NULL;

  if ((file == list) && (!file->next))
    return NULL;

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
	  if (!opt.quiet)
	    weprintf ("%s does not exist - skipping", path);
	  break;
	case ELOOP:
	  if (!opt.quiet)
	    weprintf ("%s - too many levels of symbolic links - skipping",
		      path);
	  break;
	case EACCES:
	  if (!opt.quiet)
	    weprintf ("you don't have permission to open %s - skipping",
		      path);
	  break;
	default:
	  if (!opt.quiet)
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
	      if (!opt.quiet)
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
  feh_file *file;

  D (("In delete_rm_files\n"));

  if (opt.keep_http)
    return;

  for (file = rm_filelist; file; file = file->next)
    unlink (file->filename);
}

feh_file *
feh_file_info_preload (feh_file * list)
{
  feh_file *file, *last = NULL;

  if (opt.verbose)
    fprintf (stdout, "feh - preloading...\n");

  for (file = list; file; file = file->next)
    {
      if (last)
	{
	  list = filelist_remove_file (list, last);
	  last = NULL;
	}
      if (feh_file_info_load (file))
	{
	  last = file;
	  if (opt.verbose)
	    feh_display_status ('x');
	}
      else if (opt.verbose)
	feh_display_status ('.');

    }
  if (opt.verbose)
    fprintf (stdout, "\n");

  if (last)
    list = filelist_remove_file (list, last);

  return list;
}

int
feh_file_info_load (feh_file * file)
{
  Imlib_Image *im = NULL;
  struct stat st;

  D (("In feh_file_info_load\n"));

  errno = 0;
  if (stat (file->filename, &st))
    {
      /* Display useful error message */
      switch (errno)
	{
	case ENOENT:
	case ENOTDIR:
	  if (!opt.quiet)
	    weprintf ("%s does not exist - skipping", file->filename);
	  break;
	case ELOOP:
	  if (!opt.quiet)
	    weprintf ("%s - too many levels of symbolic links - skipping",
		      file->filename);
	  break;
	case EACCES:
	  if (!opt.quiet)
	    weprintf ("you don't have permission to open %s - skipping",
		      file->filename);
	  break;
	default:
	  if (!opt.quiet)
	    weprintf ("couldn't open %s ", file->filename);
	  break;
	}
      return 1;
    }

  if (feh_load_image (&im, file))
    {
      imlib_context_set_image (im);

      file->info = feh_file_info_new ();

      file->info->width = imlib_image_get_width ();
      file->info->height = imlib_image_get_height ();

      file->info->has_alpha = imlib_image_has_alpha ();

      file->info->pixels = file->info->width * file->info->height;

      file->info->format = estrdup (imlib_image_format ());

      imlib_free_image_and_decache ();

      file->info->size = st.st_size;

      return 0;
    }
  return 1;
}

feh_file *
feh_list_sort (feh_file * list, feh_compare_fn cmp)
{
  feh_file *l1, *l2;

  D (("In feh_list_sort\n"));

  if (!list)
    return NULL;
  if (!list->next)
    return list;

  l1 = list;
  l2 = list->next;

  while ((l2 = l2->next) != NULL)
    {
      if ((l2 = l2->next) == NULL)
	break;
      l1 = l1->next;
    }
  l2 = l1->next;
  l1->next = NULL;

  return feh_list_sort_merge (feh_list_sort (list, cmp),
			      feh_list_sort (l2, cmp), cmp);
}

feh_file *
feh_list_sort_merge (feh_file * l1, feh_file * l2, feh_compare_fn cmp)
{
  feh_file list, *l, *lprev;

  D (("In feh_list_sort_merge\n"));

  l = &list;
  lprev = NULL;

  while (l1 && l2)
    {
      if (cmp (l1, l2) < 0)
	{
	  l->next = l1;
	  l = l->next;
	  l->prev = lprev;
	  lprev = l;
	  l1 = l1->next;
	}
      else
	{
	  l->next = l2;
	  l = l->next;
	  l->prev = lprev;
	  lprev = l;
	  l2 = l2->next;
	}
    }
  l->next = l1 ? l1 : l2;
  l->next->prev = l;

  return list.next;
}


int
feh_cmp_filename (feh_file * file1, feh_file * file2)
{
  D (("In feh_cmp_filename\n"));
  return strcmp (file1->filename, file2->filename);
}

int
feh_cmp_name (feh_file * file1, feh_file * file2)
{
  D (("In feh_cmp_name\n"));
  return strcmp (file1->name, file2->name);
}

int
feh_cmp_width (feh_file * file1, feh_file * file2)
{
  D (("In feh_cmp_width\n"));
  return (file1->info->width - file2->info->width);
}

int
feh_cmp_height (feh_file * file1, feh_file * file2)
{
  D (("In feh_cmp_height\n"));
  return (file1->info->height - file2->info->height);
}

int
feh_cmp_pixels (feh_file * file1, feh_file * file2)
{
  D (("In feh_cmp_pixels\n"));
  return (file1->info->pixels - file2->info->pixels);
}

int
feh_cmp_size (feh_file * file1, feh_file * file2)
{
  D (("In feh_cmp_size\n"));
  return (file1->info->size - file2->info->size);
}

int
feh_cmp_format (feh_file * file1, feh_file * file2)
{
  D (("In feh_cmp_format\n"));
  return strcmp (file1->info->format, file2->info->format);
}

void
feh_prepare_filelist (void)
{
  if (opt.list || opt.longlist || (opt.sort > SORT_FILENAME) || opt.preload)
    {
      /* For these sort options, we have to preload images */
      filelist = feh_file_info_preload (filelist);
      if (!filelist_length (filelist))
	show_mini_usage ();
    }

  D (("sort mode requested is: %d\n", opt.sort));
  switch (opt.sort)
    {
    case SORT_NONE:
      if (opt.randomize)
	{
	  /* Randomize the filename order */
	  filelist = filelist_randomize (filelist);
	}
      else
	{
	  /* Let's reverse the list. Its back-to-front right now ;) */
	  filelist = filelist_reverse (filelist);
	}
      break;
    case SORT_NAME:
      filelist = feh_list_sort (filelist, feh_cmp_name);
      break;
    case SORT_FILENAME:
      filelist = feh_list_sort (filelist, feh_cmp_filename);
      break;
    case SORT_WIDTH:
      filelist = feh_list_sort (filelist, feh_cmp_width);
      break;
    case SORT_HEIGHT:
      filelist = feh_list_sort (filelist, feh_cmp_height);
      break;
    case SORT_PIXELS:
      filelist = feh_list_sort (filelist, feh_cmp_pixels);
      break;
    case SORT_SIZE:
      filelist = feh_list_sort (filelist, feh_cmp_size);
      break;
    case SORT_FORMAT:
      filelist = feh_list_sort (filelist, feh_cmp_format);
      break;
    default:
      break;
    }
}
