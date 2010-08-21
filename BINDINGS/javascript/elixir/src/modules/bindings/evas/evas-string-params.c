#include <string.h>
#include <stdlib.h>

#include <Evas.h>

#include "evas-bindings.h"

static const elixir_parameter_t*	_evas_string_params[3] = {
   &evas_parameter,
   &string_parameter,
   NULL
};

static JSBool
elixir_evas_string_params(void (*func)(Evas *evas, const char *str),
                          JSContext *cx, uintN argc, jsval *vp)
{
   Evas *evas = NULL;
   char *file;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);
   file = elixir_file_canonicalize(elixir_get_string_bytes(val[1].v.str, NULL));

   func(evas, file);

   free(file);

   return JS_TRUE;
}

static JSBool
elixir_object_evas_string_params(Evas_Object* (*func)(const Evas *evas, const char *str),
                                 JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   const char *str;
   Evas *evas;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, evas);

   str = elixir_get_string_bytes(val[1].v.str, NULL);
   know = func(evas, str);

   return evas_object_to_jsval(cx, know, &(JS_RVAL(cx, vp)));
}

FAST_CALL_PARAMS(evas_font_path_append, elixir_evas_string_params);
FAST_CALL_PARAMS(evas_font_path_prepend, elixir_evas_string_params);
FAST_CALL_PARAMS(evas_object_name_find, elixir_object_evas_string_params);

static JSFunctionSpec     evas_string_params_function[] = {
  ELIXIR_FN(evas_font_path_append, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_font_path_prepend, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_name_find, 2, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_string_params(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_string_params_function))
     return EINA_FALSE;

   return EINA_TRUE;
}

Eina_Bool
unload_evas_string_params(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_string_params_function[i].name)
     JS_DeleteProperty(cx, parent, evas_string_params_function[i++].name);

   return EINA_TRUE;
}

