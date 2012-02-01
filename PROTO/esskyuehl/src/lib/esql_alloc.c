#include "esql_private.h"

static const char *mempool_type = NULL;

typedef struct _Esql_Mempool Esql_Mempool;
struct _Esql_Mempool
{
   const char *name;
   Eina_Mempool *mp;
   size_t size;
};

#define ESQL_ALLOC_FREE(TYPE, Type)                                  \
  Esql_Mempool Type##_mp = { #TYPE,  NULL, sizeof (TYPE) };              \
                                                                        \
  TYPE *                                                                \
  Type##_calloc(unsigned int num)                                       \
  {                                                                     \
     return eina_mempool_calloc(Type##_mp.mp, num * sizeof (TYPE));     \
  }                                                                     \
  void                                                                  \
  Type##_mp_free(TYPE *e)                                               \
  {                                                                     \
     eina_mempool_free(Type##_mp.mp, e);                                \
  }

ESQL_ALLOC_FREE(Esql_Res, esql_res);
ESQL_ALLOC_FREE(Esql_Row, esql_row);

static Esql_Mempool *mempool_array[] = {
  &esql_res_mp,
  &esql_row_mp
};

Eina_Mempool *
esql_mempool_new(unsigned int size)
{
   return eina_mempool_add(mempool_type, NULL, NULL, size, 64);
}

Eina_Bool
esql_mempool_init(void)
{
   const char *choice;
   unsigned int i;

   choice = getenv("EINA_MEMPOOL");
   if ((!choice) || (!choice[0]))
     choice = "chained_mempool";

   for (i = 0; i < sizeof(mempool_array) / sizeof(mempool_array[0]); ++i)
     {
     retry:
        mempool_array[i]->mp = eina_mempool_add(choice, mempool_array[i]->name, NULL, mempool_array[i]->size, 64);
        if (!mempool_array[i]->mp)
          {
             if (!strcmp(choice, "pass_through"))
               {
                  ERR("Falling back to pass through ! Previously tried '%s' mempool.", choice);
                  choice = "pass_through";
                  goto retry;
               }
             else
               {
                  ERR("Impossible to allocate mempool '%s' !", choice);
                  return EINA_FALSE;
               }
          }
     }
   mempool_type = eina_stringshare_add(choice);
   return EINA_TRUE;
}

void
esql_mempool_shutdown(void)
{
   unsigned int i;

   for (i = 0; i < sizeof (mempool_array) / sizeof (mempool_array[0]); ++i)
     {
        eina_mempool_del(mempool_array[i]->mp);
        mempool_array[i]->mp = NULL;
     }
   eina_stringshare_replace(&mempool_type, NULL);
}

