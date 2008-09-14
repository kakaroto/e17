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
/** retrieve the interface name from /proc/net/dev */
#define REGEXP_PROCNNET_GET_NAME " *(.+):"

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


Exalt_Regex* exalt_regex_new(const char* str_request, const char* str_regex,short debug);
void exalt_regex_set_request(Exalt_Regex* r,const char* str_request);
void exalt_regex_set_regex(Exalt_Regex* r,const char* str_regex);
void exalt_regex_set_debug(Exalt_Regex *r, short debug);
void exalt_regex_free(Exalt_Regex **r);
int exalt_regex_execute(Exalt_Regex* r);

#endif

