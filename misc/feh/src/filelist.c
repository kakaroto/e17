/* filelist.c
 *
 * Copyright (C) 2000 Tom Gilbert
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

#include "feh.h"
#include "feh_list.h"
#include "filelist.h"
#include "options.h"

feh_list *filelist = NULL;
feh_list *current_file = NULL;
extern int errno;
int file_num = 0;


static feh_list *rm_filelist = NULL;

feh_file *
feh_file_new(char *filename)
{
   feh_file *newfile;
   char *s;

   D_ENTER;

   newfile = (feh_file *) emalloc(sizeof(feh_file));
   newfile->filename = estrdup(filename);
   s = strrchr(filename, '/');
   if (s)
      newfile->name = estrdup(s + 1);
   else
      newfile->name = estrdup(filename);
   newfile->info = NULL;
   D_RETURN(newfile);
}

void
feh_file_free(feh_file * file)
{
   D_ENTER;
   if (!file)
      D_RETURN_;
   if (file->filename)
      free(file->filename);
   if (file->name)
      free(file->name);
   if (file->info)
      feh_file_info_free(file->info);
   free(file);
   D_RETURN_;
}

feh_file_info *
feh_file_info_new(void)
{
   feh_file_info *info;

   D_ENTER;

   info = (feh_file_info *) emalloc(sizeof(feh_file_info));

   info->width = 0;
   info->height = 0;
   info->size = 0;
   info->pixels = 0;
   info->has_alpha = 0;
   info->format = NULL;
   info->extension = NULL;

   D_RETURN(info);
}

void
feh_file_info_free(feh_file_info * info)
{
   D_ENTER;
   if (!info)
      D_RETURN_;
   if (info->format)
      free(info->format);
   if (info->extension)
      free(info->extension);
   free(info);
   D_RETURN_;
}

feh_list *
feh_file_rm_and_free(feh_list * list, feh_list * l)
{
   D_ENTER;
   unlink(FEH_FILE(l->data)->filename);
   D_RETURN(feh_file_remove_from_list(list, l));
}

feh_list *
feh_file_remove_from_list(feh_list *list, feh_list *l)
{
   D_ENTER;
   feh_file_free(FEH_FILE(l->data));
   D_RETURN(feh_list_remove(list, l));
}

/* Recursive */
void
add_file_to_filelist_recursively(char *origpath, unsigned char level)
{
   struct stat st;
   char *path;

   D_ENTER;
   if (!origpath)
      D_RETURN_;

   path = estrdup(origpath);
   D(("file is %s\n", path));

   if (level == FILELIST_FIRST)
   {
      /* First time through, sort out pathname */
      int len = 0;

      len = strlen(path);
      if (path[len - 1] == '/')
         path[len - 1] = '\0';

      if ((!strncmp(path, "http://", 7)) || (!strncmp(path, "ftp://", 6)))
      {
         /* Its a url */
         D(("Adding url %s to filelist\n", path));
         filelist = feh_list_add_front(filelist, feh_file_new(path));
         /* We'll download it later... */
         free(path);
         D_RETURN_;
      }
      else if (opt.filelistfile)
      {
         char *newpath = feh_absolute_path(path);

         free(path);
         path = newpath;
      }
   }

   errno = 0;
   if (stat(path, &st))
   {
      /* Display useful error message */
      switch (errno)
      {
        case ENOENT:
        case ENOTDIR:
           if (!opt.quiet)
              weprintf("%s does not exist - skipping", path);
           break;
        case ELOOP:
           if (!opt.quiet)
              weprintf("%s - too many levels of symbolic links - skipping",
                       path);
           break;
        case EACCES:
           if (!opt.quiet)
              weprintf("you don't have permission to open %s - skipping",
                       path);
           break;
        default:
           if (!opt.quiet)
              weprintf("couldn't open %s", path);
           break;
      }
      free(path);
      D_RETURN_;
   }

   if ((S_ISDIR(st.st_mode)) && (level != FILELIST_LAST))
   {
      struct dirent *de;
      DIR *dir;

      D(("It is a directory\n"));

      if ((dir = opendir(path)) == NULL)
      {
         if (!opt.quiet)
            weprintf("couldn't open directory %s:", path);
         free(path);
         D_RETURN_;
      }
      de = readdir(dir);
      while (de != NULL)
      {
         if (strcmp(de->d_name, ".") && strcmp(de->d_name, ".."))
         {
            char *newfile;

            newfile = estrjoin("", path, "/", de->d_name, NULL);

            /* This ensures we go down one level even if not fully recursive
               - this way "feh some_dir" expands to some_dir's contents */
            if (opt.recursive)
               add_file_to_filelist_recursively(newfile, FILELIST_CONTINUE);
            else
               add_file_to_filelist_recursively(newfile, FILELIST_LAST);

            free(newfile);
         }
         de = readdir(dir);
      }
      closedir(dir);
   }
   else if (S_ISREG(st.st_mode))
   {
      D(("Adding regular file %s to filelist\n", path));
      filelist = feh_list_add_front(filelist, feh_file_new(path));
   }
   free(path);
   D_RETURN_;
}

