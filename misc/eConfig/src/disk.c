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

unsigned long
_econf_finddatapointerinpath(char *path, char *loc,
			     unsigned long *position,
			     unsigned long *timestamp)
{

   /* This function is internal to eConfig
    * its goal is to see if it can find the data specified (loc)
    * inside of the theme * data at the path (path).
    * if found, it sets and returns the position in the theme along with the
    * length of the data in *length
    * This function is internal to eConfig.
    */

   FILE               *FAT_TABLE;
   char                tablepath[FILEPATH_LEN_MAX];
   eConfigFAT          tableentry;

   if (!path)
      return 0;

   sprintf(tablepath, "%s/fat", path);
   if ((FAT_TABLE = fopen(tablepath, "r")))
     {
	while (!feof(FAT_TABLE))
	  {
	     fread(&tableentry, sizeof(eConfigFAT), 1, FAT_TABLE);
	     if (!strcmp(tableentry.loc, loc))
	       {
		  fclose(FAT_TABLE);
		  *position = tableentry.position;
		  *timestamp = ntohl(tableentry.updated_on);
		  return tableentry.length;
	       }
	  }
	fclose(FAT_TABLE);
     }
   else
     {
	/* we couldn't open the FAT table, return an error */
	return 0;
     }

   /* returning a length of zero implies no data to be found */
   return 0;

}

char              **
_econf_snarf_keys_from_fat_table(char *path, char *regex,
				 unsigned long *length)
{

   FILE               *FAT_TABLE;
   char                tablepath[FILEPATH_LEN_MAX];
   eConfigFAT          tableentry;
   char              **return_table = NULL;

   *length = 0;
   if (!path)
      return NULL;

   memset(&tableentry, 0, sizeof(eConfigFAT));

   sprintf(tablepath, "%s/fat", path);
   FAT_TABLE = fopen(tablepath, "r");
   if (FAT_TABLE)
     {
	while (!feof(FAT_TABLE))
	  {
	     char               *current_pointer;

	     fread(&tableentry, sizeof(eConfigFAT), 1, FAT_TABLE);
	     if (strcmp(tableentry.loc, "dirty"))
	       {
		  if (_econf_matchregexp(regex, tableentry.loc))
		    {
		       if ((return_table && strcmp(tableentry.loc,
				 return_table[*length - 1])) || !return_table)
			 {
			    current_pointer = malloc(strlen(tableentry.loc) + 1);
			    strcpy(current_pointer, tableentry.loc);
			    (*length)++;
			    if (return_table)
			      {
				 return_table = realloc(return_table,
					      (sizeof(char *) * *length) + 1);
			      }
			    else
			      {
				 return_table = malloc((sizeof(char *)) + 1);
			      }
			    return_table[*length - 1] = current_pointer;
			 }
		    }
	       }
	  }
	return return_table;
     }
   return NULL;
}

void               *
_econf_get_data_from_disk(char *loc, unsigned long *length)
{

   /* This function is internal to eConfig
    * it searches all the themepaths to find the correct data.
    * it will return it, as well as return the length in *length
    * This function is internal to eConfig.
    */

   char              **paths;
   int                 num;

   if ((paths = eConfigPaths(&num)))
     {
	int                 i;
	unsigned long       position;
	unsigned long       timestamp;
	unsigned long       winningtimestamp;
	char               *allocedspace;

	winningtimestamp = 0;
	allocedspace = NULL;
	for (i = 0; i < num; i++)
	  {
	     if ((*length =
		  _econf_finddatapointerinpath(paths[i], loc, &position,
					       &timestamp)))
	       {
		  if (timestamp >= winningtimestamp)
		    {
		       FILE               *CONF_TABLE;
		       char                confpath[FILEPATH_LEN_MAX];

		       if (allocedspace)
			  free(allocedspace);

		       allocedspace = malloc(*length + 1);
		       sprintf(confpath, "%s/data", paths[i]);
		       CONF_TABLE = fopen(confpath, "r");

		       fseek(CONF_TABLE, position, SEEK_SET);
		       fread(allocedspace, *length, 1, CONF_TABLE);
		       fclose(CONF_TABLE);
		       winningtimestamp = timestamp;
		    }
	       }
	  }
	free(paths);
	if (allocedspace)
	   return allocedspace;
     }
   /* obviously we didn't find it anywhere in here */

   *length = 0;
   return NULL;

}

