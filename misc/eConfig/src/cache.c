#include "eConfig.internal.h"
#include "eConfig.h"

int    eConfigUpdateCache(void) {

	/* this function should force an update of all the cache in memory
	 * currently it does nothing :)
	 */

	return 0;

}

void   eConfigSetCacheSize(unsigned long newsize) {

	eConfig.cachesize = newsize;
	eConfigUpdateCache();

	return;

}

unsigned long  eConfigGetCacheSize(void) {

	return eConfig.cachesize;

}

unsigned long eConfigGetCacheFilled(void) {

	return eConfig.cachesize;

}
