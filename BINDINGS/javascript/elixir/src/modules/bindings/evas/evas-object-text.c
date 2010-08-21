#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <Evas.h>

#include "evas-bindings.h"

static elixir_parameter_t               _evas_font_parameter = {
   "Evas_Font", JOBJECT, NULL
};

static const elixir_parameter_t*	_evas_object_text_string_int_params[4] = {
   &evas_object_parameter,
   &string_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_object_text_font_params[3] = {
   &evas_object_text_parameter,
   &_evas_font_parameter,
   NULL
};
/* FIXME: for evas_object_text_font_source_set, evas_object_text_font_source_get */
/* static const elixir_parameter_t*	_evas_object_string_params[3] = { */
/*    &_evas_object_parameter, */
/*    &string_parameter, */
/*    NULL */
/* }; */

static JSBool
elixir_evas_object_text_font_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   const char *font;
   int size;
   elixir_value_t val[3];

   if (elixir_params_check(cx, _evas_object_text_string_int_params, val, argc, JS_ARGV(cx, vp)))
     {
	size_t length;

	know = JS_GetPrivate(cx, val[0].v.obj);
	font = elixir_get_string_bytes(val[1].v.str, &length);
	size = val[2].v.num;

	if (font && length != strlen(font))
	  return JS_FALSE;
     }

   if (elixir_params_check(cx, _evas_object_text_font_params, val, argc, JS_ARGV(cx, vp)))
     {
	know = JS_GetPrivate(cx, val[0].v.obj);

	if (!elixir_evas_font_extract(cx, val[1].v.obj, &font, &size))
	  return JS_FALSE;
     }

   if (!know) return JS_FALSE;

   evas_object_text_font_set(know, font, size);

   return JS_TRUE;
}

static JSFunctionSpec	evas_object_text_functions[] = {
  ELIXIR_FN(evas_object_text_font_set, 3, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_object_text_binding(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_object_text_functions))
     return EINA_FALSE;

   _evas_font_parameter.class = elixir_class_request("evas_font", NULL);

   return EINA_TRUE;
}

Eina_Bool
unload_evas_object_text_bindings(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_object_text_functions[i].name)
     JS_DeleteProperty(cx, parent, evas_object_text_functions[i++].name);

   return EINA_TRUE;
}

