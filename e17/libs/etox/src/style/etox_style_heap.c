#include "etox_style_private.h"

static void __etox_style_heap_heapify(Etox_Style_Heap * heap, int i);
static void __etox_style_heap_update_data(Etox_Style_Heap * heap);

/**
 * _etox_style_heap_new - allocate and initialize a new binary heap
 * @compare: the function for comparing keys, NULL for direct comparison
 * @size: the number of elements to allow in the heap
 *
 * Returns a pointer to the newly allocated binary heap on success, NULL on
 * failure.
 */
Etox_Style_Heap *_etox_style_heap_new(Ecore_Compare_Cb compare, int size)
{
	Etox_Style_Heap *heap = NULL;

	heap = (Etox_Style_Heap *) malloc(sizeof(Etox_Style_Heap));
	if (!heap)
		return NULL;
	memset(heap, 0, sizeof(Etox_Style_Heap));

	if (!_etox_style_heap_init(heap, compare, size)) {
		FREE(heap);
		return NULL;
	}

	return heap;
}

/**
 * _etox_style_heap_init - initialize a binary heap to default values
 * @heap: the heap to initialize
 * @compare: the function for comparing keys, NULL for direct comparison
 * @size: the number of elements to allow in the heap
 *
 * Returns TRUE on success, FALSE on failure
 */
int _etox_style_heap_init(Etox_Style_Heap * heap, Ecore_Compare_Cb compare, int size)
{
	heap->space = size;
	if (!compare)
		heap->compare = ecore_direct_compare;
	else
		heap->compare = compare;
	heap->order = ECORE_SHEAP_MIN;

	heap->data = (void **) malloc(heap->space * sizeof(void *));
	if (!heap->data)
		return FALSE;
	memset(heap->data, 0, heap->space * sizeof(void *));

	return TRUE;
}

/**
 * _etox_style_heap_destroy - free up the memory used by the heap
 * @heap: the heap to be freed
 *
 * Returns no value. Free's the memory used by @heap, calls the destroy
 * function on each data item if necessary.
 */
void _etox_style_heap_destroy(Etox_Style_Heap * heap)
{
	/*
	 * FIXME: Need to setup destructor callbacks for this class.
	 */
	FREE(heap->data);

	FREE(heap);
}

/**
 * _etox_style_heap_insert - insert new data into the heap
 * @heap: the heap to insert @data
 * @data: the data to add to @heap
 *
 * Returns TRUE on success, NULL on failure. Increases the size of the heap if
 * it becomes larger than available space.
 */
int _etox_style_heap_insert(Etox_Style_Heap * heap, void *data)
{
	int i;
	void *temp;
	int parent;
	int position;

	/*
	 * Increase the size of the allocated data area if there isn't enough
	 * space available to add this data
	 */
	if (heap->size >= heap->space)
		return FALSE;

	heap->sorted = FALSE;

	/*
	 * Place the data at the end of the heap initially. Then determine the
	 * parent and position in the array of it's parent.
	 */
	heap->data[heap->size] = data;
	position = heap->size;
	heap->size++;
	i = heap->size;
	parent = PARENT(i) - 1;

	/*
	 * Check the order of the heap to decide where to place the inserted
	 * data. The loop is placed inside the if statement to reduce the
	 * number of branching decisions that must be predicted.
	 */
	if (heap->order == ECORE_SHEAP_MIN) {
		while ((position > 0) && heap->compare(heap->data[parent],
						       heap->
						       data[position]) >
		       0) {

			/*
			 * Swap the data with it's parents to move it up in
			 * the heap.
			 */
			temp = heap->data[position];
			heap->data[position] = heap->data[parent];
			heap->data[parent] = temp;

			/*
			 * Now determine the new position for the next
			 * iteration of the loop, as well as it's parents
			 * position.
			 */
			i = PARENT(i);
			position = i - 1;
			parent = PARENT(i) - 1;
		}
	} else {
		while ((position > 0) && heap->compare(heap->data[parent],
						       heap->
						       data[position]) <
		       0) {

			/*
			 * Swap the data with it's parents to move it up in
			 * the heap.
			 */
			temp = heap->data[position];
			heap->data[position] = heap->data[PARENT(i) - 1];
			heap->data[PARENT(i) - 1] = temp;

			/*
			 * Now determine the new position for the next
			 * iteration of the loop, as well as it's parents
			 * position.
			 */
			i = PARENT(i);
			position = i - 1;
			parent = PARENT(i) - 1;
		}
	}

	return TRUE;
}

/**
 * _etox_style_heap_extract - extract the item at the top of the heap
 * @heap: the heap to remove the top item
 *
 * Returns the top item of the heap on success, NULL on failure. The extract
 * function maintains the heap properties after the extract.
 */
void *_etox_style_heap_extract(Etox_Style_Heap * heap)
{
	void *extreme;

	if (heap->size < 1)
		return NULL;

	heap->sorted = FALSE;

	extreme = heap->data[0];
	heap->size--;
	heap->data[0] = heap->data[heap->size];

	__etox_style_heap_heapify(heap, 1);

	return extreme;
}

/**
 * _etox_style_heap_extreme - examine the item at the top of the heap
 * @heap: the heap to examine the top item
 *
 * Returns the top item of the heap on success, NULL on failure. The function
 * does not alter the heap.
 */
