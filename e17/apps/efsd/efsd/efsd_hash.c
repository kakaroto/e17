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
#if HAVE_CONFIG_H
# include <config.h>
#endif
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <efsd_macros.h>
#include <efsd_debug.h>
#include <efsd_misc.h>
#include <efsd_list.h>
#include <efsd_hash.h>
#include <efsd_lock.h>

struct efsd_hash
{
  EfsdList     **buckets;
  EfsdList     **bucket_ends;
  int           *bucket_sizes;
  int            num_buckets;
  int            max_bucket_size;

  EfsdHashFunc          hash_func;
  EfsdCmpFunc           cmp_func;
  EfsdHashItemFreeFunc  free_func;
};


struct efsd_hash_iterator
{
  EfsdHash      *h;
  EfsdList      *it;
  int            bucket;
};


EfsdHash *
efsd_hash_new(int num_buckets, int bucket_size, EfsdHashFunc hash_func,
	      EfsdCmpFunc cmp_func, EfsdHashItemFreeFunc free_func)
{
  EfsdHash *h;

  D_ENTER;

  h = NEW(EfsdHash);
  memset(h, 0, sizeof(EfsdHash));

  h->num_buckets = num_buckets;
  h->max_bucket_size = bucket_size;
  h->hash_func = hash_func;
  h->cmp_func = cmp_func;
  h->free_func = free_func;

  h->buckets      = calloc(num_buckets, sizeof(EfsdList*));
  h->bucket_ends  = calloc(num_buckets, sizeof(EfsdList*));
  h->bucket_sizes = calloc(num_buckets, sizeof(int));

  D_RETURN_(h);
}


void          
efsd_hash_free(EfsdHash* h)
{
  int i;

  D_ENTER;

  if (!h)
    D_RETURN;

  for (i = 0; i < h->num_buckets; i++)
    {
      if (h->buckets[i])
	{
	  efsd_list_free(h->buckets[i], (EfsdFunc)h->free_func);
	  h->buckets[i] = NULL;
	  h->bucket_ends[i] = NULL;
	}
    }

  FREE(h);
  D_RETURN;
}


int
efsd_hash_insert(EfsdHash *h, void *key, void *data)
{
  EfsdHashItem  *it;
  unsigned int   index;

  D_ENTER;

  if (efsd_hash_find(h, key))
    {
      D("Item exists -- not inserting.\n");
      D_RETURN_(FALSE);
    }

  index = h->hash_func(h, key);

  it = NEW(EfsdHashItem);
  it->data = data;
  it->key  = key;

  if (h->bucket_sizes[index] + 1 == h->max_bucket_size)
    {
      EfsdList *new_last;

      D("Hash bucket overflow -- removing last item.\n");
      
      new_last = efsd_list_prev(h->bucket_ends[index]);
      h->buckets[index] = efsd_list_remove(h->buckets[index],
					   h->bucket_ends[index], (EfsdFunc)h->free_func);
      h->bucket_ends[index] = new_last;
      h->bucket_sizes[index]--;
    }

  h->buckets[index] = efsd_list_prepend(h->buckets[index], it);

  if (!h->bucket_ends[index])
    h->bucket_ends[index] = h->buckets[index];
  
  h->bucket_sizes[index]++;
  D_RETURN_(TRUE);
}


void  *
efsd_hash_find(EfsdHash *h, void *key)
{
  EfsdList     *l;
  unsigned int  index;
  int           i;

  D_ENTER;

  if (!h || !key)
    D_RETURN_(NULL);

  index = h->hash_func(h, key);

  for (i = 1, l = h->buckets[index]; l; l = efsd_list_next(l), i++)
    {
      if (h->cmp_func(key, ((EfsdHashItem*)efsd_list_data(l))->key) == 0)
	{
	  /* D("Key with hash value %i found at %i. item\n", index, i); */
	  h->buckets[index] = efsd_list_move_to_front(h->buckets[index], l);
	  D_RETURN_(((EfsdHashItem*)efsd_list_data(l))->data);
	}
    }

  /* D("Key with hash value %i not found.\n", index); */
  D_RETURN_(NULL);
}


void
efsd_hash_remove(EfsdHash *h, void *key)
{
  EfsdList     *l = NULL;
  uint         index;

  D_ENTER;

  if (!h || !key)
    D_RETURN;

  index = h->hash_func(h, key);

  for (l = h->buckets[index]; l; l = efsd_list_next(l))
    {
      if (h->cmp_func(key, ((EfsdHashItem*)efsd_list_data(l))->key) == 0)
	break;
    }

  if (l)
    {
      h->buckets[index] = efsd_list_remove(h->buckets[index], l, (EfsdFunc)h->free_func);
      h->bucket_sizes[index]--;
      D("Removed item hashed at %i\n", index);	  
    }
  else
    {
      D("Item hashed at %i not removed.\n", index);	  
    }

  D_RETURN;
}


