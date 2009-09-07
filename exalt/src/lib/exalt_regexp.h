/** @file exalt_regexp.h */
#ifndef REGEX_H
#define REGEX_H

#include <stdio.h>
#include <stdlib.h>
#include "Exalt.h"
#include <string.h>
#include <regex.h>

/**
 * @defgroup Exalt_Regexp
 * @brief Create and execute a regular expression.
 * @ingroup Exalt
 * @{
 */


/** Partial IP regexp (only 1 number) */
#define REGEXP_IP_PARTIAL "([01]?[0-9][0-9]?|2[0-4][0-9]|25[0-5])"
/** Complete IP IP regexp */
#define REGEXP_IP   REGEXP_IP_PARTIAL "\\."  REGEXP_IP_PARTIAL "\\."  REGEXP_IP_PARTIAL "\\."  REGEXP_IP_PARTIAL
/** IP regexp when the string request contains only a ip */
#define REGEXP_IS_IP "^"  REGEXP_IP "$"

/** Regexp to test if a key is a correct WEP hexadecimal key */
#define REGEXP_IS_WEP_HEXA "^[0-9A-F]{10}$|^[0-9A-F]{26}$"

/** Represents an regular expression */
typedef struct Exalt_Regex Exalt_Regex;

struct Exalt_Regex
{
    char* str_request;
    char* str_regex;

    char** res;
    size_t nmatch;
    short debug;
};

/**
 * @brief Create a new regexp
 * @param str_request the request string
 * @param str_regex the regular expression
 * @debug 1 if you want use the debug mode, else 0
 * @return Returns a Exalt_Regexp structure
 */
EAPI Exalt_Regex* exalt_regex_new(const char* str_request, const char* str_regex,short debug);
/**
 * @brief Update the request string
 * @param r the Exalt_Regexp
 * @param str_request the new request string
 */
EAPI void exalt_regex_set_request(Exalt_Regex* r,const char* str_request);
/**
 * @brief Update the regular expression
 * @param r the Exalt_Regexp
 * @param str_regex the new regular expression
 */
EAPI void exalt_regex_set_regex(Exalt_Regex* r,const char* str_regex);
/**
 * @brief Activate/deactivate the debug mode
 * @param r the Exalt_Regexp
 * @param debug the new debug mode status, 0 or 1
 */
EAPI void exalt_regex_set_debug(Exalt_Regex *r, short debug);
/**
 * @brief Clear the current result
 * @param r the Exalt_Regexp
 */
EAPI void exalt_regex_clear_result(Exalt_Regex* r);
/**
 * @brief Free the regexp
 * @param r the Exalt_Regexp
 */
EAPI void exalt_regex_free(Exalt_Regex **r);
/**
 * @brief Execute a regular expression
 * @param r the Exalt_Regexp
 * @return Return 1 if the regular expression is found in the request string, else 0
 */
EAPI int exalt_regex_execute(Exalt_Regex* r);

/** @} */

#endif

