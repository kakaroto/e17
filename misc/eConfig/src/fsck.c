#include "eConfig.internal.h"
#include "eConfig.h"

int
eConfigFsckPath(char *path)
{

	/* This function is supposed to fsck a theme at the specified location
	 * *path.  It is exported by the command.  returns a 0 on failure and
	 * a 1 on success.
	 */

	if(!path)
		return 0;

	return 1;

}

