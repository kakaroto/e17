#ifndef _ESTYLE_HEAP_H
#define _ESTYLE_HEAP_H

#define ESTYLE_HEAP_MIN 0
#define ESTYLE_HEAP_MAX 1

#define HEAP_INCREMENT 4096

#define PARENT(i) (i / 2)
#define LEFT(i) (2 * i)
#define RIGHT(i) (2 * i + 1)

typedef struct _estyle_heap Estyle_Heap;
#define ESTYLE_HEAP(heap) ((Estyle_Heap *)heap)

struct _estyle_heap {
	void **data;
	int size;
	int space;

	char order, sorted;

	Ewd_Compare_Cb compare;
};

Estyle_Heap *_estyle_heap_new(Ewd_Compare_Cb compare, int size);
void _estyle_heap_destroy(Estyle_Heap *heap);
int _estyle_heap_init(Estyle_Heap *heap, Ewd_Compare_Cb compare, int size);
int _estyle_heap_insert(Estyle_Heap *heap, void *data);
void *_estyle_heap_extract(Estyle_Heap *heap);
void *_estyle_heap_extreme(Estyle_Heap *heap);
int _estyle_heap_change(Estyle_Heap *heap, void *item, void *newval);
void _estyle_heap_destroy(Estyle_Heap *heap);
void _estyle_heap_sort(Estyle_Heap *heap);

inline void *_estyle_heap_item(Estyle_Heap *heap, int i);

#endif