unsigned long
_econf_append_data_to_disk_at_path(char *path,
				   unsigned long length, void *data)
{
   /* This function is pretty simplistic.  it just saves out a bit of *data to
    * the theme at *path, into the data file at the end, and knows it is
    * length long.  It returns a 0 on failure, and a the position of the
    * data on success.
    * This function is internal to eConfig
    */

   FILE               *CONF_TABLE;
   char                confpath[FILEPATH_LEN_MAX];

   if (!path)
      return 0;
   if (!length)
      return 0;
   if (!data)
      return 0;

   sprintf(confpath, "%s/data", path);
   CONF_TABLE = fopen(confpath, "r+");
   if (CONF_TABLE)
     {
	unsigned long       position;

	fseek(CONF_TABLE, 0, SEEK_END);
	position = ftell(CONF_TABLE);

	if (!fwrite(data, length, 1, CONF_TABLE))
	  {
	     /* oh shit, we didn't write enough data.  maybe we need
	      * to somehow mark all these errors into something useful
	      */
	     fclose(CONF_TABLE);
	     return 0;
	  }
	fclose(CONF_TABLE);
	/* position is position + 1 - hack hack */
	return position + 1;
     }
   else
     {
	/* we failed to open the file for writing.  return an error */
	return 0;
     }

   return 0;
}

int
_econf_save_data_to_disk_at_position(unsigned long position, char *path,
				     unsigned long length, void *data)
{

   /* This function is pretty simplistic.  it just saves out a bit of *data to
    * the theme at *path, into the data file at position, and knows it is
    * length long.  It returns a 0 on failure, and a 1 on success.
    * This function is internal to eConfig
    */

   FILE               *CONF_TABLE;
   char                confpath[FILEPATH_LEN_MAX];

   if (!position)
      return 0;
   if (!path)
      return 0;
   if (!length)
      return 0;
   if (!data)
      return 0;

   sprintf(confpath, "%s/data", path);
   CONF_TABLE = fopen(confpath, "r+");
   if (CONF_TABLE)
     {
	fseek(CONF_TABLE, position, SEEK_SET);
	if (fwrite(data, length, 1, CONF_TABLE) < length)
	  {
	     /* oh shit, we didn't write enough data.  maybe we need
	      * to somehow mark all these errors into something useful
	      */
	     fclose(CONF_TABLE);
	     return 0;
	  }
	fclose(CONF_TABLE);
     }
   else
     {
	/* we failed to open the file for writing.  return an error */
	return 0;
     }

   return 1;

}

int
_econf_new_fat_entry_to_disk(char *loc, unsigned long position,
			     unsigned long length, char *path)
{

   /* This function creates a new FAT table entry at the specified location
    * *path for the variable *loc of length length. 
    * returns a 0 on failure, 1 on success
    * This function is for internal use by eConfig only
    */

   FILE               *FAT_TABLE;
   char                tablepath[FILEPATH_LEN_MAX];
   eConfigFAT          tableentry;

   if (!path)
      return 0;
   if (!loc)
      return 0;
   if (!length)
      return 0;

   sprintf(tablepath, "%s/fat", path);
   FAT_TABLE = fopen(tablepath, "r+");
   if (FAT_TABLE)
     {
	memset(&tableentry, 0, sizeof(eConfigFAT));
	sprintf(tableentry.loc, "%s", loc);
	tableentry.length = length;
	tableentry.usage_index = 0;
	tableentry.position = position;
	tableentry.updated_on = _econf_timestamp();
	fseek(FAT_TABLE, 0, SEEK_END);
	fwrite(&tableentry, sizeof(eConfigFAT), 1, FAT_TABLE);
	fclose(FAT_TABLE);
	/* success, for now */
	return 1;
     }
   else
     {
	/* we failed to open the file for writing properly.
	 * This means we can't write anything to the disk.
	 * This is an error. :)
	 */
	return 0;
     }

   return 0;

}

