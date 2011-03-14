#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>

#include "Elixir.h"

#ifdef BUILD_MODULE_EET
# include <Eet.h>

#undef EET_DUMP

static const struct {
   const char *name;
   int         value;
} eet_const_properties[] = {
  { "EET_T_CHAR", EET_T_CHAR },
  { "EET_T_SHORT", EET_T_SHORT },
  { "EET_T_INT", EET_T_INT },
  { "EET_T_LONG_LONG", EET_T_LONG_LONG },
  { "EET_T_FLOAT", EET_T_FLOAT },
  { "EET_T_DOUBLE", EET_T_DOUBLE },
  { "EET_T_UCHAR", EET_T_UCHAR },
  { "EET_T_USHORT", EET_T_USHORT },
  { "EET_T_UINT", EET_T_UINT },
  { "EET_T_ULONG_LONG", EET_T_ULONG_LONG },
  { "EET_T_STRING", EET_T_STRING },
  { "EET_T_INLINED_STRING", EET_T_INLINED_STRING },
  { "EET_T_NULL", EET_T_NULL },
  { "EET_G_ARRAY", EET_G_ARRAY },
  { "EET_G_VAR_ARRAY", EET_G_VAR_ARRAY },
  { "EET_G_LIST", EET_G_LIST },
  { "EET_G_HASH", EET_G_HASH },
  { NULL, 0 }
};

static Eet_Node *elixir_to_eet_node(JSContext *cx, jsval data, const char *name, jsval descriptor);

static Eet_Node *
elixir_simple_to_eet_node(JSContext *cx, jsval data, const char *name, int type)
{
   char *str = NULL;
   double dbl = 0.0;
   int num = 0;

   switch (type)
     {
      case EET_T_CHAR:
      case EET_T_SHORT:
      case EET_T_INT:
      case EET_T_LONG_LONG:
      case EET_T_UCHAR:
      case EET_T_USHORT:
      case EET_T_UINT:
      case EET_T_ULONG_LONG:
	 num = elixir_get_int(cx, data);
	 break;
      case EET_T_FLOAT:
      case EET_T_DOUBLE:
	 dbl = elixir_get_dbl(cx, data);
	 break;
      case EET_T_STRING:
      case EET_T_INLINED_STRING:
	 str = elixir_get_string(cx, data);
	 break;
      case EET_T_NULL:
	 return eet_node_null_new(name);
     }

   switch (type)
     {
      case EET_T_CHAR:
	 return eet_node_char_new(name, (char) num);
      case EET_T_SHORT:
	 return eet_node_short_new(name, (short) num);
      case EET_T_INT:
	 return eet_node_int_new(name, (int) num);
      case EET_T_LONG_LONG:
	 return eet_node_long_long_new(name, (int) num);
      case EET_T_UCHAR:
	 return eet_node_unsigned_char_new(name, (unsigned char) num);
      case EET_T_USHORT:
	 return eet_node_unsigned_short_new(name, (unsigned short) num);
      case EET_T_UINT:
	 return eet_node_unsigned_int_new(name, (unsigned int) num);
      case EET_T_ULONG_LONG:
	 return eet_node_unsigned_long_long_new(name, (unsigned long long) num);

      case EET_T_FLOAT:
	 return eet_node_float_new(name, (float) dbl);
      case EET_T_DOUBLE:
	 return eet_node_double_new(name, dbl);

      case EET_T_STRING:
	 return eet_node_string_new(name, str);
      case EET_T_INLINED_STRING:
	 return eet_node_inlined_string_new(name, str);
     }

   return NULL;
}