void
add_file_to_rm_filelist(char *file)
{
   D_ENTER;
   rm_filelist = feh_list_add_front(rm_filelist, feh_file_new(file));
   D_RETURN_;
}

void
delete_rm_files(void)
{
   feh_list *l;

   D_ENTER;
   for (l = rm_filelist; l; l = l->next)
      unlink(FEH_FILE(l->data)->filename);
   D_RETURN_;
}

feh_list *
feh_file_info_preload(feh_list * list)
{
   feh_list *l, *last = NULL;
   feh_file *file = NULL;

   D_ENTER;
   if (opt.verbose)
      fprintf(stdout, PACKAGE " - preloading...\n");

   for (l = list; l; l = l->next)
   {
       file = FEH_FILE(l->data);
      D(("file %p, file->next %p, file->name %s\n", l, l->next, file->name));
      if (last)
      {
         D(("removing item %p from list\n", last));
         filelist = list = feh_list_remove(list, last);
         last = NULL;
      }
      if (feh_file_info_load(file, NULL))
      {
         D(("Failed to load file %p\n", file));
         last = l;
         if (opt.verbose)
            feh_display_status('x');
      }
      else if (opt.verbose)
         feh_display_status('.');
   }
   if (opt.verbose)
      fprintf(stdout, "\n");

   if (last)
      list = feh_list_remove(list, last);

   D_RETURN(list);
}

int
feh_file_info_load(feh_file * file, Imlib_Image im)
{
   struct stat st;
   int need_free = 1;

   D_ENTER;

   if (im)
      need_free = 0;

   errno = 0;
   if (stat(file->filename, &st))
   {
      /* Display useful error message */
      switch (errno)
      {
        case ENOENT:
        case ENOTDIR:
           if (!opt.quiet)
              weprintf("%s does not exist - skipping", file->filename);
           break;
        case ELOOP:
           if (!opt.quiet)
              weprintf("%s - too many levels of symbolic links - skipping",
                       file->filename);
           break;
        case EACCES:
           if (!opt.quiet)
              weprintf("you don't have permission to open %s - skipping",
                       file->filename);
           break;
        default:
           if (!opt.quiet)
              weprintf("couldn't open %s ", file->filename);
           break;
      }
      D_RETURN(1);
   }

   if (im || feh_load_image(&im, file))
   {
      file->info = feh_file_info_new();

      file->info->width = feh_imlib_image_get_width(im);
      file->info->height = feh_imlib_image_get_height(im);

      file->info->has_alpha = feh_imlib_image_has_alpha(im);

      file->info->pixels = file->info->width * file->info->height;

      file->info->format = estrdup(feh_imlib_image_format(im));

      file->info->size = st.st_size;

      if (need_free)
         feh_imlib_free_image_and_decache(im);

      D_RETURN(0);
   }
   D_RETURN(1);
}

int
feh_cmp_filename(void * file1, void * file2)
{
   D_ENTER;
   D_RETURN(strcmp(FEH_FILE(file1)->filename, FEH_FILE(file2)->filename));
}

int
feh_cmp_name(void * file1, void * file2)
{
   D_ENTER;
   D_RETURN(strcmp(FEH_FILE(file1)->name, FEH_FILE(file2)->name));
}

int
feh_cmp_width(void * file1, void * file2)
{
   D_ENTER;
   D_RETURN((FEH_FILE(file1)->info->width - FEH_FILE(file2)->info->width));
}

int
feh_cmp_height(void * file1, void * file2)
{
   D_ENTER;
   D_RETURN((FEH_FILE(file1)->info->height - FEH_FILE(file2)->info->height));
}

int
feh_cmp_pixels(void * file1, void * file2)
{
   D_ENTER;
   D_RETURN((FEH_FILE(file1)->info->pixels - FEH_FILE(file2)->info->pixels));
}

int
feh_cmp_size(void * file1, void * file2)
{
   D_ENTER;
   D_RETURN((FEH_FILE(file1)->info->size - FEH_FILE(file2)->info->size));
}

