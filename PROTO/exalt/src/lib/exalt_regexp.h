#ifndef REGEX_H
#define REGEX_H

#include <stdio.h>
#include <stdlib.h>
#include "libexalt.h"
#include <string.h>
#include <regex.h>


/** partial ip regexp (only 1 number) */
#define REGEXP_IP_PARTIAL "([01]?[0-9][0-9]?|2[0-4][0-9]|25[0-5])"
/** ip regexp */
#define REGEXP_IP   REGEXP_IP_PARTIAL "\\."  REGEXP_IP_PARTIAL "\\."  REGEXP_IP_PARTIAL "\\."  REGEXP_IP_PARTIAL
/** ip regexp when the string request contains only a ip */
#define REGEXP_IS_IP "^"  REGEXP_IP "$"

/** regexp to test if a key is a correct WEP hexadecimal key */
#define REGEXP_IS_WEP_HEXA "^[0-9A-F]{10}$|^[0-9A-F]{26}$"

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
 * @brief create a new regex
 * @param str_request the request string
 * @param str_regex the regular expression
 * @debug 1 if you want use the debug mode, else 0
 * @return Return a Exalt_Regex structure
 */
Exalt_Regex* exalt_regex_new(const char* str_request, const char* str_regex,short debug);
/**
 * @brief change the request string
 * @param r the Exalt_Regex
 * @param str_request the new request string
 */
void exalt_regex_set_request(Exalt_Regex* r,const char* str_request);
/**
 * @brief change the regular expression
 * @param r the Exalt_Regex
 * @param str_regex the new regular expression
 */
void exalt_regex_set_regex(Exalt_Regex* r,const char* str_regex);
/**
 * @brief set the debug mode
 * @param r the Exalt_Regex
 * @param debug the new debig mode, 0 or 1
 */
void exalt_regex_set_debug(Exalt_Regex *r, short debug);
/**
 * @brief clear the current result
 * @param r the Exalt_Regex
 */
void exalt_regex_clear_result(Exalt_Regex* r);
/*
 * @brief free tje regex
 * @param r the Exalt_Regex
 */
void exalt_regex_free(Exalt_Regex **r);
/*
 * @brief execute a regular expression
 * @param r the Exalt_Regex
 * @return Return 1 if the regular expression is found int the request string, else 0
 */
int exalt_regex_execute(Exalt_Regex* r);

#endif

