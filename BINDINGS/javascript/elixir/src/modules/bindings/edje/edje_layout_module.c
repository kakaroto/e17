#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <string.h>
#include <Edje.h>

#include <jsarray.h>

#include "Elixir.h"

static const elixir_parameter_t*	_string_4functions_any_params[7] = {
  &string_parameter,
  &function_parameter,
  &function_parameter,
  &function_parameter,
  &function_parameter,
  &any_parameter,
  NULL
};

static void
_elixir_evas_object_box_layout(Evas_Object *o, Evas_Object_Box_Data *priv, void *user_data)
{
   JSFunction **cb;
   JSContext *cx;
   JSObject *parent;
   Eina_List *l;
   Evas_Object *over;
   JSObject *jspriv = NULL;
   JSObject *jstmp = NULL;
   JSObject *jsson = NULL;
   int i;
   jsval argv[3];
   jsval jobj;
   jsval rval;

   cb = elixir_void_get_private(user_data);
   cx = elixir_void_get_cx(user_data);
   parent = elixir_void_get_parent(user_data);

   if (!cx || !parent || !cb || !cb[0] || !priv) return ;

   elixir_function_start(cx);

   if (!evas_object_to_jsval(cx, o, &argv[0]))
     goto on_error_empty;
   elixir_rval_register(cx, argv);

   jspriv = JS_NewObject(cx, elixir_class_request("Evas_Object_Box_Data", NULL), NULL, NULL);
   if (!jspriv) goto on_error;

   if (!elixir_object_register(cx, &jspriv, priv)) goto on_error;

   argv[1] = OBJECT_TO_JSVAL(jspriv);
   if (!elixir_rval_register(cx, argv + 1)) goto on_error;

   jstmp = JS_DefineObject(cx, jspriv, "base", elixir_class_request("Evas_Object_Smart_Clipped_Data", NULL), NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
   if (!jstmp) goto on_error2;

   if (!elixir_object_register(cx, &jstmp, NULL)) goto on_error2;

   if (!evas_object_to_jsval(cx, priv->base.clipper, &jobj)) goto on_error2;
   JS_DefineProperty(cx, jstmp, "clipper", jobj, NULL, NULL,  JSPROP_ENUMERATE | JSPROP_READONLY);

   jsson = JS_DefineObject(cx, jstmp, "evas", elixir_class_request("evas", NULL), NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
   if (!jsson) goto on_error2;

   elixir_object_unregister(cx, &jstmp);

   jstmp = JS_DefineObject(cx, jspriv, "align", elixir_class_request("Elixir_Align", NULL), NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
   if (!jstmp) goto on_error2;

   if (!elixir_object_register(cx, &jstmp, NULL)) goto on_error2;
   if (!elixir_add_dbl_prop(cx, jstmp, "h", priv->align.h)) goto on_error2;
   if (!elixir_add_dbl_prop(cx, jstmp, "v", priv->align.v)) goto on_error2;
   elixir_object_unregister(cx, &jstmp);

   jstmp = JS_DefineObject(cx, jspriv, "pad", elixir_class_request("Evas_Align_Coord", NULL), NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
   if (!jstmp) goto on_error2;

   if (!elixir_object_register(cx, &jstmp, NULL)) goto on_error2;
   if (!elixir_add_int_prop(cx, jstmp, "h", priv->pad.h)) goto on_error2;
   if (!elixir_add_int_prop(cx, jstmp, "v", priv->pad.v)) goto on_error2;
   elixir_object_unregister(cx, &jstmp);

   jstmp = JS_DefineObject(cx, jspriv, "children", &js_ArrayClass, NULL, JSPROP_ENUMERATE | JSPROP_READONLY);
   if (!jstmp) goto on_error2;

   i = 0;
   if (!elixir_object_register(cx, &jstmp, NULL)) goto on_error;
   EINA_LIST_FOREACH(priv->children, l, over)
     {
	jsval js_obj;

        evas_object_to_jsval(cx, over, &js_obj);
	JS_DefineElement(cx, jstmp, i++, js_obj, NULL,
			 NULL, JSPROP_INDEX | JSPROP_ENUMERATE | JSPROP_READONLY);
     }
   elixir_object_unregister(cx, &jstmp);
   jstmp = NULL;

   argv[2] = elixir_void_get_jsval(user_data);
   elixir_rval_register(cx, argv + 2);

   elixir_function_run(cx, cb[0], parent, 3, argv, &rval);
   elixir_rval_delete(cx, argv + 2);

 on_error:
   elixir_rval_delete(cx, argv + 1);

 on_error2:
   elixir_rval_delete(cx, argv);

 on_error_empty:

   elixir_object_unregister(cx, &jspriv);
   elixir_object_unregister(cx, &jstmp);

   elixir_function_stop(cx);
}

static void *
_elixir_layout_data_get(void *data)
{
   JSFunction **cb;
   JSContext *cx;
   void *res = NULL;
   JSObject *parent;
   jsval argv[1];
   jsval result;

   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);
   cb = elixir_void_get_private(data);
   argv[0] = elixir_void_get_jsval(data);

   if (!cb || !cb[1])
     return NULL;

   elixir_function_start(cx);

   if (elixir_function_run(cx, cb[1], parent, 1, argv, &result))
     res = elixir_void_new(cx, parent, result, cb[2]);

   elixir_function_stop(cx);

   return res;
}

static void
_elixir_layout_data_free(void *data)
{
   JSContext *cx;
   JSObject *parent;
   JSFunction *cb;
   jsval argv[1];
   jsval result;

   if (!data) return ;

   cx = elixir_void_get_cx(data);
   parent = elixir_void_get_parent(data);
   cb = elixir_void_get_private(data);
   argv[0] = elixir_void_get_jsval(data);

   elixir_function_start(cx);

   if (cb)
     elixir_function_run(cx, cb, parent, 1, argv, &result);

   elixir_void_free(data);

   elixir_function_stop(cx);
}

static void
_elixir_data_free(void *data)
{
   free(elixir_void_get_private(data));
   elixir_void_free(data);
}

JSBool
elixir_edje_box_layout_register(JSContext *cx, uintN argc, jsval *vp)
{
   const char *name;
   JSFunction **cb;
   void *data;
   int i;
   elixir_value_t val[6];

   if (!elixir_params_check(cx, _string_4functions_any_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   name = elixir_get_string_bytes(val[0].v.str, NULL);
   cb = malloc(sizeof (JSFunction *) * 4);
   if (!cb) return JS_FALSE;

   for (i = 0; i < 4; ++i)
     cb[i] = val[i + 1].v.fct;

   data = elixir_void_new(cx, JS_THIS_OBJECT(cx, vp), val[5].v.any, cb);

   edje_box_layout_register(name, _elixir_evas_object_box_layout, _elixir_layout_data_get, _elixir_layout_data_free, _elixir_data_free, data);

   return JS_TRUE;
}

