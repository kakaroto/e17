#include "epplet.h"
#include "utils.h"

/* Sorry about these next two functions...
 * I kind of needed them to do a quick port...
 * I'll fix this up later :)
 */

char               *
_Strdup(char *s)
{
   char               *ss;
   int                 len;

   if (!s)
      return NULL;
   len = strlen(s);
   ss = malloc(len + 1);
   if (!ss)
      return NULL;
   memcpy(ss, s, len + 1);
   return ss;
}

char               *
_Strjoin(const char *separator, ...)
{
   char               *string, *s;
   va_list             args;
   int                 len;
   int                 separator_len;

   if (separator == NULL)
      separator = "";

   separator_len = strlen(separator);
   va_start(args, separator);
   s = va_arg(args, char *);

   if (s)
     {
	len = strlen(s);
	s = va_arg(args, char *);

	while (s)
	  {
	     len += separator_len + strlen(s);
	     s = va_arg(args, char *);
	  }
	va_end(args);
	string = malloc(sizeof(char) * (len + 1));
	*string = 0;
	va_start(args, separator);
	s = va_arg(args, char *);

	strcat(string, s);
	s = va_arg(args, char *);

	while (s)
	  {
	     strcat(string, separator);
	     strcat(string, s);
	     s = va_arg(args, char *);
	  }
     }
   else
      string = _Strdup("");
   va_end(args);

   return string;
}
