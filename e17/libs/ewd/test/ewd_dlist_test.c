#include <stdio.h>
#include <stdlib.h>
#include "Ewd.h"

#define MAX_VAL 10

int main()
{
        int i = 1;

        Ewd_DList *list;

        list = ewd_dlist_new(NULL);

        while (i < MAX_VAL) {
                ewd_dlist_insert(list, (void *)i);
                i++;
        }
	printf("Got through the first loop\n");

	ewd_dlist_goto_first(list);
	while ((i = (int)ewd_dlist_next(list)))
		printf("Value: %d\n", i);

	printf("Got through the second loop\n");

	ewd_dlist_goto_last(list);
	while ((i = (int)ewd_dlist_previous(list)))
		printf("Value: %d\n", i);

	printf("Got through the third loop\n");

	ewd_dlist_destroy(list);

	return TRUE;
}
