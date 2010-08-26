#ifdef EM_TYPEDEFS

# define EM_OBJECT_MAGIC 0xe0b9ec75
# define EM_OBJECT_MAGIC_FREED 0xe0bf6eed
# define EM_OBJECT(x) ((Em_Object *)(x))
# define EM_OBJECT_CLEANUP_FUNC(x) ((Em_Object_Cleanup_Func)(x))
# define EM_OBJECT_ALLOC(x, type, cleanup_func) \
   em_object_alloc(sizeof(x), (type), EM_OBJECT_CLEANUP_FUNC(cleanup_func))

# ifdef OBJECT_CHECK
#  define EM_OBJECT_CHECK(x) \
   do {if ((!EM_OBJECT(x)) || (EM_OBJECT(x)->magic != EM_OBJECT_MAGIC)) return;} while (0)
#  define EM_OBJECT_CHECK_RETURN(x, ret) \
   do {if ((!EM_OBJECT(x)) || (EM_OBJECT(x)->magic != EM_OBJECT_MAGIC)) return ret;} while (0)
#  define EM_OBJECT_TYPE_CHECK(x, tp) \
   do {if ((EM_OBJECT(x)->type) != (tp)) { fprintf(stderr, "Object type check failed in %s\n", __FUNCTION__); return;} } while (0)
#  define EM_OBJECT_TYPE_CHECK_RETURN(x, tp, ret) \
   do {if ((EM_OBJECT(x)->type) != (tp)) { fprintf(stderr, "Object type check failed in %s\n", __FUNCTION__); return ret;} } while (0)
#  define EM_OBJECT_IF_NOT_TYPE(x, type) \
   if (EM_OBJECT(x)->type != (type))
# else
#  define EM_OBJECT_CHECK(x)
#  define EM_OBJECT_CHECK_RETURN(x, ret)
#  define EM_OBJECT_TYPE_CHECK(x, type)
#  define EM_OBJECT_TYPE_CHECK_RETURN(x, type, ret)
#  define EM_OBJECT_IF_NOT_TYPE(x, type)
# endif

typedef void (*Em_Object_Cleanup_Func) (void *obj);
typedef struct _Em_Object Em_Object;

#else
# ifndef EM_OBJECT_H
#  define EM_OBJECT_H

struct _Em_Object
{
   int magic, type, references;
   Em_Object_Cleanup_Func del_func, cleanup_func;
   void (*free_att_func) (void *obj);
   void (*del_att_func) (void *obj);
   Eina_Bool deleted : 1;
};

EM_INTERN int   em_object_init();
EM_INTERN int   em_object_shutdown();
EM_INTERN void *em_object_alloc(int size, int type, Em_Object_Cleanup_Func cleanup_func);
EM_INTERN void  em_object_del(Em_Object *o);
EM_INTERN int   em_object_is_del(Em_Object *o);
EM_INTERN void  em_object_del_func_set(Em_Object *o, Em_Object_Cleanup_Func del_func);
EM_INTERN void  em_object_type_set(Em_Object *o, int type);
EM_INTERN void  em_object_free(Em_Object *o);
EM_INTERN int   em_object_ref(Em_Object *o);
EM_INTERN int   em_object_unref(Em_Object *o);
EM_INTERN int   em_object_ref_get(Em_Object *o);
EM_INTERN int   em_object_error(Em_Object *o);
EM_INTERN void  em_object_free_attach_func_set(Em_Object *o, void (*func) (void *obj));
EM_INTERN void  em_object_del_attach_func_set(Em_Object *o, void (*func) (void *obj));

# endif
#endif
