#ifndef _DYN_FUNCTION_H_
#define _DYN_FUNCTION_H_

#include "ltdl.h"
#include "Imlib2.h"
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

typedef struct _imlib_variable
{
   void                   *ptr;
   struct _imlib_variable *next;
} IVariable;

Imlib_Image     __imlib_script_parse( Imlib_Image im, char *script, va_list );
IFunctionParam *__imlib_script_parse_parameters( Imlib_Image im, char *parameters );
Imlib_Image     __imlib_script_parse_function( Imlib_Image im, char *function );
void            __imlib_script_tidyup();
void           *__imlib_script_get_next_var();
void            __imlib_script_add_var( void *ptr );

#endif /* _FUNCTION_H_ */
