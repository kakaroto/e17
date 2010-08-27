#include "emote_private.h"

/* public functions */
EM_INTERN void *
emote_object_alloc(int size, int type, Emote_Object_Cleanup_Func cleanup_func)
{
   Emote_Object *obj = NULL;

   if (!(obj = calloc(1, size))) return NULL;
   obj->deleted = EINA_FALSE;
   obj->magic = EMOTE_OBJECT_MAGIC;
   obj->type = type;
   obj->references = 1;
   obj->cleanup_func = cleanup_func;
   return obj;
}

EM_INTERN void
emote_object_del(Emote_Object *obj)
{
   EMOTE_OBJECT_CHECK(obj);

   if (obj->deleted) return;
   if (obj->del_att_func) obj->del_att_func(obj);
   if (obj->del_func) obj->del_func(obj);
   obj->deleted = 1;
   emote_object_unref(obj);
}

EM_INTERN int
emote_object_is_del(Emote_Object *obj)
{
   EMOTE_OBJECT_CHECK_RETURN(obj, 1);
   return obj->deleted;
}

EM_INTERN void
emote_object_del_func_set(Emote_Object *obj, Emote_Object_Cleanup_Func del_func)
{
   EMOTE_OBJECT_CHECK(obj);
   obj->del_func = del_func;
}

EM_INTERN void
emote_object_type_set(Emote_Object *obj, int type)
{
   EMOTE_OBJECT_CHECK(obj);
   obj->type = type;
}

EM_INTERN void
emote_object_free(Emote_Object *obj)
{
   EMOTE_OBJECT_CHECK(obj);
   if (obj->free_att_func) obj->free_att_func(obj);
   obj->cleanup_func(obj);
}

EM_INTERN int
emote_object_ref(Emote_Object *obj)
{
   EMOTE_OBJECT_CHECK_RETURN(obj, -1);
   obj->references++;
   return obj->references;
}

EM_INTERN int
emote_object_unref(Emote_Object *obj)
{
   int ref = -1;

   EMOTE_OBJECT_CHECK_RETURN(obj, ref);
   obj->references--;
   ref = obj->references;
   if (obj->references == 0) emote_object_free(obj);
   return ref;
}

EM_INTERN int
emote_object_ref_get(Emote_Object *obj)
{
   EMOTE_OBJECT_CHECK_RETURN(obj, 0);
   return obj->references;
}

EM_INTERN int
emote_object_error(Emote_Object *obj)
{
   if (!obj) return 1;
   return 0;
}

EM_INTERN void
emote_object_data_set(Emote_Object *obj, void *data)
{
   EMOTE_OBJECT_CHECK(obj);
   obj->data = data;
}

EM_INTERN void *
emote_object_data_get(Emote_Object *obj)
{
   EMOTE_OBJECT_CHECK(obj);
   return obj->data;
}

EM_INTERN void
emote_object_free_attach_func_set(Emote_Object *obj, void (*func) (void *obj))
{
   EMOTE_OBJECT_CHECK(obj);
   obj->free_att_func = func;
}

EM_INTERN void
emote_object_del_attach_func_set(Emote_Object *obj, void (*func) (void *obj))
{
   EMOTE_OBJECT_CHECK(obj);
   obj->del_att_func = func;
}
