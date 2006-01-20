#include "evfs.h"

static long evfs_operation_count = 0;
static int evfs_operation_init = 0;

void evfs_operation_initialise() 
{
	if (evfs_operation_init) 
		return;

	evfs_operation_init = 1;

	evfs_operation_count = 0;
}
	

evfs_operation* evfs_operation_new


