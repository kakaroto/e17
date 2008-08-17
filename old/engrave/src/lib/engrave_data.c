#include "engrave_private.h"
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
EAPI Engrave_Data *
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
EAPI void
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
EAPI const char *
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
EAPI const char *
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
EAPI int
engrave_data_int_value_get(Engrave_Data *ed)
{
  return (ed ? ed->int_value : 0);
}

/**
 * engrave_data_value_set - set the value of the data object
 * @param ed: The Engrave_Data to set the value into
 * @param value: The value to set.
 * 
 * @return Returns no value.
 */
EAPI void
engrave_data_value_set(Engrave_Data *ed, const char *value)
{
    if (!ed) return;
    IF_FREE(ed->value);
    ed->value = (value ? strdup(value) : NULL);
}

/**
 * engrave_data_int_value_set - set the int value of the data object
 * @param ed: The Engrave_Data to set the value into
 * @param value: The value to set
 * 
 * @return Returns no value
 */
EAPI void
engrave_data_int_value_set(Engrave_Data *ed, int value)
{
    if (!ed) return;
    ed->int_value = value;
}

/**
 * engrave_data_parent_set - set the parent pointer on this node
 * @param ed: The Engrave_Data to set the parent on
 * @param parent: The parent pointer to set
 *
 * @return Returns no value.
 * @brief This will set the parent pointer, in the case of Engrave_Data this
 * can be either an Engrave_File or an Engrave_Group.
 */
EAPI void
engrave_data_parent_set(Engrave_Data *ed, void *parent)
{
    if (!ed) return;
    ed->parent = parent;
}

/**
 * engrave_data_parent_get - get the parent pointer on this node
 * @param ed: The Engrave_Data to get the pointer from
 * 
 * @return Returns the parent pointer, or NULL if none set.
 */
EAPI void *
engrave_data_parent_get(Engrave_Data *ed)
{
    return (ed ? ed->parent : NULL);
}


