#include <../config.h>
#include <Ewd.h>

Ewd_Hash *ewd_hash_new()
{
	Ewd_Hash *new = (Ewd_Hash *)malloc(sizeof(Ewd_Hash));

	if (!ewd_hash_init(new)) {
		FREE(new);
		return NULL;
	}

	return new;
}

int ewd_hash_init(Ewd_Hash *hash)
{
	CHECK_PARAM_POINTER_RETURN("hash", hash, FALSE);

	memset(hash, 0, sizeof(Ewd_Hash));

#ifdef __USE_PTHREADS
	hash->lock = PTHREAD_MUTEX_INITIALIZER;
#endif

}
