#include <stdio.h>
#include <stdlib.h>
#include "Ewd.h"

#define MAX_VAL 10

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
	while ((i = (int)ewd_list_next(list))) {
		printf("Value: %d\n", i);
		ewd_list_remove(list);
	}

	printf("Got through the second loop\n");

	ewd_list_destroy(list);

	return TRUE;
}
