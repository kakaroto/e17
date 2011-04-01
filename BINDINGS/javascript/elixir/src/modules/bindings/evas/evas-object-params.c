#include <string.h>
#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*        _evas_object_params[2] = {
   &evas_object_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_smart_params[2] = {
   &evas_object_smart_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_line_params[2] = {
   &evas_object_line_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_image_params[2] = {
   &evas_object_image_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_text_params[2] = {
   &evas_object_text_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_textblock_params[2] = {
   &evas_object_textblock_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_polygon_params[2] = {
   &evas_object_polygon_parameter,
   NULL
};

static JSBool
elixir_evas_position_evas_object_params(void (*func)(const Evas_Object *eo, double *x, double *y),
					JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   double x;
   double y;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   func(eo, &x, &y);
   return elixir_new_evas_position(cx, x, y, &(JS_RVAL(cx, vp)));
}

FAST_CALL_PARAMS(evas_object_size_hint_align_get, elixir_evas_position_evas_object_params);
FAST_CALL_PARAMS(evas_object_size_hint_weight_get, elixir_evas_position_evas_object_params);

static JSBool
elixir_evas_object_smart_smart_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   Evas_Smart *es;
   JSClass *evas_smart_class;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_smart_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   es = evas_object_smart_smart_get(eo);
   evas_smart_class = elixir_class_request("evas_smart", NULL);
   elixir_return_ptr(cx, vp, es, evas_smart_class);
   return JS_TRUE;
}

static JSBool
elixir_evas_object_line_xy_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *evas_line;
   JSObject *js_obj;
   JSClass *evas_line_coord_class;
   jsval propertie;
   int x1;
   int y1;
   int x2;
   int y2;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_line_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas_line);

   evas_object_line_xy_get(evas_line, &x1, &y1, &x2, &y2);
   evas_line_coord_class = elixir_class_request("evas_line_coord", NULL);

   js_obj = JS_NewObject(cx, evas_line_coord_class, NULL, NULL);
   if (!js_obj)
     return JS_FALSE;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(js_obj));

   propertie = INT_TO_JSVAL(x1);
   JS_SetProperty(cx, js_obj, "x1", &propertie);

   propertie = INT_TO_JSVAL(y1);
   JS_SetProperty(cx, js_obj, "y1", &propertie);

   propertie = INT_TO_JSVAL(x2);
   JS_SetProperty(cx, js_obj, "x2", &propertie);

   propertie = INT_TO_JSVAL(y2);
   JS_SetProperty(cx, js_obj, "y2", &propertie);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_del(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE_S(cx, val[0].v.obj, know);

   if (know) evas_object_del(know);
   JS_SetPrivate(cx, val[0].v.obj, JSVAL_NULL);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_params(void (*func)(Evas_Object *obj),
                          const elixir_parameter_t *params[],
                          JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   func(know);

   return JS_TRUE;
}

static JSBool
elixir_string_const_evas_object_params(const char* (*func)(const Evas_Object *obj),
				       const elixir_parameter_t *params[],
				       JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   const char *str;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   str = func(know);

   if (strncmp(str, "elixir/", 7) == 0)
     str = strdupa(str + 7);

   elixir_return_str(cx, vp, str);
   return JS_TRUE;
}

static JSBool
elixir_object_evas_object_params(Evas_Object* (*func)(const Evas_Object *obj),
                                 const elixir_parameter_t *params[],
                                 JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   Evas_Object *result = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   result = func(know);

   return evas_object_to_jsval(cx, result, &(JS_RVAL(cx, vp)));
}

static int
_elixir_evas_object_text_style_get(const Evas_Object *obj)
{
   return (int) evas_object_text_style_get(obj);
}

static int
_elixir_evas_object_render_op_get(const Evas_Object *obj)
{
   return (int) evas_object_render_op_get(obj);
}

static int
_elixir_evas_object_layer_get(const Evas_Object *obj)
{
   return (int) evas_object_layer_get(obj);
}

static int
_elixir_evas_object_image_border_center_fill_get(const Evas_Object *obj)
{
   return (int) evas_object_image_border_center_fill_get(obj);
}

static int
_elixir_evas_object_image_load_error_get(const Evas_Object *obj)
{
   return (int) evas_object_image_load_error_get(obj);
}

static JSBool
elixir_int_evas_object_params(int (*func)(const Evas_Object *obj),
                              const elixir_parameter_t *params[],
                              JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(know)));
   return JS_TRUE;
}

static JSBool
elixir_dbl_evas_object_params(double (*func)(const Evas_Object *obj),
                              const elixir_parameter_t *params[],
                              JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   return JS_NewNumberValue(cx, func(know), &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_bool_evas_object_params(Eina_Bool (*func)(const Evas_Object *obj),
                               const elixir_parameter_t *params[],
                               JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(func(know)));
   return JS_TRUE;
}

static JSBool
elixir_size_evas_object_params(void (*func)(const Evas_Object *obj, int *w, int *h),
                               const elixir_parameter_t *params[],
                               JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   JSObject *ret;
   int w;
   int h;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   func(know, &w, &h);

   ret = elixir_new_size(cx, w, h);
   if (!ret) return JS_FALSE;

   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(ret));
   return JS_TRUE;
}

static JSBool
elixir_evas_object_size_hint_aspect_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   Evas_Aspect_Control aspect;
   int w;
   int h;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   evas_object_size_hint_aspect_get(eo, &aspect, &w, &h);

   return elixir_new_evas_aspect(cx, (int)aspect, w, h, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_border_evas_object_params(void (*func)(const Evas_Object *obj, int *l, int *r, int *t, int *b),
                                 const elixir_parameter_t *params[],
                                 JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   int l;
   int r;
   int t;
   int b;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   func(know, &l, &r, &t, &b);

   return elixir_new_evas_border(cx, l, r, t, b, &(JS_RVAL(cx, vp)));
}


static JSBool
elixir_geometry_evas_object_params(void (*func)(const Evas_Object *obj, int *x, int *y, int *w, int *h),
                                   const elixir_parameter_t *params[],
                                   JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   JSObject *ret;
   int x;
   int y;
   int w;
   int h;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   func(know, &x, &y, &w, &h);

   ret = elixir_new_geometry(cx, x, y, w, h);
   if (!ret) return JS_FALSE;

   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(ret));
   return JS_TRUE;
}

static JSBool
elixir_color_evas_object_params(void (*func)(const Evas_Object *obj, int *r, int *g, int *b, int *a),
                                const elixir_parameter_t *params[],
                                JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   JSObject *ret;
   int r;
   int g;
   int b;
   int a;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   func(know, &r, &g, &b, &a);

   ret = elixir_new_color(cx, r, g, b, a);
   if (!ret)
     return JS_FALSE;

   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(ret));
   return JS_TRUE;
}

