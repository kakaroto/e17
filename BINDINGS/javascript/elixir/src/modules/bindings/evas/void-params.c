#include <string.h>

#include <Evas.h>

#include "evas-bindings.h"

static JSBool
elixir_evas_render_method_list(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List *walker;
   Eina_List *list;
   JSObject *array;
   int i;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   list = evas_render_method_list();
   if (!list)
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
     }

   array = elixir_return_array(cx, vp);
   if (!array)
     {
	evas_render_method_list_free(list);
	return JS_FALSE;
     }

   for (walker = list, i = 0; walker; walker = eina_list_next(walker), ++i)
     {
	const char*	method;
	jsval		js_method;

	method = eina_list_data_get(walker);
	js_method = STRING_TO_JSVAL(elixir_ndup(cx, method, strlen(method)));
	if (JS_SetElement(cx, array, i, &js_method) == JS_FALSE)
	  {
	     evas_render_method_list_free(list);
	     return JS_FALSE;
	  }
     }

   evas_render_method_list_free(list);
   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_style_new(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Style *style;
   JSClass *evas_textblock_style_class;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   style = evas_textblock_style_new();
   evas_textblock_style_class = elixir_class_request("evas_textblock_style", NULL);

   elixir_return_ptr(cx, vp, style, evas_textblock_style_class);
   return JS_TRUE;
}

FAST_CALL_PARAMS(evas_alloc_error, elixir_int_params_void);
FAST_CALL_PARAMS(evas_init, elixir_int_params_void);
FAST_CALL_PARAMS(evas_shutdown, elixir_int_params_void);

static JSFunctionSpec     void_params_function[] = {
  ELIXIR_FN(evas_init, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_shutdown, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_render_method_list, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_style_new, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_alloc_error, 0, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_void_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, void_params_function))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_void_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (void_params_function[i].name)
     JS_DeleteProperty(cx, parent, void_params_function[i++].name);

   return EINA_TRUE;
}

