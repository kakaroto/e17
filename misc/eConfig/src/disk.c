#include "eConfig.internal.h"
#include "eConfig.h"


unsigned long _econf_finddatapointerinpath(char *path,char *loc,
                                           unsigned long *position) {

	/* This function is internal to eConfig
	 * its goal is to see if it can find the data specified (loc)
	 * inside of the theme * data at the path (path).
	 * if found, it sets and returns the position in the theme along with the
	 * length of the data in *length
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

int _econf_save_data_to_disk(void *data, char *loc, unsigned long length) {

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
		int newlength;
		for(i=0;i<num;i++) {

		}
		free(paths);
	}

	return 0;
}

int _econf_purge_data_from_disk(char *loc) {

	if(!loc)
		return 0;

	return 0;
}
