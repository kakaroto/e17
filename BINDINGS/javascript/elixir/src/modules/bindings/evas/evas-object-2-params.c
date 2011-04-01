#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*	_double_evas_object_params[3] = {
   &evas_object_parameter,
   &evas_object_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_object_evas_object_smart_params[3] = {
   &evas_object_parameter,
   &evas_object_smart_parameter,
   NULL
};

static JSBool
elixir_2_evas_object_params(void (*func)(Evas_Object *ref, Evas_Object *other),
                            const elixir_parameter_t *params[],
                            JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *ref;
   Evas_Object *other;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ref);
   GET_PRIVATE(cx, val[1].v.obj, other);

   func(ref, other);

   return JS_TRUE;
}

FAST_CALL_PARAMS_SPEC(evas_object_smart_member_add, elixir_2_evas_object_params, _evas_object_evas_object_smart_params);
FAST_CALL_PARAMS_SPEC(evas_object_stack_above, elixir_2_evas_object_params, _double_evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_stack_below, elixir_2_evas_object_params, _double_evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_clip_set, elixir_2_evas_object_params, _double_evas_object_params);
FAST_CALL_PARAMS_SPEC(evas_object_map_source_set, elixir_2_evas_object_params, _double_evas_object_params);

static JSFunctionSpec     evas_object_params_function[] = {
   ELIXIR_FN(evas_object_stack_above, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(evas_object_stack_below, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(evas_object_clip_set, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(evas_object_smart_member_add, 2, JSPROP_ENUMERATE, 0 ),
   ELIXIR_FN(evas_object_map_source_set, 2, JSPROP_ENUMERATE, 0 ),
   JS_FS_END
};

Eina_Bool
load_2_evas_object_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_object_params_function))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_2_evas_object_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_object_params_function[i].name)
     JS_DeleteProperty(cx, parent, evas_object_params_function[i++].name);

   return EINA_TRUE;
}

