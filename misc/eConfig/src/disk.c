#include "eConfig.internal.h"
#include "eConfig.h"


unsigned long _econf_finddatapointerinpath(char *path,char *loc,
                                           unsigned long *position) {

	/* This function is internal to eConfig
	 * its goal is to see if it can find the data specified (loc)
	 * inside of the theme * data at the path (path).
	 * if found, it sets and returns the position in the theme along with the
	 * length of the data in *length
	 * This function is internal to eConfig.
	 */

	FILE *FAT_TABLE;
	char tablepath[FILEPATH_LEN_MAX];
	eConfigFAT tableentry;


	if(!path)
		return 0;
	
	sprintf(tablepath,"%s/fat",path);
	FAT_TABLE = fopen(tablepath,"r");
	while(!feof(FAT_TABLE)) {
		fread(&tableentry,sizeof(eConfigFAT),1,FAT_TABLE);
		if(!strcmp(tableentry.loc,loc)) {
			fclose(FAT_TABLE);
			*position = tableentry.position;
			return tableentry.length;
	    }
	}

	fclose(FAT_TABLE);

	/* returning a length of zero implies no data to be found */
	return 0;

}

void * _econf_get_data_from_disk(char *loc,unsigned long *length) {

	/* This function is internal to eConfig
	 * it searches all the themepaths to find the correct data.
	 * it will return it, as well as return the length in *length
	 * This function is internal to eConfig.
	 */

	char **paths;
	int num;


	if((paths = eConfigPaths(&num))) {
		int i;
		unsigned long position;

		for(i=0;i<num;i++) {
			if((*length =
				_econf_finddatapointerinpath(paths[i],loc,&position))) {
				FILE *CONF_TABLE;
				char confpath[FILEPATH_LEN_MAX];
				char *allocedspace;

				allocedspace = malloc(*length + 1);
				sprintf(confpath,"%s/data",paths[i]);
				CONF_TABLE = fopen(confpath,"r");

				fseek(CONF_TABLE,position,SEEK_SET);
				fread(allocedspace,*length,1,CONF_TABLE);
				fclose(CONF_TABLE);

				free(paths);
				return allocedspace;
			}
		}
		free(paths);
	}


	/* obviously we didn't find it anywhere in here */

	*length = 0;
	return NULL;

}

int _econf_save_data_to_disk_at_position(unsigned long position,char *path,
		unsigned long length, void *data) {

	/* This function is pretty simplistic.  it just saves out a bit of *data to
	 * the theme at *path, into the data file at position, and knows it is
	 * length long.  It returns a 0 on failure, and a 1 on success.
	 * This function is internal to eConfig
	 */

	FILE *CONF_TABLE;
	char confpath[FILEPATH_LEN_MAX];

	if(!position)
		return 0;
	if(!path)
		return 0;
	if(!length)
		return 0;
	if(!data)
		return 0;

	sprintf(confpath,"%s/data",path);
	CONF_TABLE = fopen(confpath,"r+");
	if(CONF_TABLE) {
		fseek(CONF_TABLE,position,SEEK_SET);
		if(fwrite(data,length,1,CONF_TABLE) < length) {
			/* oh shit, we didn't write enough data.  maybe we need
			 * to somehow mark all these errors into something useful
			 */
			fclose(CONF_TABLE);
			return 0;
		}
		fclose(CONF_TABLE);
	} else {
		return 0;
	}

	return 1;

}

int _econf_new_fat_entry_to_disk(char *loc, unsigned long length, char *path) {

	/* This function creates a new FAT table entry at the specified location
	 * *path for the variable *loc of length length. 
	 * This function is for internal use by eConfig only
	 */

	FILE *FAT_TABLE;
	char tablepath[FILEPATH_LEN_MAX];
	eConfigFAT tableentry;

	if(!path)
		return 0;
	if(!loc)
		return 0;
	if(!length)
		return 0;

	sprintf(tablepath,"%s/fat",path);
	FAT_TABLE = fopen(tablepath,"r+");
	if(FAT_TABLE) {
		memset(&tableentry,0,sizeof(eConfigFAT));
		sprintf(tableentry.loc,"%s",loc);
		tableentry.length = length;
		tableentry.usage_index = 0;
		tableentry.updated_on = _econf_timestamp();
		fclose(FAT_TABLE);
	} else {
		/* we failed to open the file for writing properly.
		 * This means we can't write anything to the disk.
		 * This is an error. :)
		 */
		return 0;
	}

	return 0;

}

