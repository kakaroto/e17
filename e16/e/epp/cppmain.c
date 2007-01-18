/* CPP main program, using CPP Library.
 * Copyright (C) 1995 Free Software Foundation, Inc.
 * Written by Per Bothner, 1994-95.
 * 
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.
 * 
 * In other words, you are welcome to use, share and improve this program.
 * You are forbidden to forbid anyone else to use, share and improve
 * what you give them.   Help stamp out software-hoarding!  */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "config.h"
#include "cpplib.h"

cpp_reader          parse_in;
cpp_options         options;

int
main(int argc, char **argv)
{
   char               *p;
   int                 i;
   int                 argi = 1;	/* Next argument to handle. */
   struct cpp_options *opts = &options;

   p = argv[0] + strlen(argv[0]);
#ifndef __EMX__
   while (p != argv[0] && p[-1] != '/')
#else
   while (p != argv[0] && p[-1] != '/' && p[-1] != '\\')
#endif
      --p;
   progname = p;

   init_parse_file(&parse_in);
   parse_in.data = opts;

   init_parse_options(opts);

   argi += cpp_handle_options(&parse_in, argc - argi, argv + argi);
   if (argi < argc)
      cpp_fatal("Invalid option `%s'", argv[argi]);
   parse_in.show_column = 1;

   i = push_parse_file(&parse_in, opts->in_fname);
   if (i != SUCCESS_EXIT_CODE)
      return i;

   /* Now that we know the input file is valid, open the output.  */

   if (!opts->out_fname || !strcmp(opts->out_fname, ""))
      opts->out_fname = "stdout";
   else if (!freopen(opts->out_fname, "w", stdout))
      cpp_pfatal_with_name(&parse_in, opts->out_fname);

   for (;;)
     {
	enum cpp_token      kind;

	if (!opts->no_output)
	  {
	     fwrite(parse_in.token_buffer, 1, CPP_WRITTEN(&parse_in), stdout);
	  }
	parse_in.limit = parse_in.token_buffer;
	kind = cpp_get_token(&parse_in);
	if (kind == CPP_EOF)
	   break;
     }

   cpp_finish(&parse_in);

   if (parse_in.errors)
      exit(FATAL_EXIT_CODE);
   exit(SUCCESS_EXIT_CODE);
}
