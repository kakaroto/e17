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
#include "Imlib2.h"
#include "image.h"
#include "file.h"
#include "dynamic_filters.h"
#include "script.h"
#include "loaderpath.h"

/*
#define FDEBUG 1
*/
int __imlib_find_string( char *haystack, char *needle )
{
   if(  strstr( haystack, needle ) != NULL )
     return ( strstr( haystack, needle ) - haystack );
   return 0;
}

char *__imlib_stripwhitespace( char *str )
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

char *__imlib_copystr( char *str, int start, int end )
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

pIFunctionParam __imlib_parse_param( char *param )
{
   int i;
   
   IFunctionParam *ptr;
   if( param != NULL )
   {
      ptr = malloc( sizeof( IFunctionParam ) );
      i = __imlib_find_string(param,"=");
      if( i > 0 )
      {
	 ptr->key = __imlib_copystr(param, 0, i-1);
	 ptr->type = 1;
	 if( param[i+1] == '\"' )
	   ptr->data = (void *)__imlib_copystr(param, i+2, strlen(param)-2);
	 else
	   ptr->data = (void *)__imlib_copystr(param, i+1, strlen(param)-1);
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
#ifdef FDEBUG
	 printf( "DEBUG:   -> s_param [%s]=\"%s\"\n", ptr->key, (char *)ptr->data );
#endif
	 ptr->next = NULL;
      }
      return ptr;
   }
   return NULL;
}

pIFunction __imlib_parse_function( char *func )
{
   int i = 0, in_quote=0, start=0;
   IFunction *ptrfunc = malloc( sizeof( IFunction ) );
   IFunctionParam *ptrparam = NULL;
   char *tmpbuf;
   
   if( func != NULL )
   {
      func = __imlib_stripwhitespace( func );
      ptrfunc->name = __imlib_copystr( func, 0, __imlib_find_string( func, "(" ) - 1 );
#ifdef FDEBUG
      printf( "DEBUG:  Function name: \"%s\"\n", ptrfunc->name );
#endif /* FDEBUG */
      
      ptrfunc->params = malloc( sizeof( IFunctionParam ) );
      ptrfunc->params->key = strdup("");
      ptrfunc->params->data = strdup("");
      ptrfunc->params->type = VAR_CHAR;
      ptrfunc->params->next = NULL;
      ptrparam = ptrfunc->params;
      
      start = __imlib_find_string( func, "(" ) + 1;
      for( i = start; i < strlen(func); i++ )
      {
	 if( func[i] == '\"' )
	    in_quote = (in_quote == 0 ? 1 : 0);
	 if( !in_quote && (func[i] == ',' || func[i] == ')') )
	 {
	    tmpbuf = __imlib_copystr( func, start, i-1 );
	    ptrparam->next = __imlib_parse_param( tmpbuf );
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

pIFunction __imlib_script_parse( Imlib_Image im, char *script, va_list param_list )
{
   int i = 0, in_quote = 0, start = 0, hit_null = 0;
   IFunction *ptr = __imlib_parse_function( NULL ), *tmp_fptr = NULL;
   IFunction *rptr = ptr;
   IFunctionParam *tmp_pptr;
   pImlibExternalFilter filter;
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
	    ptr->next = __imlib_parse_function( __imlib_copystr( script, start, i-1 ) );
	    ptr = ptr->next;
	    /* righty now we need to traverse the whole of the function tree and see if we have hit
	     * any [], if we have replace it with the next var off the list update the pointer, luverly
	     * jubly */
	    for( tmp_pptr = ptr->params; tmp_pptr != NULL; tmp_pptr = tmp_pptr->next )
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
		  printf( "Setting %s::%s to a pointer.\n", ptr->name, tmp_pptr->key );
#endif
		  tmp_pptr->type = VAR_PTR;
		  tmp_pptr->data = ( tmpptr != NULL ? tmpptr : NULL );
#ifdef FDEBUG
		  printf( "%p\n", tmp_pptr->data );
#endif
	       }
	       /* for more funcky stuff in the furutre
		* if( strlen( (char *)(tmp_pptr->data) ) == 0 )
		* {
		*   free( tmp_pptr->data );
		*   tmp_pptr->data = __imlib_script_get_variable( tmp_pptr->key );
		* }
		*/
	    }
	    /* excutre the filter */
	    filter = __imlib_get_dynamic_filter( ptr->name );
	    if( filter != NULL )
	    {
	       printf( "Executing Filter %s\n", ptr->name );
	       im = filter->exec_filter( ptr->name, im, ptr->params );
	       imlib_context_set_image( im );
	    }
	    start = i+1;
	 }
      }
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
