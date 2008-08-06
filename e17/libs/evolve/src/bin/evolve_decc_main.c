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

#include "Evolve.h"

char *progname;

void main_help()
{
   printf
     ("Usage:\n"
      "\t%s input_file.eet\n"
      "\n",
      progname);
}      

int main(int argc, char **argv)
{
   char *file_in;
   char *code;
   struct stat st;
   Evolve *evolve;
   
   progname = argv[0];
   
   if (argc < 2)
     {
	main_help();
	exit(-1);
     }
   
   file_in = argv[1];
      
   if (stat(file_in, &st) || !S_ISREG(st.st_mode))
     {
	fprintf(stderr, "%s: Error: file not found: %s.\n", progname, file_in);
	main_help();
	exit(-1);
     }
	
   evolve_init();
   evolve = evolve_eet_load(file_in);
   if (!evolve)
     {
	fprintf(stderr, "%s: Error parsing file: %s.\n", progname, file_in);
	exit(-1);
     }
   
   code = evolve_code_get(evolve);
   printf("%s\n", code);
   
   return 0;
}
