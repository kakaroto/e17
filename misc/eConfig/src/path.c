#include "eConfig.internal.h"
#include "eConfig.h"

eConfigType eConfig;

int eConfigAddPath(char *path) {
	/* This function Initializes the Configuration Engine.  
	 * It is called externally by the loading application.
	 */

	PathStruct *cur_path;

	if(!path)
		return 0;

	cur_path = eConfig.paths;
	while(cur_path) {
		if(!strcmp(cur_path->path,path))
			return 2;
		cur_path = cur_path->next;
	}

	cur_path = eConfig.paths;
	eConfig.paths = malloc(sizeof(eConfigType));
	eConfig.paths->next = cur_path;
	strcpy(eConfig.paths->path,path);

	return 1;

}

int eConfigRemovePath(char *path) {

	PathStruct *cur_path;

	if(!path)
		return 0;

	cur_path = eConfig.paths;
	if(cur_path) {
		while(cur_path) {
			if(!strcmp(cur_path->path,path)) {
				free(cur_path->path);
				cur_path = cur_path->next;
				free(cur_path);
				return 1;
			}
			if(cur_path)
				cur_path = cur_path->next;
		}
	} else {
		return 2;
	}

	return 2;
}

char ** eConfigPaths(void) {

	PathStruct *cur_path;
	char **paths;
	int i;

	cur_path = eConfig.paths;
	paths = malloc(sizeof(char *));
	if(cur_path) {
		i=0;
		while(cur_path) {
			paths = realloc(paths,(sizeof(char *))*++i);
			paths[i-1] = cur_path->path;
		}
	} else {
		return NULL;
	}

	return paths;

}
