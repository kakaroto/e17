void
evoak_object_name_set(Evoak_Object *o, const char *name)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return;
   if (o->delete_me) return;
   if (o->name)
     {
        o->evoak->object_names = evas_hash_del(o->evoak->object_names, o->name, 
o);
        free(o->name);
        o->name = NULL;
     }
   if (name)
     {
        o->name = strdup(name);
        o->evoak->object_names = evas_hash_add(o->evoak->object_names, o->name, 
o);
     }
}

const char *
evoak_object_name_get(Evoak_Object *o)
{
   if ((!o) || (o->magic != EVOAK_OBJECT_MAGIC)) return NULL;
   if (o->delete_me) return NULL;
   return o->name;
}

Evoak_Object *
evoak_object_name_find(Evoak *ev, const char *name)
{
   if ((!ev) || (ev->magic != EVOAK_MAGIC)) return NULL;
   if (!name) return NULL;
   return evas_hash_find(ev->object_names, name);
}

