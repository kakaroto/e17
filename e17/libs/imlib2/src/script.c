#include "common.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <errno.h>
#include <X11/Xlib.h>
#include "image.h"
#include "file.h"
#include "script.h"
#include "loaderpath.h"
/*
#define FDEBUG 1
*/

int find_string( char *haystack, char *needle )
{
   if(  strstr( haystack, needle ) != NULL )
     return ( strstr( haystack, needle ) - haystack );
   return 0;
}

char *stripwhitespace( char *str )
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

char *copystr( char *str, int start, int end )
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

pIFunctionParam parse_param( char *param )
{
   int i;
   
   IFunctionParam *ptr;
   if( param != NULL )
   {
      ptr = malloc( sizeof( IFunctionParam ) );
      i = find_string(param,"=");
      if( i > 0 )
      {
	 ptr->key = copystr(param, 0, i-1);
	 ptr->type = 1;
	 if( param[i+1] == '\"' )
	   ptr->data = (void *)copystr(param, i+2, strlen(param)-2);
	 else
	   ptr->data = (void *)copystr(param, i+1, strlen(param)-1);
#ifdef FDEBUG
	 printf( "DEBUG:   -> param [%s]=\"%s\"\n", ptr->key, (char *)ptr->data );
#endif /* FDEBUG */
	 ptr->next = NULL;
      }
      else
      {
	 ptr->key = strdup(param);
	 ptr->data = strdup( "" );
	 ptr->type = VAR_CHAR;
	 printf( "DEBUG:   -> s_param [%s]=\"%s\"\n", ptr->key, (char *)ptr->data );
	 ptr->next = NULL;
      }
      return ptr;
   }
   return NULL;
}

pIFunction parse_function( char *func )
{
   int i = 0, in_quote=0, start=0;
   IFunction *ptrfunc = malloc( sizeof( IFunction ) );
   IFunctionParam *ptrparam = NULL;
   char *tmpbuf;
   
   if( func != NULL )
   {
      func = stripwhitespace( func );
      ptrfunc->name = copystr( func, 0, find_string( func, "(" ) - 1 );
#ifdef FDEBUG
      printf( "DEBUG:  Function name: \"%s\"\n", ptrfunc->name );
#endif /* FDEBUG */
      
      ptrfunc->params = malloc( sizeof( IFunctionParam ) );
      ptrfunc->params->key = strdup("");
      ptrfunc->params->data = strdup("");
      ptrfunc->params->type = VAR_CHAR;
      ptrfunc->params->next = NULL;
      ptrparam = ptrfunc->params;
      
      start = find_string( func, "(" ) + 1;
      for( i = start; i < strlen(func); i++ )
      {
	 if( func[i] == '\"' )
	    in_quote = (in_quote == 0 ? 1 : 0);
	 if( !in_quote && (func[i] == ',' || func[i] == ')') )
	 {
	    tmpbuf = copystr( func, start, i-1 );
	    ptrparam->next = parse_param( tmpbuf );
	    if( ptrparam->next != NULL )
	      ptrparam = ptrparam->next;
	    start = i+1;
	    if( tmpbuf && tmpbuf != NULL ) free(tmpbuf);
	 }
      }
      free( func );
   }
   else
   {
      ptrfunc->name = strdup( "root-function" );
      ptrfunc->params = NULL;
   }
   ptrfunc->next = NULL;
   return ptrfunc;
}

pIFunction __imlib_script_parse( char *script, ... )
{
   int i = 0, in_quote = 0, start = 0, hit_null = 0;
   IFunction *ptr = parse_function( NULL ), *tmp_fptr = NULL;
   IFunction *rptr = ptr;
   IFunctionParam *tmp_pptr;
   va_list param_list;
   void *tmpptr;

#ifdef FDEBUG   
   printf( "DEBUG: Imlib Script Parser Start.\n" );
#endif
   
   if( script != NULL )
   {
      /* scan the script line */
      for( i = 0; i < strlen(script); i++ )
      {
	 if( script[i] == '\"' )
	    in_quote = (in_quote == 0 ? 1 : 0);
	 if( ! in_quote && script[i] == ';' )
	 {
	    ptr->next = parse_function( copystr( script, start, i-1 ) );
	    ptr = ptr->next;
	    start = i+1;
	 }
      }
      /* righty now we need to traverse the whole of the function tree and see if we have hit
       * any [], if we have replace it with the next var off the list update the pointer, luverly
       * jubly */
      va_start( param_list, script );
      for( tmp_fptr = rptr->next; tmp_fptr != NULL; tmp_fptr = tmp_fptr->next )
      {
	 for( tmp_pptr = tmp_fptr->params; tmp_pptr != NULL; tmp_pptr = tmp_pptr->next )
	 {
	    if( strcmp( (char *)(tmp_pptr->data), "[]" ) == 0 )
	    {
	       if( !hit_null )
		 tmpptr = va_arg( param_list, void *);
	       else
		 tmpptr = NULL;
	       
	       if( tmpptr == NULL )
	       {
#ifdef FDEBUG
		  printf( "DEBUG: WARNING: We have hit a null. (dyn_filter.c)\n" );
#endif
		  hit_null = 1;
	       }
#ifdef FDEBUG
	       printf( "Setting %s::%s to a pointer.\n", tmp_fptr->name, tmp_pptr->key );
#endif
	       tmp_pptr->type = VAR_PTR;
	       tmp_pptr->data = ( tmpptr != NULL ? tmpptr : NULL );
#ifdef FDEBUG
	       printf( "%p\n", tmp_pptr->data );
#endif
	    }
	 }
      }
      va_end( param_list );
#ifdef FDEBUG      
      printf( "DEBUG: Imlib Script Parser End.\n" );
#endif      
      return rptr;
   }
   else
   {
#ifdef FDEBUG
      printf( "DEBUG: Imlib Script Parser End. (!!)\n" );
#endif
      return NULL;
   }
}

void __imlib_script_tidyup_params( IFunctionParam *param )
{
   if( param->next ){
     __imlib_script_tidyup_params( param->next );
   }
   free( param->key );
   if( param->type == VAR_CHAR )
     free( param->data );
   free( param );
}
	  
void __imlib_script_tidyup( IFunction *func )
{
   if( func->next != NULL )
     __imlib_script_tidyup( func->next );

   if( func->params != NULL )
     __imlib_script_tidyup_params( func->params );
   
   free( func->name );
   free( func );
}
