#include <string.h>

#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*        _evas_params[2] = {
   &evas_parameter,
   NULL
};

/* FIXME: Don't say anything with an undefined arg. */
static JSBool
elixir_evas_params(void (*func)(Evas *evas),
                   JSContext* cx, uintN argc, jsval *vp)
{
   Evas *evas = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);

   func(evas);

   return JS_TRUE;
}

static int
_elixir_evas_font_hinting_get(const Evas *evas)
{
   return (int)evas_font_hinting_get(evas);
}

static JSBool
elixir_int_evas_params(int (*func)(const Evas *evas),
                       JSContext *cx, uintN argc, jsval *vp)
{
   Evas *evas = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(evas)));
   return JS_TRUE;
}

static JSBool
elixir_object_const_evas_params(Evas_Object* (*func)(const Evas *evas),
				JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *obj = NULL;
   Evas *evas = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);

   obj = func(evas);

   return evas_object_to_jsval(cx, obj, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_object_evas_params(Evas_Object* (*func)(Evas *evas),
                          JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *obj = NULL;
   Evas *evas = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);

   obj = func(evas);

   return evas_object_to_jsval(cx, obj, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_evas_font_path_list(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List *list = NULL;
   Evas *evas = NULL;
   JSObject *array;
   int i;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);

   list = (Eina_List*) evas_font_path_list(evas);
   if (!list)
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
     }

   array = elixir_return_array(cx, vp);
   if (!array) return JS_FALSE;

   for (i = 0; list; list = eina_list_next(list), ++i)
     {
	const char*	path;
	jsval		js_path;

	path = eina_list_data_get(list);
	js_path = path
	  ? STRING_TO_JSVAL(elixir_ndup(cx, path, strlen(path)))
	  : JSVAL_NULL;
	if (JS_SetElement(cx, array, i, &js_path) == JS_FALSE)
	  return JS_FALSE;
     }

   return JS_TRUE;
}

static JSBool
elixir_evas_font_available_list(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List *walker = NULL;
   Eina_List *list = NULL;
   Evas *evas = NULL;
   JSObject *array;
   int i;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);

   list = evas_font_available_list(evas);
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
	const char *font;
	jsval js_font;

	font = eina_list_data_get(walker);
	js_font = STRING_TO_JSVAL(elixir_ndup(cx, font, strlen(font)));
	if (JS_SetElement(cx, array, i, &js_font) == JS_FALSE)
	  {
	     evas_font_available_list_free(evas, list);
	     return JS_FALSE;
	  }
     }

   evas_font_available_list_free(evas, list);
   return JS_TRUE;
}

static JSBool
elixir_evas_output_size_get(JSContext *cx, uintN argc, jsval *vp)
{
   JSObject *ret;
   Evas *e;
   int w = 0;
   int h = 0;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, e);

   evas_output_size_get(e, &w, &h);

   ret = elixir_new_size(cx, w, h);
   if (!ret) return JS_FALSE;

   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(ret));
   return JS_TRUE;
}

FAST_CALL_PARAMS(evas_render, elixir_evas_params);
FAST_CALL_PARAMS(evas_image_cache_flush, elixir_evas_params);
FAST_CALL_PARAMS(evas_image_cache_reload, elixir_evas_params);
FAST_CALL_PARAMS(evas_font_path_clear, elixir_evas_params);
FAST_CALL_PARAMS(evas_font_cache_flush, elixir_evas_params);
FAST_CALL_PARAMS(evas_event_freeze, elixir_evas_params);
FAST_CALL_PARAMS(evas_event_thaw, elixir_evas_params);
/* We don't have enum in JS. */
FAST_CALL_PARAMS(evas_event_freeze_get, elixir_int_evas_params);
FAST_CALL_PARAMS_CAST(evas_font_hinting_get, elixir_int_evas_params);
FAST_CALL_PARAMS(evas_font_cache_get, elixir_int_evas_params);
FAST_CALL_PARAMS(evas_image_cache_get, elixir_int_evas_params);
FAST_CALL_PARAMS(evas_object_bottom_get, elixir_object_const_evas_params);
FAST_CALL_PARAMS(evas_object_top_get, elixir_object_const_evas_params);
FAST_CALL_PARAMS(evas_object_top_at_pointer_get, elixir_object_const_evas_params);
FAST_CALL_PARAMS(evas_focus_get, elixir_object_const_evas_params);
FAST_CALL_PARAMS(evas_object_image_add, elixir_object_evas_params);
FAST_CALL_PARAMS(evas_object_line_add, elixir_object_evas_params);
FAST_CALL_PARAMS(evas_object_polygon_add, elixir_object_evas_params);
FAST_CALL_PARAMS(evas_object_rectangle_add, elixir_object_evas_params);
FAST_CALL_PARAMS(evas_object_textblock_add, elixir_object_evas_params);
FAST_CALL_PARAMS(evas_object_text_add, elixir_object_evas_params);

static JSFunctionSpec     evas_params_function[] = {
  ELIXIR_FN(evas_event_freeze_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_event_thaw, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_event_freeze, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_render, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_image_cache_flush, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_image_cache_reload, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_font_cache_flush, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_font_path_clear, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_font_cache_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_image_cache_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_font_hinting_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_bottom_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_top_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_focus_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_add, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_line_add, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_polygon_add, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_rectangle_add, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_textblock_add, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_add, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_font_path_list, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_font_available_list, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_top_at_pointer_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_output_size_get, 1, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_params_function))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_evas_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_params_function[i].name)
     JS_DeleteProperty(cx, parent, evas_params_function[i++].name);

   return EINA_TRUE;
}

