#include "function.h"



Ecrin_Function_Param *
ecrin_function_param_new (char *type,
                          char *name)
{
  Ecrin_Function_Param *param;

  param = (Ecrin_Function_Param *)malloc (sizeof (Ecrin_Function_Param));
  if (!param)
    return NULL;

  if (type)
    param->type = strdup (type);
  else
    param->type = NULL;

  if (name)
    param->name = strdup (name);
  else
    param->name = NULL;

  return param;
}

void
ecrin_function_param_free (Ecrin_Function_Param *param)
{
  if (!param)
    return;

  if (param->type)
    free (param->type);

  if (param->name)
    free (param->name);
}


void
ecrin_function_display (Ecrin_Function *function)
{
  Ecrin_Function_Param *param;
  char                  str[4096];
  char                  space[4096];

  printf ("\n");
  snprintf (str, 4096, "%s %s (", function->return_type, function->name);
  printf ("%s", str);
  memset (space, ' ', 4096);
  space[strlen (str)] = '\0';
  ecore_list_first_goto (function->params);
  while ((param = ecore_list_next (function->params)))
    {
      if (ecore_list_index (function->params) != 1)
        printf ("%s", space);
      printf ("%s", param->type);
      if (param->name)
        printf (" %s", param->name);
      if (ecore_list_index (function->params) != ecore_list_count (function->params))
	printf (",\n");
      else
        printf (")\n");
    }
}

Ecrin_Function *
ecrin_function_new (void)
{
  Ecrin_Function *function;

  function = (Ecrin_Function *)malloc (sizeof (Ecrin_Function));
  if (!function)
    return NULL;
  
  function->name = NULL;
  function->return_type = NULL;
  function->params = ecore_list_new ();
  function->description = NULL;

  return function;
}

void
ecrin_function_name_set (Ecrin_Function *function,
                         char           *name)
{
  if (!function)
    return;
  
  if (name)
    {
      if (function->name)
	free (function->name);
      function->name = strdup (name);
    }
  else
    function->name = NULL;
}

void
ecrin_function_return_set (Ecrin_Function *function,
                                char           *return_type)
{
  if (!function)
    return;
  
  if (return_type)
    {
      if (function->return_type)
	free (function->return_type);
      function->return_type = strdup (return_type);
    }
  else
    function->return_type = NULL;
}

void
ecrin_function_free (Ecrin_Function *function)
{
  Ecrin_Function_Param *param;

  if (!function)
    return;

  if (function->name)
    free (function->name);

  if (function->return_type)
    free (function->return_type);

  /* free the list */
  ecore_list_first_goto (function->params);
  while ((param = ecore_list_next (function->params)))
    {
      ecrin_function_param_free (param);
    }
  ecore_list_destroy (function->params);

  ecrin_description_free (function->description);

  free (function);
}

void
ecrin_function_param_add (Ecrin_Function       *function,
                          Ecrin_Function_Param *param)
{
  if ((!function) || (!param))
    return;

  ecore_list_append (function->params, param);
}
