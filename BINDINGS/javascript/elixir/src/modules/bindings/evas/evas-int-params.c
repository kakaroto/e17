#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*	_evas_int_params[3] = {
   &evas_parameter,
   &int_parameter,
   NULL
};

static const elixir_parameter_t*	_evas_2int_params[4] = {
   &evas_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};

static const elixir_parameter_t*	_evas_size_parameter[3] = {
  &evas_parameter,
  &size_parameter,
  NULL
};

typedef void (*evas_int_call)(Evas *evas, int val);

static void
_elixir_evas_font_hinting_set(Evas *e, int hinting)
{
   return evas_font_hinting_set(e, (Evas_Font_Hinting_Flags)hinting);
}

static JSBool
elixir_evas_int_params(evas_int_call func,
                       JSContext *cx, uintN argc, jsval *vp)
{
   Evas *evas = NULL;
   int value;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);
   value = val[1].v.num;

   if (!evas) return JS_FALSE;

   func(evas, value);

   return JS_TRUE;
}

/* I know, it must be an enum, but we don't have that in JS. */
FAST_CALL_PARAMS_CAST(evas_font_hinting_set, elixir_evas_int_params);
FAST_CALL_PARAMS(evas_font_cache_set, elixir_evas_int_params);
FAST_CALL_PARAMS(evas_image_cache_set, elixir_evas_int_params);

static JSBool
elixir_evas_font_hinting_can_hint(JSContext *cx, uintN argc, jsval *vp)
{
   Evas *evas = NULL;
   int hinting;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);
   hinting = val[1].v.num;

   if (!evas) return JS_FALSE;

   JS_SET_RVAL(cx, vp, evas_font_hinting_can_hint(evas, hinting) ? JSVAL_TRUE : JSVAL_FALSE);
   return JS_TRUE;
}

static JSBool
elixir_evas_output_size_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas *e = NULL;
   int w;
   int h;
   elixir_value_t val[3];

   if (elixir_params_check(cx, _evas_2int_params, val, argc, JS_ARGV(cx, vp)))
     {
	e = JS_GetPrivate(cx, val[0].v.obj);

	w = val[1].v.num;
	h = val[2].v.num;
     }

   if (elixir_params_check(cx, _evas_size_parameter, val, argc, JS_ARGV(cx, vp)))
     {
	e = JS_GetPrivate(cx, val[0].v.obj);

	if (!elixir_extract_size(cx, val[1].v.obj, &w, &h))
	  return JS_FALSE;
     }

   if (!e) return JS_FALSE;

   evas_output_size_set(e, w, h);

   return JS_TRUE;
}

static JSFunctionSpec     evas_int_params_function[] = {
  ELIXIR_FN(evas_font_hinting_can_hint, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_font_cache_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_font_hinting_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_image_cache_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_output_size_set, 2, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_int_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_int_params_function))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_evas_int_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_int_params_function[i].name)
     JS_DeleteProperty(cx, parent, evas_int_params_function[i++].name);

   return EINA_TRUE;
}

