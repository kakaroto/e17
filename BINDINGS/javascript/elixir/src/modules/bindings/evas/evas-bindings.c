#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <string.h>
#include <stdlib.h>

#include <Evas.h>

#include "evas-bindings.h"

#ifdef BUILD_MODULE_ECORE_EVAS
# include <Ecore_Evas.h>
#endif

elixir_parameter_t			evas_parameter = {
  "Evas", JOBJECT, NULL
};
elixir_parameter_t                      evas_object_parameter = {
  "Evas_Object", JOBJECT, NULL
};
elixir_parameter_t                      evas_object_rectangle_parameter = {
  "Evas_Object_Rectangle", JOBJECT, NULL
};
elixir_parameter_t                      evas_object_line_parameter = {
  "Evas_Object_Line", JOBJECT, NULL
};
elixir_parameter_t                      evas_object_polygon_parameter = {
  "Evas_Object_Polygon", JOBJECT, NULL
};
elixir_parameter_t                      evas_object_image_parameter = {
  "Evas_Object_Image", JOBJECT, NULL
};
elixir_parameter_t                      evas_object_smart_parameter = {
  "Evas_Object_Smart", JOBJECT, NULL
};
elixir_parameter_t                      evas_object_text_parameter = {
  "Evas_Object_Text", JOBJECT, NULL
};
elixir_parameter_t                      evas_object_textblock_parameter = {
  "Evas_Object_Textblock", JOBJECT, NULL
};

Eina_Bool
elixir_evas_file_extract(JSContext *cx, JSObject *obj, const char **file, const char **key)
{
   GET_SCOMPONENT(cx, obj, file);
   GET_SCOMPONENT(cx, obj, key);

   return EINA_TRUE;
}

Eina_Bool
elixir_evas_border_extract(JSContext *cx, JSObject *obj, int *l, int *r, int *t, int *b)
{
   GET_ICOMPONENT(cx, obj, l);
   GET_ICOMPONENT(cx, obj, r);
   GET_ICOMPONENT(cx, obj, t);
   GET_ICOMPONENT(cx, obj, b);

   return EINA_TRUE;
}

Eina_Bool
elixir_evas_font_extract(JSContext *cx, JSObject *obj, const char **font, int *size)
{
   GET_SCOMPONENT(cx, obj, font);
   GET_ICOMPONENT(cx, obj, size);

   return EINA_TRUE;
}

Eina_Bool
elixir_evas_line_extract(JSContext *cx, JSObject *obj, int *x1, int *y1, int *x2, int *y2)
{
   GET_ICOMPONENT(cx, obj, x1);
   GET_ICOMPONENT(cx, obj, y1);
   GET_ICOMPONENT(cx, obj, x2);
   GET_ICOMPONENT(cx, obj, y2);

   return EINA_TRUE;
}

Eina_Bool
elixir_evas_position_extract(JSContext *cx, JSObject *obj, double *x, double *y)
{
   GET_DCOMPONENT(cx, obj, x);
   GET_DCOMPONENT(cx, obj, y);

   return EINA_TRUE;
}

Eina_Bool
elixir_evas_aspect_extract(JSContext *cx, JSObject *obj, int *aspect, int *w, int *h)
{
   GET_ICOMPONENT(cx, obj, aspect);
   GET_ICOMPONENT(cx, obj, w);
   GET_ICOMPONENT(cx, obj, h);

   return EINA_TRUE;
}

Eina_Bool
elixir_new_evas_file(JSContext *cx,
                     const char *file, const char *key,
                     jsval *rval)
{
   JSObject *js_obj;
   JSClass *evas_file_class;
   jsval propertie;
   Eina_Bool ret = EINA_FALSE;

   evas_file_class = elixir_class_request("evas_file", NULL);

   elixir_lock_cx(cx);

   js_obj = JS_NewObject(cx, evas_file_class, NULL, NULL);
   if (!js_obj) goto on_error;

   *rval = OBJECT_TO_JSVAL(js_obj);

   propertie = file
     ? STRING_TO_JSVAL(elixir_ndup(cx, file, strlen(file)))
     : JSVAL_NULL;
   JS_SetProperty(cx, js_obj, "file", &propertie);

   propertie = file
     ? STRING_TO_JSVAL(elixir_ndup(cx, key, strlen(key)))
     : JSVAL_NULL;
   JS_SetProperty(cx, js_obj, "key", &propertie);

   ret = EINA_TRUE;

 on_error:
   elixir_unlock_cx(cx);

   return ret;
}