static JSBool
elixir_evas_object_evas_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   Evas *evas;
   JSClass *evas_class;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   evas = evas_object_evas_get(know);
   evas_class = elixir_class_request("evas", NULL);

   elixir_return_ptr(cx, vp, evas, evas_class);
   return JS_TRUE;
}

static JSBool
elixir_evas_object_smart_data_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   void *data;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_smart_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   data = evas_object_smart_data_get(know);

   JS_SET_RVAL(cx, vp, elixir_void_get_jsval(data));
   return JS_TRUE;
}

static JSBool
elixir_evas_object_clipees_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   JSObject *array;
   Eina_List *list;
   int i;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   list = (Eina_List*) evas_object_clipees_get(know);
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

   for (i = 0; list; list = eina_list_next(list), ++i)
     {
	Evas_Object *obj;
	jsval js_obj;

	obj = eina_list_data_get(list);

	evas_object_to_jsval(cx, obj, &js_obj);

	if (JS_SetElement(cx, array, i, &js_obj) == JS_FALSE)
	  return JS_FALSE;
     }

   /* FIXME: We have a little memory leak here. */
   return JS_TRUE;
}

static JSBool
elixir_evas_object_smart_members_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   Eina_List *list;
   Eina_List *head;
   JSObject *array;
   int i;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_smart_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   list = (Eina_List*) evas_object_smart_members_get(know);
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

   for (i = 0, head = list; list; list = eina_list_next(list), ++i)
     {
	Evas_Object *obj;
	jsval js_obj;

	obj = eina_list_data_get(list);

	evas_object_to_jsval(cx, obj, &js_obj);
	if (JS_SetElement(cx, array, i, &js_obj) == JS_FALSE)
	  return JS_FALSE;
     }

   eina_list_free(head);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_image_file_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   const char *file;
   const char *key;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_image_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   evas_object_image_file_get(know, &file, &key);

   return elixir_new_evas_file(cx, file, key, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_evas_object_text_font_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   const char *font;
   int size;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_text_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   evas_object_text_font_get(know, &font, &size);

   return elixir_new_evas_font(cx, font, size, &(JS_RVAL(cx, vp)));
}