static Eet_Node *
elixir_group_to_eet_node(JSContext *cx, jsval data, const char *name, JSObject *descriptor)
{
   Eina_List *subpart = NULL;
   Eet_Node *child;
   JSObject *obj;
   JSIdArray *ida;
   char *key;
   jsval id = JSVAL_NULL;
   jsval ov = JSVAL_NULL;
   jsval dv = JSVAL_NULL;
   jsval tv = JSVAL_NULL;
   jsval cv = JSVAL_NULL;
   unsigned int size;
   int type;
   int i;

   elixir_rval_register(cx, &data);
   elixir_rval_register(cx, &id);
   elixir_rval_register(cx, &ov);
   elixir_rval_register(cx, &dv);
   elixir_rval_register(cx, &tv);
   elixir_rval_register(cx, &cv);

   if (JS_ValueToObject(cx, data, &obj) == JS_FALSE)
     goto on_error;

   if (JS_GetArrayLength(cx, obj, &size) == JS_FALSE)
     goto on_error;

   if (size < 2)
     goto on_error;

   if (JS_GetElement(cx, descriptor, 0, &tv) == JS_FALSE)
     goto on_error;

   if (JS_GetElement(cx, descriptor, 1, &dv) == JS_FALSE)
     goto on_error;

   type = elixir_get_int(cx, tv);
   if (!(type > EET_G_UNKNOWN && type < EET_G_LAST))
     goto on_error;

   ida = JS_Enumerate(cx, obj);
   if (!ida) goto on_error;

   if (type == EET_G_ARRAY && size < 3)
     {
	if (JS_GetElement(cx, descriptor, 1, &cv) == JS_FALSE)
	  goto on_error;

	size = elixir_get_int(cx, cv);
	if ((int) size != ida->length)
	  goto on_error;
     }

   for (i = 0; i < ida->length; ++i)
     {
	JS_IdToValue(cx, ida->vector[i], &id);

	key = JS_GetStringBytes(JSVAL_TO_STRING(id));
	if (key)
	  {
	     JS_LookupProperty(cx, obj, key, &ov);

	     child = elixir_to_eet_node(cx, ov, name, dv);

	     if (type == EET_G_HASH)
	       if (child)
		 child = eet_node_hash_new(name, key, child);
	     if (child)
	       subpart = eina_list_append(subpart, child);
	  }
     }

 on_error:
   elixir_rval_delete(cx, &id);
   elixir_rval_delete(cx, &ov);
   elixir_rval_delete(cx, &dv);
   elixir_rval_delete(cx, &tv);
   elixir_rval_delete(cx, &cv);
   elixir_rval_delete(cx, &data);

   if (!subpart) return NULL;
   switch (type)
     {
      case EET_G_ARRAY:
	 return eet_node_array_new(name, size, subpart);
      case EET_G_VAR_ARRAY:
	 return eet_node_var_array_new(name, subpart);
      case EET_G_LIST:
	 return eet_node_list_new(name, subpart);
      case EET_G_HASH:
	return eet_node_struct_new(name, subpart);
      default:
	 return NULL;
     }
}

static Eet_Node *
elixir_object_to_eet_node(JSContext *cx, jsval data, const char *name, JSObject *descriptor)
{
   Eina_List *subpart = NULL;
   Eet_Node *child;
   JSObject *obj;
   JSIdArray *ida;
   char *property;
   jsval id, ov, dv;
   int i;

   if (JS_ValueToObject(cx, data, &obj) == JS_FALSE)
     return NULL;
   if (!obj)
     return NULL;

   ida = JS_Enumerate(cx, descriptor);
   if (!ida) return NULL;

   for (i = 0; i < ida->length; ++i)
     {
	JS_IdToValue(cx, ida->vector[i], &id);

	property = JS_GetStringBytes(JSVAL_TO_STRING(id));
	/* Lookup inside data, if we can find this property. */
	if (property && JS_LookupProperty(cx, obj, property, &ov) == JS_TRUE)
	  {
	     JS_LookupProperty(cx, descriptor, property, &dv);

	     child = elixir_to_eet_node(cx, ov, property, dv);
	     if (child)
	       subpart = eina_list_append(subpart, eet_node_struct_child_new(property, child));
	  }
     }

   if (!subpart) return NULL;
   return eet_node_struct_new(name, subpart);
}

static Eet_Node *
elixir_to_eet_node(JSContext *cx, jsval data, const char *name, jsval descriptor)
{
   JSObject *obj;

   if (JSVAL_IS_INT(descriptor)
       || JSVAL_IS_STRING(descriptor))
     {
	int num;

	num = elixir_get_int(cx, descriptor);

	return elixir_simple_to_eet_node(cx, data, name, num);
     }

   obj = elixir_get_obj(cx, descriptor);
   if (obj)
     {
	if (JS_IsArrayObject(cx, obj) == JS_TRUE)
	  return elixir_group_to_eet_node(cx, data, name, obj);

	return elixir_object_to_eet_node(cx, data, name, obj);
     }

   return NULL;
}
#endif

