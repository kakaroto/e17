/* list.c
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
#include "list.h"

static feh_file_info *info_list = NULL;
extern int errno;

static int feh_cmp_name (feh_file_info * info1, feh_file_info * info2);
static int feh_cmp_filename (feh_file_info * info1, feh_file_info * info2);

void
init_list_mode (void)
{
  feh_file file;
  feh_file_info *info;
  int i = 0, j = 0;

  D (("In init_list_mode\n"));

  for (file = filelist; file; file = file->next)
    {
      info_list =
	feh_fileinfo_addtofront (info_list, feh_fileinfo_create (file));
      if (opt.verbose)
	{
	  if (i)
	    {
	      if (!(i % 50))
		fprintf (stdout, "\n ");
	      else if (!(i % 10))
		fprintf (stdout, " ");
	    }
	  else
	    fprintf (stdout, " ");

	  fprintf (stdout, ".");
	  fflush (stdout);
	  i++;
	}
    }
  if (opt.verbose)
    fprintf (stdout, "\n");

  /* By default sort by filename/name */
  if(opt.longlist)
	info_list = feh_list_sort (info_list, feh_cmp_filename);
  else
	info_list = feh_list_sort (info_list, feh_cmp_name);

  if (opt.longlist)
    printf
      ("NUM\tFORMAT\tWIDTH\tHEIGHT\tPIXELS\tSIZE(bytes)\tALPHA\tFILENAME\n");
  else
    printf ("NUM\tFORMAT\tWIDTH\tHEIGHT\tSIZE(bytes)\tALPHA\tNAME\n");
  for (info = info_list; info; info = info->next)
    {
      if (opt.longlist)
	printf ("%d\t%s\t%d\t%d\t%d\t%d\t\t%c\t%s\n", ++j, info->format,
		info->width, info->height, info->pixels, info->size,
		info->has_alpha ? 'X' : '-', info->filename);
      else
	printf ("%d\t%s\t%d\t%d\t%d\t\t%c\t%s\n", ++j, info->format,
		info->width, info->height, info->size,
		info->has_alpha ? 'X' : '-', info->name);
    }

  exit (0);
}

feh_file_info *
feh_fileinfo_create (feh_file file)
{
  feh_file_info *newinfo;
  Imlib_Image *im = NULL;
  struct stat st;

  D (("In feh_fileinfo_create\n"));

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
      return NULL;
    }


  if (feh_load_image (&im, file))
    {
      imlib_context_set_image (im);
      newinfo = (feh_file_info *) emalloc (sizeof (feh_file_info));

      newinfo->filename = estrdup (file->filename);
      newinfo->name = estrdup (file->name);

      newinfo->width = imlib_image_get_width (),
	newinfo->height = imlib_image_get_height ();

      newinfo->has_alpha = imlib_image_has_alpha ();

      newinfo->pixels = newinfo->width * newinfo->height;

      newinfo->format = estrdup (imlib_image_format ());

      imlib_free_image_and_decache ();

      newinfo->size = st.st_size;

      newinfo->next = NULL;
      newinfo->prev = NULL;
      return newinfo;
    }
  return NULL;
}

void
feh_fileinfo_free (feh_file_info * info)
{
  D (("In feh_fileinfo_free\n"));
  if (!info)
    return;
  if (info->filename)
    free (info->filename);
  if (info->name)
    free (info->name);
  free (info);
}

feh_file_info *
feh_fileinfo_addtofront (feh_file_info * root, feh_file_info * newinfo)
{
  D (("In feh_fileinfo_addtofront\n"));
  if (!newinfo)
    return root;
  newinfo->next = root;
  newinfo->prev = NULL;
  if (root)
    root->prev = newinfo;
  return newinfo;
}

int
feh_fileinfo_length (feh_file_info * info)
{
  int length;
  D (("In feh_fileinfo_length\n"));
  length = 0;
  while (info)
    {
      length++;
      info = info->next;
    }
  D (("   length is %d\n", length));
  return length;
}

feh_file_info *
feh_fileinfo_last (feh_file_info * info)
{
  D (("In feh_fileinfo_last\n"));
  if (info)
    {
      while (info->next)
	info = info->next;
    }
  return info;
}

feh_file_info *
feh_fileinfo_first (feh_file_info * info)
{
  D (("In feh_fileinfo_first\n"));
  if (info)
    {
      while (info->prev)
	info = info->prev;
    }
  return info;
}

feh_file_info *
feh_filelinfo_reverse (feh_file_info * list)
{
  feh_file_info *last;

  D(("In feh_filelinfo_reverse\n"));
  
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

feh_file_info *
feh_fileinfo_remove_info (feh_file_info * list, feh_file_info * info)
{
  D (("In feh_fileinfo_remove_info\n"));
  if (!info)
    return list;
  if (info->prev)
    info->prev->next = info->next;
  else
    list = info->next;
  if (info->next)
    info->next->prev = info->prev;
  feh_fileinfo_free (info);
  return list;
}

feh_file_info *
feh_list_sort (feh_file_info * list, feh_compare_fn cmp)
{
  feh_file_info *l1, *l2;

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

feh_file_info *
feh_list_sort_merge (feh_file_info * l1,
		     feh_file_info * l2, feh_compare_fn cmp)
{
  feh_file_info list, *l, *lprev;

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


static int
feh_cmp_filename (feh_file_info * info1, feh_file_info * info2)
{
    D(("In feh_cmp_filename\n"));
  return strcmp (info1->filename, info2->filename);
}

static int
feh_cmp_name (feh_file_info * info1, feh_file_info * info2)
{
    D(("In feh_cmp_name\n"));
  return strcmp (info1->name, info2->name);
}
