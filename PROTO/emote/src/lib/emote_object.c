#include "emote_private.h"

# define EM_OBJECT_T(x) (_em_object_get_alloc(x))

static Eina_Hash *_emote_object_allocs = NULL;
static Em_Object *_em_object_get_alloc(Em_Object *o);

EM_INTERN int
em_object_init()
{
   _emote_object_allocs = eina_hash_pointer_new(NULL);

   return 1;
}

/* public functions */
EM_INTERN void *
em_object_alloc(int size, int type, Em_Object_Cleanup_Func cleanup_func)
{
   Em_Object *obj = NULL;
   void *o = NULL;

   // Allocate buffer big enough for size given as well as Em_Object
   if (!(o = calloc(1, (sizeof(Em_Object)+size)))) return NULL;

   // Use end of allocated memory to store Em_Object
   obj = (Em_Object*)(o + size);
   obj->deleted = EINA_FALSE;
   obj->magic = EM_OBJECT_MAGIC;
   obj->type = type;
   obj->references = 1;
   obj->cleanup_func = cleanup_func;

   eina_hash_add(_emote_object_allocs, &o, obj);

   return o;
}

EM_INTERN void
em_object_del(Em_Object *o)
{
   Em_Object *obj;
   obj = EM_OBJECT_T(o);

   EM_OBJECT_CHECK(obj);

   if (obj->deleted) return;
   if (obj->del_att_func) obj->del_att_func(o);
   if (obj->del_func) obj->del_func(o);
   obj->deleted = 1;
   em_object_unref(o);
}

EM_INTERN int
em_object_is_del(Em_Object *o)
{
   Em_Object *obj;
   obj = EM_OBJECT_T(o);

   EM_OBJECT_CHECK_RETURN(obj, 1);
   return obj->deleted;
}

EM_INTERN void
em_object_del_func_set(Em_Object *o, Em_Object_Cleanup_Func del_func)
{
   Em_Object *obj;
   obj = EM_OBJECT_T(o);

   EM_OBJECT_CHECK(obj);
   obj->del_func = del_func;
}

EM_INTERN void
em_object_type_set(Em_Object *o, int type)
{
   Em_Object *obj;
   obj = EM_OBJECT_T(o);

   EM_OBJECT_CHECK(obj);
   obj->type = type;
}

EM_INTERN void
em_object_free(Em_Object *o)
{
   Em_Object *obj;
   obj = EM_OBJECT_T(o);

   EM_OBJECT_CHECK(obj);
   if (obj->free_att_func) obj->free_att_func(o);
   obj->cleanup_func(o);
   eina_hash_del(_emote_object_allocs, &o, NULL);
}

EM_INTERN int
em_object_ref(Em_Object *o)
{
   Em_Object *obj;
   obj = EM_OBJECT_T(o);

   EM_OBJECT_CHECK_RETURN(obj, -1);
   obj->references++;
   return obj->references;
}

EM_INTERN int
em_object_unref(Em_Object *o)
{
   int ref = -1;
   Em_Object *obj;
   obj = EM_OBJECT_T(o);

   EM_OBJECT_CHECK_RETURN(obj, ref);
   obj->references--;
   ref = obj->references;
   if (obj->references == 0) em_object_free(o);
   return ref;
}

EM_INTERN int
em_object_ref_get(Em_Object *o)
{
   Em_Object *obj;
   obj = EM_OBJECT_T(o);

   EM_OBJECT_CHECK_RETURN(obj, 0);
   return obj->references;
}

EM_INTERN int
em_object_error(Em_Object *o)
{
   Em_Object *obj;
   obj = EM_OBJECT_T(o);

   if (!obj) return 1;
   return 0;
}

EM_INTERN void
em_object_free_attach_func_set(Em_Object *o, void (*func) (void *obj))
{
   Em_Object *obj;
   obj = EM_OBJECT_T(o);

   EM_OBJECT_CHECK(obj);
   obj->free_att_func = func;
}

EM_INTERN void
em_object_del_attach_func_set(Em_Object *o, void (*func) (void *obj))
{
   Em_Object *obj;
   obj = EM_OBJECT_T(o);

   EM_OBJECT_CHECK(obj);
   obj->del_att_func = func;
}

static Em_Object *
_em_object_get_alloc(Em_Object *o)
{
   Em_Object *obj;

   if (_emote_object_allocs == NULL) return NULL;

   obj = (Em_Object*)eina_hash_find(_emote_object_allocs, &o);

   if (obj == NULL)
      printf("ERROR: METADATA LOOKUP FAILED for %p!\n", o);

   return obj;
}
