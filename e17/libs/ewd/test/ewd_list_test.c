#include <stdio.h>
#include <stdlib.h>
#include "Ewd.h"

/* #define USE_GLIB */

#ifdef USE_GLIB

#include <glib.h>
#define LIST GSList
#define LIST_NEW(arg1, arg2) g_slist_new(arg1, arg2)
#define LIST_SET(list, key, value) g_list_prepend(hash, value)
#define LIST_GET(hash, key) g_hash_table_lookup(hash, key)
#define LIST_REMOVE(hash, key) g_hash_table_remove(hash, key)
#define LIST_DESTROY(hash) g_hash_table_destroy(hash)

#else

#include "Ewd.h"
#define LIST Ewd_Hash
#define LIST_NEW(arg1, arg2) ewd_hash_new(arg1, arg2)
#define LIST_SET(hash, key, value) ewd_hash_set(hash, key, value)
#define LIST_GET(hash, key) ewd_hash_get(hash, key)
#define LIST_REMOVE(hash, key) ewd_hash_remove(hash, key)
#define LIST_DESTROY(hash) ewd_hash_destroy(hash)

#endif

#define MAX_VAL 100

int main()
{
        int i = 1;

        Ewd_List *list;

        list = ewd_list_new(NULL);

        while (i < MAX_VAL) {
                ewd_list_insert(list, (void *)i);
                i++;
        }
	printf("Got through the first loop\n");

	ewd_list_goto_first(list);
	while ((i = (int)ewd_list_remove_last(list))) {
		printf("Value: %d\n", i);
	}

	printf("Got through the second loop\n");

	ewd_list_destroy(list);

	return TRUE;
}
