#include "eConfig.internal.h"
#include "eConfig.h"


int _econf_finddatapointerinpath(char *path,char *loc,unsigned long *length) {

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
			*length = tableentry.length;
			return tableentry.position;
	    }
	}

	fclose(FAT_TABLE);

	return 0;

}

void * _econf_get_data_from_disk(char *loc,unsigned long *length) {

	char **paths;
	int num;


	if((paths = eConfigPaths(&num))) {
		int i;

		for(i=0;i<num;i++) {


		}
	}

	*length = 0;
	return NULL;

}
