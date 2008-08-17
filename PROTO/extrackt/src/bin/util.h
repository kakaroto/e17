#ifndef _EX_UTIL_H
#define _EX_UTIL_H

#define ex_util_strstrip( string ) ex_util_strchomp (string)
char *ex_util_strchomp(char *string);
char *ex_util_strndup (const char *str, int n);
char *ex_util_num_to_str(int num, int max);
    
#endif
