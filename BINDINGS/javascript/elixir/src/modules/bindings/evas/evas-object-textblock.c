#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <Evas.h>

#include "evas-bindings.h"

static elixir_parameter_t               _evas_textblock_style_parameter = {
   "Evas_Textblock_Style", JOBJECT, NULL
};
static elixir_parameter_t               _evas_textblock_cursor_parameter = {
   "Evas_Textblock_Cursor", JOBJECT, NULL
};

static const elixir_parameter_t*        _evas_object_textblock_params[2] = {
   &evas_object_textblock_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_textblock_style_params[2] = {
   &_evas_textblock_style_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_textblock_cursor_params[2] = {
   &_evas_textblock_cursor_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_textblock_style_string_params[3] = {
   &_evas_textblock_style_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_textblock_cursor_string_params[3] = {
   &_evas_textblock_cursor_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_object_textblock_evas_textblock_style_params[3] = {
   &evas_object_textblock_parameter,
   &_evas_textblock_style_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_textblock_cursor_int_params[3] = {
   &_evas_textblock_cursor_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*	_double_evas_textblock_cursor_params[3] = {
   &_evas_textblock_cursor_parameter,
   &_evas_textblock_cursor_parameter,
   NULL
};
static const elixir_parameter_t*	_double_evas_textblock_cursor_int_params[4] = {
   &_evas_textblock_cursor_parameter,
   &_evas_textblock_cursor_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_textblock_cursor_double_int_params[4] = {
   &_evas_textblock_cursor_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};

static JSBool
elixir_bool_evas_textblock_cursor_params(Eina_Bool (*func)(Evas_Textblock_Cursor* cur),
                                         JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_textblock_cursor_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(func(know)));
   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_params(void (*func)(Evas_Textblock_Cursor* cur),
                                    JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_textblock_cursor_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   func(know);

   return JS_TRUE;
}

static JSBool
elixir_int_evas_textblock_cursor_string_params(int (*func)(Evas_Textblock_Cursor* cur, const char* str),
					       JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   const char *str;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_textblock_cursor_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   str = elixir_get_string_bytes(val[1].v.str, NULL);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(func(know, str)));

   return JS_TRUE;
}

FAST_CALL_PARAMS(evas_textblock_cursor_text_append, elixir_int_evas_textblock_cursor_string_params);
FAST_CALL_PARAMS(evas_textblock_cursor_text_prepend, elixir_int_evas_textblock_cursor_string_params);

static JSBool
elixir_evas_textblock_cursor_string_params(Eina_Bool (*func)(Evas_Textblock_Cursor* cur, const char* str),
                                           JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   const char *str;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_textblock_cursor_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   str = elixir_get_string_bytes(val[1].v.str, NULL);

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(func(know, str)));

   return JS_TRUE;
}

FAST_CALL_PARAMS(evas_textblock_cursor_format_append, elixir_evas_textblock_cursor_string_params);
FAST_CALL_PARAMS(evas_textblock_cursor_format_prepend, elixir_evas_textblock_cursor_string_params);

FAST_CALL_PARAMS(evas_textblock_cursor_char_delete, elixir_evas_textblock_cursor_params);

FAST_CALL_PARAMS(evas_textblock_cursor_char_next, elixir_bool_evas_textblock_cursor_params);
FAST_CALL_PARAMS(evas_textblock_cursor_char_prev, elixir_bool_evas_textblock_cursor_params);

static JSBool
elixir_evas_textblock_style_free(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Style *know;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_textblock_style_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   evas_textblock_style_free(know);

   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_style_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Style *ts;
   const char *text;
   size_t length;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_textblock_style_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ts);
   text = elixir_get_string_bytes(val[1].v.str, &length);

   if (text && strlen(text) != length)
     return JS_FALSE;

   evas_textblock_style_set(ts, text);

   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_style_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Style *know;
   const char *text;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_textblock_style_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   text = evas_textblock_style_get(know);

   elixir_return_str(cx, vp, text);
   return JS_TRUE;
}

static JSBool
elixir_evas_object_textblock_style_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Style *kts;
   Evas_Object *kobj;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_object_textblock_evas_textblock_style_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, kobj);
   GET_PRIVATE(cx, val[1].v.obj, kts);

   evas_object_textblock_style_set(kobj, kts);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_textblock_style_get(JSContext *cx, uintN argc, jsval *vp)
{
   JSClass *evas_textblock_style_class;
   Evas_Textblock_Style *ts;
   Evas_Object *know;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_textblock_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   ts = (Evas_Textblock_Style*) evas_object_textblock_style_get(know);
   evas_textblock_style_class = elixir_class_request("evas_textblock_style", NULL);

   elixir_return_ptr(cx, vp, ts, evas_textblock_style_class);
   return JS_TRUE;
}

static JSBool
elixir_evas_object_textblock_cursor_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *cursor;
   Evas_Object *know;
   JSClass *evas_textblock_cursor_class;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_textblock_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   cursor = (Evas_Textblock_Cursor*) evas_object_textblock_cursor_get(know);
   evas_textblock_cursor_class = elixir_class_request("evas_textblock_cursor", NULL);

   elixir_return_ptr(cx, vp, cursor, evas_textblock_cursor_class);
   return JS_TRUE;
}

static JSBool
elixir_evas_object_textblock_cursor_new(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *cursor;
   Evas_Object *know;
   JSClass *evas_textblock_cursor_class;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_object_textblock_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   cursor = evas_object_textblock_cursor_new(know);
   evas_textblock_cursor_class = elixir_class_request("evas_textblock_cursor", NULL);

   elixir_return_ptr(cx, vp, cursor, evas_textblock_cursor_class);
   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_free(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_textblock_cursor_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   evas_textblock_cursor_free(know);

   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_pos_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_textblock_cursor_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(evas_textblock_cursor_pos_get(know)));
   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_pos_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   int num;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_textblock_cursor_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   num = val[1].v.num;

   evas_textblock_cursor_pos_set(know, num);

   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_line_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   elixir_value_t val[2];
   int line;

   if (!elixir_params_check(cx, _evas_textblock_cursor_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   line = val[1].v.num;

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(evas_textblock_cursor_line_set(know, line)));
   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_compare(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   Evas_Textblock_Cursor *know2;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _double_evas_textblock_cursor_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   GET_PRIVATE(cx, val[1].v.obj, know2);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(evas_textblock_cursor_compare(know, know2)));
   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_copy(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   Evas_Textblock_Cursor *know2;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _double_evas_textblock_cursor_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   GET_PRIVATE(cx, val[1].v.obj, know2);

   evas_textblock_cursor_copy(know, know2);

   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_range_delete(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   Evas_Textblock_Cursor *know2;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _double_evas_textblock_cursor_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   GET_PRIVATE(cx, val[1].v.obj, know2);

   evas_textblock_cursor_range_delete(know, know2);

   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_range_text_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   Evas_Textblock_Cursor *know2;
   char *str;
   int format;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _double_evas_textblock_cursor_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   GET_PRIVATE(cx, val[1].v.obj, know2);
   format = val[2].v.num;

   str = evas_textblock_cursor_range_text_get(know, know2, format);
   elixir_return_str(cx, vp, str);
   free(str);
   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_char_geometry_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   JSObject *js_obj;
   elixir_value_t val[1];
   jsval propertie;
   int icx = 0;
   int icy = 0;
   int icw = 0;
   int ich = 0;
   int lineno;

   if (!elixir_params_check(cx, _evas_textblock_cursor_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   lineno = evas_textblock_cursor_char_geometry_get(know, &icx, &icy, &icw, &ich);
   if (lineno == -1)
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
     }

   if (elixir_new_evas_pos(cx, icx, icy, icw, ich, &(JS_RVAL(cx, vp))) == JS_FALSE)
     return JS_FALSE;

   if (JS_ValueToObject(cx, JS_RVAL(cx, vp), &js_obj) == JS_FALSE)
     return JS_FALSE;

   propertie = INT_TO_JSVAL(lineno);
   JS_SetProperty(cx, js_obj, "lineno", &propertie);

   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_line_geometry_get(JSContext *cx, uintN argc, jsval *vp)
{
   JSObject *js_obj;
   Evas_Textblock_Cursor *know;
   jsval propertie;
   int icx = 0;
   int icy = 0;
   int icw = 0;
   int ich = 0;
   int lineno;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _evas_textblock_cursor_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   lineno = evas_textblock_cursor_line_geometry_get(know, &icx, &icy, &icw, &ich);
   if (lineno == -1)
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
     }

   if (elixir_new_evas_pos(cx, icx, icy, icw, ich, &(JS_RVAL(cx, vp))) == JS_FALSE)
     return JS_FALSE;

   if (JS_ValueToObject(cx, JS_RVAL(cx, vp), &js_obj) == JS_FALSE)
     return JS_FALSE;

   propertie = INT_TO_JSVAL(lineno);
   JS_SetProperty(cx, js_obj, "lineno", &propertie);

   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_char_coord_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   elixir_value_t val[3];
   int y;
   int x;

   if (!elixir_params_check(cx, _evas_textblock_cursor_double_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   x = val[1].v.num;
   y = val[2].v.num;

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(evas_textblock_cursor_char_coord_set(know, x, y)));
   return JS_TRUE;
}

static JSBool
elixir_evas_textblock_cursor_line_coord_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   int y;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_textblock_cursor_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   y = val[1].v.num;

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(evas_textblock_cursor_line_coord_set(know, y)));
   return JS_TRUE;
}

static void
elixir_clean_evas_list(Eina_List* lst)
{
   Evas_Textblock_Rectangle *etr;

   EINA_LIST_FREE(lst, etr)
     free(etr);
}

static JSBool
elixir_evas_textblock_cursor_range_geometry_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Textblock_Cursor *know;
   Evas_Textblock_Cursor *know2;
   Eina_List *rectangles;
   Eina_List *walker;
   JSObject *array;
   int i;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _double_evas_textblock_cursor_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   GET_PRIVATE(cx, val[1].v.obj, know2);

   rectangles = evas_textblock_cursor_range_geometry_get(know, know2);
   if (!rectangles)
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
    }

   array = elixir_return_array(cx, vp);
   if (!array)
     {
	elixir_clean_evas_list(rectangles);
	return JS_FALSE;
     }

    for (i = 0, walker = rectangles; walker; walker = eina_list_next(walker), ++i)
     {
	Evas_Textblock_Rectangle *rectangle;
        JSObject *ret;
        jsval js_rect;

	rectangle = eina_list_data_get(walker);
        ret = elixir_new_geometry(cx,
                                  rectangle->x, rectangle->y,
                                  rectangle->w, rectangle->h);
        if (!ret)
	  {
	     elixir_clean_evas_list(rectangles);
	     return JS_FALSE;
	  }
        js_rect = OBJECT_TO_JSVAL(ret);
	if (JS_SetElement(cx, array, i, &js_rect) == JS_FALSE)
	  {
	     elixir_clean_evas_list(rectangles);
	     return JS_FALSE;
	  }
     }

   return JS_TRUE;
}

static JSFunctionSpec	evas_textblock_functions[] = {
  ELIXIR_FN(evas_textblock_cursor_text_append, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_text_prepend, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_format_append, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_format_prepend, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_char_delete, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_char_next, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_char_prev, 1, JSPROP_ENUMERATE, 0 ),

  ELIXIR_FN(evas_textblock_style_free, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_style_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_style_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_textblock_style_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_textblock_style_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_textblock_cursor_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_textblock_cursor_new, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_free, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_pos_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_pos_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_line_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_compare, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_copy, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_range_delete, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_range_text_get, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_char_geometry_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_line_geometry_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_char_coord_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_line_coord_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_textblock_cursor_range_geometry_get, 2, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_object_textblock_binding(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_textblock_functions))
     return EINA_FALSE;

   _evas_textblock_style_parameter.class = elixir_class_request("evas_textblock_style", NULL);
   _evas_textblock_cursor_parameter.class = elixir_class_request("evas_textblock_cursor", NULL);

   return EINA_TRUE;
}

Eina_Bool
unload_evas_object_textblock_binding(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_textblock_functions[i].name)
     JS_DeleteProperty(cx, parent, evas_textblock_functions[i++].name);

   return EINA_TRUE;
}


