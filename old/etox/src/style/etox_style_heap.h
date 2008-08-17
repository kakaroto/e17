#ifndef _ETOX_STYLE_HEAP_H
#define _ETOX_STYLE_HEAP_H

#define ETOX_STYLE_HEAP_MIN 0
#define ETOX_STYLE_HEAP_MAX 1

#define HEAP_INCREMENT 4096

#define PARENT(i) (i / 2)
#define LEFT(i) (2 * i)
#define RIGHT(i) (2 * i + 1)

typedef struct _etox_style_heap Etox_Style_Heap;
#define ETOX_STYLE_HEAP(heap) ((Etox_Style_Heap *)heap)

struct _etox_style_heap {
	void **data;
	int size;
	int space;

	char order, sorted;

	Ecore_Compare_Cb compare;
};

Etox_Style_Heap *_etox_style_heap_new(Ecore_Compare_Cb compare, int size);
void _etox_style_heap_destroy(Etox_Style_Heap * heap);
int _etox_style_heap_init(Etox_Style_Heap * heap, Ecore_Compare_Cb compare,
		      int size);
int _etox_style_heap_insert(Etox_Style_Heap * heap, void *data);
void *_etox_style_heap_extract(Etox_Style_Heap * heap);
void *_etox_style_heap_extreme(Etox_Style_Heap * heap);
int _etox_style_heap_change(Etox_Style_Heap * heap, void *item, void *newval);
void _etox_style_heap_destroy(Etox_Style_Heap * heap);
void _etox_style_heap_sort(Etox_Style_Heap * heap);

inline void *_etox_style_heap_item(Etox_Style_Heap * heap, int i);

#endif
