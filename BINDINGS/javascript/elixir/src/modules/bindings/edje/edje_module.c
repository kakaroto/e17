#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <string.h>
#include <Edje.h>

#include "Elixir.h"

JSBool elixir_edje_box_layout_register(JSContext *cx, uintN argc, jsval *vp);

static elixir_parameter_t               _evas_object_parameter = {
  "Evas_Object", JOBJECT, NULL
};
static elixir_parameter_t               _edje_object_parameter = {
  "Edje_Object", JOBJECT, NULL
};
static elixir_parameter_t               _evas_parameter = {
  "Evas", JOBJECT, NULL
};

static const elixir_parameter_t*        _edje_object_params[2] = {
   &_edje_object_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_string_params[3] = {
   &_edje_object_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_string_2double_params[5] = {
   &_edje_object_parameter,
   &string_parameter,
   &double_parameter,
   &double_parameter,
   NULL
};
static const elixir_parameter_t*	_edje_object_double_params[3] = {
   &_edje_object_parameter,
   &double_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_params[2] = {
   &_evas_parameter,
   NULL
};
static const elixir_parameter_t*        _2strings_params[3] = {
   &string_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _2strings_int_params[4] = {
   &string_parameter,
   &string_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_2int_params[4] = {
   &_edje_object_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_3int_params[5] = {
   &_edje_object_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_int_params[3] = {
   &_edje_object_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_2strings_params[4] = {
   &_edje_object_parameter,
   &string_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_2strings_int_params[5] = {
   &_edje_object_parameter,
   &string_parameter,
   &string_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_string_evas_object_params[4] = {
   &_edje_object_parameter,
   &string_parameter,
   &_evas_object_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_string_evas_object_int_params[5] = {
   &_edje_object_parameter,
   &string_parameter,
   &_evas_object_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_string_2evas_object_params[5] = {
   &_edje_object_parameter,
   &string_parameter,
   &_evas_object_parameter,
   &_evas_object_parameter,
   NULL
};

static const elixir_parameter_t*        _edje_object_evas_object_params[3] = {
   &_edje_object_parameter,
   &_evas_object_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_string_int_params[4] = {
   &_edje_object_parameter,
   &string_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_string_4params[7] = {
   &_edje_object_parameter,
   &string_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_string_12params[15] = {
   &_edje_object_parameter,
   &string_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
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
static const elixir_parameter_t*        _string_12params[14] = {
   &string_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
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
static const elixir_parameter_t*        _edje_object_func_any_params[4] = {
   &_edje_object_parameter,
   &function_parameter,
   &any_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_2strings_func_any_params[6] = {
   &_edje_object_parameter,
   &string_parameter,
   &string_parameter,
   &function_parameter,
   &any_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_2strings_func_params[5] = {
   &_edje_object_parameter,
   &string_parameter,
   &string_parameter,
   &function_parameter,
   NULL
};
static const elixir_parameter_t*        _edje_object_2int_any_params[5] = {
   &_edje_object_parameter,
   &int_parameter,
   &int_parameter,
   &any_parameter,
   NULL
};
static const elixir_parameter_t*	_edje_object_string_evas_object_4int_params[8] = {
  &_edje_object_parameter,
  &string_parameter,
  &_evas_object_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  &int_parameter,
  NULL
};
static const elixir_parameter_t*	_edje_object_string_bool_params[4] = {
  &_edje_object_parameter,
  &string_parameter,
  &boolean_parameter,
  NULL
};

static const struct {
  const char    *evas_name;
  const char    *elixir_name;
} _matching_evas_object[] = {
  { "rectangle", "evas_object_rectangle" },
  { "polygon", "evas_object_polygon" },
  { "textblock", "evas_object_textblock" },
  { "text", "evas_object_text" },
  { "line", "evas_object_line" },
  { "image", "evas_object_image" },
  { "gradient", "evas_object_gradient" },
  { "edje", "edje_object" },
  { NULL, NULL }
};

const char*
_elixir_evas_object_to_elixir_object(Evas_Object *obj)
{
   const char   *evas_name;
   unsigned int  i;

   if (obj == NULL)
     return "evas_object";

   evas_name = evas_object_type_get(obj);

   for (i = 0; _matching_evas_object[i].evas_name != NULL; ++i)
     if (strcmp(_matching_evas_object[i].evas_name, evas_name) == 0)
       return _matching_evas_object[i].elixir_name;

   return evas_name;
}

static void
_elixir_evas_object(void *data, Evas *e, Evas_Object *obj, void *event_info)
{
   JSContext *cx;
   JSObject *js_obj;
   jsval *tmp;

   (void) data;
   (void) e;
   (void) event_info;

   tmp = evas_object_data_del(obj, "elixir_jsval");
   if (!tmp) return ;

   cx = evas_object_event_callback_del(obj, EVAS_CALLBACK_DEL, _elixir_evas_object);

   js_obj = JSVAL_TO_OBJECT(*tmp);
   if (!js_obj) return ;
   JS_SetPrivate(cx, js_obj, NULL);

   elixir_rval_delete(cx, tmp);
   free(tmp);
}

jsval
_elixir_evas_object_to_jsval(JSContext *cx, Evas_Object *obj)
{
   JSClass *evas_object_class;
   JSObject *jo;
   jsval *tmp;

   if (!obj) return JSVAL_NULL;

   tmp = evas_object_data_get(obj, "elixir_jsval");
   if (tmp) return *tmp;

   evas_object_class = elixir_class_request(_elixir_evas_object_to_elixir_object(obj), "evas_object");

   jo = elixir_build_ptr(cx, obj, evas_object_class);

   tmp = malloc(sizeof (jsval));
   if (!tmp) return JSVAL_NULL;

   *tmp = OBJECT_TO_JSVAL(jo);
   if (!elixir_rval_register(cx, tmp))
     {
	free(tmp);
	return JSVAL_NULL;
     }

   evas_object_data_set(obj, "elixir_jsval", tmp);
   evas_object_event_callback_add(obj, EVAS_CALLBACK_DEL, _elixir_evas_object, cx);

   return *tmp;
}

FAST_CALL_PARAMS(edje_init, elixir_int_params_void);
FAST_CALL_PARAMS(edje_shutdown, elixir_int_params_void);
FAST_CALL_PARAMS(edje_file_cache_get, elixir_int_params_void);
FAST_CALL_PARAMS(edje_collection_cache_get, elixir_int_params_void);

FAST_CALL_PARAMS(edje_file_cache_flush, elixir_void_params_void);
FAST_CALL_PARAMS(edje_collection_cache_flush, elixir_void_params_void);
FAST_CALL_PARAMS(edje_freeze, elixir_void_params_void);
FAST_CALL_PARAMS(edje_thaw, elixir_void_params_void);
FAST_CALL_PARAMS(edje_message_signal_process, elixir_void_params_void);

static JSBool
elixir_double_params_void(double (*func)(void),
			  JSContext *cx, uintN argc, jsval *vp)
{
   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   return JS_NewNumberValue(cx, func(), &(JS_RVAL(cx, vp)));
}

FAST_CALL_PARAMS(edje_frametime_get, elixir_double_params_void);
FAST_CALL_PARAMS(edje_scale_get, elixir_double_params_void);

static JSBool
elixir_edje_fontset_append_get(JSContext *cx, uintN argc, jsval *vp)
{
   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   elixir_return_str(cx, vp, edje_fontset_append_get());
   return JS_TRUE;
}

static JSBool
elixir_void_params_double(void (*func)(double),
			  JSContext *cx, uintN argc, jsval *vp)
{
   double t;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, double_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   t = val[0].v.dbl;
   func(t);

   return JS_TRUE;
}

FAST_CALL_PARAMS(edje_frametime_set, elixir_void_params_double);
FAST_CALL_PARAMS(edje_scale_set, elixir_void_params_double);

static JSBool
elixir_void_params_char(void (*func)(const char *params),
                        JSContext *cx, uintN argc, jsval *vp)
{
   const char *params;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   params = elixir_get_string_bytes(val[0].v.str);
   func(params);

   return JS_TRUE;
}

FAST_CALL_PARAMS(edje_fontset_append_set, elixir_void_params_char);
FAST_CALL_PARAMS(edje_color_class_del, elixir_void_params_char);
FAST_CALL_PARAMS(edje_text_class_del, elixir_void_params_char);

static JSBool
elixir_void_params_int(void (*func)(int count),
                       JSContext *cx, uintN argc, jsval *vp)
{
   int count;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   count = val[0].v.num;

   func(count);

   return JS_TRUE;
}

FAST_CALL_PARAMS(edje_file_cache_set, elixir_void_params_int);
FAST_CALL_PARAMS(edje_collection_cache_set, elixir_void_params_int);

static JSBool
elixir_edje_file_group_exists(JSContext *cx, uintN argc, jsval *vp)
{
   const char *glob;
   char *file;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _2strings_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   file = elixir_file_canonicalize(elixir_get_string_bytes(val[0].v.str));
   glob = elixir_get_string_bytes(val[1].v.str);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(edje_file_group_exists(file, glob)));

   free(file);
   return JS_TRUE;
}

static JSBool
elixir_edje_file_data_get(JSContext *cx, uintN argc, jsval *vp)
{
   const char *key;
   char *file;
   char *result;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _2strings_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   file = elixir_file_canonicalize(elixir_get_string_bytes(val[0].v.str));
   key = elixir_get_string_bytes(val[1].v.str);

   result = edje_file_data_get(file, key);
   elixir_return_str(cx, vp, result);

   free(result);
   free(file);

   return JS_TRUE;
}

static JSBool
elixir_edje_file_collection_list(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List *lst;
   JSObject *array;
   char *file;
   int index;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   file = elixir_file_canonicalize(elixir_get_string_bytes(val[0].v.str));

   array = elixir_return_array(cx, vp);
   if (!array)
     return JS_FALSE;

   lst = edje_file_collection_list(file);

   for (index = 0; lst; lst = eina_list_next(lst), ++index)
     {
        const char *dt;
        JSString *str;

        dt = eina_list_data_get(lst);
        str = elixir_ndup(cx, dt, strlen(dt));
        JS_DefineElement(cx, array, index, STRING_TO_JSVAL(str), NULL,
                         NULL, JSPROP_INDEX | JSPROP_ENUMERATE | JSPROP_READONLY);
     }

   edje_file_collection_list_free(lst);
   free(file);
   return JS_TRUE;
}

static JSBool
elixir_edje_text_class_list(JSContext *cx, uintN argc, jsval *vp)
{
   const char *dt;
   Eina_List *lst;
   JSObject *array;
   int index = 0;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   array = elixir_return_array(cx, vp);
   if (!array)
     return JS_FALSE;

   lst = edje_text_class_list();

   EINA_LIST_FREE(lst, dt)
     {
        JSString *str;

        str = elixir_ndup(cx, dt, eina_stringshare_strlen(dt));
        JS_DefineElement(cx, array, index++, STRING_TO_JSVAL(str), NULL,
                         NULL, JSPROP_INDEX | JSPROP_ENUMERATE | JSPROP_READONLY);
        eina_stringshare_del(dt);
     }

   return JS_TRUE;
}

static JSBool
elixir_edje_color_class_list(JSContext *cx, uintN argc, jsval *vp)
{
   Eina_List *lst;
   JSObject *array;
   char *dt;
   int index = 0;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   array = elixir_return_array(cx, vp);
   if (!array)
     return JS_FALSE;

   lst = edje_color_class_list();

   EINA_LIST_FREE(lst, dt)
     {
        JSString *str;

        str = elixir_ndup(cx, dt, strlen(dt));
        JS_DefineElement(cx, array, index, STRING_TO_JSVAL(str), NULL,
                         NULL, JSPROP_INDEX | JSPROP_ENUMERATE | JSPROP_READONLY);
        free(dt);
     }

   return JS_TRUE;
}

static JSBool
elixir_edje_text_class_set(JSContext *cx, uintN argc, jsval *vp)
{
   const char *text_class;
   const char *font;
   Evas_Font_Size size;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _2strings_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   text_class = elixir_get_string_bytes(val[0].v.str);
   font = elixir_get_string_bytes(val[1].v.str);
   size = val[2].v.num;

   edje_text_class_set(text_class, font, size);

   return JS_TRUE;
}

static JSBool
elixir_void_params_edje_object_2int(void (*func)(Evas_Object *obj, Evas_Coord w, Evas_Coord h),
                                    JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *obj;
   Evas_Coord w;
   Evas_Coord h;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _edje_object_2int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, obj);
   w = val[1].v.num;
   h = val[2].v.num;

   func(obj, w, h);

   return JS_TRUE;
}

FAST_CALL_PARAMS(edje_extern_object_min_size_set, elixir_void_params_edje_object_2int);
FAST_CALL_PARAMS(edje_extern_object_max_size_set, elixir_void_params_edje_object_2int);

static JSBool
elixir_edje_extern_object_aspect_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   Edje_Aspect_Control aspect;
   Evas_Coord aw;
   Evas_Coord ah;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _edje_object_3int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   aspect = val[1].v.num;
   aw = val[2].v.num;
   ah = val[3].v.num;

   edje_extern_object_aspect_set(eo, aspect, aw, ah);

   return JS_TRUE;
}

static JSBool
elixir_edje_object_size_min_restricted_calc(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   Evas_Coord minw;
   Evas_Coord minh;
   Evas_Coord restrictedw;
   Evas_Coord restrictedh;
   elixir_value_t val[3];
   JSObject *js_obj;

   if (!elixir_params_check(cx, _edje_object_2int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   restrictedw = val[1].v.num;
   restrictedh = val[2].v.num;

   edje_object_size_min_restricted_calc(eo, &minw, &minh, restrictedw, restrictedh);

   js_obj = elixir_new_size(cx, minw, minh);
   if (!js_obj)
     return JS_FALSE;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(js_obj));

   return JS_TRUE;
}

static JSBool
elixir_edje_object_add(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   Evas *e;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, e);

   eo = edje_object_add(e);

   elixir_return_ptr(cx, vp, eo, elixir_class_request("edje_object", "evas_object"));
   return JS_TRUE;
}

static JSBool
elixir_edje_object_part_table_col_row_size_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *part;
   int cols;
   int rows;
   Eina_Bool result;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _edje_object_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);

   result = edje_object_part_table_col_row_size_get(eo, part, &cols, &rows);

   if (result)
     {
	JSObject *jo;

	jo = JS_NewObject(cx, elixir_class_request("edje_table_size", "evas_table_size"), NULL, NULL);
	if (!jo) return JS_FALSE;

	if (!elixir_object_register(cx, &jo, NULL)) return JS_FALSE;
	elixir_add_int_prop(cx, jo, "cols", cols);
	elixir_add_int_prop(cx, jo, "rows", rows);

	JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(jo));

	elixir_object_unregister(cx, &jo);
     }
   else
     JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(0));
   return JS_TRUE;
}

static void
_elixir_edje_object_color_class_del(const Evas_Object* obj, const char* name)
{
   return edje_object_color_class_del((Evas_Object *)obj, name);
}

static JSBool
elixir_void_params_edje_object_string(void (*func)(const Evas_Object*, const char*),
				      JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *str;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _edje_object_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   str = elixir_get_string_bytes(val[1].v.str);

   func(eo, str);

   return JS_TRUE;
}

FAST_CALL_PARAMS_CAST(edje_object_color_class_del, elixir_void_params_edje_object_string);
FAST_CALL_PARAMS(edje_object_part_text_select_none, elixir_void_params_edje_object_string);
FAST_CALL_PARAMS(edje_object_part_text_select_all, elixir_void_params_edje_object_string);

static JSBool
elixir_string_params_edje_object_string_ret(JSContext *cx, uintN argc, jsval *vp,
                                            Evas_Object** eo, const char** param)
{
   elixir_value_t       val[2];

   *param = NULL;
   *eo = NULL;

   if (!elixir_params_check(cx, _edje_object_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, *eo);
   *param = elixir_get_string_bytes(val[1].v.str);

   return JS_TRUE;
}

static JSBool
elixir_string_params_edje_object_string(const char* (*func)(const Evas_Object *obj, const char *params),
                                        JSContext* cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *params;
   const char *ret;

   if (!elixir_string_params_edje_object_string_ret(cx, argc, vp, &eo, &params))
     return JS_FALSE;

   ret = func(eo, params);

   elixir_return_str(cx, vp, ret);
   return JS_TRUE;
}

FAST_CALL_PARAMS(edje_object_data_get, elixir_string_params_edje_object_string);
FAST_CALL_PARAMS(edje_object_part_text_get, elixir_string_params_edje_object_string);
FAST_CALL_PARAMS(edje_object_part_text_selection_get, elixir_string_params_edje_object_string);

static Evas_Object*
_elixir_edje_object_part_object_get(const Evas_Object *obj, const char *part)
{
   return (Evas_Object*)edje_object_part_object_get(obj, part);
}

static JSBool
elixir_evas_object_params_edje_object_string(Evas_Object* (*func)(const Evas_Object *obj, const char *part),
                                             JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *ret;
   Evas_Object *eo;
   const char *part;

   if (!elixir_string_params_edje_object_string_ret(cx, argc, vp, &eo, &part))
     return JS_FALSE;

   ret = func(eo, part);

   JS_SET_RVAL(cx, vp, _elixir_evas_object_to_jsval(cx, ret));
   return JS_TRUE;
}

FAST_CALL_PARAMS_CAST(edje_object_part_object_get, elixir_evas_object_params_edje_object_string);
FAST_CALL_PARAMS(edje_object_part_swallow_get, elixir_evas_object_params_edje_object_string);

static JSBool
elixir_int_params_edje_object_string(int (*func)(const Evas_Object *obj, const char *part),
                                     JSContext* cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *part;
   int ret;

   if (!elixir_string_params_edje_object_string_ret(cx, argc, vp, &eo, &part))
     return JS_FALSE;

   ret = func(eo, part);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ret));
   return JS_TRUE;
}

FAST_CALL_PARAMS(edje_object_part_drag_dir_get, elixir_int_params_edje_object_string);
FAST_CALL_PARAMS(edje_object_part_exists, elixir_int_params_edje_object_string);

static int
_elixir_edje_object_load_error_get(Evas_Object *obj)
{
   return edje_object_load_error_get((const Evas_Object *)obj);
}

static int
_elixir_edje_object_play_get(Evas_Object *obj)
{
   return edje_object_play_get((const Evas_Object *)obj);
}

static int
_elixir_edje_object_animation_get(Evas_Object *obj)
{
  return edje_object_animation_get((const Evas_Object *)obj);
}

static JSBool
elixir_int_params_edje_object(int (*func)(Evas_Object *obj),
                              JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _edje_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(eo)));
   return JS_TRUE;
}

FAST_CALL_PARAMS_CAST(edje_object_load_error_get, elixir_int_params_edje_object);
FAST_CALL_PARAMS_CAST(edje_object_play_get, elixir_int_params_edje_object);
FAST_CALL_PARAMS_CAST(edje_object_animation_get, elixir_int_params_edje_object);
FAST_CALL_PARAMS(edje_object_freeze, elixir_int_params_edje_object);
FAST_CALL_PARAMS(edje_object_thaw, elixir_int_params_edje_object);

static JSBool
elixir_void_params_edje_object(void (*func)(Evas_Object *obj),
                               JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _edje_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   func(eo);

   return JS_TRUE;
}

FAST_CALL_PARAMS(edje_object_calc_force, elixir_void_params_edje_object);
FAST_CALL_PARAMS(edje_object_message_signal_process, elixir_void_params_edje_object);

static JSBool
elixir_void_params_edje_object_int(void (*func)(Evas_Object *obj, int play),
                                   JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _edje_object_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   func(eo, val[1].v.num);

   return JS_TRUE;
}

FAST_CALL_PARAMS(edje_object_play_set, elixir_void_params_edje_object_int);
FAST_CALL_PARAMS(edje_object_animation_set, elixir_void_params_edje_object_int);

static JSBool
elixir_edje_object_file_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *part;
   char *file;
   int ret;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _edje_object_2strings_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   file = elixir_file_canonicalize(elixir_get_string_bytes(val[1].v.str));
   part = elixir_get_string_bytes(val[2].v.str);

   ret = edje_object_file_set(eo, file, part);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(ret));
   free(file);
   return JS_TRUE;
}

static JSBool
elixir_void_params_edje_object_2strings(void (*func)(Evas_Object *obj, const char *p1, const char *p2),
                                        JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *part;
   const char *text;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _edje_object_2strings_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);
   text = elixir_get_string_bytes(val[2].v.str);

   func(eo, part, text);

   return JS_TRUE;
}

FAST_CALL_PARAMS(edje_object_part_text_set, elixir_void_params_edje_object_2strings);
FAST_CALL_PARAMS(edje_object_signal_emit, elixir_void_params_edje_object_2strings);
FAST_CALL_PARAMS(edje_object_part_text_insert, elixir_void_params_edje_object_2strings);

static JSBool
elixir_edje_object_part_text_anchor_geometry_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Rectangle *geom;
   const Eina_List *geoms;
   const Eina_List *l;
   const char *anchor;
   const char *part;
   Evas_Object *eo;
   JSObject *array;
   int index;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _edje_object_2strings_params, val, argc, JS_ARGV(cx, vp)))
     return JS_TRUE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);
   anchor = elixir_get_string_bytes(val[2].v.str);

   geoms = edje_object_part_text_anchor_geometry_get(eo, part, anchor);

   array = elixir_return_array(cx, vp);
   if (!array) return JS_FALSE;

   index = 0;
   EINA_LIST_FOREACH(geoms, l, geom)
     {
	JSObject *obj;

	obj = elixir_new_geometry(cx, geom->x, geom->y, geom->w, geom->h);
	JS_DefineElement(cx, array, index++, OBJECT_TO_JSVAL(obj), NULL,
			 NULL, JSPROP_INDEX | JSPROP_ENUMERATE);
     }

   return JS_TRUE;
}

static JSBool
elixir_void_params_edje_object_string_2double(void (*func)(Evas_Object *obj, const char *part, double d1, double d2),
                                              JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *part;
   double d1;
   double d2;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _edje_object_string_2double_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);
   d1 = val[2].v.dbl;
   d2 = val[3].v.dbl;

   func(eo, part, d1, d2);

   return JS_TRUE;
}

FAST_CALL_PARAMS(edje_object_part_drag_value_set, elixir_void_params_edje_object_string_2double);
FAST_CALL_PARAMS(edje_object_part_drag_step_set, elixir_void_params_edje_object_string_2double);
FAST_CALL_PARAMS(edje_object_part_drag_page_set, elixir_void_params_edje_object_string_2double);
FAST_CALL_PARAMS(edje_object_part_drag_step, elixir_void_params_edje_object_string_2double);
FAST_CALL_PARAMS(edje_object_part_drag_page, elixir_void_params_edje_object_string_2double);
FAST_CALL_PARAMS(edje_object_part_drag_size_set, elixir_void_params_edje_object_string_2double);

static JSBool
elixir_edje_object_text_class_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *text_class;
   const char *font;
   Evas_Font_Size size;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _edje_object_2strings_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   text_class = elixir_get_string_bytes(val[1].v.str);
   font = elixir_get_string_bytes(val[2].v.str);
   size = val[3].v.num;

   edje_object_text_class_set(eo, text_class, font, size);

   return JS_TRUE;
}