void *_etox_style_heap_extreme(Etox_Style_Heap * heap)
{
	if (heap->size < 1)
		return NULL;

	return heap->data[0];
}

/**
 * _etox_style_heap_change - change the value of the specified item in the heap
 * @heap: the heap to search for the item to change
 * @item: the item in the heap to change
 * @newval: the new value assigned to the item in the heap
 *
 * Returns TRUE on success, FALSE on failure. The heap does not free the old
 * data since it must be passed in, so the caller can perform the free if
 * desired.
 */
int _etox_style_heap_change(Etox_Style_Heap * heap, void *item, void *newval)
{
	int i;

	for (i = 0; i < heap->size && heap->data[i] != item; heap++);

	if (i < heap->size)
		heap->data[i] = newval;
	else
		return FALSE;

	/*
	 * FIXME: This is not the correct procedure when a change occurs.
	 */
	__etox_style_heap_heapify(heap, 1);

	return TRUE;
}

/**
 * _etox_style_heap_set_compare - change the comparison function for the heap
 * @heap: the heap to change comparison function
 * @compare: the new function for comparing nodes
 *
 * Returns TRUE on success, FALSE on failure. The comparison function is
 * changed to @compare and the heap is heapified by the new comparison.
 */
int _etox_style_heap_set_compare(Etox_Style_Heap * heap, Ecore_Compare_Cb compare)
{
	if (!compare)
		heap->compare = ecore_direct_compare;
	else
		heap->compare = compare;

	__etox_style_heap_update_data(heap);

	return TRUE;
}

/**
 * _etox_style_heap_set_order - change the order of the heap
 * @heap: the heap to change the order
 * @order: the new order of the heap
 *
 * Returns no value. Changes the heap order of @heap and re-heapifies the data
 * to this new order. The default order is a min heap.
 */
void _etox_style_heap_set_order(Etox_Style_Heap * heap, char order)
{
	heap->order = order;

	__etox_style_heap_update_data(heap);
}

/**
 * _etox_style_heap_sort - sort the data in the heap
 * @heap: the heap to be sorted
 *
 * Returns no value. Sorts the data in the heap into the order that is used
 * for the heap's data.
 */
void _etox_style_heap_sort(Etox_Style_Heap * heap)
{
	int i = 0;
	void **new_data;

	new_data = (void **) malloc(heap->size * sizeof(void *));

	/*
	 * Extract the heap and insert into the new data array in order.
	 */
	while (heap->size > 0)
		new_data[i++] = _etox_style_heap_extract(heap);

	/*
	 * Free the old data array and update the heap with the new data, also
	 * mark as sorted.
	 */
	FREE(heap->data);
	heap->data = new_data;
	heap->size = i;
	heap->sorted = TRUE;
}

/*
 * _etox_style_heap_item - access the item at the ith position in the heap
 * @heap: the heap to access the internal data
 * @i: the index of the data within the heap
 *
 * Returns the data located at the ith position within @heap on success, NULL
 * on failure. The data is guaranteed to be in sorted order.
 */
inline void *_etox_style_heap_item(Etox_Style_Heap * heap, int i)
{
	if (i >= heap->size)
		return NULL;

	/*
	 * Make sure the data is sorted so we return the correct value.
	 */
	if (!heap->sorted)
		_etox_style_heap_sort(heap);

	return heap->data[i];
}

/*
 * __etox_style_heap_heapify - regain the heap properties starting at position i
 * @heap: the heap to regain heap properties
 * @i: the position to start heapifying
 *
 * Returns no value.
 */
static void __etox_style_heap_heapify(Etox_Style_Heap * heap, int i)
{
	int extreme;
	int left = LEFT(i);
	int right = RIGHT(i);

	if (heap->order == ECORE_SHEAP_MIN) {
		if (left <= heap->size
		    && heap->compare(heap->data[left - 1],
				     heap->data[i - 1]) < 0)
			extreme = left;
		else
			extreme = i;

		if (right <= heap->size
		    && heap->compare(heap->data[right - 1],
				     heap->data[extreme - 1]) < 0)
			extreme = right;
	} else {
		if (left <= heap->size
		    && heap->compare(heap->data[left - 1],
				     heap->data[i - 1]) > 0)
			extreme = left;
		else
			extreme = i;

		if (right <= heap->size
		    && heap->compare(heap->data[right - 1],
				     heap->data[extreme - 1]) > 0)
			extreme = right;
	}

	/*
	 * If the data needs to be swapped down the heap, recurse on
	 * heapifying it's new placement.
	 */
	if (extreme != i) {
		void *temp;

		temp = heap->data[extreme - 1];
		heap->data[extreme - 1] = heap->data[i - 1];
		heap->data[i - 1] = temp;

		__etox_style_heap_heapify(heap, extreme);
	}
}

static void __etox_style_heap_update_data(Etox_Style_Heap * heap)
{
	int i, old_size;
	void **data;

	/*
	 * Track the old values from the heap
	 */
	old_size = heap->size;
	data = heap->data;

	/*
	 *
	 */
	heap->size = 0;
	heap->data = malloc(heap->space * sizeof(void *));

	for (i = 0; i < old_size; i++)
		_etox_style_heap_insert(heap, data[i]);

	FREE(data);
}
