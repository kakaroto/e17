#ifndef _EWD_SHEAP_H
#define _EWD_SHEAP_H

#define EWD_SHEAP_MIN 0
#define EWD_SHEAP_MAX 1

#define HEAP_INCREMENT 4096

#define PARENT(i) (i / 2)
#define LEFT(i) (2 * i)
#define RIGHT(i) (2 * i + 1)

typedef struct _ewd_heap Ewd_Sheap;
#define EWD_HEAP(heap) ((Ewd_Sheap *)heap)

struct _ewd_heap {
	void **data;
	int size;
	int space;

	char order, sorted;

	Ewd_Compare_Cb compare;
};

Ewd_Sheap *ewd_sheap_new(Ewd_Compare_Cb compare, int size);
void ewd_sheap_destroy(Ewd_Sheap *heap);
int ewd_sheap_init(Ewd_Sheap *heap, Ewd_Compare_Cb compare, int size);
int ewd_sheap_insert(Ewd_Sheap *heap, void *data);
void *ewd_sheap_extract(Ewd_Sheap *heap);
void *ewd_sheap_extreme(Ewd_Sheap *heap);
int ewd_sheap_change(Ewd_Sheap *heap, void *item, void *newval);
void ewd_sheap_destroy(Ewd_Sheap *heap);
void ewd_sheap_sort(Ewd_Sheap *heap);

inline void *ewd_sheap_item(Ewd_Sheap *heap, int i);

#endif
