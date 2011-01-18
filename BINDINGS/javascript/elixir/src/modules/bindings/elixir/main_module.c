#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "config.h"
#include "Elixir.h"

static const elixir_parameter_t*        _triple_string_params[4] = {
   &string_parameter,
   &string_parameter,
   &string_parameter,
   NULL
};

static Eina_Array* loaded = NULL;

static JSBool
elixir_current(JSContext *cx, uintN argc, jsval *vp)
{
   JSObject *result;

   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   result = JS_NewObject(cx, elixir_class_request("Elixir_File", NULL), NULL, NULL);
   if (!elixir_object_register(cx, &result, NULL))
     return JS_FALSE;

   if (!elixir_add_str_prop(cx, result, "filename", elixir_id_filename()))
     goto on_error;
   if (elixir_id_section())
     {
	if (!elixir_add_str_prop(cx, result, "section", elixir_id_section()))
	  goto on_error;
     }
   else
     if (!JS_DefineProperty(cx, result, "section", JSVAL_NULL, NULL, NULL,
			    JSPROP_ENUMERATE | JSPROP_READONLY))
       goto on_error;

   elixir_object_unregister(cx, &result);

   JS_SET_RVAL(cx, vp, OBJECT_TO_JSVAL(result));
   return JS_TRUE;

 on_error:
   elixir_object_unregister(cx, &result);
   return JS_FALSE;
}

static JSBool
elixir_print(JSContext *cx, uintN argc, jsval *vp)
{
   JSString *js_str;
   uintN i;

   for (i = 0; i < argc; i++) {
      js_str = JS_ValueToString(cx, JS_ARGV(cx, vp)[i]);
      if (!js_str)
	return JS_FALSE;

      JS_ARGV(cx, vp)[i] = STRING_TO_JSVAL(js_str);
      fputs(elixir_get_string_bytes(js_str, NULL), out);
   }

   return JS_TRUE;
}

static JSBool
elixir_sleep(JSContext *cx, uintN argc, jsval *vp)
{
   int interval;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   interval = val[0].v.num;

   sleep(interval);

   return JS_TRUE;
}

static JSBool
elixir_api(JSContext *cx, uintN argc, jsval *vp)
{
   int version;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   version  = val[0].v.num;

   elixir_api_version_set(version);

   return JS_TRUE;
}

static JSBool
elixir_usleep(JSContext *cx, uintN argc, jsval *vp)
{
   int interval;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, int_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   interval = val[0].v.num;

   usleep(interval);

   return JS_TRUE;
}

static JSBool
elixir_usid(JSContext *cx, uintN argc, jsval *vp)
{
   const char *usid;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, void_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   usid = elixir_id_cid();

   elixir_return_str(cx, vp, usid);
   return JS_TRUE;
}

static JSBool
elixir_gcid(JSContext *cx, uintN argc, jsval *vp)
{
   const char *gcid;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, void_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   gcid = elixir_id_gid();

   elixir_return_str(cx, vp, gcid);
   return JS_TRUE;
}

static JSBool
elixir_load(JSContext *cx, uintN argc, jsval *vp)
{
   const char *module;
   Elixir_Module *em;
   JSObject *parent;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   module = elixir_get_string_bytes(val[0].v.str, NULL);

   /* FIXME: Add the module to a list of loaded module. */
   em = elixir_modules_find(module);
   if (!em) goto on_error;

   parent = JS_GetParent(cx, JS_THIS_OBJECT(cx, vp)) ? JS_GetParent(cx, JS_THIS_OBJECT(cx, vp)) : JS_THIS_OBJECT(cx, vp);

   if (elixir_modules_load(em, cx, parent))
     goto on_error;

   eina_array_push(loaded, em);

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(1));
   return JS_TRUE;

 on_error:
   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(0));
   return JS_TRUE;
}

