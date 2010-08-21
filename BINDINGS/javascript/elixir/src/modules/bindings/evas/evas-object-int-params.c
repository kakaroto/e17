#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*        _evas_object_int_params[3] = {
   &evas_object_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_image_int_params[3] = {
   &evas_object_image_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_text_int_params[3] = {
   &evas_object_text_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_textblock_int_params[3] = {
   &evas_object_textblock_parameter,
   &int_parameter,
   NULL
};


static void
_elixir_evas_object_layer_set(Evas_Object* obj, int size)
{
   return evas_object_layer_set(obj, (short)size);
}

static void
_elixir_evas_object_render_op_set(Evas_Object* obj, int op)
{
   return evas_object_render_op_set(obj, (Evas_Render_Op)op);
}

static void
_elixir_evas_object_text_style_set(Evas_Object* obj, int type)
{
   return evas_object_text_style_set(obj, (Evas_Text_Style_Type)type);
}

static void
_elixir_evas_object_image_border_center_fill_set(Evas_Object *obj, int type)
{
   return evas_object_image_border_center_fill_set(obj, (Evas_Border_Fill_Mode) type);
}

static JSBool
elixir_evas_object_int_params(void (*func)(Evas_Object *obj, int value),
                              const elixir_parameter_t *params[],
                              JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   int l;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   l = val[1].v.num;

   func(know, l);

   return JS_TRUE;
}

FAST_CALL_PARAMS_SPEC_CAST(evas_object_layer_set, elixir_evas_object_int_params, _evas_object_int_params);
FAST_CALL_PARAMS_SPEC(evas_object_color_interpolation_set, elixir_evas_object_int_params, _evas_object_int_params);
/* We don't have enum in JS. */
FAST_CALL_PARAMS_SPEC_CAST(evas_object_render_op_set, elixir_evas_object_int_params, _evas_object_int_params);
FAST_CALL_PARAMS_SPEC_CAST(evas_object_image_border_center_fill_set, elixir_evas_object_int_params, _evas_object_int_params);

FAST_CALL_PARAMS_SPEC(evas_object_image_load_scale_down_set, elixir_evas_object_int_params, _evas_object_image_int_params);
/* We don't have enum in JS. */
FAST_CALL_PARAMS_SPEC_CAST(evas_object_text_style_set, elixir_evas_object_int_params, _evas_object_text_int_params);

static JSBool
elixir_pos_evas_object_int_params(Eina_Bool (*func)(const Evas_Object* obj, int value, Evas_Coord *cx, Evas_Coord *cy, Evas_Coord *cw, Evas_Coord *ch),
                                  const elixir_parameter_t *params[],
                                  JSContext* cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   int value;
   int icx;
   int icy;
   int icw;
   int ich;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   value = val[1].v.num;

   if (func(know, value, &icx, &icy, &icw, &ich))
     return elixir_new_evas_pos(cx, icx, icy, icw, ich, &(JS_RVAL(cx, vp)));

   JS_SET_RVAL(cx, vp, JSVAL_NULL);
   return JS_TRUE;
}

/* Apparently Evas_Bool is not seen by GCC like an int and throw some warning. */
FAST_CALL_PARAMS_SPEC(evas_object_textblock_line_number_geometry_get, elixir_pos_evas_object_int_params, _evas_object_textblock_int_params);
FAST_CALL_PARAMS_SPEC(evas_object_text_char_pos_get, elixir_pos_evas_object_int_params, _evas_object_text_int_params);

static JSFunctionSpec     evas_object_params_function[] = {
  ELIXIR_FN(evas_object_image_border_center_fill_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_layer_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_color_interpolation_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_render_op_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_load_scale_down_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_textblock_line_number_geometry_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_char_pos_get, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_text_style_set, 2, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_object_int_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_object_params_function))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_evas_object_int_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_object_params_function[i].name)
     JS_DeleteProperty(cx, parent, evas_object_params_function[i++].name);

   return EINA_TRUE;
}

