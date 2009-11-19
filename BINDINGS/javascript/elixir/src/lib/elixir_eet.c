
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
   if (obj == NULL)
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

EAPI JSObject *
elixir_from_eet_data(JSContext *cx, Elixir_Eet_Data *data, const char *cipher)
{
#ifdef BUILD_MODULE_EET
   /* FIXME: Need help from Eet to get an Eet_Node * from a eet_data. */
   return NULL;
#else
   return NULL;
#endif
}

EAPI void
elixir_eet_init(JSContext *cx, JSObject *parent)
{
   unsigned int i;
   jsval property;

   eet_init();

   for (i = 0; eet_const_properties[i].name != NULL; ++i)
     {
	property = INT_TO_JSVAL(eet_const_properties[i].value);
	JS_DefineProperty(cx, parent,
			  eet_const_properties[i].name,
			  property,
			  NULL, NULL,
			  JSPROP_ENUMERATE | JSPROP_READONLY);
     }
}

EAPI void
elixir_eet_shutdown(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (eet_const_properties[i].name != NULL)
     JS_DeleteProperty(cx, parent, eet_const_properties[i++].name);

   eet_shutdown();
}

EAPI JSObject *
elixir_eet_data_new(JSContext *cx, void *data, int length)
{
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
}

