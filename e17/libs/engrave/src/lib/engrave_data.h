#ifndef ENGRAVE_DATA_H
#define ENGRAVE_DATA_H

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
 * The Engrave_Data typedef.
 */
typedef struct _Engrave_Data Engrave_Data;

/**
 * Stores the information for the data object 
 */
struct _Engrave_Data
{
  char *key;     /**< The data key */
  char *value;   /**< The data string value */
  int int_value; /**< The data int value */
};

Engrave_Data * engrave_data_new(const char *key, const char *value);
void engrave_data_free(Engrave_Data *ed);

char *engrave_data_key_get(Engrave_Data *ed);
char *engrave_data_value_get(Engrave_Data *ed);
int engrave_data_int_value_get(Engrave_Data *ed);

/**
 * @}
 */

#endif

