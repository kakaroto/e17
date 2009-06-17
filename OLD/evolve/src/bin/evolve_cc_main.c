/* Evolve
 * Copyright (C) 2007-2008 Hisham Mardam-Bey 
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <locale.h>
#include <ctype.h>
#include <string.h>

#include "Evolve.h"

static void main_help(void);

Eina_List *img_dirs = NULL;
Eina_List *defines = NULL;
char      *file_in = NULL;
char      *file_out = NULL;
char      *progname = NULL;
int        verbose = 0;

static void
main_help(void)
{
   printf
     ("Usage:\n"
      "\t%s [OPTIONS] input_file.etk [output_file.eet]\n"
      "\n"
      "Where OPTIONS is one or more of:\n"
      "\n"
      "-id image/directory      Add a directory to look in for relative path images\n"
      "-v                       Verbose output\n"
      "-o                       Specify output file\n"
      "-Ddefine_val=to          CPP style define to define input macro definitions to the .edc source\n"
      ,progname);
}

int
main(int argc, char **argv)
{
   int i;
   struct stat st;
#ifdef HAVE_REALPATH   
   char rpath[PATH_MAX], rpath2[PATH_MAX];
#endif   
   Evolve *evolve;
   
   setlocale(LC_NUMERIC, "C");
   
   eina_init();

   progname = argv[0];
   for (i = 1; i < argc; i++)
     {
	if (!strcmp(argv[i], "-h"))
	  {
	     main_help();
	     exit(0);
	  }
	else if (!strcmp(argv[i], "-v"))
	  {
	     verbose = 1;
	  }
	else if ((!strcmp(argv[i], "-id") || !strcmp(argv[i], "--image_dir")) && (i < (argc - 1)))
	  {
	     i++;
	     img_dirs = eina_list_append(img_dirs, argv[i]);
	  }
      else if (!strncmp(argv[i], "-D", 2))
	  {
	     defines = eina_list_append(defines, strdup(argv[i]));
	  }	
	else if ((!strcmp(argv[i], "-o")) && (i < (argc - 1)))
	  {
	     i++;
	     file_out = argv[i];
	  }
	else if (!file_in)
	  file_in = argv[i];
	else if (!file_out)
	  file_out = argv[i];
     }
   if (!file_in)
     {
	fprintf(stderr, "%s: Error: no input file specified.\n", progname);
	main_help();
	exit(-1);
     }
   
   /* check whether file_in exists */
#ifdef HAVE_REALPATH
   if (!realpath(file_in, rpath) || stat(rpath, &st) || !S_ISREG(st.st_mode))
#else
   if (stat(file_in, &st) || !S_ISREG(st.st_mode))
#endif
     {
	fprintf(stderr, "%s: Error: file not found: %s.\n", progname, file_in);
	main_help();
	exit(-1);
     }

   if (!file_out)
      {
         char *suffix;
      
         if ((suffix = strstr(file_in,".etk")) && (suffix[4] == 0))
            {
               file_out = strdup(file_in);
               if (file_out)
                  {
                     suffix = strstr(file_out,".etk");
                     strcpy(suffix,".eet");
                  }
            }
      }
   if (!file_out)
     {
	fprintf(stderr, "%s: Error: no output file specified.\n", progname);
	main_help();
	exit(-1);
     }

#ifdef HAVE_REALPATH
   if (realpath(file_out, rpath2) && !strcmp (rpath, rpath2))
#else
   if (!strcmp (file_in, file_out))
#endif
     {
	fprintf(stderr, "%s: Error: input file equals output file.\n", progname);
	main_help();
	exit(-1);
     }

   
   evolve_init();
   evolve_defines_set(defines);
   evolve = evolve_etk_load(file_in);
   if (!evolve)
     {
	fprintf(stderr, "%s: Error parsing file: %s.\n", progname, file_in);
	exit(-1);
     }

   if (!evolve_eet_save(evolve, file_out))
     {
	fprintf(stderr, "%s: Error writing file: %s.\n", progname, file_out);
	exit(-1);
     }
   
   return 0;  
}
