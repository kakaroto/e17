/* list.h
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

typedef struct __feh_file_info feh_file_info;

struct __feh_file_info
{
  char *filename;
  char *name;
  int width;
  int height;
  int size;
  int pixels;
  int has_alpha;
  char *format;
  char *extension;
  feh_file_info *next;
  feh_file_info *prev;
};

typedef int (feh_compare_fn)(feh_file_info *info1, feh_file_info *info2);

feh_file_info *feh_fileinfo_create (feh_file file);
void feh_fileinfo_free (feh_file_info * info);
feh_file_info *feh_fileinfo_addtofront (feh_file_info * root,
					feh_file_info * newinfo);
int feh_fileinfo_length (feh_file_info * info);
feh_file_info *feh_fileinfo_last (feh_file_info * info);
feh_file_info *feh_fileinfo_first (feh_file_info * info);
feh_file_info *feh_filelinfo_reverse (feh_file_info * list);
feh_file_info *feh_fileinfo_remove_info (feh_file_info * list,
					 feh_file_info * info);
feh_file_info *feh_list_sort (feh_file_info * list, feh_compare_fn cmp);
feh_file_info *feh_list_sort_merge (feh_file_info * l1,
				    feh_file_info * l2, feh_compare_fn cmp);
