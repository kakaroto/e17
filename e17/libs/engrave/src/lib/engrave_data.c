#include <Engrave.h>

/**
 * @file engrave_data.h Engrave_Data block object
 * @brief Contains all of the functions to manipulate Engrave_Data objects.
 */

/**
 * @defgroup Engrave_Data Engrave_Data: Functions to work with data blocks
 *
 * @{
 */

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
 * @}
 */

