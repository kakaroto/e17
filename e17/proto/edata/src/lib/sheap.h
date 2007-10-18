#ifndef _EDATA_SHEAP_H
#define _EDATA_SHEAP_H

typedef struct _edata_heap Edata_Sheap;
# define EDATA_HEAP(heap) ((Edata_Sheap *)heap)

struct _edata_heap {
	void **data;
	int size;
	int space;

	char order, sorted;

	/* Callback for comparing node values, default is direct comparison */
	Edata_Compare_Cb compare;

	/* Callback for freeing node data, default is NULL */
	Edata_Free_Cb free_func;
};

EAPI Edata_Sheap *edata_sheap_new(Edata_Compare_Cb compare, int size);
EAPI void edata_sheap_destroy(Edata_Sheap *heap);
EAPI int
		edata_sheap_init(Edata_Sheap *heap, Edata_Compare_Cb compare, int size);
EAPI int edata_sheap_free_cb_set(Edata_Sheap *heap, Edata_Free_Cb free_func);
EAPI int edata_sheap_insert(Edata_Sheap *heap, void *data);
EAPI void *edata_sheap_extract(Edata_Sheap *heap);
EAPI void *edata_sheap_extreme(Edata_Sheap *heap);
EAPI int edata_sheap_change(Edata_Sheap *heap, void *item, void *newval);
EAPI int edata_sheap_compare_set(Edata_Sheap *heap, Edata_Compare_Cb compare);
EAPI void edata_sheap_order_set(Edata_Sheap *heap, char order);
EAPI void edata_sheap_sort(Edata_Sheap *heap);

EAPI void *edata_sheap_item(Edata_Sheap *heap, int i);

#endif /* _EDATA_SHEAP_H */
