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
#ifndef efsd_dyn_char_array_h
#define efsd_dyn_char_array_h

typedef struct efsd_dyn_char_array EfsdDynCharArray;

/* A dynamic char* array, supporting automatic resizing,
   sorting, inserting and removal. Appends are O(1),
   sort on average is O(N log N) (Quicksort), inserts
   and removes are O(log N) for the lookup and O(N) for
   the shifts.

   Inserted strings are strdup()d, so the caller remains
   responsible for the cleanup of the passed pointers.
*/

/* Allocation and deallocation. */
EfsdDynCharArray  *efsd_dca_new(void);
void               efsd_dca_free(EfsdDynCharArray *a);

/* Append string -- no sorting. */
void               efsd_dca_append(EfsdDynCharArray *a, const char *s);

/* Sort array */
void               efsd_dca_sort(EfsdDynCharArray *a);

/* Insertion and removal -- S is duplicated before insertion */
void               efsd_dca_insert(EfsdDynCharArray *a, const char *s);
void               efsd_dca_remove(EfsdDynCharArray *a, const char *s);

/* Access to items. Returns NULL if index is invalid. */
char              *efsd_dca_get(EfsdDynCharArray *a, int i);

#endif
