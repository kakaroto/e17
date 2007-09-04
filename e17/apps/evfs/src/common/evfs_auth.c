#include "evfs.h"

static Ecore_Hash* _evfs_auth_hash;
static int _evfs_auth_cache_init = 0;

void evfs_auth_cache_init()
{
	if (!_evfs_auth_cache_init) {
		_evfs_auth_cache_init = 1;
		_evfs_auth_hash = ecore_hash_new(ecore_str_hash, 
			ecore_str_compare);

	}
}


