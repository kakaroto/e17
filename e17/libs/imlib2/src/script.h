#ifndef _DYN_FUNCTION_H_
#define _DYN_FUNCTION_H_

#include "ltdl.h"
#include <stdarg.h>

#define VAR_CHAR 1
#define VAR_PTR  2

typedef struct _imlib_function_param IFunctionParam;
typedef struct _imlib_function_param *pIFunctionParam;
struct _imlib_function_param
{
   char            *key;
   int              type;
   void            *data;
   pIFunctionParam  next;
};

typedef struct _imlib_function IFunction;
typedef struct _imlib_function *pIFunction;
struct _imlib_function
{
   char            *name;
   pIFunctionParam  params;
   pIFunction        next;
};

pIFunction   __imlib_script_parse( char *script, va_list );
void         __imlib_script_tidyup( IFunction *func );
#endif /* _FUNCTION_H_ */
