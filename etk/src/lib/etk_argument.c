/* ETK - The Enlightened ToolKit
 * Copyright (C) 2006-2008 Simon Treny, Hisham Mardam-Bey, Vincent Torri, Viktor Kojouharov
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library. 
 * If not, see <http://www.gnu.org/licenses/>.
 */

/** @file etk_argument.c */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "etk_argument.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @addtogroup Etk_Argument
 * @{
 */

#define MAX_ARGS 256

static int   _argc = 0;
static char *_argv[MAX_ARGS + 1];

/**************************
 *
 * Implementation
 *
 **************************/

/**
 * @internal
 * @brief Initializes the argument parser of Etk
 * @param argc the number of arguments in @a argv
 * @param argv a list of arguments given to the program
 * @param custom_opts a string corresponding to the custom arguments to add to argv.
 * For example, "--option1 value --toggle1". It can be set to NULL
 */
void etk_argument_init(int argc, char **argv, const char *custom_opts)
{
   int i;
   
   /* Copy the argv array to _argv */
   _argc = 0;
   if (argv)
   {
      for (i = 0; i < argc && _argc < MAX_ARGS; i++, _argc++)
      {
         if (!argv[i])
            break;
         _argv[_argc] = strdup(argv[i]);
      }
   }
   
   /* Parse the custom arguments and add them to _argv */
   if (custom_opts)
   {
      const char *start, *end;
      int len, arg_len;
      
      start = NULL;
      end = NULL;
      len = strlen(custom_opts);
      for (i = 0; i < len && _argc < MAX_ARGS; i++)
      {
         if (!start)
         {
            if (custom_opts[i] != ' ')
               start = &custom_opts[i];
         }
         if (start && ((i + 1 == len) || (custom_opts[i + 1] == ' ')))
            end = &custom_opts[i];
         
         if (start && end)
         {
            arg_len = end - start + 1;
            _argv[_argc] = malloc(arg_len + 1);
            strncpy(_argv[_argc], start, arg_len);
            _argv[_argc][arg_len] = '\0';
            _argc++;
            
            start = NULL;
            end = NULL;
         }
      }
   }
   
   _argv[_argc] = NULL;
}

/**
 * @internal
 * @brief Shutdowns the argument parser of Etk
 */
void etk_argument_shutdown(void)
{
   int i;
   
   for (i = 0; i < _argc; i++)
      free(_argv[i]);
   _argc = 0;
}

/**
 * @brief Retrieves the arguments passed to etk_init_full()
 * @param argc the location where to store the number of arguments passed to etk_init_full()
 * @param argv the location where to store the list of arguments passed to etk_init_full()
 */
void etk_argument_get(int *argc, char ***argv)
{
   if (argc)
      *argc = _argc;
   if (argv)
      *argv = _argv;
}

/**
 * @brief Checks whether the argument has been passed to the program
 * @param long_name the complete name of the argument to find. If '--long_name' is found in @a argv, this function will
 * return ETK_TRUE. You can set this param to NULL to ignore it
 * @param short_name a shortcut for the argument to find. If '-short_name' is found in @a argv, this function will
 * return ETK_TRUE. You can set this param to 0 to ignore it
 * @param remove if @a remove is set to ETK_TRUE, the argument will be removed from the list of arguments passed to
 * the program if the argument has been found
 * @return Returns ETK_TRUE if the argument has been found, ETK_FALSE otherwise
 */
Etk_Bool etk_argument_is_set(const char *long_name, char short_name, Etk_Bool remove)
{
   Etk_Bool is_set = ETK_FALSE;
   char *arg;
   int arg_len;
   int i, j;

   for (i = 0; i < _argc; i++)
   {
      arg = _argv[i];
      arg_len = strlen(arg);
      if (arg_len < 2 || arg[0] != '-')
         continue;
      
      if ((arg_len == 2) && (arg[1] == short_name))
         is_set = ETK_TRUE;
      else if ((arg_len > 2) && (arg[1] == '-'))
      {
         if (long_name && (strcmp(&arg[2], long_name) == 0))
            is_set = ETK_TRUE;
      }

      if (is_set)
      {
         if (remove)
         {
            for (j = i + 1; j <= _argc; j++)
               _argv[j - 1] = _argv[j];
            _argc--;
         }
         return ETK_TRUE;
      }
   }

   return ETK_FALSE;
}

/**
 * @brief Gets the value of an argument passed to the program
 * @param long_name the complete name of the argument to find. If --long_name is found in @a argv and is followed by a
 * value, this function will return ETK_TRUE. You can set this param to NULL to ignore it
 * @param short_name a shortcut for the argument to find. If -short_name is found in @a argv and is followed by a
 * value, this function will return ETK_TRUE. You can set this param to 0 to ignore it
 * @param remove if @a remove is ETK_TRUE, the argument and its value will be removed from the list of arguments
 * passed to the program if they have been found
 * @param value the location where to store the value of the argument. You'll have to free it when you no
 * longer need it. This parameter should not be NULL, otherwise the function will return ETK_FALSE
 * @return Returns ETK_TRUE if the argument has been found and was followed by a value, ETK_FALSE otherwise
 */
Etk_Bool etk_argument_value_get(const char *long_name, char short_name, Etk_Bool remove, char **value)
{
   int num_args = 0;
   char *arg, *next, *value_ptr = NULL;
   int arg_len, long_name_len;
   int i, j;

   if (!value)
      return ETK_FALSE;

   long_name_len = long_name ? strlen(long_name) : 0;

   for (i = 0; i < _argc; i++)
   {
      arg = _argv[i];
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
               if ((i + 1 < _argc) && (next = _argv[i + 1]) && next[0] != '-')
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
               if ((i + 1 < _argc) && (next = _argv[i + 1]) && next[0] != '-')
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

      if (value_ptr)
      {
         /* A value has been found */
         *value = strdup(value_ptr);
         if (remove)
         {
            for (j = i; j <= _argc - num_args; j++)
               _argv[j] = _argv[j + num_args];
            _argc -= num_args;
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
 *
 * To check whether an argument that takes no value is present, use etk_argument_is_set(). @n
 * To get the value of an argument, use etk_argument_value_get()
 */
