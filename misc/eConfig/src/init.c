#include "eConfig.internal.h"
#include "eConfig.h"

void eConfigInit(void) {

	/* This function just initializes a bunch of pointers and such. */

	eConfig.paths = NULL;
	eConfig.data = NULL;

	return;

}
