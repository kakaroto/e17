/*****************************************************************************/
/* eConfig - the configuration library that just wouldn't die (yet)          */
/*****************************************************************************/
/* Copyright (C) 1999 - 1999 Carsten Haitzler (The Rasterman)                */
/*                       and Geoff Harrison   (Mandrake)                     */
/*                                                                           */
/* This program and utilites is free software; you can redistribute it       */
/* and/or modify it under the terms of the License shown in COPYING          */
/*                                                                           */
/* This software is distributed in the hope that it will be useful,          */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                      */
/*****************************************************************************/

#include "eConfig.internal.h"
#include "eConfig.h"

eConfigType         eConfig;

int
eConfigAddPath(char *path)
{

   /* This function will add a physical searchpath from the
    * themepath.  it returns a 1 on success, and a 2 if it is already
    * there, and a 0 if failed
    */

   PathStruct         *cur_path;

   if (!path)
      return 0;

   cur_path = eConfig.paths;
   if (cur_path)
     {
	while (cur_path)
	  {
	     if (!strcmp(cur_path->path, path))
		return 2;
	     cur_path = cur_path->next;
	  }
     }
   cur_path = eConfig.paths;
   eConfig.paths = malloc(sizeof(eConfigType));
   eConfig.paths->next = cur_path;
   eConfig.paths->path = malloc(strlen(path) + 1);
   strncpy(eConfig.paths->path, path, strlen(path) + 1);

   return 1;

}

int
eConfigRemovePath(char *path)
{

   /* This function will remove a physical searchpath from the
    * themepath.  it returns a 1 on success, and a 2 if it is not
    * there, and a 0 if failed
    */

   PathStruct         *cur_path;
   PathStruct         *last_path;

   if (!path)
      return 0;

   cur_path = eConfig.paths;
   if (cur_path)
     {
	if (!strcmp(cur_path->path, path))
	  {
	     free(cur_path->path);
	     eConfig.paths = cur_path->next;
	     free(cur_path);
	     return 1;
	  }
	while (cur_path)
	  {
	     last_path = cur_path;
	     cur_path = cur_path->next;
	     if (cur_path)
	       {
		  if (!strcmp(cur_path->path, path))
		    {
		       free(cur_path->path);
		       last_path->next = cur_path->next;
		       free(cur_path);
		       return 1;
		    }
		  if (cur_path)
		     cur_path = cur_path->next;
	       }
	  }
     }
   else
     {
	return 2;
     }

   return 2;
}

char              **
eConfigPaths(int *num)
{

   /* This function returns a (char **) containing all of the current
    * themepaths.  it tells you how many there are in *num
    * See testapp source code for a good example.
    */

   PathStruct         *cur_path;
   char              **paths;
   int                 i;

   cur_path = eConfig.paths;
   paths = malloc(sizeof(char *));

   if (cur_path)
     {
	i = 0;
	while (cur_path)
	  {
	     paths = realloc(paths, (sizeof(char *)) * ++i);

	     paths[i - 1] = cur_path->path;
	     cur_path = cur_path->next;
	  }
     }
   else
     {
	return NULL;
     }

   *num = i;
   return paths;

}