Eina_Bool
elixir_new_evas_pos(JSContext *cx,
                    int icx, int icy, int icw, int ich,
                    jsval *rval)
{
   JSObject *js_obj;
   jsval propertie;

   js_obj = elixir_new_geometry(cx, icx, icy, icx, ich);
   if (!js_obj) return EINA_FALSE;
   *rval = OBJECT_TO_JSVAL(js_obj);

   propertie = INT_TO_JSVAL(icx);
   JS_SetProperty(cx, js_obj, "cx", &propertie);

   propertie = INT_TO_JSVAL(icy);
   JS_SetProperty(cx, js_obj, "cy", &propertie);

   propertie = INT_TO_JSVAL(icw);
   JS_SetProperty(cx, js_obj, "cw", &propertie);

   propertie = INT_TO_JSVAL(ich);
   JS_SetProperty(cx, js_obj, "ch", &propertie);

   return EINA_TRUE;
}

Eina_Bool
elixir_new_evas_position(JSContext *cx,
			 double x, double y,
			 jsval *rval)
{
   JSObject *js_obj;
   JSClass *evas_position;
   jsval propertie;
   Eina_Bool ret = EINA_FALSE;

   elixir_lock_cx(cx);

   evas_position = elixir_class_request("evas_position", NULL);

   js_obj = JS_NewObject(cx, evas_position, NULL, NULL);
   if (!js_obj) goto on_error;

   *rval = OBJECT_TO_JSVAL(js_obj);

   JS_NewNumberValue(cx, x, &propertie);
   JS_SetProperty(cx, js_obj, "x", &propertie);

   JS_NewNumberValue(cx, y, &propertie);
   JS_SetProperty(cx, js_obj, "y", &propertie);

   ret = EINA_TRUE;

 on_error:
   elixir_unlock_cx(cx);

   return ret;
}

Eina_Bool
elixir_new_evas_aspect(JSContext *cx,
                       int aspect, int w, int h,
                       jsval *rval)
{
   JSObject *js_obj;
   JSClass *evas_aspect;;
   jsval propertie;
   Eina_Bool ret = EINA_FALSE;

   elixir_lock_cx(cx);

   evas_aspect = elixir_class_request("evas_aspect", NULL);

   js_obj = JS_NewObject(cx, evas_aspect, NULL, NULL);
   if (!js_obj) goto on_error;

   *rval = OBJECT_TO_JSVAL(js_obj);

   propertie = INT_TO_JSVAL(aspect);
   JS_SetProperty(cx, js_obj, "aspect", &propertie);

   propertie = INT_TO_JSVAL(w);
   JS_SetProperty(cx, js_obj, "w", &propertie);

   propertie = INT_TO_JSVAL(h);
   JS_SetProperty(cx, js_obj, "h", &propertie);

   ret = EINA_TRUE;

 on_error:
   elixir_unlock_cx(cx);

   return ret;
}

