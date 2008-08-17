#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "define.h"
#include "util.h"


/*
 * Define
 */


void
ecrin_define_display (Ecrin_Define *define)
{
  char *p;
  char *iter;

  printf ("\n");
  printf ("#define %s", define->name);
  iter = define->value;
  if (define->value)
    {
      while ((p = strchr (iter, '\\')) != NULL)
        {
          char *str;
          int   l;
          
          l = p - iter;
          str = strndup (iter, l);
          printf (" %s\n", str);
          free (str);
          iter = p + 1;
        } 
    }
  printf (" %s\n", iter);
  remove_description (define->value);
}

Ecrin_Define *
ecrin_define_new (char              *name,
		  char              *value,
		  char               static_state,
		  Ecrin_Description *description)
{
  Ecrin_Define *define;

  define = (Ecrin_Define *)malloc (sizeof (Ecrin_Define));
  if (!define)
    return NULL;

  if (name)
    define->name = strdup (name);
  else
    define->name = NULL;
  
  if (value)
    define->value = remove_description (value);
  else
    define->value = NULL;

  define->static_state = static_state;
  define->description = description;

  return define;
}

void
ecrin_define_free (Ecrin_Define *define)
{
  if (!define)
    return;

  if (define->name)
    free (define->name);

  if (define->value)
    free (define->value);

  ecrin_description_free (define->description);
}
