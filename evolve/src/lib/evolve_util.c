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

#include <string.h>
#include <ctype.h>

#include "evolve_private.h"

char *evolve_util_string_humanize(char *str)
{
   int i;
   char *ret;
   if (!str)
     return NULL;
   
   ret = strdup(str);
   for (i = 0; i < strlen(str); ++i)
     {
	if (ret[i] == '-' || ret[i] == '_')
	  ret[i] = ' ';
	
	if (i == 0 || (i > 0 && isspace(ret[i - 1])))
	  ret[i] = toupper(ret[i]);
     }
   return ret;
}