FAST_CALL_PARAMS_SPEC(evas_object_smart_member_del, elixir_evas_object_params, _evas_object_smart_params);
FAST_CALL_PARAMS_SPEC(evas_object_lower, elixir_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_raise, elixir_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_show, elixir_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_hide, elixir_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_clip_unset, elixir_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_image_reload, elixir_evas_object_params, _evas_object_image_params);
FAST_CALL_PARAMS_SPEC(evas_object_polygon_points_clear, elixir_evas_object_params, _evas_object_polygon_params);
FAST_CALL_PARAMS_SPEC(evas_object_textblock_clear, elixir_evas_object_params, _evas_object_textblock_params);
FAST_CALL_PARAMS_SPEC(evas_object_smart_changed, elixir_evas_object_params, _evas_object_smart_params);
FAST_CALL_PARAMS_SPEC(evas_object_smart_calculate, elixir_evas_object_params, _evas_object_smart_params);

FAST_CALL_PARAMS_SPEC(evas_object_type_get, elixir_string_const_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_name_get, elixir_string_const_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_textblock_text_markup_get, elixir_string_const_evas_object_params, _evas_object_textblock_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_font_source_get, elixir_string_const_evas_object_params, _evas_object_text_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_text_get, elixir_string_const_evas_object_params, _evas_object_text_params);

FAST_CALL_PARAMS_SPEC(evas_object_smart_parent_get, elixir_object_evas_object_params, _evas_object_smart_params);
FAST_CALL_PARAMS_SPEC(evas_object_above_get, elixir_object_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_below_get, elixir_object_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_clip_get, elixir_object_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_map_source_get, elixir_object_evas_object_params, _evas_object_params);

