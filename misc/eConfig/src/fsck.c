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
eConfigFsckPath(char *path)
{

   /* This function is supposed to fsck a theme at the specified location
    * *path.  It is exported by the command.  returns a 0 on failure and
    * a 1 on success.
    */

   char                tablepath[FILEPATH_LEN_MAX];

   if (!path)
      return 0;

   sprintf(tablepath, "%s/fat", path);
   if ((FAT_TABLE = fopen(tablepath, "r")))
     {
	while (!feof(FAT_TABLE))
	  {
	  }
	fclose(FAT_TABLE);
     }

   return 1;

}
