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
#ifndef __efsd_hash_h
#define __efsd_hash_h

#include <efsd_list.h>

typedef struct efsd_hash EfsdHash;
typedef struct efsd_hash_iterator EfsdHashIterator;

typedef struct efsd_hash_item
{
  void *key;
  void *data;
}
EfsdHashItem;

typedef unsigned int (*EfsdHashFunc) (EfsdHash *h, void *data);
typedef int (*EfsdCmpFunc) (void *d1, void *d2);
typedef void (*EfsdHashItemFreeFunc) (EfsdHashItem *it);

EfsdHash         *efsd_hash_new(int num_buckets, int bucket_size,
				EfsdHashFunc hash_func, EfsdCmpFunc cmp_func,
				EfsdHashItemFreeFunc free_func);

void              efsd_hash_free(EfsdHash *h);
int               efsd_hash_insert(EfsdHash *h, void *key, void *data);

/* Returns NULL if item not found or the data of the
   EfsdHashItem if found. */
void             *efsd_hash_find(EfsdHash *h, void *key);
void              efsd_hash_remove(EfsdHash *h, void *key, EfsdFunc free_func);
void              efsd_hash_change_key(EfsdHash *h, void *key1, void *key2);

int               efsd_hash_num_buckets(EfsdHash *h);
int               efsd_hash_max_bucket_size(EfsdHash *h);

/* Standard hash functions: */
unsigned int      efsd_hash_string(EfsdHash *h, char *data);

/* Hash iterators -- iterates over all items
   that are stored inside a hash table */
EfsdHashIterator *efsd_hash_it_new(EfsdHash *h);
void              efsd_hash_it_free(EfsdHashIterator *it);
EfsdHashItem     *efsd_hash_it_item(EfsdHashIterator *it);
int               efsd_hash_it_next(EfsdHashIterator *it);
int               efsd_hash_it_valid(EfsdHashIterator *it);

#endif
