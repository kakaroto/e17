#include "Elixir.h"

JSString*
elixir_dup(JSContext* cx, const char* str)
{
   if (!str)
     return NULL;

   return JS_NewStringCopyZ(cx, str);
}

JSString*
elixir_ndup(JSContext* cx, const char* str, int length)
{
   if (!str)
     return NULL;

   return JS_NewStringCopyN(cx, str, length);
}

const char*
elixir_get_string_bytes(JSString *str, size_t *length)
{
   if (!str) {
      if (length) *length = 0;
      return NULL;
   }

   if (length) *length = JS_GetStringLength(str);
   return JS_GetStringBytes(str);
}

Eina_Bool
elixir_add_str_prop(JSContext *cx, JSObject *obj,
                    const char *name, const char *value)
{
   JSString *str;
   Eina_Bool ret = EINA_TRUE;

   str = JS_NewStringCopyZ(cx, value);
   if (!elixir_string_register(cx, &str))
     return EINA_FALSE;

   if (!JS_DefineProperty(cx, obj, name,
			  STRING_TO_JSVAL(str),
			  NULL, NULL,
			  JSPROP_ENUMERATE |
			  JSPROP_READONLY))
     ret = EINA_FALSE;

   elixir_string_unregister(cx, &str);
   return ret;
}

Eina_Bool
elixir_add_int_prop(JSContext *cx, JSObject *obj,
                    const char *name, int value)
{
   if (!JS_DefineProperty(cx, obj, name,
			  INT_TO_JSVAL(value),
			  NULL, NULL,
			  JSPROP_ENUMERATE |
			  JSPROP_READONLY))
     return EINA_FALSE;
   return EINA_TRUE;
}

Eina_Bool
elixir_add_bool_prop(JSContext *cx, JSObject *obj,
                     const char *name, int value)
{
   if (!JS_DefineProperty(cx, obj, name,
			  BOOLEAN_TO_JSVAL(value),
			  NULL, NULL,
			  JSPROP_ENUMERATE |
			  JSPROP_READONLY))
     return EINA_FALSE;
   return EINA_TRUE;
}

Eina_Bool
elixir_add_object_prop(JSContext *cx, JSObject *obj,
                       const char *name, JSObject *value)
{
   if (!JS_DefineProperty(cx, obj, name,
			  OBJECT_TO_JSVAL(value),
			  NULL, NULL,
			  JSPROP_ENUMERATE |
			  JSPROP_READONLY))
     return EINA_FALSE;
   return EINA_TRUE;
}

Eina_Bool
elixir_add_dbl_prop(JSContext *cx, JSObject *obj,
                    const char *name, double value)
{
   Eina_Bool ret = EINA_FALSE;
   jsval tmp = JSVAL_NULL;

   elixir_rval_register(cx, &tmp);

   if (!JS_NewNumberValue(cx, value, &tmp))
     goto on_error;

   if (!JS_DefineProperty(cx, obj, name,
			  tmp,
			  NULL, NULL,
			  JSPROP_ENUMERATE |
			  JSPROP_READONLY))
     goto on_error;

   ret = EINA_TRUE;

 on_error:
   elixir_rval_delete(cx, &tmp);
   return ret;
}

JSObject*
elixir_new_color(JSContext *cx, int r, int g, int b, int a)
{
   JSObject *obj;

   obj = JS_NewObject(cx, elixir_class_request("color", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj, NULL))
     return NULL;

   elixir_add_int_prop(cx, obj, "r", r);
   elixir_add_int_prop(cx, obj, "g", g);
   elixir_add_int_prop(cx, obj, "b", b);
   elixir_add_int_prop(cx, obj, "a", a);

   elixir_object_unregister(cx, &obj);

   return obj;
}

Eina_Bool
elixir_extract_color(JSContext *cx, JSObject *obj, int *r, int *g, int *b, int *a)
{
   GET_ICOMPONENT(cx, obj, r);
   GET_ICOMPONENT(cx, obj, g);
   GET_ICOMPONENT(cx, obj, b);
   GET_ICOMPONENT(cx, obj, a);

   return EINA_TRUE;
}

JSObject*
elixir_new_size(JSContext *cx, int w, int h)
{
   JSObject *obj;

   obj = JS_NewObject(cx, elixir_class_request("size", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj, NULL))
     return NULL;

   elixir_add_int_prop(cx, obj, "w", w);
   elixir_add_int_prop(cx, obj, "h", h);

   elixir_object_unregister(cx, &obj);

   return obj;
}

Eina_Bool
elixir_extract_size(JSContext *cx, JSObject *obj, int *w, int *h)
{
   GET_ICOMPONENT(cx, obj, w);
   GET_ICOMPONENT(cx, obj, h);

   return EINA_TRUE;
}

JSObject*
elixir_new_geometry(JSContext *cx, int x, int y, int w, int h)
{
   JSObject *obj;

   obj = JS_NewObject(cx, elixir_class_request("geometry", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj, NULL))
     return NULL;

   elixir_add_int_prop(cx, obj, "x", x);
   elixir_add_int_prop(cx, obj, "y", y);
   elixir_add_int_prop(cx, obj, "w", w);
   elixir_add_int_prop(cx, obj, "h", h);

   elixir_object_unregister(cx, &obj);

   return obj;
}

Eina_Bool
elixir_extract_geometry(JSContext *cx, JSObject *obj, int *x, int *y, int *w, int *h)
{
   GET_ICOMPONENT(cx, obj, x);
   GET_ICOMPONENT(cx, obj, y);
   GET_ICOMPONENT(cx, obj, w);
   GET_ICOMPONENT(cx, obj, h);

   return EINA_TRUE;
}

JSObject*
elixir_new_position(JSContext *cx, int x, int y)
{
   JSObject *obj;

   obj = JS_NewObject(cx, elixir_class_request("position", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &obj, NULL))
     return NULL;

   elixir_add_int_prop(cx, obj, "x", x);
   elixir_add_int_prop(cx, obj, "y", y);

   elixir_object_unregister(cx, &obj);

   return obj;
}

Eina_Bool
elixir_extract_position(JSContext *cx, JSObject *obj, int *x, int *y)
{
   GET_ICOMPONENT(cx, obj, x);
   GET_ICOMPONENT(cx, obj, y);

   return EINA_TRUE;
}

