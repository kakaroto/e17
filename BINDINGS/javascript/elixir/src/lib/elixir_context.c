#include <eina_hash.h>

#include "Elixir.h"

static Eina_Hash *context = NULL;

EAPI void
elixir_context_add(const char *key, const void *data)
{
   if (!context) context = eina_hash_string_superfast_new(NULL);

   eina_hash_add(context, key, data);
}

EAPI void*
elixir_context_find(const char *key)
{
   return eina_hash_find(context, key);
}

EAPI void
elixir_context_delete(const char *key)
{
   eina_hash_del(context, key, NULL);

   if (eina_hash_population(context) == 0)
     {
	eina_hash_free(context);
	context = NULL;
     }
}

