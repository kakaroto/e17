/** @file etk_utils.c */
#include "etk_utils.h"
#include "etk_types.h"
#include "etk_signal.h"

/**
 * @brief Combines the boolean returned values together by performing an "OR" operation
 * @param return_value the location where to store the result
 * @param value_to_accum the new value to combine
 * @param data unused
 */
void etk_accumulator_bool_or(void *return_value, const void *value_to_accum, void *data)
{
   Etk_Bool *return_bool;
   const Etk_Bool *result_bool;

   if (!(return_bool = return_value) || !(result_bool = value_to_accum))
      return;

   *return_bool |= *result_bool;
}

/**
 * @brief Combines the boolean returned values together by performing an "AND" operation
 * @param return_value the location where to store the result
 * @param value_to_accum the new value to combine
 * @param data unused
 */
void etk_accumulator_bool_and(void *return_value, const void *value_to_accum, void *data)
{
   Etk_Bool *return_bool;
   const Etk_Bool *result_bool;

   if (!(return_bool = return_value) || !(result_bool = value_to_accum))
      return;

   *return_bool &= *result_bool;
}

/**
 * @brief Combines the boolean returned values together by performing an "OR" operation. @n
 * The accumulator will stop the propagation of the signal when a ETK_TRUE value is returned
 * @param return_value the location where to store the result
 * @param value_to_accum the new value to combine
 * @param data unused
 */
void etk_accumulator_stopping_bool_or(void *return_value, const void *value_to_accum, void *data)
{
   Etk_Bool *return_bool;
   const Etk_Bool *result_bool;

   if (!(return_bool = return_value) || !(result_bool = value_to_accum))
      return;

   *return_bool |= *result_bool;
   if (*result_bool)
      etk_signal_stop();
}

/**
 * @brief Combines the boolean returned values together by performing an "AND" operation. @n
 * The accumulator will stop the propagation of the signal when a ETK_FALSE value is returned
 * @param return_value the location where to store the result
 * @param value_to_accum the new value to combine
 * @param data unused
 */
void etk_accumulator_stopping_bool_and(void *return_value, const void *value_to_accum, void *data)
{
   Etk_Bool *return_bool;
   const Etk_Bool *result_bool;

   if (!(return_bool = return_value) || !(result_bool = value_to_accum))
      return;

   *return_bool &= *result_bool;
   if (!(*result_bool))
      etk_signal_stop();
}

/**
 * @brief A utility function to use as a swapped callback.
 * It sets to NULL the pointer located at the adress stored in @a data. @n
 * For example, if you want to set "pointer" to NULL when "button" is clicked, you can do: @n
 * etk_signal_connect_swapped(ETK_OBJECT(button), "clicked", ETK_CALLBACK(etk_callback_set_null), &pointer);
 * @param data @a data stores the location of the pointer to set to NULL
 */
void etk_callback_set_null(void *data)
{
   if (!data)
      return;
   *((void **)data) = NULL;
}
