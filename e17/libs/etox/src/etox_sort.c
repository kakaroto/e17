#include "Etox_private.h"
#include "Etox.h"

Etox_Sort
_etox_sort_new (int size)
{
  Etox_Sort sort;

  if (size < 0)
    return NULL;

  sort = (Etox_Sort) malloc (sizeof (struct _Etox_Sort));
  sort->value = (void **) calloc (size, sizeof (void *));
  sort->data_added = 0;
  sort->size = size;

  return sort;
}

void
_etox_sort_free (Etox_Sort sort)
{
  if (!sort)
    return;
  IF_FREE (sort->value);
  FREE (sort);
}

int
_etox_sort_get_size (Etox_Sort sort)
{
  if (!sort)
    return -1;

  return sort->size;
}

void
_etox_sort_set_data_from_list (Etox_Sort sort, Ewd_List * list)
{
  void *ptr;
  int i;

  if (!sort || !list)
    return;

  ewd_list_goto_first (list);
  for (i = 0; (ptr = ewd_list_next (list)) && (i <= sort->size); i++)
    sort->value[i] = ptr;

  sort->data_added = 1;
}

void *
_etox_sort_get_data (Etox_Sort sort, int index)
{
  if (!sort || !sort->data_added)
    return NULL;

  if ((index < 0) || (index > sort->size))
    return NULL;

  return sort->value[index];
}

void
_etox_sort_swap (Etox_Sort sort, int left, int right)
{
  void *ptr;

  if (!sort)
    return;

  ptr = sort->value[left];
  sort->value[left] = sort->value[right];
  sort->value[right] = ptr;
}

void
_etox_sort_now (Etox_Sort sort, int left, int right,
		int (*compare) (void *, void *))
{
  int i, last;

  if (!sort || !sort->data_added || !compare)
    return;

  if (left >= right)
    return;

  _etox_sort_swap (sort, left, (left + right) / 2);
  last = left;
  for (i = left + 1; i <= right; i++)
    if ((*compare) (sort->value[i], sort->value[left]) < 0)
      _etox_sort_swap (sort, ++last, i);
  _etox_sort_swap (sort, left, last);
  _etox_sort_now (sort, left, last, compare);
  _etox_sort_now (sort, last + 1, right, compare);
}