static JSBool
elixir_bool_params_edje_object_string_evas_object(Eina_Bool (*func)(Evas_Object*, const char*, Evas_Object*),
						  JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *part;
   Evas_Object *child;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _edje_object_string_evas_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);
   GET_PRIVATE(cx, val[2].v.obj, child);

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(func(eo, part, child)));
   return JS_TRUE;
}

FAST_CALL_PARAMS(edje_object_part_box_append, elixir_bool_params_edje_object_string_evas_object);
FAST_CALL_PARAMS(edje_object_part_box_prepend, elixir_bool_params_edje_object_string_evas_object);
FAST_CALL_PARAMS(edje_object_part_table_unpack, elixir_bool_params_edje_object_string_evas_object);

static JSBool
elixir_edje_object_part_table_pack(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   Evas_Object *child;
   const char *part;
   unsigned short col;
   unsigned short row;
   unsigned short colspan;
   unsigned short rowspan;
   Eina_Bool result;
   elixir_value_t val[7];

   if (!elixir_params_check(cx, _edje_object_string_evas_object_4int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);
   GET_PRIVATE(cx, val[2].v.obj, child);
   col = val[3].v.num;
   row = val[4].v.num;
   colspan = val[5].v.num;
   rowspan = val[6].v.num;

   result = edje_object_part_table_pack(eo, part, child, col, row, colspan, rowspan);

   JS_SET_RVAL(cx, vp, result);
   return JS_TRUE;
}

static JSBool
elixir_edje_object_part_box_remove(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *child;
   Evas_Object *ret;
   const char *part;
   Evas_Object *eo;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _edje_object_string_evas_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);
   GET_PRIVATE(cx, val[2].v.obj, child);

   ret = edje_object_part_box_remove(eo, part, child);

   JS_SET_RVAL(cx, vp, _elixir_evas_object_to_jsval(cx, ret));
   return JS_TRUE;
}

