#include <Evas.h>

#include "evas-bindings.h"

static elixir_parameter_t               _evas_line_coord_parameter = {
   "Evas_Line_Coord", JOBJECT, NULL
};

static const elixir_parameter_t*	_line_set_params_short[3] = {
   &evas_object_line_parameter,
   &_evas_line_coord_parameter,
   NULL
};
static const elixir_parameter_t*	_line_set_params_long[6] = {
   &evas_object_line_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};

static JSBool
elixir_evas_object_line_xy_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *evas_line = NULL;
   int x1;
   int y1;
   int x2;
   int y2;
   elixir_value_t val[5];

   if (elixir_params_check(cx, _line_set_params_short, val, argc, JS_ARGV(cx, vp)))
     {
	evas_line = JS_GetPrivate(cx, val[0].v.obj);

        if (!elixir_evas_line_extract(cx, val[1].v.obj, &x1, &y1, &x2, &y2))
          return JS_FALSE;
     }

   if (elixir_params_check(cx, _line_set_params_long, val, argc, JS_ARGV(cx, vp)))
     {
	evas_line = JS_GetPrivate(cx, val[0].v.obj);

	x1 = val[1].v.num;
	y1 = val[2].v.num;
	x2 = val[3].v.num;
	y2 = val[4].v.num;
     }

   if (!evas_line) return JS_FALSE;

   evas_object_line_xy_set(evas_line, x1, y1, x2, y2);

   return JS_TRUE;
}

static JSFunctionSpec   evas_object_line_functions[] = {
  ELIXIR_FN(evas_object_line_xy_set, 5, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_object_line_binding(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_object_line_functions))
     return EINA_FALSE;

   _evas_line_coord_parameter.class = elixir_class_request("evas_line_coord", NULL);

   return EINA_TRUE;
}

Eina_Bool
unload_evas_object_line_binding(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_object_line_functions[i].name)
     JS_DeleteProperty(cx, parent, evas_object_line_functions[i++].name);

   return EINA_TRUE;
}

