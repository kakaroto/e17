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
    * a 1 on success.  This function will remove all the "dirty" tagged
    * entries in a fat table, and then it will create a new data table based
    * off of all the non-dirty data.
    */

   /*
    * TODO: we need to catch all sorts of potential errors in here that we
    * are not currently catching
    */

   char                tablepath[FILEPATH_LEN_MAX];
   char                datapath[FILEPATH_LEN_MAX];
   char                tablepath2[FILEPATH_LEN_MAX];
   char                datapath2[FILEPATH_LEN_MAX];
   FILE               *FAT_TABLE;
   FILE               *NEW_FAT_TABLE;
   FILE               *CONF_TABLE;
   FILE               *NEW_CONF_TABLE;
   eConfigFAT          tableentry;

   if (!path)
      return 0;

   sprintf(tablepath, "%s/fat", path);
   sprintf(datapath, "%s/data", path);
   sprintf(tablepath2, "%s/newfat", path);
   sprintf(datapath2, "%s/newdata", path);
   _econf_lock_path(path);
   if ((FAT_TABLE = fopen(tablepath, "r")))
     {
	if ((NEW_FAT_TABLE = fopen(tablepath2, "w")))
	  {
	     if ((CONF_TABLE = fopen(datapath2, "r")))
	       {
		  if ((NEW_CONF_TABLE = fopen(datapath2, "w")))
		    {
		       char               *last_pointer = NULL;

		       while (!feof(FAT_TABLE))
			 {

			    fread(&tableentry, sizeof(eConfigFAT), 1, FAT_TABLE);
			    if (strcmp(tableentry.loc, "dirty"))
			      {
				 if ((last_pointer && strcmp(tableentry.loc,
							     last_pointer))
				     || !last_pointer)
				   {
				      char               *allocedspace = NULL;

				      if (last_pointer)
					 free(last_pointer);

				      allocedspace = malloc(tableentry.length + 1);
				      fseek(CONF_TABLE, tableentry.position, SEEK_SET);
				      fread(allocedspace, tableentry.length, 1,
					    CONF_TABLE);
				      fseek(NEW_CONF_TABLE, 0, SEEK_END);
				      fwrite(allocedspace, tableentry.length, 1,
					     NEW_CONF_TABLE);

				      if (allocedspace)
					 free(allocedspace);

				      last_pointer = malloc(strlen(tableentry.loc) + 1);
				      strcpy(last_pointer, tableentry.loc);
				      tableentry.position = ftell(CONF_TABLE);
				      fwrite(&tableentry, sizeof(eConfigFAT), 1,
					     NEW_FAT_TABLE);
				   }
			      }
			 }
		       if (last_pointer)
			  free(last_pointer);
		       fclose(FAT_TABLE);
		       fclose(NEW_FAT_TABLE);
		       unlink(tablepath);
		       unlink(datapath);
		       rename(tablepath2, tablepath);
		       rename(datapath2, datapath);
		    }
	       }
	  }
     }
   _econf_unlock_path(path);
   return 1;

}
