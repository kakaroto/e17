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

#include <stdlib.h>
#include <string.h>

#include <efsd_debug.h>
#include <efsd_macros.h>
#include <efsd_misc.h>
#include <efsd_dynarray.h>

#define EFSD_DYN_CHAR_ARRAY_SIZE   200

struct efsd_dyn_char_array
{
  char              **array;
  int                 size;
  int                 index;

  char                is_sorted;
};


static char *dca_search(EfsdDynCharArray *a, const char *s, int *index);


static char *
dca_search(EfsdDynCharArray *a, const char *s, int *index)
{
  int upper, lower;
  int i = 0, cmp;

  D_ENTER;

  /* If the array is not sorted, we have to scan the
     array, as far as it's occupied. */

  if (!a->is_sorted)
    {
      for (i = 0; i < a->index; i++)
	{
	  if (!strcmp(a->array[i], s))
	    {
	      *index = i;
	      D_RETURN_(a->array[i]);
	    }
	}

      *index = a->index;
      D_RETURN_(NULL);
    }

  /* If it's sorted, we can do a binary search: */

  upper = a->index-1; lower = 0;

  while (upper >= lower)
    {
      i = (lower + upper)/2;
      cmp = strcmp(a->array[i], s);  

      printf("Comparing %s %s\n", a->array[i], s);  
      
      if (cmp == 0)
	{
	  *index = i;
	  D_RETURN_(a->array[i]);
	}
      else if (cmp < 0)
	lower = i+1;
      else
	upper = i-1;
    }

  /* Not found. */

  *index = lower;
  D_RETURN_(NULL);
}


EfsdDynCharArray  *
efsd_dca_new(void)
{
  EfsdDynCharArray *a;

  D_ENTER;

  a = NEW(EfsdDynCharArray);

  memset(a, 0, sizeof(EfsdDynCharArray));
  a->size = EFSD_DYN_CHAR_ARRAY_SIZE;
  a->array = malloc(sizeof(char*) * a->size);
  
  D_RETURN_(a);
}


void               
efsd_dca_free(EfsdDynCharArray *a)
{
  int i;

  D_ENTER;

  for (i = 0; i < a->index; i++)
    {
      FREE(a->array[i]);
    }

  FREE(a->array);
  FREE(a);

  D_RETURN;
}


void               
efsd_dca_append(EfsdDynCharArray *a, const char *s)
{
  D_ENTER;

  if (!a || !s || s[0] == '\0')
    D_RETURN;

  if (a->index == a->size)
    {
      a->array = realloc(a->array, sizeof(char*) * 2 * a->size);
      a->size *= 2;
    }

  a->array[a->index] = strdup(s);
  a->index++;
  a->is_sorted = FALSE;

  D_RETURN;
}


void               
efsd_dca_sort(EfsdDynCharArray *a)
{
  D_ENTER;
  
  if (!a || a->is_sorted)
    D_RETURN;

  if (a->index < 2)
    {
      /* Nothing to sort. */
      D_RETURN;
    }

  efsd_misc_quicksort(a->array, 0, a->index-1);
  a->is_sorted = TRUE;

  D_RETURN;
}


void               
efsd_dca_insert(EfsdDynCharArray *a, const char *s)
{
  int i, index;

  D_ENTER;
  
  if (!a || !s || s[0] == '\0')
    D_RETURN;

  if (!a->is_sorted)
    efsd_dca_sort(a);

  if (dca_search(a, s, &index) != NULL)
    {
      /* Item exists already. */
      D_RETURN;
    }
  
  if (a->index == a->size)
    {
      a->array = realloc(a->array, sizeof(char*) * 2 * a->size);
      a->size *= 2;
    }

  for (i = a->index-1; i >= index; i--)
    a->array[i+1] = a->array[i];
  
  a->array[index] = strdup(s);
  a->index++;

  D_RETURN;
}


void               
efsd_dca_remove(EfsdDynCharArray *a, const char *s)
{
  int i, index;

  D_ENTER;
  
  if (!a || !s || s[0] == '\0')
    D_RETURN;

  if (!a->is_sorted)
    efsd_dca_sort(a);

  if (dca_search(a, s, &index) == NULL)
    {
      /* Item doesn't exist. */
      D_RETURN;
    }

  FREE(a->array[index]);

  for (i = index; i < a->index; i++)
    a->array[i] = a->array[i+1];
  
  a->index--;

  if (a->index < a->size/2)
    {
      a->array = realloc(a->array, sizeof(char*) * (a->size/2));
      a->size /= 2;
    }

  D_RETURN;
}


char *
efsd_dca_get(EfsdDynCharArray *a, int i)
{
  D_ENTER;

  if (!a || i >= a->index)
    D_RETURN_(NULL);

  D_RETURN_(a->array[i]);
}

