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
#define HASH_FREE_KEY(hash)

#else

#include "Ewd.h"
#define HASH_TABLE Ewd_Hash
#define HASH_NEW(arg1, arg2) ewd_hash_new(arg1, arg2)
#define HASH_SET(hash, key, value) ewd_hash_set(hash, key, value)
#define HASH_GET(hash, key) ewd_hash_get(hash, key)
#define HASH_REMOVE(hash, key) ewd_hash_remove(hash, key)
#define HASH_DESTROY(hash) ewd_hash_destroy(hash)
#define HASH_DUMP(hash) ewd_hash_dump_graph(hash)
#define HASH_FREE_KEY(hash, function) ewd_hash_set_free_key(hash, function)

#endif

#define WORDLEN 1024
#define DICTIONARY "/usr/share/dict/words"

int main()
{
        int i = 1;
	FILE *dict;
	char buffer[WORDLEN];
	char *oldbuffer = NULL;
	clock_t sys_use, user_use;
	struct tms start_buf, end_buf;
        HASH_TABLE *hash;

	dict = fopen(DICTIONARY, "r");
	if (!dict) {
		perror("fopen");
		exit(1);
	}

	times(&start_buf);

        hash = HASH_NEW(ewd_str_hash, ewd_str_compare);

	HASH_FREE_KEY(hash, free);

	while (fgets(buffer, WORDLEN, dict)) {
		int value;
		char *temp;

		temp = strdup(buffer);
		HASH_SET(hash, temp, (void *)i);

		value = HASH_GET(hash, oldbuffer);
		if (oldbuffer && (!value || value != i - 1))
			fprintf(stderr, "Set failed\n");

		oldbuffer = temp;

		if (i % 10 == 0) {
			HASH_REMOVE(hash, temp);
			if (HASH_GET(hash, temp))
				fprintf(stderr, "Delete failed\n");;
			HASH_SET(hash, temp, (void *)i);
		}
                i++;
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
