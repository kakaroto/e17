#include "eConfig.internal.h"
#include "eConfig.h"

unsigned long
_econf_timestamp(void)
{

	/* this function is supposed to set a timestamp in a portable
	 * fashion, since time_t in a binary file won't work right.
	 * FIXME: this function needs to actually do something.
	 */
	
	return 0;

}
