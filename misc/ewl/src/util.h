#ifndef _EWL_UTIL_H_
#define _EWL_UTIL_H_

#include "includes.h"
#include "debug.h"
#include "string.h"

#define BOUNDS(val,base,delta) (((val)>=((base)-(delta))) && \
                                ((val)<=((base)+(delta))))

char    *e_string_dup(char *str);
EwlBool  e_check_bool_string(char *str);

#endif /* _EWL_UTIL_H_ */