static JSBool
elixir_edje_object_part_table_clear(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *part;
   Eina_Bool bool;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _edje_object_string_bool_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);
   bool = val[2].v.bol;

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(edje_object_part_table_clear(eo, part, bool)));
   return JS_TRUE;
}

static JSBool
elixir_edje_object_part_swallow(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *swallow;
   Evas_Object *eo;
   const char *part;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _edje_object_string_evas_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);
   GET_PRIVATE(cx, val[2].v.obj, swallow);

   edje_object_part_swallow(eo, part, swallow);

   return JS_TRUE;
}

static JSBool
elixir_edje_object_part_box_insert_at(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *child;
   Evas_Object *eo;
   const char *part;
   unsigned int pos;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _edje_object_string_evas_object_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);
   GET_PRIVATE(cx, val[2].v.obj, child);
   pos = val[3].v.num;

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(edje_object_part_box_insert_at(eo, part, child, pos)));

   return JS_TRUE;
}

static JSBool
elixir_edje_object_part_box_insert_before(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   Evas_Object *child;
   Evas_Object *reference;
   const char *part;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _edje_object_string_2evas_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);
   GET_PRIVATE(cx, val[2].v.obj, child);
   GET_PRIVATE(cx, val[3].v.obj, reference);

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(edje_object_part_box_insert_before(eo, part, child, reference)));

   return JS_TRUE;
}