int
_econf_replace_fat_entry_to_disk(char *loc, unsigned long position,
				 unsigned long length, char *path)
{

   /* This function replaces the current FAT table entry at the specified
    * location *path for the variable *loc of length length. 
    * This function is for internal use by eConfig only
    * returns a 0 on failure, 1 on success
    */

   FILE               *FAT_TABLE;
   char                tablepath[FILEPATH_LEN_MAX];
   eConfigFAT          tableentry;
   eConfigFAT          oldtableentry;

   if (!path)
      return 0;
   if (!loc)
      return 0;
   if (!length)
      return 0;

   sprintf(tablepath, "%s/fat", path);
   FAT_TABLE = fopen(tablepath, "r+");
   if (FAT_TABLE)
     {
	memset(&tableentry, 0, sizeof(eConfigFAT));
	sprintf(tableentry.loc, "%s", loc);
	tableentry.length = length;
	tableentry.usage_index = 0;
	tableentry.position = position;
	tableentry.updated_on = _econf_timestamp();
	while (!feof(FAT_TABLE))
	  {
	     fread(&oldtableentry, sizeof(eConfigFAT), 1, FAT_TABLE);
	     if (!strcmp(oldtableentry.loc, loc))
	       {
		  fseek(FAT_TABLE, -(sizeof(eConfigFAT)), SEEK_CUR);
		  if (length >
		      fwrite(&tableentry, sizeof(eConfigFAT), 1, FAT_TABLE))
		    {
		       /* oops, we messed up somehow during the write. */
		       fclose(FAT_TABLE);
		       return 0;
		    }
		  fclose(FAT_TABLE);
		  return 1;
	       }
	  }
     }
   else
     {
	/* we failed to open the file for writing properly.
	 * This means we can't write anything to the disk.
	 * This is an error. :)
	 */
	return 0;
     }

   return 0;
}

int
_econf_save_data_to_disk(void *data, char *loc, unsigned long length,
			 char *path)
{

   /* This function is supposed to save data out to disk.  it takes the *data
    * for key *loc of length length and saves it to the theme at *path.
    * *path MUST be supplied externally by the application.
    * returns a positive number as success, anything else is an error
    * This function is internal to eConfig.
    */

   unsigned long       position;
   unsigned long       oldlength;
   unsigned long       timestamp;

   if (!data)
      return 0;
   if (!loc)
      return 0;
   if (!length)
      return 0;
   if (!path)
      return 0;

   if ((oldlength = _econf_finddatapointerinpath(path, loc, &position,
						 &timestamp)))
     {
	/* we already exist in this datafile */
	if (oldlength >= length)
	  {
	     if (!_econf_save_data_to_disk_at_position(position, path,
						       length, data))
	       {
		  /* we failed writing to the disk at this point.  uh oh.
		   * this is bad.  return an error
		   */
		  return -1;
	       }
	     else
	       {
		  if (!_econf_replace_fat_entry_to_disk(loc, position, length,
							path))
		    {
		       /* we failed writing to the disk at this point.  uh oh.
		        * this is bad.  return an error
		        */
		       return -2;
		    }
		  return 1;
	       }
	  }
	else
	  {
	     if (!(position = _econf_append_data_to_disk_at_path(path, length, data)))
	       {
		  /* we failed writing to the disk at this point.  uh oh.
		   * this is bad.  return an error
		   */
		  return -1;
	       }
	     else
	       {
		  if (!_econf_replace_fat_entry_to_disk(loc, position, length, path))
		    {
		       /* we failed writing to the disk at this point.  uh oh.
		        * this is bad.  return an error
		        */
		       return -2;
		    }
		  return 1;
	       }
	  }
     }
   else
     {
	/* we don't exist in this datafile */
	if (!_econf_create_new_data_repository(path))
	  {
	     /* we can't write to that data path - we can't make it */
	     return 0;
	  }
	else
	  {
	     unsigned long       position;

	     /* we need to add ourselves now */
	     if (!(position = _econf_append_data_to_disk_at_path(path, length, data)))
	       {
		  /* we failed adding the data entry to disk, return an error */
		  return -3;
	       }
	     if (!_econf_new_fat_entry_to_disk(loc, position - 1, length, path))
	       {
		  /* we failed adding the fat entry to disk, return an error */
		  return -3;
	       }
	     return 1;
	  }
     }

   return 0;

}

int
_econf_purge_data_from_disk_at_path(char *loc, char *path)
{

   /* This function is supposed to "dirty" the data inside of a disk at any
    * particular location.  typically used because your data is:
    * a) no longer viable
    * b) no longer usable
    * c) too short for the new data you have to save into this file
    * if your dataspace is marked as dirty it will be fsck()d out at the
    * next instance of eConfigFsckPath() on *path.
    * This function is internal to eConfig.
    */

   FILE               *FAT_TABLE;
   char                tablepath[FILEPATH_LEN_MAX];
   eConfigFAT          tableentry;
   unsigned long       index;

   if (!loc)
      return 0;
   if (!path)
      return 0;

   index = 0;
   sprintf(tablepath, "%s/fat", path);
   FAT_TABLE = fopen(tablepath, "r+");
   if (FAT_TABLE)
     {
	while (!feof(FAT_TABLE))
	  {
	     fread(&tableentry, sizeof(eConfigFAT), 1, FAT_TABLE);
	     if (!strcmp(tableentry.loc, loc))
	       {
		  fseek(FAT_TABLE, (sizeof(eConfigFAT)) * index, SEEK_SET);
		  sprintf(tableentry.loc, "dirty");
		  tableentry.length = 0;
		  tableentry.position = 0;
		  if (fwrite(&tableentry, sizeof(eConfigFAT), 1, FAT_TABLE) <
		      sizeof(eConfigFAT))
		    {
		       /* this is probably not a good error here either, it means
		        * our write failed.  need to have a handler here, too.
		        */
		       fclose(FAT_TABLE);
		       return 0;
		    }
		  fclose(FAT_TABLE);
		  return 1;
	       }
	     index++;
	  }
     }
   else
     {
	/* We couldn't open the file for writing.  oops.  sucks to be us. */
	return 0;
     }

   return 0;

}

