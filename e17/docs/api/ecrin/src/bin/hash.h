#ifndef __HASH_H__
#define __HASH_H__



#include <Ecore.h>


/*
 * Hash data
 */

typedef enum
{
  HASH_DATA_ENUM,
  HASH_DATA_DEFINE,
  HASH_DATA_FUNCTION
}Hash_Data_Type;

typedef struct _Ecrin_Hash_Data Ecrin_Hash_Data;

struct _Ecrin_Hash_Data
{
  char          *efl_name;
  char          *data_name; /* the key for the hash table */
  void          *data;
  Hash_Data_Type type;
};

int              ecrin_hash_init     (void);

void             ecrin_hash_shutdown (void);

Ecrin_Hash_Data *ecrin_hash_data_new (char          *efl_name,
                                      char          *data_name,
                                      Hash_Data_Type type,
                                      void          *data);

void             ecrin_hash_data_add (Ecrin_Hash_Data *data);

Ecore_List *     ecrin_hash_keys_get (void);

Ecrin_Hash_Data *ecrin_hash_data_get (char *key);



#endif /* __HASH_H__ */
