/** @file etk_argument.c */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "etk_utils.h"
#include "etk_argument.h"

#define ETK_ARGUMENT_FLAG_PRIV_SET (1 << 4)

/**
 * @brief Parses the arguments as described by the user
 * @param args the arguments you are interested in
 * @param argc the number of arguments given to your program
 * @param argv the values of the arguments given to your program
 * @return Returns an int which tells the caller about the status
 *
 * Example:
 * @code
 * static void info_func(Etk_Argument *args, int index)
 * {
 *   printf("info func!\n");
 * }
 *
 * static void text_func(Etk_Argument *args, int index)
 * {
 *    printf("text func! %s\n", args[index].data);
 * }
 *
 * static void hide_func(Etk_Argument *args, int index)
 * {
 *   printf("hide func!\n");
 * }
 *
 * Etk_Argument args[] = {
 *      { "info", 'i', NULL, info_func, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, " " },
 *      { "text", 't', NULL, text_func, NULL, ETK_ARGUMENT_FLAG_OPTIONAL|ETK_ARGUMENT_FLAG_VALUE_REQUIRED, " " },
 *      { "hide", 'h', NULL, hide_func, NULL, ETK_ARGUMENT_FLAG_OPTIONAL, " " },
 *      { NULL,   -1,  NULL, NULL,      NULL, ETK_ARGUMENT_FLAG_NONE,     " " }
 * };
 *
 * int main(int argc, char **argv)
 * {
 *    etk_arguments_parse(args, argc, argv);
 *
 *    return 0;
 * }
 * @endcode
 */
int etk_arguments_parse(Etk_Argument *args, int argc, char **argv)
{
   int i;
   Etk_Argument *arg;
      
   /* no arguments */
   if(argc < 2)
   {
      /* check for required arguments */
      i = 0;
      arg = args;	     
      while(arg->short_name != -1)
      {
	 if(arg->flags & ETK_ARGUMENT_FLAG_REQUIRED)
	 {
	    printf(_("Argument %d '-%c | --%s' is required\n"), i, arg->short_name, arg->long_name);
	    return ETK_ARGUMENT_RETURN_REQUIRED_NOT_FOUND;
	 }
	 ++i; ++arg;
      }
      return ETK_ARGUMENT_RETURN_OK;
   }
   
   for(i = 1; i < argc; i++)
   {
      char *cur;
      
      cur = argv[i];
      if(!cur) continue;
      
      /* min length is 2, anything less is invalid */
      if(strlen(cur) < 2)
      {
	 printf(_("Argument %d '%s' is too short\n"), i, argv[i]);
	 return ETK_ARGUMENT_RETURN_MALFORMED;
      }
      
      /* short (single char) argument of the form -d val or -dval */
      if(cur[0] == '-' && cur[1] != '-')
      {
	 arg = args;
	 
	 while(arg->short_name != -1)
	 {
	    /* match found for short arg */
	    if(arg->short_name == cur[1])
	    {
	       /* check to see if arg needs value */
	       if((arg->flags & ETK_ARGUMENT_FLAG_VALUE_REQUIRED) &&
		  i + 1 < argc)
	       {
		  char *val = argv[i + 1];
		  
		  /* if no value is present, report error */
		  if(val[0] == '-')
		  {
		     printf(_("Argument %d '%s' requires a value\n"), i, cur);
		     return ETK_ARGUMENT_RETURN_REQUIRED_VALUE_NOT_FOUND;
		  }
		  
		  if(arg->data)			      
		    *(arg->data) = *val;
		  else
		    arg->data = val;
		  arg->flags |= ETK_ARGUMENT_FLAG_PRIV_SET;
		  ++i;
	       }
	       else if (arg->flags & ETK_ARGUMENT_FLAG_VALUE_REQUIRED
			&& i + 1 >= argc)
	       {
		  /* if no value is present, report error */
		  printf(_("Argument %d '%s' requires a value\n"), i, cur);
		  return ETK_ARGUMENT_RETURN_REQUIRED_VALUE_NOT_FOUND;
	       }
	       else if(!(arg->flags & ETK_ARGUMENT_FLAG_VALUE_REQUIRED))
		 arg->flags |= ETK_ARGUMENT_FLAG_PRIV_SET;
	    }
	    ++arg;
	 }
      }
      /* long argument of the form --debug or --debug=something */
      else if(cur[0] == '-' && cur[1] == '-' && strlen(cur) > 2)
      {
	 arg = args;
	 
	 while(arg->short_name != -1)
	 {
	    char *tmp = NULL;
	    char *tmp2;
	    
	    if(!arg->long_name)
	      continue;
	    
	    /* check if arg if of the form --foo=bar */
	    tmp = strchr(cur, '=');
	    if(tmp)
	    {
	       tmp2 = cur;
	       cur = calloc(tmp - tmp2 + 1, sizeof(char));
	       snprintf(cur, (tmp - tmp2 + 1) * sizeof(char), "%s", tmp2);		     		       
	    }
	    else		    
	      tmp = NULL;
	    
	    /* match found for long arg */
	    if(!strcmp(arg->long_name, cur + 2))
	    {		       
	       /* check to see if arg needs value */
	       if((arg->flags & ETK_ARGUMENT_FLAG_VALUE_REQUIRED) &&
		  ((i + 1 < argc) || (tmp != NULL)))
	       {
		  char *val;
		  
		  if(!tmp)
		    val = argv[i + 1];
		  else
		    val = tmp + 1;			    
		  
		  /* if no value is present, report error */
		  if(val[0] == '-')
		  {
		     printf(_("Argument %d '%s' requires a value\n"), i, cur);
		     return ETK_ARGUMENT_RETURN_REQUIRED_VALUE_NOT_FOUND;
		  }
		  
		  if(arg->data)			      
		    *(arg->data) = *val;
		  else
		    arg->data = val;
		  arg->flags |= ETK_ARGUMENT_FLAG_PRIV_SET;
		  
		  if(!tmp)
		    ++i;
	       }
	       else if (arg->flags & ETK_ARGUMENT_FLAG_VALUE_REQUIRED
			&& i + 1 >= argc)
	       {
		  /* if no value is present, report error */
		  printf(_("Argument %d '%s' requires a value\n"), i, cur);
		  return ETK_ARGUMENT_RETURN_REQUIRED_VALUE_NOT_FOUND;
	       }
	       else if(!(arg->flags & ETK_ARGUMENT_FLAG_VALUE_REQUIRED))
		 arg->flags |= ETK_ARGUMENT_FLAG_PRIV_SET;
	    }
	    
	    if(tmp)
	    {
	       free(cur);
	       cur = argv[i];
	    }
	    
	    ++arg;		  
	 }
      }	
   }
   
   /* check for required arguments */
   i = 0;
   arg = args;	     
   while(arg->short_name != -1)
   {
      if(!(arg->flags & ETK_ARGUMENT_FLAG_PRIV_SET) &&
	 arg->flags & ETK_ARGUMENT_FLAG_REQUIRED)
      {
	 printf(_("Argument %d '-%c | --%s' is required\n"), i, arg->short_name, arg->long_name);
	 return ETK_ARGUMENT_RETURN_REQUIRED_NOT_FOUND;
      }
      ++i; ++arg;
   }
   
   /* call all the callbacks */
   i = 0;
   arg = args;	     
   while(arg->short_name != -1)
   {
      if(arg->func && arg->flags & ETK_ARGUMENT_FLAG_PRIV_SET)
	arg->func(args, i);
      ++i; ++arg;
   }
   
   return ETK_ARGUMENT_RETURN_OK;
}

/** @} */
