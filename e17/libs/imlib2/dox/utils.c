#include "dox.h"

int __find_string( char *haystack, char *needle )
{
   if(  strstr( haystack, needle ) != NULL )
     return ( strstr( haystack, needle ) - haystack );
   return 0;
}

char *__stripwhitespace( char *str )
{
   int i, strt = 0, in_quote = 0;
   char *tmpstr = calloc( strlen(str)+1, sizeof(char) );
   for( i = 0; i < strlen(str); i++ )
   {
      if( str[i] == '\"' )
	in_quote = (in_quote == 0 ? 1 : 0);
      if( in_quote || ! isspace(*(str+i)) )
	tmpstr[strt++] = str[i];
   }
   strcpy( str, tmpstr );
   free(tmpstr);
   return str;
}

char *__copystr( char *str, int start, int end )
{
   int i = 0;
   char *rstr = calloc( 1024, sizeof( char ) );
   if( start <= end && end < strlen( str ) )
   {
      for( i = start; i <= end; i++ )
	rstr[i-start] = str[i];
      return rstr;
   }
   return NULL;
}
