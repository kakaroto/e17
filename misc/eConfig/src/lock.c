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

int
_econf_lock_path(char *path)
{

   /* This function is supposed to lock a theme at the specified location
    * *path.  It is not exported by the library.  returns a 0 on failure and
    * a 1 on success.
    * This function will wait until the lock is attainable, and then lock
    * the path, and check to make sure he was the one who locked it.  Once
    * this lock is obtained, the function returns.  until then it sleeps.
    */

   char                lockpath[FILEPATH_LEN_MAX];
   FILE               *LOCKFILE;

   if (!path)
      return 0;

   sprintf(lockpath, "%s/lock", path);
   if ((LOCKFILE = fopen(lockpath, "r")))
     {
	while (!feof(LOCKFILE))
	  {
	  }
	fclose(LOCKFILE);
     }
   return 1;

}
