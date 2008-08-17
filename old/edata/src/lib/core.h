#ifndef _EDATA_CORE_H
#define _EDATA_CORE_H

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

EAPI extern const unsigned int edata_prime_table[];

#define EDATA_SORT_MIN 0
#define EDATA_SORT_MAX 1

typedef void (*Edata_For_Each) (void *value, void *user_data);
#define EDATA_FOR_EACH(function) ((Edata_For_Each)function)

typedef void (*Edata_Free_Cb) (void *data);
#define EDATA_FREE_CB(func) ((Edata_Free_Cb)func)

typedef unsigned int (*Edata_Hash_Cb) (const void *key);
#define EDATA_HASH_CB(function) ((Edata_Hash_Cb)function)

typedef int (*Edata_Compare_Cb) (const void *data1, const void *data2);
#define EDATA_COMPARE_CB(function) ((Edata_Compare_Cb)function)

EAPI unsigned int edata_direct_hash(const void *key);
EAPI unsigned int edata_str_hash(const void *key);
EAPI int edata_direct_compare(const void *key1, const void *key2);
EAPI int edata_str_compare(const void *key1, const void *key2);

#endif /* _EDATA_CORE_H */
