#include "Evoak.h"
#include "evoak_private.h"

static int
_evoak_hash_keygen(Evoak_Hash *hash, int key)
{
   return (key & (64 - 1));
}

static int
_evoak_hash_store(Evoak_Hash *hash, int id, void *data)
{
   Evoak_Hash_Node *hn;
   int h;

   hn = malloc(sizeof(Evoak_Hash_Node));
   if (!hn) return 0;
   h = _evoak_hash_keygen(hash, id);
   hn->id = id;
   hn->data = data;
   hash->buckets[h] = evas_list_prepend(hash->buckets[h], hn);
   return 1;
}

static void *
_evoak_hash_unstore(Evoak_Hash *hash, int id)
{
   Evoak_Hash_Node *hn;
   Evas_List *l;
   int h;

   h = _evoak_hash_keygen(hash, id);
   for (l = hash->buckets[h]; l; l = l->next)
     {
        hn = l->data;
        if (hn->id == id)
          {
             void *data;

             hash->buckets[h] = evas_list_remove_list(hash->buckets[h], l);
             data = hn->data;
             free(hn);
             return data;
          }
     }
   return NULL;
}

static void *
_evoak_hash_find(Evoak_Hash *hash, int id)
{
   Evoak_Hash_Node *hn;
   Evas_List *l;
   int h;

   h = _evoak_hash_keygen(hash, id);
   for (l = hash->buckets[h]; l; l = l->next)
     {
        hn = l->data;
        if (hn->id == id)
          {
             hash->buckets[h] = evas_list_remove_list(hash->buckets[h], l);
             hash->buckets[h] = evas_list_prepend(hash->buckets[h], hn);
             return hn->data;
          }
     }
   return NULL;
} 
