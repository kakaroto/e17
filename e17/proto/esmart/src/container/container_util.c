#include "container.h"
#include "container_private.h"

Container *
_container_fetch(Evas_Object *obj)
{
  Container *cont;
  char *type;

  type = (char *)evas_object_type_get(obj);
  if (!type) return NULL;
  if (strcmp(type, "container")) return NULL;
  cont = evas_object_smart_data_get(obj); 
  return cont;
}

