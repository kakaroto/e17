#ifndef _EDATA_LALLOC_H_
#define _EDATA_LALLOC_H_

/**
 * @defgroup Array_Group Array
 * @{
 */
typedef void (*Edata_Array_Alloc) (void *user_data, int num);
#define EDATA_ARRAY_ALLOC(function) ((Edata_Array_Alloc)function)
typedef void (*Edata_Array_Free) (void *user_data);
#define EDATA_ARRAY_FREE(function) ((Edata_Array_Free)function)

typedef struct _Edata_Array Edata_Array;
EAPI void edata_array_free(Edata_Array *a);
EAPI Edata_Array *edata_array_new(void *data, Edata_Array_Alloc alloc_cb, Edata_Array_Free free_cb, int num_init);
EAPI void edata_array_elements_add(Edata_Array *a, int num);
EAPI void edata_array_element_add(Edata_Array *a);

/** @} */

#endif