void              
efsd_hash_change_key(EfsdHash *h, void *key1, void *key2)
{
  EfsdList     *l = NULL;
  uint         index;
  void         *it;

  D_ENTER;

  if (!h || !key1 || !key2)
    D_RETURN;
  
  index = h->hash_func(h, key1);

  for (l = h->buckets[index]; l; l = efsd_list_next(l))
    {
      if (h->cmp_func(key1, ((EfsdHashItem*)efsd_list_data(l))->key) == 0)
	break;
    }

  if (l)
    {
      it = efsd_list_data(l);
      h->buckets[index] = efsd_list_remove(h->buckets[index], l, NULL);
      h->bucket_sizes[index]--;

      index = h->hash_func(h, key2);
      if (h->bucket_sizes[index] + 1 == h->max_bucket_size)
	{
	  EfsdList *new_last;
	  
	  D("Hash bucket overflow in re-keying -- removing last item.\n");
	  
	  new_last = efsd_list_prev(h->bucket_ends[index]);
	  h->buckets[index] = efsd_list_remove(h->buckets[index],
					       h->bucket_ends[index], (EfsdFunc)h->free_func);
	  h->bucket_ends[index] = new_last;
	  h->bucket_sizes[index]--;
	}

      h->buckets[index] = efsd_list_prepend(h->buckets[index], it);

      if (!h->bucket_ends[index])
	h->bucket_ends[index] = h->buckets[index];
      
      h->bucket_sizes[index]++;
    }
  else
    {
      D("Item hashed at %i not re-keyed.\n", index);	  
    }


  D_RETURN;
}


int       
efsd_hash_num_buckets(EfsdHash *h)
{
  D_ENTER;

  D_RETURN_(h->num_buckets);
}


int       
efsd_hash_max_bucket_size(EfsdHash *h)
{
  D_ENTER;

  D_RETURN_(h->max_bucket_size);
}


unsigned int 
efsd_hash_string(EfsdHash *h, char *s)
{
  char *ss;
  unsigned int hash;

  D_ENTER;

  ss = s;

  for (hash = 0; *s != '\0'; s++)
    hash = (64*hash + *s) % h->num_buckets;
  
  /* D("String '%s' hashed to %i\n", ss, hash); */

  D_RETURN_(hash);
}


/* Hash iterator stuff below: */

EfsdHashIterator *
efsd_hash_it_new(EfsdHash *h)
{
  int i;
  EfsdHashIterator *it = NULL;

  D_ENTER;

  if (!h)
    D_RETURN_(NULL);

  for (i = 0; i < h->num_buckets; i++)
    {
      if (h->buckets[i] != NULL)
	{
	  it = NEW(EfsdHashIterator);

	  it->h = h;
	  it->bucket = i;
	  it->it = h->buckets[i];

	  D_RETURN_(it);
	}
    }

  D_RETURN_(NULL);
}


void              
efsd_hash_it_free(EfsdHashIterator *it)
{
  D_ENTER;

  FREE(it);

  D_RETURN;
}


EfsdHashItem     *
efsd_hash_it_item(EfsdHashIterator *it)
{
  D_ENTER;

  if (!it)
    D_RETURN_(NULL);
  
  D_RETURN_((EfsdHashItem*)efsd_list_data(it->it));
}


int               
efsd_hash_it_next(EfsdHashIterator *it)
{
  int i;

  D_ENTER;

  if (!it)
    D_RETURN_(FALSE);

  if (it->it)
    {
      it->it = efsd_list_next(it->it);

      if (it->it)
	{
	  D_RETURN_(TRUE);
	}

      for (i = it->bucket + 1; i < it->h->num_buckets; i++)
	{
	  if (it->h->buckets[i] != NULL)
	    {
	      it->bucket = i;
	      it->it = it->h->buckets[i];
	      D_RETURN_(TRUE);
	    }
	}
    }

  it->it = NULL;

  D_RETURN_(FALSE);
}


int               
efsd_hash_it_valid(EfsdHashIterator *it)
{
  D_ENTER;

  if (!it)
    D_RETURN_(FALSE);

  if (it->it != NULL)
    D_RETURN_(TRUE);

  D_RETURN_(FALSE);
}