static JSBool
elixir_edje_object_part_unswallow(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *swallow;
   Evas_Object *eo;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _edje_object_evas_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   GET_PRIVATE(cx, val[1].v.obj, swallow);

   edje_object_part_unswallow(eo, swallow);

   return JS_TRUE;
}

static JSBool
elixir_edje_object_scale_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   double scale;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _edje_object_double_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   scale = val[1].v.dbl;

   edje_object_scale_set(eo, scale);

   return JS_TRUE;
}

static JSBool
elixir_edje_object_scale_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   double scale;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _edje_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   scale = edje_object_scale_get(eo);

   return JS_NewNumberValue(cx, scale, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_edje_object_part_geometry_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *part;
   JSObject *js_obj;
   Evas_Coord x;
   Evas_Coord y;
   Evas_Coord w;
   Evas_Coord h;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _edje_object_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);

   edje_object_part_geometry_get(eo, part, &x, &y, &w, &h);

   js_obj = elixir_new_geometry(cx, x, y, w, h);
   if (!js_obj)
     return JS_FALSE;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(js_obj));
   return JS_TRUE;
}

static JSBool
elixir_edje_object_file_get(JSContext *cx, uintN argc, jsval *vp)
{
   const char *file;
   const char *part;
   Evas_Object *eo;
   JSObject *js_obj;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _edje_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);

   edje_object_file_get(eo, &file, &part);

   js_obj = JS_NewObject(cx, elixir_class_request("edje_file", NULL), NULL, NULL);
   if (!js_obj)
     return JS_FALSE;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(js_obj));

   elixir_add_str_prop(cx, js_obj, "file", file);
   elixir_add_str_prop(cx, js_obj, "part", part);

   return JS_TRUE;
}

static JSBool
elixir_p1p2_params_edje_object_string_2double(const char *p1, const char *p2,
                                              void (*func)(const Evas_Object *obj, const char *part, double *p1, double *p2),
                                              JSContext *cx, uintN argc, jsval *vp)
{
   const char *part;
   JSObject *js_obj;
   Evas_Object *eo;
   double d1;
   double d2;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _edje_object_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_TRUE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);

   func(eo, part, &d1, &d2);

   js_obj = JS_NewObject(cx, elixir_class_request("edje_drag", NULL), NULL, NULL);
   if (!js_obj)
     return JS_FALSE;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(js_obj));

   elixir_add_str_prop(cx, js_obj, "part", part);
   elixir_add_int_prop(cx, js_obj, p1, d1);
   elixir_add_int_prop(cx, js_obj, p2, d2);

   return JS_TRUE;
}

static JSBool
elixir_dxdy_params_edje_object_string_2double(void (*func)(const Evas_Object *obj, const char *part, double *dx, double *dy),
                                              JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_p1p2_params_edje_object_string_2double("dx", "dy", func, cx, argc, vp);
}

FAST_CALL_PARAMS(edje_object_part_drag_value_get, elixir_dxdy_params_edje_object_string_2double);
FAST_CALL_PARAMS(edje_object_part_drag_step_get, elixir_dxdy_params_edje_object_string_2double);
FAST_CALL_PARAMS(edje_object_part_drag_page_get, elixir_dxdy_params_edje_object_string_2double);

static JSBool
elixir_edje_object_part_drag_size_get(JSContext *cx, uintN argc, jsval *vp)
{
   return elixir_p1p2_params_edje_object_string_2double("dx", "dy", edje_object_part_drag_size_get, cx, argc, vp);
}

static JSBool
elixir_edje_object_part_state_get(JSContext *cx, uintN argc, jsval *vp)
{
   const char *part;
   const char *ret;
   JSObject *js_obj;
   Evas_Object *eo;
   double val_ret;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _edje_object_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);

   ret = edje_object_part_state_get(eo, part, &val_ret);

   js_obj = JS_NewObject(cx, elixir_class_request("edje_state", NULL), NULL, NULL);
   if (!js_obj)
     return JS_FALSE;
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(js_obj));

   elixir_add_str_prop(cx, js_obj, "state", ret);
   elixir_add_dbl_prop(cx, js_obj, "val_ret", val_ret);

   return JS_TRUE;
}

