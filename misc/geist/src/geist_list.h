/* geist_list.h

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

#ifndef geist_LIST_H
#define geist_LIST_H

enum __direction
{ FORWARD, BACK };

struct __geist_list
{
   void *data;

   geist_list *next;
   geist_list *prev;
};

typedef int (geist_compare_fn) (void *data1, void *data2);

geist_list *geist_list_new(void);
void geist_list_free(geist_list * l);
geist_list *geist_list_add_front(geist_list * root, void *data);
geist_list *geist_list_add_end(geist_list * root, void *data);
geist_list *geist_list_pop_to_end(geist_list * root, geist_list * l);
geist_list *geist_list_unlink(geist_list * root, geist_list * l);
void geist_debug_print_list(geist_list * l);
geist_list *geist_list_cat(geist_list * root, geist_list * l);
int geist_list_length(geist_list * l);
geist_list *geist_list_last(geist_list * l);
geist_list *geist_list_first(geist_list * l);
geist_list *geist_list_jump(geist_list * root, geist_list * l, int direction,

                            int num);
geist_list *geist_list_reverse(geist_list * l);
geist_list *geist_list_randomize(geist_list * list);
int geist_list_num(geist_list * root, geist_list * l);
geist_list *geist_list_remove(geist_list * root, geist_list * l);
geist_list *geist_list_sort(geist_list * list, geist_compare_fn cmp);
geist_list *geist_list_sort_merge(geist_list * l1, geist_list * l2,
                                  geist_compare_fn cmp);
geist_list *geist_list_nth(geist_list * root, unsigned int num);
unsigned char geist_list_has_more_than_one_item(geist_list * root);
void geist_list_free_and_data(geist_list * l);
geist_list *geist_list_dup(geist_list * list);
geist_list *geist_list_dup_special(geist_list * list,
                                   void (*cpy_func)(void **dest, void *data));
geist_list *
geist_list_move_down_by_one(geist_list * root, geist_list * l);
geist_list *
geist_list_move_up_by_one(geist_list * root, geist_list * l);

#endif