Eina_Bool
elixir_new_evas_border(JSContext *cx,
                       int l, int r, int t, int b,
                       jsval *rval)
{
   JSObject *js_obj;
   JSClass *evas_image_border_class;
   jsval propertie;
   Eina_Bool ret = EINA_FALSE;

   elixir_lock_cx(cx);

   evas_image_border_class = elixir_class_request("evas_image_border", NULL);

   js_obj = JS_NewObject(cx, evas_image_border_class, NULL, NULL);
   if (!js_obj) goto on_error;

   *rval = OBJECT_TO_JSVAL(js_obj);

   propertie = INT_TO_JSVAL(l);
   JS_SetProperty(cx, js_obj, "l", &propertie);

   propertie = INT_TO_JSVAL(r);
   JS_SetProperty(cx, js_obj, "r", &propertie);

   propertie = INT_TO_JSVAL(t);
   JS_SetProperty(cx, js_obj, "t", &propertie);

   propertie = INT_TO_JSVAL(b);
   JS_SetProperty(cx, js_obj, "b", &propertie);

   ret = EINA_TRUE;

 on_error:
   elixir_unlock_cx(cx);

   return ret;
}

Eina_Bool
elixir_new_evas_font(JSContext *cx,
                     const char *font, int size,
                     jsval *rval)
{
   JSObject *js_obj;
   JSClass *evas_font_class;
   jsval propertie;
   Eina_Bool ret = EINA_FALSE;

   elixir_lock_cx(cx);

   evas_font_class = elixir_class_request("evas_font", NULL);

   js_obj = JS_NewObject(cx, evas_font_class, NULL, NULL);
   if (!js_obj) goto on_error;

   *rval = OBJECT_TO_JSVAL(js_obj);

   propertie = INT_TO_JSVAL(size);
   JS_SetProperty(cx, js_obj, "size", &propertie);

   propertie = font ? STRING_TO_JSVAL(elixir_dup(cx, font)) : JSVAL_NULL;
   JS_SetProperty(cx, js_obj, "font", &propertie);

   ret = EINA_TRUE;

 on_error:
   elixir_unlock_cx(cx);

   return ret;
}

Eina_Bool
evas_to_jsval(JSContext *cx, Evas *e, jsval *rval)
{
#ifdef BUILD_MODULE_ECORE_EVAS
   Ecore_Evas *ee;
   JSObject *jo;
   jsval *tmp;

   if (!e)
     {
	*rval = JSVAL_NULL;
	return EINA_TRUE;
     }

   ee = evas_data_attach_get(e);
   if (ee)
     {
	tmp = ecore_evas_data_get(ee, "elixir_evas_jsval");
	if (tmp)
	  {
	     *rval = *tmp;
	     return EINA_TRUE;
	  }
     }

   tmp = malloc(sizeof (jsval));
   if (!tmp) return EINA_FALSE;

   jo = elixir_build_ptr(cx, e, elixir_class_request("evas", NULL));
   if (!jo) return EINA_FALSE;

   *tmp = OBJECT_TO_JSVAL(jo);
   if (!elixir_rval_register(cx, tmp))
     {
	free(tmp);
	return EINA_FALSE;
     }

   ecore_evas_data_set(ee, "elixir_evas_jsval", tmp);

   return EINA_TRUE;
#else
   return EINA_FALSE;
#endif
}

static JSFunctionSpec	evas_functions[] = {
   { NULL, NULL, 0, 0, 0 }
};

static const struct {
   Eina_Bool (*load)(JSContext *cx, JSObject *parent);
   Eina_Bool (*unload)(JSContext *cx, JSObject *parent);
} sub_module[] = {
   { load_evas_params, unload_evas_params },
   { load_void_params, unload_void_params },
   { load_evas_const_binding, unload_evas_const_binding },
   { load_evas_event_binding, unload_evas_event_binding },
   { load_evas_string_params, unload_evas_string_params },
   { load_evas_int_params, unload_evas_int_params },
   { load_evas_object_params, unload_evas_object_params },
   { load_2_evas_object_params, unload_2_evas_object_params },
   { load_evas_object_string_params, unload_evas_object_string_params },
   { load_evas_object_bindings, unload_evas_object_bindings },
   { load_evas_object_line_binding, unload_evas_object_line_binding },
   { load_evas_object_image_binding, unload_evas_object_image_bindings },
   { load_evas_object_textblock_binding, unload_evas_object_textblock_binding },
   { load_evas_object_text_binding, unload_evas_object_text_bindings },
   { load_evas_object_boolean_params, unload_evas_object_boolean_params },
   { load_evas_object_int_2_params, unload_evas_object_int_2_params },
   { load_evas_object_int_params, unload_evas_object_int_params },
   { load_evas_object_color_params, unload_evas_object_color_params },
   { load_evas_smart_params, unload_evas_smart_params },
   { load_4_int_params, unload_4_int_params },
   { load_3_int_params, unload_3_int_params },
   { load_2_double_params, unload_2_double_params },
   { load_1_double_params, unload_1_double_params },
   { NULL, NULL }
};