static JSBool
elixir_edje_object_part_text_anchor_list_get(JSContext *cx, uintN argc, jsval *vp)
{
   const Eina_List *texts;
   const Eina_List *l;
   const char *part;
   const char *sel;
   Evas_Object *eo;
   JSObject *array;
   int index;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _edje_object_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);

   texts = edje_object_part_text_anchor_list_get(eo, part);

   array = elixir_return_array(cx, vp);
   if (!array) return JS_FALSE;

   index = 0;
   EINA_LIST_FOREACH(texts, l, sel)
     {
	JSString *str;

	str = elixir_ndup(cx, sel, strlen(sel));
	JS_DefineElement(cx, array, index, STRING_TO_JSVAL(str), NULL,
			 NULL, JSPROP_INDEX | JSPROP_ENUMERATE | JSPROP_READONLY);
     }

   return JS_TRUE;
}

static void
_elixir_edje_object_size_min_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)
{
   return edje_object_size_min_get((const Evas_Object *)obj, w, h);
}

static void
_elixir_edje_object_size_max_get(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h)
{
   return edje_object_size_max_get((const Evas_Object *)obj, w, h);
}

static JSBool
elixir_2evas_coord_params_edje_object(void (*func)(Evas_Object *obj, Evas_Coord *w, Evas_Coord *h),
                                      JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   JSObject *js_obj;
   Evas_Coord ecw;
   Evas_Coord ech;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _edje_object_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   func(eo, &ecw, &ech);

   js_obj = elixir_new_size(cx, ecw, ech);
   if (!js_obj) return JS_FALSE;

   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(js_obj));
   return JS_TRUE;
}

FAST_CALL_PARAMS_CAST(edje_object_size_min_get, elixir_2evas_coord_params_edje_object);
FAST_CALL_PARAMS(edje_object_size_min_calc, elixir_2evas_coord_params_edje_object);
FAST_CALL_PARAMS_CAST(edje_object_size_max_get, elixir_2evas_coord_params_edje_object);

static JSBool
elixir_edje_object_part_box_remove_all(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *part;
   int clear;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _edje_object_string_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);
   clear = val[2].v.num;

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(edje_object_part_box_remove_all(eo, part, clear)));
   return JS_TRUE;
}

static JSBool
elixir_edje_object_part_box_remove_at(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   Evas_Object *ret;
   const char *part;
   unsigned int	pos;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _edje_object_string_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);
   pos = val[2].v.num;

   ret = edje_object_part_box_remove_at(eo, part, pos);

   JS_SET_RVAL(cx, vp, _elixir_evas_object_to_jsval(cx, ret));
   return JS_TRUE;
}

static JSBool
elixir_edje_object_part_text_cursor_geometry_get(JSContext *cx, uintN argc, jsval *vp)
{
   const char *part;
   Evas_Object *eo;
   JSObject *js_obj;
   int x, y, w, h;
   elixir_value_t val[6];

   if (!elixir_params_check(cx, _edje_object_string_4params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   part = elixir_get_string_bytes(val[1].v.str);

   edje_object_part_text_cursor_geometry_get(eo, part, &x, &y, &w, &h);

   js_obj = elixir_new_geometry(cx, x, y, w, h);
   if (!js_obj)
     return JS_FALSE;

   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(js_obj));
   return JS_TRUE;
}

static JSBool
elixir_edje_object_color_class_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *color_class;
   int r, g, b, a;
   int r2, g2, b2, a2;
   int r3, g3, b3, a3;
   elixir_value_t val[14];

   if (!elixir_params_check(cx, _edje_object_string_12params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   color_class = elixir_get_string_bytes(val[1].v.str);

   r = val[2].v.num;
   g = val[3].v.num;
   b = val[4].v.num;
   a = val[5].v.num;

   r2 = val[6].v.num;
   g2 = val[7].v.num;
   b2 = val[8].v.num;
   a2 = val[9].v.num;

   r3 = val[10].v.num;
   g3 = val[11].v.num;
   b3 = val[12].v.num;
   a3 = val[13].v.num;

   edje_object_color_class_set(eo, color_class, r, g, b, a, r2, g2, b2, a2, r3, g3, b3, a3);

   return JS_TRUE;
}

static JSBool
elixir_edje_color_class_set(JSContext *cx, uintN argc, jsval *vp)
{
   const char *color_class;
   int r, g, b, a;
   int r2, g2, b2, a2;
   int r3, g3, b3, a3;
   elixir_value_t val[13];

   if (!elixir_params_check(cx, _string_12params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   color_class = elixir_get_string_bytes(val[0].v.str);

   r = val[1].v.num;
   g = val[2].v.num;
   b = val[3].v.num;
   a = val[4].v.num;

   r2 = val[5].v.num;
   g2 = val[6].v.num;
   b2 = val[7].v.num;
   a2 = val[8].v.num;

   r3 = val[9].v.num;
   g3 = val[10].v.num;
   b3 = val[11].v.num;
   a3 = val[12].v.num;

   edje_color_class_set(color_class, r, g, b, a, r2, g2, b2, a2, r3, g3, b3, a3);

   return JS_FALSE;
}

static void
_elixir_edje_object_text_change_cb(void* data, Evas_Object* obj, const char* part)
{
   JSObject*    parent;
   JSContext*   cx;
   JSFunction*  cb;
   JSString*    str = NULL;
   jsval        js_return;
   jsval        argv[3];

   cb = elixir_void_get_private(data);
   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);

   if (!cx || !parent || !cb)
     return ;

   elixir_function_start(cx);

   if (!elixir_rval_new(cx, elixir_class_request("edje_object", "evas_object"), obj, argv + 1))
     goto on_firt_error;

   str = elixir_ndup(cx, part, strlen(part));
   if (!elixir_string_register(cx, &str))
     goto on_error;

   argv[0] = elixir_void_get_jsval(data);
   argv[2] = STRING_TO_JSVAL(str);

   elixir_function_run(cx, cb, parent, 3, argv, &js_return);

 on_error:
   elixir_rval_delete(cx, argv + 1);
   elixir_string_unregister(cx, &str);

 on_firt_error:
   elixir_function_stop(cx);
}

static JSBool
elixir_edje_object_text_change_cb_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   void *data;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _edje_object_func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   /* This will leak. No way to deallocte this safely. */
   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, val[1].v.fct);

   edje_object_text_change_cb_set(eo, _elixir_edje_object_text_change_cb, data);

   return JS_TRUE;
}

static void
_elixir_edje_object_signal_cb(void* data, Evas_Object* obj,
                              const char* emission, const char* source)
{
   JSObject*            parent;
   JSContext*           cx;
   JSFunction*          cb;
   JSString*            jse = NULL;
   JSString*            jss = NULL;
   jsval                js_return;
   jsval                argv[4];

   cb = elixir_void_get_private(data);
   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);
   if (!cx || !parent || !cb)
     return ;

   elixir_function_start(cx);

   if (!elixir_rval_new(cx, elixir_class_request("edje_object", "evas_object"), obj, argv + 1))
     goto on_finish;

   jse = elixir_ndup(cx, emission, strlen(emission));
   if (!elixir_string_register(cx, &jse))
     goto on_error;

   jss = elixir_ndup(cx, source, strlen(source));
   if (!elixir_string_register(cx, &jss))
     goto on_error;

   argv[0] = elixir_void_get_jsval(data);
   argv[2] = STRING_TO_JSVAL(jse);
   argv[3] = STRING_TO_JSVAL(jss);

   elixir_function_run(cx, cb, parent, 4, argv, &js_return);

 on_error:
   elixir_rval_delete(cx, argv + 1);

   elixir_string_unregister(cx, &jss);
   elixir_string_unregister(cx, &jse);

 on_finish:
   elixir_function_stop(cx);
}

