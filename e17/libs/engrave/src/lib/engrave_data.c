#include <Engrave.h>
#include "engrave_macros.h"

/**
 * engrave_data_new - create a new data block 
 * @param key: the key to access the block
 * @param value: the value to store in the block
 *
 * @return Returns a pointer to a newly allocated data block on success, 
 * NULL on failure.
 */
Engrave_Data *
engrave_data_new(const char *key, const char *value)
{
  Engrave_Data *data;
  data = NEW(Engrave_Data, 1);
  if (!data) return NULL;

  data->key = (key ? strdup(key) : NULL);
  data->value = (value ? strdup(value) : NULL);
  return data;
}

/**
 * engrave_data_free - free the given data block
 * @param ed: The Engrave_Data to free
 *
 * @return Returns no value.
 */
void
engrave_data_free(Engrave_Data *ed)
{
  if (!ed) return;

  IF_FREE(ed->key);
  IF_FREE(ed->value);
  FREE(ed);
}

/**
 * engrave_data_key_get - get the key for this data
 * @param ed: The Engrave_Data to get the key from
 *
 * @return Returns the key for this data or NULL on failure.
 */
const char *
engrave_data_key_get(Engrave_Data *ed)
{
  return (ed ? ed->key : NULL);
}

/**
 * engrave_data_value_get - get the string value of the data object
 * @param ed: The Engrave_Data to get the string value from.
 *
 * @return Returns the value of the data on success or NULL on failure.
 */
const char *
engrave_data_value_get(Engrave_Data *ed)
{
  return (ed ? ed->value : NULL);
}

/**
 * engrave_data_int_value_get - get the int value of the data object
 * @param ed: The Engrave_Data to get the value from
 * 
 * @return Returns the int value of the data or 0 if its not set.
 */
int
engrave_data_int_value_get(Engrave_Data *ed)
{
  return (ed ? ed->int_value : 0);
}


