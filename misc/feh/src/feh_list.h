/* feh_list.h

Copyright (C) 1999,2000 Tom Gilbert.

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

#ifndef FEH_LIST_H
#define FEH_LIST_H

struct __feh_list
{
   void *data;

   feh_list *next;
   feh_list *prev;
};

typedef int (feh_compare_fn) (void *data1, void *data2);

feh_list *feh_list_new(void);
feh_list *feh_list_add_front(feh_list * root, void *data);
feh_list *feh_list_cat(feh_list * root, feh_list * l);
int feh_list_length(feh_list * l);
feh_list *feh_list_last(feh_list * l);
feh_list *feh_list_first(feh_list * l);
feh_list *feh_list_jump(feh_list * root, feh_list * l, int direction,

                        int num);
feh_list *feh_list_reverse(feh_list * l);
feh_list *feh_list_randomize(feh_list * list);
int feh_list_num(feh_list * root, feh_list * l);
feh_list *feh_list_get_num(feh_list * root, unsigned int num);
feh_list *feh_list_remove(feh_list * root, feh_list * l);
feh_list *feh_list_sort(feh_list * list, feh_compare_fn cmp);
feh_list *feh_list_sort_merge(feh_list * l1, feh_list * l2,

                              feh_compare_fn cmp);
void feh_list_free(feh_list * l);

#endif