static const struct {
   const char *extention;
   const char *type_name;
} evas_types[] = {
   { ".jpeg", "Image JPEG" },
   { ".jpg",  "Image JPEG" },
   { ".gif",  "Image GIF" },
   { ".png",  "Image PNG" }
};

static Eina_Bool
module_open(Elixir_Module *em, JSContext *cx, JSObject *parent)
{
   void **tmp;
   unsigned int i = 0;

   evas_parameter.class = elixir_class_request("evas", NULL);
   evas_object_parameter.class = elixir_class_request("evas_object", NULL);
   evas_object_rectangle_parameter.class = elixir_class_request("evas_object_rectangle", "evas_object");
   evas_object_line_parameter.class = elixir_class_request("evas_object_line", "evas_object");
   evas_object_polygon_parameter.class = elixir_class_request("evas_object_polygon", "evas_object");
   evas_object_image_parameter.class = elixir_class_request("evas_object_image", "evas_object");
   evas_object_smart_parameter.class = elixir_class_request("evas_object_smart", "evas_object");
   evas_object_text_parameter.class = elixir_class_request("evas_object_text", "evas_object");
   evas_object_textblock_parameter.class = elixir_class_request("evas_object_textblock", "evas_object");

   if (!JS_DefineFunctions(cx, parent, evas_functions))
     return EINA_FALSE;

   while (sub_module[i].load)
     if (!sub_module[i++].load(cx, parent))
       goto on_error;

   em->data = parent;
   tmp = &em->data;

   if (!elixir_object_register(cx, (JSObject**) tmp, NULL))
     goto on_error;

   for (i = 0; i < sizeof (evas_types) / sizeof(*evas_types); i++)
     elixir_file_register(evas_types[i].extention, evas_types[i].type_name);

   return EINA_TRUE;

  on_error:
   for (i--; i > 0; --i)
     sub_module[i].unload(cx, parent);
   return EINA_FALSE;
}

static Eina_Bool
module_close(Elixir_Module *em, JSContext *cx)
{
   JSObject *parent;
   void **tmp;
   unsigned int i = 0;

   if (!em->data)
     return EINA_FALSE;
   parent = em->data;

   while (evas_functions[i].name)
     JS_DeleteProperty(cx, parent, evas_functions[i++].name);

   for (i = 0; sub_module[i].unload; ++i)
     sub_module[i].unload(cx, parent);

   for (i = 0; i < sizeof (evas_types) / sizeof(*evas_types); i++)
     elixir_file_unregister(evas_types[i].extention, evas_types[i].type_name);

   tmp = &em->data;

   elixir_object_unregister(cx, (JSObject**) tmp);
   em->data = NULL;

   return EINA_TRUE;
}

static Elixir_Module_Api  module_api_elixir = {
   ELIXIR_MODULE_API_VERSION,
   ELIXIR_GRANTED,
   "evas",
   "Cedric BAIL <cedric.bail@free.fr>"
};

static Elixir_Module em_evas = {
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
evas_binding_init(void)
{
   return elixir_modules_register(&em_evas);
}

void
evas_binding_shutdown(void)
{
   elixir_modules_unregister(&em_evas);
}

#ifndef EINA_STATIC_BUILD_EVAS
EINA_MODULE_INIT(evas_binding_init);
EINA_MODULE_SHUTDOWN(evas_binding_shutdown);
#endif