static JSBool
elixir_edje_object_signal_callback_add(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *emission;
   const char *source;
   void *data;
   elixir_value_t val[5];

   if (!elixir_params_check(cx, _edje_object_2strings_func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   emission = elixir_get_string_bytes(val[1].v.str);
   source = elixir_get_string_bytes(val[2].v.str);
   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[4].v.any, val[3].v.fct);

   edje_object_signal_callback_add(eo, emission, source, _elixir_edje_object_signal_cb, data);

   return JS_TRUE;
}

static JSBool
elixir_edje_object_signal_callback_del(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   const char *emission;
   const char *source;
   void *data;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _edje_object_2strings_func_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   emission = elixir_get_string_bytes(val[1].v.str);
   source = elixir_get_string_bytes(val[2].v.str);

   data = edje_object_signal_callback_del(eo, emission, source, _elixir_edje_object_signal_cb);

   JS_SET_RVAL(cx, vp, elixir_void_free(data));
   return JS_TRUE;
}

static JSBool
elixir_edje_object_message_send(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *eo;
   JSObject *emsg;
   void *msg = NULL;
   Edje_Message_Type type;
   int id;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _edje_object_2int_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   type = val[1].v.num;
   id = val[2].v.num;

   emsg = NULL;
   if (JSVAL_IS_OBJECT(val[3].v.any) == JS_TRUE)
     JS_ValueToObject(cx, val[3].v.any, &emsg);

   switch (type)
     {
     case EDJE_MESSAGE_NONE:
        break;
     case EDJE_MESSAGE_STRING:
     {
        Edje_Message_String*            ems;
        const char*                     str;

        str = NULL;
        ems = alloca(sizeof (Edje_Message_String));
        if (emsg != NULL)
          if (!elixir_object_get_str(cx, emsg, "str", &str))
            return JS_FALSE;

        if (!str)
          {
             char*                      tmp;

             tmp = elixir_get_string(cx, val[3].v.any);
             if (tmp)
               {
                  str = strdupa(tmp);
                  free(tmp);
               }
          }
        ems->str = (char*) str;
        msg = ems;
        break;
     }
     case EDJE_MESSAGE_INT:
     {
        Edje_Message_Int*               emi;

        emi = alloca(sizeof (Edje_Message_Int));
        if (emsg != NULL)
          {
             if (!elixir_object_get_int(cx, emsg, "val", &emi->val))
               return JS_FALSE;
          }
        else
          emi->val = elixir_get_int(cx, val[3].v.any);

        msg = emi;
        break;
     }
     case EDJE_MESSAGE_FLOAT:
     {
        Edje_Message_Float*             emf;

        emf = alloca(sizeof (Edje_Message_Float));
        if (emsg != NULL)
          {
             if (!elixir_object_get_dbl(cx, emsg, "val", &emf->val))
               return JS_FALSE;
          }
        else
          emf->val = elixir_get_dbl(cx, val[3].v.any);

        msg = emf;
        break;
     }
     case EDJE_MESSAGE_STRING_SET:
     {
        Edje_Message_String_Set*        emss;
        unsigned int                    size;
        unsigned int                    i;

        if (!emsg)
          return JS_FALSE;

        if (!JS_GetArrayLength(cx, emsg, &size))
          return JS_FALSE;

        emss = alloca(sizeof (Edje_Message_String_Set) + (size - 1) * sizeof (char*));
        emss->count = size;
        for (i = 0; i < size; ++i)
          {
             char*      tmp;
             jsval      propertie;

             if (!JS_GetElement(cx, emsg, i, &propertie))
               return JS_FALSE;

             tmp = elixir_get_string(cx, propertie);
             if (tmp)
               {
                  emss->str[i] = strdupa(tmp);
                  free(tmp);
               }
             else
               emss->str[i] = NULL;
          }

        msg = emss;
        break;
     }
     case EDJE_MESSAGE_INT_SET:
     {
        Edje_Message_Int_Set*           emis;
        unsigned int                    size;
        unsigned int                    i;

        if (!emsg)
          return JS_FALSE;

        if (!JS_GetArrayLength(cx, emsg, &size))
          return JS_FALSE;

        emis = alloca(sizeof (Edje_Message_Int_Set) + (size - 1) * sizeof (int));
        emis->count = size;
        for (i = 0; i < size; ++i)
          {
             jsval                      propertie;

             if (!JS_GetElement(cx, emsg, i, &propertie))
               return JS_FALSE;

             emis->val[i] = elixir_get_int(cx, propertie);
          }

        msg = emis;
        break;
     }
     case EDJE_MESSAGE_FLOAT_SET:
     {
        Edje_Message_Float_Set*         emfs;
        unsigned int                    size;
        unsigned int                    i;

        if (!emsg)
          return JS_FALSE;

        if (!JS_GetArrayLength(cx, emsg, &size))
          return JS_FALSE;

        emfs = alloca(sizeof (Edje_Message_Float_Set) + (size - 1) * sizeof (double));
        emfs->count = size;
        for (i = 0; i < size; ++i)
          {
             jsval                      propertie;

             if (!JS_GetElement(cx, emsg, i, &propertie))
               return JS_FALSE;

             emfs->val[i] = elixir_get_dbl(cx, propertie);
          }

        msg = emfs;
        break;
     }
     case EDJE_MESSAGE_STRING_INT:
     {
        Edje_Message_String_Int*        emsi;
        const char*                     str;

        if (!emsg)
            return JS_FALSE;

        emsi = alloca(sizeof (Edje_Message_String_Int));

        if (!elixir_object_get_str(cx, emsg, "str", &str))
          return JS_FALSE;
        emsi->str = (char*) str;

        if (!elixir_object_get_int(cx, emsg, "val", &emsi->val))
          return JS_FALSE;

        msg = emsi;
        break;
     }
     case EDJE_MESSAGE_STRING_FLOAT:
     {
        Edje_Message_String_Float*      emsf;
        const char*                     str;

        if (!emsg)
          return JS_FALSE;

        emsf = alloca(sizeof (Edje_Message_String_Float));

        if (!elixir_object_get_str(cx, emsg, "str", &str))
          return JS_FALSE;
        emsf->str = (char*) str;

        if (!elixir_object_get_dbl(cx, emsg, "val", &emsf->val))
          return JS_FALSE;

        msg = emsf;
        break;
     }
     case EDJE_MESSAGE_STRING_INT_SET:
     {
        Edje_Message_String_Int_Set*    emsis;
        JSObject*                       array;
        const char*                     str;
        unsigned int                    size;
        unsigned int                    i;

        if (!emsg)
          return JS_FALSE;

        array = elixir_object_get_object(cx, emsg, "val");
        if (!array)
          return JS_FALSE;

        if (!JS_GetArrayLength(cx, array, &size))
          return JS_FALSE;

        emsis = alloca(sizeof (Edje_Message_String_Float_Set) + (size - 1) * sizeof (int));

        if (!elixir_object_get_str(cx, emsg, "str", &str))
          return JS_FALSE;
        emsis->str = (char*) str;

        for (i = 0; i < size; ++i)
          {
             jsval                      propertie;

             if (!JS_GetElement(cx, array, i, &propertie))
               return JS_FALSE;

             emsis->val[i] = elixir_get_int(cx, propertie);
          }

        msg = emsis;
        break;
     }
     case EDJE_MESSAGE_STRING_FLOAT_SET:
     {
        Edje_Message_String_Float_Set*  emsfs;
        JSObject*                       array;
        const char*                     str;
        unsigned int                    size;
        unsigned int                    i;

        if (!emsg)
          return JS_FALSE;

        array = elixir_object_get_object(cx, emsg, "val");
        if (!array)
          return JS_FALSE;

        if (!JS_GetArrayLength(cx, array, &size))
          return JS_FALSE;

        emsfs = alloca(sizeof (Edje_Message_String_Float_Set) + (size - 1) * sizeof (double));

        if (!elixir_object_get_str(cx, emsg, "str", &str))
          return JS_FALSE;
        emsfs->str = (char*) str;

        for (i = 0; i < size; ++i)
          {
             jsval                      propertie;

             if (!JS_GetElement(cx, array, i, &propertie))
               return JS_FALSE;

             emsfs->val[i] = elixir_get_dbl(cx, propertie);
          }

        msg = emsfs;
        break;
     }
     default:
        return JS_FALSE;
   };

   edje_object_message_send(eo, type, id, msg);

   return JS_TRUE;
}

static void
_elixir_edje_object_message_handler_cb(void *data, Evas_Object *obj, Edje_Message_Type type, int id, void *msg)
{
   JSFunction *cb;
   JSContext *cx;
   JSObject *jmsg = NULL;
   JSObject *parent;
   jsval js_return;
   jsval argv[5];

   cb = elixir_void_get_private(data);
   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);

   if (!cx || !parent || !cb)
     return ;

   elixir_function_start(cx);

   if (!elixir_rval_new(cx, elixir_class_request("edje_object", "evas_object"), obj, argv + 1))
     goto on_finish;

   argv[0] = elixir_void_get_jsval(data);
   argv[2] = INT_TO_JSVAL(type);
   argv[3] = INT_TO_JSVAL(id);

   switch (type)
     {
     case EDJE_MESSAGE_NONE:
        argv[4] = JSVAL_VOID;
        break;
     case EDJE_MESSAGE_STRING:
     {
        Edje_Message_String*            ems;

        ems = msg;
        argv[4] = STRING_TO_JSVAL(elixir_ndup(cx, ems->str, strlen(ems->str)));
        break;
     }
     case EDJE_MESSAGE_INT:
     {
        Edje_Message_Int*               emi;

        emi = msg;
        argv[4] = INT_TO_JSVAL(emi->val);
        break;
     }
     case EDJE_MESSAGE_FLOAT:
     {
        Edje_Message_Float*             emf;

        emf = msg;
	JS_NewNumberValue(cx, emf->val, argv + 4);

        break;
     }
     case EDJE_MESSAGE_STRING_SET:
     {
        Edje_Message_String_Set*        emss;
        int                             i;

        jmsg = JS_NewArrayObject(cx, 0, NULL);
        if (!elixir_object_register(cx, &jmsg, NULL))
          goto on_error;

        emss = msg;
        for (i = 0; i < emss->count; ++i)
          {
             jsval                      str;

             str = STRING_TO_JSVAL(elixir_ndup(cx, emss->str[i], strlen(emss->str[i])));
             if (!JS_SetElement(cx, jmsg, i, &str))
               goto on_error;
          }
        break;
     }
     case EDJE_MESSAGE_INT_SET:
     {
        Edje_Message_Int_Set*           emis;
        int                             i;

        jmsg = JS_NewArrayObject(cx, 0, NULL);
        if (!elixir_object_register(cx, &jmsg, NULL))
          goto on_error;

        emis = msg;
        for (i = 0; i < emis->count; ++i)
          {
             jsval                      val;

             val = INT_TO_JSVAL(emis->val[i]);
             if (!JS_SetElement(cx, jmsg, i, &val))
               goto on_error;
          }
        break;
     }
     case EDJE_MESSAGE_FLOAT_SET:
     {
        Edje_Message_Float_Set*         emfs;
        int                             i;

        jmsg = JS_NewArrayObject(cx, 0, NULL);
        if (!elixir_object_register(cx, &jmsg, NULL))
          goto on_error;

        emfs = msg;
        for (i = 0; i < emfs->count; ++i)
          {
             jsval                      val;

	     JS_NewNumberValue(cx, emfs->val[i], &val);

             if (!JS_SetElement(cx, jmsg, i, &val))
               goto on_error;
          }
        break;
     }
     case EDJE_MESSAGE_STRING_INT:
     {
        Edje_Message_String_Int*        emsi;

        jmsg = JS_NewObject(cx, elixir_class_request("Edje_Message", NULL), NULL, NULL);
        if (!elixir_object_register(cx, &jmsg, NULL))
          goto on_error;

        emsi = msg;
        if (!elixir_add_str_prop(cx, jmsg, "str", emsi->str))
          goto on_error;
        if (!elixir_add_int_prop(cx, jmsg, "val", emsi->val))
          goto on_error;
        break;
     }
     case EDJE_MESSAGE_STRING_FLOAT:
     {
        Edje_Message_String_Float*      emsf;

        jmsg = JS_NewObject(cx, elixir_class_request("Edje_Message", NULL), NULL, NULL);
        if (!elixir_object_register(cx, &jmsg, NULL))
          goto on_error;

        emsf = msg;
        if (!elixir_add_str_prop(cx, jmsg, "str", emsf->str))
          goto on_error;
        if (!elixir_add_dbl_prop(cx, jmsg, "val", emsf->val))
          goto on_error;
        break;
     }
     case EDJE_MESSAGE_STRING_INT_SET:
     {
        Edje_Message_String_Int_Set*    emsis;
        JSObject*                       array;
        int                             i;

        jmsg = JS_NewObject(cx, elixir_class_request("Edje_Message", NULL), NULL, NULL);
        if (!elixir_object_register(cx, &jmsg, NULL))
          goto on_error;

        array = JS_NewArrayObject(cx, 0, NULL);
        if (!array)
          goto on_error;
        if (!elixir_add_object_prop(cx, jmsg, "val", array))
          goto on_error;

        emsis = msg;
        if (!elixir_add_str_prop(cx, jmsg, "str", emsis->str))
          goto on_error;
        for (i = 0; i < emsis->count; ++i)
          {
             jsval                      val;

             val = INT_TO_JSVAL(emsis->val[i]);
             if (!JS_SetElement(cx, array, i, &val))
               goto on_error;
          }
        break;
     }
     case EDJE_MESSAGE_STRING_FLOAT_SET:
     {
        Edje_Message_String_Float_Set*  emsfs;
        JSObject*                       array;
        int                             i;

        jmsg = JS_NewObject(cx, elixir_class_request("Edje_Message", NULL), NULL, NULL);
        if (!elixir_object_register(cx, &jmsg, NULL))
          goto on_error;

        array = JS_NewArrayObject(cx, 0, NULL);
        if (!array)
          goto on_error;
        if (!elixir_add_object_prop(cx, jmsg, "val", array))
          goto on_error;

        emsfs = msg;
        if (!elixir_add_str_prop(cx, jmsg, "str", emsfs->str))
          goto on_error;
        for (i = 0; i < emsfs->count; ++i)
          {
             jsval val;

	     JS_NewNumberValue(cx, emsfs->val[i], &val);

             if (!JS_SetElement(cx, array, i, &val))
               goto on_error;
          }
        break;
     }
     default:
        goto on_error;
   };

   if (jmsg)
     argv[4] = OBJECT_TO_JSVAL(jmsg);

   elixir_function_run(cx, cb, parent, 5, argv, &js_return);

 on_error:
   elixir_rval_delete(cx, argv + 1);
   if (jmsg)
     elixir_object_unregister(cx, &jmsg);
 on_finish:
   elixir_function_stop(cx);
}