int _econf_save_data_to_disk(void *data, char *loc, unsigned long length, char local) {

	/* This function is supposed to save data out to disk.  it really needs to
	 * be slapped since it really isn't doing what I think it should be doing.
	 * FIXME: this function doesn't work right.
	 * This function is internal to eConfig.
	 */

	char **paths;
	int num;

	if(!data)
		return 0;
	if(!loc)
		return 0;
	if(!length)
		return 0;

	if((paths = eConfigPaths(&num))) {
		int i;
		unsigned long oldlength;
		unsigned long position;
		for(i=0;i<num;i++) {
			if((oldlength =
				_econf_finddatapointerinpath(paths[i],loc,&position))) {
				if(oldlength >= length) {
					if(!_econf_save_data_to_disk_at_position(position,paths[i],
							length,data)) {
						/* We failed writing to the disk.  This is probably
						 * bad.
						 */
					} else {
						/* write successful */
					}
				} else {
					if(_econf_purge_data_from_disk_at_path(loc,paths[i])) {
						/* purge succeeded */
					} else {
						/* We failed purging it somehow, even though we found
						 * it...  This is probably not a good place to be.
						 */
					}
				}
			}
		}
		free(paths);
	}

	/* We're somehow trying to save data without HAVING anywhere to save it
	 * to, which is probably a bad thing
	 */

	return 0;

}

int _econf_purge_data_from_disk_at_path(char *loc, char *path) {

	/* This function is supposed to "dirty" the data inside of a disk at any
	 * particular location.  typically used because your data is:
	 * a) no longer viable
	 * b) no longer usable
	 * c) too short for the new data you have to save into this file
	 * if your dataspace is marked as dirty it will be fsck()d out at the
	 * next instance of eConfigFsckPath() on *path.
	 * This function is internal to eConfig.
	 */

	FILE *FAT_TABLE;
	char tablepath[FILEPATH_LEN_MAX];
	eConfigFAT tableentry;
	unsigned long index;

	if(!loc)
		return 0;
	if(!path)
		return 0;

	index=0;
	sprintf(tablepath,"%s/fat",path);
	FAT_TABLE = fopen(tablepath,"r+");
	if(FAT_TABLE) {
		while(!feof(FAT_TABLE)) {
			fread(&tableentry,sizeof(eConfigFAT),1,FAT_TABLE);
			if(!strcmp(tableentry.loc,loc)) {
				fseek(FAT_TABLE,(sizeof(eConfigFAT))*index,SEEK_SET);
				sprintf(tableentry.loc,"dirty");
				tableentry.length = 0;
				tableentry.position = 0;
				if(fwrite(&tableentry,sizeof(eConfigFAT),1,FAT_TABLE) <
						sizeof(eConfigFAT)) {
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
	} else {
		/* We couldn't open the file for writing.  oops.  sucks to be us. */
		return 0;
	}

	return 0;

}

int _econf_purge_data_from_disk(char *loc) {

	/* This tries to remove a value completely from all writable config files.
	 * It searches the path and dirties the contents using
	 * _econf_purge_data_from_disk_at_path() on every place in the path it can
	 * find it
	 * returns the number of unsuccessful deletions it had.
	 * 0 is success (completely)
	 * This function is internal to eConfig.
	 */

	char **paths;
	int num;
	int num_undeleted;

	if(!loc)
		return 0;

	num_undeleted=0;

	if((paths = eConfigPaths(&num))) {
		int i;
		unsigned long length;
		unsigned long position;
		for(i=0;i<num;i++) {
			if((length =
				_econf_finddatapointerinpath(paths[i],loc,&position))) {
				if(_econf_purge_data_from_disk_at_path(loc, paths[i])) {
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
