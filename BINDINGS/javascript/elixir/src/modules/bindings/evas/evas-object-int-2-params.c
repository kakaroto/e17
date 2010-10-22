#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*        _evas_object_double_int_params[4] = {
   &evas_object_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_polygon_double_int_params[4] = {
   &evas_object_polygon_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_image_double_int_params[4] = {
   &evas_object_image_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_smart_double_int_params[4] = {
   &evas_object_smart_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_size_params[3] = {
   &evas_object_parameter,
   &size_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_image_size_params[3] = {
   &evas_object_image_parameter,
   &size_parameter,
   NULL
};

static JSBool
elixir_evas_object_int_2_params(void (*func)(Evas_Object* obj, int a, int b),
                                const elixir_parameter_t *params[],
                                JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   int a;
   int b;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   a = val[1].v.num;
   b = val[2].v.num;

   func(know, a, b);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_int_2_size_params(void (*func)(Evas_Object* obj, int w, int h),
                                     const elixir_parameter_t *params1[],
                                     const elixir_parameter_t *params2[],
                                     JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   int w;
   int h;
   elixir_value_t val[3];

   if (elixir_params_check(cx, params1, val, argc, JS_ARGV(cx, vp)))
     {
	know = JS_GetPrivate(cx, val[0].v.obj);

	w = val[1].v.num;
	h = val[2].v.num;
     }

   if (elixir_params_check(cx, params2, val, argc, JS_ARGV(cx, vp)))
     {
	know = JS_GetPrivate(cx, val[0].v.obj);

	if (!elixir_extract_size(cx, val[1].v.obj, &w, &h))
	  return JS_FALSE;
     }

   if (!know) return JS_FALSE;

   func(know, w, h);

   return JS_TRUE;
}

FAST_CALL_PARAMS_2SPEC(evas_object_resize, elixir_evas_object_int_2_size_params, _evas_object_double_int_params, _evas_object_size_params);
FAST_CALL_PARAMS_2SPEC(evas_object_image_size_set, elixir_evas_object_int_2_size_params, _evas_object_image_double_int_params, _evas_object_image_size_params);
FAST_CALL_PARAMS_2SPEC(evas_object_image_load_size_set, elixir_evas_object_int_2_size_params, _evas_object_image_double_int_params, _evas_object_image_size_params);

FAST_CALL_PARAMS_SPEC(evas_object_move, elixir_evas_object_int_2_params, _evas_object_double_int_params);
FAST_CALL_PARAMS_SPEC(evas_object_polygon_point_add, elixir_evas_object_int_2_params, _evas_object_polygon_double_int_params);
FAST_CALL_PARAMS_SPEC(evas_object_size_hint_min_set, elixir_evas_object_int_2_params, _evas_object_double_int_params);
FAST_CALL_PARAMS_SPEC(evas_object_size_hint_max_set, elixir_evas_object_int_2_params, _evas_object_double_int_params);
FAST_CALL_PARAMS_SPEC(evas_object_size_hint_request_set, elixir_evas_object_int_2_params, _evas_object_double_int_params);

static JSBool
elixir_evas_object_text_char_coords_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   int icx;
   int icy;
   int icw;
   int ich;
   int x;
   int y;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _evas_object_double_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   x = val[1].v.num;
   y = val[2].v.num;

   if (!evas_object_text_char_coords_get(know, x, y, &icx, &icy, &icw, &ich))
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
     }

   return elixir_new_evas_pos(cx, icx, icy, icw, ich, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_evas_object_smart_move_children_relative(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   int dx;
   int dy;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _evas_object_smart_double_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   dx = val[1].v.num;
   dy = val[2].v.num;

   evas_object_smart_move_children_relative(know, dx, dy);

   return JS_TRUE;
}


static JSFunctionSpec     evas_object_params_function[] = {
  ELIXIR_FN(evas_object_move, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_polygon_point_add, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_size_hint_min_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_size_hint_max_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_size_hint_request_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_char_coords_get, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_resize, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_size_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_load_size_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_move_children_relative, 3, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_object_int_2_params(JSContext *cx, JSObject *parent)
{
   if (JS_DefineFunctions(cx, parent, evas_object_params_function) == JS_FALSE)
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_evas_object_int_2_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_object_params_function[i].name)
     JS_DeleteProperty(cx, parent, evas_object_params_function[i++].name);

   return EINA_TRUE;
}

