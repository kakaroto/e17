/* filelist.h
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

struct __feh_file
{
   char *filename;
   char *name;

   /* info stuff */
   feh_file_info *info;

   feh_file *next;
   feh_file *prev;
};

struct __feh_file_info
{
   int width;
   int height;
   int size;
   int pixels;
   unsigned char has_alpha;
   char *format;
   char *extension;
};

enum filelist_recurse
{ FILELIST_FIRST, FILELIST_CONTINUE, FILELIST_LAST };

enum sort_type
{ SORT_NONE, SORT_NAME, SORT_FILENAME, SORT_WIDTH, SORT_HEIGHT, SORT_PIXELS,
   SORT_SIZE, SORT_FORMAT
};


feh_file *filelist_addtofront(feh_file * root, feh_file * newfile);
feh_file *filelist_newitem(char *filename);
feh_file *filelist_remove_file(feh_file * list, feh_file * file);
void feh_file_free(feh_file * file);
int filelist_length(feh_file * file);
feh_file *filelist_last(feh_file * file);
feh_file *filelist_first(feh_file * file);
feh_file *filelist_join(feh_file * root, feh_file * newfile);
feh_file *filelist_jump(feh_file * list, feh_file * file, int direction,

                        int num);
feh_file *feh_file_rm_and_free(feh_file * list, feh_file * file);
int filelist_num(feh_file * list, feh_file * file);
feh_file *filelist_reverse(feh_file * list);
feh_file *filelist_randomize(feh_file * list);
typedef int (feh_compare_fn) (feh_file * file1, feh_file * file2);
feh_file_info *feh_file_info_new(void);
void feh_file_info_free(feh_file_info * info);
int feh_file_info_load(feh_file * file);
feh_file *feh_list_sort(feh_file * list, feh_compare_fn cmp);
feh_file *feh_list_sort_merge(feh_file * l1, feh_file * l2,
                              feh_compare_fn cmp);
int feh_write_filelist(feh_file * list, char *filename);
feh_file *feh_read_filelist(char *filename); 
int feh_cmp_name(feh_file * file1, feh_file * file2);
int feh_cmp_filename(feh_file * file1, feh_file * file2);
int feh_cmp_width(feh_file * file1, feh_file * file2);
int feh_cmp_height(feh_file * file1, feh_file * file2);
int feh_cmp_pixels(feh_file * file1, feh_file * file2);
int feh_cmp_size(feh_file * file1, feh_file * file2);
int feh_cmp_format(feh_file * file1, feh_file * file2);
feh_file *feh_file_info_preload(feh_file * list);
void add_file_to_rm_filelist(char *file);
void delete_rm_files(void);
void add_file_to_filelist_recursively(char *origpath, unsigned char level);
void feh_prepare_filelist(void);

extern feh_file *filelist;
extern feh_file *current_file;

