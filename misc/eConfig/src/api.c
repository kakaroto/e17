#include "eConfig.internal.h"
#include "eConfig.h"


void *
eConfigGetData(char *loc, unsigned long *length)
{

	/* This function will take a key (loc) and return arbitrary data from the
	 * first convenient location it finds in the themepath.  See path.c for
	 * details on how the paths work.  It returns a (void *) to the data,
	 * as well as the length (in bytes) of the data it points to.
	 * DO NOT free the pointer yourself.  Free it using eConfigUnloadData().
	 */

	eConfigData *cur_data;
	void *data;

	if(!loc) {
		*length = 0;
		return NULL;
	}

	cur_data = eConfig.data;

	if(cur_data) {
		while(cur_data) {
			if(!strcmp(cur_data->loc,loc)) {
				cur_data->refcount++;
				*length = cur_data->length;
				return cur_data->data;
			}
			cur_data = cur_data->next;
		}
	}

	if((data = _econf_get_data_from_disk(loc,length))) {
		eConfigUpdateCache();
		return data;
	}

	eConfigUpdateCache();

	*length = 0;
	return NULL;

}


void *
eConfigRefreshData(char *loc, unsigned long *length)
{

	/* This function will take a key (loc) and return arbitrary data from the
	 * first convenient location it finds in the themepath.  See path.c for
	 * details on how the paths work.  It returns a (void *) to the data,
	 * as well as the length (in bytes) of the data it points to.
	 * DO NOT free the pointer yourself.  Free it using eConfigUnloadData().
	 * This function should be used to refresh a current pointer without
	 * unloading it.
	 */

	eConfigData *cur_data;

	if(!loc) {
		*length = 0;
		return NULL;
	}

	cur_data = eConfig.data;

	if(cur_data) {
		while(cur_data) {
			if(!strcmp(cur_data->loc,loc)) {
				*length = cur_data->length;
				return cur_data->data;
			}
			cur_data = cur_data->next;
		}
	}

	eConfigUpdateCache();
	*length = 0;
	return NULL;

}

int
eConfigUnloadData(char *loc)
{

	/* This function will unload the data for you at the location specified.
	 * You should use this function instead of freeing the data you retreived
	 * by hand.  This way everything works with the caching system properly
	 * The function takes the same key (loc) that you used for eConfigGetData
	 */

	eConfigData *cur_data;

	if(!loc)
		return 0;

	cur_data = eConfig.data;

	if(cur_data) {
		while(cur_data) {
			if(!strcmp(cur_data->loc,loc)) {
				cur_data->refcount--;
				if(cur_data->refcount < 1) {
					eConfigUpdateCache();
				}
				return 1;
			}
			cur_data = cur_data->next;
		}
	}

	eConfigUpdateCache();

	return 0;

}

int
eConfigStoreData(char *loc, void *data, unsigned long length, char *path)
{

	if(!loc)
		return 0;
	if(!data)
		return 0;
	if(!length)
		return 0;
	if(!path)
		return 0;

	return 0;

}
