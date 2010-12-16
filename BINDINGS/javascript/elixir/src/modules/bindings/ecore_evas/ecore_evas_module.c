#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <stdlib.h>
#include <Ecore_Evas.h>

#include "Elixir.h"

static elixir_parameter_t               _ecore_evas_parameter = {
  "Ecore_Evas", JOBJECT, NULL
};
static elixir_parameter_t               _evas_parameter = {
  "Evas", JOBJECT, NULL
};
static elixir_parameter_t               _evas_object_parameter = {
  "Evas_Object", JOBJECT, NULL
};
static elixir_parameter_t               _evas_object_image_parameter = {
  "Evas_Object_Image", JOBJECT, NULL
};

static const elixir_parameter_t*        _ecore_evas_params[2] = {
   &_ecore_evas_parameter,
   NULL
};
static const elixir_parameter_t*        _ecore_evas_func_params[3] = {
   &_ecore_evas_parameter,
   &function_parameter,
   NULL
};
static const elixir_parameter_t*        _ecore_evas_int_params[3] = {
   &_ecore_evas_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _ecore_evas_bool_params[3] = {
   &_ecore_evas_parameter,
   &boolean_parameter,
   NULL
};
static const elixir_parameter_t*        _ecore_evas_2int_params[4] = {
   &_ecore_evas_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_params[2] = {
   &_evas_parameter,
   NULL
};
static const elixir_parameter_t*        _ecore_evas_string_params[] = {
   &_ecore_evas_parameter,
   &string_parameter,
   NULL
};
/* static const elixir_parameter_t*        _string_4int_params[6] = { */
/*    &string_parameter, */
/*    &int_parameter, */
/*    &int_parameter, */
/*    &int_parameter, */
/*    &int_parameter, */
/*    NULL */
/* }; */
static const elixir_parameter_t*	_string_4int_string_params[7] = {
  &string_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &string_parameter,
  NULL
};
static const elixir_parameter_t*        _ecore_evas_string_any_params[4] = {
   &_ecore_evas_parameter,
   &string_parameter,
   &any_parameter,
   NULL
};
static const elixir_parameter_t*        _ecore_evas_string_3int_params[6] = {
   &_ecore_evas_parameter,
   &string_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _ecore_evas_evas_object_3int_params[6] = {
   &_ecore_evas_parameter,
   &_evas_object_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _ecore_evas_2string_params[4] = {
   &_ecore_evas_parameter,
   &string_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _ecore_evas_4int_params[6] = {
   &_ecore_evas_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_image_params[2] = {
  &_evas_object_image_parameter,
  NULL
};

static Eina_Bool
elixir_evas_to_jsval(JSContext *cx, Ecore_Evas *ee, jsval *rval)
{
   JSObject *jo;
   Evas *e;
   jsval *tmp;

   tmp = ecore_evas_data_get(ee, "elixir_evas_jsval");
   if (tmp) goto on_found;
   e = ecore_evas_get(ee);

   jo = elixir_build_ptr(cx, e, elixir_class_request("evas", NULL));
   if (!jo) return EINA_FALSE;

   tmp = malloc(sizeof (jsval));
   if (!tmp) return EINA_FALSE;

   *tmp = OBJECT_TO_JSVAL(jo);
   if (!elixir_rval_register(cx, tmp))
     {
	free(tmp);
	return EINA_FALSE;
     }

   ecore_evas_data_set(ee, "elixir_evas_jsval", tmp);

 on_found:
   *rval = *tmp;
   return EINA_TRUE;
}

#if 0
static void
_elixir_ecore_evas_destroy(JSContext *cx, Ecore_Evas *ee)
{
   JSObject *js_obj;
   jsval *tmp;

   if (!ee) return ;

   tmp = ecore_evas_data_get(ee, "_elixir_jsval");
   if (tmp)
     {
	js_obj = JSVAL_TO_OBJECT(*tmp);
	if (js_obj)
	  JS_SetPrivate(cx, js_obj, NULL);

	elixir_rval_delete(cx, tmp);
	free(tmp);

	ecore_evas_data_set(ee, "_elixir_jsval", NULL);
     }

   tmp = ecore_evas_data_get(ee, "_elixir_evas_jsval");
   if (tmp)
     {
	js_obj = JSVAL_TO_OBJECT(*tmp);
	if (js_obj)
	  JS_SetPrivate(cx, js_obj, NULL);

	elixir_rval_delete(cx, tmp);
	free(tmp);

	ecore_evas_data_set(ee, "_elixir_evas_jsval", NULL);
     }
}
#endif

static jsval
_elixir_ecore_evas_to_jsval(JSContext *cx, Ecore_Evas *ee)
{
   JSClass *ecore_evas_class;
   JSObject *jo;
   jsval *tmp;

   if (!ee) return JSVAL_NULL;

   tmp = ecore_evas_data_get(ee, "_elixir_jsval");
   if (tmp) return *tmp;

   ecore_evas_class = elixir_class_request("ecore_evas", NULL);

   jo = elixir_build_ptr(cx, ee, ecore_evas_class);
   if (!jo) return JSVAL_NULL;

   tmp = malloc(sizeof (jsval));
   if (!tmp) return JSVAL_NULL;

   *tmp = OBJECT_TO_JSVAL(jo);
   if (!elixir_rval_register(cx, tmp))
     {
	free(tmp);
	return JSVAL_NULL;
     }

   ecore_evas_data_set(ee, "_elixir_jsval", tmp);

   return *tmp;
}

FAST_CALL_PARAMS(ecore_evas_init, elixir_int_params_void);
FAST_CALL_PARAMS(ecore_evas_shutdown, elixir_int_params_void);

static JSBool
elixir_void_params_ecore_evas(void (*func)(Ecore_Evas *ee),
                              JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   func(ee);

   return JS_TRUE;
}

FAST_CALL_PARAMS(ecore_evas_show, elixir_void_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_hide, elixir_void_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_raise, elixir_void_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_lower, elixir_void_params_ecore_evas);

static JSBool
elixir_ecore_evas_free(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   void *data;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   data = evas_data_attach_get(ecore_evas_get(ee));
   evas_data_attach_set(ecore_evas_get(ee), NULL);
   elixir_void_free(data);

   data = ecore_evas_data_get(ee, "_elixir_jsval");
   ecore_evas_data_set(ee, "_elixir_jsval", NULL);
   elixir_void_free(data);

   ecore_evas_free(ee);

   return JS_TRUE;
}


static int
_elixir_ecore_evas_avoid_damage_get(const Ecore_Evas *ee)
{
   return (int) ecore_evas_avoid_damage_get(ee);
}

static JSBool
elixir_int_params_ecore_evas(int (*func)(const Ecore_Evas *ee),
                             JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(ee)));
   return JS_TRUE;
}

static JSBool
elixir_boolean_params_ecore_evas(Eina_Bool (*func)(const Ecore_Evas *ee),
				 JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(func(ee)));
   return JS_TRUE;
}

FAST_CALL_PARAMS(ecore_evas_rotation_get, elixir_int_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_shaped_get, elixir_boolean_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_alpha_get, elixir_boolean_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_visibility_get, elixir_int_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_layer_get, elixir_int_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_focus_get, elixir_boolean_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_iconified_get, elixir_boolean_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_borderless_get, elixir_boolean_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_override_get, elixir_boolean_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_maximized_get, elixir_boolean_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_fullscreen_get, elixir_boolean_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_withdrawn_get, elixir_boolean_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_sticky_get, elixir_boolean_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_ignore_events_get, elixir_boolean_params_ecore_evas);

FAST_CALL_PARAMS_CAST(ecore_evas_avoid_damage_get, elixir_int_params_ecore_evas);

static JSBool
elixir_void_params_2int(void (*func)(Ecore_Evas *ee, int a, int b),
                        JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _ecore_evas_2int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   func(ee, val[1].v.num, val[2].v.num);

   return JS_TRUE;
}

FAST_CALL_PARAMS(ecore_evas_move, elixir_void_params_2int);
FAST_CALL_PARAMS(ecore_evas_managed_move, elixir_void_params_2int);
FAST_CALL_PARAMS(ecore_evas_resize, elixir_void_params_2int);
FAST_CALL_PARAMS(ecore_evas_size_min_set, elixir_void_params_2int);
FAST_CALL_PARAMS(ecore_evas_size_max_set, elixir_void_params_2int);
FAST_CALL_PARAMS(ecore_evas_size_base_set, elixir_void_params_2int);
FAST_CALL_PARAMS(ecore_evas_size_step_set, elixir_void_params_2int);

static void
_elixir_ecore_evas_avoid_damage_set(Ecore_Evas *ee, int a)
{
   ecore_evas_avoid_damage_set(ee, (Ecore_Evas_Avoid_Damage_Type) a);
}

static JSBool
elixir_void_params_int(void (*func)(Ecore_Evas *ee, int a),
                       JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_evas_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   func(ee, val[1].v.num);

   return JS_TRUE;
}

static JSBool
elixir_void_params_boolean(void (*func)(Ecore_Evas *ee, Eina_Bool a),
			   JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_evas_bool_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   func(ee, val[1].v.bol);

   return JS_TRUE;
}

FAST_CALL_PARAMS(ecore_evas_rotation_set, elixir_void_params_int);
FAST_CALL_PARAMS(ecore_evas_shaped_set, elixir_void_params_boolean);
FAST_CALL_PARAMS(ecore_evas_alpha_set, elixir_void_params_boolean);
FAST_CALL_PARAMS(ecore_evas_layer_set, elixir_void_params_int);
FAST_CALL_PARAMS(ecore_evas_focus_set, elixir_void_params_boolean);
FAST_CALL_PARAMS(ecore_evas_iconified_set, elixir_void_params_boolean);
FAST_CALL_PARAMS(ecore_evas_borderless_set, elixir_void_params_boolean);
FAST_CALL_PARAMS(ecore_evas_override_set, elixir_void_params_boolean);
FAST_CALL_PARAMS(ecore_evas_maximized_set, elixir_void_params_boolean);
FAST_CALL_PARAMS(ecore_evas_fullscreen_set, elixir_void_params_boolean);
FAST_CALL_PARAMS(ecore_evas_withdrawn_set, elixir_void_params_boolean);
FAST_CALL_PARAMS(ecore_evas_sticky_set, elixir_void_params_boolean);
FAST_CALL_PARAMS(ecore_evas_ignore_events_set, elixir_void_params_boolean);

FAST_CALL_PARAMS_CAST(ecore_evas_avoid_damage_set, elixir_void_params_int);

static JSBool
elixir_2int_params_ecore_evas(void (*func)(const Ecore_Evas *ee, int *w, int *h),
                              JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas*          ee;
   JSObject*            obj;
   int                  w;
   int                  h;
   elixir_value_t       val[1];

   if (!elixir_params_check(cx, _ecore_evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   func(ee, &w, &h);

   obj = JS_NewObject(cx, elixir_class_request("Ecore_Evas", NULL), NULL, NULL);
   if (!obj)
     return JS_FALSE;

   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));

   elixir_add_int_prop(cx, obj, "w", w);
   elixir_add_int_prop(cx, obj, "h", h);

   return JS_TRUE;
}

FAST_CALL_PARAMS(ecore_evas_size_min_get, elixir_2int_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_size_max_get, elixir_2int_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_size_base_get, elixir_2int_params_ecore_evas);
FAST_CALL_PARAMS(ecore_evas_size_step_get, elixir_2int_params_ecore_evas);

static JSBool
elixir_ecore_evas_engine_type_supported_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas_Engine_Type engine;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   engine = val[0].v.num;
   JS_SET_RVAL(cx, vp, ecore_evas_engine_type_supported_get(engine));
   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_engines_get(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List *list;
   Eina_List *it;
   JSObject *array;
   const char *engine;
   int index;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   list = ecore_evas_engines_get();

   array = elixir_return_array(cx, vp);
   if (!array) return JS_FALSE;

   index = 0;
   EINA_LIST_FOREACH(list, it, engine)
     {
	JSString *str;

	str = elixir_ndup(cx, engine, strlen(engine));
	JS_DefineElement(cx, array, index, STRING_TO_JSVAL(str), NULL,
			 NULL, JSPROP_INDEX | JSPROP_ENUMERATE | JSPROP_READONLY);
     }

   ecore_evas_engines_free(list);
   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_new(JSContext *cx, uintN argc, jsval *vp)
{
   const char *engine_name;
   const char *options;
   Ecore_Evas *ee;
   int x;
   int y;
   int w;
   int h;
   elixir_value_t val[6];

   if (!elixir_params_check(cx, _string_4int_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   engine_name = elixir_get_string_bytes(val[0].v.str, NULL);
   x = val[1].v.num;
   y = val[2].v.num;
   w = val[3].v.num;
   h = val[4].v.num;
   options = JS_GetStringBytes(val[5].v.str);

   ee = ecore_evas_new(engine_name, x, y, w, h, options);

   JS_SET_RVAL(cx, vp, _elixir_ecore_evas_to_jsval(cx, ee));
   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   return elixir_evas_to_jsval(cx, ee, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_ecore_evas_title_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   const char *title;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   title = ecore_evas_title_get(ee);

   elixir_return_str(cx, vp, title);
   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_object_image_new(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee_target;
   Evas_Object *eo;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee_target);

   eo = ecore_evas_object_image_new(ee_target);

   return evas_object_to_jsval(cx, eo, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_ecore_evas_object_ecore_evas_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   Evas_Object *eo;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_image_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   ee = ecore_evas_object_ecore_evas_get(eo);

   JS_SET_RVAL(cx, vp, _elixir_ecore_evas_to_jsval(cx, ee));
   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_object_evas_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   Evas_Object *eo;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_image_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   ee = ecore_evas_object_ecore_evas_get(eo);

   return elixir_evas_to_jsval(cx, ee, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_ecore_evas_ecore_evas_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   Evas *e;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, e);

   ee = ecore_evas_ecore_evas_get(e);

   JS_SET_RVAL(cx, vp, _elixir_ecore_evas_to_jsval(cx, ee));
   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_cursor_get(JSContext *cx, uintN argc, jsval *vp)
{
   JSObject *ret;
   Ecore_Evas *ee;
   Evas_Object *eo;
   int layer;
   int hot_x;
   int hot_y;
   elixir_value_t val[1];
   jsval tmp;

   if (!elixir_params_check(cx, _ecore_evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   ecore_evas_cursor_get(ee, &eo, &layer, &hot_x, &hot_y);

   ret = JS_NewObject(cx, elixir_class_request("Ecore_Cursor", NULL), NULL, NULL);
   if (!ret)
     return JS_FALSE;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(ret));

   elixir_add_int_prop(cx, ret, "layer", layer);
   elixir_add_int_prop(cx, ret, "hot_x", hot_x);
   elixir_add_int_prop(cx, ret, "hot_y", hot_y);

   if (!evas_object_to_jsval(cx, eo, &tmp))
     return JS_FALSE;

   JS_DefineProperty(cx, ret, "obj", tmp, NULL, NULL, JSPROP_ENUMERATE | JSPROP_READONLY);

   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_geometry_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   JSObject *ret;
   int x;
   int y;
   int w;
   int h;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   ecore_evas_geometry_get(ee, &x, &y, &w, &h);

   ret = elixir_new_geometry(cx, x, y, w, h);
   if (!ret)
     return JS_FALSE;

   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(ret));
   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_data_get(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   const char *key;
   void *data;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_evas_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);
   key = elixir_get_string_bytes(val[1].v.str, NULL);

   if (key && strncmp(key, "_elixir_", 8) == 0)
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
     }

   data = ecore_evas_data_get(ee, key);

   JS_SET_RVAL(cx, vp, elixir_void_get_jsval(data));
   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_title_set(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   const char *t;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_evas_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);
   t = elixir_get_string_bytes(val[1].v.str, NULL);

   ecore_evas_title_set(ee, t);

   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_data_set(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   const char *key;
   void *data;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _ecore_evas_string_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);
   key = elixir_get_string_bytes(val[1].v.str, NULL);

   if (key && strncmp(key, "_elixir_", 8) == 0)
     return JS_FALSE;

   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, NULL);

   elixir_void_free(ecore_evas_data_get(ee, key));
   ecore_evas_data_set(ee, key, data);

   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_cursor_set(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   char *file;
   int layer;
   int hot_x;
   int hot_y;
   elixir_value_t val[5];

   if (!elixir_params_check(cx, _ecore_evas_string_3int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);
   file = elixir_file_canonicalize(elixir_get_string_bytes(val[1].v.str, NULL));
   layer = val[2].v.num;
   hot_x = val[3].v.num;
   hot_y = val[4].v.num;

   ecore_evas_cursor_set(ee, file, layer, hot_x, hot_y);

   free(file);

   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_object_cursor_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   Ecore_Evas *ee;
   int layer;
   int hot_x;
   int hot_y;
   elixir_value_t val[5];

   if (!elixir_params_check(cx, _ecore_evas_evas_object_3int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);
   GET_PRIVATE(cx, val[1].v.obj, eo);
   layer = val[2].v.num;
   hot_x = val[3].v.num;
   hot_y = val[4].v.num;

   ecore_evas_object_cursor_set(ee, eo, layer, hot_x, hot_y);

   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_name_class_set(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   const char *n;
   const char *c;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _ecore_evas_2string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);
   n = elixir_get_string_bytes(val[1].v.str, NULL);
   c = elixir_get_string_bytes(val[2].v.str, NULL);

   ecore_evas_name_class_set(ee, n, c);

   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_name_class_get(JSContext *cx, uintN argc, jsval *vp)
{
   const char *n = NULL;
   const char *c = NULL;
   Ecore_Evas *ee;
   JSObject *ret;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   ecore_evas_name_class_get(ee, &n, &c);

   ret = JS_NewObject(cx, elixir_class_request("Ecore_Class", NULL), NULL, NULL);
   if (!ret)
     return JS_FALSE;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(ret));

   elixir_add_str_prop(cx, ret, "n", n);
   elixir_add_str_prop(cx, ret, "c", c);

   return JS_TRUE;
}

static JSBool
elixir_ecore_evas_move_resize(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   int x;
   int y;
   int w;
   int h;
   elixir_value_t val[5];

   if (!elixir_params_check(cx, _ecore_evas_4int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);
   x = val[1].v.num;
   y = val[2].v.num;
   w = val[3].v.num;
   h = val[4].v.num;

   ecore_evas_move_resize(ee, x, y, w, h);

   return JS_TRUE;
}

static void
_elixir_ecore_evas_callback(const char *name, Ecore_Evas *ee)
{
   JSFunction *cb;
   JSContext *cx;
   JSObject *parent;
   void *data;
   Eina_Bool suspended;
   jsval js_return;
   jsval argv[1];

   data = ecore_evas_data_get(ee, name);

   cb = elixir_void_get_private(data);
   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);

   if (!cx || !parent || !cb)
     return ;

   suspended = elixir_function_suspended(cx);
   if (suspended)
     elixir_function_start(cx);

   argv[0] = _elixir_ecore_evas_to_jsval(cx, ee);
   elixir_rval_register(cx, argv);

   elixir_function_run(cx, cb, parent, 1, argv, &js_return);

   elixir_rval_delete(cx, argv + 0);

   if (suspended)
     elixir_function_stop(cx);
}

static JSBool
_elixir_ecore_evas_callback_set(const char* desc,
                                void (*func_call)(Ecore_Evas *ee, void (*func)(Ecore_Evas *ee)),
                                void (*func_eeecb)(Ecore_Evas *ee),
                                JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Evas *ee;
   void *data;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _ecore_evas_func_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ee);

   data = ecore_evas_data_get(ee, desc);
   if (data)
     {
	elixir_void_free(data);
	if (val[1].v.fct)
	  {
	     data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), JSVAL_VOID, val[1].v.fct);
	     ecore_evas_data_set(ee, desc, data);

	     return JS_TRUE;
	  }
     }

   if (val[1].v.fct)
     data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), JSVAL_VOID, val[1].v.fct);
   else
     data = NULL;

   ecore_evas_data_set(ee, desc, data);

   func_call(ee, !!val[1].v.fct ? func_eeecb : NULL);

   return JS_TRUE;
}

#define EEECB(Name)							\
  static void _eeecb_##Name(Ecore_Evas* ee)				\
  {									\
     _elixir_ecore_evas_callback("_elixir_" #Name, ee);			\
  }									\
  static JSBool								\
  elixir_##Name(JSContext *cx, uintN argc, jsval *vp)			\
  {									\
     return _elixir_ecore_evas_callback_set("_elixir_" #Name, Name, _eeecb_##Name, cx, argc, vp); \
  }

EEECB(ecore_evas_callback_resize_set);
EEECB(ecore_evas_callback_move_set);
EEECB(ecore_evas_callback_show_set);
EEECB(ecore_evas_callback_hide_set);
EEECB(ecore_evas_callback_delete_request_set);
EEECB(ecore_evas_callback_destroy_set);
EEECB(ecore_evas_callback_focus_in_set);
EEECB(ecore_evas_callback_focus_out_set);
EEECB(ecore_evas_callback_sticky_set);
EEECB(ecore_evas_callback_unsticky_set);
EEECB(ecore_evas_callback_mouse_in_set);
EEECB(ecore_evas_callback_mouse_out_set);
EEECB(ecore_evas_callback_pre_render_set);
EEECB(ecore_evas_callback_post_render_set);

static JSFunctionSpec   ecore_evas_functions[] = {
  ELIXIR_FN(ecore_evas_callback_resize_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_move_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_show_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_hide_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_delete_request_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_destroy_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_focus_in_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_focus_out_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_sticky_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_unsticky_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_mouse_in_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_mouse_out_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_pre_render_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_callback_post_render_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_move_resize, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_name_class_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_name_class_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_cursor_set, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_object_cursor_set, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_data_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_title_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_data_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_geometry_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_cursor_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_engine_type_supported_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_title_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_object_image_new, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_object_ecore_evas_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_object_evas_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_ecore_evas_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_init, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_shutdown, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_free, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_show, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_hide, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_raise, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_lower, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_rotation_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_shaped_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_alpha_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_visibility_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_layer_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_focus_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_iconified_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_borderless_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_override_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_maximized_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_fullscreen_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_avoid_damage_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_withdrawn_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_sticky_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_ignore_events_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_move, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_managed_move, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_resize, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_size_min_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_size_max_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_size_base_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_size_step_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_rotation_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_shaped_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_alpha_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_layer_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_focus_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_iconified_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_borderless_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_override_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_maximized_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_fullscreen_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_avoid_damage_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_withdrawn_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_sticky_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_ignore_events_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_size_min_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_size_max_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_size_base_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_size_step_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_engines_get, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(ecore_evas_new, 6, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

static const struct {
   const char*  name;
   int          value;
} ecore_evas_const_properties[] = {
   { "ECORE_EVAS_ENGINE_SOFTWARE_BUFFER", ECORE_EVAS_ENGINE_SOFTWARE_BUFFER },
   { "ECORE_EVAS_ENGINE_SOFTWARE_XLIB", ECORE_EVAS_ENGINE_SOFTWARE_XLIB },
   { "ECORE_EVAS_ENGINE_XRENDER_X11", ECORE_EVAS_ENGINE_XRENDER_X11 },
   { "ECORE_EVAS_ENGINE_OPENGL_X11", ECORE_EVAS_ENGINE_OPENGL_X11 },
   { "ECORE_EVAS_ENGINE_SOFTWARE_XCB", ECORE_EVAS_ENGINE_SOFTWARE_XCB },
   { "ECORE_EVAS_ENGINE_XRENDER_XCB", ECORE_EVAS_ENGINE_XRENDER_XCB },
   { "ECORE_EVAS_ENGINE_SOFTWARE_DDRAW", ECORE_EVAS_ENGINE_SOFTWARE_DDRAW },
   { "ECORE_EVAS_ENGINE_DIRECT3D", ECORE_EVAS_ENGINE_DIRECT3D },
   { "ECORE_EVAS_ENGINE_OPENGL_GLEW", ECORE_EVAS_ENGINE_OPENGL_GLEW },
   { "ECORE_EVAS_ENGINE_SOFTWARE_SDL", ECORE_EVAS_ENGINE_SOFTWARE_SDL },
   { "ECORE_EVAS_ENGINE_DIRECTFB", ECORE_EVAS_ENGINE_DIRECTFB },
   { "ECORE_EVAS_ENGINE_SOFTWARE_FB", ECORE_EVAS_ENGINE_SOFTWARE_FB },
   { "ECORE_EVAS_ENGINE_SOFTWARE_16_X11", ECORE_EVAS_ENGINE_SOFTWARE_16_X11 },
   { "ECORE_EVAS_ENGINE_SOFTWARE_16_DDRAW", ECORE_EVAS_ENGINE_SOFTWARE_16_DDRAW },
   { "ECORE_EVAS_ENGINE_SOFTWARE_16_WINCE", ECORE_EVAS_ENGINE_SOFTWARE_16_WINCE },

   { "ECORE_EVAS_AVOID_DAMAGE_NONE", ECORE_EVAS_AVOID_DAMAGE_NONE },
   { "ECORE_EVAS_AVOID_DAMAGE_EXPOSE", ECORE_EVAS_AVOID_DAMAGE_EXPOSE },
   { "ECORE_EVAS_AVOID_DAMAGE_BUILT_IN", ECORE_EVAS_AVOID_DAMAGE_BUILT_IN },

   { "ECORE_EVAS_OBJECT_ASSOCIATE_BASE", ECORE_EVAS_OBJECT_ASSOCIATE_BASE },
   { "ECORE_EVAS_OBJECT_ASSOCIATE_STACK", ECORE_EVAS_OBJECT_ASSOCIATE_STACK },
   { "ECORE_EVAS_OBJECT_ASSOCIATE_LAYER", ECORE_EVAS_OBJECT_ASSOCIATE_LAYER },

   { NULL, 0 }
};

static Eina_Bool
module_open(Elixir_Module *em, JSContext *cx, JSObject *parent)
{
   void *tmp;
   unsigned int i = 0;
   jsval property;

   if (em->data)
     return EINA_TRUE;

   em->data = parent;
   tmp = &em->data;
   if (!elixir_object_register(cx, (JSObject**) tmp, NULL))
     goto on_error;

   if (!JS_DefineFunctions(cx, *((JSObject**) tmp), ecore_evas_functions))
     goto on_error;

   while (ecore_evas_const_properties[i].name)
     {
        property = INT_TO_JSVAL(ecore_evas_const_properties[i].value);
        if (!JS_DefineProperty(cx, parent,
			       ecore_evas_const_properties[i].name,
			       property,
			       NULL, NULL,
			       JSPROP_ENUMERATE | JSPROP_READONLY))
          goto on_error;
        ++i;
     }

   _ecore_evas_parameter.class = elixir_class_request("ecore_evas", NULL);
   _evas_parameter.class = elixir_class_request("evas", NULL);
   _evas_object_image_parameter.class = elixir_class_request("evas_object_image", "evas_object");

   return EINA_TRUE;

  on_error:
   if (tmp)
     elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;
   return EINA_FALSE;
}

static Eina_Bool
module_close(Elixir_Module *em, JSContext *cx)
{
   JSObject *parent;
   void **tmp;
   unsigned int i;

   if (!em->data)
     return EINA_FALSE;

   parent = (JSObject*) em->data;
   tmp = &em->data;

   i = 0;
   while (ecore_evas_functions[i].name)
     JS_DeleteProperty(cx, parent, ecore_evas_functions[i++].name);

   i = 0;
   while (ecore_evas_const_properties[i].name)
     JS_DeleteProperty(cx, parent, ecore_evas_const_properties[i++].name);

   elixir_object_unregister(cx, (JSObject**) tmp);

   em->data = NULL;

   return EINA_TRUE;
}

static const Elixir_Module_Api  module_api_elixir = {
   ELIXIR_MODULE_API_VERSION,
   ELIXIR_GRANTED,
   "ecore-evas",
   "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_ecore_evas = {
  &module_api_elixir,
  NULL,
  EINA_FALSE,
  {
    module_open,
    NULL,
    module_close
  }
};

Eina_Bool
ecore_evas_binding_init(void)
{
   return elixir_modules_register(&em_ecore_evas);
}

void
ecore_evas_binding_shutdown(void)
{
   elixir_modules_unregister(&em_ecore_evas);
}

#ifndef EINA_STATIC_BUILD_ECORE_EVAS
EINA_MODULE_INIT(ecore_evas_binding_init);
EINA_MODULE_SHUTDOWN(ecore_evas_binding_shutdown);
#endif
