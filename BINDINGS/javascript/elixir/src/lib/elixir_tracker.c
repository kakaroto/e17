#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <eina_stringshare.h>

#include "elixir_private.h"

#include "Elixir.h"

JSObject*
elixir_build_ptr(JSContext *cx, void *ret, JSClass *cls)
{
   JSObject *jo;

   if (!ret) return NULL;

   elixir_lock_cx(cx);

   jo = JS_NewObject(cx, cls, NULL, NULL);

   if (jo && JS_SetPrivate(cx, jo, ret) == JS_FALSE)
     jo = NULL;

   elixir_unlock_cx(cx);

   return jo;
}

JSObject*
elixir_return_ptr(JSContext *cx, jsval *vp, void *ret, JSClass *cls)
{
   JSObject *jo;

   jo = elixir_build_ptr(cx, ret, cls);

   JS_SET_RVAL(cx, vp, jo ? OBJECT_TO_JSVAL(jo) : JSVAL_NULL);
   return jo;
}

JSString*
elixir_return_str(JSContext *cx, jsval *vp, const char *str)
{
   JSString *js = NULL;

   if (str)
     js = elixir_ndup(cx, str, strlen(str));

   JS_SET_RVAL(cx, vp, js ? STRING_TO_JSVAL(js) : JSVAL_NULL);
   return js;
}

JSObject*
elixir_return_array(JSContext *cx, jsval *vp)
{
   JSObject* array;

   elixir_lock_cx(cx);

   array = JS_NewArrayObject(cx, 0, NULL);

   JS_SET_RVAL(cx, vp, array ? OBJECT_TO_JSVAL(array) : JSVAL_NULL);

   elixir_unlock_cx(cx);

   return array;
}

JSObject *
elixir_return_script(JSContext* cx, jsval *vp, Elixir_Script *es)
{
   JSObject *obj;

   elixir_lock_cx(cx);

   obj = JS_NewScriptObject(cx, es->script);
   if (!obj)
     return NULL;

   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));

   elixir_unlock_cx(cx);

   return obj;
}