static JSBool
elixir_unload(JSContext *cx,  uintN argc, jsval *vp)
{
   Elixir_Module *em;
   elixir_value_t val[1];
   const char *module;
   unsigned int i;

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   module = elixir_get_string_bytes(val[0].v.str, NULL);

   /* FIXME: Hack preventing self unloading. */
   if (strcmp(module, "elixir") == 0)
     goto on_error;

   em = elixir_modules_find(module);
   if (!em) goto on_error;

   if (loaded)
     {
	Eina_Array_Iterator it;
	Elixir_Module *over;

	EINA_ARRAY_ITER_NEXT(loaded, i, over, it)
	  if (over == em)
	    {
	       eina_array_data_set(loaded, i, NULL);

	       elixir_modules_unload(em, cx);

	       /* FIXME: Cleanup the array. */
	       JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(1));
	       return JS_TRUE;
	    }
     }

 on_error:
   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(0));
   return JS_TRUE;
}

static JSBool
elixir_include(JSContext *cx, uintN argc, jsval *vp)
{
   Elixir_Runtime *er;
   Elixir_Script *es;
   JSObject *parent;
   const char *params[4];
   elixir_value_t val[3];
   JSBool ret = JS_FALSE;
   int count;
   int i;

   parent = JS_GetParent(cx, JS_THIS_OBJECT(cx, vp)) ? JS_GetParent(cx, JS_THIS_OBJECT(cx, vp)) : JS_THIS_OBJECT(cx, vp);

   memset(val, 0, sizeof (elixir_value_t) * 3);

   if (!elixir_params_check_with_options(cx, _triple_string_params, val, argc, JS_ARGV(cx, vp), 1))
     return JS_FALSE;

   for (count = 0, i = 0; i < 3; ++i)
     {
	if (val[i].v.str) params[i] = elixir_get_string_bytes(val[i].v.str, NULL);
	else params[i] = NULL;
	if (params[i]) count = i + 1;
     }

   params[3] = NULL;

   er = elixir_clone(cx, parent);
   /* clone only fail in really bad situation. */
   if (!er) return JS_FALSE;

   es = elixir_script_file(er, count, params);

   if (!es)
     {
	JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(0));
	elixir_shutdown(er);
	JS_ReportError(cx, "Impossible to load file %s", params[0]);
	return JS_TRUE;
     }
   elixir_script_set_out(es, elixir_script_get_current_out());

   if (elixir_script_run(es, &(JS_RVAL(cx, vp))))
     ret = JS_TRUE;

   elixir_script_free(es);
   elixir_maybe_shutdown(er);

   return ret;
}

static JSBool
elixir_parse(JSContext *cx, uintN argc, jsval *vp)
{
   Elixir_Runtime *er;
   Elixir_Script *es;
   JSObject *parent;
   const char *params[4];
   elixir_value_t val[3];
   int count;
   int i;

   parent = JS_GetParent(cx, JS_THIS_OBJECT(cx, vp)) ? JS_GetParent(cx, JS_THIS_OBJECT(cx, vp)) : JS_THIS_OBJECT(cx, vp);

   memset(val, 0, sizeof (elixir_value_t) * 3);

   if (!elixir_params_check_with_options(cx, _triple_string_params, val, argc, JS_ARGV(cx, vp), 1))
     return JS_FALSE;

   for (count = 0, i = 0; i < 3; ++i)
     {
	params[i] = elixir_get_string_bytes(val[i].v.str, NULL);
	if (params[i]) count = i + 1;
     }

   params[3] = NULL;

   er = elixir_clone(cx, parent);
   /* clone only fail in really bad situation. */
   if (!er) return JS_FALSE;

   es = elixir_script_template(er, count, params);
   if (!es)
     {
	JS_SET_RVAL(cx, vp, JSVAL_NULL);
	elixir_shutdown(er);
	return JS_TRUE;
     }
   elixir_return_script(cx, vp, es);

   elixir_script_free(es);
   elixir_shutdown(er);
   return JS_TRUE;
}

static JSBool
elixir_version(JSContext *cx, uintN argc, jsval *vp)
{
   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   elixir_return_str(cx, vp, VERSION);
   return JS_TRUE;
}

