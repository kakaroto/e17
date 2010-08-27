#ifdef EM_TYPEDEFS

# define EMOTE_OBJECT_MAGIC 0xe0b9ec75
# define EMOTE_OBJECT_MAGIC_FREED 0xe0bf6eed
# define EMOTE_OBJECT(x) ((Emote_Object *)(x))
# define EMOTE_OBJECT_CLEANUP_FUNC(x) ((Emote_Object_Cleanup_Func)(x))
# define EMOTE_OBJECT_ALLOC(x, type, cleanup_func) \
   emote_object_alloc(sizeof(x), (type), EMOTE_OBJECT_CLEANUP_FUNC(cleanup_func))

# ifdef OBJECT_CHECK
#  define EMOTE_OBJECT_CHECK(x) \
   do {if ((!EMOTE_OBJECT(x)) || (EMOTE_OBJECT(x)->magic != EMOTE_OBJECT_MAGIC)) return;} while (0)
#  define EMOTE_OBJECT_CHECK_RETURN(x, ret) \
   do {if ((!EMOTE_OBJECT(x)) || (EMOTE_OBJECT(x)->magic != EMOTE_OBJECT_MAGIC)) return ret;} while (0)
#  define EMOTE_OBJECT_TYPE_CHECK(x, tp) \
   do {if ((EMOTE_OBJECT(x)->type) != (tp)) { fprintf(stderr, "Object type check failed in %s\n", __FUNCTION__); return;} } while (0)
#  define EMOTE_OBJECT_TYPE_CHECK_RETURN(x, tp, ret) \
   do {if ((EMOTE_OBJECT(x)->type) != (tp)) { fprintf(stderr, "Object type check failed in %s\n", __FUNCTION__); return ret;} } while (0)
#  define EMOTE_OBJECT_IF_NOT_TYPE(x, type) \
   if (EMOTE_OBJECT(x)->type != (type))
# else
#  define EMOTE_OBJECT_CHECK(x)
#  define EMOTE_OBJECT_CHECK_RETURN(x, ret)
#  define EMOTE_OBJECT_TYPE_CHECK(x, type)
#  define EMOTE_OBJECT_TYPE_CHECK_RETURN(x, type, ret)
#  define EMOTE_OBJECT_IF_NOT_TYPE(x, type)
# endif

#else
# ifndef EMOTE_OBJECT_H
#  define EMOTE_OBJECT_H

EM_INTERN void *emote_object_alloc(int size, int type, Emote_Object_Cleanup_Func cleanup_func);
EM_INTERN void  emote_object_del(Emote_Object *obj);
EM_INTERN int   emote_object_is_del(Emote_Object *obj);
EM_INTERN void  emote_object_del_func_set(Emote_Object *obj, Emote_Object_Cleanup_Func del_func);
EM_INTERN void  emote_object_type_set(Emote_Object *obj, int type);
EM_INTERN void  emote_object_free(Emote_Object *obj);
EM_INTERN int   emote_object_ref(Emote_Object *obj);
EM_INTERN int   emote_object_unref(Emote_Object *obj);
EM_INTERN int   emote_object_ref_get(Emote_Object *obj);
EM_INTERN int   emote_object_error(Emote_Object *obj);
EM_INTERN void  emote_object_data_set(Emote_Object *obj, void *data);
EM_INTERN void *emote_object_data_get(Emote_Object *obj);
EM_INTERN void  emote_object_free_attach_func_set(Emote_Object *obj, void (*func) (void *obj));
EM_INTERN void  emote_object_del_attach_func_set(Emote_Object *obj, void (*func) (void *obj));

# endif
#endif
