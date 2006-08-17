/** @file etk_argument.c */
#include "etk_argument.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @addtogroup Etk_Argument
 * @{
 */

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @brief Checks whether the argument has been passed to the program
 * @param argc the location of the "argc" parameter passed to main()
 * @param argv the location of the "argv" parameter passed to main()
 * @param long_name the complete name of the argument to find. If '--long_name' is found in @a argv, this function will
 * return ETK_TRUE. You can set it to NULL to ignore it
 * @param short_name a shortcut for the argument to find. If '-short_name' is found in @a argv, this function will
 * return ETK_TRUE. You can set it to NULL to ignore it
 * @param remove if @a remove is ETK_TRUE, the argument will be removed from @a argv if it has been found
 * @return Returns ETK_TRUE if the argument has been found, ETK_FALSE otherwise
 */
Etk_Bool etk_argument_is_set(int *argc, char ***argv, const char *long_name, char short_name, Etk_Bool remove)
{
   Etk_Bool is_set = ETK_FALSE;
   char *arg;
   int arg_len;
   int i, j;
   
   if (!argc || !argv)
      return ETK_FALSE;
   
   for (i = 0; i < *argc; i++)
   {
      if (!(arg = ((*argv)[i])))
         continue;
      
      arg_len = strlen(arg);
      if ((arg_len == 2) && (arg[0] == '-') && (arg[1] == short_name))
         is_set = ETK_TRUE;
      else if ((arg_len > 2) && (arg[0] == '-') && (arg[1] == '-'))
      {
         if (long_name && (strcmp(&arg[2], long_name) == 0))
            is_set = ETK_TRUE;
      }
      
      if (is_set)
      {
         if (remove)
         {
            for (j = i + 1; j < *argc; j++)
               (*argv)[j - 1] = (*argv)[j];
            *argc--;
         }
         return ETK_TRUE;
      }
   }
   
   return ETK_FALSE;
}

/**
 * @brief Gets the value of an argument passed to the program
 * @param argc the location of the "argc" parameter passed to main()
 * @param argv the location of the "argv" parameter passed to main()
 * @param long_name the complete name of the argument to find. If --long_name is found in @a argv and is followed by a
 * value, this function will return ETK_TRUE. You can set it to NULL to ignore it
 * @param short_name a shortcut for the argument to find. If -short_name is found in @a argv and is followed by a
 * value, this function will return ETK_TRUE. You can set it to NULL to ignore it
 * @param remove if @a remove is ETK_TRUE, the argument and its value will be removed from @a argv
 * if they have been found
 * @param value the location where to store the value of the argument. You'll have to free it when you no longer need it
 * @return Returns ETK_TRUE if the argument has been found and was followed by a value, ETK_FALSE otherwise
 */
Etk_Bool etk_argument_value_get(int *argc, char ***argv, const char *long_name, char short_name, Etk_Bool remove, char **value)
{
   int num_args = 0;
   char *arg, *next, *value_ptr = NULL;
   int arg_len, long_name_len = 0;
   int i, j;
   
   if (!argc || !argv)
      return ETK_FALSE;
   
   if (long_name)
      long_name_len = strlen(long_name);
   
   for (i = 0; i < *argc; i++)
   {
      if (!(arg = (*argv)[i]))
         continue;
      
      arg_len = strlen(arg);
      if (arg_len < 2 || arg[0] != '-')
         continue;
      
      /* Short argument */
      if (arg[1] != '-')
      {
         if (arg[1] == short_name)
         {
            /* -s value */
            if (arg_len == 2)
            {
               if ((i + 1 < *argc) && (next = (*argv)[i + 1]) && next[0] != '-')
               {
                  value_ptr = next;
                  num_args = 2;
               }
            }
            /* -svalue */
            else
            {
               value_ptr = &arg[2];
               num_args = 1;
            }
         }
      }
      /* Long argument */
      else if (long_name_len > 0)
      {
         if (strncmp(&arg[2], long_name, long_name_len) == 0)
         {
            /* --long_name value */
            if (arg_len == long_name_len + 2)
            {
               if ((i + 1 < *argc) && (next = (*argv)[i + 1]) && next[0] != '-')
               {
                  value_ptr = next;
                  num_args = 2;
               }
            }
            /* --long_name=value */
            else if ((arg_len > long_name_len + 3) && (arg[long_name_len + 2] == '='))
            {
               value_ptr = &arg[long_name_len + 3];
               num_args = 1;
            }
         }
      }
      
      /* A value has been found */
      if (value_ptr)
      {
         if (value)
            *value = strdup(value_ptr);
         if (remove)
         {
            for (j = i + num_args; j < *argc; j++)
               (*argv)[j - num_args] = (*argv)[j];
            *argc--;
         }
         return ETK_TRUE;
      }
   }
   
   return ETK_FALSE;
}

/** @} */

/**************************
 *
 * Documentation
 *
 **************************/

/**
 * @addtogroup Etk_Argument
 *
 * You can use those two functions to parse the arguments given to your program. @n
 * Two kinds of arguments are supported:
 * - <b>Short arguments</b>: <i>"-c"</i>, <i>"-c value"</i> or <i>"-cvalue"</i>
 * - <b>Long arguments</b>: <i>"--argument"</i>, <i>"--agument value"</i> or <i>"--argument=value"</i>
 */
