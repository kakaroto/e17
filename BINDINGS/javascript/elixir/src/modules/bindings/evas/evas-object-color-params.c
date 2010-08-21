#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*        _evas_object_color_params[3] = {
   &evas_object_parameter,
   &color_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_4int_params[6] = {
   &evas_object_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_text_color_params[3] = {
   &evas_object_text_parameter,
   &color_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_text_4int_params[6] = {
   &evas_object_text_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};

static JSBool
elixir_evas_object_color_params(void (*func)(Evas_Object *obj, int r, int g, int b, int a),
                                const elixir_parameter_t *params1[],
                                const elixir_parameter_t *params2[],
                                JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   int r;
   int g;
   int b;
   int a;
   elixir_value_t val[5];

   if (elixir_params_check(cx, params1, val, argc, JS_ARGV(cx, vp)))
     {
	know = JS_GetPrivate(cx, val[0].v.obj);

	if (!elixir_extract_color(cx, val[1].v.obj, &r, &g, &b, &a))
	  return JS_FALSE;
     }

   if (elixir_params_check(cx, params2, val, argc, JS_ARGV(cx, vp)))
     {
	know = JS_GetPrivate(cx, val[0].v.obj);

	r = val[1].v.num;
	g = val[2].v.num;
	b = val[3].v.num;
	a = val[4].v.num;
     }

   if (!know) return JS_FALSE;

   func(know, r, g, b, a);

   return JS_TRUE;
}

FAST_CALL_PARAMS_2SPEC(evas_object_text_shadow_color_set, elixir_evas_object_color_params, _evas_object_text_color_params, _evas_object_text_4int_params);
FAST_CALL_PARAMS_2SPEC(evas_object_text_glow_color_set, elixir_evas_object_color_params, _evas_object_text_color_params, _evas_object_text_4int_params);
FAST_CALL_PARAMS_2SPEC(evas_object_text_glow2_color_set, elixir_evas_object_color_params, _evas_object_text_color_params, _evas_object_text_4int_params);
FAST_CALL_PARAMS_2SPEC(evas_object_text_outline_color_set, elixir_evas_object_color_params, _evas_object_text_color_params, _evas_object_text_4int_params);
FAST_CALL_PARAMS_2SPEC(evas_object_color_set, elixir_evas_object_color_params, _evas_object_color_params, _evas_object_4int_params);

static JSFunctionSpec     evas_object_params_function[] = {
   ELIXIR_FN(evas_object_text_shadow_color_set, 5, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(evas_object_text_glow_color_set, 5, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(evas_object_text_glow2_color_set, 5, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(evas_object_text_outline_color_set, 5, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(evas_object_color_set, 5, JSPROP_ENUMERATE, 0 ),
   JS_FS_END
};

Eina_Bool
load_evas_object_color_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_object_params_function))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_evas_object_color_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_object_params_function[i].name)
     JS_DeleteProperty(cx, parent, evas_object_params_function[i++].name);

   return EINA_TRUE;
}

