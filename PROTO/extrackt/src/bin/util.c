#include "Extrackt.h"

char *
ex_util_strchomp(char *string)
{
   int len;
   
   if(string == NULL) return NULL;

   len = strlen (string);
   while (len--)
     {
	if (string[len] == ' ')
	  string[len] = '\0';
	else
	  break;
     }
   
   return string;
}

char *
ex_util_strndup (const char *str, int n)
{
   char *new_str;
   
   if (str)
     {
	new_str = E_NEW(n + 1, char);
	strncpy(new_str, str, n);
	new_str[n] = '\0';
     }
   else
     new_str = NULL;
   
   return new_str;
}

char *
ex_util_num_to_str(int num, int max)
{
   int   digits_num;
   int   digits_max;
   int   num_orig;
   int   i;
   char *ret;
   
   num_orig = num;
   digits_num = 0;
   digits_max = 0;
   
   while (num > 0)
     {
	        num /= 10;
	++digits_num;
     }
   
   while(max > 0)
     {
	max /= 10;
	++digits_max;
     }
   
   ret = E_NEW(digits_max + 1, char);
   
   if(digits_num == digits_max)
     {
	snprintf(ret, digits_max + 1, "%d", num_orig);
	return ret;
     }
   
   for(i = 0; i < digits_max - digits_num; i++)
     snprintf(ret + i, 2, "0");
   
   snprintf(ret + i, digits_num + 1, "%d", num_orig);
   
   return ret;
}