int
feh_cmp_format(void * file1, void * file2)
{
   D_ENTER;
   D_RETURN(strcmp(FEH_FILE(file1)->info->format, FEH_FILE(file2)->info->format));
}

void
feh_prepare_filelist(void)
{
   D_ENTER;
   if (opt.list || opt.customlist || (opt.sort > SORT_FILENAME)
       || opt.preload)
   {
      /* For these sort options, we have to preload images */
      filelist = feh_file_info_preload(filelist);
      if (!feh_list_length(filelist))
         show_mini_usage();
   }

   D(("sort mode requested is: %d\n", opt.sort));
   switch (opt.sort)
   {
     case SORT_NONE:
        if (opt.randomize)
        {
           /* Randomize the filename order */
           filelist = feh_list_randomize(filelist);
        }
        else if (!opt.reverse)
        {
           /* Let's reverse the list. Its back-to-front right now ;) */
           filelist = feh_list_reverse(filelist);
        }
        break;
     case SORT_NAME:
        filelist = feh_list_sort(filelist, feh_cmp_name);
        break;
     case SORT_FILENAME:
        filelist = feh_list_sort(filelist, feh_cmp_filename);
        break;
     case SORT_WIDTH:
        filelist = feh_list_sort(filelist, feh_cmp_width);
        break;
     case SORT_HEIGHT:
        filelist = feh_list_sort(filelist, feh_cmp_height);
        break;
     case SORT_PIXELS:
        filelist = feh_list_sort(filelist, feh_cmp_pixels);
        break;
     case SORT_SIZE:
        filelist = feh_list_sort(filelist, feh_cmp_size);
        break;
     case SORT_FORMAT:
        filelist = feh_list_sort(filelist, feh_cmp_format);
        break;
     default:
        break;
   }

   /* no point reversing a random list */
   if (opt.reverse && (opt.sort != SORT_NONE))
   {
      D(("Reversing filelist as requested\n"));
      filelist = feh_list_reverse(filelist);
   }

   D_RETURN_;
}

int
feh_write_filelist(feh_list * list, char *filename)
{
   FILE *fp;
   feh_list *l;

   D_ENTER;

   if (!list || !filename)
      D_RETURN(0);

   errno = 0;
   if ((fp = fopen(filename, "w")) == NULL)
   {
      weprintf("can't write filelist %s:", filename);
      D_RETURN(0);
   }

   for (l = list; l; l = l->next)
      fprintf(fp, "%s\n", (FEH_FILE(l->data)->filename));

   fclose(fp);

   D_RETURN(1);
}

feh_list *
feh_read_filelist(char *filename)
{
   FILE *fp;
   feh_list *list = NULL;
   char s[1024], s1[1024];

   D_ENTER;

   if (!filename)
      D_RETURN(NULL);

   errno = 0;
   if ((fp = fopen(filename, "r")) == NULL)
   {
      /* return quietly, as it's okay to specify a filelist file that doesn't
         exist. In that case we create it on exit. */
      D_RETURN(NULL);
   }

   for (; fgets(s, sizeof(s), fp);)
   {
      D(("Got line '%s'\n", s));
      s1[0] = '\0';
      sscanf(s, "%s", (char *) &s1);
      if (!(*s1) || (*s1 == '\n'))
         continue;
      D(("Got filename %s from filelist file\n", s1));
      /* Add it to the new list */
      list = feh_list_add_front(list, feh_file_new(s1));
   }
   fclose(fp);

   D_RETURN(list);
}

char *
feh_absolute_path(char *path)
{
   char cwd[PATH_MAX];
   char fullpath[PATH_MAX];
   char temp[PATH_MAX];
   char *ret;

   D_ENTER;

   if (!path)
      D_RETURN(NULL);
   if (path[0] == '/')
      D_RETURN(estrdup(path));
   /* This path is not relative. We're gonna convert it, so that a
      filelist file can be saved anywhere and feh will still find the
      images */
   D(("Need to convert %s to an absolute form\n", path));
   /* I SHOULD be able to just use a simple realpath() here, but dumb * 
      old Solaris's realpath doesn't return an absolute path if the
      path you give it is relative. Linux and BSD get this right... */
   getcwd(cwd, sizeof(cwd));
   snprintf(temp, sizeof(temp), "%s/%s", cwd, path);
   if (realpath(temp, fullpath) != NULL)
   {
      ret = estrdup(fullpath);
   }
   else
   {
      ret = estrdup(temp);
   }
   D(("Converted path to %s\n", ret));
   D_RETURN(ret);
}