/* No enum in JS. */
FAST_CALL_PARAMS_SPEC_CAST(evas_object_text_style_get, elixir_int_evas_object_params, _evas_object_text_params);
FAST_CALL_PARAMS_SPEC_CAST(evas_object_render_op_get, elixir_int_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC_CAST(evas_object_image_border_center_fill_get, elixir_int_evas_object_params, _evas_object_image_params);

FAST_CALL_PARAMS_SPEC_CAST(evas_object_layer_get, elixir_int_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC_CAST(evas_object_image_load_error_get, elixir_int_evas_object_params, _evas_object_image_params);
FAST_CALL_PARAMS_SPEC(evas_object_image_load_scale_down_get, elixir_int_evas_object_params, _evas_object_image_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_ascent_get, elixir_int_evas_object_params, _evas_object_text_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_descent_get, elixir_int_evas_object_params, _evas_object_text_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_max_ascent_get, elixir_int_evas_object_params, _evas_object_text_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_max_descent_get, elixir_int_evas_object_params, _evas_object_text_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_horiz_advance_get, elixir_int_evas_object_params, _evas_object_text_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_vert_advance_get, elixir_int_evas_object_params, _evas_object_text_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_inset_get, elixir_int_evas_object_params, _evas_object_text_params);

FAST_CALL_PARAMS_SPEC(evas_object_focus_get, elixir_bool_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_visible_get, elixir_bool_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_anti_alias_get, elixir_bool_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_map_enable_get, elixir_bool_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_image_alpha_get, elixir_bool_evas_object_params, _evas_object_image_params);
FAST_CALL_PARAMS_SPEC(evas_object_image_smooth_scale_get, elixir_bool_evas_object_params, _evas_object_image_params);
FAST_CALL_PARAMS_SPEC(evas_object_image_pixels_dirty_get, elixir_bool_evas_object_params, _evas_object_image_params);
FAST_CALL_PARAMS_SPEC(evas_object_pass_events_get, elixir_bool_evas_object_params, _evas_object_image_params);
FAST_CALL_PARAMS_SPEC(evas_object_repeat_events_get, elixir_bool_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_propagate_events_get, elixir_bool_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_smart_need_recalculate_get, elixir_bool_evas_object_params, _evas_object_smart_params);

FAST_CALL_PARAMS_SPEC(evas_object_image_load_dpi_get, elixir_dbl_evas_object_params, _evas_object_image_params);
FAST_CALL_PARAMS_SPEC(evas_object_scale_get, elixir_dbl_evas_object_params, _evas_object_params);

FAST_CALL_PARAMS_SPEC(evas_object_image_size_get, elixir_size_evas_object_params, _evas_object_image_params);
FAST_CALL_PARAMS_SPEC(evas_object_image_load_size_get, elixir_size_evas_object_params, _evas_object_image_params);
FAST_CALL_PARAMS_SPEC(evas_object_textblock_size_formatted_get, elixir_size_evas_object_params, _evas_object_textblock_params);
FAST_CALL_PARAMS_SPEC(evas_object_textblock_size_native_get, elixir_size_evas_object_params, _evas_object_textblock_params);
FAST_CALL_PARAMS_SPEC(evas_object_size_hint_min_get, elixir_size_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_size_hint_max_get, elixir_size_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_size_hint_request_get, elixir_size_evas_object_params, _evas_object_params);

FAST_CALL_PARAMS_SPEC(evas_object_image_border_get, elixir_border_evas_object_params, _evas_object_image_params);
FAST_CALL_PARAMS_SPEC(evas_object_textblock_style_insets_get, elixir_border_evas_object_params, _evas_object_textblock_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_style_pad_get, elixir_border_evas_object_params, _evas_object_text_params);
FAST_CALL_PARAMS_SPEC(evas_object_size_hint_padding_get, elixir_border_evas_object_params, _evas_object_params);

FAST_CALL_PARAMS_SPEC(evas_object_geometry_get, elixir_geometry_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_image_fill_get, elixir_geometry_evas_object_params, _evas_object_image_params);

FAST_CALL_PARAMS_SPEC(evas_object_color_get, elixir_color_evas_object_params, _evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_shadow_color_get, elixir_color_evas_object_params, _evas_object_text_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_glow_color_get, elixir_color_evas_object_params, _evas_object_text_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_glow2_color_get, elixir_color_evas_object_params, _evas_object_text_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_outline_color_get, elixir_color_evas_object_params, _evas_object_text_params);

static JSFunctionSpec     evas_object_params_function[] = {
  ELIXIR_FN(evas_object_smart_changed, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_need_recalculate_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_calculate, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_size_hint_align_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_size_hint_weight_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_size_hint_aspect_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_size_hint_min_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_size_hint_max_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_size_hint_request_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_size_hint_padding_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_scale_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_lower, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_raise, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_show, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_hide, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_clip_unset, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_type_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_name_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_above_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_below_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_clip_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_map_source_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_layer_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_render_op_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_visible_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_anti_alias_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_map_enable_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_geometry_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_color_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_clipees_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_border_center_fill_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_fill_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_size_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_load_error_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_alpha_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_smooth_scale_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_reload, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_pixels_dirty_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_load_size_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_load_scale_down_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_load_dpi_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_polygon_points_clear, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_border_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_textblock_style_insets_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_textblock_text_markup_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_textblock_clear, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_textblock_size_formatted_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_textblock_size_native_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_font_source_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_text_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_ascent_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_descent_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_max_ascent_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_max_descent_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_horiz_advance_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_vert_advance_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_inset_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_style_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_shadow_color_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_glow_color_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_glow2_color_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_outline_color_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_style_pad_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_line_xy_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_evas_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_file_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_font_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_focus_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_pass_events_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_repeat_events_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_propagate_events_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_member_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_parent_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_members_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_data_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_smart_get, 1, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_object_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_object_params_function))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_evas_object_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_object_params_function[i].name)
     JS_DeleteProperty(cx, parent, evas_object_params_function[i++].name);

   return EINA_TRUE;
}