static JSBool
elixir_gc(JSContext *cx, uintN argc, jsval *vp)
{
   if (!elixir_params_check(cx, void_params, NULL, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   elixir_suspended_gc();

   return JS_TRUE;
}

static JSBool
elixir_chdir(JSContext *cx, uintN argc, jsval *vp)
{
   char *path;
   elixir_value_t val[1];

   if (!elixir_params_check(cx, string_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   path = elixir_file_canonicalize(elixir_get_string_bytes(val[0].v.str, NULL));

   JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL((chdir(path) == -1)));
   return JS_TRUE;
}

static Eina_Bool
module_open(Elixir_Module *em, JSContext *cx, JSObject *root)
{
   static JSFunctionSpec functions[] =
     {
       ELIXIR_FN(print, 1, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(sleep, 1, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(usleep, 1, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(usid, 0, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(gcid, 0, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(load, 1, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(unload, 1, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(include, 2, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(parse, 2, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(version, 0, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(chdir, 1, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(gc, 0, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(current, 0, JSPROP_ENUMERATE, 0 ),
       ELIXIR_FN(api, 1, JSPROP_ENUMERATE, 0 ),
       JS_FS_END
     };
   Elixir_Sub_Module *esmd;

   if (em->data)
     return EINA_TRUE;

   esmd = malloc(sizeof(Elixir_Sub_Module));
   if (!esmd)
     return EINA_FALSE;
   esmd->parent = root;
   if (!elixir_object_register(cx, &esmd->parent, NULL))
     goto on_error;

   esmd->item = JS_DefineObject(cx, esmd->parent, "elx", elixir_class_request("elixir", NULL), NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
   if (!elixir_object_register(cx, &esmd->item, NULL))
     goto on_error;

   if (!JS_DefineFunctions(cx, esmd->item, functions))
     goto on_error;

   em->data = esmd;

   loaded = eina_array_new(4);

   return EINA_TRUE;
  on_error:
   if (esmd->item)
     {
        elixir_object_unregister(cx, (JSObject**) &esmd->item);
        JS_DeleteProperty(cx, esmd->parent, "elx");
        elixir_object_unregister(cx, (JSObject**) &esmd->parent);
     }
   free(esmd);
   return EINA_FALSE;
}

static Eina_Bool
module_close(Elixir_Module *em, JSContext *cx)
{
   Elixir_Sub_Module *esmd;
   unsigned int i;

   if (!em->data)
     return EINA_FALSE;
   esmd = em->data;

   if (loaded)
     {
	Eina_Array_Iterator iterator;
	Elixir_Module *em;

	EINA_ARRAY_ITER_NEXT(loaded, i, em, iterator)
	  if (em) elixir_modules_unload(em, cx);

	eina_array_free(loaded);
        loaded = NULL;
     }

   JS_DeleteProperty(cx, esmd->item, "print");
   JS_DeleteProperty(cx, esmd->item, "sleep");
   JS_DeleteProperty(cx, esmd->item, "usleep");
   JS_DeleteProperty(cx, esmd->item, "load");
   JS_DeleteProperty(cx, esmd->item, "unload");
   JS_DeleteProperty(cx, esmd->item, "include");
   JS_DeleteProperty(cx, esmd->item, "version");
   elixir_object_unregister(cx, (JSObject**) &esmd->item);
   JS_DeleteProperty(cx, esmd->parent, "elx");
   elixir_object_unregister(cx, (JSObject**) &esmd->parent);

   free(esmd);
   em->data = NULL;

   return EINA_TRUE;
}

static Elixir_Module_Api  module_api_elixir = {
   ELIXIR_MODULE_API_VERSION,
   ELIXIR_GRANTED,
   "elixir",
   "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_elixir = {
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
elixir_binding_init(void)
{
   return elixir_modules_register(&em_elixir);
}

void
elixir_binding_shutdown(void)
{
   elixir_modules_unregister(&em_elixir);
}

#ifndef EINA_STATIC_BUILD_ELIXIR
EINA_MODULE_INIT(elixir_binding_init);
EINA_MODULE_SHUTDOWN(elixir_binding_shutdown);
#endif
