#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*        _evas_object_int_4_params[6] = {
   &evas_object_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};

static JSBool
elixir_evas_object_size_hint_padding_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   int l;
   int r;
   int t;
   int b;
   elixir_value_t val[5];

   if (!elixir_params_check(cx, _evas_object_int_4_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   l = val[1].v.num;
   r = val[2].v.num;
   t = val[3].v.num;
   b = val[4].v.num;

   evas_object_size_hint_padding_set(know, l, r, t, b);

   return JS_TRUE;
}

static JSFunctionSpec     evas_objects_functions[] = {
  ELIXIR_FN(evas_object_size_hint_padding_set, 4, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_4_int_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_objects_functions))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_4_int_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_objects_functions[i].name)
     JS_DeleteProperty(cx, parent, evas_objects_functions[i++].name);

   return EINA_TRUE;
}