#ifdef EET_DUMP
static void elixir_dump(void *data, char *str)
{
   fprintf((FILE*)data, "%s", str);
}
#endif

EAPI Elixir_Eet_Data *
elixir_to_eet_data(JSContext *cx, jsval data, const char *name, jsval descriptor, const char *cipher)
{
#ifdef BUILD_MODULE_EET
   Elixir_Eet_Data *ed;
   Eet_Node *nd;
   void *buffer;
   int size;
#ifdef EET_DUMP
   FILE *f;
#endif

   elixir_rval_register(cx, &data);
   elixir_rval_register(cx, &descriptor);

   nd = elixir_to_eet_node(cx, data, name, descriptor);

   elixir_rval_delete(cx, &data);
   elixir_rval_delete(cx, &descriptor);

   if (!nd) return NULL;

   buffer = eet_data_node_encode_cipher(nd, cipher, &size);

   if (!buffer) return NULL;

#ifdef EET_DUMP
   f = fopen("/tmp/elixir_to_eet", "w");
   fwrite(buffer, size, 1, f);
   fclose(f);

   fprintf(stderr, "*** DUMPING EET DATA ***\n");
   eet_data_text_dump(buffer, size, elixir_dump, stderr);
   fprintf(stderr, "**** ****\n");
#endif

   ed = malloc(sizeof (Elixir_Eet_Data));
   if (!ed) return NULL;

   ed->count = size;
   ed->data = buffer;

   return ed;
#else
   return NULL;
#endif
}

#ifdef BUILD_MODULE_EET
typedef struct _Elixir_Eet_Node_Converter Elixir_Eet_Node_Converter;

struct _Elixir_Eet_Node_Converter
{
   JSContext *cx;

   Eina_List *gc;
};

static jsval *
_elixir_gc_js_new(Elixir_Eet_Node_Converter *ctx)
{
   jsval *vp;

   vp = malloc(sizeof (jsval));
   if (!vp) return NULL;

   *vp = JSVAL_NULL;
   elixir_rval_register(ctx->cx, vp);

   ctx->gc = eina_list_prepend(ctx->gc, vp);

   return vp;
}

static void
 _elixir_gc_js_delete(Elixir_Eet_Node_Converter *ctx, jsval *vp)
{
   ctx->gc = eina_list_remove(ctx->gc, vp);
   elixir_rval_delete(ctx->cx, vp);
   free(vp);
}

static void *
_elixir_eet_struct_alloc(const char *type, void *user_data)
{
   Elixir_Eet_Node_Converter *ctx = user_data;
   JSObject *obj;
   jsval *vp;

   vp = _elixir_gc_js_new(ctx);
   if (!vp) return NULL;

   obj = JS_NewObject(ctx->cx, NULL, NULL, NULL);
   if (!obj)
     {
	_elixir_gc_js_delete(ctx, vp);
	return NULL;
     }

   *vp = OBJECT_TO_JSVAL(obj);
   return vp;
}

static void
_elixir_eet_struct_add(void *parent, const char *name, void *child, void *user_data)
{
   Elixir_Eet_Node_Converter *ctx = user_data;
   jsval *inside;
   jsval *add;

   inside = parent;
   add = child;

   JS_DefineProperty(ctx->cx, JSVAL_TO_OBJECT(*inside), name, add ? *add : JSVAL_NULL, NULL, NULL,
		     JSPROP_ENUMERATE | JSPROP_READONLY);

   _elixir_gc_js_delete(ctx, child);
}

static void *
_elixir_eet_array(Eina_Bool variable, const char *name, int count, void *user_data)
{
   Elixir_Eet_Node_Converter *ctx = user_data;
   JSObject *obj;
   jsval *vp;

   vp = _elixir_gc_js_new(ctx);
   if (!vp) return NULL;

   obj = JS_NewArrayObject(ctx->cx, 0, NULL);
   if (!obj)
     {
	_elixir_gc_js_delete(ctx, vp);
	return NULL;
     }

   *vp = OBJECT_TO_JSVAL(obj);
   return vp;
}

