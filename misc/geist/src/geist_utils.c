/* geist_utils.c */

#include "geist_utils.h"
#include "geist_list.h"

geist_list *
geist_string_split(const char *string, const char *delimiter)
{
   geist_list *string_list = NULL;
   char *s;
   unsigned int n = 1;

   D_ENTER(3);

   if (!string || !delimiter)
      D_RETURN(3, NULL);

   s = strstr(string, delimiter);
   if (s)
   {
      unsigned int delimiter_len = strlen(delimiter);

      do
      {
         unsigned int len;
         char *new_string;

         len = s - string;
         new_string = emalloc(sizeof(char) * (len + 1));

         strncpy(new_string, string, len);
         new_string[len] = 0;
         string_list = geist_list_add_front(string_list, new_string);
         n++;
         string = s + delimiter_len;
         s = strstr(string, delimiter);
      }
      while (s);
   }
   if (*string)
   {
      n++;
      string_list = geist_list_add_front(string_list, estrdup((char *)string));
   }

   string_list = geist_list_reverse(string_list);

   D_RETURN(3, string_list);
}

