#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>

/* #define USE_GLIB */

#ifdef USE_GLIB

#include <glib.h>
#define HASH_TABLE GHashTable
#define HASH_NEW(arg1, arg2) g_hash_table_new(arg1, arg2)
#define HASH_SET(hash, key, value) g_hash_table_insert(hash, key, value)
#define HASH_GET(hash, key) g_hash_table_lookup(hash, key)
#define HASH_REMOVE(hash, key) g_hash_table_remove(hash, key)
#define HASH_DESTROY(hash) g_hash_table_destroy(hash)
#define HASH_DUMP(hash)

#else

#include "Ewd.h"
#define HASH_TABLE Ewd_Hash
#define HASH_NEW(arg1, arg2) ewd_hash_new(arg1, arg2)
#define HASH_SET(hash, key, value) ewd_hash_set(hash, key, value)
#define HASH_GET(hash, key) ewd_hash_get(hash, key)
#define HASH_REMOVE(hash, key) ewd_hash_remove(hash, key)
#define HASH_DESTROY(hash) ewd_hash_destroy(hash)
#define HASH_DUMP(hash) ewd_hash_dump_graph(hash)

#endif

#define MAX_VAL 10000

int main()
{
        int i = 1;
        HASH_TABLE *hash;
	clock_t sys_use, user_use;
	struct tms start_buf, end_buf;

	times(&start_buf);

        hash = HASH_NEW(NULL, NULL);

        while (i < MAX_VAL) {
                HASH_SET(hash, (void *)i, (void *)(i + 1));
		if (i % 10 == 0) {
			HASH_REMOVE(hash, i);
			if (HASH_GET(hash, (void *)i))
				fprintf(stderr, "Delete failed\n");;
		}
                i++;
        }
/*	printf("Got through the first loop\n"); */

	while (--i) {
		int value;

		value = (int)HASH_GET(hash, (void *)i);
		if (!value)
			fprintf(stderr, "Could not find key: %d\n", i);

	}

	times(&end_buf);
	sys_use = end_buf.tms_stime - start_buf.tms_stime;
	user_use = end_buf.tms_utime - start_buf.tms_utime;

	fprintf(stderr, "Used %ld ms of system time total\n", sys_use);
	fprintf(stderr, "Used %ld ms of user time total\n", user_use);

	HASH_DUMP(hash);

	HASH_DESTROY(hash);

	return TRUE;
}
