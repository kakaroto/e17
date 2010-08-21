#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*        _evas_object_double_2_params[4] = {
   &evas_object_parameter,
   &double_parameter,
   &double_parameter,
   NULL
};

static JSBool
elixir_evas_object_double_2_params(void (*func)(Evas_Object* obj, double a, double b),
				   JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   double a;
   double b;
   elixir_value_t val[3];

   if (!elixir_params_check(cx, _evas_object_double_2_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   a = val[1].v.dbl;
   b = val[2].v.dbl;

   func(know, a, b);

   return JS_TRUE;
}

FAST_CALL_PARAMS(evas_object_size_hint_align_set, elixir_evas_object_double_2_params);
FAST_CALL_PARAMS(evas_object_size_hint_weight_set, elixir_evas_object_double_2_params);

static JSFunctionSpec     evas_objects_functions[] = {
  ELIXIR_FN(evas_object_size_hint_align_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_size_hint_weight_set, 3, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_2_double_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_objects_functions))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_2_double_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_objects_functions[i].name)
     JS_DeleteProperty(cx, parent, evas_objects_functions[i++].name);

   return EINA_TRUE;
}
