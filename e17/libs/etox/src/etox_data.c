#include "Etox_private.h"
#include "Etox.h"

Etox_Data
_etox_data_new(int size)
{
  Etox_Data data;
  int i;

  if (size < 0)
    return NULL;

  data = (Etox_Data) malloc(sizeof(struct _Etox_Data));
  data->value = (void **) calloc(size, sizeof(void *));
  for (i = 0; i <= size; i++)
    data->value[i] = NULL;
  data->data_added = 0;
  data->size = size;

  return data;
}

void
_etox_data_free(Etox_Data data)
{
  if (!data)
    return;
  IF_FREE(data->value);
  FREE(data);
}

int
_etox_data_get_size(Etox_Data data)
{
  if (!data)
    return -1;

  return data->size;
}

void
_etox_data_set(Etox_Data data, int index, void *value)
{
  if (!data || (index < 0) || (index > data->size))
    return;

  data->value[index] = value;
  data->data_added = 1;
}

void
_etox_data_set_from_list(Etox_Data data, Ewd_List * list)
{
  void *ptr;
  int i;

  if (!data || !list)
    return;

  ewd_list_goto_first(list);
  for (i = 0; (ptr = ewd_list_next(list)) && (i <= data->size); i++)
    data->value[i] = ptr;

  data->data_added = 1;
}

void *
_etox_data_get(Etox_Data data, int index)
{
  if (!data || !data->data_added)
    return NULL;

  if ((index < 0) || (index > data->size))
    return NULL;

  return data->value[index];
}

void
_etox_data_swap(Etox_Data data, int left, int right)
{
  void *ptr;

  if (!data)
    return;

  ptr = data->value[left];
  data->value[left] = data->value[right];
  data->value[right] = ptr;
}

void
_etox_data_sort(Etox_Data data, int left, int right,
		int (*compare) (void *, void *))
{
  int i, last;

  if (!data || !data->data_added || !compare)
    return;

  if (left >= right)
    return;

  _etox_data_swap(data, left, (left + right) / 2);
  last = left;
  for (i = left + 1; i <= right; i++)
    if ((*compare) (data->value[i], data->value[left]) < 0)
      _etox_data_swap(data, ++last, i);
  _etox_data_swap(data, left, last);
  _etox_data_sort(data, left, last, compare);
  _etox_data_sort(data, last + 1, right, compare);
}