int
_econf_purge_data_from_disk(char *loc)
{

   /* This tries to remove a value completely from all writable config files.
    * It searches the path and dirties the contents using
    * _econf_purge_data_from_disk_at_path() on every place in the path it can
    * find it
    * returns the number of unsuccessful deletions it had.
    * 0 is success (completely)
    * This function is internal to eConfig.
    */

   char              **paths;
   int                 num;
   int                 num_undeleted;

   if (!loc)
      return 0;

   num_undeleted = 0;

   if ((paths = eConfigPaths(&num)))
     {
	int                 i;
	unsigned long       length;
	unsigned long       position;
	unsigned long       timestamp;

	for (i = 0; i < num; i++)
	  {
	     if ((length =
		  _econf_finddatapointerinpath(paths[i], loc, &position,
					       &timestamp)))
	       {
		  if (_econf_purge_data_from_disk_at_path(loc, paths[i]))
		    {
		       num_undeleted++;
		    }
	       }
	  }
	free(paths);
     }
   /* num_undeleted should be set to 0 if everything went according to plan
    * -- it would be nice if I could count how many times it was successfully
    * deleted also, but that's outside the api as spec'd
    */

   return num_undeleted;

}

int
_econf_create_new_data_repository(char *path)
{

   /* This function will create all the stub files necessary for a new
    * repository, as well as create the directory they're housed in.
    * returns a negative number on failure.
    * returns a 0 or higher on success
    * (should return the number of un-necessary creations)
    * This function is internal to eConfig
    */

   struct stat         st;
   char                testingfile[FILEPATH_LEN_MAX];
   int                 retval;

   if (!path)
     {
	return -1;
     }
   retval = 0;
   if (stat(path, &st) < 0)
     {
	/* the physical path doesn't exist so we'll have to create it */
	if (mkdir(path, S_IRWXU) < 0)
	  {
	     /* we couldn't make the directory -  return an error */
	     return -4;
	  }
	stat(path, &st);
     }
   else
     {
	/* we have something there, let's take a look and see what it is */
	if (!S_ISDIR(st.st_mode))
	  {
	     /* we're obviously not a directory, so what is the problem? */
	     if (S_ISREG(st.st_mode))
	       {
		  /* The directory is a regular file.  this is not a good thing.
		   * return a -2 error
		   */
		  return -2;
	       }
	     /* some other oddity here happened, return "unknown issue" error */
	     return -3;
	  }
	retval++;
     }

   /* now we have a directory, it's time to populate it with some files */

   sprintf(testingfile, "%s/fat", path);
   if (stat(testingfile, &st) < 0)
     {
	/* the fat table doesn't exist so lets create it */
	FILE               *FATTABLE;

	FATTABLE = fopen(testingfile, "w");
	if (!FATTABLE)
	  {
	     /* we couldn't write to that file.  return an error */
	     return -6;
	  }
	fclose(FATTABLE);

     }
   else
     {
	if (!S_ISREG(st.st_mode))
	  {
	     /* we're not a regular file, so there are probably some issues
	      * here - returning an error */
	     return -5;
	  }
	retval++;
     }

   sprintf(testingfile, "%s/data", path);
   if (stat(testingfile, &st) < 0)
     {
	/* the data table doesn't exist so lets create it */
	FILE               *CONFTABLE;

	CONFTABLE = fopen(testingfile, "w");
	if (!CONFTABLE)
	  {
	     /* we couldn't write to that file.  return an error */
	     return -7;
	  }
	fclose(CONFTABLE);

     }
   else
     {
	if (!S_ISREG(st.st_mode))
	  {
	     /* we're not a regular file, so there are probably some issues
	      * here - returning an error */
	     return -5;
	  }
	retval++;
     }

   return retval;

}
