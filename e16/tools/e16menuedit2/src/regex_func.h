#ifndef _REGEX_FUNC_H
#define _REGEX_FUNC_H

#include <stdlib.h>
#include <regex.h>
#include <stdio.h>
#include <string.h>

struct substrings
{
  char *match;
  char *start;
  char *end;
};

int regex_match (const char *string, char *pattern);
int regex_match2 (regex_t *re, const char *string, char *pattern);
int regex_sub (const char *string, char *pattern, struct substrings **ss_ptr,
               int maxsub, int *numsub);
int regex_sub2 (regex_t *re, const char *string, char *pattern,
                struct substrings **ss_ptr, int maxsub, int *numsub);



#endif /* _REGEX_FUNC_H */
