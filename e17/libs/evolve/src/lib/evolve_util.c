#include <string.h>
#include <ctype.h>

#include "evolve_private.h"

char *evolve_util_string_humanize(char *str)
{
   int i;
   char *ret;
   if (!str)
     return NULL;
   
   ret = strdup(str);
   for (i = 0; i < strlen(str); ++i)
     {
	if (ret[i] == '-' || ret[i] == '_')
	  ret[i] = ' ';
	
	if (i == 0 || (i > 0 && isspace(ret[i - 1])))
	  ret[i] = toupper(ret[i]);
     }
   return ret;
}
