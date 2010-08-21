#include <string.h>
#include <stdlib.h>
#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*	_evas_double_int_double_bool_params[6] = {
  &evas_parameter,
  &int_parameter,
  &int_parameter,
  &boolean_parameter,
  &boolean_parameter,
  NULL
};
static const elixir_parameter_t*	_evas_four_int_double_bool_params[8] = {
  &evas_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &boolean_parameter,
  &boolean_parameter,
  NULL
};
static const elixir_parameter_t*	_evas_object_string_any_params[4] = {
  &evas_object_parameter,
  &string_parameter,
  &any_parameter,
  NULL
};
static const elixir_parameter_t*        _evas_object_smart_any_params[3] = {
  &evas_object_smart_parameter,
  &any_parameter,
  NULL
};

static JSBool
elixir_evas_object_top_at_xy_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *js_obj;
   Evas *evas;
   int include_pass_events_objects;
   int include_hidden_objects;
   int x;
   int y;
   elixir_value_t val[5];

   if (!elixir_params_check(cx, _evas_double_int_double_bool_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);
   x = val[1].v.num;
   y = val[2].v.num;
   include_pass_events_objects = val[3].v.bol;
   include_hidden_objects = val[4].v.bol;

   js_obj = evas_object_top_at_xy_get(evas, x, y, include_pass_events_objects, include_hidden_objects);

   return evas_object_to_jsval(cx, js_obj, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_evas_object_top_in_rectangle_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *js_obj;
   Evas *evas;
   int include_pass_events_objects;
   int include_hidden_objects;
   int x;
   int y;
   int w;
   int h;
   elixir_value_t val[7];

   if (!elixir_params_check(cx, _evas_four_int_double_bool_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);
   x = val[1].v.num;
   y = val[2].v.num;
   w = val[3].v.num;
   h = val[4].v.num;
   include_pass_events_objects = val[5].v.bol;
   include_hidden_objects = val[6].v.bol;

   js_obj = evas_object_top_in_rectangle_get(evas, x, y, w, h, include_pass_events_objects, include_hidden_objects);

   return evas_object_to_jsval(cx, js_obj, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_evas_objects_at_xy_get(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List *list;
   Eina_List *walker;
   JSObject *array;
   Evas *evas;
   int include_pass_events_objects;
   int include_hidden_objects;
   int x;
   int y;
   int i;
   elixir_value_t val[5];

   if (!elixir_params_check(cx, _evas_double_int_double_bool_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);
   x = val[1].v.num;
   y = val[2].v.num;
   include_pass_events_objects = val[3].v.bol;
   include_hidden_objects = val[4].v.bol;

   list = evas_objects_at_xy_get(evas, x, y, include_pass_events_objects, include_hidden_objects);
   if (!list)
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
     }

   array = elixir_return_array(cx, vp);
   if (!array)
     {
	eina_list_free(list);
	return JS_FALSE;
     }

   for (i = 0, walker = list; walker; walker = eina_list_next(walker), ++i)
     {
	Evas_Object*	eobj;
	jsval		js_obj;

	eobj = eina_list_data_get(walker);
	evas_object_to_jsval(cx, eobj, &js_obj);
	if (JS_SetElement(cx, array, i, &js_obj) == JS_FALSE)
	  {
	     eina_list_free(list);
	     return JS_FALSE;
	  }
     }

   return JS_TRUE;
}

static JSBool
elixir_evas_objects_in_rectangle_get(JSContext *cx, uintN argc, jsval *vp)
{
   JSObject *array;
   Eina_List *list;
   Eina_List *walker;
   Evas *evas;
   int x;
   int y;
   int w;
   int h;
   int include_pass_events_objects;
   int include_hidden_objects;
   int i;
   elixir_value_t val[7];

   if (!elixir_params_check(cx, _evas_four_int_double_bool_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);
   x = val[1].v.num;
   y = val[2].v.num;
   w = val[3].v.num;
   h = val[4].v.num;
   include_pass_events_objects = val[5].v.bol;
   include_hidden_objects = val[6].v.bol;

   list = evas_objects_in_rectangle_get(evas, x, y, w, h, include_pass_events_objects, include_hidden_objects);
   if (!list)
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
     }

   array = elixir_return_array(cx, vp);
   if (!array)
     {
	eina_list_free(list);
	return JS_FALSE;
     }

   for (i = 0, walker = list; walker; walker = eina_list_next(walker), ++i)
     {
	Evas_Object *obj;
	jsval js_obj;

	obj = eina_list_data_get(walker);
	evas_object_to_jsval(cx, obj, &js_obj);
	if (JS_SetElement(cx, array, i, &js_obj) == JS_FALSE)
	  {
	     eina_list_free(list);
	     return JS_FALSE;
	  }
     }

   return JS_TRUE;
}

static JSBool
elixir_evas_object_data_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   void *dt;
   const char *key;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _evas_object_string_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   key = elixir_get_string_bytes(val[1].v.str, NULL);

   if (key && strncmp(key, "elixir_jsval", 12) == 0)
     return JS_FALSE;

   elixir_void_free(evas_object_data_get(know, key));

   dt = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, NULL);
   evas_object_data_set(know, key, dt);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_smart_data_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   void *dt;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_object_smart_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   elixir_void_free(evas_object_smart_data_get(know));

   dt = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[1].v.any, NULL);
   evas_object_smart_data_set(know, dt);

   return JS_TRUE;
}

static JSFunctionSpec	evas_objects_functions[] = {
  ELIXIR_FN(evas_object_top_at_xy_get, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_top_in_rectangle_get, 7, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_objects_at_xy_get, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_objects_in_rectangle_get, 7, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_data_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_data_set, 2, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_object_bindings(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_objects_functions))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_evas_object_bindings(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_objects_functions[i].name)
     JS_DeleteProperty(cx, parent, evas_objects_functions[i++].name);

   return EINA_TRUE;
}

