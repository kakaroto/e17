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
engrave_data_new(char *key, char *value)
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

  if (ed->key) free(ed->key);
  ed->key = NULL;

  if (ed->value) free(ed->value);
  ed->value = NULL;

  free(ed);
  ed = NULL;
}

/**
 * engrave_data_key_get - get the key for this data
 * @param ed: The Engrave_Data to get the key from
 *
 * @return Returns a pointer to the key for this data or 
 * NULL on failure. This pointer must be free'd by the user.
 */
char *
engrave_data_key_get(Engrave_Data *ed)
{
  if (!ed) return NULL;
  return (ed->key ? strdup(ed->key) : NULL);
}

/**
 * engrave_data_value_get - get the string value of the data object
 * @param ed: The Engrave_Data to get the string value from.
 *
 * @return Returns a pointer to the string value of the data on success
 * or NULL on failure. This pointer must be free'd by the user.
 */
char *
engrave_data_value_get(Engrave_Data *ed)
{
  if (!ed) return NULL;
  return (ed->value ? strdup(ed->value) : NULL);
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


