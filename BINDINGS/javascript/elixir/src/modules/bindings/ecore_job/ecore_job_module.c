#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <string.h>
#include <Ecore_Job.h>

#include "Elixir.h"

static elixir_parameter_t               _ecore_job_parameter = {
   "Ecore_Job", JOBJECT, NULL
};
static const elixir_parameter_t*              _func_any_params[3] = {
   &function_parameter,
   &any_parameter,
   NULL
};
static const elixir_parameter_t*              _ecore_job_params[2] = {
   &_ecore_job_parameter,
   NULL
};

FAST_CALL_PARAMS(ecore_job_init, elixir_int_params_void);
FAST_CALL_PARAMS(ecore_job_shutdown, elixir_int_params_void);

static void
_elixir_ecore_job_cb(void* data)
{
   JSObject*            parent;
   JSContext*           cx;
   JSFunction*          cb;
   jsval                js_return;
   jsval                argv[1];

   cb = elixir_void_get_private(data);
   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);
   if (!cx || !parent || !cb)
     return ;

   elixir_function_start(cx);

   argv[0] = elixir_void_get_jsval(data);

   elixir_function_run(cx, cb, parent, 1, argv, &js_return);

   elixir_function_stop(cx);
}

static JSBool
elixir_ecore_job_add(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Job *ej;
   void *data;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _func_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   /* If you don't call ecore_job_del, this will leak. Perhaps better to implement ecore_job directly in JS. */
   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[1].v.any, val[0].v.fct);

   ej = ecore_job_add(_elixir_ecore_job_cb, data);

   elixir_return_ptr(cx, vp, ej, elixir_class_request("ecore_job", NULL));
   return JS_TRUE;
}

static JSBool
elixir_ecore_job_del(JSContext *cx, uintN argc, jsval *vp)
{
   Ecore_Job *ej;
   void *data;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, _ecore_job_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, ej);

   data = ecore_job_del(ej);

   JS_SET_RVAL(cx, vp, elixir_void_free(data));
   return JS_TRUE;
}

static JSFunctionSpec   ecore_job_functions[] = {
  ELIXIR_FN(ecore_job_add, 2, JSPROP_READONLY, 0 ),
  ELIXIR_FN(ecore_job_del, 1, JSPROP_READONLY, 0 ),
  ELIXIR_FN(ecore_job_init, 0, JSPROP_READONLY, 0 ),
  ELIXIR_FN(ecore_job_shutdown, 0, JSPROP_READONLY, 0 ),
  JS_FS_END
};

static Eina_Bool
module_open(Elixir_Module *em, JSContext *cx, JSObject *parent)
{
   void **tmp;

   if (em->data)
     return EINA_TRUE;

   em->data = parent;
   tmp = &em->data;
   if (!elixir_object_register(cx, (JSObject**) tmp, NULL))
     goto on_error;

   if (!JS_DefineFunctions(cx, *((JSObject**) tmp), ecore_job_functions))
     goto on_error;

   _ecore_job_parameter.class = elixir_class_request("ecore_job", NULL);

   return EINA_TRUE;

  on_error:
   if (tmp)
     elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;
   return EINA_FALSE;
}

static Eina_Bool
module_close(Elixir_Module *em, JSContext *cx)
{
   JSObject *parent;
   void **tmp;
   unsigned int i;

   if (!em->data)
     return EINA_FALSE;

   parent = (JSObject*) em->data;
   tmp = &em->data;

   i = 0;
   while (ecore_job_functions[i].name != NULL)
     JS_DeleteProperty(cx, parent, ecore_job_functions[i++].name);

   elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;

   return EINA_TRUE;
}

static Elixir_Module_Api  module_api_elixir = {
   ELIXIR_MODULE_API_VERSION,
   ELIXIR_GRANTED,
   "ecore-job",
   "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_ecore_job = {
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
ecore_job_binding_init(void)
{
   return elixir_modules_register(&em_ecore_job);
}

void
ecore_job_binding_shutdown(void)
{
   elixir_modules_unregister(&em_ecore_job);
}

#ifndef EINA_STATIC_BUILD_ECORE_JOB
EINA_MODULE_INIT(ecore_job_binding_init);
EINA_MODULE_SHUTDOWN(ecore_job_binding_shutdown);
#endif
