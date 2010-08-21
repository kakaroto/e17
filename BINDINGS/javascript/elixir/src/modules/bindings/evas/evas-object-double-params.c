#include <Evas.h>
#include "evas-bindings.h"

static const elixir_parameter_t*        _evas_object_double_params[3] = {
   &evas_object_parameter,
   &double_parameter,
   NULL
};

static JSBool
elixir_evas_object_scale_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   double scale;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_object_double_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   scale = val[1].v.num;

   evas_object_scale_set(know, scale);

   return JS_TRUE;
}

static JSFunctionSpec evas_objects_functions[] = {
  ELIXIR_FN(evas_object_scale_set, 2, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_1_double_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_objects_functions))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_1_double_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_objects_functions[i].name)
     JS_DeleteProperty(cx, parent, evas_objects_functions[i++].name);

   return EINA_TRUE;
}
