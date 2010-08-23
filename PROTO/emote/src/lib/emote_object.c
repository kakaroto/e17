#include "emote_private.h"

/* public functions */
EM_INTERN void *
em_object_alloc(int size, int type, Em_Object_Cleanup_Func cleanup_func)
{
   Em_Object *obj = NULL;

   if (!(obj = calloc(1, size))) return NULL;
   obj->magic = EM_OBJECT_MAGIC;
   obj->type = type;
   obj->references = 1;
   obj->cleanup_func = cleanup_func;
   return obj;
}

EM_INTERN void 
em_object_del(Em_Object *obj)
{
   EM_OBJECT_CHECK(obj);
   if (obj->deleted) return;
   if (obj->del_att_func) obj->del_att_func(obj);
   if (obj->del_func) obj->del_func(obj);
   obj->deleted = 1;
   em_object_unref(obj);
}

EM_INTERN int 
em_object_is_del(Em_Object *obj)
{
   EM_OBJECT_CHECK_RETURN(obj, 1);
   return obj->deleted;
}

EM_INTERN void 
em_object_del_func_set(Em_Object *obj, Em_Object_Cleanup_Func del_func)
{
   EM_OBJECT_CHECK(obj);
   obj->del_func = del_func;
}

EM_INTERN void 
em_object_type_set(Em_Object *obj, int type)
{
   EM_OBJECT_CHECK(obj);
   obj->type = type;
}

EM_INTERN void 
em_object_free(Em_Object *obj)
{
   EM_OBJECT_CHECK(obj);
   if (obj->free_att_func) obj->free_att_func(obj);
   obj->cleanup_func(obj);
}

EM_INTERN int 
em_object_ref(Em_Object *obj)
{
   EM_OBJECT_CHECK_RETURN(obj, -1);
   obj->references++;
   return obj->references;
}

EM_INTERN int 
em_object_unref(Em_Object *obj)
{
   int ref = -1;

   EM_OBJECT_CHECK_RETURN(obj, ref);
   obj->references--;
   ref = obj->references;
   if (obj->references == 0) em_object_free(obj);
   return ref;
}

EM_INTERN int 
em_object_ref_get(Em_Object *obj)
{
   EM_OBJECT_CHECK_RETURN(obj, 0);
   return obj->references;
}

EM_INTERN int 
em_object_error(Em_Object *obj)
{
   if (!obj) return 1;
   return 0;
}

EM_INTERN void 
em_object_data_set(Em_Object *obj, void *data)
{
   EM_OBJECT_CHECK(obj);
   obj->data = data;
}

EM_INTERN void *
em_object_data_get(Em_Object *obj)
{
   EM_OBJECT_CHECK(obj);
   return obj->data;
}

EM_INTERN void 
em_object_free_attach_func_set(Em_Object *obj, void (*func) (void *obj))
{
   EM_OBJECT_CHECK(obj);
   obj->free_att_func = func;
}

EM_INTERN void 
em_object_del_attach_func_set(Em_Object *obj, void (*func) (void *obj))
{
   EM_OBJECT_CHECK(obj);
   obj->del_att_func = func;
}
