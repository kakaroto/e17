#include <stdio.h>
#include <stdlib.h>
#include "Ewd.h"

#define MAX_VAL 10

int main()
{
        int i = 1;

        Ewd_Tree *tree;

        tree = ewd_tree_new(NULL);

        while (i < MAX_VAL) {
                ewd_tree_set(tree, (void *)i, (void *)(i + 1));
                i++;
        }
	printf("Got through the first loop\n");

	while (--i) {
		int value;

		printf("Key: %d\t", i);
		value = (int)ewd_tree_get(tree, (void *)i);
		printf("Value: %d\n", value);
		fflush(stdout);

	}
	printf("Got through the second loop\n");

	ewd_tree_destroy(tree);

	return TRUE;
}
