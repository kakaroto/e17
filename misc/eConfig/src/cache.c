#include "eConfig.internal.h"
#include "eConfig.h"

int    eConfigUpdateCache(void) {

	/* this function should force an update of all the cache in memory
	 * currently it does nothing :)
	 * This function is exported by eConfig.
	 */

	return 0;

}

void   eConfigSetCacheSize(unsigned long newsize) {

	/* This function allows an application to set the cache size
	 * that eConfig will use.
	 * This function is exported by eConfig.
	 */

	eConfig.cachesize = newsize;
	eConfigUpdateCache();

	return;

}

unsigned long  eConfigGetCacheSize(void) {

	/* This function will return the current cachesize that 
	 * eConfig is using.
	 * This function is exported by eConfig.
	 */

	return eConfig.cachesize;

}

unsigned long eConfigGetCacheFilled(void) {

	/* This function will return the current amount of filled data
	 * that eConfig has used in the cache size.  Useful for displaying
	 * statistics, as well as trimming the size of the cache using
	 * eConfigSetCacheSize().
	 * This function is exported by eConfig.
	 */

	return eConfig.cachesize;

}
