#include "Evoak.h"
#include "evoak_private.h"

void
evoak_object_data_set(Evoak_Object *o, const char *key, const void *data)
{
   Evoak_Data *ed;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if ((!key) || (!data)) return;
   evoak_object_data_del(o, key);
   ed = malloc(sizeof(Evoak_Data));
   if (!ed) return;
   ed->key = strdup(key);
   ed->data = (void *)data;
   o->data = evas_list_prepend(o->data, ed);
}

void *
evoak_object_data_get(Evoak_Object *o, const char *key)
{
   Evas_List *l;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   if (!key) return NULL;
   for (l = o->data; l; l = l->next)
     {
        Evoak_Data *ed;

        ed = l->data;
        if (!strcmp(ed->key, key))
          {
             if (l->prev)
               {
                  o->data = evas_list_remove_list(o->data, l);
                  o->data = evas_list_prepend(o->data, ed);
               }
             return ed->data;
          }
     }
   return NULL;
}

void *
evoak_object_data_del(Evoak_Object *o, const char *key)
{
   Evas_List *l;

   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   if (!key) return NULL;
   for (l = o->data; l; l = l->next)
     {
        Evoak_Data *ed;

        ed = l->data;
        if (!strcmp(ed->key, key))
          {
             void *data;

             o->data = evas_list_remove_list(o->data, l);
             data = ed->data;
             free(ed->key);
             free(ed);
             return data;
          }
     }
   return NULL;
}
