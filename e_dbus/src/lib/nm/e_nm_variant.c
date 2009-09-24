#include <string.h>

#include "E_Nm.h"
#include "e_dbus_private.h"
EAPI E_NM_Variant *
e_nm_variant_new(int type, const void *value)
{
  E_NM_Variant *variant = NULL;

  switch (type)
  {
    case 's':
    case 'o':
      variant = malloc(sizeof(E_NM_Variant));
      variant->type = type;
      variant->s = strdup(*(char **)value);
      break;
    case 'u':
      variant = malloc(sizeof(E_NM_Variant));
      variant->type = type;
      variant->u = *(unsigned int *)value;
      break;
    case 'b':
      variant = malloc(sizeof(E_NM_Variant));
      variant->type = type;
      variant->b = *(int *)value;
      break;
    case 'y':
      variant = malloc(sizeof(E_NM_Variant));
      variant->type = type;
      variant->y = *(unsigned char *)value;
      break;
    case 't':
      variant = malloc(sizeof(E_NM_Variant));
      variant->type = type;
      variant->t = *(unsigned long long *)value;
      break;
  }
  return variant;
}

EAPI E_NM_Variant *
e_nm_variant_array_new(int type, const void *value, int size)
{
  E_NM_Variant *variant = NULL;
  int           i;

  variant = malloc(sizeof(E_NM_Variant));
  variant->type = 'a';
  variant->a = NULL;
  for (i = 0; i < size; i++)
    variant->a = eina_list_append(variant->a, e_nm_variant_new(type, &(value[i])));
  return variant;
}

EAPI void
e_nm_variant_free(E_NM_Variant *variant)
{
  if (variant->type == 'a')
  {
    void *data;

    EINA_LIST_FREE(variant->a, data)
      e_nm_variant_free(data);
  }
  else if ((variant->type == 's') || (variant->type == 'o')) free(variant->s);
  free(variant);
}

