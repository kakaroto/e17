#include <Ewd.h>
#include <stdio.h>

#define MAX_VAL 10000

int main()
{
	int i, value;
	Ewd_Sheap *heap;

	/*
	 * For this test use direct comparison of integers for determining
	 * priority.
	 */
	heap = ewd_sheap_new(NULL, 20);

	for (i = 0, value = 13; value < MAX_VAL; i++, value += 13) {
		ewd_sheap_insert(heap, (void *)value);
		printf("Top of heap %d\n", (int)ewd_sheap_extreme(heap));

		/*
		 * Remove some items in the queue to make sure extracts
		 * followed by inserts continue to work correctly.
		 */
		if (i % 3 == 0) {
			printf("Removed %d from the heap\n",
					(int)ewd_sheap_extract(heap)); 
		}
	}

	/*
	 * Empty the heap
	 */
	while ((value = (int)ewd_sheap_extract(heap)))
		printf("Popped %d off the heap\n", value);

	return TRUE;
}
