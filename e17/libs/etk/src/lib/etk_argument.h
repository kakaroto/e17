/** @file etk_argument.h */
#ifndef _ETK_ARGUMENT_H_
#define _ETK_ARGUMENT_H_

#include "etk_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup Etk_Argument Argument parser
 * @brief A couple of functions to parse the arguments of a program
 * @{
 */

Etk_Bool etk_argument_is_set(int *argc, char ***argv, const char *long_name, char short_name, Etk_Bool remove);
Etk_Bool etk_argument_value_get(int *argc, char ***argv, const char *long_name, char short_name, Etk_Bool remove, char **value);

/** @} */

#ifdef __cplusplus
}
#endif

#endif
