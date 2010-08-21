#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <Eet.h>

#include "Elixir.h"

static const elixir_parameter_t* _object_str_object_str[] = {
  &jsobject_parameter,
  &string_parameter,
  &jsobject_parameter,
  &string_parameter,
  NULL
};

static const elixir_parameter_t* _eet_data_str[] = {
  &eet_parameter,
  &string_parameter,
  NULL
};


static JSBool
elixir_eet_data_encode(JSContext *cx, uintN argc, jsval *vp)
{
   Elixir_Eet_Data *dt;
   const char *name;
   const char *key;
   elixir_value_t val[4];

   /* Default with no key. */
   val[3].v.str = NULL;

   if (!elixir_params_check_with_options(cx, _object_str_object_str, val, argc, JS_ARGV(cx, vp), 4))
     return JS_FALSE;

   name = elixir_get_string_bytes(val[1].v.str, NULL);
   key = elixir_get_string_bytes(val[3].v.str, NULL);

   dt = elixir_to_eet_data(cx, JS_ARGV(cx, vp)[0], name, JS_ARGV(cx, vp)[2], key);
   elixir_return_ptr(cx, vp, dt, elixir_class_request("eet_data", NULL));
   return JS_TRUE;
}

static JSBool
elixir_eet_data_decode(JSContext *cx, uintN argc, jsval *vp)
{
   Elixir_Eet_Data *dt;
   const char *key;
   JSObject *obj;
   elixir_value_t val[2];

   /* Default with no key */
   val[1].v.str = NULL;

   if (!elixir_params_check_with_options(cx, _eet_data_str, val, argc, JS_ARGV(cx, vp), 2))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, dt);
   key = elixir_get_string_bytes(val[1].v.str, NULL);

   obj = elixir_from_eet_data(cx, dt, key);
   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(obj));

   return JS_TRUE;
}

static JSFunctionSpec eet_functions[] = {
  ELIXIR_FN(eet_data_encode, 4, JSPROP_ENUMERATE, 0  ),
  ELIXIR_FN(eet_data_decode, 2, JSPROP_ENUMERATE, 0  ),
  JS_FS_END
};

static Eina_Bool
module_open(Elixir_Module *em, JSContext *cx, JSObject *parent)
{
   void **tmp = NULL;

   if (em->data) return EINA_TRUE;

   em->data = parent;
   tmp = &em->data;

   if (!elixir_object_register(cx, (JSObject**) tmp, NULL))
     goto on_error;

   if (!JS_DefineFunctions(cx, *((JSObject**) tmp), eet_functions))
     goto on_error;

   elixir_eet_init(cx, parent);

   return EINA_TRUE;

 on_error:
   if (tmp) elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;

   return EINA_FALSE;
}

static Eina_Bool
module_close(Elixir_Module *em, JSContext *cx)
{
   void **tmp;
   unsigned int i = 0;

   if (!em->data)
     return EINA_FALSE;

   elixir_eet_shutdown(cx, em->data);

   tmp = &em->data;

   while (eet_functions[i].name)
     JS_DeleteProperty(cx, *((JSObject**) tmp), eet_functions[i++].name);

   elixir_object_unregister(cx, (JSObject**) tmp);

   em->data = NULL;

   return EINA_TRUE;
}

static Elixir_Module_Api  module_api_elixir = {
  ELIXIR_MODULE_API_VERSION,
  ELIXIR_GRANTED,
  "eet",
  "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_eet = {
  &module_api_elixir,
  NULL,
  EINA_FALSE,
  {
    module_open,
    NULL,
    module_close
  }
};

Eina_Bool
eet_binding_init(void)
{
   return elixir_modules_register(&em_eet);
}

void
eet_binding_shutdown(void)
{
   elixir_modules_unregister(&em_eet);
}

#ifndef EINA_STATIC_BUILD_EET
EINA_MODULE_INIT(eet_binding_init);
EINA_MODULE_SHUTDOWN(eet_binding_shutdown);
#endif
