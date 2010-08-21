#include <string.h>

#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*        _evas_string_params[3] = {
   &evas_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_string_params[3] = {
   &evas_object_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_string_2int_bool_params[6] = {
   &evas_object_parameter,
   &string_parameter,
   &int_parameter,
   &int_parameter,
   &boolean_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_string_2int_params[6] = {
   &evas_object_parameter,
   &string_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_text_string_params[3] = {
   &evas_object_text_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_textblock_string_params[3] = {
   &evas_object_textblock_parameter,
   &string_parameter,
   NULL
};

static JSBool
elixir_evas_key_modifier_mask_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas *e = NULL;
   const char *str;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, e);
   str = elixir_get_string_bytes(val[1].v.str, NULL);

   JS_SET_RVAL(cx, vp, INT_TO_JSVAL(evas_key_modifier_mask_get(e, str)));
   return JS_TRUE;
}

static JSBool
elixir_evas_object_key_grab(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   const char *str;
   Eina_Bool result;
   elixir_value_t val[5];

   if (!elixir_params_check(cx, _evas_object_string_2int_bool_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   str = elixir_get_string_bytes(val[1].v.str, NULL);

   result = evas_object_key_grab(know, str, val[2].v.num, val[3].v.num, val[4].v.bol);

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(result));
   return JS_TRUE;
}

static JSBool
elixir_evas_object_key_ungrab(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   const char *str;
   elixir_value_t val[4];

   if (!elixir_params_check(cx, _evas_object_string_2int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   str = elixir_get_string_bytes(val[1].v.str, NULL);

   evas_object_key_ungrab(know, str, val[2].v.num, val[3].v.num);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_string_params(void (*func)(Evas_Object* obj, const char* str),
                                 const elixir_parameter_t *params[],
                                 JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   const char *str;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   str = elixir_get_string_bytes(val[1].v.str, NULL);

   func(know, str);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_data_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   const char *str;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_object_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   str = elixir_get_string_bytes(val[1].v.str, NULL);

   if (str && strncmp(str, "elixir_jsval", 12) == 0)
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
     }

   JS_SET_RVAL(cx, vp, elixir_void_get_jsval(evas_object_data_get(know, str)));
   return JS_TRUE;
}

static JSBool
elixir_evas_object_data_del(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   const char *str;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_object_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   str = elixir_get_string_bytes(val[1].v.str, NULL);

   if (str && strncmp(str, "elixir_jsval", 12) == 0)
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	return JS_TRUE;
     }

   JS_SET_RVAL(cx, vp, elixir_void_free(evas_object_data_del(know, str)));
   return JS_TRUE;
}

FAST_CALL_PARAMS_SPEC(evas_object_name_set, elixir_evas_object_string_params, _evas_object_string_params);
FAST_CALL_PARAMS_SPEC(evas_object_textblock_text_markup_set, elixir_evas_object_string_params, _evas_object_textblock_string_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_font_source_set, elixir_evas_object_string_params, _evas_object_text_string_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_text_set, elixir_evas_object_string_params, _evas_object_text_string_params);

static JSFunctionSpec     evas_object_params_function[] = {
  ELIXIR_FN(evas_key_modifier_mask_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_key_grab, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_key_ungrab, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_name_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_data_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_data_del, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_textblock_text_markup_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_font_source_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_text_set, 2, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_object_string_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_object_params_function))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_evas_object_string_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_object_params_function[i].name)
     JS_DeleteProperty(cx, parent, evas_object_params_function[i++].name);

   return EINA_TRUE;
}

