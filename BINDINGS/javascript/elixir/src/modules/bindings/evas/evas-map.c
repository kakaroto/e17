#include <Evas.h>

#include "evas-bindings.h"

static elixir_parameter_t evas_map_parameter = {
  "Evas_Map", JOBJECT, NULL
};

static elixir_parameter_t evas_3d_point_parameter = {
  "Evas_3D_Point", JOBJECT, NULL
};

static elixir_parameter_t evas_uv_parameter = {
  "Evas_UV", JOBJECT, NULL
};

static const elixir_parameter_t *_evas_object_evas_map_params[3] = {
  &evas_object_parameter,
  &evas_map_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_evas_object_int_params[4] = {
  &evas_map_parameter,
  &evas_object_parameter,
  &int_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_evas_object_params[3] = {
  &evas_map_parameter,
  &evas_object_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_4_int_params[6] = {
  &evas_map_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_geometry_int_params[4] = {
  &evas_map_parameter,
  &geometry_parameter,
  &int_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_color_params[3] = {
  &evas_map_parameter,
  &color_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_double_2_int_params[5] = {
  &evas_map_parameter,
  &double_parameter,
  &int_parameter,
  &int_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_2_double_2_int_params[7] = {
  &evas_map_parameter,
  &double_parameter,
  &double_parameter,
  &int_parameter,
  &int_parameter,
  NULL
};  

static const elixir_parameter_t *_evas_map_3_double_3_int_params[8] = {
  &evas_map_parameter,
  &double_parameter,
  &double_parameter,
  &double_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  NULL
};  

static const elixir_parameter_t *_evas_map_9_int_params[11] = {
  &evas_map_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_params[2] = {
  &evas_map_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_bool_params[3] = {
  &evas_map_parameter,
  &boolean_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_int_params[3] = {
  &evas_map_parameter,
  &int_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_int_3d_params[4] = {
  &evas_map_parameter,
  &int_parameter,
  &evas_3d_point_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_int_2_double_params[5] = {
  &evas_map_parameter,
  &int_parameter,
  &double_parameter,
  &double_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_int_uv_params[4] = {
  &evas_map_parameter,
  &int_parameter,
  &evas_uv_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_5_int_params[7] = {
  &evas_map_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  NULL
};

static const elixir_parameter_t *_evas_map_int_color_params[4] = {
  &evas_map_parameter,
  &int_parameter,
  &color_parameter,
  NULL
};

static JSBool
elixir_evas_object_map_set(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Object *eo;
  Evas_Map *m;
  elixir_value_t val[2];

  if (!elixir_params_check(cx, _evas_object_evas_map_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, eo);
  GET_PRIVATE(cx, val[1].v.obj, m);

  evas_object_map_set(eo, m);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_util_points_populate_from_object_full(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Object *eo;
  Evas_Map *m;
  elixir_value_t val[3];
  
  if (!elixir_params_check(cx, _evas_map_evas_object_int_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);
  GET_PRIVATE(cx, val[1].v.obj, eo);

  evas_map_util_points_populate_from_object_full(m, eo, val[2].v.num);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_util_points_populate_from_object(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Object *eo;
  Evas_Map *m;
  elixir_value_t val[2];

  if (!elixir_params_check(cx, _evas_map_evas_object_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);
  GET_PRIVATE(cx, val[1].v.obj, eo);

  evas_map_util_points_populate_from_object(m, eo);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_util_points_populate_from_geometry(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m = NULL;
  Evas_Coord x, y, w, h, z;
  elixir_value_t val[6];

  if (elixir_params_check(cx, _evas_map_5_int_params, val, argc, JS_ARGV(cx, vp)))
    {
      GET_PRIVATE(cx, val[0].v.obj, m);

      x = val[1].v.num;
      y = val[2].v.num;
      w = val[3].v.num;
      h = val[4].v.num;
      z = val[5].v.num;
    }
  else if (elixir_params_check(cx, _evas_map_geometry_int_params, val, argc, JS_ARGV(cx, vp)))
    {
      GET_PRIVATE(cx, val[0].v.obj, m);

      if (elixir_extract_geometry(cx, val[1].v.obj, &x, &y, &w, &h))
	m = NULL;

      z = val[2].v.num;
    }
  else return JS_FALSE;

  evas_map_util_points_populate_from_geometry(m, x, y, w, h, z);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_util_points_color_set(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m = NULL;
  int r, g, b, a;
  elixir_value_t val[5];

  if (elixir_params_check(cx, _evas_map_4_int_params, val, argc, JS_ARGV(cx, vp)))
    {
      GET_PRIVATE(cx, val[0].v.obj, m);

      r = val[1].v.num;
      g = val[2].v.num;
      b = val[3].v.num;
      a = val[4].v.num;
    }
  else if (elixir_params_check(cx, _evas_map_color_params, val, argc, JS_ARGV(cx, vp)))
    {
      GET_PRIVATE(cx, val[0].v.obj, m);

      if (!elixir_extract_color(cx, val[1].v.obj, &r, &g, &b, &a))
	m = NULL;
    }
  else return JS_FALSE;

  evas_map_util_points_color_set(m, r, g, b, a);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_util_rotate(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m = NULL;
  elixir_value_t val[4];

  if (!elixir_params_check(cx, _evas_map_double_2_int_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);
  
  evas_map_util_rotate(m, val[1].v.dbl, val[2].v.num, val[3].v.num);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_util_zoom(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m = NULL;
  elixir_value_t val[5];

  if (!elixir_params_check(cx, _evas_map_2_double_2_int_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);

  evas_map_util_zoom(m, val[1].v.dbl, val[2].v.dbl, val[3].v.num, val[4].v.num);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_util_3d_rotate(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m = NULL;
  elixir_value_t val[7];

  if (!elixir_params_check(cx, _evas_map_3_double_3_int_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);

  evas_map_util_3d_rotate(m,
			  val[1].v.dbl, val[2].v.dbl, val[3].v.dbl,
			  val[4].v.num, val[5].v.num, val[6].v.num);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_util_3d_lighting(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m = NULL;
  elixir_value_t val[11];

  if (!elixir_params_check(cx, _evas_map_9_int_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);

  /* FIXME: not handling color here, because of alpha not taked into account */
  evas_map_util_3d_lighting(m,
			    val[1].v.num, val[2].v.num, val[3].v.num,
			    val[4].v.num, val[5].v.num, val[6].v.num,
			    val[7].v.num, val[8].v.num, val[9].v.num);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_util_3d_perspective(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m = NULL;
  elixir_value_t val[5];

  if (!elixir_params_check(cx, _evas_map_4_int_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);

  evas_map_util_3d_perspective(m,
			       val[1].v.num, val[2].v.num, 
			       val[3].v.num,
			       val[4].v.num);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_util_clockwise_get(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m;
  elixir_value_t val[1];

  if (!elixir_params_check(cx, _evas_map_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);

  JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(evas_map_util_clockwise_get(m)));

  return JS_TRUE;
}

static JSBool
elixir_evas_map_new(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m;
  elixir_value_t val[1];

  if (!elixir_params_check(cx, int_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  m = evas_map_new(val[0].v.num);

  elixir_return_ptr(cx, vp, m, elixir_class_request("evas_map", NULL));
  return JS_TRUE;
}



static JSBool
elixir_void_evas_map_bool(void (*func)(Evas_Map *m, Eina_Bool enabled),
			  JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m;
  elixir_value_t val[2];

  if (!elixir_params_check(cx, _evas_map_bool_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);

  func(m, val[1].v.bol);

  return JS_TRUE;
}

FAST_CALL_PARAMS(evas_map_smooth_set,  elixir_void_evas_map_bool);
FAST_CALL_PARAMS(evas_map_alpha_set, elixir_void_evas_map_bool);

static JSBool
elixir_bool_evas_map(Eina_Bool (*func)(const Evas_Map *m),
		     JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m;
  elixir_value_t val[1];

  if (!elixir_params_check(cx, _evas_map_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);

  JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(func(m)));

  return JS_TRUE;
}

FAST_CALL_PARAMS(evas_map_smooth_get, elixir_bool_evas_map);
FAST_CALL_PARAMS(evas_map_alpha_get, elixir_bool_evas_map);

static JSBool
elixir_evas_map_dup(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m;
  elixir_value_t val[1];

  if (!elixir_params_check(cx, _evas_map_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);

  m = evas_map_dup(m);

  elixir_return_ptr(cx, vp, m, elixir_class_request("evas_map", NULL));
  return JS_TRUE;
}

static JSBool
elixir_evas_map_free(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m;
  elixir_value_t val[1];

  if (!elixir_params_check(cx, _evas_map_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);
  JS_SetPrivate(cx, val[0].v.obj, NULL);

  evas_map_free(m);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_count_get(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m;
  elixir_value_t val[1];

  if (!elixir_params_check(cx, _evas_map_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);

  JS_SET_RVAL(cx, vp, INT_TO_JSVAL(evas_map_count_get(m)));
  return JS_TRUE;
}

static JSBool
elixir_evas_map_point_coord_set(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m = NULL;
  int idx;
  Evas_Coord x, y, z;
  elixir_value_t val[5];

  if (elixir_params_check(cx, _evas_map_4_int_params, val, argc, JS_ARGV(cx, vp)))
    {
      GET_PRIVATE(cx, val[0].v.obj, m);
      
      idx = val[1].v.num;

      x = val[2].v.num;
      y = val[3].v.num;
      z = val[4].v.num;
    }
  else if (elixir_params_check(cx, _evas_map_int_3d_params, val, argc, JS_ARGV(cx, vp)))
    {
      GET_PRIVATE(cx, val[0].v.obj, m);

      idx = val[1].v.num;

      GET_ICOMPONENT(cx, val[2].v.obj, &x);
      GET_ICOMPONENT(cx, val[2].v.obj, &y);
      GET_ICOMPONENT(cx, val[2].v.obj, &z);
    }
  else return JS_FALSE;

  evas_map_point_coord_set(m, idx, x, y, z);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_point_coord_get(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m;
  JSObject *obj;
  Evas_Coord x, y, z;
  elixir_value_t val[2];

  if (!elixir_params_check(cx, _evas_map_int_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);

  evas_map_point_coord_get(m, val[1].v.num, &x, &y, &z);

  obj = JS_NewObject(cx, elixir_class_request("evas_3d_point", NULL), NULL, NULL);
  if (!elixir_object_register(cx, &obj, NULL)) 
    return JS_FALSE;

  elixir_add_int_prop(cx, obj, "x", x);
  elixir_add_int_prop(cx, obj, "y", y);
  elixir_add_int_prop(cx, obj, "z", z);

  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));

  elixir_object_unregister(cx, &obj);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_point_image_uv_set(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m = NULL;
  int idx;
  double u, v;
  elixir_value_t val[4];

  if (elixir_params_check(cx, _evas_map_int_2_double_params, val, argc, JS_ARGV(cx, vp)))
    {
      GET_PRIVATE(cx, val[0].v.obj, m);

      idx = val[1].v.num;

      u = val[2].v.dbl;
      v = val[3].v.dbl;
    }
  else if (elixir_params_check(cx, _evas_map_int_uv_params, val, argc, JS_ARGV(cx, vp)))
    {
      GET_PRIVATE(cx, val[0].v.obj, m);

      idx = val[1].v.num;

      GET_DCOMPONENT(cx, val[2].v.obj, &u);
      GET_DCOMPONENT(cx, val[2].v.obj, &v);
    }
  else return JS_FALSE;

  evas_map_point_image_uv_set(m, idx, u, v);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_point_image_uv_get(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m = NULL;
  JSObject *obj;
  double u, v;
  elixir_value_t val[2];

  if (!elixir_params_check(cx, _evas_map_int_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);

  evas_map_point_image_uv_get(m, val[1].v.num, &u, &v);

  obj = JS_NewObject(cx, elixir_class_request("evas_uv", NULL), NULL, NULL);
  if (!elixir_object_register(cx, &obj, NULL))
    return JS_FALSE;

  elixir_add_int_prop(cx, obj, "u", u);
  elixir_add_int_prop(cx, obj, "v", v);

  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));

  elixir_object_unregister(cx, &obj);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_point_color_set(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m = NULL;
  int idx;
  int r, g, b, a;
  elixir_value_t val[6];

  if (elixir_params_check(cx, _evas_map_5_int_params, val, argc, JS_ARGV(cx, vp)))
    {
      GET_PRIVATE(cx, val[0].v.obj, m);

      idx = val[1].v.num;

      r = val[2].v.num;
      g = val[3].v.num;
      b = val[4].v.num;
      a = val[5].v.num;
    }
  else if (elixir_params_check(cx, _evas_map_int_color_params, val, argc, JS_ARGV(cx, vp)))
    {
      GET_PRIVATE(cx, val[0].v.obj, m);

      idx = val[1].v.num;

      if (!elixir_extract_color(cx, val[2].v.obj, &r, &g, &b, &a))
	return JS_FALSE;
    }
  else return JS_FALSE;

  evas_map_point_color_set(m, idx, r, g, b, a);

  return JS_TRUE;
}

static JSBool
elixir_evas_map_point_color_get(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Map *m = NULL;
  int r, g, b, a;
  elixir_value_t val[2];

  if (!elixir_params_check(cx, _evas_map_int_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, m);

  evas_map_point_color_get(m, val[1].v.num, &r, &g, &b, &a);

  JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(elixir_new_color(cx, r, g, b, a)));
  return JS_TRUE;
}

static JSFunctionSpec evas_map_functions[] = {
  ELIXIR_FN(evas_object_map_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_util_points_populate_from_object_full, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_util_points_populate_from_object, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_util_points_populate_from_geometry, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_util_points_color_set, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_util_rotate, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_util_zoom, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_util_3d_rotate, 7, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_util_3d_lighting, 10, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_util_3d_perspective, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_util_clockwise_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_new, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_smooth_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_smooth_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_alpha_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_alpha_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_dup, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_free, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_count_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_point_coord_set, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_point_coord_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_point_image_uv_set, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_point_image_uv_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_point_color_set, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_map_point_color_get, 2, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_map_binding(JSContext *cx, JSObject *parent)
{
  if (!JS_DefineFunctions(cx, parent, evas_map_functions))
    return EINA_FALSE;     

  evas_map_parameter.class = elixir_class_request("evas_map", NULL);
  evas_3d_point_parameter.class = elixir_class_request("evas_3d_map", NULL);
  evas_uv_parameter.class = elixir_class_request("evas_uv", NULL);

  return EINA_TRUE;
}

Eina_Bool
unload_evas_map_binding(JSContext *cx, JSObject *parent)
{
  unsigned int i = 0;

  while (evas_map_functions[i].name)
    JS_DeleteProperty(cx, parent, evas_map_functions[i++].name);

  return EINA_TRUE;
}
