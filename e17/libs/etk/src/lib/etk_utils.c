/** @file etk_utils.c */
#include "etk_utils.h"
#include <sys/time.h>
#include "etk_types.h"
#include "etk_signal.h"

/**
 * @addtogroup Etk_Utils
 * @{
 */
 
/**
 * @brief Combines the returned boolean values together by performing an "OR" operation
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
 * @brief Combines the returned boolean values together by performing an "AND" operation
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
 * @brief Combines the returned boolean values together by performing an "OR" operation. @n
 * The accumulator will stop the propagation of the signal as soon as a callback returns ETK_TRUE
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
 * @brief Combines the returned boolean values together by performing an "AND" operation. @n
 * The accumulator will stop the propagation of the signal as soon as a callback returns ETK_FALSE
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

/**
 * @brief Gets the current time, in milliseconds
 * @return Returns the current time, in milliseconds
 */
unsigned int etk_current_time_get()
{
   struct timeval timev;

   gettimeofday(&timev, NULL);
   return ((timev.tv_sec * 1000) + ((timev.tv_usec) / 1000));
}

/** @} */
