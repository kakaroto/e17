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
#ifndef efsd_list_h
#define efsd_list_h

typedef struct efsd_list EfsdList;
typedef void(*EfsdFunc) (void *data);

EfsdList *efsd_list_new(void *data);
void      efsd_list_free(EfsdList *l, EfsdFunc free_func);

EfsdList *efsd_list_head(EfsdList *l);
EfsdList *efsd_list_next(EfsdList *l);
EfsdList *efsd_list_prev(EfsdList *l);

/* Appends item to end of list and returns pointer to
   the new item (NOT to the list!). */
EfsdList *efsd_list_append(EfsdList *l, void *data);

/* Prepends item and returns pointer to it. */
EfsdList *efsd_list_prepend(EfsdList *l, void *data);
EfsdList *efsd_list_remove(EfsdList *l, EfsdList *ll, EfsdFunc free_func);
void     *efsd_list_data(EfsdList *l);
EfsdList *efsd_list_move_to_front(EfsdList *l, EfsdList *item);

#endif
