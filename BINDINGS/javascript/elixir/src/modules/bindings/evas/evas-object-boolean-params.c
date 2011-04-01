#include <stdint.h>
#include <stdlib.h>
#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*        _evas_object_boolean_params[3] = {
   &evas_object_parameter,
   &boolean_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_image_boolean_params[3] = {
   &evas_object_image_parameter,
   &boolean_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_smart_boolean_params[3] = {
  &evas_object_smart_parameter,
  &boolean_parameter,
  NULL
};

static JSBool
elixir_evas_object_boolean_params(void (*func)(Evas_Object* obj, Eina_Bool bool),
				  const elixir_parameter_t *params[],
                                  JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   Eina_Bool boolean;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   boolean = val[1].v.bol;

   func(know, boolean);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_image_data_get(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   JSObject *array;
   uint32_t *data;
   int i;
   int x;
   int y;
   int w;
   int h;
   Eina_Bool for_writing;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_object_image_boolean_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   for_writing = val[1].v.bol;
   data = evas_object_image_data_get(know, for_writing);

   if (!data) return JS_FALSE;

   array = elixir_return_array(cx, vp);
   if (!array) goto on_error;

   evas_object_image_size_get(know, &w, &h);

   for (i = 0, y = 0; y < h; ++y)
     for (x = 0; x < w; ++x, ++data, ++i)
       {
          JSObject*     obj_evas_color;
          jsval         evas_color;

          obj_evas_color = elixir_new_color(cx,
                                            *data & 0x00ff0000, *data & 0x0000ff00,
                                            *data & 0x000000ff, *data & 0xff000000);
          if (!obj_evas_color) goto on_error;

          evas_color = OBJECT_TO_JSVAL(obj_evas_color);
	  if (JS_SetElement(cx, array, i, &evas_color) == JS_FALSE)
	    goto on_error;
       }

   if (JS_SetPrivate(cx, array, data) == JS_FALSE)
     goto on_error;

   return JS_TRUE;

 on_error:
   free(data);
   return JS_FALSE;
}

FAST_CALL_PARAMS_SPEC(evas_object_anti_alias_set, elixir_evas_object_boolean_params, _evas_object_boolean_params);
FAST_CALL_PARAMS_SPEC(evas_object_focus_set, elixir_evas_object_boolean_params, _evas_object_boolean_params);
FAST_CALL_PARAMS_SPEC(evas_object_image_alpha_set, elixir_evas_object_boolean_params, _evas_object_image_boolean_params);
FAST_CALL_PARAMS_SPEC(evas_object_image_smooth_scale_set, elixir_evas_object_boolean_params, _evas_object_image_boolean_params);
FAST_CALL_PARAMS_SPEC(evas_object_image_pixels_dirty_set, elixir_evas_object_boolean_params, _evas_object_image_boolean_params);
FAST_CALL_PARAMS_SPEC(evas_object_pass_events_set, elixir_evas_object_boolean_params, _evas_object_boolean_params);
FAST_CALL_PARAMS_SPEC(evas_object_repeat_events_set, elixir_evas_object_boolean_params, _evas_object_boolean_params);
FAST_CALL_PARAMS_SPEC(evas_object_propagate_events_set, elixir_evas_object_boolean_params, _evas_object_boolean_params);
FAST_CALL_PARAMS_SPEC(evas_object_map_enable_set, elixir_evas_object_boolean_params, _evas_object_boolean_params);
FAST_CALL_PARAMS_SPEC(evas_object_smart_need_recalculate_set, elixir_evas_object_boolean_params, _evas_object_smart_boolean_params);

static JSFunctionSpec     evas_object_params_function[] = {
  ELIXIR_FN(evas_object_image_data_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_anti_alias_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_focus_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_alpha_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_smooth_scale_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_pixels_dirty_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_pass_events_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_repeat_events_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_propagate_events_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_map_enable_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_smart_need_recalculate_set, 2, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_object_boolean_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_object_params_function))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_evas_object_boolean_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_object_params_function[i].name)
     JS_DeleteProperty(cx, parent, evas_object_params_function[i++].name);

   return EINA_TRUE;
}

