#include <string.h>
#include <stdlib.h>
#include <Evas.h>

#include "evas-bindings.h"

static elixir_parameter_t               _evas_file_parameter = {
  "Evas_File", JOBJECT, NULL
};
static elixir_parameter_t               _evas_image_border_parameter = {
  "Evas_Image_Border", JOBJECT, NULL
};

static const elixir_parameter_t*	_evas_object_image_double_params[3] = {
   &evas_object_image_parameter,
   &double_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_object_image_jsobject_params[3] = {
   &evas_object_image_parameter,
   &jsobject_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_object_image_image_border_params[3] = {
   &evas_object_image_parameter,
   &_evas_image_border_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_image_file_params[3] = {
   &evas_object_image_parameter,
   &_evas_file_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_image_geometry_params[3] = {
   &evas_object_image_parameter,
   &geometry_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_image_boolean_params[3] = {
   &evas_object_image_parameter,
   &boolean_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_image_double_string_params[4] = {
   &evas_object_image_parameter,
   &string_parameter,
   &string_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_image_4int_params[6] = {
   &evas_object_image_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   &int_parameter,
   NULL
};
static const elixir_parameter_t*        _evas_object_image_evas_object_params[3] = {
   &evas_object_image_parameter,
   &evas_object_parameter,
   NULL
};
static const elixir_parameter_t*	_evas_object_image_params[2] = {
   &evas_object_image_parameter,
   NULL
};

static JSBool
elixir_evas_object_image_file_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   const char *key;
   char *file;
   elixir_value_t val[3];

   if (elixir_params_check(cx, _evas_object_image_file_params, val, argc, JS_ARGV(cx, vp)))
     {
        GET_PRIVATE(cx, val[0].v.obj, know);

	if (!elixir_evas_file_extract(cx, val[1].v.obj, (const char**) &file, &key))
	  return JS_FALSE;
        file = elixir_file_canonicalize(file);
     }

   if (elixir_params_check(cx, _evas_object_image_double_string_params, val, argc, JS_ARGV(cx, vp)))
     {
	size_t length;

	GET_PRIVATE(cx, val[0].v.obj, know);

	file = elixir_file_canonicalize(elixir_get_string_bytes(val[1].v.str, NULL));
	key = elixir_get_string_bytes(val[2].v.str, &length);

	if (key && strlen(key) != length)
	  return JS_FALSE;
     }

   if (!know) return JS_FALSE;

   evas_object_image_file_set(know, file, key);

   free(file);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_image_border_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   int l;
   int r;
   int t;
   int b;
   elixir_value_t val[5];

   if (elixir_params_check(cx, _evas_object_image_image_border_params, val, argc, JS_ARGV(cx, vp)))
     {
        GET_PRIVATE(cx, val[0].v.obj, know);

	if (!elixir_evas_border_extract(cx, val[1].v.obj, &l, &r, &t, &b))
	  return JS_FALSE;
     }

   if (elixir_params_check(cx, _evas_object_image_4int_params, val, argc, JS_ARGV(cx, vp)))
     {
        GET_PRIVATE(cx, val[0].v.obj, know);

	l = val[1].v.num;
	r = val[2].v.num;
	t = val[3].v.num;
	b = val[4].v.num;
     }

   if (!know) return JS_FALSE;

   evas_object_image_border_set(know, l, r, t, b);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_image_fill_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   int x;
   int y;
   int w;
   int h;
   elixir_value_t val[5];

   if (elixir_params_check(cx, _evas_object_image_geometry_params, val, argc, JS_ARGV(cx, vp)))
     {
        GET_PRIVATE(cx, val[0].v.obj, know);

	if (!elixir_extract_geometry(cx, val[1].v.obj, &x, &y, &w, &h))
	  return JS_FALSE;
     }

   if (elixir_params_check(cx, _evas_object_image_4int_params, val, argc, JS_ARGV(cx, vp)))
     {
        GET_PRIVATE(cx, val[0].v.obj, know);

	x = val[1].v.num;
	y = val[2].v.num;
	w = val[3].v.num;
	h = val[4].v.num;
     }

   if (!know) return JS_FALSE;

   evas_object_image_fill_set(know, x, y, w, h);

   return JS_TRUE;
}

/* FIXME: Should be a special object, with direct access to the surface behind, or it will stay completely unusable. */
static JSBool
elixir_evas_object_image_data_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   JSObject *array;
   int *data;
   int *walk;
   unsigned int length;
   int i = 0;
   int x = 0;
   int y = 0;
   int w = 0;
   int h = 0;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_object_image_jsobject_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   array = val[1].v.obj;
   if (!array) return JS_FALSE;
   if (JS_IsArrayObject(cx, array) == JS_FALSE)
     return JS_FALSE;

   if (JS_GetArrayLength(cx, array, &length) == JS_FALSE)
     return JS_FALSE;

   if (!(data = JS_GetPrivate(cx, array)))
     return JS_FALSE;

   evas_object_image_size_get(know, &w, &h);

   if (length != (unsigned int)w * h)
     return JS_FALSE;

   for (walk = data, i = 0, y = 0; y < h; ++y)
     for (x = 0; x < w; ++x, ++i, ++walk)
       {
	  JSObject *evas_color;
	  jsval js_evas_color;
	  int r;
	  int g;
	  int b;
	  int a;

	  if (JS_GetElement(cx, array, i, &js_evas_color) == JS_FALSE)
	    return JS_FALSE;

	  if (JS_ValueToObject(cx, js_evas_color, &evas_color) == JS_FALSE)
	    return JS_FALSE;

	  if (elixir_extract_color(cx, evas_color, &r, &g, &b, &a) == JS_FALSE)
	    return JS_FALSE;

	  *walk = (a << 24) | (r << 16) | (g << 8) | b;
       }

   evas_object_image_data_set(know, data);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_image_data_update_add(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   int x;
   int y;
   int w;
   int h;
   elixir_value_t val[5];

   if (elixir_params_check(cx, _evas_object_image_geometry_params, val, argc, JS_ARGV(cx, vp)))
     {
        GET_PRIVATE(cx, val[0].v.obj, know);

	if (!elixir_extract_geometry(cx, val[1].v.obj, &x, &y, &w, &h))
	  return JS_FALSE;
     }

   if (elixir_params_check(cx, _evas_object_image_4int_params, val, argc, JS_ARGV(cx, vp)))
     {
        GET_PRIVATE(cx, val[0].v.obj, know);

	x = val[1].v.num;
	y = val[2].v.num;
	w = val[3].v.num;
	h = val[4].v.num;
     }

   if (!know)
     return JS_FALSE;

   evas_object_image_data_update_add(know, x, y, w, h);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_image_preload(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_object_image_boolean_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);

   evas_object_image_preload(know, val[1].v.bol);

   return JS_TRUE;
}


/* Evas_Bool         evas_object_image_save            (Evas_Object *obj, const char *file, const char *key, const char *flags); */
/* Evas_Bool         evas_object_image_pixels_import          (Evas_Object *obj, Evas_Pixel_Import_Source *pixels); */
/* void              evas_object_image_pixels_get_callback_set(Evas_Object *obj, void (*func) (void *data, Evas_Object *o), void *data); */

static JSBool
elixir_evas_object_image_load_dpi_set(JSContext *cx, uintN argc, jsval *vp)
{
   Evas_Object *know = NULL;
   double dpi;
   elixir_value_t val[2];

   if (!elixir_params_check(cx, _evas_object_image_double_params, val, argc, JS_ARGV(cx, vp)))
     return JS_FALSE;

   GET_PRIVATE(cx, val[0].v.obj, know);
   dpi = val[1].v.dbl;

   evas_object_image_load_dpi_set(know, dpi);

   return JS_TRUE;
}

static JSBool
elixir_evas_object_image_source_set(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Object *know;
  Evas_Object *src;
  elixir_value_t val[2];

  if (!elixir_params_check(cx, _evas_object_image_evas_object_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, know);
  GET_PRIVATE(cx, val[1].v.obj, src);

  JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(evas_object_image_source_set(know, src)));
  return JS_TRUE;
}

static JSBool
elixir_evas_object_image_source_get(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Object *know;
  Evas_Object *src;
  elixir_value_t val[1];

  if (!elixir_params_check(cx, _evas_object_image_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, know);

  src = evas_object_image_source_get(know);
  return evas_object_to_jsval(cx, src, &(JS_RVAL(cx, vp)));
}

static JSBool
elixir_evas_object_image_source_unset(JSContext *cx, uintN argc, jsval *vp)
{
  Evas_Object *know;
  elixir_value_t val[1];

  if (!elixir_params_check(cx, _evas_object_image_params, val, argc, JS_ARGV(cx, vp)))
    return JS_FALSE;

  GET_PRIVATE(cx, val[0].v.obj, know);

  JS_SET_RVAL(cx, vp, BOOLEAN_TO_JSVAL(evas_object_image_source_unset(know)));
  return JS_TRUE;
}

static JSFunctionSpec	evas_object_image_functions[] = {
  ELIXIR_FN(evas_object_image_preload, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_file_set, 3, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_border_set, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_fill_set, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_data_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_data_update_add, 5, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_load_dpi_set, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_source_set, 2, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_source_get, 1, JSPROP_ENUMERATE, 0 ),
  ELIXIR_FN(evas_object_image_source_unset, 1, JSPROP_ENUMERATE, 0 ),
  JS_FS_END
};

Eina_Bool
load_evas_object_image_binding(JSContext *cx, JSObject *parent)
{
   if (!JS_DefineFunctions(cx, parent, evas_object_image_functions))
     return EINA_FALSE;

   _evas_file_parameter.class = elixir_class_request("evas_file", NULL);
   _evas_image_border_parameter.class = elixir_class_request("evas_image_border", NULL);

   return EINA_TRUE;
}

Eina_Bool
unload_evas_object_image_bindings(JSContext *cx, JSObject *parent)
{
   unsigned int i = 0;

   while (evas_object_image_functions[i].name)
     JS_DeleteProperty(cx, parent, evas_object_image_functions[i++].name);

   return EINA_TRUE;
}

