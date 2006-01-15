/** @file etk_utils.c */
#include "etk_utils.h"
#include "etk_types.h"
#include "etk_signal.h"

/**
 * @brief Combine boolean returned values together by performing an "OR" operation
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
 * @brief Combine boolean returned values together by performing an "AND" operation
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
 * @brief Combine boolean returned values together by performing an "OR" operation. @n
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
 * @brief Combine boolean returned values together by performing an "AND" operation. @n
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
