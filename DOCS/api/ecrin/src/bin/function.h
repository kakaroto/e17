#ifndef __FUNCTION_H__
#define __FUNCTION_H__


#include <Ecore.h>

#include "description.h"



/*
 * Function parameter
 */

typedef struct _Ecrin_Function_Param Ecrin_Function_Param;
struct _Ecrin_Function_Param
{
  char *type;
  char *name;
};

Ecrin_Function_Param *ecrin_function_param_new  (char *type,
                                                 char *name);

void                  ecrin_function_param_free (Ecrin_Function_Param *param);

/*
 * Function
 */

typedef struct _Ecrin_Function Ecrin_Function;
struct _Ecrin_Function
{
  char              *name;
  char              *return_type;
  Ecore_List        *params;
  Ecrin_Description *description;
};

void            ecrin_function_display    (Ecrin_Function *function);

Ecrin_Function *ecrin_function_new        (void);

void            ecrin_function_name_set   (Ecrin_Function *function,
                                           char           *name);

void            ecrin_function_return_set (Ecrin_Function *function,
                                           char           *return_type);

void            ecrin_function_free       (Ecrin_Function *function);

void            ecrin_function_param_add  (Ecrin_Function       *function,
                                           Ecrin_Function_Param *param);

#endif /* __FUNCTION_H__ */
