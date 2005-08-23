#include <stdlib.h>

#include "hash.h"



static Ecore_Hash *hash = NULL;

int
ecrin_hash_init (void)
{
  hash = ecore_hash_new (NULL, NULL);
  if (!hash)
    {
      printf ("Could not allocate hash. Exiting...\n");
      return 0;
    }

  return 1;
}

void
ecrin_hash_shutdown (void)
{
  ecore_hash_destroy (hash);
}

Ecrin_Hash_Data *
ecrin_hash_data_new (char          *efl_name,
                     char          *data_name,
                     Hash_Data_Type type,
                     void          *data)
{
  Ecrin_Hash_Data *hash_data;

  hash_data = (Ecrin_Hash_Data *)malloc (sizeof (Ecrin_Hash_Data));
  if (!hash_data)
    return NULL;

  hash_data->efl_name = efl_name;
  hash_data->data_name = data_name;
  hash_data->type = type;
  hash_data->data = data;
  
  return hash_data;
}

void
ecrin_hash_data_add (Ecrin_Hash_Data *data)
{
  if (!data)
    return;

  ecore_hash_set (hash, data->data_name, data);
}

Ecore_List *
ecrin_hash_keys_get (void)
{
  return ecore_hash_keys (hash);
}

Ecrin_Hash_Data *
ecrin_hash_data_get (char *key)
{
  return (Ecrin_Hash_Data *)ecore_hash_get (hash, key);
}