static JSBool
elixir_edje_object_message_handler_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object*         eo;
   void*                data;
   elixir_value_t       val[3];

   if (!elixir_params_check(cx, _edje_object_func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, eo);
   /* FIXME: This will leak some memory. */
   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[2].v.any, val[1].v.fct);

   edje_object_message_handler_set(eo, _elixir_edje_object_message_handler_cb, data);

   return JS_TRUE;
}

static JSFunctionSpec   edje_functions[] = {
  ELIXIR_FN(edje_object_message_handler_set, 3, JSPROP_READONLY, 0 ),
  ELIXIR_FN(edje_object_message_send, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_signal_callback_add, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_signal_callback_del, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_text_change_cb_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_color_class_set, 14, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_color_class_set, 13, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_init, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_shutdown, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_file_cache_get, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_collection_cache_get, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_file_cache_flush, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_collection_cache_flush, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_freeze, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_thaw, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_message_signal_process, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_frametime_get, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_scale_get, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_fontset_append_get, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_frametime_set, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_scale_set, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_fontset_append_set, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_color_class_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_text_class_del, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_file_cache_set, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_collection_cache_set, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_file_group_exists, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_file_data_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_file_collection_list, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_text_class_list, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_color_class_list, 0, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_text_class_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_extern_object_min_size_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_extern_object_max_size_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_extern_object_aspect_set, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_size_min_restricted_calc, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_add, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_color_class_del, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_text_select_none, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_text_select_all, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_data_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_text_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_text_selection_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_object_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_swallow_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_load_error_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_play_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_animation_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_freeze, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_thaw, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_drag_dir_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_exists, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_play_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_animation_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_calc_force, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_message_signal_process, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_file_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_text_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_signal_emit, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_text_insert, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_text_anchor_geometry_get, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_drag_value_set, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_drag_step_set, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_drag_page_set, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_drag_step, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_drag_page, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_drag_size_set, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_text_class_set, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_box_append, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_box_prepend, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_box_remove, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_swallow, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_box_insert_at, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_box_insert_before, 4, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_unswallow, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_drag_value_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_drag_step_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_drag_page_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_drag_size_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_file_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_geometry_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_size_min_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_size_min_calc, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_size_max_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_box_remove_all, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_box_remove_at, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_table_unpack, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_text_cursor_geometry_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_state_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_text_anchor_list_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_box_layout_register, 6, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_table_pack, 7, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_table_col_row_size_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_part_table_clear, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(edje_object_scale_set, 2, JSPROP_ENUMERATE, 0),
  ELIXIR_FN(edje_object_scale_get, 1, JSPROP_ENUMERATE, 0),
  JS_FS_END
};

static const struct {
   const char*  name;
   int          value;
} edje_const_properties[] = {
   { "EDJE_ASPECT_CONTROL_NONE", EDJE_ASPECT_CONTROL_NONE },
   { "EDJE_ASPECT_CONTROL_NEITHER", EDJE_ASPECT_CONTROL_NEITHER },
   { "EDJE_ASPECT_CONTROL_HORIZONTAL", EDJE_ASPECT_CONTROL_HORIZONTAL },
   { "EDJE_ASPECT_CONTROL_VERTICAL", EDJE_ASPECT_CONTROL_VERTICAL },
   { "EDJE_ASPECT_CONTROL_BOTH", EDJE_ASPECT_CONTROL_BOTH },
   { "EDJE_MESSAGE_NONE", EDJE_MESSAGE_NONE },
   { "EDJE_MESSAGE_STRING", EDJE_MESSAGE_STRING },
   { "EDJE_MESSAGE_INT", EDJE_MESSAGE_INT },
   { "EDJE_MESSAGE_FLOAT", EDJE_MESSAGE_FLOAT },
   { "EDJE_MESSAGE_STRING_SET", EDJE_MESSAGE_STRING_SET },
   { "EDJE_MESSAGE_INT_SET", EDJE_MESSAGE_INT_SET },
   { "EDJE_MESSAGE_FLOAT_SET", EDJE_MESSAGE_FLOAT_SET },
   { "EDJE_MESSAGE_STRING_INT", EDJE_MESSAGE_STRING_INT },
   { "EDJE_MESSAGE_STRING_FLOAT", EDJE_MESSAGE_STRING_FLOAT },
   { "EDJE_MESSAGE_STRING_INT_SET", EDJE_MESSAGE_STRING_INT_SET },
   { "EDJE_MESSAGE_STRING_FLOAT_SET", EDJE_MESSAGE_STRING_FLOAT_SET },
   { "EDJE_DRAG_DIR_NONE", EDJE_DRAG_DIR_NONE },
   { "EDJE_DRAG_DIR_X", EDJE_DRAG_DIR_X },
   { "EDJE_DRAG_DIR_Y", EDJE_DRAG_DIR_Y },
   { "EDJE_DRAG_DIR_XY", EDJE_DRAG_DIR_XY },
   { "EDJE_LOAD_ERROR_NONE", EDJE_LOAD_ERROR_NONE },
   { "EDJE_LOAD_ERROR_GENERIC", EDJE_LOAD_ERROR_GENERIC },
   { "EDJE_LOAD_ERROR_DOES_NOT_EXIST", EDJE_LOAD_ERROR_DOES_NOT_EXIST },
   { "EDJE_LOAD_ERROR_PERMISSION_DENIED", EDJE_LOAD_ERROR_PERMISSION_DENIED },
   { "EDJE_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED", EDJE_LOAD_ERROR_RESOURCE_ALLOCATION_FAILED },
   { "EDJE_LOAD_ERROR_CORRUPT_FILE", EDJE_LOAD_ERROR_CORRUPT_FILE },
   { "EDJE_LOAD_ERROR_UNKNOWN_FORMAT", EDJE_LOAD_ERROR_UNKNOWN_FORMAT },
   { "EDJE_LOAD_ERROR_INCOMPATIBLE_FILE", EDJE_LOAD_ERROR_INCOMPATIBLE_FILE },
   { "EDJE_LOAD_ERROR_UNKNOWN_COLLECTION", EDJE_LOAD_ERROR_UNKNOWN_COLLECTION },
   { NULL, 0 }
};

static Eina_Bool
module_open(Elixir_Module *em, JSContext *cx, JSObject *parent)
{
   void **tmp;
   unsigned int i = 0;
   jsval property;

   if (em->data)
     return EINA_TRUE;

   em->data = parent;
   tmp = &em->data;
   if (!elixir_object_register(cx, (JSObject**) tmp, NULL))
     goto on_error;

   if (!JS_DefineFunctions(cx, *((JSObject**) tmp), edje_functions))
     goto on_error;

   while (edje_const_properties[i].name != NULL)
     {
        property = INT_TO_JSVAL(edje_const_properties[i].value);
        if (!JS_DefineProperty(cx, parent,
			       edje_const_properties[i].name,
			       property,
			       NULL, NULL,
			       JSPROP_ENUMERATE | JSPROP_READONLY))
          goto on_error;
        ++i;
     }

   _evas_object_parameter.class = elixir_class_request("evas_object", NULL);
   (void) elixir_class_request("evas_object_smart", "evas_object");
   _edje_object_parameter.class = elixir_class_request("edje_object", "evas_object_smart");
   _evas_parameter.class = elixir_class_request("evas", NULL);

   return EINA_TRUE;

  on_error:
   if (em->data)
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
   while (edje_functions[i].name != NULL)
     JS_DeleteProperty(cx, parent, edje_functions[i++].name);

   i = 0;
   while (edje_const_properties[i].name != NULL)
     JS_DeleteProperty(cx, parent, edje_const_properties[i++].name);

   elixir_object_unregister(cx, (JSObject**) tmp);

   em->data = NULL;

   return EINA_TRUE;
}

static Elixir_Module_Api  module_api_elixir = {
   ELIXIR_MODULE_API_VERSION,
   ELIXIR_GRANTED,
   "edje",
   "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_edje = {
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
edje_binding_init(void)
{
   return elixir_modules_register(&em_edje);
}

void
edje_binding_shutdown(void)
{
   elixir_modules_unregister(&em_edje);
}

#ifndef EINA_STATIC_BUILD_EDJE
EINA_MODULE_INIT(edje_binding_init);
EINA_MODULE_SHUTDOWN(edje_binding_shutdown);
#endif