static void
_elixir_eet_insert(void *array, int index, void *child, void *user_data)
{
   Elixir_Eet_Node_Converter *ctx = user_data;
   jsval *inside;
   jsval *add;

   inside = array;
   add = child;

   JS_SetElement(ctx->cx, JSVAL_TO_OBJECT(*inside), index, add);

   _elixir_gc_js_delete(ctx, child);
}

static void *
_elixir_eet_list(const char *name, void *user_data)
{
   Elixir_Eet_Node_Converter *ctx = user_data;
   JSObject *obj;
   jsval *vp;

   vp = _elixir_gc_js_new(ctx);
   if (!vp) return NULL;

   obj = JS_NewArrayObject(ctx->cx, 0, NULL);
   if (!obj)
     {
	_elixir_gc_js_delete(ctx, vp);
	return NULL;
     }

   *vp = OBJECT_TO_JSVAL(obj);
   return vp;
}

static void
_elixir_eet_append(void *list, void *child, void *user_data)
{
   Elixir_Eet_Node_Converter *ctx = user_data;
   jsval *inside;
   jsval *add;
   jsuint length = 0;

   inside = list;
   add = child;

   JS_GetArrayLength(ctx->cx, JSVAL_TO_OBJECT(*inside), &length);

   JS_SetElement(ctx->cx, JSVAL_TO_OBJECT(*inside), length, add);

   _elixir_gc_js_delete(ctx, child);
}

static void *_elixir_eet_hash(void *parent, const char *name, const char *key, void *value, void *user_data)
{
   Elixir_Eet_Node_Converter *ctx = user_data;
   jsval *vp = NULL;
   jsval *inside;
   jsval *child;
   jsval hash;

   inside = parent;
   child = value;

   if (!JS_GetProperty(ctx->cx, JSVAL_TO_OBJECT(*inside), name, &hash))
     return NULL;

   if (hash == JSVAL_VOID)
     {
	JSObject *obj;

	obj = JS_NewObject(ctx->cx, NULL, NULL, NULL);
	if (!obj)
	  return NULL;

	hash = OBJECT_TO_JSVAL(obj);
	elixir_rval_register(ctx->cx, &hash);

	if (!JS_DefineProperty(ctx->cx, JSVAL_TO_OBJECT(*inside), name, hash,
			       NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY))
	  goto on_error;
     }
   else
     elixir_rval_register(ctx->cx, &hash);

   if (!JS_DefineProperty(ctx->cx, JSVAL_TO_OBJECT(hash), key, *child, NULL, NULL,
			  JSPROP_ENUMERATE | JSPROP_READONLY))
     goto on_error;

   vp = _elixir_gc_js_new(ctx);
   if (!vp)
     goto on_error;

   if (!JS_GetProperty(ctx->cx, JSVAL_TO_OBJECT(*inside), key, vp))
     {
	_elixir_gc_js_delete(ctx, vp);
	goto on_error;
     }

 on_error:
   elixir_rval_delete(ctx->cx, &hash);
   return vp;
}

