#include "evfs.h"

static long evfs_operation_count = 0;
static int evfs_operation_init = 0;
static Ecore_Hash* evfs_operation_hash;

void evfs_operation_initialise() 
{
	if (evfs_operation_init) 
		return;

	evfs_operation_init = 1;
	evfs_operation_count = 0;

	evfs_operation_hash = ecore_hash_new(ecore_direct_hash, ecore_direct_compare);
}
	

evfs_operation* evfs_operation_new() 
{
	evfs_operation_count++;
	
	evfs_operation* op = NEW(evfs_operation);
	op->id = evfs_operation_count;
	op->status = EVFS_OPERATION_STATUS_NORMAL;

	ecore_hash_set(evfs_operation_hash, (long*)op->id, op);
	
	return op;
}

void evfs_operation_destroy(evfs_operation* op) 
{
	ecore_hash_remove(evfs_operation_hash, (long*)op->id);
	free(op);
}

evfs_operation* evfs_operation_get_by_id(long id) 
{
	return ecore_hash_get(evfs_operation_hash, (long*)id);
}

void evfs_operation_status_set(evfs_operation* op, int status) {
	op->status = status;
}

void evfs_operation_user_dispatch(evfs_client* client, evfs_operation* op) {
	printf("stub");
}