static void *_elixir_eet_simple(int type, Eet_Node_Data *data, void *user_data)
{
   Elixir_Eet_Node_Converter *ctx = user_data;
   jsval *vp;

   vp = _elixir_gc_js_new(ctx);
   if (!vp) return NULL;

   switch (type)
     {
      case EET_T_CHAR:
	 *vp = INT_TO_JSVAL((int)(data->value.c));
	 break;
      case EET_T_SHORT:
	 *vp = INT_TO_JSVAL((int)(data->value.s));
	 break;
      case EET_T_INT:
	 *vp = INT_TO_JSVAL(data->value.i);
	 break;
      case EET_T_LONG_LONG:
 	 *vp = DOUBLE_TO_JSVAL((double)(data->value.l));
	 break;
      case EET_T_FLOAT:
	 *vp = DOUBLE_TO_JSVAL((double)(data->value.f));
	 break;
      case EET_T_DOUBLE:
	 *vp = DOUBLE_TO_JSVAL(data->value.d);
	 break;
      case EET_T_UCHAR:
	 *vp = INT_TO_JSVAL((int)(data->value.uc));
	 break;
      case EET_T_USHORT:
	 *vp = INT_TO_JSVAL((int)(data->value.us));
	 break;
      case EET_T_UINT:
	 *vp = INT_TO_JSVAL((unsigned int)(data->value.ui));
	 break;
      case EET_T_ULONG_LONG:
 	 *vp = DOUBLE_TO_JSVAL((double)(data->value.ul));
	 break;
      case EET_T_STRING:
      case EET_T_INLINED_STRING:
	{
	   JSString *str;

	   str = elixir_dup(ctx->cx, data->value.str);
	   if (!str) goto on_error;
	   *vp = STRING_TO_JSVAL(str);

	   break;
	}
      case EET_T_NULL:
	 *vp = JSVAL_NULL;
	 break;
     }

   return vp;

 on_error:
   _elixir_gc_js_delete(ctx, vp);
   return NULL;
}

static Eet_Node_Walk Elixir_Eet_Node_Walk = {
  _elixir_eet_struct_alloc,
  _elixir_eet_struct_add,
  _elixir_eet_array,
  _elixir_eet_insert,
  _elixir_eet_list,
  _elixir_eet_append,
  _elixir_eet_hash,
  _elixir_eet_simple
};
#endif

EAPI JSObject *
elixir_from_eet_data(JSContext *cx, Elixir_Eet_Data *data, const char *cipher)
{
#ifdef BUILD_MODULE_EET
   Elixir_Eet_Node_Converter context;
   Eet_Node *root;
   JSObject *result = NULL;
   jsval *vp;

   root = eet_data_node_decode_cipher(data->data, cipher, data->count);

   if (!root) return NULL;

   context.cx = cx;
   context.gc = NULL;

   vp = eet_node_walk(NULL, NULL, root, &Elixir_Eet_Node_Walk, &context);

   if (vp)
     result = JSVAL_TO_OBJECT(*vp);

   if (!elixir_object_register(cx, &result, NULL))
     return NULL;

   while (context.gc)
     {
	vp = eina_list_data_get(context.gc);
	_elixir_gc_js_delete(&context, vp);
     }

   eet_node_del(root);

   elixir_object_unregister(cx, &result);

   return result;
#else
   return NULL;
#endif
}

EAPI void
elixir_eet_init(JSContext *cx, JSObject *parent)
{
#ifdef BUILD_MODULE_EET
   unsigned int i;
   jsval property;

   eet_init();

   for (i = 0; eet_const_properties[i].name; ++i)
     {
	property = INT_TO_JSVAL(eet_const_properties[i].value);
	JS_DefineProperty(cx, parent,
			  eet_const_properties[i].name,
			  property,
			  NULL, NULL,
			  JSPROP_ENUMERATE | JSPROP_READONLY);
     }
#endif
}

EAPI void
elixir_eet_shutdown(JSContext *cx, JSObject *parent)
{
#ifdef BUILD_MODULE_EET
   unsigned int i = 0;

   while (eet_const_properties[i].name)
     JS_DeleteProperty(cx, parent, eet_const_properties[i++].name);

   eet_shutdown();
#endif
}

EAPI JSObject *
elixir_eet_data_new(JSContext *cx, void *data, int length)
{
#ifdef BUILD_MODULE_EET
   Elixir_Eet_Data *eed;
   JSObject *jeed;

   if (length < 4) return NULL;

   eed = malloc(sizeof (Elixir_Eet_Data));
   if (!eed) return NULL;

   /* After this call do not free data. It's now under elixir_eet responsibility. */
   eed->data = data;

   eed->count = length;
   memcpy(eed->data, data, length);

   /* FIXME: Need to learn how to use GC callback to cleanup the mess. */
   jeed = elixir_build_ptr(cx, eed, elixir_class_request("eet_data", NULL));
   if (!jeed)
     {
	free(eed);
	return NULL;
     }

   return jeed;
#else
   return NULL;
#endif
}

